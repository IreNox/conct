#include "conct_dynamic_memory.h"

#include "conct_functions.h"
#include "conct_memory.h"
#include "conct_mutex.h"
#include "conct_thread_local_storage.h"
#include "conct_trace.h"

#include <tlsf/tlsf.h>

#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
#	include <windows.h>
#elif CONCT_ENABLED( CONCT_PLATFORM_ANDROID ) || CONCT_ENABLED( CONCT_PLATFORM_LINUX )
#	include <stdlib.h>
#	include <sys/mman.h>
#	include <unistd.h>
#endif

#if CONCT_ENABLED( CONCT_TARGET_DLL )
#	include "conct_simulator.h"
#	include "conct_simulator_context.h"
#endif

namespace conct
{
	class DynamicMemory
	{
	public:

				DynamicMemory();
				~DynamicMemory();

		void*	allocateMemory( uintptr size, uintptr alignment /*= 0u */ );
		void	freeMemory( void* pAddress );

	private:

		struct Pool
		{
			Pool*						pPrevious;

			uintptr						size;
			pool_t						pool;
		};

		struct Allocator
		{
			tlsf_t						tlsf;
			Pool*						pCurrentPool;

			uintptr						poolCount;
			uintptr						allocatedSize;
			uintptr						allocationCount;
			uintptr						totalAllocations;
			uintptr						maxAllocated;
		};

		ThreadLocalStorageHandle	m_allocatorTls;

		uintptr						m_pageSize;

		Mutex						m_mutex;
		Allocator*					m_pDefaultAllocator;

		Allocator*					createAllocator( uintptr size );
		void*						allocateFromAllocator( Allocator* pAllocator, uintptr size, uintptr alignment );
		void						freeFromAllocator( Allocator* pAllocator, void* pAddress );
	};

	static const uintptr s_defaultPoolSize = 100u * 1024u; // 100 kib
#if CONCT_ENABLED( CONCT_TARGET_DLL )
	static DynamicMemory* s_pDynamicMemory = getSimulatorContext().getDynamicMemory();
#else
	static DynamicMemory s_dynamicMemory;
	static DynamicMemory* s_pDynamicMemory = &s_dynamicMemory;
#endif


	DynamicMemory::DynamicMemory()
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		SYSTEM_INFO systemInfo;
		GetSystemInfo( &systemInfo );
		m_pageSize = systemInfo.dwPageSize;
#elif CONCT_ENABLED( CONCT_PLATFORM_ANDROID ) || CONCT_ENABLED( CONCT_PLATFORM_LINUX )
		m_pageSize = sysconf( _SC_PAGE_SIZE );
#else
#	error "Platform not supported"
#endif

