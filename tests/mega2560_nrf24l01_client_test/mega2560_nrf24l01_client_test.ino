#include "conct_port_nrf24l01_client.h"

#include "conct_reader.h"
#include "conct_writer.h"

namespace conct
{
  PortNRF24L01Client port;
  uint8 lastSendByte = 0u;
  uint8 lastReceiveByte = 0u;
  uint32 bytes = 0;

  void setup()
  {
    PortNRF24L01ClientParameters parameters;
    parameters.cePin = 7;
    parameters.csPin = 8;

    port.setup( parameters );
  }

  void loop()
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
            Serial.println( "Failed to write byte." );
          }

          lastSendByte = nextByte;
        }

        port.closeSend( writer, endpoints[ i ] );
      }

      Reader reader;
      uintreg endpointId;
      while( port.openReceived( reader, endpointId ) )
      {
        while( !reader.isEnd() )
        {
          const uint8 expectedByte = lastReceiveByte + 1u;

          uint8 newByte;
          if( !reader.readByte( newByte ) )
          {
            Serial.println( "Failed to read byte." );
          }

          if( newByte != expectedByte )
          {
            Serial.print( "Received wrong byte: " );
            Serial.print( newByte );
            Serial.print( " != " );
            Serial.println( expectedByte );
          }

          lastReceiveByte = newByte;
          bytes++;
        }

        port.closeReceived( reader, endpointId );

        Serial.print( "Received: " );
        Serial.println( bytes );
      }

      delay( 5 );
  }
}

void setup()
{
  Serial.begin(9600);

  conct::setup();
}

void loop()
{
  conct::loop();
}
