#include "conct_port_nrf24l01_server.h"

#include "conct_reader.h"
#include "conct_string_tools.h"
#include "conct_trace.h"
#include "conct_writer.h"

#include <unistd.h>

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

		uint32 bytes = 0u;
		uint32 startTime = millis();
		uint32 lastSecond = millis() / 1000;

		bool send = false;
		uint8 lastSendByte = 0u;
		uint8 lastReceiveByte = 0u;
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
					const uint8 nextByte = lastSendByte + 1u;
					if( !writer.writeByte( nextByte ) )
					{
						trace::write( "Failed to write byte." );
					}

					lastSendByte = nextByte;
				}

				port.closeSend( writer, endpoints[ i ] );
				send = false;
			}

			Reader reader;
			uintreg endpointId;
			while( port.openReceived( reader, endpointId ) )
			{
				//trace::write( "Received!" );

				while( !reader.isEnd() )
				{
					const uint8 expectedByte = lastReceiveByte + 1u;

					uint8 newByte;
					if( !reader.readByte( newByte ) )
					{
						trace::write( "Failed to read byte. Remaining: "_s + string_tools::toString( reader.getRemainingSize() ) );
					}

					if( newByte != expectedByte )
					{
						trace::write( "Received wrong byte '"_s + string_tools::toString( newByte ) + "' expected '" + string_tools::toString( expectedByte ) + "'." );
					}

					lastReceiveByte = newByte;
					bytes++;
				}

				port.closeReceived( reader, endpointId );
				send = true;
			}

			const uint32 newTime = millis();
			const uint32 newSecond = newTime / 1000;
			if( newSecond != lastSecond )
			{
				const float time = (float( newTime ) - startTime) / 1000;
				const float bandwidth = float( bytes ) / time;
				trace::write( "Bandwidth: "_s + string_tools::toString( bandwidth ) + " bps." );

				lastSecond = newSecond;
			}

			delayMicroseconds( 5000 );
		}
	}
}

int main( int argc, char *argv[] )
{
	conct::run();
	return 0;
}
