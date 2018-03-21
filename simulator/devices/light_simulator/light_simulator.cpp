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

	void LightSimulator::fillData( SimulatorDeviceData& targetData )
	{
		targetData.pController = nullptr;

		ArrayView< LocalInstance > instances;
		m_device.getInstances( instances );

		targetData.instances.clear();
		for( uint i = 0u; i < instances.getCount(); ++i )
		{
			const LocalInstance& instance = instances[ i ];

			SimulatorInstance targetInstance;
			targetInstance.id	= instance.id;
			targetInstance.type	= instance.pProxy->getTypeCrc();

			switch( targetInstance.type )
			{
			case Light::s_TypeCrc:
				{
					Light* pLight = static_cast< Light* >( instance.pInstance );

				}
				break;

			default:
				break;
			}

			targetData.instances.push_back( targetInstance );
		}
	}
}

extern "C"
{
	__declspec( dllexport ) conct::SimulatorDevice* createSimulator()
	{
		return new conct::LightSimulator();
	}
}
