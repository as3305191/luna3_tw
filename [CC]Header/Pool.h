/*
081107 LUJ, STL �ڷᱸ���� �̿��� Ǯ �ڷᱸ��. ���� CMemoryPoolTempl Ŭ������ ��ü�ϱ� ���� ���������.
���� �ڷ� ������ �Ҵ纸�� ������ ���� �� ��� ������ �ε����� �߸��Ǵ� ���װ� �ִ�. �� ���װ�
�߻��ϸ� �޸𸮸� �� �̻� �Ҵ���� �� ���Եȴ�.
*/
#pragma once

#include <hash_set>
#include <vector>
#include <tchar.h>

template< typename Data >
class CPool
{
public:
	CPool()
	{}

	CPool(size_t baseSize, size_t increaseSize, LPCTSTR name)
	{
		Init(
			baseSize,
			increaseSize,
			name);
	}

	virtual ~CPool()
	{
		Release();
	}

	void Release()
	{
		for( MemoryList::const_iterator it = mMemoryList.begin();
			mMemoryList.end() != it;
			++it )
		{
			Data* data = *it;
			delete[] data;
		}

		mMemoryList.clear();
		mReservedDataSet.clear();
		mAllocatedDataSet.clear();
	}

	void Init( size_t baseSize, size_t increaseSize, const TCHAR* name )
	{
		mSetting.mBaseSize		= baseSize;
		mSetting.mIncreaseSize	= increaseSize;
		_tcsncpy( mSetting.mName, name, sizeof( mSetting.mName ) / sizeof( *mSetting.mName ) - 1 );

		Increase( baseSize );
	}

	void Free( Data* const info )
	{
		Deallocate( info );
	}

	Data* const Alloc()
	{
		return Allocate();
	}

private:
	void Increase( size_t size )
	{
		Data* dataArray = new Data[ size ];
		mMemoryList.push_back( dataArray );

		for( Data* data = dataArray + size; data-- != dataArray; )
		{
			mReservedDataSet.insert( data );
		}
	}

	Data* const Allocate()
	{
		if( mReservedDataSet.empty() )
		{
			Increase( mSetting.mIncreaseSize );

			// 081117 LUJ, �뷮�� �÷ȴµ��� �Ҵ� ���� ������ ������ ���� �Ұ�
			if( mReservedDataSet.empty() )
			{
				return 0;
			}
		}

		Data* const data = *( mReservedDataSet.begin() );

		mAllocatedDataSet.insert( data );
		mReservedDataSet.erase( data );

		return data;
	}

	void Deallocate( Data* const data )
	{	
		// 090225 LUJ, �Ҵ�Ǿ��� �޸𸮸� �����ؾ��Ѵ�
		if( mAllocatedDataSet.erase( data ) )
		{
			mReservedDataSet.insert( data );
		}
	}

	bool IsAllocated( const Data* data ) const
	{
		return mAllocatedDataSet.end() != mAllocatedDataSet.find( const_cast< Data* >( data ) );
	}

	bool IsReserved( const Data* data ) const
	{
		return mReservedDataSet.end() != mReservedDataSet.find( const_cast< Data* >( data ) );
	}

	typedef stdext::hash_set< Data* > DataSet;
	// 081117 LUJ, �Ҵ�� ������ ��
	DataSet	mAllocatedDataSet;
	// 081117 LUJ, ��� ������ ������ ��
	DataSet	mReservedDataSet;
	typedef std::list< Data* > MemoryList;
	MemoryList mMemoryList;
	
	struct Setting
	{
		size_t	mBaseSize;
		size_t	mIncreaseSize;
		TCHAR	mName[ 100 ];

		Setting()
		{
			memset( this, 0, sizeof( *this ) );
		}
	}
	mSetting;
};