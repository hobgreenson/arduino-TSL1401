#ifndef TSL1401_H
#define TSL1401_H

class TSLXX ;

class TSL1401 : public TSLXX
{
public:
		TSL1401(uint8_t clkPin, uint8_t siPin, uint8_t analogPin);

		static volatile uint8_t pixels[ 128 ];
		static uint8_t pixelBuffer[ 128 ]; 

		const int L_ARRAY = 128;
		const int L_ARRAY_1 = 129;

};

	volatile uint8_t TSL1401::pixels[ 128 ] = {0};
	uint8_t TSL1401::pixelBuffer[ 128 ] = {0}; 

TSL1401::TSL1401(uint8_t clkPin, uint8_t siPin, uint8_t analogPin) {
	TSLXX TSL;
	TSL.init(clkPin, siPin, analogPin);
	TSL.pixelBuffer = pixelBuffer;
	TSL.pixels = pixels;
	TSL._L_ARRAY = L_ARRAY;
    TSL._L_ARRAY_1 = L_ARRAY_1;
}

#endif // TSL1401_H