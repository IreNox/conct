#pragma once

#include "conct_core.h"

namespace conct
{
	static const uintreg InvalidStringIndex = ( uintreg )-1;

	CONCT_FORCE_INLINE uintreg		getStringSize( const char* pSource );		///< returns the string size in bytes
	CONCT_FORCE_INLINE uintreg		getStringLength( const char* pSource );		///< returns the string length in chars(for UTF8)

	CONCT_FORCE_INLINE uintreg		copyString( char* pTargetBuffer, uintreg bufferSize, const char* pSource );
	CONCT_FORCE_INLINE uintreg		appendString( char* pTargetbuffer, uintreg bufferSize, const char* pSource );

	CONCT_FORCE_INLINE bool			isStringEmpty( const char* pString );
	CONCT_FORCE_INLINE bool			isStringEquals( const char* pString1, const char* pString2 );

	CONCT_FORCE_INLINE bool			doesStringStartWith( const char* pString, const char* pValue );
	CONCT_FORCE_INLINE bool			doesStringEndWith( const char* pString, const char* pValue );

	CONCT_FORCE_INLINE void			stringReplace( char* pString, char oldChar, char newChar );

	CONCT_FORCE_INLINE uintreg		stringIndexOf( const char* pString, char c, uintreg index = 0 );
	CONCT_FORCE_INLINE uintreg		stringIndexOf( const char* pString, const char* pSearch, uintreg index = 0 );
	CONCT_FORCE_INLINE uintreg		stringLastIndexOf( const char* pString, char c, uintreg index = InvalidStringIndex );
	CONCT_FORCE_INLINE uintreg		stringLastIndexOf( const char* pString, const char* pSearch, uintreg index = InvalidStringIndex );
}

#include "conct_string.inl"
