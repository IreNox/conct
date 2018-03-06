#pragma once

#include "conct_core.h"
#include "conct_value.h"

namespace conct
{
	typedef uint16_t RuntimeRequestId;

	enum RuntimeErrorCode : uint8_t
	{
		RuntimeErrorCode_Unknown,
		RuntimeErrorCode_OutOfMemory,
		RuntimeErrorCode_Unsupported,
		RuntimeErrorCode_NoSuchInstance,
		RuntimeErrorCode_NoSuchField,

		RuntimeErrorCode_Count
	};

	enum RuntimeMessageType : uint8_t
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
		uint16_t	payloadSize;
		uint8_t		sourceHops;
		uint8_t		destinationHops;
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
}