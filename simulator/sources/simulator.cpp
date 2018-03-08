#include "simulator.h"

#include "simulator_context.h"
#include "simulator_device.h"

#include <filesystem>
#include <iostream>

namespace std
{
	namespace filesystem = std::experimental::filesystem;
}

namespace conct
{
	int Simulator::run()
	{
		if( !loadDevices() )
		{
			return 1u;
		}

		setupDevices();

		while( true )
		{
			loopDevices();
		}

		freeDevices();
		return 0u;
	}

	bool Simulator::loadDevices()
	{
		char szExeFileName[ MAX_PATH ];
		GetModuleFileNameA( nullptr, szExeFileName, MAX_PATH );

		std::filesystem::path exePath = szExeFileName;
		std::filesystem::path programPath = exePath.parent_path();

		std::filesystem::directory_iterator dir( programPath );
		for( const std::filesystem::directory_entry& entry : dir )
		{
			if( entry.path().extension() != ".dll" )
			{
				continue;
			}

			Device device;
			device.dllHandle = LoadLibraryA( entry.path().generic_string().c_str() );
			if( device.dllHandle == nullptr )
			{
				std::cout << "Error: could not load library: '" << entry.path() << "'" << std::endl;
				continue;
			}

			FARPROC createSimulatorProc = GetProcAddress( device.dllHandle, "createSimulator" );
			if( createSimulatorProc == nullptr )
			{
				FreeLibrary( device.dllHandle );
				std::cout << "Error: could not get device from library: '" << entry.path() << "'" << std::endl;
				continue;
			}

			CreateSimulatorFunction pCreateSimulatorFunction = ( CreateSimulatorFunction )createSimulatorProc;
			device.pDevice = pCreateSimulatorFunction();

			if( device.pDevice == nullptr )
			{
				FreeLibrary( device.dllHandle );
				std::cout << "Error: could not create device from library: '" << entry.path() << "'" << std::endl;
				continue;
			}

			m_devices.push_back( device );
		}

		if( m_devices.empty() )
		{
			std::cout << "Error: no devices found to simulate." << std::endl;
			return false;
		}

		return true;
	}

	void Simulator::setupDevices()
	{
		for( Device& device : m_devices )
		{
			device.pDevice->setup();
		}
	}

	void Simulator::loopDevices()
	{
		for( Device& device : m_devices )
		{
			device.pDevice->loop();
		}
	}

	void Simulator::freeDevices()
	{
		for( Device& device : m_devices )
		{
			delete device.pDevice;
			FreeLibrary( device.dllHandle );
		}

		m_devices.clear();
	}

	SIMULATOR_DLL SimulatorContext& getSimulatorContext()
	{
		static SimulatorContext s_context;
		return s_context;
	}
}
