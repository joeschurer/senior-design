#include <stdint.h>
#include <Wire.h>
#include "lidarv3hp.h"
#include <SPI.h>
#include <SD.h>

String dataBuf;
LIDARLite_v3HP myLidarLite;

//#define FAST_I2C
File myFile;
int config_val = 7;
bool waiting = true;
int mode = 0; //1 = low res, 2 = high res, 3 = upload

void setup(){
    pinMode(7, OUTPUT);
    pinMode(10, OUTPUT);
    Serial.begin(9600);

    Wire.begin();
    #ifdef FAST_I2C
        #if ARDUINO >= 157
            Wire.setClock(400000UL);
        #else
            TWBR = ((F_CPU / 400000UL) - 16) / 2;
        #endif
    #endif

    //7 is good
    myLidarLite.configure(config_val);

    if (!SD.begin(10)) {
      Serial.println("initialization failed!");
      while (1);
    }

    SD.remove("test.txt");
    myFile = SD.open("test.txt", FILE_WRITE);

    //Discard first distance
    newDistance = distanceFast(&distance);
     
}


void loop(){
  //wait for instruction from serial
  while(waiting){
    Serial.println("Waiting for instruction");
    if(Serial.available()){
      mode = Serial.read();
      Serial.println(mode);
    }
    if(mode == 1 || mode == 2 || mode == 3){
      waiting = false;
    }
  }

  if(mode == 1|| mode == 2){
    delay(10000);
    uint16_t distance;
    uint8_t  newDistance = 0;
    uint8_t  c;

    //int startTime = millis();
    for(int i=0;i<1000;i++){
      newDistance = distanceFast(&distance);
      myFile.print(distance);
      //Serial.println(distance);

    }

    //int endTime = millis();
    //Serial.println(endTime-startTime);
    myFile.close();

    Serial.println("done");
    waiting = true;
    mode = 0;
  }
  else if(mode == 3){
    File dataFile = SD.open("test.txt");
    if (dataFile) {
      while (dataFile.available()) {
        Serial.write(dataFile.read());
      }
      dataFile.close();
    }  
    else {
      Serial.println("error opening datalog.txt");
    }

    waiting = true;
    mode = 0;
  }
  else{
    Serial.println("invalid mode");
  }

    
 
}

uint8_t distanceFast(uint16_t * distance)
{
    myLidarLite.waitForBusy();
    myLidarLite.takeRange();
    *distance = myLidarLite.readDistance();

    return 1;
}
