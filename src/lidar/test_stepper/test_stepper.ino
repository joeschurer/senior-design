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
int config_val = 7;
bool waiting = true;
char mode = '0'; //1 = low res, 2 = high res, 3 = upload

//https://www.elegoo.com/products/elegoo-uln2003-5v-stepper-motor-uln2003-driver-board

//Motor control
const int stepsPerRevolution = 2048;  // change this to fit the number of steps per revolution
const int RevolutionsPerMinute = 15;         // Adjustable range of 28BYJ-48 stepper is 0~17 rpm

// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 8, 5, 6, 9);

void setup(){
  //motor setup
  myStepper.setSpeed(RevolutionsPerMinute);

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
    delay(10000);
    uint16_t distance;
    uint8_t  newDistance = 0;
    uint8_t  c;
    newDistance = distanceFast(&distance);

    myFile.println("BEGIN_SCAN");
    Serial.println("BEGIN_SCAN");
    //int startTime = millis();

    /*
    for(int i=0;i<1000;i++){
      newDistance = distanceFast(&distance);
      myFile.println(distance);
      //Serial.println(distance);

    }
    */
  int stepCount = 0;
  while(stepCount < stepsPerRevolution){
    newDistance = distanceFast(&distance);
    myStepper.step(1);
    //create string to write to file with distance and angle
    String dataBuf = String(distance) + "," + String(stepCount) + "\n";
    myFile.println(dataBuf);
    stepCount++;
  }
    myFile.println("END_SCAN");
    Serial.println("END_SCAN");

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
