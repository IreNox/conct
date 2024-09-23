#include "system_clipboard_proxy.h"

#include "system_clipboard.h"
#include "conct_core.h"

namespace conct
{
	ClipboardProxy::ClipboardProxy()
		: Proxy( Clipboard::s_typeCrc )
	{
	}

	bool ClipboardProxy::getProperty( ValueBuilder& targetValueBuilder, const void* pInstance, uint16 nameCrc ) const
	{
		const Clipboard* pTypedInstance = static_cast< const Clipboard* >( pInstance );

		if( nameCrc == 0x0822 )
		{
			targetValueBuilder.setString( pTypedInstance->getText() );
			return true;
		}

		return false;
	}

	bool ClipboardProxy::setProperty( void* pInstance, uint16 nameCrc, const Value& value ) const
	{
		Clipboard* pTypedInstance = static_cast< Clipboard* >( pInstance );

		if( nameCrc == 0x0822 )
		{
			pTypedInstance->setText( value.getString() );
			return true;
		}

		return false;
	}

	bool ClipboardProxy::callFunction( ValueBuilder& targetValueBuilder, void* pInstance, uint16 nameCrc, const ArrayView< const Value >& parameters ) const
	{
		Clipboard* pTypedInstance = static_cast< Clipboard* >( pInstance );

		if( nameCrc == 0x35e1 )
		{
			pTypedInstance->Clear(); targetValueBuilder.setVoid();
			return true;
		}

		return false;
	}
}
