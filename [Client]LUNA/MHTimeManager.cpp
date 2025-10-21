// MHTimeManager.cpp: implementation of the CMHTimeManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MHTimeManager.h"
#include "mmsystem.h"
#include "./interface/cFont.h"
#include "ObjectManager.h"
#include "QuestManager.h"
#include "GameResourceManager.h"

DWORD gCurTime = 0;		//0 �ʱ�ȭ �߰�...
DWORD gTickTime;
float gAntiGravity = 1.0f;

#define TICK_PER_DAY 86400000		// 24 * 60 * 60 * 1000

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
GLOBALTON(CMHTimeManager)
CMHTimeManager::CMHTimeManager()
{
	m_MHDate = 0;
	m_MHTime = 0;

	m_lasttime = 0;
	ZeroMemory(
		m_StartCount,
		sizeof(m_StartCount));
	ZeroMemory(
		m_LastCount,
		sizeof(m_LastCount));
	ZeroMemory(
		m_ElaspedCount,
		sizeof(m_ElaspedCount));
	ZeroMemory(
		m_ElaspedCountAccum,
		sizeof(m_ElaspedCountAccum));
	ZeroMemory(
		m_AccumNumber,
		sizeof(m_AccumNumber));
	ZeroMemory(
		m_Frequency,
		sizeof(m_Frequency));
	ZeroMemory(
		m_StartTime,
		sizeof(m_StartTime));
	ZeroMemory(
		m_LastTime,
		sizeof(m_LastTime));
	ZeroMemory(
		m_ElaspedTime,
		sizeof(m_ElaspedTime));
	ZeroMemory(
		m_ElaspedTime_Accum,
		sizeof(m_ElaspedTime_Accum));

	// -1�� �ʱ�ȭ�ؾߵ�
	for ( int i = 0 ; i < eRT_NUMBER_TYPE ; ++i )
		m_MinimizeTime[i] = m_MaximizeTime[i] = -1.0;

	// ���� ���� �����̶� ������ ���� �� �ִ�.
	// ���۽ð����� ������� ����ϴ� ���
	SetStartTime_g_Curtime();

	m_bDrawStatus = FALSE;

	m_ServerTimeTick = 0;
	_time64( &m_ServerTime );
}

CMHTimeManager::~CMHTimeManager()
{

}

void CMHTimeManager::Init(DWORD mhDate,DWORD mhTime)
{
	m_MHDate = mhDate;
	m_MHTime = mhTime;
}

void CMHTimeManager::Process()
{
	static bool bFirst = true;
	static DWORD curtime = 0;
	static int tempDay = 0;

	if( bFirst )
	{
		curtime = m_lasttime = GetTickCount();
		bFirst = false;
	}
	else
	{
		curtime = GetTickCount();		

		if( curtime < m_lasttime )		// DWORD ���� �Ѱ踦 �Ѿ��
			gTickTime = curtime - m_lasttime + 4294967295; //( 2^32 - 1 )
		else
			gTickTime = curtime - m_lasttime;
		m_lasttime = curtime;
		
		gCurTime += gTickTime;

		
		//////////////////////////////////////////////////////////////////////////
		// �����-_-a; 
		// ����� ������� �ʾ� ���ǰ� ���� �ð�� ����
		m_MHTime += gTickTime;
		if(m_MHTime >= TICK_PER_DAY)
		{
			++m_MHDate;
			m_MHTime -= TICK_PER_DAY;
		}
	}

	// 100408 ShinJS --- ���� �ð� ����
	m_ServerTimeTick += (int)gTickTime;
	int sec = int( m_ServerTimeTick / 1000 );
	m_ServerTimeTick = m_ServerTimeTick % 1000;
	m_ServerTime += sec;
}

DWORD CMHTimeManager::GetNewCalcCurTime()	//curŸ�� ���� �޾ƿ��� ���� gCurTime, gTickTime�� �������.
{
	DWORD lcurtime = GetTickCount();
	DWORD lTickTime;
		
	if( lcurtime < m_lasttime )		// DWORD ���� �Ѱ踦 �Ѿ��
		lTickTime = lcurtime - m_lasttime + 4294967295; //( 2^32 - 1 )
	else
		lTickTime = lcurtime - m_lasttime;
	
	return gCurTime + lTickTime;
}

DWORD CMHTimeManager::GetMHDate()
{
	return m_MHDate;
}

DWORD CMHTimeManager::GetMHTime()
{
	return m_MHTime;
}

