#include "simulator_context.h"

#include "i_simulator_port.h"

namespace conct
{
	SimulatorContext::SimulatorContext()
	{
	}

	void SimulatorContext::registerPort( ISimulatorPort* pPort )
	{
		m_ports.push_back( pPort );

		if( m_ports.size() == 2u )
		{
			m_ports[ 0u ]->setCounterpartPort( m_ports[ 1u ] );
			m_ports[ 1u ]->setCounterpartPort( m_ports[ 0u ] );
		}
	}

	SIMULATOR_DLL ISimulatorContext& getSimulatorContext()
	{
		static SimulatorContext s_context;
		return s_context;
	}
}