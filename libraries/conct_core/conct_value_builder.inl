#pragma once

namespace conct
{
	template< class T >
	ResultId ValueBuilder::setStruct( const T& value )
	{
		T* pStruct = m_builder.pushStruct< T >();
		if( pStruct == nullptr )
		{
			return ResultId_OutOfMemory;
		}
		*pStruct = value;

		CONCT_ASSERT( ( uintptr )pStruct - ( uintptr )m_pValue < NumberLimits< uint16 >::max() );
		m_pValue->type = ValueType_Struct;
		m_pValue->data.structure.offset = uint16( ( uintptr )pStruct - ( uintptr )m_pValue );

		return ResultId_Success;
	}

	template< class T >
	ResultId ValueBuilder::setArray( const ArrayView< T >& value )
	{
		T* pArray = m_builder.pushArray< T >( value.getCount() );
		if( pStruct == nullptr )
		{
			return ResultId_OutOfMemory;
		}

		for( uintreg i = 0u; i < value.getCount(); ++i )
		{
			pArray[ i ] = value[ i ];
		}

		CONCT_ASSERT( ( uintptr )pArray - ( uintptr )m_pValue < NumberLimits< uint16 >::max() );
		m_pValue->type = ValueType_Struct;
		m_pValue->data.array.offset = uint16( ( uintptr )pArray - ( uintptr )m_pValue );
		m_pValue->data.array.size = uint16( value.getCount() );

		return ResultId_Success;
	}
}