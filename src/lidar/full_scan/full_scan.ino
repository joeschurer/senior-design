#include <stdint.h>
#include <Wire.h>
#include "lidarv3hp.h"
#include <SPI.h>
#include <SD.h>

String dataBuf;
LIDARLite_v3HP myLidarLite;

//#define FAST_I2C
File myFile;
int config_val = 2; // Be accurate
bool waiting = true;
char mode = '1'; //1 = low res, 2 = high res, 3 = upload

//From Adams code

// Define constants
// Store encoder counts on sweep and elevation motors
uint16_t enc1 = 0;
uint16_t enc2 = 0;

// Store step commands on sweep and elevation motors
uint16_t stepCount = 0;
uint16_t stepCount2 = 0;

// Store direction on sweep and elevation motors
bool dir1 = 1;
bool dir2 = 1;

// For proper edge detection
bool edge = 0;

// Determines number of pulses in one commanded step
const uint8_t maxPulses = 2*10; // Sets number of pulses in a step
uint8_t pulseCount = 0; // Iterator - do not change

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

const uint8_t e2z = 7; // Elevation z phase

void setup(){
    Serial.begin(115200);

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
    clkSetup(10,10);

     encoderSetup();

  
}

void loop(){
  //wait for instruction from serial
  while(waiting){
    delay(2000);
    Serial.println("Waiting for instruction");
    while(Serial.available()){
      char temp = Serial.read();
      if(temp == '1' || temp == '2' || temp == '3'){
      mode = temp;
      }
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
    //int startTime = millis();
    
    //dir 
    /*while(dir1 ==1){
      startClk();
      delay(2);
    }*/
    while(stepCount2 < 4800){
      newDistance = distanceFast(&distance);
      String dataBuf = String(distance) + "," + String(stepCount)+ "," + String(stepCount2)+ "," + String(dir1);
      Serial.println(dataBuf);
      myFile.println(dataBuf);
      sweepStep();
      //Serial.println("Loop two"); 
    }
    myFile.println("END_SCAN");
    Serial.println("END_SCAN");
    myFile.print(dir1);
    myFile.print("\t");
    myFile.print(enc1);
    myFile.print("\t");
    myFile.print(stepCount);
    myFile.print("\t");
    myFile.println(enc2);

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
