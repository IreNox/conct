#include "simulator.h"

#include "conct_timer.h"

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
	struct DevicePrototype
	{
		HMODULE				dllHandle;
		SimulatorDevice*	pDevice;
	};

	int Simulator::run()
	{
		if( !loadDevices() )
		{
			return 1u;
		}

		setupDevices();
		startDevices();

		Timer timer;
		while( true )
		{
			timer.reset();

			m_console.update();

			const double elapstedMs = timer.getElapsedSeconds() * 1000.0;
			if( elapstedMs < 1000.0 / 60.0 )
			{
				std::this_thread::sleep_for( std::chrono::duration< double, std::milli >( ( 1000.0 / 60.0 ) - elapstedMs ) );
			}
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

		std::vector< DevicePrototype > devices;
		std::filesystem::directory_iterator dir( programPath );
		for( const std::filesystem::directory_entry& entry : dir )
		{
			if( entry.path().extension() != ".dll" )
			{
				continue;
			}

			DevicePrototype device;
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

			devices.push_back( device );
		}

		if( devices.empty() )
		{
			std::cout << "Error: no devices found to simulate." << std::endl;
			return false;
		}

		Device* pDevices = new Device[ devices.size() ];
		m_devices.set( pDevices, devices.size() );

		for( size_t i = 0u; i < devices.size(); ++i )
		{
			const DevicePrototype& prototype = devices[ i ];
			Device& device = m_devices[ i ];

			device.dllHandle	= prototype.dllHandle;
			device.pDevice		= prototype.pDevice;
		}

		return true;
	}

	void Simulator::setupDevices()
	{
		for( Device& device : m_devices )
		{
			device.pDevice->setup();
			m_console.addDevice( device.pDevice );
		}
	}

	void Simulator::startDevices()
	{
		for( Device& device : m_devices )
		{
			device.running = true;
			device.thread = std::thread( &Simulator::runDevice, &device );
		}
	}

	void Simulator::freeDevices()
	{
		for( Device& device : m_devices )
		{
			m_console.removeDevice( device.pDevice );
			device.running = false;
		}

		for( Device& device : m_devices )
		{
			device.thread.join();

			delete device.pDevice;
			FreeLibrary( device.dllHandle );
		}

		delete[] m_devices.getData();
		m_devices.clear();
	}

	void Simulator::runDevice( Device* pDevice )
	{
		Timer timer;
		while( pDevice->running )
		{
			timer.reset();

			pDevice->pDevice->loop();

			const double elapstedMs = timer.getElapsedSeconds() * 1000.0;
			if( elapstedMs < 1000.0 / 60.0 )
			{
				std::this_thread::sleep_for( std::chrono::duration< double, std::milli >( ( 1000.0 / 60.0 ) - elapstedMs ) );
			}
		}
	}

	SIMULATOR_DLL SimulatorContext& getSimulatorContext()
	{
		static SimulatorContext s_context;
		return s_context;
	}
}
