#pragma once

#include <tiki/tiki_types.h>

namespace conct
{
	using uint8			= tiki::uint8;
	using sint8			= tiki::sint8;
	using uint16		= tiki::uint16;
	using sint16		= tiki::sint16;
	using uint32		= tiki::uint32;
	using sint32		= tiki::sint32;
	using uint64		= tiki::uint64;
	using sint64		= tiki::sint64;

	using sint			= tiki::sint;
	using uint			= tiki::uint;
	using uintptr		= tiki::uintptr;
	using uintreg		= tiki::uintreg;

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
