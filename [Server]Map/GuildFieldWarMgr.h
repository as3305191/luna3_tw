/**********************************************************************

�̸�: GuildWarMgr
����: ��� ���� ������ �����Ѵ�

����: 070830 ����: ����� ���� �ð�(1�� 1��) �ּ� ó��
                                                                     
***********************************************************************/
#pragma once


class CGuild;
class CGuildUnion;


struct sGFWENEMY
{
	DWORD	dwEnemyGuildIdx;
	DWORDEX	dwGFWMoney;
};

class CGuildWar 
{
protected:
	DWORD				m_dwGuildIdx;
	CYHHashTable<sGFWENEMY>	m_EnemyGuildTable;	
	DWORD				m_dwVictoryCount;
	DWORD				m_dwDrawCount;
	DWORD				m_dwLooseCount;

public:
	CGuildWar();
	virtual ~CGuildWar();

	void	Init( DWORD dwGuildIdx )	{ m_dwGuildIdx = dwGuildIdx; }
	void	Release();
	
	void	RegistGuildWar( DWORD dwEnemyGuildIdx, DWORDEX dwMoney );
	void	UnregistGuildWar( DWORD dwEnemyGuildIdx );
	BOOL	IsEnemy( DWORD dwEnemyGuildIdx );
	void	AddPlayer( CPlayer* pPlayer );
	BOOL	IsGuildWar();
	void	SetGuildWarRecord( DWORD dwVictory, DWORD dwDraw, DWORD dwLoose )
	{	m_dwVictoryCount = dwVictory;	m_dwDrawCount = dwDraw;	m_dwLooseCount = dwLoose;	}
	void	VictoryGuildWar()	{ ++m_dwVictoryCount; }
	void	DrawGuildWar()		{ ++m_dwDrawCount; }
	void	LooseGuildWar()	{ ++m_dwLooseCount; }
	void	UpdateGuildWarRecord();
	DWORDEX	GetWarMoney( DWORD dwEnemyGuildIdx );

	void	GetEnemy( std::list< sGFWENEMY* >& );
};

//struct sGFWTIME
//{
//	DWORD		dwEnemyGuildIdx;
//	stTIME		Time;
//	DWORD		dwCount;
//};

//class CGFWCheck
//{
//protected:
//	DWORD					m_dwGuildIdx;
//	CYHHashTable<sGFWTIME>	m_GFWDenyTable;
//	CYHHashTable<sGFWTIME>	m_GFWEndTable;
//
//public:
//	CGFWCheck();
//	virtual ~CGFWCheck();
//
//	void	Init( DWORD dwGuildIdx )	{ m_dwGuildIdx = dwGuildIdx; }
//	void	Release();
//
//	int		CanGuildWar( DWORD dwEnemyGuildIdx );
//	void	RegistDenyTime( DWORD dwEnemyGuildIdx );
//	void	RegistEndTime( DWORD dwEnemyGuildIdx );
//};

#define GUILDWARMGR CGuildWarManager::GetInstance()

class CGuildWarManager  
{
protected:
	CYHHashTable<CGuildWar>	m_GuildWarTable;
	//CYHHashTable<CGFWCheck>			m_GFWCheckTable;
	
public:
	MAKESINGLETON( CGuildWarManager );

	CGuildWarManager();
	virtual ~CGuildWarManager();

	void	Init();
	void	Release();

	void	RegistGuildWarFromRecord( DWORD dwGuildIdx, DWORD dwVictory, DWORD dwDraw, DWORD dwLoose );
	void	UpdateGuildWarRecord( BOOL bDB, WORD wFlag, DWORD dwGuildIdx );
	BOOL	RegistGuildWar( DWORD dwGuildIdx1, DWORD dwGuildIdx2, DWORDEX dwMoney );
	BOOL	UnregistGuildWar( DWORD dwGuildIdx1, DWORD dwGuildIdx2 );
	BOOL	JudgeGuildWar( CPlayer* pPlayer, CPlayer* pAttacker );
	BOOL	IsEnemy( CPlayer* pOper, CPlayer* pTarget );
	BOOL	IsEnemyInField( CPlayer* pOper, CPlayer* pTarget );
	void	AddPlayer( CPlayer* );
	BOOL	IsGuildWar( DWORD dwGuildIdx1, DWORD dwGuildIdx2 );
	BOOL	IsGuildWar( DWORD dwGuildIdx );
	DWORDEX	GetWarMoney( DWORD dwGuildIdx, DWORD dwEnemyGuildIdx );
	
	void	NetworkMsgParse( BYTE Protocol, void* pMsg );

private:
	void	Declare( void* pMsg );
	void	DeclareAccept( void* pMsg );
	void	DeclareDeny( void* pMsg );
	void	DeclareDenyNotifyToMap( void* pMsg );

	void	StartNotifyToMap( void* pMsg );
	//void	End( DWORD dwGuildIdxWinner, DWORD dwGuildIdxLoser );
	
	void	SuggestEnd( void* pMsg );
	void	SuggestEndAccept( void* pMsg );
	void	SuggestEndAcceptNotifyToMap( void* pMsg );
	void	SuggestEndDeny( void* pMsg );
	void	SuggestEndDenyNotifyToMap( void* pMsg );
	
	void	Surrender( void* pMsg );
	void	SurrenderNotifyToMap( void* pMsg );

	void	AddMoneyToMap( void* pMsg );
	void	SendDeclareNackMsg( DWORD dwReceiver, DWORD dwState );
	void	SendMoneyMsg( CPlayer* pPlayer1, CPlayer* pPlayer2, DWORDEX dwMoney, BYTE Protocol );
	void	SendToAgentAddMoneyMsg( DWORD dwPlayerIdx, DWORDEX dwMoney );

	// 071009 ����, ����� �� ���� ���� �ڵ�
public:
	// Ű: ��� ������, ��: ������
	typedef std::map< CGuild*, MONEYTYPE >	Alliance;

	// ������ ���� ���� ��忡�Ե� ����Ѵ�
	void AddUnionEnemyToGuild( CGuildUnion*, CGuild* );

	// ������ ���� ���� ��忡�Լ� �����Ѵ�.
	void RemoveUnionEnemyFromGuild( CGuildUnion*, CGuild* );

	void SendToAllUser( DWORD dwFlag, const char* pGuildName1, const char* pGuildName2 );

private:
	// ����� ����� �����´�
	void GetEnemy( DWORD guildIndex, std::list< sGFWENEMY* >& );

	// ��ƿ��Ƽ �Լ�. �ش� ��尡 ���Ϳ� ����������, ������ ��� ��ü�� Alliance�� ��´�
	void AddGuildToGroup( CGuildUnion*, Alliance& );

	// ��� �׷��� ���� ������ ��Ͻ�Ű�� �ٸ� �� �������� �޽����� ���� �����ϰ� �Ѵ�.
	void AddEnemyToFriendly( Alliance& friendlyMap, Alliance& enemyMap );
};