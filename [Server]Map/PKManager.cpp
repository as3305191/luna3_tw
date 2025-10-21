// PKManager.cpp: implementation of the CPKManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PKManager.h"
#include "UserTable.h"
#include "PackedData.h"
#include "Player.h"
#include "LootingManager.h"
#include "MapDBMsgParser.h"
#include "ItemManager.h"
#include "SkillTreeManager.h"
#include "GridTable.h"

#include "..\[CC]Header\GameResourceManager.h"
#include "GuildFieldWarMgr.h"
#include "Battle.h"
#include "QuestMapMgr.h"

// 080918 --- �����Ŵ��� ��� ȣ��
#include "./SiegeWarfareMgr.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPKManager::CPKManager()
{
}

CPKManager::~CPKManager()
{

}

void CPKManager::Init( BOOL bPKAllow )
{
	m_bPKAllow = bPKAllow;

//---KES CHEAT PKEVENT
	m_bPKEvent = FALSE;
//-------------------
}

void CPKManager::SetPKAllowWithMsg( BOOL bAllow )
{
	if( m_bPKAllow == bAllow ) return;
	m_bPKAllow = bAllow;
	
	MSG_BYTE msg;
	msg.Category	= MP_PK;
	msg.Protocol	= MP_PK_PKALLOW_CHANGED;
	
	CObject* pObject = NULL;
	g_pUserTable->SetPositionUserHead();
	while( (pObject = g_pUserTable->GetUserData() )!= NULL)
	{
		if( pObject->GetObjectKind() != eObjectKind_Player ) continue;

		if( bAllow == FALSE )
		{
			if( ((CPlayer*)pObject)->IsPKMode() )
				((CPlayer*)pObject)->PKModeOffForce();
			//�����ϴ��͵� ĵ��?
		}

		msg.bData		= BYTE( bAllow );
						
		((CPlayer*)pObject)->SendMsg( &msg, sizeof( msg ) );
	}
}

void CPKManager::DiePanelty( CPlayer* pDiePlayer, CObject* pAttacker )
{
	// questmap
	if( QUESTMAPMGR->IsQuestMap() )
		return;
	//�񹫽ÿ� �״� �г�Ƽ�� ����. �񹫷� �����ϰ� �׾���.
	if( pDiePlayer->m_bNeedRevive == FALSE )
		return;
	// RaMa - 05.05.20 �������ÿ� �׾ �г�Ƽ ����
	if( pDiePlayer->GetBattle()->GetBattleKind() == eBATTLE_KIND_GTOURNAMENT )
		return;

	// 080918 KTH -- ������ �������� �׾ �г�Ƽ ����
	if( SIEGEWARFAREMGR->IsSiegeWarfareZone() )
		return;

	//�Ǹ�ġ ������ ������ �޴� �г�Ƽ.
	if( pDiePlayer->IsPKMode() )
	{
		pDiePlayer->SetDieForGFW( FALSE );	// PK����̸� �����ʵ������� ������ �ƴϴ�
		PKModeDiePanelty( pDiePlayer );
		PKModeTimeReduceByDie( pDiePlayer );
	}
	else
	{
		NoPKModeDiePanelty( pDiePlayer );
	}

	if( pAttacker->GetObjectKind() != eObjectKind_Player )
		return;

	//PK�ϸ� �޴� �Ǹ�ġ
	PKPlayerKillPanelty( pDiePlayer, (CPlayer*)pAttacker );

	//ô����ϸ� �޴� �г�Ƽ.
	//PKPlayerKiiledByWantedPanelty( pDiePlayer, (CPlayer*)pAttacker );	//---KES PK 071202 ���� ���� �ȵǾ���
}


void CPKManager::PKPlayerKillPanelty( CPlayer* pDiePlayer, CPlayer* pAttacker )
{
	if( pAttacker->IsPKMode() )
	{
		if( !pDiePlayer->IsPKMode() )
		{
			//---KES PK 071124
			int AddBadFame = pAttacker->GetLevel() < pDiePlayer->GetLevel() ?
						  pDiePlayer->GetLevel() - pAttacker->GetLevel() + 15 :
						  ( pAttacker->GetLevel() - pDiePlayer->GetLevel() ) * 2 + 15;
			
			pAttacker->SetBadFame( pAttacker->GetBadFame() + AddBadFame );
			//----------------
			
			BadFameCharacterUpdate( pAttacker->GetID(), pAttacker->GetBadFame() );

			{
				MSG_FAME msg;
				msg.Category = MP_CHAR;
				msg.Protocol = MP_CHAR_BADFAME_NOTIFY;
				msg.dwObjectID = pAttacker->GetID();
				msg.Fame = pAttacker->GetBadFame();

				PACKEDDATA_OBJ->QuickSend(
					pAttacker,
					&msg,
					sizeof(msg));
			}

			pAttacker->SetPKContinueTime( pAttacker->GetPKContinueTime() + 10*60*1000 );	//��� ���ӽð� ���� 10��	//---KES PK 071124

			//MSG������
		}
	}
}

