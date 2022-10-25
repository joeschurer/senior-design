
#include <SD.h>

void setup()
{
  Serial.begin(9600);
  Serial.print("Initializing SD card...");
  pinMode(10, OUTPUT);
 
  if (!SD.begin(10)) {
    Serial.println("Card failed, or not present");
    return;
  }
  Serial.println("card initialized.");
 
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
}

void loop()
{
}