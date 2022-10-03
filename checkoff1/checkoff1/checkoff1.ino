
#include <stdint.h>
#include <Wire.h>
#include "lidarv3hp.h"
#include <SPI.h>
#include <SD.h>

LIDARLite_v3HP myLidarLite;

#define FAST_I2C
File myFile;

enum rangeType_T
{
    RANGE_NONE,
    RANGE_SINGLE,
    RANGE_CONTINUOUS,
    RANGE_TIMER
};

void setup()
{
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

    myLidarLite.configure(7);

    if (!SD.begin(4)) {
      Serial.println("initialization failed!");
      while (1);
    }

    myFile = SD.open("test.txt", FILE_WRITE);

    
}


void loop()
{
    uint16_t distance;
    uint8_t  newDistance = 0;
    uint8_t  c;

    int startTime = millis();
    for(int i=0;i<100;i++){
      newDistance = distanceFast(&distance);
      myFile.println(distance);
      Serial.println(distance);
    }

    int endTime = millis();
    Serial.println("Time: ");
    int finalTime = endTime-startTime;
    Serial.println(finalTime);
    while(1){
      myFile.close();
      delay(1);
    }
    
 
}

uint8_t distanceFast(uint16_t * distance)
{
    // 1. Wait for busyFlag to indicate device is idle. This must be
    //    done before triggering a range measurement.
    myLidarLite.waitForBusy();

    // 2. Trigger range measurement.
    myLidarLite.takeRange();

    // 3. Read previous distance data from device registers.
    //    After starting a measurement we can immediately read previous
    //    distance measurement while the current range acquisition is
    //    ongoing. This distance data is valid until the next
    //    measurement finishes. The I2C transaction finishes before new
    //    distance measurement data is acquired.
    *distance = myLidarLite.readDistance();

    return 1;
}
