#pragma once

#include "conct_array_view.h"
#include "conct_core.h"

namespace conct
{
	struct Value;

	class DataBuilder
	{
	public:

							DataBuilder();
							DataBuilder( void* pData, uintreg size );

		void				set( void* pData, uintreg size );

		bool				isCreated() const { return m_pBaseData != nullptr; }
		bool				isEnd() const { return m_pData == m_pEnd; }
		bool				isExceeded() const { return m_pData > m_pEnd; }

		uintreg				getSize() const { return m_pEnd - m_pBaseData; }
		uintreg				getRemainingSize() const { return m_pEnd - m_pData; }

		template< class T >
		T*					pushStruct();
		template< class T >
		void				pushStruct( const T& data );

		template< class T >
		T*					pushArray( uintreg count );

		void*				pushData( uintreg length );
		void				pushData( const void* pData, uintreg length );

		const char*			pushString( const char* pString );
		void				pushValueData( Value* pTargetValue, const Value* pSourceValue );

		ArrayView< uint8 >	toArrayView() const;

	private:

		uint8*		m_pBaseData;
		uint8*		m_pData;
		uint8*		m_pEnd;
	};

	template< uintreg TBufferSize >
	class BufferedDataBuilder : public DataBuilder
	{
	public:

				BufferedDataBuilder();

	private:

		uint8	m_buffer[ TBufferSize ];
	};

	template< class T >
	class StructDataBuilder : public BufferedDataBuilder< sizeof( T ) >
	{
	public:

				StructDataBuilder();

		T*		pushGenericStruct();
	};
}

#include "conct_data_builder.inl"
