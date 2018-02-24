/* PWM Outputs */

const uint8_t Red = 9;
const uint8_t Green = 10;
const uint8_t Blue = 11;

static const uint8_t Pins[] = { Red, Green, Blue };

void setup()
{
	pinMode(Red, OUTPUT);
	pinMode(Green, OUTPUT);
	pinMode(Blue, OUTPUT);

	analogWrite( Red, 0 );
	analogWrite( Green, 0 );
	analogWrite( Blue, 0 );
}

// Sets colour Red=0 Green=1 Blue=2 to specified intensity 0 (off) to 255 (max)
void SetColour (int colour, int intensity)
{
	analogWrite( Pins[colour], 255 - intensity );
}

void loop()
{
    /*for (int i=0; i <= 255; i++)
    {
      SetColour(0, i );
      delay(25);
    }*/
	
  /*for (int colour=0; colour < 3; colour++)
  {
    for (int i=0; i <= 255; i++)
    {
      SetColour(colour, i );
      delay(25);
    }
    
    SetColour( colour, 255 );
  }*/
}
