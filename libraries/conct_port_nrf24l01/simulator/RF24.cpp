#pragma once

#include "RF24.h"

namespace conct
{
	bool RF24::begin( int cePin, int csPin )
	{
		return true;
	}

	void RF24::startListening()
	{
		m_mode = Mode_Read;
	}

	void RF24::stopListening()
	{
		m_mode = Mode_Write;
	}

	bool RF24::available()
	{
		uint8 pipeIndex;
		return available( &pipeIndex );
	}

	void RF24::read( void* pBuffer, uint8 bufferLength )
	{
		if( m_mode != Mode_Read )
		{
			return;
		}

		SimulatorRf24Context& context = getSimulatorContext().getRf24Context();
		for( uintreg i = 0u; i < m_readAddresses.getLength(); ++i )
		{
			const uint64 address = m_readAddresses[ i ];
			if( address == 0u || !context.available( address ) )
			{
				continue;
			}

			context.read( address, pBuffer, bufferLength );
			return;
		}
	}

	void RF24::write( const void* pData, uint8 dataLength )
	{
		if( m_mode != Mode_Write )
		{
			return;
		}

		getSimulatorContext().getRf24Context().write( m_writeAddress, pData, dataLength );
	}

	void RF24::openWritingPipe( const uint8_t* pAddress )
	{
		m_mode			= Mode_Write;
		m_writeAddress	= getIntAddress( pAddress );
	}

	void RF24::openReadingPipe( uint8 pipeIndex, const uint8* pAddress )
	{
		m_readAddresses[ pipeIndex ] = getIntAddress( pAddress );
	}

	void RF24::closeReadingPipe( uint8 pipeIndex )
	{
		m_readAddresses[ pipeIndex ] = 0u;
	}

	bool RF24::available( uint8* pPipeIndex )
	{
		if( m_mode != Mode_Read )
		{
			return false;
		}

		SimulatorRf24Context& context = getSimulatorContext().getRf24Context();
		for( uintreg i = 0u; i < m_readAddresses.getLength(); ++i )
		{
			if( m_readAddresses[ i ] == 0u )
			{
				continue;
			}

			if( context.available( m_readAddresses[ i ] ) )
			{
				*pPipeIndex = uint8( i );
				return true;
			}
		}

		return false;
	}

	uint64 RF24::getIntAddress( const uint8* pAddress )
	{
		uint64 address = 0u;
		uint8* pTarget = (uint8*)&address;

		uintreg i = 0u;
		while( pAddress[ i ] != '\0' && i < sizeof( uint64 ) )
		{
			pTarget[ i ] = pAddress[ i ];
			i++;
		}

		return address;
	}
}
