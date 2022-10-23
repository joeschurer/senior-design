#include <stdint.h>
#include <Wire.h>
#include "lidarv3hp.h"
#include <SPI.h>
#include <SD.h>

String dataBuf;
LIDARLite_v3HP myLidarLite;

//#define FAST_I2C
File myFile;
int config_val = 0; // Be accurate
bool waiting = true;
char mode = '0'; //1 = low res, 2 = high res, 3 = upload

// Define constants
uint16_t enc1 = 0;
uint16_t enc2 = 0;
uint16_t stepCount = 0;
uint16_t stepCount2 = 0;
bool dir1 = 1;
bool dir2 = 1;
bool edge = 0;

// Step, direction, encoder pins

// Port B variables
const uint8_t e1a = 0; // Sweep a/b phase
const uint8_t e1b = 1;

// Port C variables (0,1,2,3)
const uint8_t e1z = 0; // Sweep z phase

const uint8_t step1 = 1; // Step, direction sweep
const uint8_t d1 = 2;

// Port D variables (3,4,5,6,7)
const uint8_t e2a = 3; // Elevation a/b phase
const uint8_t e2b = 4;

const uint8_t step2 = 5; // Step, direction elevation
const uint8_t d2 = 6;

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

    clkSetup(3200);

  encoderSetup();

      
     
}


void loop(){
  //wait for instruction from serial
  while(waiting){
    delay(2000);
    Serial.println("Waiting for instruction");
    while(Serial.available()){
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

    
    //int startTime = millis();
    startClk();

    //dir s
    while(dir1 ==1){
      Serial.println("Loop one");
    }
    myFile.println("BEGIN_SCAN");
    Serial.println("BEGIN_SCAN");
    while(dir1 == 0){
      newDistance = distanceSingle(&distance);
      if(distance == 0){
        newDistance = distanceSingle(&distance);
      }
      String dataBuf = String(distance) + "," + String(stepCount);
      myFile.println(dataBuf);
      //Serial.println("Loop two");
      
    }
    //stopClk();
    
    myFile.println("END_SCAN");
    Serial.println("END_SCAN");
    myFile.print(dir1);
    myFile.print("\t");
    myFile.print(enc1);
    myFile.print("\t");
    myFile.print(stepCount);
    myFile.print("\t");
    myFile.println(enc2);

    //int endTime = millis();
    //Serial.println(endTime-startTime);
    myFile.close();

    Serial.println("done");
    waiting = true;
    mode = '0';
  }
  else if(mode == '3'){
    
    File dataFile = SD.open("test.txt");
    if (dataFile) {
      Serial.println("file_begin");
      while (dataFile.available()) {
        Serial.write(dataFile.read());
      }
      dataFile.close();
      Serial.println("file_end");
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

uint8_t distanceSingle(uint16_t * distance){
    myLidarLite.waitForBusy();
    myLidarLite.takeRange();
    myLidarLite.waitForBusy();
    *distance = myLidarLite.readDistance();

    return 1;
}
