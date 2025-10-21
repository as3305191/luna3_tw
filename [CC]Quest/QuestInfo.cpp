// QuestInfo.cpp: implementation of the CQuestInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"																	// ǥ�� �ý��۰�, ������Ʈ�� ������, ���־��� �ش����� ���� �ش������� �ҷ��´�.
#include "QuestInfo.h"																// ����Ʈ ���� ��������� �ҷ��´�.

#include "SubQuestInfo.h"															// ���� ����Ʈ ���� ��� ������ �ҷ��´�.

#ifdef _CLIENT_
	#include "MHTimeManager.h"
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQuestInfo::CQuestInfo( DWORD dwQuestIdx )											// ������ �Լ�.
{
	m_dwQuestIdx = dwQuestIdx;														// ����Ʈ �ε����� �����Ѵ�.

	m_dwSubQuestCount = 0;															// ���� ����Ʈ ī��Ʈ�� 0���� �����Ѵ�.
	m_dwEndParam = 0;																// ���� �Ķ���͸� 0���� �����Ѵ�.
	m_SubQuestTable.Initialize( MAX_SUBQUEST );										// ���� ����Ʈ ���� ���̺��� �ִ� ���� ����Ʈ ������ �ʱ�ȭ �Ѵ�.

	m_bRepeat = FALSE;
}

CQuestInfo::~CQuestInfo()															// �Ҹ��� �Լ�.
{
	Release();																		// ���� �Լ��� ȣ���Ѵ�.
}

void CQuestInfo::Release()															// ���� �Լ�.
{
	CSubQuestInfo* pSubQuestInfo = NULL;											// ���� ����Ʈ ������ ���� ������ ������ �����ϰ�, null ó���� �Ѵ�.

	m_SubQuestTable.SetPositionHead();												// ���� ����Ʈ ���̺��� ���� �����Ѵ�.

	while( (pSubQuestInfo = m_SubQuestTable.GetData()) != NULL)					// ���� ����Ʈ ������ �ִ� ���� while ���� ������.
	{
		delete pSubQuestInfo;														// ���� ����Ʈ ������ �����Ѵ�.
	}

	m_SubQuestTable.RemoveAll();													// ���� ����Ʈ ���� ���̺��� ��� ����.
}

void CQuestInfo::AddSubQuestInfo( DWORD dwSubQuestIdx, CSubQuestInfo* pSubQuest )	// ���� ����Ʈ ������ �߰��ϴ� �Լ�.
{
	if( dwSubQuestIdx >= MAX_SUBQUEST ) return;										// ���� ����Ʈ �ε�����, �ִ� ���� ����Ʈ �ε����� �Ѿ��, ���� ó���� �Ѵ�.

	m_SubQuestTable.Add( pSubQuest, dwSubQuestIdx );								// ���� ����Ʈ ���� ���̺� ���� ����Ʈ�� �߰��Ѵ�.
	
	++m_dwSubQuestCount;															// ���� ����Ʈ ī���͸� �����Ѵ�.
}