void CMHTimeManager::GetMHDate(BYTE& year,BYTE& month,BYTE& day)
{
	year = (BYTE)(m_MHDate / 360) + 1;
	month = (BYTE)((m_MHDate - year) / 30) + 1;
	day = (BYTE)m_MHDate % 30 + 1;  // -_-a; ���⿡�� ��� ���� 30�ϱ����� -_-	
}

void CMHTimeManager::GetMHTime(BYTE& hour,BYTE& minute)
{
	hour = BYTE(m_MHTime / 3600000);
	minute = BYTE((m_MHTime - hour) / 60000);
}

///--------------------------------------------------
/// Ư�� ������ �ð��� ��� ���� �Լ�
/// �ܼ� �ð�, �������, �ּ� �ִ븦 �� �� �ִ�.
///--------------------------------------------------
void CMHTimeManager::RecordTime_Start( eRecordType eType )
{
#ifdef _GMTOOL_
	assert( 0 <= eType );
	assert( eRT_NUMBER_TYPE > eType );

	// ���� ī��Ʈ ���
	QueryPerformanceCounter(&m_StartCount[eType]);

	// ��������(�Ƹ��� �ػ�) ���
	// �̰��� 0�� ������ �� �ػ� �Լ��� �� �� ���� �ý����̶��Ŵ�
	QueryPerformanceFrequency(&m_Frequency[eType]);
	if ( m_Frequency[eType].QuadPart == 0 ) HTRACE( "QueryPerformanceFrequency�� 0�Դϴ�!" );
#endif
}

void CMHTimeManager::RecordTime_End( eRecordType eType )
{
#ifdef _GMTOOL_
	assert( 0 <= eType );
	assert( eRT_NUMBER_TYPE > eType );

	// �ֱ� ī��Ʈ ���
	QueryPerformanceCounter(&m_LastCount[eType]);

	// ������ ī��Ʈ�� ����
	m_ElaspedCount[eType].QuadPart	=	m_LastCount[eType].QuadPart	 - m_StartCount[eType].QuadPart;

	// ������Ŵ
	m_ElaspedCountAccum[eType].QuadPart += m_ElaspedCount[eType].QuadPart;
	// ���� Ƚ�� 
	m_AccumNumber[eType]++;


	// �ּ� �ִ�ð��� �������ش�.
	if ( m_MinimizeTime[eType] < 0.0 )
	{
		// ó�� �ð��� ����̸� �׳� �����Ѵ�.
		m_MinimizeTime[eType] = m_MaximizeTime[eType] = GetRecordTime(eType);
	}
	else
	{
		double		ElapsedTime = GetRecordTime(eType);
		m_MinimizeTime[eType] = min(ElapsedTime, m_MinimizeTime[eType]);
		m_MaximizeTime[eType] = max(ElapsedTime, m_MaximizeTime[eType]);		
	}
#endif
}

double CMHTimeManager::GetRecordTime( eRecordType eType )
{
	assert( 0 <= eType );
	assert( eRT_NUMBER_TYPE > eType );

	// ���� �ð� ���
	if (m_Frequency[eType].QuadPart == 0) return 0;
	m_ElaspedTime[eType]	=	(double)(m_ElaspedCount[eType].QuadPart)  /  (double)(m_Frequency[eType].QuadPart);

	return m_ElaspedTime[eType] * 1000;
}

double CMHTimeManager::GetRecordTime_Sec( eRecordType eType )
{
	assert( 0 <= eType );
	assert( eRT_NUMBER_TYPE > eType );

	if (m_Frequency[eType].QuadPart == 0) return 0;
	m_ElaspedTime[eType]	=	(double)(m_ElaspedCount[eType].QuadPart)  /  (double)(m_Frequency[eType].QuadPart);

	return m_ElaspedTime[eType];
}

double CMHTimeManager::GetRecordTime_Accum( eRecordType eType )
{
	assert( 0 <= eType );
	assert( eRT_NUMBER_TYPE > eType );

	if (m_Frequency[eType].QuadPart == 0) return 0;
	m_ElaspedTime_Accum[eType] = (double)(m_ElaspedCountAccum[eType].QuadPart) / (double)(m_Frequency[eType].QuadPart) / (double)m_AccumNumber[eType];

	return m_ElaspedTime_Accum[eType] * 1000;
}

double	CMHTimeManager::GetRecordTime_Min(eRecordType eType)
{
	return m_MinimizeTime[eType];
}

double	CMHTimeManager::GetRecordTime_Max(eRecordType eType)
{
	return m_MaximizeTime[eType];
}

void CMHTimeManager::ResetAccumulation( eRecordType eType )
{
	assert( 0 <= eType );
	assert( eRT_NUMBER_TYPE > eType );

	m_ElaspedCountAccum[eType].QuadPart = 0;
	m_ElaspedTime_Accum[eType] = 0;
	m_AccumNumber[eType] = 0;
	m_MinimizeTime[eType] = -1;
	m_MaximizeTime[eType] = -1;
}


