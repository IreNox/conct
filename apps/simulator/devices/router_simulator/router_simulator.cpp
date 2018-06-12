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
	}

	void RouterSimulator::loop()
	{
		m_device.loopDevice();
	}

	void RouterSimulator::fillData( SimulatorDeviceData& targetData )
	{
		targetData.name			= "Router";
		targetData.pController	= nullptr;
	}
}

extern "C"
{
	__declspec( dllexport ) conct::SimulatorDevice* createSimulator()
	{
		return new conct::RouterSimulator();
	}
}
