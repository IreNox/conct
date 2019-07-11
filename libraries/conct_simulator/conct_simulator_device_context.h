#pragma once

#include "conct_atomic.h"
#include "conct_mutex.h"
#include "conct_queue.h"

namespace conct
{
	class SimulatorDeviceContext
	{
	public:

							SimulatorDeviceContext( uintreg deviceId );

		uintreg				getDeviceId() const;

		bool				getGpio( uintreg pin ) const;
		void				setGpio( uintreg pin, bool value );

		int					getAvailableSerialData() const;
		uintreg				readSerialData( void* pBuffer, uintreg bufferLength );
		void				writeSerialData( uint8 byte );

	private:

		uintreg				m_deviceId;

		Atomic< uint32 >	m_gpio;

		Mutex				m_serialMutex;
		Queue< uint8 >		m_serialData;
	};
}