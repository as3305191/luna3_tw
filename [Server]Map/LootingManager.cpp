// LootingManager.cpp: implementation of the CLootingManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LootingManager.h"
#include "LootingRoom.h"
#include "Player.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLootingManager::CLootingManager()
{
	m_htLootingRoom.Initialize( 100 );
	m_htDeleteRoom.Initialize( 10 );
	m_mpLootingRoom = new CPool<CLootingRoom>;
	m_mpLootingRoom->Init( 100, 10, "CLootingRoom" );
}

CLootingManager::~CLootingManager()
{
	m_htLootingRoom.SetPositionHead();
	CLootingRoom* pRoom = NULL;
	while( (pRoom = m_htLootingRoom.GetData()) != NULL)
	{
		//pRoom release	//���� �ɸ��� ���״�!!!!!!!!
		m_mpLootingRoom->Free( pRoom );
	}
	m_htLootingRoom.RemoveAll();
	m_htDeleteRoom.RemoveAll();
	SAFE_DELETE( m_mpLootingRoom );	
}

//ô����� �Ҹ�Ǳ� ���� ȣ������!
//---KES PK 071202
BOOL CLootingManager::IsLootingSituation( CPlayer* pDiePlayer, CPlayer* pAttackPlayer )
{
	if( pAttackPlayer->GetState() == eObjectState_Die )	//�׾����� ������ �� ����.
		return FALSE;

	if( pAttackPlayer->IsPKMode() )		//PK����̰ų�
		return TRUE;
	
	if( pDiePlayer->IsPKMode() )		//���� ����� PK Mode
		return TRUE;

//---KES PK ��� �ּ�
//	if( pAttackPlayer->IsWantedOwner( pDiePlayer->GetWantedIdx() ) )	//ô����� ��������
//		return TRUE;

	return FALSE;
}

int CLootingManager::AutoLooting( CPlayer* pDiePlayer, CPlayer* pAttackPlayer )
{
//�̹� �ִٸ� ���ְ� �ٽ� ������!
	DWORD dwDiePlayerID = pDiePlayer->GetID();
	CloseLootingRoom( dwDiePlayerID, FALSE );
//
	CLootingRoom* pRoom = m_mpLootingRoom->Alloc();
	pRoom->MakeLootingRoom( pDiePlayer, pAttackPlayer );
	
	int nArrayNum = rand()%PKLOOTING_ITEM_NUM;
	pRoom->Loot( nArrayNum );

	int nLootedItemNum = pRoom->GetLootedItemCount();

	m_mpLootingRoom->Free( pRoom );

	return nLootedItemNum;
}

void CLootingManager::CreateLootingRoom( CPlayer* pDiePlayer, CPlayer* pAttackPlayer )
{
//�̹� �ִٸ� ���ְ� �ٽ� ������!
	CloseLootingRoom( pDiePlayer->GetID(), FALSE );
//
	CLootingRoom* pRoom = m_mpLootingRoom->Alloc();
	//��ȯ��ҿ� ������ҵ��� ���� ��Ų�Ŀ�!	//PKLOOT
	pRoom->MakeLootingRoom( pDiePlayer, pAttackPlayer );
	m_htLootingRoom.Add( pRoom, pDiePlayer->GetID() );

//------------------------------------
// �޼����� ���߿� ������ ���� �ִ�.	//PKLOOT
	//msg to Attacker
	MSG_DWORD msg;
	msg.Category	= MP_PK;
	msg.Protocol	= MP_PK_LOOTING_START;
	msg.dwData		= pDiePlayer->GetID();
	pAttackPlayer->SendMsg( &msg, sizeof(msg) );
	//�����߿� �� �� ���°��� �����ϱ�?			//PKLOOT

	//msg to DiePlayer
	msg.Protocol	= MP_PK_LOOTING_BEINGLOOTED;
	msg.dwData		= pAttackPlayer->GetID();
	pDiePlayer->SendMsg( &msg, sizeof(msg) );
	//���ô��ϴ� �߿� �Ҽ� ���� ���� �����ϱ�?	//PKLOOT

	pRoom->SetLootingStart();
//---------------------------------------
}


CLootingRoom* CLootingManager::GetLootingRoom( DWORD dwDiePlayer )
{
	return m_htLootingRoom.GetData( dwDiePlayer );
}


void CLootingManager::Loot( DWORD dwDiePlayer, int nArrayNum )
{
	CLootingRoom* pRoom = m_htLootingRoom.GetData( dwDiePlayer );

	if( pRoom->Loot( nArrayNum ) == FALSE )	//error
	{
		CloseLootingRoom( dwDiePlayer, TRUE );	//�÷��̾ ���߿� �Ѹ��� ���°���.
		return;
	}

//	if( pRoom->GetChance() <= 0 || pRoom->GetLootCount() <= 0 )
//	{
//		CloseLootingRoom( dwDiePlayer );
//	}
}


