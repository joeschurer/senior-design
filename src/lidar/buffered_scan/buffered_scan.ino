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
char mode = '0'; //1 = low res, 2 = high res, 3 = upload

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
volatile uint8_t maxPulses = 2*10;
//2*10 is 1.2 million almost high
//7*2 is 1.67 million over high
//50 is low res so prob add delay
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

//Buffer creation
// Max buffer size
const uint16_t bufSize = 240;
// Buffer for storing read values
uint8_t buf[bufSize];
// Index for buffer storage
uint16_t index = 0;

void setup(){
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

    if (SD.exists("test.txt"))
    {
      SD.remove("test.txt");
      Serial.println("test.txt removed");
    }

    //Discard first distance
    clkSetup(10,10);

     encoderSetup();

  
}

void loop(){
  //wait for instruction from serial
  while(waiting){
    delay(2000);
    Serial.println("Waiting for instruction");
    //Serial.println(TIMSK1);
    if(Serial.available()){
      mode = Serial.read();
      Serial.println(mode);
    }
    if(mode == '1' || mode == '2' || mode == '3'|| mode == '4'){
      waiting = false;
      if(mode == '1'){
        maxPulses = 50;
      }
      else if(mode == '2'){
        //maxPulses = 7*2;
        maxPulses = 2*5;
      } else if(mode == '3'){
        maxPulses = 2*5;
      }
    }
  }

  if(mode == '1'|| mode == '2'){
    delay(5000);
    uint16_t distance;
    uint8_t  newDistance = 0;
    uint8_t  c;
    newDistance = distanceFast(&distance);
    String dataBuf = String(distance) + "," + String(stepCount)+ "," + String(stepCount2)+ "," + String(dir1);
      //Serial.println(TIMSK1);
    Serial.println(dataBuf);

    
    myFile.println("BEGIN_SCAN");
    Serial.println("BEGIN_SCAN");
    //int startTime = millis();
    
    while(dir1==1){
      sweepStep();
      delay(5);
    }
    while(stepCount2 < 12800 && stepCount <30000){
      newDistance = distanceFast(&distance);
      String dataBuf = String(distance) + "," + String(stepCount)+ "," + String(stepCount2)+ "," + String(dir1);
      //Serial.println(TIMSK1);
      myFile.println(dataBuf);
      sweepStep();
      if(mode == '1'){
        delay(5);
      }
      //delay(5);
      //delay(5);
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
  } else if(mode == '3'){
    delay(5000);
    uint16_t distance;
    uint8_t  newDistance = 0;
    uint8_t  c;
    newDistance = distanceFast(&distance);
    String dataBuf = String(distance) + "," + String(stepCount)+ "," + String(stepCount2)+ "," + String(dir1);
      //Serial.println(TIMSK1);
    Serial.println(dataBuf);

    
    myFile.println("BEGIN_SCAN");
    Serial.println("BEGIN_SCAN");

    while(dir1==1){
      sweepStep();
      delay(5);
    }

    //LOOP HERE
    while(stepCount2 < 6400){
      newDistance = distanceFast(&distance);
      String dataBuf = String(distance) + "," + String(stepCount)+ "," + String(stepCount2)+ "," + String(dir1);
      //Serial.println(TIMSK1);
      myFile.println(dataBuf);
      enc1+=maxPulses/2;
      sweepStep();
      if(stepCount >= 6400){
        dir1 = 1; // toggle dir1
        PORTC |= (1<<d1); // Flip direction of sweep encoder
        while(dir1==1){
          sweepStep();
          delay(5);
         }
      }
      
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
  else if(mode == '4'){
    
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
