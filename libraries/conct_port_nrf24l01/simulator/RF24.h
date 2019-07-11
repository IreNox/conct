#pragma once

#include "conct_core.h"
#include "conct_static_array.h"

#include "conct_simulator_arduino_interface.h"

namespace conct
{
	typedef enum { RF24_PA_MIN = 0, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX, RF24_PA_ERROR } rf24_pa_dbm_e;

	class RF24
	{
	public:

		bool begin( int cePin, int csPin );

		void startListening();
		void stopListening();

		bool available();

		void read( void* pBuffer, uint8 bufferLength );
		void write( const void* pData, uint8 dataLength );

		void openWritingPipe( const uint8_t* pAddress );

		void openReadingPipe( uint8 pipeIndex, const uint8* pAddress );
		void closeReadingPipe( uint8 pipeIndex );

		bool available( uint8* pPipeIndex );

		bool isValid() const { return true; }

		void setPALevel( uint8 level ) { }

	private:

		enum Mode
		{
			Mode_Read,
			Mode_Write
		};

		using ReadAddressArray = StaticArray< uint64, 6u >;

		Mode				m_mode;

		uint64				m_writeAddress;
		ReadAddressArray	m_readAddresses;

		uint64				getIntAddress( const uint8* pAddress );
	};
}