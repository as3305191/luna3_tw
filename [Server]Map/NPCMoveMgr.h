// 090227 ShinJS --- �̵�NPC����
// NPCMoveMgr.h: interface for the CNPCMoveMgr class.
//				 �̵��ϴ� NPC�� �����ϴ� Manager class
//////////////////////////////////////////////////////////////////////
#ifndef __NPCMOVEMGR_H__
#define __NPCMOVEMGR_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MOVENPCRES CMoveNPCRes::GetInstance()
#define NPCMOVEMGR CNPCMoveMgr::GetInstance()

#define MAX_NPC_MOVEINFO	10									// NPC�� �̵����� �ִ� ����

// NPC�� �̵�����(�ʹ�ȣ, �̵���ǥ)
struct stNPC_MoveInfo{
	WORD wMapNum;			// �̵��� ��ȣ
	WORD wDir;				// ����
	COMPRESSEDPOS cPos;		// �̵���ǥ(x, y)
};

// �̵� NPC ������ ���� class
class CMoveNPC{
	WORD							m_wNPCIdx;					// NPC Index
	WORD							m_wNPCKindIdx;				// NPC ����
	WORD							m_wStartMapNum;				// ó�� ������ �� ��ȣ
	DWORD							m_dwMoveTime;				// �̵��ð� ����(��)
	DWORD							m_dwReadyToTime;			// �̵��� �غ�ð�(�۾��� ���ϰ� ���´�)
	BOOL							m_bSendReadyToMove;			// �̵��� �޼��� ���� ����	
	DWORD							m_dwLastMoveTime;			// ������ �̵��ð�
	WORD							m_wCurrentMoveIdx;			// ���� �̵����� Index

	CYHHashTable<stNPC_MoveInfo>	m_htMoveInfo;				// NPC�� �̵�����(�ִ밹��=MAX_NPC_MOVEINFO)
public:
	CMoveNPC( WORD wNPCIdx, WORD wNPCKindIdx, WORD wStartMapNum, DWORD dwMoveTime, DWORD dwReadyToTime ){
		m_wNPCIdx			= wNPCIdx;
		m_wNPCKindIdx		= wNPCKindIdx;
		m_wStartMapNum		= wStartMapNum;
		m_dwMoveTime		= dwMoveTime;
		m_dwReadyToTime		= dwReadyToTime;
		m_bSendReadyToMove	= FALSE;
		m_dwLastMoveTime	= 0;
		m_wCurrentMoveIdx	= 0;

		// �̵����� ���̺� �ʱ�ȭ
		m_htMoveInfo.Initialize( MAX_NPC_MOVEINFO );
	}
	~CMoveNPC()	{
		// �̵����� ���̺� ����
		stNPC_MoveInfo* pData = NULL;
		// 090707 ShinJS --- �޸� ���� ���� ����
		m_htMoveInfo.SetPositionHead();
		while( (pData = m_htMoveInfo.GetData()) != NULL )
		{
			delete pData;
			pData = NULL;
		}
		m_htMoveInfo.RemoveAll();
	}

	// NPC �̵����� �߰�
	void AddMoveInfo( stNPC_MoveInfo* pMoveInfo, WORD wIndex )	{ m_htMoveInfo.Add( pMoveInfo, wIndex ); }

	BOOL IsSendReadyToMove()	const { return m_bSendReadyToMove; }

	WORD GetNPCIdx()			const { return m_wNPCIdx; }
	WORD GetNPCKindIdx()		const { return m_wNPCKindIdx; }
	WORD GetStartMapNum()		const { return m_wStartMapNum; }
	DWORD GetMoveTime()			const { return m_dwMoveTime; }
	DWORD GetReadyToTime()		const { return m_dwReadyToTime; }
	DWORD GetLastMoveTime()		const { return m_dwLastMoveTime; }
	WORD GetCurMoveIdx()		const { return m_wCurrentMoveIdx; }

	void SetSendReadyToMove( BOOL bSendReadyToMove ) { m_bSendReadyToMove = bSendReadyToMove; }
	void SetLastMoveTime( DWORD dwLastMoveTime ) { m_dwLastMoveTime = dwLastMoveTime; }	

	// NPC �̵� (�̵�����Index�� ������Ų��)
	void Move()	{ if( m_htMoveInfo.GetDataNum() > 0 )	m_wCurrentMoveIdx = (m_wCurrentMoveIdx + 1) % WORD(m_htMoveInfo.GetDataNum()); }
	void Move( WORD wMoveIdx ) { if( m_htMoveInfo.GetDataNum() > 0 )	m_wCurrentMoveIdx = (wMoveIdx + 1) % WORD(m_htMoveInfo.GetDataNum()); }
	stNPC_MoveInfo* GetCurMoveInfo() { return (stNPC_MoveInfo*)m_htMoveInfo.GetData( m_wCurrentMoveIdx ); }
	stNPC_MoveInfo* GetNextMoveInfo() { return (stNPC_MoveInfo*)m_htMoveInfo.GetData( (m_wCurrentMoveIdx + 1) % m_htMoveInfo.GetDataNum() ); }
};

// �̵�NPC ������ ��� ������ �ִ� ResourceClass
class CMoveNPCRes
{
	CYHHashTable<CMoveNPC>			 m_htMoveNpc;				// �̵�NPC ������ ���� ���̺�
public:
	MAKESINGLETON( CMoveNPCRes );

	CMoveNPCRes();
	~CMoveNPCRes();

	CYHHashTable<CMoveNPC>* GetMoveNpcTable() { return &m_htMoveNpc; }
	CMoveNPC* GetMoveNpc( WORD wNpcIdx ) { return (CMoveNPC*)m_htMoveNpc.GetData( wNpcIdx ); }
};


// �̵�NPC ������ �����ϴ� Manager
class CNPCMoveMgr
{
	CYHHashTable<CMoveNPC>			 m_htMoveNpc;				// �̵�NPC ������ ���� ���̺�
public:
	MAKESINGLETON( CNPCMoveMgr );

	CNPCMoveMgr();
	~CNPCMoveMgr();

	void Init();
	void Process();												// �̵��ð� üũ

	BOOL AddMoveNpc( WORD wNpcIdx, WORD wMoveIdx );				// �̵�NPC �߰�
	void RemoveMoveNpc( WORD wNpcIdx );							// �̵�NPC ����

	BOOL IsReadyToMove( WORD wNpcIdx );							// �̵�NPC �̵��غ� �������� Ȯ��
};


#endif // #ifndef __NPCMOVEMGR_H__
