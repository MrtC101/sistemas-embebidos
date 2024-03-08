void setup() {
  // put your setup code here, to run once:
  pinMode(A3, INPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(13, OUTPUT);
  Serial.begin(9600,SERIAL_8N1);
}

int vars[4] = {0,0,0,0};

void loop() {
  // put your main code here, to run repeatedly:
  readBitFrame(vars);
  ledSwitchKey(13,vars[0]);
  ledBrightness(9,vars[1]);
  ledBrightness(10,vars[2]);
  ledBrightness(11,vars[3]);
  sendBitFrame(buildBitFrame(getMesuredLight()));
  delay(1000);
}

String buildBitFrame(int lightMesure){
  String bitFrame = String(lightMesure,10);
  //bitFrame.concat("\n");
  return bitFrame;
}

int sendBitFrame(String bitFrame){
  Serial.println(bitFrame);
  return 0;
}

int readBitFrame(int* vars){
  if(Serial.available()>0){
    vars[0] = Serial.readStringUntil(';').toInt();
    vars[1] = Serial.readStringUntil(';').toInt();
    vars[2] = Serial.readStringUntil(';').toInt();
    vars[3] = Serial.readStringUntil('\n').toInt();
  }
  return 0;
}

//========================//

int ledBrightness(unsigned short ledNum,unsigned short brightness){
  /*
  * ledNum: led pin number
  * brightness: value beteween 0 and 255
  * if the LED assingned to pin number "ledNum" 
  * is adjusted to "brightness" value.
  */
  if(ledNum > 8 & ledNum < 12){
    if(brightness <= 255 & brightness >= 0){
      analogWrite(ledNum,brightness);
    }else{
      return -1;
    }
  }else{
    return -1;
  }
  return 0;
}

int ledSwitchKey(unsigned short ledNum,short value){
  /*
  * ledNum: id del led a encender o apagar
  * if the LED assingned to pin number "ledNum" is currently turned on 
  * it's turned off; otherwise it's turned on.
  */
  if(ledNum == 13){
    if(value == 1){
      digitalWrite(ledNum, HIGH);
    }else{
      digitalWrite(ledNum, LOW);
    }
  }else{
    return -1;
  }
  return 0;
}

int getMesuredLight(){
  int value = analogRead(A3);
  return value;
}