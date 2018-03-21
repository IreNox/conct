#pragma once

#include "conct_core.h"
#include "conct_value.h"

namespace conct
{
	class Proxy;

	typedef uint16 CommandId;

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
		InstanceId	id;
		void*		pInstance;
		Proxy*		pProxy;
	};

	struct RemoteInstance
	{
		DeviceAddress	address;
		InstanceId		id;
	};

	enum ResultId : uint8
	{
		ResultId_Unknown,

		ResultId_Success,

		ResultId_OutOfMemory,
		ResultId_Timeout,
		ResultId_Unsupported,
		ResultId_NoSuchDevice,
		ResultId_NoSuchInstance,
		ResultId_NoSuchField,
		ResultId_NoSource,
		ResultId_NoDestination,

		ResultId_Count
	};

	struct ResultBase
	{
		ResultId		result;

		inline bool		isSuccess() const { return result == ResultId_Success; }
		inline bool		isError() const { return result != ResultId_Success; }

		inline bool		isBusy() const { return result == ResultId_Timeout; }
		inline bool		isDone() const { return result != ResultId_Timeout; }
	};

	template< class TValue >
	struct Result : public ResultBase
	{
		TValue			value;
	};

	template< >
	struct Result< void > : public ResultBase
	{
	};

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
	};

	struct MessageBaseHeader
	{
		uint8		sourceHops;
		uint8		destinationHops;
		uint16		payloadSize;
		CommandId	commandId;
		MessageType	messageType;
		ResultId		messageResult;
	};

	struct GetInstanceRequest
	{
		TypeCrc			typeCrc;
	};

	struct FindInstanceRequest
	{
		TypeCrc			typeCrc;
	};

	struct GetPropertyRequest
	{
		InstanceId		instanceId;
		char			name[ 0u ];
	};

	struct GetPropertyResponse
	{
		Value			value;
	};

	struct SetPropertyRequest
	{
		InstanceId		instanceId;
		Value			value;
		char			name[ 0u ];
	};

	struct CallFunctionRequest
	{
		InstanceId		instanceId;
		Value			value;
		char			name[ 0u ];
	};

	static const uint16 s_messageBaseHeaderMagic = 0xc0c7u;
}