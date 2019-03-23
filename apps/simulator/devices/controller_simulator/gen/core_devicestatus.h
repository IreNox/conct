#pragma once

namespace conct
{
	static constexpr TypeCrc DeviceStatusTypeCrc = 52634;

	enum DeviceStatus
	{
		DeviceStatus_Unknown,
		DeviceStatus_Blocked,
		DeviceStatus_Client,
		DeviceStatus_Controller,
	};
}
