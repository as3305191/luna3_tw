//-------------------------------------------------------------------------------------------------
//	NAME		: CNpcRecallMoveable.cpp
//	DESC		: Implementation part of CNpcRecallMoveable class.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------
//		Include part.
//-------------------------------------------------------------------------------------------------
#include "stdafx.h"																						// An include file for standard system include files

#include "./NpcRecallMoveable.h"





//-------------------------------------------------------------------------------------------------
//	NAME		: CNpcRecallMoveable
//	DESC		: ������ �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
CNpcRecallMoveable::CNpcRecallMoveable(void)
{
	// ��� �Լ��� �ʱ�ȭ �Ѵ�.
	m_dwRemainTime		= 0 ;

	m_dwMoveTime		= 0 ;

	m_fMoveXpos			= 0.0f ;
	m_fMoveZpos			= 0.0f ;

	m_ChangeMapNum		= 0 ;

	m_fChangeMapXPos	= 0.0f ;
	m_fChangeMapZPos	= 0.0f ;

	m_byMoved			= FALSE ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: ~CNpcRecallMoveable
//	DESC		: �Ҹ��� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
CNpcRecallMoveable::~CNpcRecallMoveable(void)
{
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Set_RemainTime
//	DESC		: Npc�� ��ȯ�� ���� �� �ð��� �����ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMoveable::Set_RemainTime(DWORD dwRemainTime)
{
	// �̵� �� �ð����� ���� ��ġ�� �������� Ȯ���Ѵ�.
	// 081029 LYW --- NpcRecallMoveable : ���� ����Ʈ ��ȯ���� �ʴ� ���� ����.
	//if( dwRemainTime <= m_dwMoveTime )
	//{
	//	Throw_Error("Invalid remain time!!", __FUNCTION__) ;
	//	return ;
	//}


	// ��ȯ ���� �ð��� �����Ѵ�.
	m_dwRemainTime = dwRemainTime ; 
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Set_MoveTime
//	DESC		: Npc�� �̵��� X/Z��ǥ�� �����ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMoveable::Set_MoveTime(DWORD dwMoveTime)
{
	// ��ȯ ���� �ð� ���� ū ��ġ�� �������� Ȯ���Ѵ�.
	if( dwMoveTime == 0 || dwMoveTime >= m_dwRemainTime )
	{
		Throw_Error("Invalid move time!!", __FUNCTION__) ;
		return ;
	}


	// �̵� �� �ð��� �����Ѵ�.
	m_dwMoveTime = dwMoveTime ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Set_MoveposX
//	DESC		: Npc�� �̵��� X��ǥ�� �����ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMoveable::Set_MoveposX(float fXpos)
{
	// �̵��� ��ǥ�� ��ȿ���� Ȯ���Ѵ�.
	if( fXpos <= 0.0f )
	{
		Throw_Error("Invalid move position!!", __FUNCTION__) ;
		return ;
	}


	// �̵� �� ��ǥ�� �����Ѵ�.
	m_fMoveXpos = fXpos ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Set_MoveposZ
//	DESC		: Npc�� �̵��� Z��ǥ�� �����ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMoveable::Set_MoveposZ(float fZpos)
{
	// �̵��� ��ǥ�� ��ȿ���� Ȯ���Ѵ�.
	if( fZpos <= 0.0f )
	{
		Throw_Error("Invalid move position!!", __FUNCTION__) ;
		return ;
	}


	// �̵� �� ��ǥ�� �����Ѵ�.
	m_fMoveZpos = fZpos ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: IsMoveable
//	DESC		: �̵��� ���� �� �������� ��ȯ�ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
BYTE CNpcRecallMoveable::IsMoveable(DWORD dwCurTime)
{
	// �̵��� ������ �ð��� �������� Ȯ���Ѵ�.
	DWORD dwCheckTime = dwCurTime - Get_RecallTime() ;


	// ����� return �Ѵ�.
	if(dwCheckTime >= m_dwMoveTime) return TRUE ;
	else return FALSE ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: IsEndRemainTime
//	DESC		: ��ȯ ���� �ð��� �������� ��ȯ�ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
BYTE CNpcRecallMoveable::IsEndRemainTime(DWORD dwCurTime) 
{
	// ��ȯ ���� �ð��� �������� Ȯ���Ѵ�.
	DWORD dwCheckTime = dwCurTime - Get_RecallTime() ;


	// ����� return �Ѵ�.
	if( dwCheckTime >= m_dwRemainTime ) return TRUE ;
	else return FALSE ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Set_ChangeMapNum
//	DESC		: npc�� ���� �̵��� ���� �� ��ȣ�� �����ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 21, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMoveable::Set_ChangeMapNum(MAPTYPE mapNum) 
{
	// ���ڷ� �Ѿ���� �� ��ȣ�� Ȯ���Ѵ�.
	if( mapNum == 0 )
	{
		Throw_Error("Invalid map number!!", __FUNCTION__) ;
		return ;
	}


	// ��ȯ �� �� ��ȣ�� �����Ѵ�.
	m_ChangeMapNum = mapNum ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Set_ChangeMapXPos
//	DESC		: npc�� ���� �̵��� ���� x��ǥ�� �����ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 21, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMoveable::Set_ChangeMapXPos(float fXpos) 
{
	// ���ڷ� �Ҿ���� ��ȯ ��ǥ�� Ȯ���Ѵ�.
	if( fXpos <= 0.0f )
	{
		Throw_Error("Invalid recall position!!", __FUNCTION__) ;
		return ;
	}


	// ��ȯ �� ��ǥ�� �����Ѵ�.
	m_fChangeMapXPos = fXpos ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Set_ChangeMapZPos
//	DESC		: npc�� ���� �̵��� ���� z��ǥ�� �����ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 21, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMoveable::Set_ChangeMapZPos(float fZpos) 
{
	// ���ڷ� �Ҿ���� ��ȯ ��ǥ�� Ȯ���Ѵ�.
	if( fZpos <= 0.0f )
	{
		Throw_Error("Invalid recall position!!", __FUNCTION__) ;
		return ;
	}


	// ��ȯ �� ��ǥ�� �����Ѵ�.
	m_fChangeMapZPos = fZpos ;
}













