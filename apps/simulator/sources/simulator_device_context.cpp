#include "simulator_device_context.h"

namespace conct
{
	SimulatorDeviceContext::SimulatorDeviceContext( uintreg deviceId )
	{
		m_deviceId = deviceId;
		m_serialData.reserve( 1024u * 1024u );
	}

	uintreg SimulatorDeviceContext::getDeviceId() const
	{
		return m_deviceId;
	}

	bool SimulatorDeviceContext::getGpio( uintreg pin ) const
	{
		const uint32 gpioValue = m_gpio.getRelaxed();
		return ( gpioValue & ( 1u << pin ) ) != 0u;
	}

	void SimulatorDeviceContext::setGpio( uintreg pin, bool value )
	{
		if( value )
		{
			const uint32 mask = ( 1u << pin );
			m_gpio.orRelaxed( mask );
		}
		else
		{
			const uint32 mask = ~( 1u << pin );
			m_gpio.andRelaxed( mask );
		}
	}

	int SimulatorDeviceContext::getAvailableSerialData() const
	{
		MutexLock lock( m_serialMutex );
		return (int)m_serialData.getLength();
	}

	uintreg SimulatorDeviceContext::readSerialData( void* pBuffer, uintreg bufferLength )
	{
		uint8* pTarget = ( uint8* )pBuffer;
		uint8* pTargetEnd = pTarget + bufferLength;

		{
			MutexLock lock( m_serialMutex );
			while( pTarget != pTargetEnd && !m_serialData.isEmpty() )
			{
				*pTarget++ = m_serialData.getFront();
				m_serialData.popFront();
			}
		}

		return uintptr( pTarget ) - uintptr( pBuffer );
	}

	void SimulatorDeviceContext::writeSerialData( uint8 byte )
	{
		MutexLock lock( m_serialMutex );
		m_serialData.pushBack( byte );
	}
}