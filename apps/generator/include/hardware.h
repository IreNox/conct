#pragma once

#include "conct_array_view.h"
#include "conct_dynamic_string.h"
#include "conct_pair.h"

#include "capabilities.h"

namespace conct
{
	class HardwareCollection;
	class Path;

	enum HardwareRuntime
	{
		HardwareRuntime_Low,
		HardwareRuntime_High,

		HardwareRuntime_Count
	};

	enum HardwareSystem
	{
		HardwareSystem_Android,
		HardwareSystem_Arduino,
		HardwareSystem_Esp,
		HardwareSystem_Linux,
		HardwareSystem_Windows,

		HardwareSystem_Count
	};

	enum HardwareEnvironment
	{
		HardwareEnvironment_Production,
		HardwareEnvironment_Simulator,
		HardwareEnvironment_Tests,

		HardwareEnvironment_Count
	};

	class Hardware
	{
		friend class HardwareCollection;

	public:

		const DynamicString&	getFileName() const { return m_fileName; }
		const DynamicString&	getName() const { return m_name; }

		HardwareRuntime			getRuntime() const { return m_runtime; }
		HardwareSystem			getSystem() const { return m_system; }
		HardwareEnvironment		getEnvironment() const { return m_environment; }

		const size_t			getProgramDataSize() const { return m_programDataSize; }
		const size_t			getRamSize() const { return m_ramSize; }
		const size_t			getFlashSize() const { return m_flashSize; }

		const Capabilities&		getCapabilities() const { return m_capabilities; }

	private: // friend

								Hardware();

		bool					load( const Path& fileName );

	private:

		DynamicString			m_fileName;
		DynamicString			m_name;

		HardwareRuntime			m_runtime;
		HardwareSystem			m_system;
		HardwareEnvironment		m_environment;

		uintptr					m_programDataSize;
		uintptr					m_ramSize;
		uintptr					m_flashSize;

		Capabilities			m_capabilities;
	};

	const ArrayView< Pair< HardwareRuntime, const char* > >&		getHardwareRuntimeMapping();
	const ArrayView< Pair< HardwareSystem, const char* > >&			getHardwareSystemMapping();
	const ArrayView< Pair< HardwareEnvironment, const char* > >&	getHardwareEnvironmentMapping();
}