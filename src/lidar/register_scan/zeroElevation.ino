// To be manually updated each time the mechanical system is changed

void zeroElevation()
{
  // Mask elevation a/b phases, enable elevation z phase
  PCMSK2 &= ~(1<<e2a | 1<<e2b);
  PCMSK2|= (1<<e2z);

  // Set elevation direction to down
  dir2 = 0;
  PORTD &= ~(1<<d2);

  while(enc2 <= 0)
  {
    PIND |= (1<<step2); // Jog elevation motor until z phase limit reached
    delay(5); // Slow down reasonable amount
  }

  // Reset interrupt mask
  PCMSK2 &= ~(1<<e2z);
  PCMSK2 |= (1<<e2a | 1<<e2b);

  // Flip direction back
  dir2 = 1;
  PORTD |= (1<<d2);

  // Reset enc2 count
  enc2 = 0;

  // MANUALLY ADJUST ONCE SETUP COMPLETE
  const uint8_t elevationOffset = 1000;
  
  while(enc2 < elevationOffset)
  {
    PIND |= (1<<step2); // Jog until elevation angle of 180 degrees reached
    delay(3);
  }

  // Reset enc2
  enc2 = 0;

  // Pull step 2 low if not done already
  PORTD &= ~(1<<step2);
}