//---KES PK 071202
void CPKManager::PKModeDiePanelty( CPlayer* pPlayer )	//pk��� �϶�
{
	if( !pPlayer ) return;
// ��ȹ����(071201)
// ���ҵǴ� ����ġ = 3 * (INT(����� PK ��ġ / 75) + 2), MAX. = 50	//--- ��ȹ���� 5* �� 3* ���� 2008.1.4
// ���ҵǴ� PK ��� ����Ʈ Ÿ�� = 10�� / ȸ

	//---�߿�! ����ġ ��ȣ �ֹ��� ���� ����ġ ��ȣ �ȴ�!
	//---����ġ ��ȣ �ֹ��� ó�� �߰� �ؾ���.

	DWORD dwBadFame = pPlayer->GetBadFame();

	if( dwBadFame > 0 )
	{
		EXPTYPE pointForLevel	= GAMERESRCMNGR->GetMaxExpPoint( pPlayer->GetLevel() );

		float fPenaltyRate		= (float)( 3 * ( ( dwBadFame / 75 ) + 2 ) ) / 100.f;
		if( fPenaltyRate > .50f ) fPenaltyRate = .50f;	//MAX 50%
	
		EXPTYPE dwPenaltyExp	= EXPTYPE((float)pointForLevel * fPenaltyRate);

		if( dwPenaltyExp )
		{
			pPlayer->ReduceExpPoint( dwPenaltyExp , eExpLog_LoseByPKMode );

			// 080602 LYW --- Player : ����ġ ��ġ (__int32) ���� (__int64) ������� ���� ó��.
			//MSG_DWORD2 msg;
			//msg.Category	= MP_PK;
			//msg.Protocol	= MP_PK_EXPPENALTY;
			//msg.dwData1		= dwPenaltyExp;
			//msg.dwData2		= 1;	//���� -  PK��� Ų����
			//pPlayer->SendMsg( &msg, sizeof(msg) );

			MSG_DWORDEX2 msg;
			msg.Category	= MP_PK;
			msg.Protocol	= MP_PK_EXPPENALTY;
			msg.dweData1	= dwPenaltyExp;
			msg.dweData2	= 1;	//���� -  PK��� Ų����
			pPlayer->SendMsg( &msg, sizeof(msg) );
		}
	}

	//---������ �ı�Ȯ���� ����.
}

//---KES PK 071202
void CPKManager::PKModeTimeReduceByDie( CPlayer* pPlayer )
{
	//---������ ���� PK��� ���ӽð� ����
	DWORD dwPKContinueTime = pPlayer->GetPKContinueTime();
	if( dwPKContinueTime == 0 ) return;

    if( dwPKContinueTime > 10*60*1000 )
		dwPKContinueTime -= 10*60*1000;
	else
		dwPKContinueTime = 0;

	pPlayer->SetPKContinueTime( dwPKContinueTime );	
	//-----------------------------------
}

//---KES PK 071202
void CPKManager::NoPKModeDiePanelty( CPlayer* pPlayer ) //PKMode �ƴҶ�
{
	if( !pPlayer ) return;

	//---�߿�! ����ġ ��ȣ �ֹ��� ���� ����ġ ��ȣ �ȴ�!
	//---����ġ ��ȣ �ֹ��� ó�� �߰� �ؾ���.

	//��� �� ���ҵǴ� �߰� ����ġ = 2 * (INT(����� PK ��ġ / 150) + 1), MAX. = 25		//---5* �� 2*�� ��ȹ���� 2008.1.4
	DWORD dwBadFame = pPlayer->GetBadFame();


	if( dwBadFame > 0 )
	{
		EXPTYPE pointForLevel	= GAMERESRCMNGR->GetMaxExpPoint( pPlayer->GetLevel() );

		float fPenaltyRate		= (float)( 1 * ( dwBadFame / 150 ) ) / 100.f; //(float)( 5 * ( ( dwBadFame / 150 ) ) ) / 100.f; <--�����Ͽ� 1��		PK 0227
		if( fPenaltyRate > .25f ) fPenaltyRate = .25f;	//MAX 25%
	
		EXPTYPE dwPenaltyExp	= EXPTYPE((float)pointForLevel * fPenaltyRate);

		if( dwPenaltyExp )
		{
			pPlayer->ReduceExpPoint( dwPenaltyExp , eExpLog_LosebyBadFame );

			// 080602 LYW --- Player : ����ġ ��ġ (__int32) ���� (__int64) ������� ���� ó��.
			//MSG_DWORD2 msg;
			//msg.Category	= MP_PK;
			//msg.Protocol	= MP_PK_EXPPENALTY;
			//msg.dwData1		= dwPenaltyExp;
			//msg.dwData2		= 0; //����	- PK��� ��Ų ����
			//pPlayer->SendMsg( &msg, sizeof(msg) );

			MSG_DWORDEX2 msg;
			msg.Category	= MP_PK;
			msg.Protocol	= MP_PK_EXPPENALTY;
			msg.dweData1	= dwPenaltyExp;
			msg.dweData2	= 0; //����	- PK��� ��Ų ����
			pPlayer->SendMsg( &msg, sizeof(msg) );
		}
	}
}

