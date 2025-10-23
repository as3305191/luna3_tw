// Battle.cpp: implementation of the CBattle class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Battle.h"

#include "Player.h"
#include "BattleTeam.h"
#include "PartyManager.h"
#include "Party.h"

#include "PKManager.h"

#include "SiegeWarfareMgr.h"

// 080821 KTH -- SiegeDungeonMgr Include
#include "../[CC]SiegeDungeon/SiegeDungeonMgr.h"

#ifdef _MAPSERVER_
#include "Monster.h"
#include "pet.h"
#include "ItemDrop.h"
#include "ItemManager.h"
#include "../[cc]skill/server/manager/skillmanager.h"
#include "ChannelSystem.h"
#include "..\[CC]Quest\QuestEvent.h"
#include "QuestManager.h"
#include "GuildFieldWarMgr.h"
#include "./Guild.h"
#include "./GuildManager.h"
#include "UserTable.h"
#else
#include "ObjectManager.h"
#include "ChatManager.h"
#include "GuildFieldWar.h"
#include "ItemManager.h"
#include "MHMap.h"
#endif

CBattle::CBattle()
{
	m_BattleFlag = BATTLE_FLAG_NONE;
	m_bDestroyFlag = FALSE;
	
	ZeroMemory( m_TeamArray, sizeof(m_TeamArray) );
}

CBattle::~CBattle()
{
}
void CBattle::Initialize(BATTLE_INFO_BASE* pCreateInfo, CBattleTeam* pTeam1, CBattleTeam* pTeam2)
{	
	OnCreate(pCreateInfo,pTeam1,pTeam2);
	
	m_BattleInfo = *pCreateInfo;
	m_Team1 = pTeam1;
	m_Team2 = pTeam2;
	m_TeamObserver = NULL;

	m_bDestroyFlag = FALSE;

	m_WinnerTeam = eBattleTeam_Max;
	m_BattleFlag = BATTLE_FLAG_NONE;
	
}

void CBattle::Release()
{
	// 091231 ONS ��Ʋ������ �����Ѵ�.
	SAFE_DELETE(m_Team1);
	SAFE_DELETE(m_Team2);
}

void CBattle::StartBattle()
{
	OnFightStart();

#ifdef _MAPSERVER_

	MSG_DWORD msg;
	msg.Category = MP_BATTLE;
	msg.Protocol = MP_BATTLE_START_NOTIFY;
	msg.dwData = GetBattleID();

	for(int n=0;n<eBattleTeam_Max;++n)
	{
		if( m_TeamArray[n] )
			m_TeamArray[n]->SendTeamMsg(&msg,sizeof(msg));
	}

	// 090116 ShinJS --- ĳ���� �Ⱥ��̱� �ɼǿ� ���� ������ ���� ����
#else
	OBJECTMGR->ApplyOverInfoOptionToAll();

#endif
}

