#include "light_simulator.h"

namespace conct
{
	LightSimulator::LightSimulator()
	{
	}

	LightSimulator::~LightSimulator()
	{
	}

	void LightSimulator::setup()
	{
		m_device.setupDevice();
	}

	void LightSimulator::loop()
	{
		m_device.loopDevice();
	}
}

extern "C"
{
	__declspec( dllexport ) conct::SimulatorDevice* createSimulator()
	{
		return new conct::LightSimulator();
	}
}
