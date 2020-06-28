#include "simulator.h"

#include "conct_directory_iterator.h"
#include "conct_path.h"
#include "conct_timer.h"
#include "conct_trace.h"

#include "conct_simulator.h"
#include "conct_simulator_context.h"

#include <windows.h>
#include <WinSock2.h>

namespace conct
{
	struct DevicePrototype
	{
		HMODULE				dllHandle;
		ISimulatorDevice*	pDevice;
	};

	int Simulator::run()
	{
		getSimulatorContext();

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

		const Path exePath( szExeFileName );
		const Path programPath = exePath.getParent();

		Vector< DevicePrototype > devices;
		DirectoryIterator directorIterator( programPath );
		while( directorIterator.next() )
		{
			const Path entry = directorIterator.getCurrent();
			if( entry.getExtension() != ".dll" )
			{
				continue;
			}

			DevicePrototype device;
			device.dllHandle = LoadLibraryA( entry.getNativePath().toConstCharPointer() );
			if( device.dllHandle == nullptr )
			{
				trace::write( "Error: could not load library: '"_s + entry.getGenericPath() + "'\n" );
				continue;
			}

			FARPROC createSimulatorProc = GetProcAddress( device.dllHandle, "createSimulator" );
			if( createSimulatorProc == nullptr )
			{
				FreeLibrary( device.dllHandle );
				trace::write( "Error: could not get device from library: '"_s + entry.getGenericPath() + "'\n" );
				continue;
			}

			CreateSimulatorFunction pCreateSimulatorFunction = ( CreateSimulatorFunction )createSimulatorProc;
			device.pDevice = pCreateSimulatorFunction();

			if( device.pDevice == nullptr )
			{
				FreeLibrary( device.dllHandle );
				trace::write( "Error: could not create device from library: '"_s + entry.getGenericPath() + "'\n" );
				continue;
			}

			devices.insert( device, 0u );
		}

		if( devices.isEmpty() )
		{
			trace::write( "Error: no devices found to simulate.\n" );
			return false;
		}

		Device* pDevices = new Device[ devices.getLength() ];
		m_devices = Array< Device >( pDevices, devices.getLength() );

		uintreg i = 0u;
		for( const DevicePrototype& prototype : devices )
		{
			Device& device = m_devices[ i ];

			device.deviceId		= i;
			device.dllHandle	= prototype.dllHandle;
			device.pDevice		= prototype.pDevice;

			i++;
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
			device.running.setRelaxed( true );
			device.thread = std::thread( &Simulator::runDevice, &device );
		}
	}

	void Simulator::freeDevices()
	{
		for( Device& device : m_devices )
		{
			m_console.removeDevice( device.pDevice );
			device.running.setRelaxed( false );
		}

		for( Device& device : m_devices )
		{
			device.thread.join();

			delete device.pDevice;
			FreeLibrary( device.dllHandle );
		}

		delete[] m_devices.getData();
		m_devices = Array< Device >();
	}

	void Simulator::runDevice( Device* pDevice )
	{
		getSimulatorContext().registerDevice( pDevice->deviceId );

		Timer timer;
		while( pDevice->running.getRelaxed() )
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

	CONCT_SIMULATOR_DLL SimulatorContext& getSimulatorContext()
	{
		static SimulatorContext s_context;
		return s_context;
	}
}
