#include "Entropy/Entropy.h"
#include "Entropy/Entropy.cpp"

void setup()
{
  Serial.begin(9600);

  Entropy.initialize();
}

void loop()
{
  Serial.println( Entropy.randomByte() );
}
