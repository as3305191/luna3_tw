// Battle_GTournament.h: interface for the CBattle_GTournament class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BATTLE_GTOURNAMENT_H__8B9560E3_8E1A_4622_A35B_FE7D9455C4BC__INCLUDED_)
#define AFX_BATTLE_GTOURNAMENT_H__8B9560E3_8E1A_4622_A35B_FE7D9455C4BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "Battle.h"
#include "BattleTeam_GTournament.h"

#ifndef _MAPSERVER_
#include "../ImageNumber.h"
#endif

struct BATTLE_INFO_GTOURNAMENT : public BATTLE_INFO_BASE
{
	DWORD					m_dwEntranceTime;
	DWORD					m_dwFightTime;
	DWORD					m_dwLeaveTime;
	DWORD					Ranking;				// �� ������
	GTTEAM_MEMBERINFO		MemberInfo;
};


class CBattle_GTournament : public CBattle
{
	CBattleTeam_GTournament		m_Observer;

	BATTLE_INFO_GTOURNAMENT		m_GTInfo;

	VECTOR3		m_Team1Pos;
	VECTOR3		m_Team2Pos;
	VECTOR3		m_ObserverPos;

	DWORD			m_ReadyTime;
	DWORD			m_FightTime;
	DWORD			m_ResultTime;

#ifndef _MAPSERVER_
	DWORD			m_GuildIdx[2];

	CImageNumber	m_ImageNumber;

	cImage			m_ImageWin;
	cImage			m_ImageLose;
	//cImage			m_ImageStart;
	cImage*			m_pCurShowImage;

	VECTOR2			m_vTitlePos;
	VECTOR2			m_vTitleScale;

	DWORD			m_RenderTime;
	DWORD			m_nLastNum;

	BOOL			m_bMovedBridge;
	DWORD			m_dwMoveBridgeTime;
#endif


public:
	CBattle_GTournament();
	virtual ~CBattle_GTournament();
	
	void Initialize(BATTLE_INFO_BASE* pCreateInfo, CBattleTeam* pTeam1, CBattleTeam* pTeam2);
	void Release();

#ifdef _MAPSERVER_
	virtual void GetBattleInfo(char* pInfo,WORD* size);
	virtual BOOL AddObjectToBattle(CObject* pObject);
	virtual BOOL DeleteObjectFromBattle(CObject* pObject);
	virtual DWORD GetTeamMemberNum(int i);
	BOOL AddObserverToBattle(CObject* pObject);
	void ReleaseAllObject();

	WORD CalcKillPointByLevel(LEVELTYPE KillerLevel, LEVELTYPE VictimLevel);
	WORD CalcKillPointByStage(BYTE KillerStage, BYTE VictimStage);
#else
	void SetBrokenBridge();
	void SetMoveBridge();
	virtual void Render();
	BOOL IsObserver( CObject* pObject );

	void SetGTName( CObject* pObject, DWORD NameType );
	void ReduceEntranceTime(DWORD dwTickTime) {if(dwTickTime<m_GTInfo.m_dwEntranceTime) m_GTInfo.m_dwEntranceTime-=dwTickTime;}
#endif

	//
	virtual void OnCreate(BATTLE_INFO_BASE* pCreateInfo, CBattleTeam* pTeam1, CBattleTeam* pTeam2);
	virtual void OnDestroy();
	virtual BOOL OnTeamMemberDie(int Team,DWORD VictimMemberID,DWORD KillerID);
	virtual void OnTeamMemberRevive(int Team,CObject* pReviver);
	virtual void OnTeamMemberAdd(int Team,DWORD MemberID,char* Name);
	//
	void OnTick();
	
	// 
	virtual BOOL IsEnemy(CObject* pOperator,CObject* pTarget);
	virtual BOOL IsFriend(CObject* pOperator,CObject* pTarget);

	virtual BOOL Judge();		// Judge���� Victory�� Draw �Լ��� ȣ�����־�� �Ѵ�.
	BOOL JudgeMemberExist();
	BOOL JudgeKillPoint();			// ų����Ʈ
	BOOL JudgeGuildLevel();
	BOOL JudgeMemberKill();			// ���� ���� Kill ��
	BOOL JudgeMemberLevel();		// ��Ƴ��� ����� Level�� ���� ������
	BOOL JudgeMemberLevelExp();		// ��Ƴ��� ����� ����ġ ������ ������
	virtual void OnFightStart();
	virtual void Victory(int WinnerTeamNum,int LoserTeamNum);
	virtual void Draw();

	void LoseAll();
	void StartBattle();

	VECTOR3* GetRevivePoint(DWORD dwTeam);
	
	//
	DWORD GetKillPoint(DWORD Team)					{	return ((CBattleTeam_GTournament*)m_TeamArray[Team])->GetKillPoint();	}
	DWORD GetBattleTeamGuildIdx( DWORD Team )		{	return ((CBattleTeam_GTournament*)m_TeamArray[Team])->GetGuildIdx();	}
	DWORD GetEntranceTime()							{	return m_GTInfo.m_dwEntranceTime;		}
	DWORD GetFightTime()							{	return m_GTInfo.m_dwFightTime;			}
	void ReturnToMap( DWORD playerID );
	
	virtual DWORD GetBattleTeamID( CObject* pObject );			// 1 = Battleteam1, 2 = Battleteam2

};

#endif // !defined(AFX_BATTLE_GTOURNAMENT_H__8B9560E3_8E1A_4622_A35B_FE7D9455C4BC__INCLUDED_)
