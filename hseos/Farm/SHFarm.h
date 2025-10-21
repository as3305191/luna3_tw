/*********************************************************************

	 ����		: SHFarm.h
	 �ۼ���		: hseos
	 �ۼ���		: 2007/04/10

	 ���ϼ���	: ���� Ŭ������ ���

 *********************************************************************/

#pragma once

#include "SHGarden.h"
#include "Pen.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
//
class CSHFarm : public CSHFarmObj
{
public:
	//----------------------------------------------------------------------------------------------------------------
	enum FARM_EVENT										// ���� �̺�Ʈ
	{
		FARM_EVENT_NOTHING = 0,							// ..�ƹ��͵� �ƴ�
		FARM_EVENT_GARDEN,								// ..�Թ�
		FARM_EVENT_ANIMALCAGE,							// ..���
		FARM_EVENT_FENCE,								// ..�WŸ��
		FARM_EVENT_HOUSE,								// ..��
		FARM_EVENT_WAREHOUSE,							// ..â��
		FARM_EVENT_MAX = FARM_EVENT_WAREHOUSE,
	};

private:
	CYHHashTable< CSHGarden > m_pcsGarden;
	CYHHashTable< CSHFarmObj > m_pcsFence;
	CYHHashTable< CPen > m_pcsAnimalCage;
	CYHHashTable< CSHFarmObj > m_pcsHouse;
	CYHHashTable< CSHFarmObj > m_pcsWarehouse;
	BOOL					m_bRendering;								// ���� ���ΰ�

	UINT					m_nTaxArrearageFreq;						// ������ ��ü Ƚ��
	char					m_szTaxPayPlayerName[MAX_NAME_LENGTH+1];	// ������ ���� �÷��̾�

public:
	//----------------------------------------------------------------------------------------------------------------
	CSHFarm();
	// 090629 LUJ, �Ļ� Ŭ������ �Ҹ��ڰ� ȣ��ǵ��� ����
	virtual ~CSHFarm();
	void Init();
	void Release();
	void CreateGarden( int gardenSize, int cropSize );
	CSHGarden* GetGarden(int nGardenID)	{ return m_pcsGarden.GetData( nGardenID ); }
	int GetGardenNum() { return m_pcsGarden.GetDataNum(); }
	//						��Ÿ�� ����
	VOID					CreateFence(int nFenceNum);
	CSHFarmObj*				GetFence(int nFenceID)		{ return m_pcsFence.GetData( nFenceID ); }
	int						GetFenceNum()				{ return m_pcsFence.GetDataNum(); }
	//----------------------------------------------------------------------------------------------------------------
	//						��� ����
	VOID					CreateAnimalCage( int nAnimalCageNum, int animalSize );
	CPen*					GetAnimalCage(int nAnimalCageID)	{ return m_pcsAnimalCage.GetData( nAnimalCageID ); }
	int						GetAnimalCageNum()					{ return m_pcsAnimalCage.GetDataNum(); }
	//----------------------------------------------------------------------------------------------------------------
	//						�� ����
	VOID					CreateHouse(int nHouseNum);
	CSHFarmObj*				GetHouse(int nHouseID)		{ return m_pcsHouse.GetData( nHouseID ); }
	int						GetHouseNum()				{ return m_pcsHouse.GetDataNum(); }
	//----------------------------------------------------------------------------------------------------------------
	//						â�� ����
	VOID					CreateWarehouse(int nWarehouseNum);
	CSHFarmObj*				GetWarehouse(int nWarehouseID)		{ return m_pcsWarehouse.GetData( nWarehouseID ); }
	int						GetWarehouseNum()					{ return m_pcsWarehouse.GetDataNum(); }

	VOID					SetIsRendering(BOOL bRendering)		{ m_bRendering = bRendering; }
	BOOL					GetIsRendering()					{ return m_bRendering; }

	//----------------------------------------------------------------------------------------------------------------
	//						������ ��ü Ƚ�� ����
	VOID					SetTaxArrearageFreq(UINT nFreq)		{ m_nTaxArrearageFreq = nFreq; }
	//						������ ��ü Ƚ�� ���
	UINT					GetTaxArrearageFreq()				{ return m_nTaxArrearageFreq; }
	//						������ ���� �÷��̾� ����
	VOID					SetTaxPayPlayerName(char* pszName)	{ pszName == NULL ? ZeroMemory(m_szTaxPayPlayerName, sizeof(m_szTaxPayPlayerName)) : SafeStrCpy(m_szTaxPayPlayerName, pszName, MAX_NAME_LENGTH+1); }
	//						������ ���� �÷��̾� ���
	char*					GetTaxPayPlayerName()				{ return m_szTaxPayPlayerName; }
	//						������ ���� ���� ���
	BOOL					IsPayTax()							{ return m_szTaxPayPlayerName[0]; }
};