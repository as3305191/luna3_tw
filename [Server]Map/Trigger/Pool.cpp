#include "StdAfx.h"
#include "Pool.h"

namespace Trigger
{
	CPool::CPool(void) :
	mAllocatedSizeAtOneTime( 10 )
	{
	}

	CPool::~CPool(void)
	{
		// ��ü �Ҹ� �� �Ҵ��� ������ �����Ѵ�
		for( MemorySet::const_iterator it = mAllocatedMemorySet.begin();
			mAllocatedMemorySet.end() != it;
			++it )
		{
			free( *it );
		}
	}

	CPool& CPool::GetInstance()
	{
		static CPool triggerPool;

		return triggerPool;
	}

	void* CPool::Allocate( size_t size )
	{
		// �Ҵ���� ũ�⿡ �ش��ϴ� �޸� ���� �����´�
		MemorySet& memorySet = mPoolMemoryMap[ size ];
		
		// �Ҵ��� �޸𸮰� ���� ��� ū ����� �Ҵ��� �ް�, �޸� �¿� �־ ��ȯ�ϱ� ���ϰ� �Ѵ�
		if( memorySet.empty() )
		{
			// ū ����� �Ҵ�ް�, �̱��� ��ü�� �Ҹ��� �� ���� free�� �� �ֵ��� �����̳ʿ� �ִ´�
			void* const allocatedMemory = malloc( mAllocatedSizeAtOneTime * size );
			mAllocatedMemorySet.insert( allocatedMemory );

			// ��ȯ�ϱ� ������ �Ҵ� ������ �ɰ��� �����̳ʿ� �ִ´�.
			for( size_t i = 0; i < mAllocatedSizeAtOneTime; ++i )
			{
				char* const chunk = ( char* )allocatedMemory + i * size;
				memorySet.insert( ( void* )chunk );
			}
		}

		// �޸� �¿� �� ���� ��ȯ�ϰ�, �����̳ʿ��� �����Ѵ�
		void* const memory = *( memorySet.begin() );
		memorySet.erase( memory );

		return memory;
	}

	void CPool::Deallocate( void* chunk, size_t size )
	{
		// ��ȯ�� �޸𸮸� ������ �� �ֵ��� �����̳ʿ� �ִ´�
		MemorySet& memorySet = mPoolMemoryMap[ size ];
		memorySet.insert( chunk );
	}

	void* CPoolObject::operator new( size_t size )
	{
		return CPool::GetInstance().Allocate( size );
	}

	void* CPoolObject::operator new[]( size_t size )
	{ 
		return CPool::GetInstance().Allocate( size );
	}

	void CPoolObject::operator delete( void* memory, size_t size )
	{
		CPool::GetInstance().Deallocate( memory, size );
	}

	void CPoolObject::operator delete[] ( void* memory, size_t size )
	{
		CPool::GetInstance().Deallocate( memory, size );
	}
}