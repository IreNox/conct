#include "console_instances.h"

#include "console_render.h"

namespace conct
{
	void ConsoleInstances::activate( ConsoleDevice& device )
	{

	}

	void ConsoleInstances::deactivate( ConsoleDevice& device )
	{

	}

	void ConsoleInstances::update( ConsoleDevice& device )
	{

	}

	void ConsoleInstances::draw( const ConsoleDevice& device ) const
	{
		uint16 x = 0u;
		uint16 y = 6u;
		for( const SimulatorInstance& instance : device.data.instances )
		{
			const uint16x2 size = ConsoleRenderer::drawTextMultiline( x + 2u, y + 1u, instance.data.toConstCharPointer() );
			ConsoleRenderer::drawRectangle( x, y, size.x + 4u, size.y + 2u, LineType_Single );

			x += size.x + 4u;
		}
	}

	const char* ConsoleInstances::getName() const
	{
		return "Instances";
	}
}