BOOL CQuestInfo::IsValidDateTime() const
{
	if( m_DateLimitList.empty() )
		return TRUE;

	// ���� �ð� ���ϱ�(�����ð����� ���)
	__time64_t curTime=0;
	struct tm curTimeWhen={0,};

#ifdef _CLIENT_
	curTime = MHTIMEMGR->GetServerTime();
#else
	_time64( &curTime );
#endif

	curTimeWhen = *_localtime64( &curTime );
	
	for( std::vector< QuestDateLimitData >::const_iterator iterList = m_DateLimitList.begin() ; iterList != m_DateLimitList.end() ; ++iterList )
	{
		const struct tm& scriptStartTimeWhen = iterList->first;
		const struct tm& scriptEndTimeWhen = iterList->second;

		struct tm compareStartTimeWhen = curTimeWhen;
		struct tm compareEndTimeWhen = curTimeWhen;

		// ��/�� ��:��
		if( scriptStartTimeWhen.tm_mon >= 0 )				compareStartTimeWhen.tm_mon = scriptStartTimeWhen.tm_mon;
		if( scriptStartTimeWhen.tm_mday > 0 )				compareStartTimeWhen.tm_mday = scriptStartTimeWhen.tm_mday;
		if( scriptStartTimeWhen.tm_hour > 0 )
		{
			compareStartTimeWhen.tm_hour = scriptStartTimeWhen.tm_hour;
			compareStartTimeWhen.tm_min = 0;
			compareStartTimeWhen.tm_sec = 0;
		}
		if( scriptStartTimeWhen.tm_min > 0 )
		{
			compareStartTimeWhen.tm_min = scriptStartTimeWhen.tm_min;
			compareStartTimeWhen.tm_sec = 0;
		}

		// ��/�� ��:��
		if( scriptEndTimeWhen.tm_mon >= 0 )				compareEndTimeWhen.tm_mon = scriptEndTimeWhen.tm_mon;
		if( scriptEndTimeWhen.tm_mday > 0 )				compareEndTimeWhen.tm_mday = scriptEndTimeWhen.tm_mday;
		if( scriptEndTimeWhen.tm_hour > 0 )
		{
			compareEndTimeWhen.tm_hour = scriptEndTimeWhen.tm_hour;
			compareEndTimeWhen.tm_min = 0;
			compareEndTimeWhen.tm_sec = 0;
		}
		if( scriptEndTimeWhen.tm_min > 0 )
		{
			compareEndTimeWhen.tm_min = scriptEndTimeWhen.tm_min;
			compareEndTimeWhen.tm_sec = 0;
		}

		// ����
		const int scriptStartWeekday = scriptStartTimeWhen.tm_wday;
		const int scriptEndWeekday = ( scriptStartTimeWhen.tm_wday > scriptEndTimeWhen.tm_wday ? scriptEndTimeWhen.tm_wday + 7 : scriptEndTimeWhen.tm_wday );
		const int curWeekday = ( scriptEndWeekday > 6 ? curTimeWhen.tm_wday + 7 : curTimeWhen.tm_wday );
		if( scriptStartWeekday >= 0 )
		{
			int day = scriptStartWeekday - curWeekday;
			day += ( scriptEndWeekday < curWeekday ? 7 : 0 );
			
			compareStartTimeWhen.tm_mday += day;
		}
		if( scriptEndWeekday >= 0 )
		{
			int day = scriptEndWeekday - curWeekday;
			day += ( scriptEndWeekday < curWeekday ? 7 : 0 );
			compareEndTimeWhen.tm_mday += day;
		}

		compareStartTimeWhen.tm_sec = compareEndTimeWhen.tm_sec = 0;
		__time64_t compareStartTime = _mktime64( &compareStartTimeWhen );
		__time64_t compareEndTime = _mktime64( &compareEndTimeWhen );

		if( compareStartTime <= curTime && curTime <= compareEndTime )
		{
			return TRUE;
		}
	}
	
	return FALSE;
}

__time64_t CQuestInfo::GetNextStartDateTime( __time64_t curTime ) const
{
	// ���� �ð� ���ϱ�(�����ð����� ���)
	if( curTime == 0 )
	{
#ifdef _CLIENT_
		curTime = MHTIMEMGR->GetServerTime();
#else
		_time64( &curTime );
#endif
	}

	struct tm curTimeWhen = *_localtime64( &curTime );
	__time64_t nextTime=0;

	for( std::vector< QuestDateLimitData >::const_iterator iterList = m_DateLimitList.begin() ; iterList != m_DateLimitList.end() ; ++iterList )
	{
		const struct tm& scriptStartTimeWhen = iterList->first;
		const struct tm& scriptEndTimeWhen = iterList->second;

		struct tm startTimeWhen = curTimeWhen;
		startTimeWhen.tm_hour = startTimeWhen.tm_min = startTimeWhen.tm_sec = 0;

		const int scriptStartWeekday = scriptStartTimeWhen.tm_wday;
		const int scriptEndWeekday = ( scriptStartTimeWhen.tm_wday > scriptEndTimeWhen.tm_wday ? scriptEndTimeWhen.tm_wday + 7 : scriptEndTimeWhen.tm_wday );
		const int curWeekday = ( scriptEndWeekday > 6 ? curTimeWhen.tm_wday + 7 : curTimeWhen.tm_wday );

		// ����
		if( scriptStartWeekday >= 0 )
		{
			int day = scriptStartWeekday - curWeekday;
			day += ( scriptEndWeekday <= curWeekday ? 7 : 0 );
			day %= 7;
			
			startTimeWhen.tm_mday += day;
		}

		// ��
		if( scriptStartTimeWhen.tm_min > 0 )		startTimeWhen.tm_min = scriptStartTimeWhen.tm_min;
		// ��
		if( scriptStartTimeWhen.tm_hour > 0 )		startTimeWhen.tm_hour = scriptStartTimeWhen.tm_hour;
		// ��
		if( scriptStartTimeWhen.tm_mday > 0 )		startTimeWhen.tm_mday = scriptStartTimeWhen.tm_mday;
		// ��
		if( scriptStartTimeWhen.tm_mon > 0 )		startTimeWhen.tm_mon = scriptStartTimeWhen.tm_mon;

		__time64_t startTime = _mktime64( &startTimeWhen );

		// ����� ���
		if( curTime > startTime )
		{
			struct {
				void GetNextStartDate( const struct tm& scriptStartTimeWhen, const struct tm curTimeWhen, struct tm& startTimeWhen ) {
					// ��
					if( scriptStartTimeWhen.tm_mon > 0 && 
						( curTimeWhen.tm_mon > scriptStartTimeWhen.tm_mon ||
						( curTimeWhen.tm_mon == scriptStartTimeWhen.tm_mon && scriptStartTimeWhen.tm_mday > 0 && scriptStartTimeWhen.tm_mday > 0 && curTimeWhen.tm_mday ) ) )
					{
						++startTimeWhen.tm_year;
						return;
					}
					// ��
					if( scriptStartTimeWhen.tm_mday > 0 && 
						( curTimeWhen.tm_mday > scriptStartTimeWhen.tm_mday ||
						( curTimeWhen.tm_mday == scriptStartTimeWhen.tm_mday && scriptStartTimeWhen.tm_hour > 0 && curTimeWhen.tm_hour > scriptStartTimeWhen.tm_hour ) ) )
					{
						++startTimeWhen.tm_mon;
						return;
					}
					// ��
					if( scriptStartTimeWhen.tm_hour > 0 &&
						( curTimeWhen.tm_hour > scriptStartTimeWhen.tm_hour ||
						( curTimeWhen.tm_hour == scriptStartTimeWhen.tm_hour && scriptStartTimeWhen.tm_min > 0 && curTimeWhen.tm_min > scriptStartTimeWhen.tm_min ) ) )
					{
						++startTimeWhen.tm_mday;
						return;
					}
					// ��
					if( scriptStartTimeWhen.tm_min > 0 && curTimeWhen.tm_min > scriptStartTimeWhen.tm_min )
					{
						++startTimeWhen.tm_hour;
						return;
					}
				}
			}GETNEXTSTARTDATE;

			GETNEXTSTARTDATE.GetNextStartDate( scriptStartTimeWhen, curTimeWhen, startTimeWhen );
			startTime = _mktime64( &startTimeWhen );
		}

		if( curTime < startTime && ( nextTime == 0 || nextTime > startTime ) )
			nextTime = startTime;
	}

	return nextTime;
}

