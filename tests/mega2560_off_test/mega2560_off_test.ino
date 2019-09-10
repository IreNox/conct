/* PWM Outputs */

static const uint8_t Red = 3;
static const uint8_t Green = 5;
static const uint8_t Blue = 6;

static const uint8_t Pins[] = { Red, Green, Blue };

void setup()
{
	pinMode(Red, OUTPUT);
	pinMode(Green, OUTPUT);
	pinMode(Blue, OUTPUT);

	analogWrite( Red, 255 );
	analogWrite( Green, 0 );
	analogWrite( Blue, 0 );
}

// Sets colour Red=0 Green=1 Blue=2 to specified intensity 0 (off) to 255 (max)
void SetColour (int colour, int intensity)
{
	analogWrite( Pins[colour], intensity );
}

void loop()
{
    /*for (int i=0; i <= 255; i++)
    {
      SetColour(0, i );
      delay(25 );
    }*/

  //TCCR1B |= 0x04;
  //TCCR1B &= 0xfc;

  /*for (int colour=0; colour < 3; colour++)
  {
    for (int i=0; i <= 255; i++)
    {
      SetColour(colour, 255 - i );
      SetColour((colour + 1) % 3, i );
      delay(10);
    }
  }*/
}
