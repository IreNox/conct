#include "interface.h"

namespace conct
{
	Interface::Interface()
	{
		m_pBase = false;
	}

	bool Interface::load( const std::string& fileName, TypeCollection& typeCollection )
	{
		return true;
	}
}