__time64_t CQuestInfo::GetNextEndDateTime( __time64_t curTime ) const
{
	// ���� �ð� ���ϱ�(�����ð����� ���)
	if( curTime == 0 )
	{
#ifdef _CLIENT_
		curTime = MHTIMEMGR->GetServerTime();
#else
		_time64( &curTime );
#endif
	}

	struct tm curTimeWhen = *_localtime64( &curTime );
	__time64_t nextTime=0;

	for( std::vector< QuestDateLimitData >::const_iterator iterList = m_DateLimitList.begin() ; iterList != m_DateLimitList.end() ; ++iterList )
	{
		const struct tm& scriptStartTimeWhen = iterList->first;
		const struct tm& scriptEndTimeWhen = iterList->second;

		struct tm endTimeWhen = curTimeWhen;
		endTimeWhen.tm_hour = endTimeWhen.tm_min = endTimeWhen.tm_sec = 0;

		const int scriptEndWeekday = ( scriptStartTimeWhen.tm_wday > scriptEndTimeWhen.tm_wday ? scriptEndTimeWhen.tm_wday + 7 : scriptEndTimeWhen.tm_wday );
		const int curWeekday = ( scriptEndWeekday > 6 ? curTimeWhen.tm_wday + 7 : curTimeWhen.tm_wday );
		
		// ����
		if( scriptEndWeekday >= 0 )
		{
			int day = scriptEndWeekday - curWeekday;
			day += ( scriptEndWeekday < curWeekday ? 7 : 0 );
			endTimeWhen.tm_mday += day;
		}

		// ��/�� ��:��
		if( scriptEndTimeWhen.tm_mon >= 0 )				endTimeWhen.tm_mon = scriptEndTimeWhen.tm_mon;
		if( scriptEndTimeWhen.tm_mday > 0 )				endTimeWhen.tm_mday = scriptEndTimeWhen.tm_mday;
		if( scriptEndTimeWhen.tm_hour > 0 )
		{
			endTimeWhen.tm_hour = scriptEndTimeWhen.tm_hour;
			endTimeWhen.tm_min = 0;
			endTimeWhen.tm_sec = 0;
		}
		if( scriptEndTimeWhen.tm_min > 0 )
		{
			endTimeWhen.tm_min = scriptEndTimeWhen.tm_min;
			endTimeWhen.tm_sec = 0;
		}

		__time64_t endTime = _mktime64( &endTimeWhen );
		if( curTime < endTime && ( nextTime == 0 || nextTime > endTime ) )
			nextTime = endTime;
	}

	return nextTime;
}