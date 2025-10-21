// Party.h: interface for the CParty class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARTY_H__F6D978D7_2592_4CB4_B54E_2AF0835E1A20__INCLUDED_)
#define AFX_PARTY_H__F6D978D7_2592_4CB4_B54E_2AF0835E1A20__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CParty  
{
	// 091127 LUJ, ������ ������ 0�� �迭�� �ִ�
	PARTY_MEMBER m_Member[MAX_PARTY_LISTNUM];	
	BYTE m_DiceGrade;
	BYTE m_Distribution;
public:
	CParty();
	virtual ~CParty();
	
	void InitParty(PARTY_INFO* pInfo);
	
//	void SetPartyIdx(DWORD Partyid);
	void SetMaster(PARTY_MEMBER* pMasterInfo);
	void SetMaster(DWORD MasterIdx,char* strName,BOOL bLog,BYTE LifePercent,BYTE ManaPercent);
	DWORD GetMemberID(int num); //��Ƽ������ ��� ����
	int GetMemberNum();
//	DWORD GetPartyIdx();
	DWORD GetMasterIdx();

	char* GetMasterName();

	void AddMember(PARTY_MEMBER* pMemberInfo);
	void DelMember(DWORD id);
	void BreakUpResult();

	void LogIn(PARTY_MEMBER* pMemberInfo);
	void LogOut(DWORD PlayerID);
	
	char* GetMemberName(DWORD MemberID);
	PARTY_MEMBER* GetPartyMemberInfo(int i);
	
	void Clear();
	void ChangeMaster(DWORD dwPlayerID);
	
	BOOL IsPartyMember(DWORD dwPlayerID);

	// 071006 LYW --- Party : Add function to check member for name.
	BOOL IsPartyMember(char* pName) ;
	// 071006 LYW --- Party : Add function to return member id to use name.
	DWORD GetMemberIdForName(char* pName);
	void SetDiceGrade(BYTE Option) { m_DiceGrade = Option; }
	void SetDistribution(BYTE Distribution) { m_Distribution = Distribution; }
	BOOL IsLogIn(int n);
};

#endif // !defined(AFX_PARTY_H__F6D978D7_2592_4CB4_B54E_2AF0835E1A20__INCLUDED_)

