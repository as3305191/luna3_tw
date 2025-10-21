// MHTimeManager.h: interface for the CMHTimeManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MHTIMEMANAGER_H__890F483A_4FB0_4A20_91BF_BD8BD48941A0__INCLUDED_)
#define AFX_MHTIMEMANAGER_H__890F483A_4FB0_4A20_91BF_BD8BD48941A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// 090907 ShinJS --- 5�� ��ã�� Source ����� �߰�. ��ã�� ���� ���
#define MHTIMEMGR	USINGTON(CMHTimeManager)
#define HTR_S(x)	USINGTON(CMHTimeManager)->RecordTime_Start(x);
#define HTR_E(x)	USINGTON(CMHTimeManager)->RecordTime_End(x);


enum eRecordType	
{
	// �ð������ ���ÿ� ���� ������ �Ҽ� �ֵ��� Ÿ���� �ξ
	// ���ϴ� Ÿ���� �ð��� ���� �Ѵ�.
	eRT_OneLineCheck,		// �������� �̵������Ѱ�
	eRT_InputPoint,
	eRT_PathFind_Astar,
	eRT_MakeAStarPath,
	eRT_Optimize,
	eRT_Optimize1,
	eRT_Optimize2,
	eRT_AStarTotal,
	eRT_PathManagerLoad,

	eRT_NUMBER_TYPE,
};

class CMHTimeManager  
{
	DWORD m_MHDate;
	DWORD m_MHTime;
	
	DWORD m_lasttime;

	int m_ServerTimeTick;
	__time64_t m_ServerTime;

public:
//	//MAKESINGLETON(CMHTimeManager);
	
	CMHTimeManager();
	virtual ~CMHTimeManager();

	void Init(DWORD mhDate,DWORD mhTime);
	void Process();

	DWORD GetMHDate();
	DWORD GetMHTime();
	
	void GetMHDate(BYTE& year,BYTE& month,BYTE& day);
	void GetMHTime(BYTE& hour,BYTE& minute);

	DWORD GetNewCalcCurTime();

	void SetServerTime( __time64_t serverTime );
	__time64_t GetServerTime() const { return m_ServerTime; }

	///--------------------------------------------------
	/// �������ϸ� - Ư�� ������ �ð��� ��� ���� �Լ���
	/// �ܼ� �ð�, �������, �ּ� �ִ븦 �� �� �ִ�.
	///--------------------------------------------------
private:
	LARGE_INTEGER		m_StartCount[eRT_NUMBER_TYPE];
	LARGE_INTEGER		m_LastCount[eRT_NUMBER_TYPE];
	LARGE_INTEGER		m_ElaspedCount[eRT_NUMBER_TYPE];
	LARGE_INTEGER		m_ElaspedCountAccum[eRT_NUMBER_TYPE];
	int					m_AccumNumber[eRT_NUMBER_TYPE];
	LARGE_INTEGER		m_Frequency[eRT_NUMBER_TYPE];

	double				m_StartTime[eRT_NUMBER_TYPE];
	double				m_LastTime[eRT_NUMBER_TYPE];
	double				m_ElaspedTime[eRT_NUMBER_TYPE];			// ��ϵ� �ð�
	double				m_ElaspedTime_Accum[eRT_NUMBER_TYPE];	// ��ϵ� ���� �ð�
	double				m_MinimizeTime[eRT_NUMBER_TYPE];		// ��ϵ� �ּҽð�
	double				m_MaximizeTime[eRT_NUMBER_TYPE];		// ��ϵ� �ִ�ð�.

public:
	void		RecordTime_Start(eRecordType eType);			// �ð� ��� ����
	void		RecordTime_End(eRecordType eType);				// �ð� ��� ��
	double		GetRecordTime(eRecordType eType);				// ����� �ð� ����(ms)
	double		GetRecordTime_Sec(eRecordType eType);			// ����� �ð� ����(Sec)
	double		GetRecordTime_Accum(eRecordType eType);			// ���� �ð�
	double		GetRecordTime_Min(eRecordType eType);
	double		GetRecordTime_Max(eRecordType eType);
	void		ResetAccumulation(eRecordType eType);			// ��ȭ���� ���߳����Ѱ�찡 ������ �����Ƿ� �������� �������� �ʿ䵵 �ִ�
	void		DrawStatus();									// ���� �����͸� �׸���
	void		SetDrawStatus( BOOL bDrawStatus )				{ m_bDrawStatus = bDrawStatus; }
private:

private:
	LARGE_INTEGER		m_StartTime_g_Curtime;
	LARGE_INTEGER		m_NowTime_g_Curtime;
	LARGE_INTEGER		m_Frequency_g_Curtime;
	BOOL				m_bDrawStatus;

	void	SetStartTime_g_Curtime();
	DWORD	GetCurrentTime_g_Curtime();	// DWORD�μ� ���α׷��� ���۵� ���� ��(ms)


public:
	static CMHTimeManager* GetInstance()	{static CMHTimeManager gInstance;return &gInstance;}
	bool  CheckTimer500();
};
EXTERNGLOBALTON(CMHTimeManager)
#endif // !defined(AFX_MHTIMEMANAGER_H__890F483A_4FB0_4A20_91BF_BD8BD48941A0__INCLUDED_)
