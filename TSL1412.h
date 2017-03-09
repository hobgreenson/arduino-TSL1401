#ifndef TSL1412_H
#define TSL1412_H

class TSLXX ;

class TSL1412 : public TSLXX
{
public:
		TSL1412(uint8_t clkPin, uint8_t siPin, uint8_t analogPin);

		static volatile uint8_t pixels[ 1536 ];
		static uint8_t pixelBuffer[ 1536 ]; 

		const int L_ARRAY = 1536;
		const int L_ARRAY_1 = 1537;

};

	volatile uint8_t TSL1412::pixels[ 1536 ] = {0};
	uint8_t TSL1412::pixelBuffer[ 1536 ] = {0}; 

TSL1412::TSL1412(uint8_t clkPin, uint8_t siPin, uint8_t analogPin) {
	TSLXX TSL;
	TSL.init(clkPin, siPin, analogPin);
	TSL.pixelBuffer = pixelBuffer;
	TSL.pixels = pixels;
	TSL._L_ARRAY = L_ARRAY;
    TSL._L_ARRAY_1 = L_ARRAY_1;
}

#endif // TSL1412_H