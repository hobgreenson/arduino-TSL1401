#ifndef TSL1401
#define TSL1401

#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL
#define L_ARRAY 128
#define L_ARRAY_1 129



class TSL1401
{
    public:
        TSL1401(uint8_t clkPin, uint8_t siPin, uint8_t analogPin);
    private:
        uint8_t CLKcycleCounter = 0; // counts the number of clock cycles expired
        volatile uint8_t pixels[L_ARRAY] = {0}; // pixel buffer - holds data from ADC
        uint8_t i, m, pix; // i is an index for loops, m is the mimimum pixel value
        uint8_t mi; // index of darkest pixel on the array
        uint8_t CLK_PORT;
        uint8_t CLK_PIN;
        uint8_t CLK_REGISTER;
        uint8_t SI_PORT;
        uint8_t SI_PIN;
        uint8_t SI_REGISTER;
        uint8_t AO_PIN;
        uint8_t ADC_CHANNEL;
};

TSL1401::TSL1401(uint8_t clkPin, uint8_t siPin, uint8_t analogPin) {
        pinMode(clkPin, OUTPUT);
        pinMode(siPin, OUTPUT);
        pinMode(analogPin, INPUT);
        CLK_PORT = digitalPinToPort(clkPin);
        CLK_PIN = digitalPinToBitMask(clkPin);
        CLK_REGISTER = portOutputRegister(CLK_PORT);
        SI_PORT = digitalPinToPort(siPin);
        SI_PIN = digitalPinToBitMask(siPin);
        SI_REGISTER = portOutputRegister(SI_PORT);
        AO_PIN = analogPin;
}


// ADC interrupt routine - this happens when the ADC is finished taking a sample
ISR(ADC_vect) {
    // if AO is sending pixel data (first 128 clock cycles of each frame)
    // then do an ADC and put it in the pixel buffer
    if (CLKcycleCounter < L_ARRAY) {
        // start next CLK cycle
        __asm__("nop\n\t"); // delay one CPU clock cycle = 62.5 nanosec
        __asm__("nop\n\t"); // delay one CPU clock cycle = 62.5 nanosec
        __asm__("nop\n\t"); // delay one CPU clock cycle = 62.5 nanosec
        CLK_PORT |= (1 << CLK_PIN); // CLK = 5V
        __asm__("nop\n\t"); // delay one CPU clock cycle = 62.5 nanosec
        __asm__("nop\n\t"); // delay one CPU clock cycle = 62.5 nanosec
        __asm__("nop\n\t"); // delay one CPU clock cycle = 62.5 nanosec
        CLK_PORT &= ~(1 << CLK_PIN); // CLK = 0V
        pixels[CLKcycleCounter] = ADCH; // high byte of ADC data
        CLKcycleCounter++; // iterate clock cycle counter
    } else if (CLKcycleCounter == L_ARRAY || CLKcycleCounter == L_ARRAY_1) { 
        // there is an extra clock cycle at the end (129) with no data
        // do a second extra clock for charge transfer
        // | CLKcycleCounter == L_ARRAY_1
        // start next CLK cycle
        __asm__("nop\n\t"); // delay one CPU clock cycle = 62.5 nanosec
        __asm__("nop\n\t"); // delay one CPU clock cycle = 62.5 nanosec
        __asm__("nop\n\t"); // delay one CPU clock cycle = 62.5 nanosec
        CLK_PORT |= (1 << CLK_PIN); // CLK = 5V
        __asm__("nop\n\t"); // delay one CPU clock cycle = 62.5 nanosec
        __asm__("nop\n\t"); // delay one CPU clock cycle = 62.5 nanosec
        __asm__("nop\n\t"); // delay one CPU clock cycle = 62.5 nanosec
        CLK_PORT &= ~(1 << CLK_PIN); // CLK = 0V
        CLKcycleCounter++; // iterate clock cycle counter
    } else { 
        // start next frame by sending a pulse on SI
        SI_PORT |= (1 << SI_PIN); // SI = 5V
        __asm__("nop\n\t"); // delay one CPU clock cycle = 62.5 nanosec
        __asm__("nop\n\t"); // delay one CPU clock cycle = 62.5 nanosec
        CLK_PORT |= (1 << CLK_PIN); // CLK = 5V
        __asm__("nop\n\t"); // delay one CPU clock cycle = 62.5 nanosec
        __asm__("nop\n\t"); // delay one CPU clock cycle = 62.5 nanosec
        SI_PORT &= ~(1 << SI_PIN); // SI = 0V, must go low before next clock rise!!!
        __asm__("nop\n\t"); // delay one CPU clock cycle = 62.5 nanosec
        __asm__("nop\n\t"); // delay one CPU clock cycle = 62.5 nanosec
        CLK_PORT &= ~(1 << CLK_PIN); // CLK = 0V
        CLKcycleCounter = 0; // reset clock cycle counter to zero  
    }
    ADCSRA |= (1 << ADSC); // start next conversion
}

