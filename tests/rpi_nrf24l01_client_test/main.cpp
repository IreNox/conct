#include "conct_port_nrf24l01_client.h"

#include "conct_writer.h"
#include "conct_string_tools.h"
#include "conct_trace.h"

namespace conct
{
	void run()
	{
		PortNRF24L01ClientParameters parameters;
		parameters.cePin = 22;
		parameters.csPin = 0;

		PortNRF24L01Client port;
		if( !port.setup( parameters ) )
		{
			return;
		}

		uint8 lastByte = 0u;
		while( true )
		{
			port.loop();

			ArrayView< uintreg > endpoints;
			port.getEndpoints( endpoints );

			for( uint i = 0u; i < endpoints.getLength(); ++i )
			{
				Writer writer;
				if( !port.openSend( writer, 28u, endpoints[ i ] ) )
				{
					continue;
				}

				while( !writer.isEnd() )
				{
					const uint8 nextByte = lastByte + 1u;
					if( !writer.writeByte( nextByte ) )
					{
						trace::write( "Failed to write byte. Remaining: "_s + string_tools::toString( writer.getRemainingSize() ) );
					}

					lastByte = nextByte;
				}

				port.closeSend( writer, endpoints[ i ] );
			}

			delayMicroseconds( 5000 );
		}
	}
}

int main(int argc, char *argv[])
{
	conct::run();
	return 0;
}