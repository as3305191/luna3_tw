// 090227 ShinJS --- �̵�NPC����
// NPCMoveMgr.cpp: implementation of the CNPCMoveMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NPCMoveMgr.h"
#include "MHFile.h"
#include "Network.h"
#include "../[CC]Header/GameResourceManager.h"
#include "Player.h"
#include "UserTable.h"

//////////////////////////////////////////////////////////////////////
// CMoveNPCRes : �̵�NPC ������ ������ ���ҽ� Ŭ����
//////////////////////////////////////////////////////////////////////
CMoveNPCRes::CMoveNPCRes()
{
	m_htMoveNpc.Initialize( MAX_MOVENPC_NUM );		// �̵�NPC �ʱ�ȭ

	// �̵�NPC ���ҽ����� �о����
	CMHFile file;
	if( !file.Init( "./system/Resource/MoveNpc.bin", "rb") )
		return;

	char Token[256];	
	while( !file.IsEOF() )
	{
		file.GetString( Token );

		if( strcmp(Token, "//") == 0 )
		{
			file.GetLine(Token, 256);
			continue;
		}

		// �̵�NPC �߰�
		if( strcmp(Token,"$NPC") == 0 )
		{
			// �ִ� �̵�NPC ���� �ʰ��ΰ��
			if( m_htMoveNpc.GetDataNum() >= MAX_MOVENPC_NUM )
				break;

			WORD wNpcIdx		= file.GetWord();							// NPC Index
			WORD wNpcKindIdx	= file.GetWord();							// NPC Kind
			WORD wStartMapNum	= file.GetWord();							// ó�� ������ �� ��ȣ
			DWORD dwMoveTime	= file.GetDword();							// �̵��ð� ����(��)
			DWORD dwReadyToTime	= file.GetDword();							// �̵��� �غ�ð�(��)
			WORD wMoveInfoIdx	= 0;										// ���̺� �ε���

			CMoveNPC* pMoveNPC = new CMoveNPC( wNpcIdx, wNpcKindIdx, wStartMapNum, dwMoveTime, dwReadyToTime );
			ASSERT( pMoveNPC );
			if( !pMoveNPC )		break;

			while( !file.IsEOF() )
			{
				file.GetString( Token );

				if( strcmp(Token, "//") == 0 )
				{
					file.GetLine(Token, 256);
					continue;
				}

				if( Token[0] == '}' )
					break;

				// �̵�NPC�� �̵����� �߰�
				if( strcmp(Token, "#MOVEINFO") == 0 )
				{
					// �̵����� ������ �Ѿ ��� �߰����� �ʴ´�
					if( wMoveInfoIdx >= MAX_NPC_MOVEINFO )
						continue;

					stNPC_MoveInfo* pMoveInfo = new stNPC_MoveInfo();
					ASSERT( pMoveInfo );
					if( !pMoveInfo )	break;

					pMoveInfo->wMapNum = file.GetWord();					// �̵��ϴ� �ʹ�ȣ
					pMoveInfo->cPos.wx = file.GetWord();					// �̵��ϴ� x��ǥ
					pMoveInfo->cPos.wz = file.GetWord();					// �̵��ϴ� z��ǥ
					pMoveInfo->wDir	   = file.GetWord();					// �̵��� ĳ���� ����

					pMoveNPC->AddMoveInfo( pMoveInfo, wMoveInfoIdx++ );		// ���̺� ���� �߰�
				}
			}

			m_htMoveNpc.Add( pMoveNPC, wNpcIdx );							// �̵�NPC �߰�
		}
	}
	file.Release();
}

CMoveNPCRes::~CMoveNPCRes()
{
	// �̵�NPC ����
	CMoveNPC* pData = NULL;
	// 090707 ShinJS --- �޸� ���� ���� ����
	m_htMoveNpc.SetPositionHead();
	while( (pData = m_htMoveNpc.GetData()) != NULL )
	{
		delete pData;
		pData = NULL;
	}
	m_htMoveNpc.RemoveAll();
}


//////////////////////////////////////////////////////////////////////
// CNPCMoveMgr : NPC �̵��� ������ Ŭ����
//////////////////////////////////////////////////////////////////////
CNPCMoveMgr::CNPCMoveMgr()
{
	m_htMoveNpc.Initialize( MAX_MOVENPC_NUM );		// �̵�NPC �ʱ�ȭ
}

CNPCMoveMgr::~CNPCMoveMgr()
{
	// �̵�NPC ����
	m_htMoveNpc.RemoveAll();
}


