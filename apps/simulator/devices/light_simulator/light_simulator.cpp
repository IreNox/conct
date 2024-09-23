#include "light_simulator.h"

#include <tiki/tiki_string_tools.h>

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
		targetData.name			= "Light"_s;
		targetData.pRouter		= nullptr;
		targetData.pController	= nullptr;

		const ConstArrayView< Instance > instances = m_device.getInstances();

		targetData.instances.clear();
		for( uintreg i = 0u; i < instances.getLength(); ++i )
		{
			const Instance& instance = instances[ i ];

			SimulatorInstance targetInstance;
			targetInstance.id	= instance.id;
			targetInstance.type	= instance.type;

			const LocalInstance* pLocalInstance = m_device.getInstance( instance.id );
			if( pLocalInstance != nullptr )
			{
				switch( targetInstance.type )
				{
				case Light::s_typeCrc:
					{
						Light* pLight = static_cast< Light* >( pLocalInstance->pInstance );

						DynamicString data;
						data += "Light\n";
						data += "On: "_s + DynamicString( pLight->getOn() ? "true" : "false" ) + "\n";

						targetInstance.data = data;
					}
					break;

				//case Dimmer::s_typeCrc:
				//	{
				//		Dimmer* pDimmer = static_cast< Dimmer* >( pLocalInstance->pInstance );

				//		DynamicString data;
				//		data += "Dimmer\n";
				//		data += "Brightness: "_s + string_tools::toString( ( pDimmer->getBrightness() / 65535.0f ) * 100.0f ) + "%\n";

				//		targetInstance.data = data;
				//	}
				//	break;

				//case RGB::s_typeCrc:
				//	{
				//		RGB* pRGB = static_cast< RGB* >( pLocalInstance->pInstance );

				//		DynamicString data;
				//		data += "RGB\n";
				//		data += "Red:   "_s + string_tools::toString( ( pRGB->getRed() / 65535.0f ) * 100.0f ) + "%\n";
				//		data += "Green: "_s + string_tools::toString( ( pRGB->getGreen() / 65535.0f ) * 100.0f ) + "%\n";
				//		data += "Blue:  "_s + string_tools::toString( ( pRGB->getBlue() / 65535.0f ) * 100.0f ) + "%\n";

				//		targetInstance.data = data;
				//	}
				//	break;

				default:
					continue;
				}
			}

			targetData.instances.pushBack( targetInstance );
		}
	}
}

extern "C"
{
	__declspec( dllexport ) conct::ISimulatorDevice* createSimulator()
	{
		return new conct::LightSimulator();
	}
}
