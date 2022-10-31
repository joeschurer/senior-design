void encoderSetup()
{
  cli(); // Disable interrupts
  
  PCICR |= (0b00000111); // Turn on port b, c, d interrupts
  
  PCMSK0 |= ((1<<e1a) | (1<<e1b)); // Enable sweep a/b phase
  PCMSK1 |= (1<<e1z); // Enable sweep z phase
  PCMSK2 |= (1<<e2a) | (1<<e2b); // Enable elevation a/b phase
  
  sei(); // Enable interrupts
}

// Test Program
// Port 0 tracks number of encoder counts on A and B phases
// Port 1 triggers on Z phase pulse (absolute 0)

// Port 0 dedicated to sweeping encoder
ISR(PCINT0_vect) // Port 0 ISR
{
//  PCIFR |= (1<<0); // Manually clear IFG - redundant
  enc1++;
}

// Port 1 dedicated to elevation encoder
ISR(PCINT1_vect) // Port 1 ISR
{
  if (dir1 || (~dir1 && edge))
  {
    if (stepCount > 1000)
    {
      dir1 ^= 1; // toggle dir1
      PORTC ^= (1<<d1); // Flip direction of sweep encoder
      
      if (enc1 > 3900)
      {
        for (uint8_t i = 0;i<maxPulses;i++)
        {
          PORTD |= (1<<step2); // Move elevation motor
        }
      }
  
      Serial.print(dir1);
      Serial.print("\t");
      Serial.print(enc1);
      Serial.print("\t");
      Serial.print(stepCount);
      Serial.print("\t");
      Serial.println(enc2);

      // End current pulse at end of travel
      TCCR1B = 0; // Put timer in stop mode
      TIFR1 &= ~(1 << OCF1A); // Clear IFG
      TCNT1 = 0; // reset counter to 0
      pulseCount = maxPulses;
      
      // Clear any other pending IFGs
      PCIFR |= (0b111);

      edge = 0;
      stepCount = 0;
      enc1 = 0;

    }
  }
  else edge = 1;
}
