void clkSetup(uint16_t timeAllowed) // Configure step clock
{
  // Configure timer 1 to act as step clk generator
  cli(); // Disable interrupts
  TCCR1A &= ~(0xFF); // Clear timer 1 configuration registers
  TCCR1B &= ~(0xFF);
  
  // TCCR1A |= (1<<6); // Set Timer 1 to toggle OC1A on timer trigger
  
  TCNT1 = 0; // Clear timer 1 count 
  TCCR1B = 0; // Put timer 1 in stop mode
  
  OCR1AH = highByte(timeAllowed); // Load time limit into registers
  OCR1AL = lowByte(timeAllowed);
  
  TIMSK1 |= (1 << OCIE1A); // Enable capture/compare interrupt 1
  TIFR1 &= ~(1 << OCF1A); // Clear IFG

  // Configure output pins for motor driver step and direction signals
  // Write 1 to DDRx to set as output, 0 as input

  DDRC |= (1<<step1) | (1<<d1); // Enable step1/d1 as output
  DDRD |= (1<<step2) | (1<<d2); // Enable step2/d1 as output

  PORTC |= (1<<d1); // Set sweep direction output to 1
  PORTD |= (1<<d2); // Set elevation direction output to 2
    
  sei(); // Enable interrupts
}

void startClk() // Starts step clock
{
  TCNT1 = 0;// Reset count
  
  TCCR1B |= 0x01; // Put timer in start modee with 1 prescale
}

void stopClk()
{
  TCCR1B = 0; // Put timer in stop mode
  TCNT1 = 0; // Reset count

  TIFR1 &= ~(1 << OCF1A); // Clear IFG just in case
}

ISR(TIMER1_COMPA_vect) // ISR for Timer 1
{
  if (OCF1A) // Ensure that correct IFG triggered
  {
    TIFR1 &= ~(1 << OCF1A); // Clear IFG

    PINC |= (1<<step1); // Toggle PB2 (pin 10)

    stepCount++;
    TCNT1 = 0; // reset counter to 0
  }
}
