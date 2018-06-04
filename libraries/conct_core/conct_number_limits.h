#pragma once

#include "conct_core.h"

#undef min
#undef max

namespace conct
{
	template <class T>
	class NumberLimits
	{
	public:

		CONCT_FORCE_INLINE static bool isSigned()
		{
			return false;
		}

		CONCT_FORCE_INLINE static T min()
		{
			return (T)0;
		}

		CONCT_FORCE_INLINE static T max()
		{
			return (T)0;
		}

		CONCT_FORCE_INLINE static T lowest()
		{
			return (T)0;
		}

		CONCT_FORCE_INLINE static T epsilon()
		{
			return (T)0;
		}
	};

	template< class T >
	class NumberLimits< const T > : public NumberLimits< T >
	{
	};

	template< class T >
	class NumberLimits< volatile T > : public NumberLimits< T >
	{
	};

	template< class T >
	class NumberLimits< const volatile T > : public NumberLimits< T >
	{
	};

	template <>
	class NumberLimits< sint8 >
	{
	public:

		typedef sint8 T;

		CONCT_FORCE_INLINE static bool isSigned()
		{
			return true;
		}

		CONCT_FORCE_INLINE static T min()
		{
			return (T)0x80;
		}

		CONCT_FORCE_INLINE static T max()
		{
			return (T)0x7f;
		}

		CONCT_FORCE_INLINE static T lowest()
		{
			return min();
		}

		CONCT_FORCE_INLINE static T epsilon()
		{
			return (T)0;
		}
	};

	template <>
	class NumberLimits< sint16 >
	{
	public:

		typedef sint16 T;

		CONCT_FORCE_INLINE static bool isSigned()
		{
			return true;
		}

		CONCT_FORCE_INLINE static T min()
		{
			return (T)0x8000;
		}

		CONCT_FORCE_INLINE static T max()
		{
			return (T)0x7fff;
		}

		CONCT_FORCE_INLINE static T lowest()
		{
			return min();
		}

		CONCT_FORCE_INLINE static T epsilon()
		{
			return (T)0;
		}
	};

	template <>
	class NumberLimits< sint32 >
	{
	public:

		typedef sint32 T;

		CONCT_FORCE_INLINE static bool isSigned()
		{
			return true;
		}

		CONCT_FORCE_INLINE static T min()
		{
			return (T)0x80000000;
		}

		CONCT_FORCE_INLINE static T max()
		{
			return (T)0x7fffffff;
		}

		CONCT_FORCE_INLINE static T lowest()
		{
			return min();
		}

		CONCT_FORCE_INLINE static T epsilon()
		{
			return (T)0;
		}
	};

	template <>
	class NumberLimits< sint64 >
	{
	public:

		typedef sint64 T;

		CONCT_FORCE_INLINE static bool isSigned()
		{
			return true;
		}

		CONCT_FORCE_INLINE static T min()
		{
			return 0x8000000000000000ll;
		}

		CONCT_FORCE_INLINE static T max()
		{
			return 0x7fffffffffffffffll;
		}

		CONCT_FORCE_INLINE static T lowest()
		{
			return min();
		}

		CONCT_FORCE_INLINE static T epsilon()
		{
			return 0ll;
		}
	};

	template <>
	class NumberLimits< uint8 >
	{
	public:

		typedef uint8 T;

		CONCT_FORCE_INLINE static bool isSigned()
		{
			return false;
		}

		CONCT_FORCE_INLINE static T min()
		{
			return (T)0;
		}

		CONCT_FORCE_INLINE static T max()
		{
			return (T)0xff;
		}

		CONCT_FORCE_INLINE static T lowest()
		{
			return min();
		}

		CONCT_FORCE_INLINE static T epsilon()
		{
			return (T)0;
		}
	};

	template <>
	class NumberLimits< uint16 >
	{
	public:

		typedef uint16 T;

		CONCT_FORCE_INLINE static bool isSigned()
		{
			return false;
		}

		CONCT_FORCE_INLINE static T min()
		{
			return (T)0;
		}

		CONCT_FORCE_INLINE static T max()
		{
			return (T)0xffff;
		}

		CONCT_FORCE_INLINE static T lowest()
		{
			return min();
		}

		CONCT_FORCE_INLINE static T epsilon()
		{
			return (T)0;
		}
	};

	template <>
	class NumberLimits< uint32 >
	{
	public:

		typedef uint32 T;

		CONCT_FORCE_INLINE static bool isSigned()
		{
			return false;
		}

		CONCT_FORCE_INLINE static T min()
		{
			return (T)0;
		}

		CONCT_FORCE_INLINE static T max()
		{
			return (T)0xffffffff;
		}

		CONCT_FORCE_INLINE static T lowest()
		{
			return min();
		}

		CONCT_FORCE_INLINE static T epsilon()
		{
			return (T)0;
		}
	};

	template <>
	class NumberLimits< uint64 >
	{
	public:

		typedef uint64 T;

		CONCT_FORCE_INLINE static bool isSigned()
		{
			return false;
		}

		CONCT_FORCE_INLINE static T min()
		{
			return (T)0;
		}

		CONCT_FORCE_INLINE static T max()
		{
			return (T)0xffffffffffffffff;
		}

		CONCT_FORCE_INLINE static T lowest()
		{
			return min();
		}

		CONCT_FORCE_INLINE static T epsilon()
		{
			return (T)0;
		}
	};

	template <>
	class NumberLimits< float >
	{
	public:

		typedef float T;

		CONCT_FORCE_INLINE static bool isSigned()
		{
			return true;
		}

		CONCT_FORCE_INLINE static T min()
		{
			return (T)1.175494351e-38f;
		}

		CONCT_FORCE_INLINE static T max()
		{
			return (T)3.402823466e+38f;
		}

		CONCT_FORCE_INLINE static T lowest()
		{
			return -max();
		}

		CONCT_FORCE_INLINE static T epsilon()
		{
			return (T)1.192092896e-07f;
		}
	};

	template <>
	class NumberLimits< double >
	{
	public:

		typedef double T;

		CONCT_FORCE_INLINE static bool isSigned()
		{
			return true;
		}

		CONCT_FORCE_INLINE static T min()
		{
			return (T)2.2250738585072014e-308;
		}

		CONCT_FORCE_INLINE static T max()
		{
			return (T)1.7976931348623158e+308;
		}

		CONCT_FORCE_INLINE static T lowest()
		{
			return -max();
		}

		CONCT_FORCE_INLINE static T epsilon()
		{
			return (T)2.2204460492503131e-016;
		}
	};
}
