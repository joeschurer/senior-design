
void zeroElevation()

{

  // Mask elevation a/b phases, enable elevation z phase

  Serial.println(PCMSK2);

  PCMSK2 &= ~(1<<e2a | 1<<e2b);

  PCMSK2 |= (1<<e2z);

 

  Serial.println(PCMSK2);

  // Set elevation direction to down

  dir2 = 0;

  PORTD &= ~(1<<d2);

 

  while(enc2 <= 0)

  {

    PIND |= (1<<step2); // Jog elevation motor until z phase limit reached

    delayMicroseconds(200); // Slow down reasonable amount

  }

 

  // Reset enc2 count

  enc2 = 0;

  stepCount2 = 0;

 

  // Set to 8800 for 180 degree start

  // Set to 5600 for 135 degree start

  // Set to 2400 for 90 degree start
  const uint16_t elevationOffset = 8800;
 
  if(mode == '3'){
    const uint16_t elevationOffset = 5600;
  }
 

  while(stepCount2 < elevationOffset)

  {

    stepCount2++;

    PIND |= (1<<step2); // Jog until elevation angle of 180 degrees reached

    delayMicroseconds(200);

  }

 

  // Flip direction back

  dir2 = 1;

  PORTD |= (1<<d2);

 

  // Reset enc2, stepCount2

 enc2 = 0;

  stepCount2 = 0;

 

  // Pull step 2 low if not done already

  PORTD &= ~(1<<step2);

}
