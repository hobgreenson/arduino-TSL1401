#ifndef TSL201_H
#define TSL201_H

class TSLXX ;

class TSL201 : public TSLXX
{
public:
		TSL201(uint8_t clkPin, uint8_t siPin, uint8_t analogPin);

		static volatile uint8_t pixels[ 64 ];
		static uint8_t pixelBuffer[ 64 ]; 

		const int L_ARRAY = 64;
		const int L_ARRAY_1 = 65;

};

	volatile uint8_t TSL201::pixels[ 64 ] = {0};
	uint8_t TSL201::pixelBuffer[ 64 ] = {0}; 

TSL201::TSL201(uint8_t clkPin, uint8_t siPin, uint8_t analogPin) {
	TSLXX TSL;
	TSL.init(clkPin, siPin, analogPin);
	TSL.pixelBuffer = pixelBuffer;
	TSL.pixels = pixels;
	TSL._L_ARRAY = L_ARRAY;
    TSL._L_ARRAY_1 = L_ARRAY_1;
}

#endif // TSL201_H