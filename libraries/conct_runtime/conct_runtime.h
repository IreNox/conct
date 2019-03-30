#pragma once

#include "conct_core.h"
#include "conct_relative_array.h"
#include "conct_relative_pointer.h"
#include "conct_value.h"
#include "conct_result.h"

#define CONCT_RUNTIME_USE_CRYPTO CONCT_ON

namespace conct
{
	class Proxy;

	typedef uint16 CommandId;
	static const CommandId InvalidCommandId = 0u;
	static const CommandId FirstCommandId = 1u;

	typedef RelativePointer8< const char > RelativeStringPointer;
	typedef RelativeArray8< Value > RelativeValueArray;

	struct DeviceAddress
	{
		enum
		{
			Size = 128u
		};

		DeviceId address[ Size ];
	};

	struct LocalInstance
	{
		InstanceId		id;
		void*			pInstance;
		const Proxy*	pProxy;
	};

	struct RemoteInstance
	{
		DeviceAddress	address;
		InstanceId		id;
	};

	enum DeviceStatus
	{
		DeviceStatus_Unknown,
		DeviceStatus_Blocked,
		DeviceStatus_Client,
		DeviceStatus_Controller
	};

	struct DeviceConnection
	{
		static const TypeCrc s_typeCrc = 24671u;

		DeviceStatus	status;
		DeviceId		id;
	};

#if CONCT_ENABLED( CONCT_RUNTIME_USE_CRYPTO )
	enum CryptoState
	{
		CryptoState_Unencrypted,
		CryptoState_Encrypted
	};

	struct CryptoKey
	{
		uint8	data[ 48u ];
	};

	struct CryptoCounter
	{
		uint8	data[ 8u ];
	};

	struct CryptoIV
	{
		uint8	data[ 8u ];
	};
#endif

	enum MessageType : uint8
	{
		MessageType_ErrorResponse,
		MessageType_PingRequest,
		MessageType_PingResponse,
		MessageType_GetPropertyRequest,
		MessageType_GetPropertyResponse,
		MessageType_SetPropertyRequest,
		MessageType_SetPropertyResponse,
		MessageType_CallFunctionRequest,
		MessageType_CallFunctionResponse,
		//MessageType_RegisterEvent,
		//MessageType_UnregisterEvent,
		//MessageType_CallEventHandler,
		//MessageType_CheckEventHandler,
#if CONCT_ENABLED( CONCT_RUNTIME_USE_CRYPTO )
		MessageType_CryptoHandshakeRequest,
		MessageType_CryptoHandshakeResponse,
		MessageType_CryptoAssignKeyRequest,
#endif
	};

	struct MessageBaseHeader
	{
		uint8		sourceHops;
		uint8		destinationHops;
		uint16		payloadSize;
		CommandId	commandId;
		MessageType	messageType;
		ResultId	messageResult;
#if CONCT_ENABLED( CONCT_RUNTIME_USE_CRYPTO )
		CryptoIV	cryptoIV;
#endif
	};

	struct GetInstanceRequest
	{
		TypeCrc					typeCrc;
	};

	struct GetInstanceResponse
	{
		InstanceId				instanceId;
	};

	struct FindInstancesRequest
	{
		TypeCrc					typeCrc;
	};

	struct GetPropertyRequest
	{
		InstanceId				instanceId;
		uint16					nameCrc;
	};

	struct GetPropertyResponse
	{
		Value					value;
	};

	struct SetPropertyRequest
	{
		InstanceId				instanceId;
		Value					value;
		uint16					nameCrc;
	};

	struct CallFunctionRequest
	{
		InstanceId				instanceId;
		uint16					nameCrc;
		RelativeValueArray		arguments;
	};

	struct CallFunctionResponse
	{
		Value					value;
	};

#if CONCT_ENABLED( CONCT_RUNTIME_USE_CRYPTO )
	struct CryptoHandshakeRequest
	{
		uint32					serialNumber;
	};

	struct CryptoHandshakeResponse
	{
		uint32					serialNumber;
		uint16					keyHash;
	};

	struct CryptoAssingKeyRequest
	{
		CryptoKey				key;
		CryptoCounter			counter;
	};
#endif

	namespace runtime
	{
		bool		isDeviceAddressEmpty( const DeviceAddress& address );
		bool		isDeviceAddressEquals( const DeviceAddress& lhs, const DeviceAddress& rhs );

		void		copyDeviceAddress( DeviceAddress& target, const DeviceAddress& source );
	}
}