void CNPCMoveMgr::Init()
{	
	CYHHashTable<CMoveNPC>* pMoveNpcRes = MOVENPCRES->GetMoveNpcTable();
	if( !pMoveNpcRes )		return;

	CMoveNPC* pMoveNpc = NULL;
	pMoveNpcRes->SetPositionHead();

	while( (pMoveNpc = pMoveNpcRes->GetData()) != NULL )
	{
		// ���� �ʹ�ȣ�� �´� �̵�NPC �߰�
		if( pMoveNpc->GetStartMapNum() == GAMERESRCMNGR->GetLoadMapNum() )
		{
			// �����ڿ� �̵�NPC�� ����Ѵ�
			m_htMoveNpc.Add( pMoveNpc, pMoveNpc->GetNPCIdx() );

			stNPC_MoveInfo* pMoveInfo = pMoveNpc->GetCurMoveInfo();
			if( !pMoveInfo )	continue;

			// GAMERESRCMNGR�� �̵�NPC ������ �߰��Ͽ� �۾�(�ŷ�)�� Ȱ��ȭ ��Ų��
			GAMERESRCMNGR->AddMoveNpcInfo( pMoveNpc->GetNPCIdx(), pMoveNpc->GetNPCKindIdx(), pMoveInfo->cPos.wx, pMoveInfo->cPos.wz, pMoveInfo->wDir );
		}
	}

}

void CNPCMoveMgr::Process()
{
	CMoveNPC* pMoveNPC = NULL;

	m_htMoveNpc.SetPositionHead();
	while( (pMoveNPC = m_htMoveNpc.GetData()) != NULL )
	{
		DWORD dwLastMoveTime	= pMoveNPC->GetLastMoveTime();				// ������ �̵��ð�
		DWORD dwMoveTime		= pMoveNPC->GetMoveTime();					// �̵��ð� ����(��)
		DWORD dwReadyToTime		= pMoveNPC->GetReadyToTime();				// �̵��غ�ð� ����(��)

		// �̵��غ��۾� �˸� (�̵��غ�ð��̰� �غ�޼����� �������� �������)
		if( (gCurTime - dwLastMoveTime) > (dwMoveTime - dwReadyToTime)*1000 &&
			!pMoveNPC->IsSendReadyToMove() )
		{
			stNPC_MoveInfo* pCurMoveInfo = pMoveNPC->GetCurMoveInfo();		// �̵��� ����
			if( !pCurMoveInfo )		continue;

			// StaticNpc Table�� �̵�NPC ���� (StaticNpc Table�� �����Ͽ� �۾�(�ŷ�)�� ���ϵ����Ѵ�)
			GAMERESRCMNGR->RemoveStaticNpcInfo( pMoveNPC->GetNPCIdx() );
			
			// �������� �̵��غ� �޼����� ������
			MSG_WORD msg;
			memset( &msg, 0, sizeof(MSG_WORD) ) ;

			msg.Category	= MP_NPC ;
			msg.Protocol	= MP_NPC_READYTOMOVE_NOTIFY;
			msg.wData		= pMoveNPC->GetNPCIdx();

			g_pUserTable->SetPositionUserHead();
			CObject* pObject = NULL;
			while( (pObject = g_pUserTable->GetUserData() ) != NULL)
			{	
				if( pObject->GetObjectKind() != eObjectKind_Player ) continue;

				CPlayer* pReceiver = (CPlayer*)pObject;
				pReceiver->SendMsg( &msg, sizeof(MSG_WORD) );
			}

			// �̵��غ� �÷��� ����
			pMoveNPC->SetSendReadyToMove( TRUE );
		}

		// �̵��ؾ��� �ð��� ���
		if( (gCurTime - dwLastMoveTime) > dwMoveTime*1000 )
		{
			stNPC_MoveInfo* pCurMoveInfo = pMoveNPC->GetCurMoveInfo();		// �̵��� ����
			stNPC_MoveInfo* pNextMoveInfo = pMoveNPC->GetNextMoveInfo();	// �̵��� ����
			if( !pCurMoveInfo || !pNextMoveInfo )		continue;

			// ���� �ٲ�� ���
			if( pCurMoveInfo->wMapNum != pNextMoveInfo->wMapNum )
			{
				// �̵�NPC �̵� �޼��� ���� (�̵��� �ʿ� �߰� ������ ����)
				MSG_WORD4 msg;
				memset( &msg, 0, sizeof(MSG_WORD4) );

				msg.Category	= MP_NPC;
				msg.Protocol	= MP_NPC_MOVE_MOVENPC_SYN;
				msg.wData1		= pCurMoveInfo->wMapNum;
				msg.wData2		= pNextMoveInfo->wMapNum;
				msg.wData3		= pMoveNPC->GetNPCIdx();
				msg.wData4		= pMoveNPC->GetCurMoveIdx();

				g_Network.Send2AgentServer( (char*)&msg, sizeof(msg) );

				pMoveNPC->SetLastMoveTime( gCurTime );												// ������ �̵��ð� ����
			}
			// ���� ������ �̵��ϴ� ���
			else
			{
				// �ٷ� �̵���Ų��
				pMoveNPC->Move();																	// �̵�NPC�� �̵��Ѵ�
				pMoveNPC->SetLastMoveTime( gCurTime );												// ������ �̵��ð� ����
				pMoveNPC->SetSendReadyToMove( FALSE );												// �̵��غ� �÷��� ����

				// �̵��� ������ �������ش�(�߰��� �����ϴ� ��� ����)
				GAMERESRCMNGR->AddMoveNpcInfo( pMoveNPC->GetNPCIdx(), pMoveNPC->GetNPCKindIdx(), pNextMoveInfo->cPos.wx, pNextMoveInfo->cPos.wz, pNextMoveInfo->wDir );
				
				// �������� �߰��޼��� ����(�����ϴ°�� �߰����� ����NPC�� �̵��Ѵ�)
				MSG_DWORD4 msg;
				memset( &msg, 0, sizeof(MSG_DWORD4) );

				msg.Category	= MP_NPC;
				msg.Protocol	= MP_NPC_ADD_MOVENPC_SYN;
				msg.dwData1		= MAKEDWORD( pNextMoveInfo->wMapNum, pNextMoveInfo->wDir );			// Map ��ȣ, ����
				msg.dwData2		= MAKEDWORD( pMoveNPC->GetNPCIdx(), pMoveNPC->GetNPCKindIdx() );	// NPC Index, NPC Kind
				msg.dwData3		= MAKEDWORD( pNextMoveInfo->cPos.wx, pNextMoveInfo->cPos.wz );		// NPC ��ġ

				g_pUserTable->SetPositionUserHead();
				CObject* pObject = NULL ;
				while( (pObject = g_pUserTable->GetUserData()) != NULL )
				{	
					if( pObject->GetObjectKind() != eObjectKind_Player ) continue;

					CPlayer* pReceiver = (CPlayer*)pObject;
					pReceiver->SendMsg( &msg, sizeof(MSG_DWORD4) );
				}
			}
		}
	}
}

