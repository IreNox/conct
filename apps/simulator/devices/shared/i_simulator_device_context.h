#pragma once

#include "conct_core.h"

namespace conct
{
	class ISimulatorDeviceContext
	{
	public:

		virtual uintreg	getDeviceId() const = 0;

		virtual bool	getGpio( uintreg pin ) const = 0;
		virtual void	setGpio( uintreg pin, bool value ) = 0;

		virtual int		getAvailableSerialData() const = 0;
		virtual uintreg	readSerialData( void* pBuffer, uintreg bufferLength ) = 0;
		virtual void	writeSerialData( uint8 byte ) = 0;
	};
}