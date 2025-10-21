/*********************************************************************

	 ����		: Pen.cpp
	 �ۼ���		: Shinobi
	 �ۼ���		: 2008/03/11

	 ���ϼ���	: ��� Ŭ������ �ҽ�

 *********************************************************************/

#include "stdafx.h"
#include "../Common/SHMath.h"
#include "Pen.h"


// -------------------------------------------------------------------------------------------------------------------------------------
// CPen Method																												  ������
//
CPen::CPen()
{
	m_pcsAnimal.Initialize( 5 );
	m_nEventKind = GARDEN_EVENT_ANIMAL;
}

// -------------------------------------------------------------------------------------------------------------------------------------
// ~CPen Method																											  �ı���
//
CPen::~CPen()
{
	m_pcsAnimal.SetPositionHead();
	CAnimal* animal = NULL;
	while( (animal = m_pcsAnimal.GetData()) != NULL )
	{
		SAFE_DELETE( animal );
	}
}

// -------------------------------------------------------------------------------------------------------------------------------------
// Create Method																										���۹� ���� ����
//
VOID CPen::Create(int nAnimalNum)
{
	for(int i=0; i<nAnimalNum; i++)
	{
		CAnimal* animal = GetAnimal( i );

		if( 0 == animal )
		{
			animal = new CAnimal;
			m_pcsAnimal.Add( animal, i );
		}

		animal->SetID((WORD)i);
		animal->SetParent(this);
		AddChild( animal, GARDEN_EVENT_ANIMAL );
	}
}