//---KES PK 071202
int CLootingManager::GetLootingChance( CPlayer* pDiePlayer, CPlayer* pAttackPlayer )
{
	int nLootingChance = 0;

	if( pDiePlayer->IsPKMode() )	//���� ����� PK��� �̸�
	{
		//���� �õ� ���� Ƚ��: INT(����� PK ��ġ / 150) + 3, MAX. = 5
		nLootingChance = 3 + pDiePlayer->GetBadFame() / 150;
	}
	else
	{
		//���� �õ� ���� Ƚ��: INT(������ PK ��ġ / 300) + 3, MAX. = 5		
		nLootingChance = 3 + pAttackPlayer->GetBadFame() / 300;
	}

	if( nLootingChance > 5 )	nLootingChance = 5;

	return nLootingChance;
}

//---KES PK 071202
int CLootingManager::GetLootingItemNum( CPlayer* pDiePlayer, CPlayer* pAttackPlayer )
{
	int nLootingItems = 0;

	if( pDiePlayer->IsPKMode() )	//���� ����� PK��� �̸�
	{
		//���� �õ� ���� Ƚ��: INT(����� PK ��ġ / 150) + 3, MAX. = 5
		nLootingItems = 1 + pDiePlayer->GetBadFame() / 150;
	}
	else
	{
		//���� ���� ���� Ƚ��: INT(������ PK ��ġ / 300) + 1, MAX. = 3		
		nLootingItems = 1 + pAttackPlayer->GetBadFame() / 300;
	}

	if( nLootingItems > 3 )	nLootingItems = 3;

	return nLootingItems;	
}


int CLootingManager::GetWearItemLootingRatio( DWORD dwBadFame )
{
	if( dwBadFame == 0 )
		return 0;
	if( dwBadFame < 50 )
		return 1;
	else if( dwBadFame < 4000 )
		return 10;
	else if( dwBadFame < 20000 )
		return 20;
	else if( dwBadFame < 80000 )
		return 30;
	else if( dwBadFame < 400000 )
		return 40;
	else if( dwBadFame < 1600000 )
		return 50;
	else if( dwBadFame < 8000000 )
		return 60;
	else if( dwBadFame < 32000000 )
		return 70;
	else if( dwBadFame < 100000000 )
		return 85;
	else if( dwBadFame < 500000000 )
		return 100;
	else
		return 100;
}

void CLootingManager::CloseLootingRoom( DWORD dwDiePlayer, BOOL bMsgToClient )
{
	CLootingRoom* pRoom = m_htLootingRoom.GetData( dwDiePlayer );
	if( pRoom )
	{
		pRoom->EndLootingRoomMsg( bMsgToClient );	//end Msg�� ������.
		m_htLootingRoom.Remove( dwDiePlayer );
		m_mpLootingRoom->Free( pRoom );
	}
}

BOOL CLootingManager::IsLootedPlayer( DWORD dwPlayerIdx )
{
	CLootingRoom* pRoom = m_htLootingRoom.GetData( dwPlayerIdx );
	if( pRoom )
		return TRUE;
	return FALSE;
}

void CLootingManager::DiePlayerExitPanelty( CPlayer* pPlayer )
{
	CLootingRoom* pRoom = m_htLootingRoom.GetData( pPlayer->GetID() );
	if( !pRoom )
		return;

	pRoom->DiePlayerExitPanelty();
}

void CLootingManager::ProcessTimeOutCheck()
{
	m_htLootingRoom.SetPositionHead();
	CLootingRoom* pRoom = NULL;
	while( (pRoom = m_htLootingRoom.GetData()) != NULL)
	{
		if( pRoom->IsTimeOut() )
		{
			m_htDeleteRoom.Add( pRoom, pRoom->GetDiePlayerID() );
		}
	}

	m_htDeleteRoom.SetPositionHead();
	while( (pRoom = m_htDeleteRoom.GetData()) != NULL)
	{
		CloseLootingRoom( pRoom->GetDiePlayerID(), TRUE );
	}
	m_htDeleteRoom.RemoveAll();
}

void CLootingManager::LootingCancel( CPlayer* pPlayer )
{
	DWORD	dwAttackerID = pPlayer->GetID();

	m_htLootingRoom.SetPositionHead();
	CLootingRoom* pRoom = NULL;
	while( (pRoom = m_htLootingRoom.GetData()) != NULL)
	{
		if( pRoom->GetAttackerID() == dwAttackerID )
		{
			m_htDeleteRoom.Add( pRoom, dwAttackerID );
		}
	}

	m_htDeleteRoom.SetPositionHead();
	while( (pRoom = m_htDeleteRoom.GetData()) != NULL)
	{
		CloseLootingRoom( pRoom->GetDiePlayerID(), TRUE );
	}
	m_htDeleteRoom.RemoveAll();
}