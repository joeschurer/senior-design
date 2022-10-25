#include <Wire.h>

// Define constants
uint16_t enc1 = 0;
uint16_t enc2 = 0;
uint16_t stepCount = 0;
uint16_t stepCount2 = 0;
bool dir1 = 1;
bool dir2 = 1;
bool edge = 0;

// Step, direction, encoder pins

// Port B variables
const uint8_t e1a = 0; // Sweep a/b phase
const uint8_t e1b = 1;

// Port C variables (0,1,2,3)
const uint8_t e1z = 0; // Sweep z phase

const uint8_t step1 = 1; // Step, direction sweep
const uint8_t d1 = 2;

// Port D variables (3,4,5,6,7)
const uint8_t e2a = 3; // Elevation a/b phase
const uint8_t e2b = 4;

const uint8_t step2 = 5; // Step, direction elevation
const uint8_t d2 = 6;


void setup()
{
  Serial.begin(115200); // Begin serial
  Serial.print("Begin serial");

  // For real square wave step signal at 40kHz, clk divider = 150
  // 1kHz, clk divider = 8000
  // Clk Frequency = 2*(Square Wave Frequency) = 16MHz/159 = 40kHz
  // Output pins for motors (STEP 2)
  clkSetup(3200);

  encoderSetup();

  startClk();
}

void loop()
{

}