// ����ü�� �׸���
void CMHTimeManager::DrawStatus()
{
#ifdef _GMTOOL_

	if( !m_bDrawStatus )
		return;

	char temp[ 128 ];
	RECT rc;
	rc.left = 220;
	rc.right = 1220;
	rc.top = 20 * 3;
	rc.bottom = rc.top + 20;

	/// millisecond������ ��� QueryPerformanceCounter�̿� ������ �͵�...
	sprintf( temp, "OneLineCheck     : %7.4fms", GetRecordTime( eRT_OneLineCheck ) );
	rc.top = 20 * 3;	rc.bottom = rc.top + 20;
	CFONT_OBJ->RenderFont( 0, temp, strlen(temp), &rc, RGBA_MAKE(255,255,255,200));
	sprintf( temp, "OneLineCheck Avg : %7.4fms", GetRecordTime_Accum( eRT_OneLineCheck ) );
	rc.top = 20 * 4;	rc.bottom = rc.top + 20;
	CFONT_OBJ->RenderFont( 0, temp, strlen(temp), &rc, RGBA_MAKE(255,255,255,200));

	sprintf( temp, "InputPoint     : %7.4fms", GetRecordTime( eRT_InputPoint ) );
	rc.top = 20 * 5;	rc.bottom = rc.top + 20;
	CFONT_OBJ->RenderFont( 0, temp, strlen(temp), &rc, RGBA_MAKE(255,255,255,200));
	sprintf( temp, "InputPoint Avg : %7.4fms", GetRecordTime_Accum( eRT_InputPoint ) );
	rc.top = 20 * 6;	rc.bottom = rc.top + 20;
	CFONT_OBJ->RenderFont( 0, temp, strlen(temp), &rc, RGBA_MAKE(255,255,255,200));

	sprintf( temp, "PathFind_Astar     : %7.4fms", GetRecordTime( eRT_PathFind_Astar ) );
	rc.top = 20 * 7;	rc.bottom = rc.top + 20;
	CFONT_OBJ->RenderFont( 0, temp, strlen(temp), &rc, RGBA_MAKE(255,255,255,200));
	sprintf( temp, "PathFind_Astar Avg : %7.4fms", GetRecordTime_Accum( eRT_PathFind_Astar ) );
	rc.top = 20 * 8;	rc.bottom = rc.top + 20;
	CFONT_OBJ->RenderFont( 0, temp, strlen(temp), &rc, RGBA_MAKE(255,255,255,200));

	sprintf( temp, "MakeAStarPath     : %7.4fms", GetRecordTime( eRT_MakeAStarPath ) );
	rc.top = 20 * 9;	rc.bottom = rc.top + 20;
	CFONT_OBJ->RenderFont( 0, temp, strlen(temp), &rc, RGBA_MAKE(255,255,255,200));
	sprintf( temp, "MakeAStarPath Avg : %7.4fms", GetRecordTime_Accum( eRT_MakeAStarPath ) );
	rc.top = 20 * 10;	rc.bottom = rc.top + 20;
	CFONT_OBJ->RenderFont( 0, temp, strlen(temp), &rc, RGBA_MAKE(255,255,255,200));

	sprintf( temp, "Optimize     : %7.4fms", GetRecordTime( eRT_Optimize ) );
	rc.top = 20 * 11;	rc.bottom = rc.top + 20;
	CFONT_OBJ->RenderFont( 0, temp, strlen(temp), &rc, RGBA_MAKE(255,255,255,200));
	sprintf( temp, "Optimize Avg : %7.4fms", GetRecordTime_Accum( eRT_Optimize ) );
	rc.top = 20 * 12;	rc.bottom = rc.top + 20;
	CFONT_OBJ->RenderFont( 0, temp, strlen(temp), &rc, RGBA_MAKE(255,255,255,200));

	sprintf( temp, "Optimize1     : %7.4fms", GetRecordTime( eRT_Optimize1 ) );
	rc.top = 20 * 13;	rc.bottom = rc.top + 20;
	CFONT_OBJ->RenderFont( 0, temp, strlen(temp), &rc, RGBA_MAKE(255,255,255,200));
	sprintf( temp, "Optimize1 Avg : %7.4fms", GetRecordTime_Accum( eRT_Optimize1 ) );
	rc.top = 20 * 14;	rc.bottom = rc.top + 20;
	CFONT_OBJ->RenderFont( 0, temp, strlen(temp), &rc, RGBA_MAKE(255,255,255,200));

	sprintf( temp, "Optimize2     : %7.4fms", GetRecordTime( eRT_Optimize2 ) );
	rc.top = 20 * 15;	rc.bottom = rc.top + 20;
	CFONT_OBJ->RenderFont( 0, temp, strlen(temp), &rc, RGBA_MAKE(255,255,255,200));
	sprintf( temp, "Optimize2 Avg : %7.4fms", GetRecordTime_Accum( eRT_Optimize2 ) );
	rc.top = 20 * 16;	rc.bottom = rc.top + 20;
	CFONT_OBJ->RenderFont( 0, temp, strlen(temp), &rc, RGBA_MAKE(255,255,255,200));


	rc.left = 430;
	rc.right = 1430;

	sprintf( temp, "AStarTotal     : %7.4fms", GetRecordTime( eRT_AStarTotal ) );
	rc.top = 20 * 11;	rc.bottom = rc.top + 20;
	CFONT_OBJ->RenderFont( 0, temp, strlen(temp), &rc, RGBA_MAKE(255,255,255,200));
	sprintf( temp, "AStarTotal Avg : %7.4fms", GetRecordTime_Accum( eRT_AStarTotal ) );
	rc.top = 20 * 12;	rc.bottom = rc.top + 20;
	CFONT_OBJ->RenderFont( 0, temp, strlen(temp), &rc, RGBA_MAKE(255,255,255,200));

	sprintf( temp, "PathManagerLoad     : %7.4fms", GetRecordTime( eRT_PathManagerLoad ) );
	rc.top = 20 * 14;	rc.bottom = rc.top + 20;
	CFONT_OBJ->RenderFont( 0, temp, strlen(temp), &rc, RGBA_MAKE(255,255,255,200));
	sprintf( temp, "PathManagerLoad Avg : %7.4fms", GetRecordTime_Accum( eRT_PathManagerLoad ) );
	rc.top = 20 * 15;	rc.bottom = rc.top + 20;
	CFONT_OBJ->RenderFont( 0, temp, strlen(temp), &rc, RGBA_MAKE(255,255,255,200));

	// ���ΰ� ��ġ ���
	if( HERO )
	{
		VECTOR3 pos;
		HERO->GetPosition( &pos );
		sprintf( temp, "Hero Pos : %7.4f, %7.4f, %7.4f", pos.x, pos.y, pos.z );
		rc.left = 10;		rc.right = 1010;
		rc.top = 20 * 21;	rc.bottom = rc.top + 20;
		CFONT_OBJ->RenderFont( 0, temp, strlen(temp), &rc, 0xFF00FF00);
	}

#endif // _GMTOOL_

}


