#include <stdint.h>
#include <Wire.h>
#include "lidarv3hp.h"
#include <SPI.h>
#include <SD.h>
#include <Stepper.h>

String dataBuf;
LIDARLite_v3HP myLidarLite;

//#define FAST_I2C
File myFile;
int config_val = 2;
bool waiting = true;
char mode = '0'; //1 = low res, 2 = high res, 3 = upload
void setup(){

    pinMode(7, OUTPUT);
    pinMode(10, OUTPUT);
    Serial.begin(230400);

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
    uint8_t  newDistance = 0;
    uint8_t  c;
    newDistance = distanceFast(&distance);

    myFile.println("BEGIN_SCAN");
    Serial.println("BEGIN_SCAN");
    /*/unsigned long startTime = millis();
    for(int i = 0; i< 100000;i++){
      newDistance = distanceFast(&distance);
      String dataBuf = String(distance) + "," + String(distance)+ "," + String(distance) + "\n";
      myFile.print(dataBuf);
      if(i%10000==0){
        Serial.println(i);
      }
    }*/

    for(unsigned long i = 0; i< 100000;i++){
      newDistance = distanceFast(&distance);
      String dataBuf = String(distance) + "," + String(distance)+ "," + String(distance) + "\n";
      myFile.print(dataBuf);
      //Serial.println(dataBuf);
      if(i%10000==0){
        Serial.println(i);
      }
    }
    myFile.println("FROM PCB :)");
    //unsigned long endTime = millis();
    myFile.println("END_SCAN");
    Serial.println("END_SCAN");

    //Serial.println(endTime-startTime);
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

uint8_t distanceFast(uint16_t * distance)
{
    myLidarLite.waitForBusy();
    myLidarLite.takeRange();
    *distance = myLidarLite.readDistance();

    return 1;
}
