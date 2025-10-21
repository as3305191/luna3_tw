/*********************************************************************

	 ����		: SHGarden.h
	 �ۼ���		: hseos
	 �ۼ���		: 2007/04/10

	 ���ϼ���	: �Թ� Ŭ������ ���

 *********************************************************************/

#pragma once

#include "SHCrop.h"
#include "SHFarmObj.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
class CSHGarden : public CSHFarmObj
{
public:
	//----------------------------------------------------------------------------------------------------------------
	typedef enum														// ���� �Թ� ���� ���
	{
		GARDEN_EVENT_NOTHING = 0,										// ..�ƹ� �ϵ�..
		GARDEN_EVENT_CROP,												// ..���۹�
	} GARDEN_EVENT;

private:
	CYHHashTable< CSHCrop > m_pcsCrop;

public:
	//----------------------------------------------------------------------------------------------------------------
	CSHGarden();
	// 090629 LUJ, �Ļ� Ŭ������ �Ҹ��ڰ� ȣ��ǵ��� ����
	virtual ~CSHGarden();
	void Create(int nCropNum);
	CSHCrop* GetCrop(int nCropID) { return m_pcsCrop.GetData( nCropID ); }
	int	GetCropNum() { return m_pcsCrop.GetDataNum(); }	
};