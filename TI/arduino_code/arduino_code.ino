#include <Arduino_FreeRTOS.h>
#include <task.h>
#include <EEPROM.h>

//TP2
#define ALARM '!'
#define STABLE '-'
#define NOT_MESURING 0
#define MESURING 1

unsigned long int mesure = 0;
unsigned char mesuring_state = MESURING;
char alarm_state = STABLE;

TaskHandle_t MesureLightTask;
TaskHandle_t SendMesureTask;
TaskHandle_t AlarmTask;
TaskHandle_t BlinkingTask;
TaskHandle_t AlarmBlinkingTask;

//TP3
struct InterruptEvent{
  unsigned long int time;
  unsigned char pin_num;
};

unsigned short int last_eeAdd;
unsigned long int curr_time;

TaskHandle_t AddOneSecondTask;
TaskHandle_t ReadSerialTask;
TaskHandle_t SendTimeTask;

void setup() {
  Serial.begin(9600,SERIAL_8N1);

  // TP1
  pinMode(A3, INPUT);
  pinMode(8, INPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(13, OUTPUT);
  
  //TP2
  attachInterrupt(digitalPinToInterrupt(3),switchMesuringStateInterruption,RISING);

  xTaskCreate(vMesureLightTask, "MesureLight",70, NULL, 1, &MesureLightTask);
  xTaskCreate(vSendMesureTask,"SendMesure",70, NULL, 1, &SendMesureTask);
  xTaskCreate(vAlarmTask,"Alarm", 70, NULL, 1, &AlarmTask);
  xTaskCreate(vAlarmBlinkingTask,"BlinkingAlarm", 70, NULL, 1, &AlarmBlinkingTask);
  xTaskCreate(vBlinkingTask,"Blinking", 60, NULL, 1, &BlinkingTask);
  
  //TP3
  //2-byte int last address written in EEPROM.
  EEPROM.get(0,last_eeAdd);
  //4-byte long int time in seconds.
  EEPROM.get(2,curr_time);

  pinMode(2, INPUT);
  pinMode(3, INPUT);
  attachInterrupt(digitalPinToInterrupt(2),pin2Interruption,RISING);
  attachInterrupt(digitalPinToInterrupt(3),pin3Interruption,RISING);  
  //xTaskCreate(vAddOneSecondTask, "AddOneSecond",110, NULL, 2, &AddOneSecondTask);
  xTaskCreate(vReadSerialTask, "ReadSerial",150, NULL, 1, &ReadSerialTask);
  //xTaskCreate(vSendTimeTask,"",40,NULL,1,&SendTimeTask);
}

void loop() {
  // put your main code here, to run repeatedly:
}

void vReadSerialTask(){
  //Dicedes which action to do based on the symbol of the Serial bus.
  while(true){
    Serial.print("wt:");
    Serial.println(uxTaskGetStackHighWaterMark(NULL));
    if(Serial.available()>0){
      char frame[2];
      Serial.readBytes(frame,2);
      switch(frame[0]){
        case '?':{
          //get Data
          Serial.flush();
          sendEventsData();
          break;
        };      
        case '~':{
          //delete Data
          Serial.flush();
          deleteEventsData();
          break;
        };
        case '*':{
          //Update Time
          unsigned long int seconds = Serial.readStringUntil("\n").toInt();
          EEPROM.put(2,seconds);
          break;
        };
        case '+':{
          //Modify pin lights
          updatePins();
          break;
        };
      }
    } 
  }
  vTaskDelete(NULL);
}

void deleteEventsData(){
  last_eeAdd = 6;
  EEPROM.put(0,last_eeAdd);
  for(short unsigned int i = 6;i < EEPROM.length();i++){
    EEPROM.write(i,0);
  }
}

void sendEventsData(){
  short unsigned int i = 6;
  struct InterruptEvent curr_e;
  char *frame;
  while(i < last_eeAdd){
    EEPROM.get(i,curr_e);
    Serial.print("e;");
    Serial.print(curr_e.time);
    Serial.print(";");
    Serial.println(curr_e.pin_num);
    i = i+sizeof(struct InterruptEvent);
  }
}

void vSendTimeTask(){
  while(true){
    EEPROM.get(2,curr_time);
    Serial.print("t;");
    Serial.println(curr_time);
    vTaskDelay(66);
  }
  vTaskDelete(NULL);
}

void vAddOneSecondTask(){
  TickType_t xLastWakeTime;
  const TickType_t xFrequency = 66;
  while(true){
    vTaskDelayUntil(&xLastWakeTime,xFrequency);
    EEPROM.put(2,curr_time+1);
    curr_time+=1;
  }
  vTaskDelete(NULL);
}

void pin2Interruption(){
  saveEvent(2);
}

void pin3Interruption(){
  saveEvent(3);
}

void saveEvent(unsigned char pinNum){
  unsigned long int curr_time;
  EEPROM.get(2,curr_time);
  struct InterruptEvent new_event;
  new_event.time = curr_time;
  new_event.pin_num = pinNum;
  EEPROM.put(last_eeAdd,new_event);
  last_eeAdd = last_eeAdd + sizeof(struct InterruptEvent);
  EEPROM.put(0,last_eeAdd);
}

//Tp2

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
método que crea un arreglo de caracteres y lo envia por el bus serial.
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
*/

/**
* Tarea que envia los datos al bus serial cada 3 segundos.
*/
void vSendMesureTask(){
  for(;;){
    vTaskDelay(pdMS_TO_TICKS(3000));
    Serial.print("l;");
    Serial.print(mesure);
    Serial.print(';');
    Serial.print(alarm_state);
    Serial.println('\0');
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
      vTaskResume(AlarmBlinkingTask);
    }else{
      alarm_state = STABLE;
      vTaskSuspend(AlarmBlinkingTask);
      digitalWrite(12,LOW);
    }
  }
  vTaskDelete(NULL);
}

