#ifndef TSLXX_H
#define TSLXX_H

#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
  #include "pins_arduino.h"
  #include "WConstants.h"
#endif

#include <avr/io.h>
#include <avr/interrupt.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif



class TSLXX
{
    public:
        TSLXX();

        void init(uint8_t clkPin, uint8_t siPin, uint8_t analogPin);
        void readSensor (void);
        uint16_t p(int pixelN);
        uint8_t m, mi, i;
        static uint8_t CLK_PORT;
        static uint8_t CLK_PIN;
        static uint8_t SI_PORT;
        static uint8_t SI_PIN;
        static uint8_t _L_ARRAY;
        static uint8_t _L_ARRAY_1;
        static uint8_t* pixelBuffer;
        static uint8_t* pixels;
    private:
        static uint8_t CLK_REGISTER;
        static uint8_t SI_REGISTER;
        static uint8_t AO_PIN;
        static uint8_t ADC_CHANNEL;
};


#endif // TSLXX_H