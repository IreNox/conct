#include "FastLED.h"

#define    CLOCK_PIN            4      // Clock and data pins for LED data
#define    DATA_PIN             5 

int       color[3];

CRGB rgb;

void setup() {
  // put your setup code here, to run once:
  FastLED.addLeds<P9813, DATA_PIN, CLOCK_PIN>(&rgb, 1u);

  rgb.setRGB(255, 0, 0);
  FastLED.show();
}

void loop() {
 /* rgb.setRGB(255, 0, 0);
  FastLED.show();
  delay(3000);
  rgb.setRGB(0, 255, 0);
  FastLED.show();
  delay(3000);
  rgb.setRGB(0, 0, 255);
  FastLED.show();
  delay(3000);*/

  // put your main code here, to run repeatedly:
  for (int colour=0; colour < 3; colour++)
  {
    for (int i=0; i <= 255; i++)
    {
      color[colour] = 255 - i;
      color[(colour+1)%3] = i;
      
      rgb.setRGB(color[0], color[1], color[2]);
      FastLED.show();
      delay(10);
    }
  }
}
