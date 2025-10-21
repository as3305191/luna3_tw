//-------------------------------------------------------------------------------------------------
//	NAME		: CNpcRecallSelf.cpp
//	DESC		: Implementation part of CNpcRecallSelf class.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------
//		Include part.
//-------------------------------------------------------------------------------------------------
#include "stdafx.h"																						// An include file for standard system include files

#include "./NpcRecallSelf.h"





//-------------------------------------------------------------------------------------------------
//	NAME		: CNpcRecallSelf
//	DESC		: ������ �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
CNpcRecallSelf::CNpcRecallSelf(void)
{
	Set_ParentNpcType( eParentNpcType_OnlySelf ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: ~CNpcRecallSelf
//	DESC		: �Ҹ��� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
CNpcRecallSelf::~CNpcRecallSelf(void)
{
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Insert_TimeTable
//	DESC		: Npc ��ȯ/�Ҹ� �ð��� ��� �ִ� ���� �߰� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallSelf::Insert_TimeTable(st_RECALL_TIMETABLE* pInfo)
{
	// ���ڷ� �Ѿ�� ���� ��ȿ Ȯ��.
	if( !pInfo )
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// ���� �ε��� ���̺��� �ִ��� Ȯ���Ѵ�.
	MAP_TIMETABLE::iterator it ;
	for( it = m_mTimeTable.begin() ; it != m_mTimeTable.end() ; ++it )
	{
		if( it->second.byTableIdx == pInfo->byTableIdx )
		{
			Throw_Error("Failed to insert time table!!\\[Same time table index]", __FUNCTION__) ;
			return ;
		}
	}


	// ���̺��� �߰��Ѵ�.
	m_mTimeTable.insert( std::make_pair( pInfo->byTableIdx, *pInfo ) ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Get_TimeTable
//	DESC		: Npc ��ȯ/�Ҹ� �ð��� ��ȯ�ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
st_RECALL_TIMETABLE* CNpcRecallSelf::Get_TimeTable(BYTE byIdx)
{
	// ��ġ�ϴ� ���̺��� �ִ��� Ȯ���Ѵ�.
	MAP_TIMETABLE::iterator it ;
	for( it = m_mTimeTable.begin() ; it != m_mTimeTable.end() ; ++it )
	{
		if( it->second.byTableIdx == byIdx )
		{
			return &it->second ;
		}
	}


	// null�� return.
	return NULL ;
}

void CNpcRecallSelf::Copy(CNpcRecallSelf* pSrc)
{
	memcpy(this , pSrc , sizeof( CNpcRecallBase )  ) ;

	MAP_TIMETABLE::iterator it ;
	for( it = pSrc->m_mTimeTable.begin() ; it != pSrc->m_mTimeTable.end() ; ++it )
	{
		this->m_mTimeTable.insert( std::make_pair( it->second.byTableIdx, it->second ) ) ;
	}
}















