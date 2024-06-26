#pragma once

#include "capabilities.h"

namespace conct
{
	class HardwareCollection;

	enum class HardwareSystem : uint8
	{
		Computer,
		Android,
		Esp,

		Count
	};

	enum class HardwareEnvironment : uint8
	{
		Production,
		Simulator,
		Tests,

		Count
	};

	class Hardware
	{
		friend class HardwareCollection;

	public:

		const DynamicString&	getFileName() const { return m_fileName; }
		const DynamicString&	getName() const { return m_name; }

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

		HardwareSystem			m_system;
		HardwareEnvironment		m_environment;

		uintptr					m_programDataSize;
		uintptr					m_ramSize;
		uintptr					m_flashSize;

		Capabilities			m_capabilities;
	};

	const ArrayView< const Pair< HardwareSystem, const char* > >&		getHardwareSystemMapping();
	const ArrayView< const Pair< HardwareEnvironment, const char* > >&	getHardwareEnvironmentMapping();
}