//---KES PK 071202
void CPKManager::NetworkMsgParse( BYTE Protocol, void* pMsg )
{
	switch( Protocol )
	{
	case MP_PK_PKON_SYN:
		{
			MSGBASE* pmsg = (MSGBASE*)pMsg;

			CPlayer* pPlayer	= (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if( !pPlayer ) break;

//---KES PKEVENT
			if( !IsPKAllow() )
			{
				if( pPlayer->GetUserLevel() <= eUSERLEVEL_GM )	//GM���� �̻��̸�
				{
					if( !IsPKEvent() )
					{
						MSG_BYTE msg;
						msg.Category	= MP_PK;
						msg.Protocol	= MP_PK_PKON_NACK;
						msg.bData		= ePKCODE_NOTALLAW;
						pPlayer->SendMsg( &msg, sizeof(msg) );
						break;
					}
				}
				else
				{
					MSG_BYTE msg;				//error code?
					msg.Category	= MP_PK;
					msg.Protocol	= MP_PK_PKON_NACK;
					msg.bData		= ePKCODE_NOTALLAW;
					pPlayer->SendMsg( &msg, sizeof(msg) );
					break;
				}
			}
//--------------

			int rt;
			if( ( rt = pPlayer->PKModeOn() ) == ePKCODE_OK )
			{
				MSG_DWORD msg;
				msg.Category	= MP_PK;
				msg.Protocol	= MP_PK_PKON_ACK;
				msg.dwData		= pmsg->dwObjectID;
				msg.dwObjectID	= pPlayer->GetID();

				PACKEDDATA_OBJ->QuickSend( pPlayer, &msg, sizeof(msg) );
			}
			else
			{
				MSG_BYTE msg;
				msg.Category	= MP_PK;
				msg.Protocol	= MP_PK_PKON_NACK;
				msg.bData		= BYTE(rt);
				pPlayer->SendMsg( &msg, sizeof(msg) );
			}
		}
		break;

	case MP_PK_PKOFF_SYN:
		{
			MSGBASE* pmsg = (MSGBASE*)pMsg;

			CPlayer* pPlayer	= (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if( !pPlayer ) break;

			if( pPlayer->PKModeOff() )
			{
				MSG_DWORD msg;
				msg.Category	= MP_PK;
				msg.Protocol	= MP_PK_PKOFF_ACK;
				msg.dwData		= pmsg->dwObjectID;
				msg.dwObjectID	= pPlayer->GetID();

				PACKEDDATA_OBJ->QuickSend( pPlayer, &msg, sizeof(msg) );
			}
			else
			{
				MSGBASE msg;
				msg.Category	= MP_PK;
				msg.Protocol	= MP_PK_PKOFF_NACK;
				pPlayer->SendMsg( &msg, sizeof(msg) );
			}
		}
		break;
		
	case MP_PK_LOOTING_SELECT_SYN:
		{
			MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
			CLootingRoom* pRoom = LOOTINGMGR->GetLootingRoom( pmsg->dwData1 );
			if( pRoom )
			{
				LOOTINGMGR->Loot( pmsg->dwData1, pmsg->dwData2 );
				
			}
			else
			{
				CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
				if( pPlayer )
				{
					MSG_DWORD2 msg;
					msg.Category	= MP_PK;
					msg.Protocol	= MP_PK_LOOTING_ERROR;
					msg.dwData1		= pmsg->dwData1;
					msg.dwData2		= eLOOTINGERROR_NO_LOOTINGROOM;
					pPlayer->SendMsg( &msg, sizeof(msg) );
				}
			}
		}
		break;
	
	case MP_PK_LOOTING_ENDLOOTING:
		{
			MSG_DWORD*	pmsg = (MSG_DWORD*)pMsg;
			LOOTINGMGR->CloseLootingRoom( pmsg->dwData, FALSE );
		}
	}	
}
