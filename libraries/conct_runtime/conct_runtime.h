#pragma once

#include "conct_core.h"
#include "conct_relative_array.h"
#include "conct_relative_pointer.h"
#include "conct_value.h"
#include "conct_result.h"

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

	enum MessageType : uint8
	{
		MessageType_ErrorResponse,
		MessageType_PingRequest,
		MessageType_PingResponse,
		MessageType_GetInstanceRequest,
		MessageType_GetInstanceResponse,
		MessageType_FindInstancesRequest,
		MessageType_FindInstancesResponse,
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
		ResultId	messageResult;
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
		RelativeStringPointer	name;
	};

	struct GetPropertyResponse
	{
		Value					value;
	};

	struct SetPropertyRequest
	{
		InstanceId				instanceId;
		Value					value;
		RelativeStringPointer	name;
	};

	struct CallFunctionRequest
	{
		InstanceId				instanceId;
		RelativeStringPointer	name;
		RelativeValueArray		arguments;
	};

	struct CallFunctionResponse
	{
		Value					value;
	};
}