#include "conct_port_nrf24l01.h"

namespace conct
{
	void PortNRF24L01::getAddressForPipe( Address& targetAddress, uintreg radioIndex, uintreg pipeIndex ) const
	{
		targetAddress.pipeIndex		= '0' + pipeIndex;
		targetAddress.radioIndex	= '0' + radioIndex;
		targetAddress.static1		= 'c';
		targetAddress.static2		= 'o';
		targetAddress.static3		= 'n';
	}

	uint16 PortNRF24L01::getMagicFromHeader( const Buffer& buffer ) const
	{
		const uint16 magic = (buffer[ 0u ] << 4u) | ((buffer[ 1u ] & 0xf0u) >> 4u);
		return (magic & 0xfff0u);
	}

	uint8 PortNRF24L01::getSizeFromHeader( const Buffer& buffer ) const
	{
		uint8 dataSize = (buffer[ 1u ] & 0x0fu) << 1u;
		dataSize |= (buffer[ 2u ] & 0x80u) >> 7u;
		return dataSize;
	}

	PortNRF24L01::PacketType PortNRF24L01::getTypeFromHeader( const Buffer& buffer ) const
	{
		return (PacketType)((buffer[ 2u ] & 0x40u) >> 6u);
	}

	uint8 PortNRF24L01::getIdFromHeader( const Buffer& buffer ) const
	{
		return buffer[ 2u ] & 0x3fu;
	}

	uint8* PortNRF24L01::writeHeader( Buffer& buffer, uint8 size, PacketType type, uint8 id )
	{
		buffer[ 0u ] = 0xe6u;
		buffer[ 1u ] = 0x70u | ((size >> 1u) & 0x0fu);
		buffer[ 2u ] = ((size & 0x01u) << 7u) | (type << 6u) | (id & 0x3fu);

		return buffer + sizeof( Header );
	}

	uint8* PortNRF24L01::writeProtocolHeader( Buffer& buffer, uint8& packetSize, ProtocolMessageType type, uint8 size )
	{
		uint8* pData = writeHeader( buffer, sizeof( ProtocolMessageHeader ) + size, PacketType_Protocol, 0u );

		ProtocolMessageHeader* pProtocolHeader = (ProtocolMessageHeader*)pData;
		pProtocolHeader->messageType = type;

		packetSize = sizeof( Header ) + sizeof( ProtocolMessageHeader ) + size;
		return pData + sizeof( ProtocolMessageHeader );
	}

	uint8 PortNRF24L01::calculateChecksum( const Buffer& buffer, uint8 size ) const
	{
		// source: https://stackoverflow.com/questions/13491700/8-bit-fletcher-checksum-of-16-byte-data
		uint32 sum1 = 1;
		uint32 sum2 = 0;

		const uint8* pData = buffer;
		const uint8* pDataEnd = pData + size;
		while( pData < pDataEnd )
		{
			sum2 += sum1 += *pData++;
		}

		sum1 %= 15u;
		sum2 %= 15u;

		// bit shift modulo 15
		//k = ( k >> 16 ) + ( k & 0xffff );
		//k = ( k >> 8 ) + ( k & 0xff );
		//k = ( k >> 4 ) + ( k & 0xf );
		//k = ( k >> 4 ) + ( k & 0xf );
		//if( k > 14 )
		//{
		//	k -= 15;
		//}

		return (sum2 << 4u) + sum1;
	}
}