BOOL CBattle::IsEnemy(CObject* pOperator,CObject* pTarget)
{
	// �Լ� ���� Ȯ��.
	if( !pOperator || !pTarget ) return FALSE ;


	// ������ ������, �ƴ��� Ȯ���Ѵ�.
	BOOL IsSiegeWarning = SIEGEWARFAREMGR->IsSiegeWarfareZone(
#ifdef _MAPSERVER_
		g_pServerSystem->GetMapNum()
#else
		MAP->GetMapNum()
#endif
		);
	BOOL IsSiegeDungeon = SIEGEDUNGEONMGR->IsSiegeDungeon(
#ifdef _MAPSERVER_
		g_pServerSystem->GetMapNum()
#else
		MAP->GetMapNum()
#endif
		);

	if( IsSiegeWarning || IsSiegeDungeon )
	{
		return IsEnemy_SiegeWarning( pOperator, pTarget ) ;
	}
	else
	{
		return IsEnemy_NormalTimes( pOperator, pTarget ) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: IsEnemy_NormalTimes
//	DESC		: ������ ���� �ƴ� ���ÿ� ��/�Ʊ��� �Ǵ� �ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 07, 2008
//-------------------------------------------------------------------------------------------------
BYTE CBattle::IsEnemy_NormalTimes(CObject* pOperator,CObject* pTarget)
{
	// �Լ� ���� Ȯ��.
	if( !pOperator || !pTarget ) return FALSE ;


#ifdef _MAPSERVER_
	if( pTarget->GetState() == eObjectState_Immortal || pTarget->GetState() == eObjectState_None ) return FALSE ;
#endif // _MAPSERVER_

	const EObjectKind byOperatorType = pOperator->GetObjectKind();
	const EObjectKind byTargetType = pTarget->GetObjectKind();

	// �������� Ÿ���� Ȯ���Ѵ�.
	switch( byOperatorType )
	{
	// �����ڰ� ���� �̸�,
	case eObjectKind_Monster :
		{
			if(byTargetType & eObjectKind_Monster) return FALSE ;
		}
		break ;

	// �����ڰ� Player �̸�,
	case eObjectKind_Player : 
		{
			if( pTarget->GetObjectKind() & eObjectKind_Monster )	// ���ʹ� ���̴�
			{
				return TRUE;
			}
			else if( byTargetType == eObjectKind_Player )
			{
#ifdef _MAPSERVER_
				if( pTarget->GetState() == eObjectState_Immortal )	//�������´� ���� �ƴϴ�.
				return FALSE ;

				// �������� ��Ƽ�� Ȯ���Ѵ�.
				DWORD dwPartyIdx = 0 ;
				dwPartyIdx = ((CPlayer*)pOperator)->GetPartyIdx() ;
                
				// �����ڰ� ��Ƽ�� ���� ������,
				if( dwPartyIdx != 0 )
				{
					// ��Ƽ ������ �޴´�.
					CParty* pParty = NULL ;
					pParty = PARTYMGR->GetParty( dwPartyIdx ) ;

					// ��Ƽ ������ ��ȿ�ϸ�,
					if( pParty )
					{
						// ��Ƽ���� ���� �ƴϴ�.
						if( pParty->IsPartyMember(pTarget->GetID()) ) return FALSE ;
					}
				}

				DWORD dwGuildIdx = 0 ;
				dwGuildIdx = ((CPlayer*)pOperator)->GetGuildIdx() ;

				// �����ڰ� ��忡 ���� ������,
				if( dwGuildIdx != 0 )
				{
					// ��� ������ �޴´�.
					CGuild* pGuild = NULL ;
					pGuild = GUILDMGR->GetGuild( dwGuildIdx ) ;

					// ��� ������ ��ȿ�ϸ�,
					if( pGuild )
					{
						// ������ ���� �ƴϴ�.
						if( pGuild->IsMember(pTarget->GetID()) ) return FALSE ;

						// ���ͱ�嵵 ���� �ƴϴ�.
						 DWORD dwOperUnionGuildIdx = ((CPlayer*)pOperator)->GetGuildUnionIdx();
						 DWORD dwTargetUnionGuildIdx = ((CPlayer*)pTarget)->GetGuildUnionIdx();

						 if( dwOperUnionGuildIdx && dwTargetUnionGuildIdx &&
							 dwOperUnionGuildIdx == dwTargetUnionGuildIdx	)
							 return FALSE ;
					}
				}

				// Pk�� ���ǰų� �������ֹ����� ������̸�
				if( PKMGR->IsPKAllow() )
				{
					if( GUILDWARMGR->IsEnemyInField( (CPlayer*)pOperator, (CPlayer*)pTarget ) == TRUE )
						return TRUE;
				}
#else
				if( pOperator == HERO )
				{
					// ��Ƽ���� ���� �ƴϴ�.
					if( PARTYMGR->IsPartyMember( pTarget->GetID() ) ) return FALSE ;
					if( GUILDWARMGR->IsEnemy( (CPlayer*)pTarget ) ) return TRUE ;
				}
#endif
				//---KES CHEAT PKEVENT �Ʒ� �ּ�ó���Ͽ� ������� �ƴ϶� �̺�Ʈ �����ϵ���
				// if( PKMGR->IsPKAllow() )
				{
					if( ((CPlayer*)pOperator)->IsPKMode() )				
					{
						if(pOperator == pTarget ) return FALSE ;
						//�ڽ��� PK����̸� �ڽ��� �� �������� ���̴�.
						else return TRUE ;
					}

					//��밡 PK����̸� ���̴�.
					if( ((CPlayer*)pTarget)->IsPKMode() ) return TRUE ;
				}

				return FALSE;

			}
			else if(pTarget->GetObjectKind() == eObjectKind_SkillObject)
			{
				return TRUE;
			}
			else if(pTarget->GetObjectKind() == eObjectKind_Pet )
			{
	#ifdef _MAPSERVER_
				CObject* const ownerObject = g_pUserTable->FindUser(
					pTarget->GetOwnerIndex());

				if(0 == ownerObject)
				{
					return FALSE;
				}

				return BYTE(IsEnemy(
					pOperator,
					ownerObject));
	#else
				if( ( ( CPet* )pTarget )->GetMaster() )
				{
					return BYTE(IsEnemy( pOperator, ( ( CPet* )pTarget )->GetMaster() ));
				}
				else
					return FALSE;
	#endif
			}
			else	//�������� ���� �ƴϴ�.
			{
				return FALSE;
			}
		}
		break ;
	// �����ڰ� �� �̸�,
	case eObjectKind_Pet :
		{
#ifdef _MAPSERVER_
			CObject* const ownerObject = g_pUserTable->FindUser(
				pTarget->GetOwnerIndex());

			if(0 == ownerObject)
			{
				return FALSE;
			}

			return BYTE(IsEnemy(
				ownerObject,
				pTarget));
#else
			if( ( ( CPet* )pOperator )->GetMaster() )
			{
				return BYTE(IsEnemy( ( ( CPet* )pOperator )->GetMaster(), pTarget ));
			}
			else
				return FALSE;
#endif
		}
		break ;

	default : break ;
	}


	// �������� ��� ��.
	return TRUE ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: IsEnemy_SiegeWarning
//	DESC		: ������ �߿� ��/�Ʊ��� �Ǵ��ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 07, 2008
//-------------------------------------------------------------------------------------------------
BYTE CBattle::IsEnemy_SiegeWarning(CObject* pOperator,CObject* pTarget)
{
	// �Լ� ���� Ȯ��.
	if( !pOperator || !pTarget ) return FALSE ;


#ifdef _MAPSERVER_
	if( pTarget->GetState() == eObjectState_Immortal || pTarget->GetState() == eObjectState_None ) return FALSE ;
#else
	if( pTarget->GetState() == eObjectState_Immortal ) return FALSE ;
#endif // _MAPSERVER_

	const EObjectKind byOperatorType = pOperator->GetObjectKind();
	const EObjectKind byTargetType = pTarget->GetObjectKind();

	switch( byOperatorType )
	{
	// �����ڰ� ���� �̸�,
	case eObjectKind_Monster :
		{
			if(byTargetType & eObjectKind_Monster) return FALSE ;
		}
		break ;

	// �����ڰ� Player �̸�,
	case eObjectKind_Player : 
		{
            if( pTarget->GetObjectKind() & eObjectKind_Monster )	// ���ʹ� ���̴�
			{
				return TRUE;
			}
			else if( byTargetType == eObjectKind_Player )
			{
				// ���� + ��Ƽ���� ���̾ƴϰ� ��� ������ �����Ѵ�.
#ifdef _MAPSERVER_
				if( pTarget->GetState() == eObjectState_Immortal )	//�������´� ���� �ƴϴ�.
				return FALSE ;

				// �������� ��Ƽ�� Ȯ���Ѵ�.
				DWORD dwPartyIdx = 0 ;
				dwPartyIdx = ((CPlayer*)pOperator)->GetPartyIdx() ;
                
				// �����ڰ� ��Ƽ�� ���� ������,
				if( dwPartyIdx != 0 )
				{
					// ��Ƽ ������ �޴´�.
					CParty* pParty = NULL ;
					pParty = PARTYMGR->GetParty( dwPartyIdx ) ;

					// ��Ƽ ������ ��ȿ�ϸ�,
					if( pParty )
					{
						// ��Ƽ���� ���� �ƴϴ�.
						if( pParty->IsPartyMember(pTarget->GetID()) ) return FALSE ;
					}
				}

				// �������� ��带 Ȯ���Ѵ�.
				DWORD dwGuildIdx = 0 ;
				dwGuildIdx = ((CPlayer*)pOperator)->GetGuildIdx() ;

				// �����ڰ� ��忡 ���� ������,
				if( dwGuildIdx != 0 )
				{
					// ��� ������ �޴´�.
					CGuild* pGuild = NULL ;
					pGuild = GUILDMGR->GetGuild( dwGuildIdx ) ;

					// ��� ������ ��ȿ�ϸ�,
					if( pGuild )
					{
						// ������ ���� �ƴϴ�.
						if( pGuild->IsMember(pTarget->GetID()) ) return FALSE ;

						// ���ͱ�嵵 ���� �ƴϴ�.
						 DWORD dwOperUnionGuildIdx = ((CPlayer*)pOperator)->GetGuildUnionIdx();
						 DWORD dwTargetUnionGuildIdx = ((CPlayer*)pTarget)->GetGuildUnionIdx();

						 if( dwOperUnionGuildIdx && dwTargetUnionGuildIdx &&
							 dwOperUnionGuildIdx == dwTargetUnionGuildIdx	)
							 return FALSE ;
					}
				}
#else
				if( pOperator == HERO )
				{
					// ��Ƽ���� ���� �ƴϴ�.
					if( PARTYMGR->IsPartyMember( pTarget->GetID() ) ) return FALSE ;

					// ������ ���� �ƴϴ�.

					DWORD dwHeroGuildIdx = ((CPlayer*)pOperator)->GetGuildIdx() ;
					if( dwHeroGuildIdx )
					{
						if( dwHeroGuildIdx == ((CPlayer*)pTarget)->GetGuildIdx() ) return FALSE ;

						// ���ͱ�嵵 ���� �ƴϴ�.
						 DWORD dwOperUnionGuildIdx = ((CPlayer*)pOperator)->GetGuildUnionIdx();
						 DWORD dwTargetUnionGuildIdx = ((CPlayer*)pTarget)->GetGuildUnionIdx();

						 if( dwOperUnionGuildIdx && dwTargetUnionGuildIdx &&
							 dwOperUnionGuildIdx == dwTargetUnionGuildIdx	)
							 return FALSE ;
					}
					else
					{
						if( HEROID == pTarget->GetID() ) return FALSE ;
						else return TRUE ;
					}
				}

				return TRUE ;
#endif
			}
			else if(pTarget->GetObjectKind() == eObjectKind_SkillObject)
			{
				return TRUE;
			}
			else if(pTarget->GetObjectKind() == eObjectKind_Pet )
			{
	#ifdef _MAPSERVER_
				CObject* const ownerObject = g_pUserTable->FindUser(
					pTarget->GetOwnerIndex());

				if(0 == ownerObject)
				{
					return FALSE;
				}

				return BYTE(IsEnemy(
					pOperator,
					ownerObject));
	#else
				if( ( ( CPet* )pTarget )->GetMaster() )
				{
					return BYTE(IsEnemy( pOperator, ( ( CPet* )pTarget )->GetMaster() ));
				}
				else
					return FALSE;
	#endif
			}
			else	//�������� ���� �ƴϴ�.
			{
				return FALSE;
			}
		}
		break ;

	// �����ڰ� �� �̸�,
	case eObjectKind_Pet :
		{
#ifdef _MAPSERVER_
			CObject* const ownerObject = g_pUserTable->FindUser(
				pTarget->GetOwnerIndex());

			if(0 == ownerObject)
			{
				return FALSE;
			}

			return BYTE(IsEnemy(
				ownerObject,
				pTarget));
#else
			if( ( ( CPet* )pOperator )->GetMaster() )
			{
				return BYTE(IsEnemy( ( ( CPet* )pOperator )->GetMaster(), pTarget ));
			}
			else
				return FALSE;
#endif
		}
		break ;
	}


	// �������� ��� ��.
	return TRUE ;
}

DWORD CBattle::GetBattleTeamID( CObject* pObject )
{
	for(int n=0; n<eBattleTeam_Max; ++n)
	{
		if( m_TeamArray[n] )
		if( m_TeamArray[n]->IsTeamMember( pObject ) )
			return n+1;
	}

	return 0;
}

#ifdef _MAPSERVER_
BOOL CBattle::AddObjectToBattle(CObject* pObject)
{
	for(int n=0;n<eBattleTeam_Max;++n)
	{
		if( m_TeamArray[n] )
		if( m_TeamArray[n]->IsAddableTeamMember(pObject))
		{
			pObject->SetBattle(GetBattleID(),(BYTE)n);
			m_TeamArray[n]->AddTeamMember(pObject);

			MSG_BATTLE_TEAMMEMBER_ADDDELETE msg;
			SetProtocol(&msg,MP_BATTLE,MP_BATTLE_TEAMMEMBER_ADD_NOTIFY);
			msg.Member.set(pObject->GetID(),pObject->GetObjectName());
			msg.Team = (BYTE)n;
			ToEachTeam(pTeam)
				pTeam->SendTeamMsg(&msg,sizeof(msg));
			EndToEachTeam

			OnTeamMemberAdd(n,pObject->GetID(),pObject->GetObjectName());

			if(pObject->GetObjectKind() == eObjectKind_Player)
				SendBattleInfo((CPlayer*)pObject);
			
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CBattle::IsAddableBattle(CObject* pObject)
{
	for(int n=0;n<eBattleTeam_Max;++n)
	{
		if(m_TeamArray[n])
		if(m_TeamArray[n]->IsAddableTeamMember(pObject))
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CBattle::DeleteObjectFromBattle(CObject* pObject)
{
	ToEachTeam(pTeam)
		if(pTeam->DeleteTeamMember(pObject) == TRUE)
		{
			pObject->SetBattle(pObject->GetGridID(),0);
			
			if(OnTeamMemberDelete(TeamNumber,pObject->GetID(),pObject->GetObjectName()) == TRUE)
			{				
				MSG_BATTLE_TEAMMEMBER_ADDDELETE msg;
				SetProtocol(&msg,MP_BATTLE,MP_BATTLE_TEAMMEMBER_DELETE_NOTIFY);
				msg.Member.set(pObject->GetID(),pObject->GetObjectName());
				msg.Team = (BYTE)TeamNumber;
				ToEachTeam(pTeam)
					pTeam->SendTeamMsg(&msg,sizeof(msg));
				EndToEachTeam
			}			
			
			return TRUE;
		}
	EndToEachTeam

	return FALSE;
}

void CBattle::SendBattleInfo(CPlayer* pPlayer)
{
	static MSG_BATTLE_INFO msg;
	SetProtocol(&msg,MP_BATTLE,MP_BATTLE_INFO);
	GetBattleInfo(msg.info,&msg.msglen);	
	pPlayer->SendMsg(&msg,msg.GetMsgLength());
}


void CBattle::GetBattleInfo(char* pInfo,WORD* size)
{
	memcpy(pInfo,&m_BattleInfo,sizeof(BATTLE_INFO_BASE));
	*size = sizeof(BATTLE_INFO_BASE);
}

DWORD CBattle::GetTeamMemberNum(int i)
{
	if( m_TeamArray[i] ) 
		return m_TeamArray[i]->GetMemberNum();

	return 0;
}
#endif




// event func
void CBattle::OnCreate(BATTLE_INFO_BASE* pCreateInfo, CBattleTeam* pTeam1, CBattleTeam* pTeam2)
{
#ifdef _MAPSERVER_
	pCreateInfo->BattleTime = gCurTime;
	pCreateInfo->BattleState = eBATTLE_STATE_READY;
#else
	pCreateInfo->BattleTime = pCreateInfo->BattleTime;
	pCreateInfo->BattleState = pCreateInfo->BattleState;
	pCreateInfo->BattleTime = gCurTime;
#endif

	//pCreateInfo->BattleState = eBATTLE_STATE_READY;
}
void CBattle::OnFightStart()
{
	m_BattleInfo.BattleTime = gCurTime;
	m_BattleInfo.BattleState = eBATTLE_STATE_FIGHT;
}
void CBattle::OnDestroy()
{
#ifdef _MAPSERVER_
	MSGBASE msg;
	SetProtocol(&msg,MP_BATTLE,MP_BATTLE_DESTROY_NOTIFY);
	ToEachTeam(pTeam)
		pTeam->SendTeamMsg(&msg,sizeof(msg));
		YHTPOSITION pos = pTeam->GetPositionHead();
		CObject* pObject = NULL ;
		while((pObject = pTeam->GetNextTeamMember(&pos))!= NULL)
		{
			pTeam->DeleteTeamMember(pObject);
		}
	EndToEachTeam
#endif
}

void CBattle::OnMonsterDistribute(CMonster* pMonster,CObject* pKiller)
{
#ifdef _MAPSERVER_
	pMonster->Drop();
	
	if(pKiller->GetObjectKind() == eObjectKind_Player)
	{
		CQuestEvent qe;
		// 100104 LUJ, ���� �޼ҵ带 �����ϵ��� ����
		qe.SetValue(
			eQuestEvent_Hunt,
			pMonster->GetMonsterKind(),
			0);
		QUESTMGR->AddQuestEvent( (CPlayer*)pKiller, &qe );
		//
		((CPlayer*)pKiller)->SetKillMonsterLevel( pMonster->GetLevel() );
	}
	else if(pKiller->GetObjectKind() == eObjectKind_Pet)
	{
		CPlayer* const pPlayer = (CPlayer*)g_pUserTable->FindUser(
			pKiller->GetOwnerIndex());

		if(0 == pPlayer)
		{
			return;
		}
		else if(eObjectKind_Player != pPlayer->GetObjectKind())
		{
			return;
		}

		CQuestEvent qe;
		// 100104 LUJ, ���� �޼ҵ带 �����ϵ��� ����
		qe.SetValue(
			eQuestEvent_Hunt,
			pMonster->GetMonsterKind(),
			0);
		QUESTMGR->AddQuestEvent( pPlayer, &qe );
		//
		pPlayer->SetKillMonsterLevel( pMonster->GetLevel() );
	}
#endif
}

void CBattle::Victory(int WinnerTeamNum,int LoserTeamNum)
{
#ifdef _MAPSERVER_
	MSG_DWORD2 msg;
	msg.Category = MP_BATTLE;
	msg.Protocol = MP_BATTLE_VICTORY_NOTIFY;
	msg.dwData1 = WinnerTeamNum;
	msg.dwData2 = LoserTeamNum;
	
	ToEachTeam(pTeam)
		pTeam->SendTeamMsg(&msg,sizeof(msg));
	EndToEachTeam

#endif

	m_WinnerTeam = WinnerTeamNum;
	m_BattleInfo.BattleState = eBATTLE_STATE_RESULT;
	m_BattleInfo.BattleTime = gCurTime;
}
void CBattle::Draw()
{
#ifdef _MAPSERVER_
	MSGBASE msg;
	msg.Category = MP_BATTLE;
	msg.Protocol = MP_BATTLE_DRAW_NOTIFY;
	
	ToEachTeam(pTeam)
		pTeam->SendTeamMsg(&msg,sizeof(msg));
	EndToEachTeam
#endif
		
	m_WinnerTeam = eBattleTeam_Max;
	m_BattleInfo.BattleState = eBATTLE_STATE_RESULT;
	m_BattleInfo.BattleTime = gCurTime;
}