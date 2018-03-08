#pragma once

#include "conct_array_view.h"
#include "conct_core.h"
#include "conct_runtime.h"

namespace conct
{
	class Device;
	class Port;
	class Reader;

	class RuntimeHigh
	{
	public:

		void					setup( Device* pDevice );

		void					processPort( Port* pPort );

	private:

		ArrayView< LocalInstance >	m_instances;
	};
}
