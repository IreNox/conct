#pragma once

namespace conct
{
	template< class TData >
	Command< TData >::Command( CommandId id )
		: CommandBase( id )
	{
	}
}