BOOL CNPCMoveMgr::AddMoveNpc( WORD wNpcIdx, WORD wMoveIdx )
{
	// �̵�NPC ���ҽ����� �ش�Index�� ���� �̵�NPC�� ���Ѵ�
	CMoveNPC* pMoveNpc = MOVENPCRES->GetMoveNpc( wNpcIdx );
	if( !pMoveNpc ) return FALSE;

	// �ش� NPC�� �������� �ʴ� ��쿡 �߰� (�� �ٿ��̳� ��������� ���� MoveNpc�� ���߻����� ���?)
	if( m_htMoveNpc.GetData( pMoveNpc->GetNPCIdx() ) == NULL )
		m_htMoveNpc.Add( pMoveNpc, pMoveNpc->GetNPCIdx() );								// �̵�NPC �߰�	
	
	pMoveNpc->Move( wMoveIdx );															// �̵�NPC �̵�
	pMoveNpc->SetLastMoveTime( gCurTime );												// ������ �̵��ð� ����
	pMoveNpc->SetSendReadyToMove( FALSE );												// �̵��غ� �÷��� ����

	stNPC_MoveInfo* pCurMoveInfo = pMoveNpc->GetCurMoveInfo();
	if( !pCurMoveInfo )	return FALSE;

	// �̵��� ������ �������ش�(�߰��� �����ϴ� ��� ����)
	GAMERESRCMNGR->AddMoveNpcInfo( pMoveNpc->GetNPCIdx(), pMoveNpc->GetNPCKindIdx(), pCurMoveInfo->cPos.wx, pCurMoveInfo->cPos.wz, pCurMoveInfo->wDir );

	// �����鿡�� �߰� �޼��� ����
	MSG_DWORD4 msg;
	memset( &msg, 0, sizeof(MSG_DWORD4) );

	msg.Category	= MP_NPC;
	msg.Protocol	= MP_NPC_ADD_MOVENPC_SYN;
	msg.dwData1		= MAKEDWORD( pCurMoveInfo->wMapNum, pCurMoveInfo->wDir );			// Map ��ȣ, ����
	msg.dwData2		= MAKEDWORD( pMoveNpc->GetNPCIdx(), pMoveNpc->GetNPCKindIdx() );	// NPC Index, NPC Kind
	msg.dwData3		= MAKEDWORD( pCurMoveInfo->cPos.wx, pCurMoveInfo->cPos.wz );		// NPC ��ġ

	g_pUserTable->SetPositionUserHead();
	CObject* pObject = NULL ;
	while( (pObject = g_pUserTable->GetUserData()) != NULL )
	{	
		if( pObject->GetObjectKind() != eObjectKind_Player ) continue;

		CPlayer* pReceiver = (CPlayer*)pObject;
		pReceiver->SendMsg( &msg, sizeof(MSG_DWORD4) );
	}

	return TRUE;
}

void CNPCMoveMgr::RemoveMoveNpc( WORD wNpcIdx )
{
	m_htMoveNpc.Remove( wNpcIdx );

	// GAMERESRCMNGR�� �̵�NPC ������ �����Ͽ� �۾��� ��Ȱ��ȭ ��Ų��
	GAMERESRCMNGR->RemoveMoveNpcInfo( wNpcIdx );
}

BOOL CNPCMoveMgr::IsReadyToMove( WORD wNpcIdx )
{
	CMoveNPC* pMoveNpc = (CMoveNPC*)m_htMoveNpc.GetData( wNpcIdx );
	if( pMoveNpc )
		return pMoveNpc->IsSendReadyToMove();

	return FALSE;
}