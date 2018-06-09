#pragma once

#define CONCT_IS_DLL CONCT_OFF
#include "i_simulator_context.h"

#include <vector>

namespace conct
{
	class SimulatorContext : public ISimulatorContext
	{
	public:

						SimulatorContext();

		virtual void	registerPort( ISimulatorPort* pPort ) CONCT_OVERRIDE_FINAL;

	private:

		typedef std::vector< ISimulatorPort* > PortVector;

		PortVector		m_ports;

	};
}