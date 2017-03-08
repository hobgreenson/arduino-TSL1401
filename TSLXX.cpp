#ifndef TSLXX_CPP
#define TSLXX_CPP


#include "TSLXX.h"

#include "ic-classes.h"


// #ifdef IC_TSL1401
//     #include "TSL1401.h"
// #endif
// #ifdef IC_TSL1402
//     #include "TSL1402.h"
// #endif
// #ifdef IC_TSL1406
//     #include "TSL1406.h"
// #endif
// #ifdef IC_TSL1412
//     #include "TSL1412.h"
// #endif
// #ifdef IC_TSL201
//     #include "TSL201.h"
// #endif
// #ifdef IC_LF1401
//     #include "LF1401.h"
// #endif

//extern const int L_ARRAY;
//extern const int L_ARRAY_1;

//global vars
uint8_t CLKcycleCounter = 0; // counts the number of clock cycles expired

#define NOP __asm__ __volatile__ ("nop\n\t")


uint8_t TSLXX::CLK_PORT;
uint8_t TSLXX::CLK_PIN;
uint8_t TSLXX::SI_PORT;
uint8_t TSLXX::SI_PIN;
uint8_t TSLXX::_L_ARRAY_1;
uint8_t TSLXX::_L_ARRAY;
uint8_t TSLXX::CLK_REGISTER;
uint8_t TSLXX::SI_REGISTER;
uint8_t TSLXX::AO_PIN;
uint8_t TSLXX::ADC_CHANNEL;

uint8_t* TSLXX::pixelBuffer;
uint8_t* TSLXX::pixels;

TSLXX::TSLXX() {
}

void TSLXX::init(uint8_t clkPin, uint8_t siPin, uint8_t analogPin) {
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

}


// ADC interrupt routine - this happens when the ADC is finished taking a sample
ISR(ADC_vect) {
    // if AO is sending pixel data (first 128 clock cycles of each frame)
    // then do an ADC and put it in the pixel buffer
    if (CLKcycleCounter < TSLXX::_L_ARRAY) {
        // start next CLK cycle
        NOP; NOP; NOP; // delay three CPU clock cycle = 3x 62.5 nanosec = 187.5 nanosec

        TSLXX::CLK_PORT |= (1 << TSLXX::CLK_PIN); // CLK = 5V

        NOP; NOP; NOP; // delay three CPU clock cycle = 3x 62.5 nanosec = 187.5 nanosec

        TSLXX::CLK_PORT &= ~(1 << TSLXX::CLK_PIN); // CLK = 0V

        TSLXX::pixels[CLKcycleCounter] = ADCH; // high byte of ADC data

        CLKcycleCounter++; // iterate clock cycle counter

    } else if (CLKcycleCounter == TSLXX::_L_ARRAY || CLKcycleCounter == TSLXX::_L_ARRAY_1) { 
        // there is an extra clock cycle at the end (129) with no data
        // do a second extra clock for charge transfer
        // | CLKcycleCounter == L_ARRAY_1
        // start next CLK cycle
        NOP; NOP; NOP; // delay three CPU clock cycle = 3x 62.5 nanosec = 187.5 nanosec

        TSLXX::CLK_PORT |= (1 << TSLXX::CLK_PIN); // CLK = 5V

        NOP; NOP; NOP; // delay three CPU clock cycle = 3x 62.5 nanosec = 187.5 nanosec

        TSLXX::CLK_PORT &= ~(1 << TSLXX::CLK_PIN); // CLK = 0V

        CLKcycleCounter++; // iterate clock cycle counter

    } else { 
        // start next frame by sending a pulse on SI
        TSLXX::SI_PORT |= (1 << TSLXX::SI_PIN); // SI = 5V

        NOP; NOP; // delay two CPU clock cycle = 3x 62.5 nanosec = 125 nanosec

        TSLXX::CLK_PORT |= (1 << TSLXX::CLK_PIN); // CLK = 5V

        NOP; NOP; // delay two CPU clock cycle = 3x 62.5 nanosec = 125 nanosec

        TSLXX::SI_PORT &= ~(1 << TSLXX::SI_PIN); // SI = 0V, must go low before next clock rise!!!

        NOP; NOP; // delay two CPU clock cycle = 3x 62.5 nanosec = 125 nanosec

        TSLXX::CLK_PORT &= ~(1 << TSLXX::CLK_PIN); // CLK = 0V

        CLKcycleCounter = 0; // reset clock cycle counter to zero  
    }
//      ADCSRA |= (1 << ADSC); // start next conversion
}

void TSLXX::readSensor() {

    // start first frame
    SI_PORT |= (1 << SI_PIN); // SI = 5V

    NOP; NOP; // delay two CPU clock cycle = 3x 62.5 nanosec = 125 nanosec

    CLK_PORT |= (1 << CLK_PIN); // CLK = 5V

    NOP; NOP; // delay two CPU clock cycle = 3x 62.5 nanosec = 125 nanosec

    SI_PORT &= ~(1 << SI_PIN); // SI = 0V, must go low before next clock rise!!!

    NOP; NOP; // delay two CPU clock cycle = 3x 62.5 nanosec = 125 nanosec

    CLK_PORT &= ~(1 << CLK_PIN); // CLK = 0V

    // first conversion
    ADCSRA |= (1 << ADSC);
    

    //while (1) {
/*        // find the minimum pixel value, indicating tail position
        m = 30;
        mi = 0;
        for (i = 1; i < _L_ARRAY; ++i) {
            if (pixels[i] < m) {
                mi = i;
                m = pixels[i];
            }
        }
        // send minimum pixel index*/
        // copy volatile array into non-volatile array
        for(unsigned int i = 0; i < TSLXX::_L_ARRAY; ++i) {
            TSLXX::pixelBuffer[i] = TSLXX::pixels[i];
        }
        TSLXX::pixelBuffer[TSLXX::_L_ARRAY] = 0xFF;
    return;
}


uint16_t TSLXX::p(int pixelN) {
    return TSLXX::pixelBuffer[pixelN];
}

#endif // TSLXX_CPP