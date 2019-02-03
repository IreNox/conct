#include "conct_port_nrf24l01_server.h"

namespace conct
{
	void run()
	{
		PortNRF24L01ServerParameters parameters;
		parameters.cePin0 = 22;

		PortNRF24L01Server port;
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

int main( int argc, char *argv[] )
{
	conct::run();
	return 0;
}
