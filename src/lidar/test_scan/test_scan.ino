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

void setup(){
    pinMode(7, OUTPUT);
    pinMode(10, OUTPUT);
    Serial.begin(9600);

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

    SD.remove("test.txt");
    myFile = SD.open("test.txt", FILE_WRITE);

    //Discard first distance
    newDistance = distanceFast(&distance);
     
}


void loop(){
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

    
    while(1){
      digitalWrite(7,HIGH);
      delay(2000);
      digitalWrite(7,LOW);
      delay(1000);
      Serial.println("Scan Complete");
    }
    
 
}

uint8_t distanceFast(uint16_t * distance)
{
    myLidarLite.waitForBusy();
    myLidarLite.takeRange();
    *distance = myLidarLite.readDistance();

    return 1;
}
