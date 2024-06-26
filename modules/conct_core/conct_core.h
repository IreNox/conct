#pragma once

#include <tiki/tiki_types.h>

using namespace tiki;

namespace conct
{
	using DeviceId		= uint8;
	using TypeCrc		= uint16;
	using InstanceId	= uint16;
	using PercentValue	= uint16;

	struct Instance
	{
		InstanceId	id;
		TypeCrc		type;
	};

	struct Decimal
	{
		sint32		number;
	};

	static const DeviceId InvalidDeviceId = 0u;
	static const DeviceId FirstDeviceId = 1u;
	static const TypeCrc InvalidTypeCrc = 0u;
	static const InstanceId InvalidInstanceId = 0xffffu;
	static const Instance InvalidInstance = { InvalidInstanceId, InvalidTypeCrc };
	static const PercentValue PercentValueFull = 0xffffu;
}
