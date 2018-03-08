#pragma once

#include "conct_core.h"
#include "conct_value.h"

namespace conct
{
	typedef uint16 RuntimeMessageId;

	enum RuntimeResult : uint8
	{
		RuntimeResult_Unknown,

		RuntimeResult_Success,

		RuntimeResult_OutOfMemory,
		RuntimeResult_Unsupported,
		RuntimeResult_NoSuchInstance,
		RuntimeResult_NoSuchField,

		RuntimeResult_Count
	};

	enum RuntimeMessageType : uint8
	{
		RuntimeMessageType_ErrorResponse,
		RuntimeMessageType_PingRequest,
		RuntimeMessageType_PingResponse,
		RuntimeMessageType_GetPropertyRequest,
		RuntimeMessageType_GetPropertyResponse,
		RuntimeMessageType_SetPropertyRequest,
		RuntimeMessageType_SetPropertyResponse,
		RuntimeMessageType_CallFunctionRequest,
		RuntimeMessageType_CallFunctionResponse,
		//RuntimeMessageType_RegisterEvent,
		//RuntimeMessageType_UnregisterEvent,
		//RuntimeMessageType_CallEventHandler,
		//RuntimeMessageType_CheckEventHandler,
	};

	struct RuntimeMessageBaseHeader
	{
		uint8_t				sourceHops;
		uint8_t				destinationHops;
		uint16_t			payloadSize;
		RuntimeMessageId	messageId;
		RuntimeMessageType	messageType;
		RuntimeResult		messageResult;
	};

	struct RuntimeGetPropertyRequest
	{
		InstanceId		instanceId;
		char			name[ 0u ];
	};

	struct RuntimeGetPropertyResponse
	{
		Value			value;
	};

	static const uint16 s_runtimeMagic = 0xc0c7u;
}