
/*
  SD card read/write
  This example shows how to read and write data to and from an SD card file
  The circuit:
   SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)
*/

#include <Wire.h>
#include <LIDARLite.h>
LIDARLite lidarLite;
int cal_cnt = 0;
int iter =0;

void setup(){
  Serial.begin(9600); // Initialize serial connection to display distance readings
    pinMode(7, OUTPUT);
  Serial.println("Test");
  lidarLite.begin(0, true); // Set configuration to default and I2C to 400 kHz
  lidarLite.configure(0); // Change this number to try out alternate configurations

}

void loop()
{
  int dist;

  // At the beginning of every 100 readings,
  // take a measurement with receiver bias correction
  if ( cal_cnt == 0 ) {
    dist = lidarLite.distance();      // With bias correction
  } else {
    dist = lidarLite.distance(false); // Without bias correction
  }

  // Increment reading counter
  cal_cnt++;
  cal_cnt = cal_cnt % 100;

  // Display distance
  Serial.print(dist);
  Serial.println(" cm");

  if(dist > 5){
    digitalWrite(7, HIGH);   // turn the LED on (HIGH is the voltage level)
  }else{
    digitalWrite(7,LOW);
  }
  
  
}
