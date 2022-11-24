#include<Wire.h>
int start = 0;
bool waiting = true;
char mode  = '0';
void setup() {
  Serial.begin(230400);

}

void loop() {
  while(waiting){
    delay(2000);
    Serial.println("Waiting for instruction");
    //Serial.println(TIMSK1);
    if(Serial.available()){
      mode = Serial.read();
      Serial.println(mode);
    }
    if(mode == '4'){
      waiting = false;
      Serial.println("Starting");
    }
 }

  Serial.println("blah");
  Serial.println("file_begin");
  Serial.println("BEGIN_SCAN");
  for(unsigned long i=0;i < 100000;i++){
    Serial.println("DATA,DATA,DATA,DATA");
  }
  
  Serial.println("END_SCAN");
  Serial.println("file_end");


 waiting = true;
 mode = 0;

}
