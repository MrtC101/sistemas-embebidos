#include <Arduino_FreeRTOS.h>
#include <task.h>
#include <EEPROM.h>

struct InterruptEvent{
  long int time;
  short pin_num;
};
unsigned short int last_eeAdd;
unsigned long int curr_time;

TaskHandle_t AddOneSecondTask;
TaskHandle_t ReadSerialTask;
TaskHandle_t SendTimeTask;

void setup() {
  //2-byte int last address written in EEPROM.
  EEPROM.get(0,last_eeAdd);
  //4-byte long int time in seconds.
  EEPROM.get(2,curr_time);
    xTaskCreate(vAddOneSecondTask, "AddOneSecond",100, NULL, 2, &AddOneSecondTask);

  // put your setup code here, to run once:
  Serial.begin(9600,SERIAL_8N1);
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  attachInterrupt(digitalPinToInterrupt(2),pin2Interruption,RISING);
  attachInterrupt(digitalPinToInterrupt(3),pin3Interruption,RISING);  
  xTaskCreate(vAddOneSecondTask, "AddOneSecond",100, NULL, 2, &AddOneSecondTask);
  xTaskCreate(vReadSerialTask, "ReadSerial",100, NULL, 1, &ReadSerialTask);
  xTaskCreate(vSendTimeTask,"",100,NULL,1,&SendTimeTask);
}

void loop() {
  // put your main code here, to run repeatedly:
}


void vReadSerialTask(){
  //Dicedes which action to do based on the symbol of the Serial bus.
  while(true){
    if(Serial.available()>0){
      String frame = Serial.readStringUntil(';');
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
          long int seconds = Serial.readStringUntil("\n").toInt();
          Serial.println(seconds);
          updateTime(seconds);
          break;
        };
      }
    } 
  }
  vTaskDelete(NULL);
}

void updateTime(long int seconds){
  curr_time = seconds;
  EEPROM.put(2,curr_time);
}

void deleteEventsData(){
  last_eeAdd = 6;
  EEPROM.put(0,last_eeAdd);
  for(int i = 6;i < EEPROM.length();i++){
    EEPROM.write(i,0);
  }
}

void sendEventsData(){
  short i = 6;
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
  char *frame;
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
    updateTime(curr_time+1);
  }
  vTaskDelete(NULL);
}

void pin2Interruption(){
  saveEvent(2);
}

void pin3Interruption(){
  saveEvent(3);
}

void saveEvent(short pinNum){
  long int curr_time;
  EEPROM.get(2,curr_time);
  struct InterruptEvent new_event;
  new_event.time = curr_time;
  new_event.pin_num = pinNum;
  EEPROM.put(last_eeAdd,new_event);
  last_eeAdd = last_eeAdd + sizeof(struct InterruptEvent);
  EEPROM.put(0,last_eeAdd);
}