#include <Arduino_FreeRTOS.h>
#include <task.h>

#define ALARM '!'
#define STABLE '-'
#define NOT_MESURING 0
#define MESURING 1


int mesure = 0;
int mesuring_state = MESURING;
int alarm_state = STABLE;

TaskHandle_t MesureLightTask;
TaskHandle_t SendSerialTask;
TaskHandle_t AlarmTask;
TaskHandle_t ReadSerialTask;
TaskHandle_t BlinkingTask;

void setup() {
  // put your setup code here, to run once:
  pinMode(3, INPUT);
  pinMode(A3, INPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT),
  Serial.begin(9600,SERIAL_8N1);  
  attachInterrupt(digitalPinToInterrupt(3),switchMesuringStateInterruption,RISING);
  
  xTaskCreate(vMesureLightTask, "MesureLight",70, NULL, 3, &MesureLightTask);
  xTaskCreate(vSendSerialTask,"SendSerial",120, NULL, 3, &SendSerialTask);
  xTaskCreate(vAlarmTask,"Alarm", 60, NULL, 3, &AlarmTask);
  xTaskCreate(vReadSerialTask,"ReadSerial", 70, NULL,3, &ReadSerialTask);
  xTaskCreate(vBlinkingTask,"Blinking", 60, NULL, 3, &BlinkingTask);
}

void loop() {
  // put your main code here, to run repeatedly:
}

/**
* Tarea que lee contantemente la luminosidad
*/
void vMesureLightTask(){
  while(true){
      mesure = analogRead(A3);
  }
  vTaskDelete(NULL);
}

/**
* Tarea que envia los datos al bus serial cada 3 segundos.
*/
void vSendSerialTask(){
  for(;;){
    vTaskDelay(pdMS_TO_TICKS(3000));
    sendBitFrame(mesure,alarm_state);
    //Serial.println(uxTaskGetStackHighWaterMark(BlinkingTask));
  }
  vTaskDelete(NULL);
}

/**
* Tarea que enciende la alarma si la medida de luz detectada 
* es mayor a 800 lux.
*/
void vAlarmTask(){
  for(;;){
    if(mesure >= 800){
      alarm_state = ALARM;
      digitalWrite(12,HIGH);
      delay(500);
      digitalWrite(12,LOW);
      delay(500);
    }else{
      alarm_state = STABLE;
    }
  }
}

/**
* Tarea que llama el codigo de la interrupción si le llega un 1 en el bus Serial.
*/
void vReadSerialTask(){
  for(;;){
    if(Serial.available()>0){
      int var = Serial.readStringUntil('\n').toInt();
      if( var == 1){
        switchMesuringStateInterruption();
      }
    }else{
      delay(1);
    }
  }
  vTaskDelete(NULL);
}

/**
* Interrupción que incia o detiene la tarea para medir luz.
*/
void switchMesuringStateInterruption(){
  if(mesuring_state == NOT_MESURING){
    mesuring_state = MESURING;
    vTaskResume(MesureLightTask);
  }else if(mesuring_state == MESURING){
    mesuring_state = NOT_MESURING;
    mesure = 0;
    vTaskSuspend(MesureLightTask);
  }
}

/**
* Tarea que se encarga de hacer parpadear el led en el pin 11.
*/
void vBlinkingTask(){
  while(true){
    if(mesuring_state == MESURING){
      digitalWrite(11, HIGH);
      delay(200);
      digitalWrite(11, LOW);
      delay(1000);
    }else{
      //cuando quito esto falla. Una optimización finaliza la tarea?
      delay(1);
    }
  }
  vTaskDelete(NULL);
}

/**
* método que crea un arreglo de caracteres y lo envia por el bus serial.
*/
void sendBitFrame(short lightMesure,char alarmState){
  char frame[7] = {'0','0','0','0',';',alarmState,'\0'};
  char number[4] = {0};
  itoa(mesure,number,10);
  short numChars =  0;
  for(short i = 0; i < 4;i++){
    if(number[i]!='\0'){
      numChars++;
    }
  }
  memmove(frame+(4-numChars),number,numChars);
  Serial.println(frame);
}