		m_allocatorTls		= thread_local_storage::create();
		m_pDefaultAllocator	= createAllocator( s_defaultPoolSize );
	}

	DynamicMemory::~DynamicMemory()
	{
		thread_local_storage::destroy( m_allocatorTls );
	}

	void* DynamicMemory::allocateMemory( uintptr size, uintptr alignment /*= 0u */ )
	{
		Allocator* pAllocator = (Allocator*)thread_local_storage::getValue( m_allocatorTls );
		if( pAllocator != nullptr )
		{
			return allocateFromAllocator( pAllocator, size, alignment );
		}
		else
		{
			MutexLock lock( m_mutex );
			return allocateFromAllocator( m_pDefaultAllocator, size, alignment );
		}
	}

	void DynamicMemory::freeMemory( void* pAddress )
	{
		if( pAddress == nullptr )
		{
			return;
		}

		Allocator* pAllocator = (Allocator*)thread_local_storage::getValue( m_allocatorTls );
		if( pAllocator != nullptr )
		{
			freeFromAllocator( pAllocator, pAddress );
		}
		else
		{
			MutexLock lock( m_mutex );
			freeFromAllocator( m_pDefaultAllocator, pAddress );
		}
	}

	DynamicMemory::Allocator* DynamicMemory::createAllocator( uintptr size )
	{
		const uintptr allocatorSize	= alignValue( sizeof( Allocator ) + tlsf_size() + sizeof( Pool ) + tlsf_pool_overhead() + size, m_pageSize );

		void* pAllocatorMemory = memory::allocateSystemMemory( allocatorSize, m_pageSize );
		if( pAllocatorMemory == nullptr )
		{
			return nullptr;
		}

		Pool* pPool					= (Pool*)pAllocatorMemory;
		Allocator* pAllocator		= (Allocator*)alignPointer( &pPool[ 1u ], CONCT_ALIGNOF( Allocator ) );
		uint8* pTlsf				= (uint8*)alignPointer( &pAllocator[ 1u ], tlsf_align_size() );
		void* pPoolData				= alignPointer( pTlsf + tlsf_size(), tlsf_align_size() );

		const uintptr poolSize		= allocatorSize - (uintptr( pPoolData ) - uintptr( pAllocatorMemory ));

		memory::zero( *pAllocator );
		pAllocator->pCurrentPool	= pPool;
		pAllocator->tlsf			= tlsf_create( pTlsf );
		pAllocator->poolCount		= 1u;

		pPool->pPrevious			= nullptr;
		pPool->size					= allocatorSize;
		pPool->pool					= tlsf_add_pool( pAllocator->tlsf, pPoolData, poolSize );

		return pAllocator;
	}

	void* DynamicMemory::allocateFromAllocator( Allocator* pAllocator, uintptr size, uintptr alignment )
	{
		void* pAddress = tlsf_memalign( pAllocator->tlsf, alignment, size );
		if( pAddress == nullptr )
		{
			const uintptr minPoolSize	= alignValue( sizeof( Pool ) + tlsf_pool_overhead() + tlsf_alloc_overhead() + alignment + size, m_pageSize );
			const uintptr poolSize		= CONCT_MAX( s_defaultPoolSize, minPoolSize );

			void* pPoolMemory = memory::allocateSystemMemory( poolSize, m_pageSize );
			if( pPoolMemory == nullptr )
			{
				return nullptr;
			}

			Pool* pPool		= (Pool*)pPoolMemory;
			void* pPoolData	= alignPointer( &pPool[ 1u ], tlsf_align_size() );

			pPool->size		= poolSize;
			pPool->pool		= tlsf_add_pool( pAllocator->tlsf, pPoolData, poolSize - sizeof( Pool ) );

			pPool->pPrevious = pAllocator->pCurrentPool;
			pAllocator->pCurrentPool = pPool;

			pAddress = tlsf_memalign( pAllocator->tlsf, alignment, size );
			if( pAddress == nullptr )
			{
				trace::write( "Could not allocate memory.\n" );
				return nullptr;
			}

			pAllocator->poolCount++;
		}

		pAllocator->allocatedSize += tlsf_block_size( pAddress );
		pAllocator->allocationCount++;
		pAllocator->totalAllocations++;
		pAllocator->maxAllocated = CONCT_MAX( pAllocator->maxAllocated, pAllocator->allocatedSize );

		return pAddress;
	}

	void DynamicMemory::freeFromAllocator( Allocator* pAllocator, void* pAddress )
	{
		pAllocator->allocatedSize -= tlsf_block_size( pAddress );
		pAllocator->allocationCount--;

		tlsf_free( pAllocator->tlsf, pAddress );
	}

	DynamicMemory* memory::getDynamicMemory()
	{
		return s_pDynamicMemory;
	}

	void* memory::allocateSystemMemory( uintptr size, uintptr alignment )
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		return _aligned_malloc( size, alignment );
#elif CONCT_ENABLED( CONCT_PLATFORM_ANDROID ) || CONCT_ENABLED( CONCT_PLATFORM_LINUX )
		void* pAddress;
		if( posix_memalign( &pAddress, alignment, size ) != 0 )
		{
			return nullptr;
		}
		return pAddress;
#else
#	error "Platform not supported"
#endif
	}

	void memory::freeSystemMemory( void* pAddress )
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		_aligned_free( pAddress );
#elif CONCT_ENABLED( CONCT_PLATFORM_ANDROID ) || CONCT_ENABLED( CONCT_PLATFORM_LINUX )
		free( pAddress );
#else
#	error "Platform not supported"
#endif
	}

	void memory::beginThreadAllocator( uintptr size )
	{

	}

	void memory::protectThreadAllocator()
	{
		//mprotect( m_name.getBegin(), int( m_name.getLength() ), PROT_READ );
	}

	void memory::unprotectThreadAllocator()
	{

	}

	void memory::endThreadAllocator()
	{

	}

	void* memory::allocateMemory( uintptr size, uintptr allignment /*= 0u */ )
	{
		return s_pDynamicMemory->allocateMemory( size, allignment );
	}

	void memory::freeMemory( void* pAddress )
	{
		s_pDynamicMemory->freeMemory( pAddress );
	}
}

void* operator new(size_t size)
{
	return conct::memory::allocateMemory( size );
}

void operator delete(void* pAddress)
{
	conct::memory::freeMemory( pAddress );
}

void* operator new[]( size_t size )
{
	return conct::memory::allocateMemory( size );
}

void operator delete[]( void* pAddress )
{
	conct::memory::freeMemory( pAddress );
}
