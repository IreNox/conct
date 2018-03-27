#include "light_simulator.h"

#include <sstream>

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
		targetData.name			= "Light";
		targetData.pController	= nullptr;

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
			case Light::s_typeCrc:
				{
					Light* pLight = static_cast< Light* >( instance.pInstance );

					std::ostringstream data;
					data << "Light" << std::endl;
					data << "On: " << ( pLight->getOn() ? "true" : "false" ) << std::endl;

					targetInstance.data = data.str();
				}
				break;

			case Dimmer::s_typeCrc:
				{
					Dimmer* pDimmer = static_cast< Dimmer* >( instance.pInstance );

					std::ostringstream data;
					data << "Dimmer" << std::endl;
					data << "Brightness: " << ( ( pDimmer->getBrightness() / 65536.0f ) * 100.0f ) << "%" << std::endl;

					targetInstance.data = data.str();
				}
				break;

			case RGB::s_typeCrc:
				{
					RGB* pRGB = static_cast< RGB* >( instance.pInstance );

					std::ostringstream data;
					data << "RGB" << std::endl;
					data << "Red:   " << ( ( pRGB->getRed() / 65536.0f ) * 100.0f ) << "%" << std::endl;
					data << "Green: " << ( ( pRGB->getGreen() / 65536.0f ) * 100.0f ) << "%" << std::endl;
					data << "Blue:  " << ( ( pRGB->getBlue() / 65536.0f ) * 100.0f ) << "%" << std::endl;

					targetInstance.data = data.str();
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
