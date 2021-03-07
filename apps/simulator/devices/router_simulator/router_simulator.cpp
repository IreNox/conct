#include "router_simulator.h"

namespace conct
{
	RouterSimulator::RouterSimulator()
	{
	}

	RouterSimulator::~RouterSimulator()
	{
	}

	void RouterSimulator::setup()
	{
		m_device.setupDevice();
		m_controller.setup( m_device.getRuntime() );
	}

	void RouterSimulator::loop()
	{
		m_controller.loop();
		m_device.loopDevice();
	}

	void RouterSimulator::fillData( SimulatorDeviceData& targetData )
	{
		targetData.name			= "Router"_s;
		targetData.pRouter		= &m_device.getRouter();
		targetData.pController	= &m_controller;
	}
}

extern "C"
{
	__declspec( dllexport ) conct::ISimulatorDevice* createSimulator()
	{
		return new conct::RouterSimulator();
	}
}
