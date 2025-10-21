/*********************************************************************

	 ����		: SHFarmZone.cpp
	 �ۼ���		: hseos
	 �ۼ���		: 2007/04/10

	 ���ϼ���	: ���� ���� Ŭ������ �ҽ�

 *********************************************************************/

#include "stdafx.h"
#include "../Common/SHMath.h"
#include "SHFarmZone.h"


// -------------------------------------------------------------------------------------------------------------------------------------
// CSHFarmZone Method																											  ������
//
CSHFarmZone::CSHFarmZone(FARM_ZONE eBelongZone, int nMapID, int nFarmNum)
{
	m_csFarm.Initialize( 10 );

	for( int i = 0; i < nFarmNum; ++i )
	{
		CSHFarm* farm = new CSHFarm;
		farm->SetID( (WORD)i );
		farm->SetParent( this );
		AddChild(
			farm,
			FARM_ZONE_EVENT_FARM );
		m_csFarm.Add( farm, i );
	}
	m_eBelongZone = eBelongZone;
	m_nMapID = nMapID;
	m_nEventKind = FARM_ZONE_EVENT_FARM;
}

// -------------------------------------------------------------------------------------------------------------------------------------
// ~CSHFarmZone Method																											  �ı���
//
CSHFarmZone::~CSHFarmZone()
{
	m_csFarm.SetPositionHead();
	CSHFarm* farm = NULL;
	while( (farm = m_csFarm.GetData()) != NULL )
	{
		SAFE_DELETE( farm );
	}
}

int CSHFarmZone::GetFarmNum( DWORD objectIndex )
{
	DWORD ownedFarmCount = 0;

	m_csFarm.SetPositionHead();
	CSHFarm* farm = NULL;
	while( (farm = m_csFarm.GetData()) != NULL )
	{
		if( farm->GetOwner() == objectIndex )
		{
			++ownedFarmCount;
		}
	}

	return ownedFarmCount;
}