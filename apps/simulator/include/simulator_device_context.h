#pragma once

#include "conct_atomic.h"
#include "conct_mutex.h"
#include "conct_queue.h"

#define CONCT_IS_DLL CONCT_OFF
#include "i_simulator_device_context.h"

namespace conct
{
	class SimulatorDeviceContext : public ISimulatorDeviceContext
	{
	public:
		
						SimulatorDeviceContext( uintreg deviceId );

		virtual uintreg	getDeviceId() const CONCT_OVERRIDE_FINAL;

		virtual bool	getGpio( uintreg pin ) const CONCT_OVERRIDE_FINAL;
		virtual void	setGpio( uintreg pin, bool value ) CONCT_OVERRIDE_FINAL;

		virtual int		getAvailableSerialData() const CONCT_OVERRIDE_FINAL;
		virtual uintreg	readSerialData( void* pBuffer, uintreg bufferLength ) CONCT_OVERRIDE_FINAL;
		virtual void	writeSerialData( uint8 byte ) CONCT_OVERRIDE_FINAL;

	private:

		uintreg				m_deviceId;

		Atomic< uint32 >	m_gpio;

		Mutex				m_serialMutex;
		Queue< uint8 >		m_serialData;
	};
}