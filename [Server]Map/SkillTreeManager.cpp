#include "stdafx.h"
#include "SkillTreeManager.h"
#include "MapDBMsgParser.h"
#include "Player.h"
#include "UserTable.h"
#include "../[cc]skill/server/manager/skillmanager.h"
#include "../[cc]skill/server/info/activeskillinfo.h"
#include "../[cc]skill/Server/Tree/SkillTree.h"

CSkillTreeManager::CSkillTreeManager()
{

}

CSkillTreeManager::~CSkillTreeManager()
{

}
void CSkillTreeManager::NetworkMsgParse( BYTE Protocol, void* pMsg )
{
	switch(Protocol)
	{
	case MP_SKILLTREE_UPDATE_SYN:
		{
			const MSG_SKILL_UPDATE* const pmsg = (MSG_SKILL_UPDATE*)pMsg;
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);

			if(0 == pPlayer)
			{
				break;
			}
			else if(eObjectKind_Player != pPlayer->GetObjectKind())
			{
				break;
			}
			else if(pPlayer->IsResetSkill())
			{
				break;
			}
			else if(FALSE == SKILLMGR->IsLearnableSkill(pPlayer->GetCharacterTotalInfo(), pmsg->SkillBase))
			{
				break;
			}

			const cActiveSkillInfo* const activeSkillInfo = SKILLMGR->GetActiveInfo(pmsg->SkillBase.wSkillIdx + pmsg->SkillBase.Level);

			if(0 == activeSkillInfo)
			{
				break;
			}

			DWORD point = pPlayer->GetSkillPoint();
			DWORDEX money = pPlayer->GetMoney();

			// ���ڸ��� ����
			if( point < activeSkillInfo->GetInfo().TrainPoint ||
				money < activeSkillInfo->GetInfo().TrainMoney )
			{
				UpdateNack( pPlayer, 0 );
				return;
			}
			// 080310 LUJ, ���� ������ �������� �˻�
			else if( pPlayer->GetLevel() < activeSkillInfo->GetInfo().RequiredPlayerLevel )
			{
				UpdateNack( pPlayer, 0 );
				return;
			}
			else if ( pmsg->SkillBase.wSkillMoney != activeSkillInfo->GetInfo().TrainMoney ||
				pmsg->SkillBase.wSkillPoint != activeSkillInfo->GetInfo().TrainPoint )
			{
				UpdateNack(pPlayer, 0);
				return;
			}
			else if( const SKILL_BASE* skillChecker = pPlayer->GetSkillTree().GetData( pmsg->SkillBase.wSkillIdx ) )
			{
				if( activeSkillInfo->GetIndex() < ( skillChecker->wSkillIdx - 1 + ( skillChecker->Level > 1 ? skillChecker->Level : 1 ) ) )
				{
					UpdateNack(pPlayer, 0);
					return;
				}

				if( activeSkillInfo->GetIndex() - ( skillChecker->wSkillIdx - 1 + ( skillChecker->Level > 1 ? skillChecker->Level : 1 ) ) > 1 )
				{
					UpdateNack(pPlayer, 0);
					return;
				}
			}

			const SKILL_BASE* const oldSkill = pPlayer->GetSkillTree().GetData(
				pmsg->SkillBase.wSkillIdx);

			if(0 == oldSkill ||
				0 == oldSkill->mLearnedLevel)
			{
				point -= activeSkillInfo->GetInfo().TrainPoint;

				pPlayer->GetHeroTotalInfo()->SkillPoint = point;
				pPlayer->SetMoney(
					activeSkillInfo->GetInfo().TrainMoney,
					MONEY_SUBTRACTION);

				SkillPointUpdate(
					pPlayer->GetID(),
					point );

				SkillMoneyUpdate(
					pPlayer->GetID(), 
					activeSkillInfo->GetInfo().TrainMoney);

				SkillInsertToDB(
					pPlayer->GetID(),
					pmsg->SkillBase.wSkillIdx,
					1);

				LogItemMoney(
					pPlayer->GetID(),
					pPlayer->GetObjectName(),
					pmsg->SkillBase.wSkillIdx,
					"lvl:1",
					eLog_ItemSkillLearn,
					pPlayer->GetMoney(),
					0,
					activeSkillInfo->GetInfo().TrainMoney,
					0,
					0,
					0,
					0,
					0,
					0);
				break;
			}

			if( oldSkill && oldSkill->dwDBIdx != 0 )
			{
				point -= activeSkillInfo->GetInfo().TrainPoint;

				pPlayer->GetHeroTotalInfo()->SkillPoint = point;
				pPlayer->SetMoney(
					activeSkillInfo->GetInfo().TrainMoney,
					MONEY_SUBTRACTION);

				SkillPointUpdate(
					pPlayer->GetID(),
					point );

				SkillMoneyUpdate(
					pPlayer->GetID(), 
					activeSkillInfo->GetInfo().TrainMoney);

				SKILL_BASE skill = *oldSkill;
				skill.Level = skill.Level + 1;
				skill.mLearnedLevel = skill.mLearnedLevel + 1;
				pPlayer->GetSkillTree().Update(
					skill);

				SkillUpdateToDB(
					&skill,
					pPlayer->GetID());
				InsertLogSkill(
					pPlayer,
					&skill,
					eLog_SkillLevelup);
				UpdateAck(
					pPlayer,
					&skill);

				char text[MAX_PATH] = { 0 };
				sprintf(
					text,
					"lvl:%d",
					skill.mLearnedLevel);
				LogItemMoney(
					pPlayer->GetID(),
					pPlayer->GetObjectName(),
					skill.wSkillIdx,
					text,
					eLog_ItemSkillLearn,
					pPlayer->GetMoney(),
					0,
					activeSkillInfo->GetInfo().TrainMoney,
					0,
					0,
					0,
					0,
					0,
					0);
			} else {
				UpdateNack(pPlayer, 0);
				return;
			}
		}
		break;
	} 
}


void CSkillTreeManager::UpdateSkill( SKILL_BASE* pSkillBase, CPlayer* pPlayer )
{
	// 071231 KTH -- GetID Add
	SkillUpdateToDB( pSkillBase, pPlayer->GetID() );
}


void CSkillTreeManager::UpdateAck( CPlayer * pPlayer, SKILL_BASE* pSkillBase )
{
	MSG_SKILL_UPDATE msg;
	msg.Category = MP_SKILLTREE;
	msg.Protocol = MP_SKILLTREE_UPDATE_ACK;
	msg.SkillBase = *pSkillBase;

	pPlayer->SendMsg( &msg, sizeof( msg ) );
}

void CSkillTreeManager::UpdateNack( CPlayer * pPlayer, BYTE result )
{
	MSG_BYTE msg;

	msg.Category = MP_SKILLTREE;
	msg.Protocol = MP_SKILLTREE_UPDATE_NACK;
	msg.bData = result;

	pPlayer->SendMsg( &msg, sizeof( msg ) );
}

void CSkillTreeManager::AddSkill( CPlayer * pPlayer, SKILL_BASE* pSkillBase )
{
	SkillInsertToDB( pPlayer->GetID(), pSkillBase->wSkillIdx, pSkillBase->Level );
}

void CSkillTreeManager::AddSkillDBResult( CPlayer* pPlayer, SKILL_BASE* learnedSkill)
{
	UpdateAck(
		pPlayer,
		learnedSkill);

	pPlayer->GetSkillTree().Update(
		*learnedSkill);
}

CSkillTreeManager* CSkillTreeManager::GetInstance()
{
	static CSkillTreeManager instance;

	return &instance;
}
