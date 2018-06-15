
void setup()
{
  pinMode( 5, OUTPUT );
  digitalWrite( 5, LOW );

  Serial.begin( 9600 );
  Serial1.begin( 28800 );

  Serial.write( "Start!\n" );
}

void loop()
{
  if( Serial.available() > 0 )
  {
    digitalWrite( 5, HIGH );

    while( Serial.available() > 0 )
    {
      Serial1.write( Serial.read() );
    }

    delayMicroseconds( 660 );
    digitalWrite( 5, LOW );
  }
  else
  {
    while( Serial1.available() > 0 )
    {
      Serial.write( Serial1.read() );
    }
  }
}
