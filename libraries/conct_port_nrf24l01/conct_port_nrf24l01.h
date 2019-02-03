#pragma once

#include "conct_port.h"

namespace conct
{
	class PortNRF24L01 : public Port
	{
	protected:

		static constexpr uintreg PipesPerRadio		= 6u;
		static constexpr uint8 MaxPacketPayloadSize	= 28u;
		static constexpr uint32 PacketResendTime	= 100u;
		static constexpr uint16 PacketMagic			= 0xe670u;

		enum PacketType : uint8
		{
			PacketType_Protocol,
			PacketType_Data,

			PacketType_Count
		};

		enum ProtocolMessageType : uint8
		{
			ProtocolMessageType_Request,
			ProtocolMessageType_Address,
			ProtocolMessageType_Depleted,
			ProtocolMessageType_Ack,
			ProtocolMessageType_Reset
		};

		typedef uint8 Header[ 5u ];
		typedef uint8 Buffer[ 32u ];

		union Address
		{
			uint8				data[ 5u ];
			struct
			{
				uint8			pipeIndex;
				uint8			radioIndex;
				uint8			static1;
				uint8			static2;
				uint8			static3;
			};
		};

		struct ProtocolMessageHeader
		{
			ProtocolMessageType	messageType;
		};

		struct RequestProtocolMessageData
		{
			uint32				requestId;
		};

		struct AddressProtocolMessageData
		{
			uint32				requestId;
			Address				address;
		};

		struct DepletedProtocolMessageData
		{
			uint32				requestId;
		};

		struct AcknowledgeProtocolMessageData
		{
			uint8				packetId;
		};

		void					getAddressForPipe( Address& targetAddress, uintreg radioIndex, uintreg pipeIndex ) const;

		uint16					getMagicFromHeader( const Buffer& header ) const;
		uint8					getSizeFromHeader( const Buffer& header ) const;
		PacketType				getTypeFromHeader( const Buffer& header ) const;
		uint8					getIdFromHeader( const Buffer& header ) const;

		uint8*					writeHeader( Buffer& buffer, uint8 size, PacketType type, uint8 id );
		uint8*					writeProtocolHeader( Buffer& buffer, uint8& payloadSize, ProtocolMessageType type, uint8 size );

		uint8					finalizePacket( Buffer& buffer, uint8 payloadSize );

		uint8					calculateChecksum( const Buffer& buffer, uint8 payloadSize ) const;
	};
}