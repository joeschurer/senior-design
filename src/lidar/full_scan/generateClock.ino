void clkSetup(uint16_t SCD, uint16_t ECD) // Configure sweep step clock
{
  // Configure timer 1 to act as step clk generator
  cli(); // Disable interrupts
  TCCR1A &= ~(0xFF); // Clear timer 1 configuration registers
  TCCR1B &= ~(0xFF);
    
  TCNT1 = 0; // Clear timer 1 count 
  TCCR1B = 0; // Put timer 1 in stop mode
  
  OCR1AH = highByte(SCD); // Load time limit into registers
  OCR1AL = lowByte(SCD);

  OCR1BH = highByte(ECD);
  OCR1BL = lowByte(ECD);
  
  TIMSK1 = (1 << OCIE1A); // Enable capture/compare interrupt 1
  TIFR1 &= ~(1 << OCF1A); // Clear IFG

  // Configure output pins for motor driver step and direction signals
  // Write 1 to DDRx to set as output, 0 as input

  DDRC |= (1<<step1) | (1<<d1); // Enable step1/d1 as output
  DDRD |= (1<<step2) | (1<<d2); // Enable step2/d1 as output

  PORTC |= (1<<d1); // Set sweep direction output to 1
  PORTD |= (1<<d2); // Set elevation direction output to 2
    
  sei(); // Enable interrupts
}

void sweepStep() // Starts step clock
{
  TCNT1 = 0;// Reset timer count
  pulseCount = 0;
  TIFR1 |= (1<<OCF1A | 1<<OCF1B); // Clear IFGs
  TCCR1B |= 0x01; // Put timer in start mode with 1 prescale
}

void elevationStep()
{
  TIMSK1 = (1<<OCIE1B); // Enable elevation, disable sweep interrupts
  TIFR1 |= (1<<OCF1A | 1<<OCF1B); // Clear IFGs
  TCNT1 = 0;
  pulseCount = 0;
  TCCR1B |= 0x01;
}

ISR(TIMER1_COMPA_vect) // ISR for Timer 1
{
  PINC |= (1<<step1); // Toggle PC1
  pulseCount++; // increment pulse count
    
  if (pulseCount >= maxPulses)
  {
    TCCR1B = 0; // Put timer in stop mode
  }
    
  TIFR1 |= (1<<OCF1A | 1<<OCF1B); // Clear IFG
  TCNT1 = 0; // reset counter to 0
  if(dir1){
    stepCount++;
  }else{
    stepCount--;
  }
  
}

ISR(TIMER1_COMPB_vect) // ISR for Timer 0
{
  PIND |= (1<<step2); // Toggle step 2
  pulseCount++; // increment pulse count

  if (pulseCount >= maxPulses*10)
  {
    TCCR1B = 0; // Put timer in stop mode
    TIMSK1 = (1<<OCIE1A); // Enable elevation, disable sweep interrupts
  }

  TIFR1 |= (1<<OCF1A | 1<<OCF1B); // Clear IFG
  TCNT1 = 0; // Reset count
  stepCount2++;
}
