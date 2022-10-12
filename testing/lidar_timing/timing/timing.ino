#include <stdint.h>
#include <Wire.h>
#include "lidarv3hp.h"
#include <SPI.h>
#include <SD.h>

String dataBuf;
LIDARLite_v3HP myLidarLite;

#define FAST_I2C
File myFile;
int config_val = 7;

const uint16_t bufSize = 240;
uint8_t buf[bufSize];
uint16_t index = 0;

unsigned int chunkSize = 512;

enum rangeType_T
{
    RANGE_NONE,
    RANGE_SINGLE,
    RANGE_CONTINUOUS,
    RANGE_TIMER
};

void setup(){
    dataBuf.reserve(1024);
    chunkSize = myFile.availableForWrite();
    pinMode(7, OUTPUT);
    // Initialize Arduino serial port (for display of ASCII output to PC)
    Serial.begin(9600);

    // Initialize Arduino I2C (for communication to LidarLite)
    Wire.begin();
    #ifdef FAST_I2C
        #if ARDUINO >= 157
            Wire.setClock(400000UL); // Set I2C frequency to 400kHz (for Arduino Due)
        #else
            TWBR = ((F_CPU / 400000UL) - 16) / 2; // Set I2C frequency to 400kHz
        #endif
    #endif

    //7 is good
    myLidarLite.configure(config_val);

    if (!SD.begin(10)) {
      Serial.println("initialization failed!");
      while (1);
    }

    //SD.remove("test.txt");
    myFile = SD.open("test.txt", FILE_WRITE);

    //Discard first distance
     
}


void loop(){
    //delay(3000);
     digitalWrite(7,HIGH);
      delay(2000);
      digitalWrite(7,LOW);
      delay(10000);
    uint16_t distance;
    uint8_t  newDistance = 0;
    uint8_t  c;

    int startTime = millis();
    for(int i=0;i<10000;i++){
      newDistance = distanceFast(&distance);
      myFile.print(distance);
    }
    int endTime = millis();
    //bufFlush();

    myFile.print("Time: ");
    int finalTime = endTime-startTime;

    myFile.println(finalTime);
    myFile.println(config_val);
    myFile.println("Please help me");
    myFile.close();
    
    while(1){
      digitalWrite(7,HIGH);
      delay(2000);
      digitalWrite(7,LOW);
      delay(1000);
      Serial.println("working");
    }
    
 
}

uint8_t distanceFast(uint16_t * distance)
{
    myLidarLite.waitForBusy();
    myLidarLite.takeRange();
    *distance = myLidarLite.readDistance();

    return 1;
}

uint8_t distanceSingle(uint16_t * distance)
{
    myLidarLite.waitForBusy();
    myLidarLite.takeRange();
    myLidarLite.waitForBusy();
    *distance = myLidarLite.readDistance();

    return 1;
}
