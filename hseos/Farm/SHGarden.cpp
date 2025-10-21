/*********************************************************************

	 ����		: SHGarden.cpp
	 �ۼ���		: hseos
	 �ۼ���		: 2007/04/10

	 ���ϼ���	: �Թ� Ŭ������ �ҽ�

 *********************************************************************/

#include "stdafx.h"
#include "../Common/SHMath.h"
#include "SHGarden.h"


// -------------------------------------------------------------------------------------------------------------------------------------
// CSHGarden Method																												  ������
//
CSHGarden::CSHGarden()
{
	m_pcsCrop.Initialize( 5 );
	m_nEventKind = GARDEN_EVENT_CROP;
}

// -------------------------------------------------------------------------------------------------------------------------------------
// ~CSHGarden Method																											  �ı���
//
CSHGarden::~CSHGarden()
{
	m_pcsCrop.SetPositionHead();
	for( CSHCrop* crop = 0;
		(crop = m_pcsCrop.GetData()) != NULL; )
	{
		SAFE_DELETE( crop );
	}
}

// -------------------------------------------------------------------------------------------------------------------------------------
// Create Method																										���۹� ���� ����
//
VOID CSHGarden::Create(int nCropNum)
{
	for( int i = 0; i < nCropNum; ++i )
	{
		CSHCrop* crop = GetCrop( i );

		if( 0 == crop )
		{
			crop = new CSHCrop;
			m_pcsCrop.Add( crop, i );
		}

		crop->SetID( (WORD)i );
		crop->SetParent( this );
		AddChild( crop, GARDEN_EVENT_CROP );
	}
}