/**
* Tarea que parpadea el pin 12
*/
void vAlarmBlinkingTask(){
    while(true){
      digitalWrite(12,HIGH);
      delay(500);
      digitalWrite(12,LOW);
      delay(500);
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
* Tarea que se encarga de hacer parpadear el led en el pin 8.
(cambio del 11 al 8
*/
void vBlinkingTask(){
  while(true){
    if(mesuring_state == MESURING){
      digitalWrite(8, HIGH);
      delay(200);
      digitalWrite(8, LOW);
      delay(1000);
    }else{
      //cuando quito esto falla. Una optimización finaliza la tarea?
      delay(1);
    }
  }
  vTaskDelete(NULL);
}

//TP1

unsigned char vars[5] = {0,0,0,0,0};

void updatePins() {
  // put your main code here, to run repeatedly:
  vars[0] = Serial.readStringUntil(';').toInt();
  vars[1] = Serial.readStringUntil(';').toInt();
  vars[2] = Serial.readStringUntil(';').toInt();
  vars[3] = Serial.readStringUntil(';').toInt();
  vars[4] = Serial.readStringUntil('\n').toInt();
  ledSwitchKey(13,vars[0]);
  ledBrightness(9,vars[1]);
  ledBrightness(10,vars[2]);
  ledBrightness(11,vars[3]);
  if(vars[4]==1){
    switchMesuringStateInterruption();
  }
}

void ledBrightness(unsigned char ledNum,unsigned char brightness){
  /*
  * ledNum: led pin number
  * brightness: value beteween 0 and 255
  */
  if(brightness <= 255 & brightness >= 0){
    analogWrite(ledNum,brightness);
  }
 }

void ledSwitchKey(unsigned char ledNum,unsigned char value){
  /*
  * ledNum: id del led a encender o apagar
  * value: bool
  */
  if(value == 1){
    digitalWrite(ledNum, HIGH);
  }else{
    digitalWrite(ledNum, LOW);
  }
}