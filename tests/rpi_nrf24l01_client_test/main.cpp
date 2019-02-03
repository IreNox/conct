#include "conct_port_nrf24l01_client.h"

namespace conct
{
	void run()
	{
		PortNRF24L01ClientParameters parameters;
		parameters.cePin = 22;

		PortNRF24L01Client port;
		if( !port.setup( parameters ) )
		{
			return;
		}

		while( true )
		{
			port.loop();
		}
	}
}

int main(int argc, char *argv[])
{
	conct::run();
	return 0;
}