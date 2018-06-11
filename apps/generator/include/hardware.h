#pragma once

#include "conct_dynamic_string.h"

namespace conct
{
	class HardwareCollection;
	class Path;

	enum HardwareRuntime
	{
		HardwareRuntime_Low,
		HardwareRuntime_High
	};

	class Hardware
	{
		friend class HardwareCollection;

	public:

		const DynamicString&	getFileName() const { return m_fileName; }
		const DynamicString&	getName() const { return m_name; }

		HardwareRuntime			getRuntime() const { return m_runtime; }

		const size_t			getProgramDataSize() const { return m_programDataSize; }
		const size_t			getRamSize() const { return m_ramSize; }
		const size_t			getFlashSize() const { return m_flashSize; }

	private: // friend

								Hardware();

		bool					load( const Path& fileName );

	private:

		DynamicString			m_fileName;
		DynamicString			m_name;

		HardwareRuntime			m_runtime;

		size_t					m_programDataSize;
		size_t					m_ramSize;
		size_t					m_flashSize;
	};
}