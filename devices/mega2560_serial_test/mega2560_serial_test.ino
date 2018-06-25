
void setup()
{
  pinMode( 22, OUTPUT );
  digitalWrite( 22, LOW );

  Serial.begin( 9600 );
  Serial1.begin( 115200 );

  Serial.write( "Start!\n" );
}

void loop()
{
  if( Serial.available() > 0 )
  {
    digitalWrite( 22, HIGH );

    while( Serial.available() > 0 )
    {
      Serial1.write( Serial.read() );
    }

    /*while (!(UCSR0A & (1 << UDRE0)))  // Wait for empty transmit buffer
    UCSR0A |= 1 << TXC0;  // mark transmission not complete
    while (!(UCSR0A & (1 << TXC0)));   // Wait for the transmission to complete*/

    delayMicroseconds( 660 );
    digitalWrite( 22, LOW );
  }
  else
  {
    while( Serial1.available() > 0 )
    {
      Serial.write( Serial1.read() );
    }
  }
}
