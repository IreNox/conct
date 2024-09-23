#include "system_browser_proxy.h"

#include "system_browser.h"
#include "conct_core.h"

namespace conct
{
	BrowserProxy::BrowserProxy()
		: Proxy( Browser::s_typeCrc )
	{
	}

	bool BrowserProxy::getProperty( ValueBuilder& targetValueBuilder, const void* pInstance, uint16 nameCrc ) const
	{
		const Browser* pTypedInstance = static_cast< const Browser* >( pInstance );

		if( nameCrc == 0x21b6 )
		{
			targetValueBuilder.setBoolean( pTypedInstance->getIsOpen() );
			return true;
		}

		return false;
	}

	bool BrowserProxy::setProperty( void* pInstance, uint16 nameCrc, const Value& value ) const
	{
		Browser* pTypedInstance = static_cast< Browser* >( pInstance );

		if( nameCrc == 0x21b6 )
		{
			pTypedInstance->setIsOpen( value.getBoolean() );
			return true;
		}

		return false;
	}

	bool BrowserProxy::callFunction( ValueBuilder& targetValueBuilder, void* pInstance, uint16 nameCrc, const ArrayView< const Value >& parameters ) const
	{
		Browser* pTypedInstance = static_cast< Browser* >( pInstance );

		if( nameCrc == 0xb3bd )
		{
			pTypedInstance->Open( parameters[ 0u ].getString() ); targetValueBuilder.setVoid();
			return true;
		}

		return false;
	}
}
