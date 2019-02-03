#include <RF24.h>
#include <unistd.h>

const uint8_t pipeName[ 6 ] = "00001";

int main(int argc, char *argv[])
{
	RF24 radio;
	radio.begin( RPI_GPIO_P1_15, BCM2835_SPI_CS0 );

	// optionally, increase the delay between retries & # of retries
	radio.setRetries( 15, 15 );
	// Dump the configuration of the rf unit for debugging
	radio.printDetails();

	radio.openWritingPipe( pipeName );

	while( true )
	{
		const char* pTest = "Hello World";
		radio.write( pTest, strlen( pTest ) );
		sleep( 1 );
	}

	return 0;
}