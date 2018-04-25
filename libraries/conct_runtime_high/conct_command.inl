#pragma once

namespace conct
{
	template< class TData >
	Command< TData >::Command( CommandId id )
		: CommandBase( id )
	{
	}

	template< class TData >
	void conct::Command<TData>::setResponse( ResultId result, const TData& data )
	{
		m_data = data;
		CommandBase::setResponse( result );
	}
}
