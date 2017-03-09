#ifndef TSL1406_H
#define TSL1406_H

class TSLXX ;

class TSL1406 : public TSLXX
{
public:
		TSL1406(uint8_t clkPin, uint8_t siPin, uint8_t analogPin);

		static volatile uint8_t pixels[ 768 ];
		static uint8_t pixelBuffer[ 768 ]; 

		const int L_ARRAY = 768;
		const int L_ARRAY_1 = 769;

};

	volatile uint8_t TSL1406::pixels[ 768 ] = {0};
	uint8_t TSL1406::pixelBuffer[ 768 ] = {0}; 

TSL1406::TSL1406(uint8_t clkPin, uint8_t siPin, uint8_t analogPin) {
	TSLXX TSL;
	TSL.init(clkPin, siPin, analogPin);
	TSL.pixelBuffer = pixelBuffer;
	TSL.pixels = pixels;
	TSL._L_ARRAY = L_ARRAY;
    TSL._L_ARRAY_1 = L_ARRAY_1;
}

#endif // TSL1406_H