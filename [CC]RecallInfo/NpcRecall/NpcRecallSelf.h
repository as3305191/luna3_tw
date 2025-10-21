#pragma once
//-------------------------------------------------------------------------------------------------
//	NAME		: CNpcRecallSelf.h
//	DESC		: ��ü �ð� ���̺��� ������ �־�, ������ ��ȯ / �Ҹ��� �ϴ� npc Ŭ����.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------
//		Include part.
//-------------------------------------------------------------------------------------------------
#include <map>

#include "./NpcRecallBase.h"





//-------------------------------------------------------------------------------------------------
//		Define some macroinstruction.
//-------------------------------------------------------------------------------------------------
struct st_RECALL_TIMETABLE								// Npc�� ��ȯ/�Ҹ� �ð� ������ ���� ����ü ����.
{
	BYTE byTableIdx ;									// ���̺� �ε���.

	WORD wStartDay ;									// ���� ��¥.
	WORD wStartHour ;									// ���� �ð�.
	WORD wStartMinute ;									// ���� ��.

	WORD wEndDay ;										// ���� ��¥.
	WORD wEndHour ;										// ���� �ð�.
	WORD wEndMinute ;									// ���� ��.
} ;




//-------------------------------------------------------------------------------------------------
//		The class CNpcRecallSelf.
//-------------------------------------------------------------------------------------------------
class CNpcRecallSelf : public CNpcRecallBase
{
	typedef std::map< BYTE, st_RECALL_TIMETABLE >		MAP_TIMETABLE ;				// �� �����̳ʿ� ���� �ε����� ������ ����.
	MAP_TIMETABLE										m_mTimeTable ;				// ��ȯ �ð� ���̺��� ���� �� �����̳�.

public:
	CNpcRecallSelf(void);
	virtual ~CNpcRecallSelf(void);
	BYTE Get_TableCount() { return BYTE(m_mTimeTable.size()); }
	void Insert_TimeTable(st_RECALL_TIMETABLE*);
	st_RECALL_TIMETABLE* Get_TimeTable(BYTE byIdx);
	void Copy(CNpcRecallSelf*);
};
