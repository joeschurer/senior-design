#include <stdint.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

String dataBuf;
#define LIDARLITE_ADDR_DEFAULT 0x62
File myFile;
int config_val = 0;
bool waiting = true;
char mode = '0'; //1 = low res, 2 = high res, 3 = upload

//Lidar settings (see library for reccomendations)
uint8_t sigCountMax = 0x20;
uint8_t acqConfigReg = 0x01;
uint8_t refCountMax = 0x03;
uint8_t thresholdBypass = 0x00;

void setup(){

    pinMode(7, OUTPUT);
    pinMode(10, OUTPUT);
    Serial.begin(230400);

    Wire.begin();

    if (!SD.begin(10)) {
      Serial.println("initialization failed!");
      while (1);
    }

    SD.remove("test.txt");
    myFile = SD.open("test.txt", FILE_WRITE);

    //Configure lidar
    write(0x02, &sigCountMax    , 1);
    write(0x04, &acqConfigReg   , 1);
    write(0x12, &refCountMax    , 1);
    write(0x1c, &thresholdBypass, 1);
}


void loop(){
  //wait for instruction from serial
  while(waiting){
    delay(2000);
    Serial.println("Waiting for instruction");
    if(Serial.available()){
      mode = Serial.read();
      Serial.println(mode);
    }
    if(mode == '1' || mode == '2' || mode == '3'){
      waiting = false;
    }
  }

  if(mode == '1'|| mode == '2'){
    delay(5000);
    uint16_t distance;
    distance = getRange();

    myFile.println("BEGIN_SCAN");
    Serial.println("BEGIN_SCAN");
    unsigned long startTime = millis();
    for(int i = 0; i< 100000;i++){
      distance = getRange();
      String dataBuf = String(distance) + "," + String(distance)+ "," + String(distance) + "\n";
      myFile.print(dataBuf);
    }
    unsigned long endTime = millis();
    myFile.println("END_SCAN");
    Serial.println("END_SCAN");

    Serial.println(endTime-startTime);
    myFile.print("Time : ");
    myFile.println(endTime-startTime);
    myFile.close();

    Serial.println("done");
    waiting = true;
    mode = '0';
  }
  else if(mode == '3'){
    File dataFile = SD.open("test.txt");
    if (dataFile) {
      while (dataFile.available()) {
        //Serial.write(dataFile.read());
        Serial.println(dataFile.read());
      }
      dataFile.close();
    }  
    else {
      Serial.println("error opening datalog.txt");
    }

    waiting = true;
    mode = '0';
  }
  else{
    Serial.println("invalid mode");
  }

    
 
}


//Get distance (this version waits for measurement to be done to return)
uint16_t getRange(){
  uint8_t  busyFlag;
  read(0x01, &busyFlag, 1);
  busyFlag &= 0x01;

  //while busy continue to wait
  while(busyFlag){
    read(0x01, &busyFlag, 1);
    busyFlag &= 0x01;
  }

  write(0x00, 0x01, 1);

  //wait for measurement to complete
  read(0x01, &busyFlag, 1);
  busyFlag &= 0x01;
  //while busy continue to wait
  while(busyFlag){
    read(0x01, &busyFlag, 1);
    busyFlag &= 0x01;
  }

  //now retrieve the distance
  uint16_t  distance;
  uint8_t dataBytes[2];
  read(0x0f, dataBytes, 2);
  distance = (dataBytes[0] << 8) | dataBytes[1];

  return (distance);

  
}

void write(uint8_t regAddr,uint8_t * dataBytes,uint8_t numBytes){
    Wire.beginTransmission(LIDARLITE_ADDR_DEFAULT);
    Wire.write(regAddr);
    Wire.write(dataBytes, numBytes);
    if ( Wire.endTransmission() ){
        Serial.println("> nack");
    }
    //ehhhh
    //delayMicroseconds(100); 
}

void read(uint8_t regAddr,uint8_t * dataBytes,uint8_t numBytes){
  Wire.requestFrom(LIDARLITE_ADDR_DEFAULT,numBytes,regAddr,1,true);
  uint8_t  numHere = Wire.available();
  uint8_t  i       = 0;

  while (i < numHere){
    dataBytes[i] = Wire.read();
    i++;
  }
  //eh
  //delayMicroseconds(100);
}
