#pragma once
//-------------------------------------------------------------------------------------------------
//	NAME		: CNpcRecallRemainTime.h
//	DESC		: �ѹ� ��ȯ �� ��, ���� �ð��� ������, ������ �Ҹ�Ǵ� npc Ŭ����.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------
//		Include part.
//-------------------------------------------------------------------------------------------------
#include "./NpcRecallBase.h"





//-------------------------------------------------------------------------------------------------
//		Define some macroinstruction.
//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------
//		The class CNpcRecallRemainTime.
//-------------------------------------------------------------------------------------------------
class CNpcRecallRemainTime : public CNpcRecallBase
{
	DWORD	m_dwRemainTime ;								// ��ȯ�� ���� �� �ð�.
	
public:
	CNpcRecallRemainTime(void);
	virtual ~CNpcRecallRemainTime(void);
	// ��ȯ�� ���� �� �ð��� ����/��ȯ �ϴ� �Լ�.
	void Set_RemainTime(DWORD dwRemainTime) { m_dwRemainTime = dwRemainTime ; }
	DWORD Get_RemainTime() { return m_dwRemainTime ; }


	// ��ȯ�� ���� �� �ð��� �������� ��ȯ�ϴ� �Լ�.
	BYTE IsEndRemainTime(DWORD dwCurTime) ;


	// ��ȯ�� ���� �� �ð��� �ʱ�ȭ �ϴ� �Լ�.
	void ClearRemainTime() { m_dwRemainTime = 0 ; }
};
