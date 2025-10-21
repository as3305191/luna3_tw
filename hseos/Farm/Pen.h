/*********************************************************************

	 ����		: Pen.h
	 �ۼ���		: Shinobi
	 �ۼ���		: 2008/03/11

	 ���ϼ���	: ��� Ŭ������ ���

 *********************************************************************/

#pragma once

#include "Animal.h"
#include "SHFarmObj.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
class CPen : public CSHFarmObj
{
public:
	//----------------------------------------------------------------------------------------------------------------
	typedef enum														// ��� ���� ���� ���
	{
		GARDEN_EVENT_NOTHING = 0,										// ..�ƹ� �ϵ�..
		GARDEN_EVENT_ANIMAL,											// ..����
	} GARDEN_EVENT;

private:
	CYHHashTable< CAnimal > m_pcsAnimal;

public:
	//----------------------------------------------------------------------------------------------------------------
	CPen();
	// 090629 LUJ, �Ļ� Ŭ������ �Ҹ��ڰ� ȣ��ǵ��� ����
	virtual ~CPen();
	void Create( int nAnimalNum );
	CAnimal* GetAnimal( int nAnimalID )	{ return m_pcsAnimal.GetData( nAnimalID ); }
	int GetAnimalNum() { return m_pcsAnimal.GetDataNum(); }
};