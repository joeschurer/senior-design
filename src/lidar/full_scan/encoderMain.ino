void encoderSetup()
{
  cli(); // Disable interrupts
  
  PCICR |= (0b00000111); // Turn on port b, c, d interrupts
  
  PCMSK0 |= ((1<<e1a) | (1<<e1b)); // Enable sweep a/b phase
  PCMSK1 |= (1<<e1z); // Enable sweep z phase
  PCMSK2 |= (1<<e2a) | (1<<e2b) | (1<<e2z); // Enable elevation a/b phase
  
  sei(); // Enable interrupts
}

// Test Program
// Port 0 tracks number of encoder counts on A and B phases
// Port 1 triggers on Z phase pulse (absolute 0)

// Port B dedicated to sweep encoder A/B phase
ISR(PCINT0_vect) // Port 0 ISR
{
  enc1++;
}

// Port 1 dedicated to sweep z phase encoder
ISR(PCINT1_vect) // Port 1 ISR
{
  if (dir1 || (~dir1 && edge))
  {
    if (stepCount > 1000) // Don't trigger again immediately
    {
      dir1 ^= 1; // toggle dir1
      PORTC ^= (1<<d1); // Flip direction of sweep encoder

      Serial.print(dir1);
      Serial.print("\t");
      Serial.print(enc1);
      Serial.print("\t");
      Serial.print(stepCount);
      Serial.print("\t");
      Serial.print(enc2);
      Serial.print("\t");
      Serial.println(stepCount2);
      
      // End current pulse at end of travel
      TCCR1B = 0; // Put timer in stop mode
      TIFR1 |= (1<<OCF1A | 1<<OCF1B); // Clear timers IFGs
      TCNT1 = 0; // reset counter to 0
      pulseCount = 0; // Clear pulse count
      edge = 0; // Reset edge flag

      // Step elevation motor
      if (enc1 > 3900)
      {
        elevationStep();
      }

      stepCount = 0; // Reset step count
      enc1 = 0; // Reset sweep encoder count
      
      
      

      // Clear pin change IFGs
      PCIFR |= (0b111);

    }
  }
  else edge = 1;
}

// Port D dedicated to elevation encoder A/B phases
ISR(PCINT2_vect)
{
  enc2++;
}
