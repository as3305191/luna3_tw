#pragma once
#include "stdafx.h"

#include "./EFFECT/EffectManager.h"

#define FISHINGMGR USINGTON(CFishingManager)

extern DWORD g_FishingMissionCode[MAX_FISHINGMISSIONCODE];
extern char  g_FishingExpGrade[MAX_FISHING_LEVEL][32];

class CFishingManager
{
public:
	CFishingManager(void);
	virtual ~CFishingManager(void);

	void Init();
	void NetworkMsgParse(BYTE Protocol,void* pMsg);

	void Fishing_Ready_Ack(void* pMsg);
	void Fishing_Ready_Nack(void* pMsg);
	void Fishing_GetFish_Ack(void* pMsg);
	void Fishing_GetFish_Nack(void* pMsg);
	void Fishing_Mission_Update(void* pMsg);
	void Fishing_Mission_Reward(void* pMsg);
	void Fishing_Exp_Ack(void* pMsg);
	void Fishing_Point_Ack(void* pMsg);
	void Fishing_LevelUp_Ack(void* pMsg);
	void Fishing_FPChange_Ack(void* pMsg);
	void Fishing_FPChange_Nack(void* pMsg);

	void SendFishing_Ready(DWORD dwBaitIdx, POSTYPE BaitPos);
	void SendFishing_Cancel();

	void Process();

protected:
	BOOL  m_bActive;						// ����Ȱ�� �÷���
	BOOL  m_bPulling;						// ������ �÷���
	DWORD m_dwFishingPlaceIdx;				// ������NPC Instance Index
	DWORD m_dwFishingStartTime;				// ���۽ð�

	BOOL  m_bUseMission;

	WORD  m_wFishingLevel;
	DWORD m_dwFishingExp;
	DWORDEX m_dwFishPoint;

	// ������ �������
	float m_fGaugeStartPos;					// ������ ������ġ
	float m_fGaugeBarSpeed;					// ������ �̵��ӵ�
	int   m_nRepeatCount;					// ������ �ݺ�Ƚ��
	DWORD m_dwProcessTime;					// ������ ����ð�
	DWORD m_dwGaugeStartTime;				// ������ �̵����۽ð�


	DWORD m_dwUpdateTime;					// ������ ���Žð�(ms����)

	int	  m_nGaugeBarDir;

public:
	void FishingInfoClear();				// ���ð������� �ѹ濡 Ŭ����.

	DWORD GetItemIdxFromFM_Code(int nCode);	// ���ù̼� �ڵ�� ������Index���

	BOOL IsActive()											{return m_bActive;}
	BOOL IsPulling()										{return m_bPulling;}
	BOOL IsMission()										{return m_bUseMission;}

	void SetFishingPlace(DWORD dwIndex)						{m_dwFishingPlaceIdx = dwIndex;}
	DWORD GetFishingPlace()									{return m_dwFishingPlaceIdx;}

	void SetFishingStartTime(DWORD dwStartTime)				{m_dwFishingStartTime = dwStartTime;}
	DWORD GetFishingStartTime()								{return m_dwFishingStartTime;}

	int GetGaugeRepeatCount()								{return m_nRepeatCount;}
	DWORD GetUpdateTime()									{return m_dwUpdateTime;}

	float GetGaugeBarSpeed()								{return m_fGaugeBarSpeed;}
	int   GetGaugeBarDir()									{return m_nGaugeBarDir;}
	void  SetReverseDir()									{m_nGaugeBarDir *= -1;}

	WORD  GetFishingLevel()									{return m_wFishingLevel;}
	DWORD GetFishingExp()									{return m_dwFishingExp;}
	DWORDEX GetFishPoint()									{return m_dwFishPoint;}
};

EXTERNGLOBALTON(CFishingManager)