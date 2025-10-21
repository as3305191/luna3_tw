/*********************************************************************

	 ����		: SHFarmZone.h
	 �ۼ���		: hseos
	 �ۼ���		: 2007/04/10

	 ���ϼ���	: ���� ���� Ŭ������ ���

 *********************************************************************/

#pragma once

#include "SHFarm.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
//
class CSHFarmZone : public CSHFarmObj
{
public:
	//----------------------------------------------------------------------------------------------------------------
	typedef enum										// ������ ���� ����
	{
		FARM_ZONE_ALKER,
		// 090921 LUJ, �׶� ���� �߰�
		FARM_ZONE_NERA,
		FARM_ZONE_MAX
	} FARM_ZONE;

	//----------------------------------------------------------------------------------------------------------------
	typedef enum										// ���� ���� �̺�Ʈ
	{
		FARM_ZONE_EVENT_NOTHING = 0,					// ..�ƹ��͵� �ƴ�
		FARM_ZONE_EVENT_FARM,							// ..����
	} FARM_ZONE_EVENT;

private:
	CYHHashTable< CSHFarm > m_csFarm;
	FARM_ZONE				m_eBelongZone;				// ��� ������ ����?
	int						m_nMapID;					// �� ��ȣ

public:
	//----------------------------------------------------------------------------------------------------------------
	CSHFarmZone(FARM_ZONE eBelongZone, int nMapID, int nFarmNum);
	// 090629 LUJ, �Ļ� Ŭ������ �Ҹ��ڰ� ȣ��ǵ��� ����
	virtual ~CSHFarmZone();
	CSHFarm* GetFarm( DWORD index )	{ return m_csFarm.GetData( index ); }
	int GetMapID() { return m_nMapID; }
	int GetFarmNum() { return m_csFarm.GetDataNum(); }
	// 090629 LUJ, ������Ʈ�� ������ ���� ������ ��ȯ�Ѵ�
	int GetFarmNum( DWORD objectIndex );
};
