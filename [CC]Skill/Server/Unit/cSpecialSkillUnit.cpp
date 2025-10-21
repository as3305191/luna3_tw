#include "stdafx.h"
#include ".\cspecialskillunit.h"
#include "../Target/Target.h"
#include "Object.h"
#include "Monster.h"
#include "Player.h"
#include "../Object/ActiveSkillObject.h"
#include "StateMachinen.h"
#include "../hseos/Date/SHDateManager.h"
#include "MapNetworkMsgParser.h"
#include "../Manager/SkillManager.h"

cSpecialSkillUnit::cSpecialSkillUnit(cActiveSkillObject& skillObject) :
cSkillUnit(skillObject)
{}

cSpecialSkillUnit::~cSpecialSkillUnit(void)
{
}

BOOL cSpecialSkillUnit::Excute()
{
	switch( mUnitType )
	{
	case UNITKIND_RESURRECTION:
		{
			if (g_csDateManager.IsChallengeZoneHere())
			{
				return FALSE;
			}
			
			mpParentSkill->GetTarget().SetPositionHead();

			while(CObject* const pTarget = mpParentSkill->GetTarget().GetData())
			{
				if( pTarget->GetObjectKind() != eObjectKind_Player )
				{
					continue;
				}

				CPlayer* pPlayer = ( ( CPlayer* )pTarget );

				// 080317 LUJ, ���/PK ���� ��뿡�Դ� ����� �� ����
				if( pPlayer->IsPKMode() ||
					pPlayer->IsShowdown() )
				{
					continue;
				}
				// 080318 LUJ, ��밡 PK�� ���� ����ġ ��ս� ������ ��쿡�� ����� �� ����
				else if( pPlayer->IsNoExpPenaltyByPK() )
				{
					continue;
				}
				// 081208 LUJ, ��������� ����� �������� ����� �� ����
				else if( pPlayer->IsDieFromGFW() )
				{
					continue;
				}

				// 100211 ONS ��Ȱ�迭 ��ų ���� ����ڿ��� ��Ȱ�ǻ縦 ���� ó�� �߰�
				pPlayer->ReviveBySkill( mpParentSkill );
			}
		}
		break;
	case UNITKIND_FAKE_DAMAGE:
	case UNITKIND_PHYSIC_FAKE_DAMAGE:
	case UNITKIND_MAGIC_FAKE_DAMAGE:
		{
			CObject* const operatorObject = mpParentSkill->GetOperator();

			if(0 == operatorObject)
			{
				return FALSE;
			}

			const float attraction = float(mpParentSkill->GetInfo().UnitDataType);

			mpParentSkill->GetTarget().SetPositionHead();

			while(CMonster* const targetMonster = (CMonster*)mpParentSkill->GetTarget().GetData())
			{
				if(FALSE == (eObjectKind_Monster & targetMonster->GetObjectKind()))
				{
					continue;
				}

				targetMonster->GetAbnormalStatus()->Attract = attraction;
				targetMonster->AddAggro(
					operatorObject->GetID(),
					int(mAddDamage),
					mpParentSkill->GetSkillIdx());
				targetMonster->ThinkAggro();
			}
		}
		break;
	case UNITKIND_DECOY:
		{
			CObject* const operatorObject = mpParentSkill->GetOperator();

			if(0 == operatorObject)
			{
				return FALSE;
			}

			mpParentSkill->GetTarget().SetPositionHead();

			while(CObject* const pTarget = mpParentSkill->GetTarget().GetData())
			{
				if(FALSE == (eObjectKind_Monster & pTarget->GetObjectKind()))
				{
					continue;
				}
				else if( ((CMonster*)pTarget)->SetTObject(mpParentSkill->GetOperator() ) )
				{
					GSTATEMACHINE.SetState( pTarget, eMA_PERSUIT );
				}
			}
		}
		break;
	case UNITKIND_RETURN:
		{
			mpParentSkill->GetTarget().SetPositionHead();

			while(CObject* const pTarget = mpParentSkill->GetTarget().GetData())
			{
				if( pTarget->GetObjectKind() != eObjectKind_Player )
					continue;

				CPlayer* pPlayer = ( CPlayer* )pTarget;

				if( pPlayer->IsPKMode() )
				{
					continue;
				}
								
				MSG_WORD message;
				message.dwObjectID	= pPlayer->GetID();
				message.Category	= MP_USERCONN;
				message.Protocol	= MP_USERCONN_RETURN_SYN;
				message.wData		= pPlayer->GetPlayerLoginPoint();

				UserConn_Return_Syn( pPlayer->GetAgentNum(), ( char* )&message );
			}
		}
		break;
	case UNITKIND_SPELLBREAK:
		{
			mpParentSkill->GetTarget().SetPositionHead();

			while(CObject* const pTarget = mpParentSkill->GetTarget().GetData())
			{
				if( pTarget->CurCastingSkillID )
				{
					MSG_DWORD msg;
					msg.dwObjectID = pTarget->GetID();
					msg.dwData = pTarget->CurCastingSkillID;
					SKILLMGR->OnSkillCancel( &msg );
				}
			}
		}
		break;

	}

	return TRUE;
}