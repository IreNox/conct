#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio;

const byte address[6] = "a0con";

void setup()
{
  Serial.begin(9600);
  radio.begin(7, 8);  // CE, CSN
  radio.openReadingPipe(1, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

void loop()
{
  if ( radio.available() )
  {
    char text[32] = "";
    radio.read(&text, sizeof(text));
    Serial.println(text);
  }
}