int main(void) {
    // use serial library for communication with MATLAB
    Serial.begin(4000000);
  
    // set up digital output pins on port B; CLK = B3 (pin 11) and SI = B0 (pin 8)
    CLK_REGISTER = 0; // zero the CLKregister
    SI_REGISTER = 0; // zero the SIregister
    CLK_PORT = 0; // zero the register
    CLK_PORT = 0; // zero the SIregister
    CLK_REGISTER |= (1 << CLK_PIN); // CLK(B3) 
    SI_REGISTER |= (1 << SI_PIN); // SI(B0)
    CLK_PORT &= ~(1 << CLK_PIN); // CLK = 0V
    SI_PORT &= ~(1 << SI_PIN); // SI = 0V
  
    // ADC set-up; ADC will read the analog output (AO) voltage from the sensor
    sei(); // enable global interrupts
    ADCSRA = 0; // zero the register
    ADCSRA |= (1 << ADEN); // turn on ADC
    ADCSRA |= (1 << ADPS2); // set clock to F_CPU/16 = 1Mhz
    ADCSRA |= (1 << ADIE); // enable ADC interrupt


#if defined(analogPinToChannel) 
    ADC_CHANNEL = analogPinToChannel(analogPin);
#else
    ADC_CHANNEL = analogPin - 18;
#endif // analogPinToChannel

    if (ADC_CHANNEL == 0) { ADMUX |= (1 << ADLAR) | (1 << REFS0); }
    if (ADC_CHANNEL == 1) { ADMUX |= (1 << ADLAR) | (1 << REFS0) | (1 << MUX0); }
    if (ADC_CHANNEL == 2) { ADMUX |= (1 << ADLAR) | (1 << REFS0) | (1 << MUX1); }
    if (ADC_CHANNEL == 3) { ADMUX |= (1 << ADLAR) | (1 << REFS0) | (1 << MUX1) | (1 << MUX0); }
    if (ADC_CHANNEL == 4) { ADMUX |= (1 << ADLAR) | (1 << REFS0) | (1 << MUX1); }
    if (ADC_CHANNEL == 5) { ADMUX |= (1 << ADLAR) | (1 << REFS0) | (1 << MUX2) | (1 << MUX0); }

#if NUM_ANALOG_INPUTS == 8

    if (ADC_CHANNEL == 6) { ADMUX |= (1 << ADLAR) | (1 << REFS0) | (1 << MUX2) | (1 << MUX1); }
    if (ADC_CHANNEL == 7) { ADMUX |= (1 << ADLAR) | (1 << REFS0) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0); }

#endif // NUM_ANALOG_INPUTS


    // start first frame
    SI_PORT |= (1 << SI_PIN); // SI = 5V
    __asm__("nop\n\t"); // delay one CPU clock cycle = 62.5 nanosec
    __asm__("nop\n\t"); // delay one CPU clock cycle = 62.5 nanosec
    CLK_PORT |= (1 << CLK_PIN); // CLK = 5V
    __asm__("nop\n\t"); // delay one CPU clock cycle = 62.5 nanosec
    __asm__("nop\n\t"); // delay one CPU clock cycle = 62.5 nanosec
    SI_PORT &= ~(1 << SI_PIN); // SI = 0V, must go low before next clock rise!!!
    __asm__("nop\n\t"); // delay one CPU clock cycle = 62.5 nanosec
    __asm__("nop\n\t"); // delay one CPU clock cycle = 62.5 nanosec
    CLK_PORT &= ~(1 << CLK_PIN); // CLK = 0V
  
    // first conversion
    ADCSRA |= (1 << ADSC);
    
    while (1) {
        // find the minimum pixel value, indicating tail position
        m = 30;
        mi = 0;
        for (i = 1; i < L_ARRAY; ++i) {
            if (pixels[i] < m) {
                mi = i;
                m = pixels[i];
            }
        }
        // send minimum pixel index
        Serial.write(mi);
    } 
    return 0;
}


#endif // TSL1401