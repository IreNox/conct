#include "controller_simulator.h"

namespace conct
{
	ControllerSimulator::ControllerSimulator()
	{
	}

	ControllerSimulator::~ControllerSimulator()
	{
	}

	void ControllerSimulator::setup()
	{
		m_device.setupDevice();
		m_controller.setup( m_device.getRuntime() );
	}

	void ControllerSimulator::loop()
	{
		m_device.loopDevice();
	}

	void ControllerSimulator::fillData( SimulatorDeviceData& targetData )
	{
		targetData.name			= "Controller"_s;
		targetData.pRouter		= &m_device.getRouter();
		targetData.pController	= &m_controller;
	}
}

extern "C"
{
	__declspec( dllexport ) conct::ISimulatorDevice* createSimulator()
	{
		return new conct::ControllerSimulator();
	}
}
