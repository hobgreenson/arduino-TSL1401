#define IC_LF1401

#include <TSLXX.h>
#include <LF1401.h>

TSLXX tsl;


void setup() {
  tsl.init(11,8, A5);
  Serial.begin(230400);
}

void loop() {
  // put your main code here, to run repeatedly:
    while (1) {
        // copy volatile array into non-volatile array
        Serial.print("[");
        int pixel;
        for(int i = 0; i < L_ARRAY; i++) {
            //send_pixels[i] = pixels[i];
            //Serial.print((int)readPixels);
            pixel = tsl.p(i);
            Serial.print(pixel);
            if ((i+1) < L_ARRAY) {
            Serial.print(",");
            }
        }
        Serial.print("]");
        // send the frame
        Serial.println(""); 
    }
}
