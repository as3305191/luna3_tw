//-------------------------------------------------------------------------------------------------
//	NAME		: CNpcRecallRemainTime.cpp
//	DESC		: Implementation part of CNpcRecallRemainTime class.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------
//		Include part.
//-------------------------------------------------------------------------------------------------
#include "stdafx.h"																						// An include file for standard system include files

#include "./NpcRecallRemainTime.h"





//-------------------------------------------------------------------------------------------------
//	NAME		: CNpcRecallRemainTime
//	DESC		: ������ �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
CNpcRecallRemainTime::CNpcRecallRemainTime(void)
{
	// ��� ������ �ʱ�ȭ �Ѵ�.
	m_dwRemainTime	= 0 ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: ~CNpcRecallRemainTime
//	DESC		: �Ҹ��� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
CNpcRecallRemainTime::~CNpcRecallRemainTime(void)
{
}





//-------------------------------------------------------------------------------------------------
//	NAME		: IsEndRemainTime
//	DESC		: ��ȯ�� ���� �� �ð��� �������� ��ȯ�ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
BYTE CNpcRecallRemainTime::IsEndRemainTime(DWORD dwCurTime)
{
	// ��ȯ ���� �ð��� �������� Ȯ���Ѵ�.
	DWORD dwCheckTime = dwCurTime - Get_RecallTime() ;

	if( dwCheckTime > m_dwRemainTime ) return TRUE ;
	else return FALSE ;

	// 1�� �ֱ�� Remain Time�� DB�� ������Ʈ �ұ�?
}















