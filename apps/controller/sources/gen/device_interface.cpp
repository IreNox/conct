#include "device_interface.h"

namespace conct
{
	DeviceInterface::DeviceInterface()
	{
	}

	void DeviceInterface::setupDevice()
	{

		m_runtime.setup( this );

		setup();
	}

	void DeviceInterface::loopDevice()
	{


		loop();
	}

	const char* DeviceInterface::getName() const
	{
		 return "Controller";
	}

	void DeviceInterface::getEmptyInstances( ArrayView< Instance >& instances )
	{
		static Instance s_instances[ 1u ];
		instances = ArrayView< Instance >( s_instances, TIKI_ARRAY_COUNT( s_instances ) );
	}

	void DeviceInterface::getPublicInstances( ConstInstanceView& instances ) const
	{
		static const Instance s_instances[] =
		{
			{ 0, 32636 },
		};

		instances.set( s_instances, TIKI_ARRAY_COUNT( s_instances ) );
	}

	void DeviceInterface::getLocalInstances( ArrayView< const LocalInstance >& instances )
	{
		static const LocalInstance s_instances[] =
		{
			{ 0, this, &m_proxyDevice },
		};

		instances.set( s_instances, TIKI_ARRAY_COUNT( s_instances ) );
	}
}