void CMHTimeManager::SetStartTime_g_Curtime()
{
	// ���� ī��Ʈ ���
	QueryPerformanceCounter(&m_StartTime_g_Curtime);

	// ��������(�Ƹ��� �ػ�) ���
	// �̰��� 0�� ������ �� �ػ� �Լ��� �� �� ���� �ý����̶��Ŵ�
	QueryPerformanceFrequency(&m_Frequency_g_Curtime);
	if ( m_Frequency_g_Curtime.QuadPart == 0 ) HTRACE( "QueryPerformanceFrequency�� 0�Դϴ�!" );
}

DWORD CMHTimeManager::GetCurrentTime_g_Curtime()
{
	// �ֱ� ī��Ʈ ���
	QueryPerformanceCounter(&m_NowTime_g_Curtime);

	// ������ ī��Ʈ�� ����
	double t_ElapsedTime = (double)(m_NowTime_g_Curtime.QuadPart - m_StartTime_g_Curtime.QuadPart) / (double)m_Frequency_g_Curtime.QuadPart;
	
	return (DWORD)(t_ElapsedTime * 1000);
}

void CMHTimeManager::SetServerTime( __time64_t serverTime )
{
	m_ServerTimeTick = 0;
	m_ServerTime = serverTime;

	// ���� �ð��� ����ϴ� �۾����� �ʱ�ȭ ��Ų��.
	// �ð� ���� ����Ʈ �ʱ�ȭ
	QUESTMGR->InitDateTime();
}

bool CMHTimeManager::CheckTimer500()
{   
	DWORD TickByFps = GAMERESRCMNGR->m_GameDesc.FramePerSec;
	DWORD mTick = 0;
	if(TickByFps >= 30 && TickByFps < 60 )
	{
		mTick = 200;
	}
	else if (TickByFps >= 60)
	{
		mTick = 100;
	}
	static DWORD nTick = gCurTime;
	if (gCurTime-nTick < mTick) return FALSE;
	nTick = gCurTime;
	return TRUE;
}