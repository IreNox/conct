#pragma once

#include "conct_core.h"

namespace conct
{
	class Proxy;

	class LocalInstance
	{
	public:

		InstanceId	getId() const { return m_id;  }
		Proxy*		getProxy() const { return m_pProxy; }

	private:

		InstanceId	m_id;
		Proxy*		m_pProxy;
	};
}