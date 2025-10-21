#pragma once

class CParty;
class CPlayer;

class CDistributer
{
	struct DAMAGEOBJ
	{
		DWORD dwID;
		DWORD dwData;
	};

	CYHHashTable<DAMAGEOBJ>	m_DamageObjectTableSolo;	// ������ �������� ���� ���̺�.
	CYHHashTable<DAMAGEOBJ>	m_DamageObjectTableParty;	// ��Ƽ�� �������� ���� ���̺�.
	DWORD m_1stPlayerID;								// ó�� �������� �� �÷��̾��� ���̵�.
	DWORD m_1stPartyID;									// ó�� �������� �� ��Ƽ�� ���̵�.
	DWORD m_TotalDamage;								// ������ �հ�.
	DWORD  m_FirstDamage ;								// ù ������.
	int m_nTotalKillerCount;
	LEVELTYPE m_HighLevel;
	DWORD mKillerPlayerIndex;
	CPlayer* m_pKiller;
	CMonster* m_pMob;
	VECTOR3	m_pKilledPosition;
	DWORD m_dwDropItemID;
	DWORD m_dwDropItemRatio;
	typedef DWORD ObjectIndex;
	typedef std::list< ObjectIndex > IndexContainer;
	IndexContainer mPlayerIndexContainer;

public:
	CDistributer();
	~CDistributer();
	void Release();
	void AddDamageObject(CPlayer*, DWORD damage, DWORD plusdamage);
	void DistributeItemPerDamage(CMonster&) ;
	void DistributePerDamage(CMonster&);
	void DamageInit();
	void DeleteDamagedPlayer(DWORD CharacterID);
	void SetInfoToDistribute(DWORD dwKillerID, DWORD DropItemID, DWORD DropItemRatio, CMonster&);
	void Distribute();

private:
	BOOL Chk(CPlayer&, DWORD GridID);
	void ChooseOne(const DAMAGEOBJ&, DWORD& pBigDamage, DWORD& pBigID);
	void DoAddDamageObj(CYHHashTable<DAMAGEOBJ>&, DWORD dwID, DWORD damage);
	// ų���� �������� �� ó���ϴ� �Լ� ��Ʈ																												 //
	void DistributeToKillers() ;										// ų���� �ټ��� �� ó���ϴ� �Լ�.
	void DistributerToSolo(MONEYTYPE money) ;							// ų������ ��Ƽ�� �Ҽӵ��� �ʾ����� ó���ϴ� �Լ�.
	void DistributeToFirstKiller(CPlayer&, DWORD dwMyDamage) ;	// ��Ƽ�� ����, ù �������� �� ų���� ó���ϴ� �Լ�.
	void DistributeToOtherKiller(CPlayer&, DWORD dwMyDamage) ;	// ��Ƽ�� ����, ���߿� �������� �� ų���� ó���ϴ� �Լ�.
	void DistributeToPartys(MONEYTYPE money) ;							// ų���� ��Ƽ�� �ҼӵǾ��� �� ó���ϴ� �Լ�.
	void DistributeToFirstParty(CParty&, DWORD dwPartyDamage);			// ù �������� �� ��Ƽ�� ó���ϴ� �Լ�.
	void DistributeToOtherParty(CParty&, DWORD dwPartyDamage) ;			// ���߿� �������� �� ��Ƽ�� ó���ϴ� �Լ�.
	void SendMoneyToPartys(float ChangeValue) ;							// ų���� ��Ƽ�� �Ӵϸ� �����ϴ� �Լ�.
	void DistributeItemToKillers() ;									// ų���� �ټ��� �� ������ �й踦 ó���ϴ� �Լ�.

	// ų���� �Ѹ��� �� ó���ϴ� �Լ� ��Ʈ
	void DistributeToKiller() ;											// ų���� �Ѹ��� �� ó���ϴ� �Լ�.
	void DistributeToPerson(CPlayer&);
	void DistributeToParty(CPlayer&);									// ų���� �Ѹ��̰�, ��Ƽ�� �ҼӵǾ��� �� ó���ϴ� �Լ�. 
	void SendMoneyToPerson(CPlayer* pPlayer, MONEYTYPE ChangeValue) ;	// ų������ �Ӵϸ� �����ϴ� �Լ�.
	void SendItemToPerson(CPlayer* pPlayer) ;							// ų������ �������� �����ϴ� �Լ�.
	DWORD CalcObtainExp(LEVELTYPE KillerLevel, int nMemberCount) ;		// �־��� ������ ���� ������ ����ġ�� ��ȯ�ϴ� �Լ�.
	BOOL GetFirstDamange();
	
	void GetTotalKillerCount() ;										// ��Ƽ�� 1���ϰ� �� ų������ ���ϴ� �Լ�.
	void GetHighLevelOfKillers() ;										// ų�� �� �� �ְ� ������ �̴´�.
	int  GetLevelPenaltyToMonster(LEVELTYPE);
	void GetAllPartyDamage();
	void SaveCandidacy(CParty&);										// ��Ƽ�� �� �ĺ��� ��� �Լ�.
	void DistribuSequence(CParty&);
	void DistributeDamage();
	void SendToPersonalExp(CPlayer*, EXPTYPE);

	enum eGET_LVTYPE { eGET_CURLEVEL, eGET_MAXLEVEL };
	void GetPartyLevels(CParty&, LEVELTYPE& pHighLevel, LEVELTYPE& pTotalLevel , eGET_LVTYPE eGetLvType = eGET_CURLEVEL) ;	// ��Ƽ�� �ְ�����, �� ���� ���� ���ϴ� �Լ�.
	void AddMoney(CPlayer*, MONEYTYPE, WORD MonsterKind);
};