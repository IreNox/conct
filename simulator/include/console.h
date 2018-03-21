#pragma once

namespace conct
{
	class SimulatorDevice;

	class Console
	{
	public:

		void	addDevice( SimulatorDevice* pDevice );
		void	removeDevice( SimulatorDevice* pDevice );

	private:

		struct Device
		{
			SimulatorDevice*	pDevice;
			SimulatorDeviceData	data;
		};

		std::vector< Device >	m_devices;

	};
}