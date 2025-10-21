#include "stdafx.h"
#include "SkillManager.h"
#include "../Info/ActiveSkillInfo.h"
#include "../Info/BuffSkillInfo.h"
#include "../Object/BuffSkillObject.h"
#include "MoveManager.h"
#include "ActionTarget.h"
#include "GuildDialog.h"
#include "PartyDialog.h"
#include "PartyMemberDlg.h"
#include "Gamein.h"
#include "cWindowManager.h"
#include "WindowIDEnum.h"
#include "cMsgBox.h"
#include "ObjectStateManager.h"
#include "ObjectManager.h"
#include "ChatManager.h"
#include "GameResourceManager.h"
#include "PKManager.h"
#include "ObjectActionManager.h"
#include "BattleSystem_Client.h"
#include "../../[CC]BattleSystem/GTournament/Battle_GTournament.h"
#include "InventoryExDialog.h"
#include "Item.h"
#include "GuildManager.h"
#include "ItemManager.h"
#include "CharacterDialog.h"
#include "FamilyDialog.h"
#include "PetManager.h"
#include "PartyManager.h"
#include "SiegeWarfareMgr.h"
#include "MHFile.h"
#include "ReviveDialog.h"


#include "MonsterGuageDlg.h"


#include "..\hseos\Family\SHFamily.h"
// 100312 ShinJS --- ToolTipMsg ����� Type
enum eToolTipMsgKeyWordType{
	eToolTipMsgKeyWordType_None = 0,

	// struct SKILL_INFO �� ����
	eToolTipMsgKeyWordType_Skill_Index,
	eToolTipMsgKeyWordType_Skill_Name,
	eToolTipMsgKeyWordType_Skill_Tooltip,
	eToolTipMsgKeyWordType_Skill_Image,
	eToolTipMsgKeyWordType_Skill_Kind,
	eToolTipMsgKeyWordType_Skill_Level,

	// struct ACTIVE_SKILL_INFO �� ����
	eToolTipMsgKeyWordType_ActiveSkill,
	eToolTipMsgKeyWordType_ActiveSkill_Money,
	eToolTipMsgKeyWordType_ActiveSkill_Life_Percent,
	eToolTipMsgKeyWordType_ActiveSkill_Life_Plus,
	eToolTipMsgKeyWordType_ActiveSkill_Mana_Percent,
	eToolTipMsgKeyWordType_ActiveSkill_Mana_Plus,
	eToolTipMsgKeyWordType_ActiveSkill_EquipType,
	eToolTipMsgKeyWordType_ActiveSkill_OperatorEffect,
	eToolTipMsgKeyWordType_ActiveSkill_TargetEffect,
	eToolTipMsgKeyWordType_ActiveSkill_SkillEffect,
	eToolTipMsgKeyWordType_ActiveSkill_AnimationTime,
	eToolTipMsgKeyWordType_ActiveSkill_CastingTime,
	eToolTipMsgKeyWordType_ActiveSkill_DelayType,
	eToolTipMsgKeyWordType_ActiveSkill_DelayTime,
	eToolTipMsgKeyWordType_ActiveSkill_CoolTime,
	eToolTipMsgKeyWordType_ActiveSkill_Target,
	eToolTipMsgKeyWordType_ActiveSkill_Range,
	eToolTipMsgKeyWordType_ActiveSkill_Area,
	eToolTipMsgKeyWordType_ActiveSkill_AreaTarget,
	eToolTipMsgKeyWordType_ActiveSkill_AreaData,
	eToolTipMsgKeyWordType_ActiveSkill_Unit,
	eToolTipMsgKeyWordType_ActiveSkill_UnitDataType,
	eToolTipMsgKeyWordType_ActiveSkill_UnitData,
	eToolTipMsgKeyWordType_ActiveSkill_Success,
	eToolTipMsgKeyWordType_ActiveSkill_Cancel,
	eToolTipMsgKeyWordType_ActiveSkill_TrainPoint,
	eToolTipMsgKeyWordType_ActiveSkill_TrainMoney,
	eToolTipMsgKeyWordType_ActiveSkill_Buff1,
	eToolTipMsgKeyWordType_ActiveSkill_Buff2,
	eToolTipMsgKeyWordType_ActiveSkill_Buff3,
	eToolTipMsgKeyWordType_ActiveSkill_Buff4,
	eToolTipMsgKeyWordType_ActiveSkill_Buff5,
	eToolTipMsgKeyWordType_ActiveSkill_BuffRate1,
	eToolTipMsgKeyWordType_ActiveSkill_BuffRate2,
	eToolTipMsgKeyWordType_ActiveSkill_BuffRate3,
	eToolTipMsgKeyWordType_ActiveSkill_BuffRate4,
	eToolTipMsgKeyWordType_ActiveSkill_BuffRate5,
	eToolTipMsgKeyWordType_ActiveSkill_RequiredBuff,
	eToolTipMsgKeyWordType_ActiveSkill_IsMove,
	eToolTipMsgKeyWordType_ActiveSkill_RequiredPlayerLevel,
	eToolTipMsgKeyWordType_ActiveSkill_ConsumeItemIndex,
	eToolTipMsgKeyWordType_ActiveSkill_ConsumeItemCount,
	eToolTipMsgKeyWordType_ActiveSkill_ConsumePointType,
	eToolTipMsgKeyWordType_ActiveSkill_ConsumePointValue,
	eToolTipMsgKeyWordType_ActiveSkill_IsBattle,
	eToolTipMsgKeyWordType_ActiveSkill_IsPeace,
	eToolTipMsgKeyWordType_ActiveSkill_mIsMove,
	eToolTipMsgKeyWordType_ActiveSkill_IsStop,

	// BUFF_SKILL_INFO �� ����
	eToolTipMsgKeyWordType_BuffSkill,
	eToolTipMsgKeyWordType_BuffSkill_Money,
	eToolTipMsgKeyWordType_BuffSkill_WeaponType,
	eToolTipMsgKeyWordType_BuffSkill_ArmorType,
	eToolTipMsgKeyWordType_BuffSkill_DelayTime,
	eToolTipMsgKeyWordType_BuffSkill_SkillEffect,
	eToolTipMsgKeyWordType_BuffSkill_Status,
	eToolTipMsgKeyWordType_BuffSkill_StatusDataType,
	eToolTipMsgKeyWordType_BuffSkill_StatusData,
	eToolTipMsgKeyWordType_BuffSkill_ConditonType,
	eToolTipMsgKeyWordType_BuffSkill_ConditionOperation,
	eToolTipMsgKeyWordType_BuffSkill_ConditionValue,
	eToolTipMsgKeyWordType_BuffSkill_Count,
	eToolTipMsgKeyWordType_BuffSkill_RuleBattle,
	eToolTipMsgKeyWordType_BuffSkill_RulePeace,
	eToolTipMsgKeyWordType_BuffSkill_RuleMove,
	eToolTipMsgKeyWordType_BuffSkill_RuleStop,
	eToolTipMsgKeyWordType_BuffSkill_RuleRest,
	eToolTipMsgKeyWordType_BuffSkill_Die,
	eToolTipMsgKeyWordType_BuffSkill_CountType,
	eToolTipMsgKeyWordType_BuffSkill_IsEndTime,
	eToolTipMsgKeyWordType_BuffSkill_EventSkillIndex,
	eToolTipMsgKeyWordType_BuffSkill_Item1,
	eToolTipMsgKeyWordType_BuffSkill_Item2,
	eToolTipMsgKeyWordType_BuffSkill_ItemCount1,
	eToolTipMsgKeyWordType_BuffSkill_ItemCount2,

	// ITEM_INFO �� ����
	eToolTipMsgKeyWordType_Item,
	eToolTipMsgKeyWordType_Item_Name,
};

// 100312 ShinJS --- ToolTipMsg ����� ����
struct stToolTipMsgKeyWordData{
	char* KeyWord;
	eToolTipMsgKeyWordType eType;
	DWORD dwHashCode;
};

// 100312 ShinJS --- ToolTipMsg ����� Table
stToolTipMsgKeyWordData ToolTipMsgKeyWordTable[] = {

	// struct SKILL_INFO �� ����
	{ "$Index$", eToolTipMsgKeyWordType_Skill_Index, 0 },
	{ "$Name$", eToolTipMsgKeyWordType_Skill_Name, 0 },
	{ "$Tooltip$", eToolTipMsgKeyWordType_Skill_Tooltip, 0 },
	{ "$Image$", eToolTipMsgKeyWordType_Skill_Image, 0 },
	{ "$Kind$", eToolTipMsgKeyWordType_Skill_Kind, 0 },
	{ "$Level$", eToolTipMsgKeyWordType_Skill_Level, 0 },

	// struct ACTIVE_SKILL_INFO �� ����
	{ "$Money$", eToolTipMsgKeyWordType_ActiveSkill_Money, 0 },
	{ "$LifePercent$", eToolTipMsgKeyWordType_ActiveSkill_Life_Percent, 0 },
	{ "$LifePlus$", eToolTipMsgKeyWordType_ActiveSkill_Life_Plus, 0 },
	{ "$ManaPercent$", eToolTipMsgKeyWordType_ActiveSkill_Mana_Percent, 0 },
	{ "$ManaPlus$", eToolTipMsgKeyWordType_ActiveSkill_Mana_Plus, 0 },
	{ "$EquipType$", eToolTipMsgKeyWordType_ActiveSkill_EquipType, 0 },
	{ "$OperatorEffect$", eToolTipMsgKeyWordType_ActiveSkill_OperatorEffect, 0 },
	{ "$TargetEffect$", eToolTipMsgKeyWordType_ActiveSkill_TargetEffect, 0 },
	{ "$SkillEffect$", eToolTipMsgKeyWordType_ActiveSkill_SkillEffect, 0 },
	{ "$AnimationTime$", eToolTipMsgKeyWordType_ActiveSkill_AnimationTime, 0 },
	{ "$CastingTime$", eToolTipMsgKeyWordType_ActiveSkill_CastingTime, 0 },
	{ "$DelayType$", eToolTipMsgKeyWordType_ActiveSkill_DelayType, 0 },
	{ "$DelayTime$", eToolTipMsgKeyWordType_ActiveSkill_DelayTime, 0 },
	{ "$CoolTime$", eToolTipMsgKeyWordType_ActiveSkill_CoolTime, 0 },
	{ "$Target$", eToolTipMsgKeyWordType_ActiveSkill_Target, 0 },
	{ "$Range$", eToolTipMsgKeyWordType_ActiveSkill_Range, 0 },
	{ "$Area$", eToolTipMsgKeyWordType_ActiveSkill_Area, 0 },
	{ "$AreaTarget$", eToolTipMsgKeyWordType_ActiveSkill_AreaTarget, 0 },
	{ "$AreaData$", eToolTipMsgKeyWordType_ActiveSkill_AreaData, 0 },
	{ "$Unit$", eToolTipMsgKeyWordType_ActiveSkill_Unit, 0 },
	{ "$UnitDataType$", eToolTipMsgKeyWordType_ActiveSkill_UnitDataType, 0 },
	{ "$UnitData$", eToolTipMsgKeyWordType_ActiveSkill_UnitData, 0 },
	{ "$Success$", eToolTipMsgKeyWordType_ActiveSkill_Success, 0 },
	{ "$Cancel$", eToolTipMsgKeyWordType_ActiveSkill_Cancel, 0 },
	{ "$TrainPoint$", eToolTipMsgKeyWordType_ActiveSkill_TrainPoint, 0 },
	{ "$TrainMoney$", eToolTipMsgKeyWordType_ActiveSkill_TrainMoney, 0 },
	{ "$Buff1$", eToolTipMsgKeyWordType_ActiveSkill_Buff1, 0 },
	{ "$Buff2$", eToolTipMsgKeyWordType_ActiveSkill_Buff2, 0 },
	{ "$Buff3$", eToolTipMsgKeyWordType_ActiveSkill_Buff3, 0 },
	{ "$Buff4$", eToolTipMsgKeyWordType_ActiveSkill_Buff4, 0 },
	{ "$Buff5$", eToolTipMsgKeyWordType_ActiveSkill_Buff5, 0 },
	{ "$BuffRate1$", eToolTipMsgKeyWordType_ActiveSkill_BuffRate1, 0 },
	{ "$BuffRate2$", eToolTipMsgKeyWordType_ActiveSkill_BuffRate2, 0 },
	{ "$BuffRate3$", eToolTipMsgKeyWordType_ActiveSkill_BuffRate3, 0 },
	{ "$BuffRate4$", eToolTipMsgKeyWordType_ActiveSkill_BuffRate4, 0 },
	{ "$BuffRate5$", eToolTipMsgKeyWordType_ActiveSkill_BuffRate5, 0 },
	{ "$RequiredBuff$", eToolTipMsgKeyWordType_ActiveSkill_RequiredBuff, 0 },
	{ "$IsMove$", eToolTipMsgKeyWordType_ActiveSkill_IsMove, 0 },
	{ "$RequiredPlayerLevel$", eToolTipMsgKeyWordType_ActiveSkill_RequiredPlayerLevel, 0 },
	{ "$ConsumeItemIndex$", eToolTipMsgKeyWordType_ActiveSkill_ConsumeItemIndex, 0 },
	{ "$ConsumeItemCount$", eToolTipMsgKeyWordType_ActiveSkill_ConsumeItemCount, 0 },
	{ "$ConsumePointType$", eToolTipMsgKeyWordType_ActiveSkill_ConsumePointType, 0 },
	{ "$ConsumePointValue$", eToolTipMsgKeyWordType_ActiveSkill_ConsumePointValue, 0 },
	{ "$IsBattle$", eToolTipMsgKeyWordType_ActiveSkill_IsBattle, 0 },
	{ "$IsPeace$", eToolTipMsgKeyWordType_ActiveSkill_IsPeace, 0 },
	{ "$mIsMove$", eToolTipMsgKeyWordType_ActiveSkill_mIsMove, 0 },
	{ "$IsStop$", eToolTipMsgKeyWordType_ActiveSkill_IsStop, 0 },

	// BUFF_SKILL_INFO �� ����
	{ "$BuffMoney$", eToolTipMsgKeyWordType_BuffSkill_Money, 0 },
	{ "$WeaponType$", eToolTipMsgKeyWordType_BuffSkill_WeaponType, 0 },
	{ "$ArmorType$", eToolTipMsgKeyWordType_BuffSkill_ArmorType, 0 },
	{ "$BuffDelayTime$", eToolTipMsgKeyWordType_BuffSkill_DelayTime, 0 },
	{ "$BuffSkillEffect$", eToolTipMsgKeyWordType_BuffSkill_SkillEffect, 0 },
	{ "$Status$", eToolTipMsgKeyWordType_BuffSkill_Status, 0 },
	{ "$StatusDataType$", eToolTipMsgKeyWordType_BuffSkill_StatusDataType, 0 },
	{ "$StatusData$", eToolTipMsgKeyWordType_BuffSkill_StatusData, 0 },
	{ "$ConditonType$", eToolTipMsgKeyWordType_BuffSkill_ConditonType, 0 },
	{ "$ConditionOperation$", eToolTipMsgKeyWordType_BuffSkill_ConditionOperation, 0 },
	{ "$ConditionValue$", eToolTipMsgKeyWordType_BuffSkill_ConditionValue, 0 },
	{ "$Count$", eToolTipMsgKeyWordType_BuffSkill_Count, 0 },
	{ "$RuleBattle$", eToolTipMsgKeyWordType_BuffSkill_RuleBattle, 0 },
	{ "$RulePeace$", eToolTipMsgKeyWordType_BuffSkill_RulePeace, 0 },
	{ "$RuleMove$", eToolTipMsgKeyWordType_BuffSkill_RuleMove, 0 },
	{ "$RuleStop$", eToolTipMsgKeyWordType_BuffSkill_RuleStop, 0 },
	{ "$RuleRest$", eToolTipMsgKeyWordType_BuffSkill_RuleRest, 0 },
	{ "$Die$", eToolTipMsgKeyWordType_BuffSkill_Die, 0 },
	{ "$CountType$", eToolTipMsgKeyWordType_BuffSkill_CountType, 0 },
	{ "$IsEndTime$", eToolTipMsgKeyWordType_BuffSkill_IsEndTime, 0 },
	{ "$EventSkillIndex$", eToolTipMsgKeyWordType_BuffSkill_EventSkillIndex, 0 },
	{ "$Item1$", eToolTipMsgKeyWordType_BuffSkill_Item1, 0 },
	{ "$Item2$", eToolTipMsgKeyWordType_BuffSkill_Item2, 0 },
	{ "$ItemCount1$", eToolTipMsgKeyWordType_BuffSkill_ItemCount1, 0 },
	{ "$ItemCount2$", eToolTipMsgKeyWordType_BuffSkill_ItemCount2, 0 },
	
	// ITEM_INFO �� ����
	{ "$ItemName$", eToolTipMsgKeyWordType_Item_Name, 0 },
};

cSkillManager::cSkillManager(void)
{
	mpSkillInfo = NULL;

	// 081021 LYW --- SkillManager : �� ��ų ���� ������ �ʱ�ȭ.
	mpPetSkillInfo = NULL ;

	Init();
}

cSkillManager::~cSkillManager(void)
{
	Release();
}

void cSkillManager::Init()
{
	m_SkillInfoTable.Initialize(64);
	m_SkillObjectTable.Initialize(64);
	m_SkillTreeTable.Initialize(64);
	LoadSkillInfoList();

	// 100312 ShinJS --- �������ڿ����̺� ������ �ʱ�ȭ�Ѵ�.
	InitToolTipMsgKeyWordTable();
}
void cSkillManager::Release()
{
	{
		m_SkillInfoTable.SetPositionHead();

		for(cSkillInfo* pSInfo = m_SkillInfoTable.GetData();
			0 < pSInfo;
			pSInfo = m_SkillInfoTable.GetData())
		{
			SAFE_DELETE(
				pSInfo);
		}

		m_SkillInfoTable.RemoveAll();
	}

	{
		m_SkillObjectTable.SetPositionHead();

		for(cSkillObject* pSObj = m_SkillObjectTable.GetData();
			0 < pSObj;
			pSObj = m_SkillObjectTable.GetData())
		{
			SAFE_DELETE(
				pSObj);
		}

		m_SkillObjectTable.RemoveAll();
	}

	{
		m_SkillTreeTable.SetPositionHead();

		for(SKILL_TREE_INFO* pInfo = m_SkillTreeTable.GetData();
			0 < pInfo;
			pInfo = m_SkillTreeTable.GetData())
		{
			SAFE_DELETE(
				pInfo);
		}

		m_SkillTreeTable.RemoveAll();
	}
}

void cSkillManager::ReleaseAllSkillObject()
{
	m_SkillObjectTable.SetPositionHead();

	for(cSkillObject* pSObj = m_SkillObjectTable.GetData();
		0 < pSObj;
		pSObj = m_SkillObjectTable.GetData())
	{
		SAFE_DELETE(
			pSObj);
	}

	m_SkillObjectTable.RemoveAll();
}

void cSkillManager::LoadSkillInfoList()
{
	CMHFile file;
	file.Init("System/Resource/SkillList.bin","rb");

	while(FALSE == file.IsEOF())
	{
		TCHAR text[MAX_PATH] = {0};
		file.GetLine(
			text,
			sizeof(text) / sizeof(*text));

		cActiveSkillInfo* const pInfo = new cActiveSkillInfo;
		pInfo->Parse(text);

		if( pInfo->GetIndex() == 0 )
		{
			delete pInfo;
			continue;
		}

		m_SkillInfoTable.Add(pInfo,pInfo->GetIndex());

		// 091211 LUJ, ��ų ������ ����
		const DWORD index = pInfo->GetIndex() / 100 * 100 + 1;
		++mSkillSize[index];
	}

	file.Release();
	file.Init("System/Resource/Skill_Buff_List.bin","rb");

	while(FALSE == file.IsEOF())
	{
		TCHAR text[MAX_PATH] = {0};
		file.GetLine(
			text,
			sizeof(text) / sizeof(*text));

		cBuffSkillInfo* const pInfo = new cBuffSkillInfo;
		pInfo->Parse(text);

		if( pInfo->GetIndex() == 0 )
		{
			delete pInfo;
			continue;
		}

		m_SkillInfoTable.Add(pInfo,pInfo->GetIndex());

		// 091211 LUJ, ��ų ������ ����
		const DWORD index = pInfo->GetIndex() / 100 * 100 + 1;
		++mSkillSize[index];
	}

	file.Release();
	file.Init("System/Resource/skill_get_list.bin","rb");

	while(FALSE == file.IsEOF())
	{
		SKILL_TREE_INFO* pInfo = new SKILL_TREE_INFO;

		pInfo->ClassIndex = file.GetWord();
		pInfo->Count = file.GetWord();

		for( WORD cnt = 0; cnt < pInfo->Count; cnt++ )
		{
			pInfo->SkillIndex[ cnt ] = file.GetDword();
		}

		m_SkillTreeTable.Add( pInfo, pInfo->ClassIndex );
	}

	file.Release();

}

cSkillInfo*	cSkillManager::GetSkillInfo(DWORD SkillInfoIdx)
{
	return (cSkillInfo*)m_SkillInfoTable.GetData(SkillInfoIdx);
}

SKILL_TREE_INFO* cSkillManager::GetSkillTreeInfo( WORD ClassIndex )
{
	return (SKILL_TREE_INFO*)m_SkillTreeTable.GetData( ClassIndex );
}

DWORD cSkillManager::GetSkillTooltipInfo(DWORD SkillInfoIdx)
{
	cSkillInfo* pInfo = m_SkillInfoTable.GetData( SkillInfoIdx );

	if(!pInfo)
		return 0;

	if( pInfo->GetKind() <= SKILLKIND_ONOFF )
	{
		return pInfo->GetTooltip();
	}
	else
	{
		return 0;
	}
}

cSkillObject* cSkillManager::GetSkillObject(DWORD SkillObjectID)
{
	return (cSkillObject*)m_SkillObjectTable.GetData(SkillObjectID);
}

DWORD cSkillManager::GetComboSkillIdx(CHero* pHero)
{
	DWORD SkillNum = 1000000;
	int WeaponType	= HERO->GetWeaponEquipType();
	int WeaponAniType = pHero->GetWeaponAniType();
	int CurComboNum = pHero->GetCurComboNum();
	int Skill = 0;

	if( CurComboNum == MAX_COMBO_NUM )
	{
		CurComboNum = 0;
		pHero->SetCurComboNum(0);
	}

	switch( WeaponAniType )
	{
	case eWeaponAnimationType_None:			Skill = 1;	break;
	case eWeaponAnimationType_OneHand:	
		{
			// 091117 pdy �Ѽտϵ��� ��쿡 ������ Ÿ�� ��ų�ε����� �־��ش�.
			if( WeaponType == eWeaponType_Staff )
			{
				// ���� �Ѽ� �ϵ��ϰ�쿡 Ÿ����
				// WeaponAniType(eWeaponAnimationType_OneHand)
				// WeaponType(eWeaponType_Staff)
				Skill = 6;
			}
			else
			{
				//�������� �Ѽ� �б� ��ų
				Skill = 3;	
			}
		}
		break;
	case eWeaponAnimationType_TwoHand:		Skill = 2;	break;
	case eWeaponAnimationType_Dagger:		Skill = 4;	break;
	case eWeaponAnimationType_Staff:		Skill = 6;	break;
	case eWeaponAnimationType_Bow:			Skill = 5;	break;
		// 080710 LUJ, �ѱ� �⺻ ��ų ���
	case eWeaponAnimationType_Gun:			Skill = 7;	break;
		// 080703 LUJ, �̵����� �ܰ�/�Ѽչ��Ⱑ ���� �ִϸ��̼��� �ִ�.
	case eWeaponAnimationType_TwoBlade:
		{
			Skill = ( eWeaponType_Dagger == pHero->GetWeaponEquipType() ? 8 : 10 );
			break;
		}
	// 090521 ONS �ű����� ��ų����Ʈ �Ӽ� �߰�
	case eWeaponAnimationType_BothDagger:	Skill = 20;	break;

	}
	
	SkillNum = SkillNum + ( ( Skill ) * 100 ) + CurComboNum + 1;

	return SkillNum;
}


// ��ų�� ���
BOOL cSkillManager::ExcuteSkillSYN(CHero* pHero,CActionTarget* pTarget,BOOL bSkill)
{
	DWORD SkillNum;
	cSkillInfo* pSkillInfo;

	/// ��ų�� �⺻������ ���
	if(bSkill == FALSE)
	{
		/// �⺻���� ������ �����´�
		SkillNum = GetComboSkillIdx(pHero);
		pSkillInfo = GetSkillInfo(SkillNum);
		mpSkillInfo = pSkillInfo;
	}
	else
		pSkillInfo = mpSkillInfo;
		
	/// ��ų ������ ���ٸ� ����
	if(pSkillInfo == NULL)
		return FALSE;

	/// ��ų�� �����Ѵ�
	return ExcuteSkillSYN(pHero,pTarget,pSkillInfo);
}

BOOL cSkillManager::ExcuteSkillSYN(CHero* pHero,CActionTarget* pTarget,cSkillInfo* pInfo)
{
	CActionTarget target;
	target.CopyFrom(pTarget);
	
	/// ��ų ������ ������ ����
	if(pInfo == NULL)
		return FALSE;

//	if( pInfo->GetKind() == SKILLKIND_BUFF || pInfo->GetKind() == SKILLKIND_PASSIVE )
//		return FALSE;

	cActiveSkillInfo* pSkillInfo = ( cActiveSkillInfo* )pInfo;
	
	/// ���� ���� ������� �Ǵ�
	if(pSkillInfo->IsValidTarget( pHero, pTarget ) == FALSE )
	{
		return FALSE;
	}

	/// ��ų ������ �����´�
	//int SkillLevel = pHero->GetSkillLevel(pSkillInfo->GetIndex());
	int SkillLevel = pSkillInfo->GetIndex() % 100;

	/// ���� �������� �Ǵ�
	if(pSkillInfo->IsExcutableSkillState(pHero,SkillLevel,&target) == FALSE)
	{
		/// �⺻ �������� ��ȯ�ϰ� ��ų ������ �Ѵ�
		//pHero->SetCurComboNum(0);
		//printf( "%d\t%s\n", 0, "cSkillManager::ExcuteSkillSYN1", 0 );
		return FALSE;
	}

	/// Ÿ�� ���� - �����ؾ� ��

	/// ��ȭ����� ���
	// 070124 LYW --- PeaceWar : Delete peacewar part.
	/*
	if(PEACEWARMGR->IsPeaceMode(pHero) == TRUE)
		/// �������� �ٲ۴�
		PEACEWARMGR->ToggleHeroPeace_WarMode();
		*/

	// 080917 KTH -- ���������� ��ų ������ �˻�
	if( SIEGEWARFAREMGR->IsSiegeWarfareZone() )
	{
		// �������� ������ ��ų �Ǵ� ��� ���� ������ �̶�� ��� ���и� �˸���.
		if( SIEGEWARFAREMGR->IsFobiddenSkill(pInfo->GetIndex()) )
		{
			// ���� �������� ����� �� ���� ��ų�Դϴ�. ��� �ý��� �޽����� ��� �Ͽ� �ش�.
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1685 ) );
			return FALSE;
		}
	}

	
	/// Ÿ���� ��ġ���� ������ ����
	VECTOR3* pTargetPos = target.GetTargetPosition();
	if(pTargetPos == NULL)
		return FALSE;
	
	if(pSkillInfo->IsInRange(*pHero, target))
	{
		HERO->ClearWayPoint();
		MOVEMGR->MoveStop( pHero, 0 );
		
		/// �ڽ��� Ÿ���� ��쿡�� ���� ������ �ٲ��� �ʴ´�.
		if( pSkillInfo->GetSkillInfo()->Target != 1 )
			MOVEMGR->SetLookatPos(pHero,pTargetPos,0,gCurTime);
				
		/// �̵� �ൿ�� �ʱ�ȭ
		pHero->SetMovingAction(NULL);

		/// ��ų ���
		return RealExcuteSkillSYN(pHero,&target,pSkillInfo);
	}

	pHero->SetCurComboNum(0);

	/// ��ų ����� ���� �̵�����
	CAction MoveAction;
	MoveAction.InitSkillAction(pSkillInfo,&target);
	MOVEMGR->SetHeroActionMove(&target,&MoveAction);
	return TRUE;
}

BOOL cSkillManager::RealExcuteSkillSYN(CHero* pHero,CActionTarget* pTarget,cSkillInfo* pInfo)
{
	/// ��ų ������ ������ ����
	if(pInfo == NULL)
		return FALSE;
	
//	if( pInfo->GetKind() == SKILLKIND_BUFF || pInfo->GetKind() == SKILLKIND_PASSIVE )
//		return FALSE;

	cActiveSkillInfo* pSkillInfo = ( cActiveSkillInfo* )pInfo;
	
	/// ��ų ������ �����´�
	//int SkillLevel = pHero->GetSkillLevel(pSkillInfo->GetIndex());
	int SkillLevel = pSkillInfo->GetIndex() % 100;

	/// ���� �������� �Ǵ�
	if(pSkillInfo->IsExcutableSkillState(pHero,SkillLevel,pTarget) == FALSE)
	{
		/// �⺻ �������� ��ȯ�ϰ� ��ų ������ �Ѵ�
		pHero->SetCurComboNum(0);
		return FALSE;
	}

	// 080602 LUJ, �Ҹ� ������ ������ ����
	ITEMBASE consumeItem;
	ZeroMemory( &consumeItem, sizeof( consumeItem ) );

	// 080602 LUJ, �Ҹ� ����Ʈ/�������� ������� �˻�
	{
		const ACTIVE_SKILL_INFO*	info	= pSkillInfo->GetSkillInfo();
		CHero*						hero	= OBJECTMGR->GetHero();

		if( ! info ||
			! hero )
		{
			return FALSE;
		}

		// 080602 LUJ, ����Ʈ �˻�
		{
			const ACTIVE_SKILL_INFO::ConsumePoint& point = info->mConsumePoint;

			switch( point.mType )
			{
			case ACTIVE_SKILL_INFO::ConsumePoint::TypeFamily:
				{
					CSHFamily* family = hero->GetFamily();

					if( ! family )
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 828 ) );
						return FALSE;
					}

					const CSHFamily::stINFO_EX* info = family->GetEx();

					if( !	info	||
							info->nHonorPoint < point.mValue )
					{
						CHATMGR->AddMsg(
							CTC_SYSMSG,
							CHATMGR->GetChatMsg( 1568 ),
							point.mValue - info->nHonorPoint );
						return FALSE;
					}
					
					break;
				}
			case ACTIVE_SKILL_INFO::ConsumePoint::TypeGuild:
				{
					if( ! hero->GetGuildIdx() )
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 939 ) );
						return FALSE;
					}
					else if( GUILDMGR->GetScore() < point.mValue )
					{
						CHATMGR->AddMsg(
							CTC_SYSMSG,
							CHATMGR->GetChatMsg( 1566 ),
							point.mValue - GUILDMGR->GetScore() );
						return FALSE;
					}

					break;
				}
			case ACTIVE_SKILL_INFO::ConsumePoint::TypeExperience:
				{
					if( hero->GetExpPoint() < EXPTYPE( point.mValue ) )
					{
						CHATMGR->AddMsg(
							CTC_SYSMSG,
							CHATMGR->GetChatMsg( 1567 ),
							EXPTYPE( point.mValue ) - hero->GetExpPoint() );
						return FALSE;
					}

					break;
				}
			case ACTIVE_SKILL_INFO::ConsumePoint::TypePlayerKill:
				{
					if( hero->GetBadFame() < FAMETYPE( point.mValue ) )
					{
						CHATMGR->AddMsg(
							CTC_SYSMSG,
							CHATMGR->GetChatMsg( 1565 ),
							FAMETYPE( point.mValue ) - hero->GetBadFame() );
						return FALSE;
					}

					break;
				}
			}
		}

		// 080602 LUJ, ������ �˻�
		if( info->Money > hero->GetMoney() )
		{
			CHATMGR->AddMsg(
				CTC_SYSMSG,
				CHATMGR->GetChatMsg( 1333 ),
				info->Money - hero->GetMoney() );

			return FALSE;
		}
		// 080602 LUJ, ������ �˻�
		else if( info->mConsumeItem.wIconIdx )
		{
			CInventoryExDialog* inventoryDialog	= GAMEIN->GetInventoryDialog();

			if( ! inventoryDialog )
			{
				return FALSE;
			}

			const POSTYPE inventoryStartPosition = TP_INVENTORY_START;
			const POSTYPE inventoryEndPosition = POSTYPE(TP_INVENTORY_END + TABCELL_INVENTORY_NUM * HERO->Get_HeroExtendedInvenCount());

			for(
				POSTYPE position = inventoryStartPosition;
				inventoryEndPosition > position;
				++position )
			{
				CItem* item = inventoryDialog->GetItemForPos( position );

				if( !	item													||
						item->GetItemIdx()		!= info->mConsumeItem.wIconIdx	||
					(	ITEMMGR->IsDupItem( item->GetItemIdx() )	&&
						item->GetDurability()	< info->mConsumeItem.Durability ) )
				{
					continue;
				}

				consumeItem = item->GetItemBaseInfo();
				break;
			}

			// 080602 LUJ, �Ҹ��� �������� ã�� ����
			if( ! consumeItem.wIconIdx )
			{
				const ITEM_INFO* itemInfo = ITEMMGR->GetItemInfo( info->mConsumeItem.wIconIdx );

				if( ! itemInfo )
				{
					return FALSE;
				}

				CHATMGR->AddMsg(
					CTC_SYSMSG,
					CHATMGR->GetChatMsg( 1543 ),
					itemInfo->ItemName, info->mConsumeItem.Durability );
				
				return FALSE;
			}
		}
	}

	/// ȿ���ߵ� �����̰� ���� ��ų�� Ŭ���̾�Ʈ���� �ٷ� Ÿ�ٸ���Ʈ �ۼ�
	//if( pSkillInfo->GetSkillInfo()->DelayType == 0 )
	{
		VECTOR3* pTargetPos = pTarget->GetTargetPosition();
		if(pTargetPos == NULL)
			return FALSE;

		switch( pSkillInfo->GetSkillInfo()->Area )
		{
		case 1:
			{
				pTarget->SetTargetObjectsInRange(
					pTargetPos,
					pSkillInfo->GetSkillInfo()->AreaData,
					WORD(pSkillInfo->GetSkillInfo()->AreaTarget));
			}
			break;
		}
	}

	float AttackSpeedRate = 1.0f;

	DWORD idx = pInfo->GetIndex();

	if( (idx / 100000) % 10 )
	{
		if( IsUsedPhysicAttackSpeed( *pSkillInfo->GetSkillInfo() ) )
		{
			AttackSpeedRate += ( ( HERO->GetRatePassiveStatus()->PhysicSkillSpeedRate + HERO->GetRateBuffStatus()->PhysicSkillSpeedRate ) * 0.01f );
		}
		else
		{
			float SpeedRate = min( HERO->GetRateBuffStatus()->MagicSkillSpeedRate + HERO->GetRatePassiveStatus()->MagicSkillSpeedRate, 99.f );
			AttackSpeedRate = ( 1 / ( 1 - ( SpeedRate*0.01f ) ) );
			if( (pSkillInfo->GetSkillInfo()->CastingTime / AttackSpeedRate) < MIN_MAGIC_CASTING_TIME )
				AttackSpeedRate = pSkillInfo->GetSkillInfo()->CastingTime / MIN_MAGIC_CASTING_TIME;
		}
	}
	else
	{
		AttackSpeedRate += ( ( HERO->GetRatePassiveStatus()->NormalSpeedRate + HERO->GetRateBuffStatus()->NormalSpeedRate ) * 0.01f );
	}
	
	if( AttackSpeedRate <= 0 )
	{
		AttackSpeedRate = 1.0f;
	}

	const DWORD time = DWORD( pSkillInfo->GetSkillInfo()->AnimationTime / AttackSpeedRate );
	const DWORD CastingTime = DWORD( pSkillInfo->GetSkillInfo()->CastingTime / AttackSpeedRate );
	HERO->SetSkillDelay(
		time);
	HERO->StartSkillProgressBar(
		CastingTime);
	HERO->SetNextAction(
		0);

	pSkillInfo->SendMsgToServer(
		pHero,
		pTarget,
		consumeItem);
	pSkillInfo->SetCooling();

	// 080602 LUJ, �Ҹ�� �������� ��� ó���Ѵ�
	{
		CItem* item = ITEMMGR->GetItem( consumeItem.dwDBIdx );

		if( item )
		{
			item->SetLock( TRUE );
		}
	}

	DoCreateTempSkillObject(
		*pSkillInfo,
		*pTarget,
		consumeItem);

	if(0 == ((pInfo->GetIndex() / 100000) % 10))
	{
		pHero->SetCurComboNum( pHero->GetCurComboNum() + 1 );
	}
	else
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(777), pSkillInfo->GetName() );
		pHero->SetCurComboNum(0);
	}

	mpSkillInfo = NULL;

#ifdef _TESTCLIENT_
	static IDDDD = 0;
	
	MSG_SKILLOBJECT_ADD2 msg;
	
	msg.Category = MP_SKILL;
	msg.Protocol = MP_SKILL_SKILLOBJECT_ADD;
	msg.bCreate = TRUE;

	msg.SkillObjectInfo.SkillObjectIdx = SKILLOBJECT_ID_START+IDDDD++;
	msg.SkillObjectInfo.SkillIdx = pSkillInfo->GetIndex();

	if( pSkillInfo->GetSkillInfo()->Target )
	{
		msg.SkillObjectInfo.Pos = *pTarget->GetTargetPosition();
	}
	else
	{
		HERO->GetPosition( &msg.SkillObjectInfo.Pos );
	}

	msg.SkillObjectInfo.StartTime = 0;
	ROTATEINFO* pRotateInfo = HERO->GetBaseRotateInfo();
//	if( !pRotateInfo )	return;
	const float fOperatorAngle = (pRotateInfo->bRotating == TRUE ? pRotateInfo->Angle.GetTargetAngleRad() : HERO->GetAngle());
	msg.SkillObjectInfo.Direction = GetRadToVector( fOperatorAngle );
	msg.SkillObjectInfo.SkillLevel = ( BYTE )SkillLevel;
	msg.SkillObjectInfo.Operator = gHeroID;

	pTarget->GetMainTarget( &msg.SkillObjectInfo.MainTarget );

	CBattle* pBattle = BATTLESYSTEM->GetBattle();

	msg.SkillObjectInfo.BattleID = pBattle->GetBattleID();
	//msg.SkillObjectInfo.BattleKind = pBattle->GetBattleKind();

	NetworkMsgParse( MP_SKILL_SKILLOBJECT_ADD, &msg );

	MSG_SKILL_RESULT msg2;

	msg2.InitMsg( msg.SkillObjectInfo.SkillObjectIdx );
	msg2.SkillDamageKind = false;
	CTargetListIterator iter(&msg2.TargetList);
	
	CObject* pObject = OBJECTMGR->GetObject( msg.SkillObjectInfo.MainTarget.dwMainTargetID );
	
	if( pObject )
	{
		RESULTINFO dinfo;
//		dinfo.Clear();
		dinfo.bCritical = 1;//rand()%5 ? FALSE : TRUE;
		dinfo.RealDamage = rand() % 50;
		dinfo.CounterDamage = 0;
		dinfo.StunTime = 1000;//(rand() % 30 == 0) ? 2000 : 0;
		iter.AddTargetWithResultInfo(pObject->GetID(),1,&dinfo);
	}
	iter.Release();

	NetworkMsgParse( MP_SKILL_SKILL_RESULT, &msg2 );
#endif

	return TRUE;
}

BOOL cSkillManager::OnSkillCommand(CHero* pHero,CActionTarget* pTarget,BOOL bSkill)
{
	CBattle* pBattle = BATTLESYSTEM->GetBattle();
	if( pBattle && pBattle->GetBattleKind() == eBATTLE_KIND_GTOURNAMENT )
	{
		if( pHero->GetBattleTeam() == 2 )
			return FALSE;	
	}

//---KES ���󰡱�
	pHero->SetFollowPlayer( 0 );
//---------------

	BOOL bResult = FALSE;
	if( pHero->GetState() == eObjectState_SkillSyn ||
		pHero->GetState() == eObjectState_SkillUsing)
		bResult = SetNextComboSkill(pHero,pTarget,bSkill);
	else
		bResult = ExcuteSkillSYN(pHero,pTarget,bSkill);

	return bResult;
}

BOOL cSkillManager::OnSkillCommand( DWORD idx )
{
	if ( eObjectState_HouseRiding == HERO->GetState() )
	{
		//090527 pdy �Ͽ�¡ �ý��۸޼��� ž�»��½� �����ൿ [��ų]
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1876 ) );	//1876	"ž�� ���¸� ������ �� ����ϼž� �մϴ�."
		return FALSE;
	}

	cActiveSkillInfo* const pInfo = (cActiveSkillInfo*)m_SkillInfoTable.GetData( idx );

	if(0 == pInfo)
	{
		return FALSE;
	}

	// 091127 LUJ, �Ϻ� ��ų�� ���� ����� �� ����
	switch(pInfo->GetKind())
	{
	case SKILLKIND_PASSIVE:
	case SKILLKIND_BUFF:
		{
			return FALSE;
		}
	}

	// 091127 LUJ, ���� ��ų ó��
	if(UNITKIND_RECALL == pInfo->GetSkillInfo()->Unit)
	{
		switch(pInfo->GetSkillInfo()->AreaTarget)
		{
		case eSkillAreaTargetParty:
			{
				RecallParty(*pInfo->GetSkillInfo());
				break;
			}
		case eSkillAreaTargetGuild:
			{
				RecallGuild(*pInfo->GetSkillInfo());
				break;
			}
		}

		return FALSE;
	}

	CActionTarget Target;

	switch( pInfo->GetSkillInfo()->Target )
	{
	case eSkillTargetKind_None:
		{
			return FALSE;
		}
		break;
	case eSkillTargetKind_OnlyOthers:
		{
			CObject* const targetObj = OBJECTMGR->GetSelectedObject();

			if(0 == targetObj)
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(655) );
				return FALSE;
			}
			else if(targetObj->GetID() == gHeroID)
			{
				return FALSE;
			}

			if( targetObj->GetObjectKind() == eObjectKind_Player )
			{
				PKMGR->SetPKStartTimeReset();
			}

			if( pInfo->GetSkillInfo()->Unit == UNITKIND_RESURRECTION )
			{
				if( targetObj->GetObjectKind() != eObjectKind_Player)
				{
					return FALSE;
				}

				if( !targetObj->IsDied() || targetObj->GetState() != eObjectState_Die )
				{
					return FALSE;
				}

				Target.InitActionTarget(targetObj,NULL);
			}
			else
			{
				if( targetObj->IsDied() || targetObj->GetState() == eObjectState_Die )
				{
					return FALSE;
				}

				Target.InitActionTarget(targetObj,NULL);
				HERO->EnableAutoAttack(&Target);
			}
		}
		break;
	case eSkillTargetKind_OnlySelf:
		{
			Target.InitActionTarget(HERO,NULL);	
		}
		break;
	case eSkillTargetKind_AllObject:
		{
			CObject * targetObj = OBJECTMGR->GetSelectedObject();

			if(0 == targetObj)
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(655) );
				return FALSE;
			}
			else if( targetObj->IsDied() || targetObj->GetState() == eObjectState_Die )
			{
				return FALSE;
			}

			Target.InitActionTarget(targetObj,NULL);
			HERO->EnableAutoAttack(&Target);
		}
		break;
	}

	mpSkillInfo = pInfo;
	if( idx )
	{
		return OnSkillCommand( HERO, &Target, TRUE );
	}
	else
	{
		return OnSkillCommand( HERO, &Target, FALSE );
	}
}

BOOL cSkillManager::SetNextComboSkill(CHero* pHero,CActionTarget* pTarget,BOOL bSkill)
{
	cSkillInfo* pNextSkill;
	DWORD NextSkillIdx;
	/*
	if(bMugong)
	NextSkillIdx = GetMugongSkillIdx(pHero);
	else
	NextSkillIdx = GetComboSkillIdx(pHero);
	pNextSkill = GetSkillInfo(NextSkillIdx);
	*/ //GetMugongSkillIdx() �Լ��� ������� �ʰ� �Ǿ� ����
	if(bSkill)
		pNextSkill = mpSkillInfo;
	else
	{
		NextSkillIdx = GetComboSkillIdx(pHero);
		pNextSkill = GetSkillInfo(NextSkillIdx);
	}

	if(pNextSkill == NULL)
		return FALSE;

	CAction act;
	act.InitSkillAction( ( cActiveSkillInfo* )pNextSkill, pTarget );
	pHero->SetNextAction(&act);

	return TRUE;
}

void cSkillManager::OnPetComboTurningPoint(CHeroPet* pHeroPet)
{
	if(pHeroPet->GetNextAction()->HasAction())
	{
		pHeroPet->GetNextAction()->ExcutePetAction(pHeroPet);
		pHeroPet->GetNextAction()->Clear();
	}
	else
	{		
		if(pHeroPet->IsAutoAttacking())
		{
			if(OnPetSkillCommand(pHeroPet,pHeroPet->GetAutoAttackTarget(),FALSE) == FALSE)
				pHeroPet->DisableAutoAttack();
		}
	}
}

void cSkillManager::OnComboTurningPoint(CHero* pHero)
{
	if(pHero->GetNextAction()->HasAction())
	{
		if(pHero->GetNextAction()->GetActionKind() != eActionKind_Skill)
		{
			pHero->SetCurComboNum(0);
		}

		// 100702 ShinJS �׼ǽ����� ������ ��� �׼��� �����Ѵ�.
		if( pHero->GetNextAction()->ExcuteAction( pHero ) == TRUE )
		{
			pHero->GetNextAction()->Clear();
		}
	}
	else
	{		
		if(pHero->IsAutoAttacking())
		{
			if(pHero->GetCurComboNum() < 3)	// �ڵ������� �޺� 2������	12/3�� ȸ�� ��� 3���� 2�� �ٲ�
			{
				if(OnSkillCommand(pHero,pHero->GetAutoAttackTarget(),FALSE) == FALSE)
					pHero->DisableAutoAttack();
			}
			else
			{
				OBJECTSTATEMGR->StartObjectState(pHero,eObjectState_SkillDelay);
				OBJECTSTATEMGR->EndObjectState(pHero,eObjectState_SkillDelay,300);
				pHero->SetCurComboNum(0);
			}
		}
	}
}

cSkillObject* cSkillManager::DoCreateSkillObject(const SKILLOBJECT_INFO& objectInfo, const ITEMBASE& usedItem)
{
	if(cSkillObject* const skillObject = DoCreateSkillObject(objectInfo))
	{
		skillObject->GetSkillObjectInfo().usedItem = usedItem;

		return skillObject;
	}

	return NULL;
}

cSkillObject* cSkillManager::DoCreateSkillObject(const SKILLOBJECT_INFO& pSOInfo)
{
	cSkillInfo* const skillInfo = GetSkillInfo(
		pSOInfo.SkillIdx);

	if(NULL == skillInfo)
	{
		return NULL;
	}

	cSkillObject* skillObject = NULL;

	switch(skillInfo->GetKind())
	{
	case SKILLKIND_BUFF:
		{
			skillObject = new cBuffSkillObject(
				(cBuffSkillInfo*)skillInfo);
		}
		break;
	case SKILLKIND_PHYSIC:
	case SKILLKIND_MAGIC:
	case SKILLKIND_ONOFF:
		{
			skillObject = new cSkillObject(
				skillInfo);
		}
		break;
	}

	if(NULL == skillObject)
	{
		return NULL;
	}

	skillObject->Init(
		pSOInfo);
	
	if(cSkillObject* pPreObj = m_SkillObjectTable.GetData(skillObject->GetID()))
	{
		ReleaseSkillObject(
			pPreObj);
	}

	m_SkillObjectTable.Add(
		skillObject,
		skillObject->GetID());

	return skillObject;
}

void cSkillManager::DoCreateTempSkillObject(cActiveSkillInfo& pSkillInfo, CActionTarget& pTarget, const ITEMBASE& usedItem)
{
	SKILLOBJECT_INFO SkillObjectInfo = {0};
	SkillObjectInfo.SkillIdx = pSkillInfo.GetIndex();
	SkillObjectInfo.SkillObjectIdx = TEMP_SKILLOBJECT_ID;

	if(pSkillInfo.GetSkillInfo()->Target)
	{
		SkillObjectInfo.Pos = *pTarget.GetTargetPosition();
	}
	else
	{
		HERO->GetPosition( &SkillObjectInfo.Pos );
	}

	ROTATEINFO* pRotateInfo = HERO->GetBaseRotateInfo();
	if( !pRotateInfo )	return;
	const float fOperatorAngle = (pRotateInfo->bRotating == TRUE ? pRotateInfo->Angle.GetTargetAngleRad() : HERO->GetAngle());

	SkillObjectInfo.Direction = GetRadToVector( fOperatorAngle );
	SkillObjectInfo.SkillLevel = BYTE(pSkillInfo.GetLevel());
	SkillObjectInfo.Operator = gHeroID;

	pTarget.GetMainTarget(
		&SkillObjectInfo.MainTarget);

	CBattle* pBattle = BATTLESYSTEM->GetBattle();

	SkillObjectInfo.BattleID = pBattle->GetBattleID();

	float AttackSpeedRate = 1.0f;

	if( pSkillInfo.GetKind() == SKILLKIND_PHYSIC ||
		pSkillInfo.GetKind() == SKILLKIND_MAGIC )
	{
		DWORD idx = pSkillInfo.GetIndex();

		if( (idx / 100000) % 10 )
		{
			if( IsUsedPhysicAttackSpeed( *pSkillInfo.GetSkillInfo() ) )
			{
				AttackSpeedRate += ( ( HERO->GetRatePassiveStatus()->PhysicSkillSpeedRate + HERO->GetRateBuffStatus()->PhysicSkillSpeedRate ) * 0.01f );
			}
			else
			{
				float SpeedRate = min( HERO->GetRateBuffStatus()->MagicSkillSpeedRate + HERO->GetRatePassiveStatus()->MagicSkillSpeedRate, 99.f );
				AttackSpeedRate = ( 1 / ( 1 - ( SpeedRate*0.01f ) ) );

				if( (pSkillInfo.GetSkillInfo()->CastingTime / AttackSpeedRate) < MIN_MAGIC_CASTING_TIME )
				{
					AttackSpeedRate = pSkillInfo.GetSkillInfo()->CastingTime / MIN_MAGIC_CASTING_TIME;
				}
			}
		}
		else
		{
			AttackSpeedRate += ( ( HERO->GetRateBuffStatus()->NormalSpeedRate + HERO->GetRatePassiveStatus()->NormalSpeedRate ) * 0.01f );
		}
	}

	if( AttackSpeedRate <= 0 )
	{
		AttackSpeedRate = 1.0f;
	}

	SkillObjectInfo.Rate = AttackSpeedRate;

	VECTOR3 MainTargetPos;
	GetMainTargetPos(&SkillObjectInfo.MainTarget,&MainTargetPos,NULL);
	MOVEMGR->SetLookatPos(HERO,&MainTargetPos,0,gCurTime);

	DoCreateSkillObject(
		SkillObjectInfo,
		usedItem);

	HERO->SetCurrentSkill( TEMP_SKILLOBJECT_ID );

	if( HEROPET )
	{
		HEROPET->SetTarget( SkillObjectInfo.MainTarget.dwMainTargetID );
	}
}

BOOL cSkillManager::DoChangeTempSkillObject(SKILLOBJECT_INFO* pSOInfo)
{
	/// �ӽ� ��ü�� ��ü���ش�
	cSkillInfo* pSInfo = GetSkillInfo( pSOInfo->SkillIdx );

	/// �нú�� ���� ��ų�� �ӽ� ��ü�� ���鸮 ����.
	if( pSInfo->GetKind() == SKILLKIND_PASSIVE ||
		pSInfo->GetKind() == SKILLKIND_BUFF )
	{
		return FALSE;
	}

	/// �ӽ� ��ü�� �����´�.
	cSkillObject* pTempObj = m_SkillObjectTable.GetData( TEMP_SKILLOBJECT_ID );

	/// �ӽ� ��ü�� ����...
	if( !pTempObj )
	{
		return FALSE;
	}

	/// ��ų �ε����� �ٸ���!
	if( pTempObj->GetSkillIdx() != pSOInfo->SkillIdx )
	{
		return FALSE;
	}

	/// ��ų ������Ʈ ���̺��� �ӽ� ��ü�� �����.
	m_SkillObjectTable.Remove( TEMP_SKILLOBJECT_ID );
	
	/// ID �缳��
	pTempObj->ResetSkillObjectId( pSOInfo->SkillObjectIdx );

	/// �ٽ� ���̺� �ִ´�.
	m_SkillObjectTable.Add( pTempObj, pTempObj->GetID() );

	HERO->SetCurrentSkill( pSOInfo->SkillObjectIdx );

	return TRUE;
}

void cSkillManager::ReleaseSkillObject(cSkillObject* pSkillObject)
{
	m_SkillObjectTable.Remove(pSkillObject->GetID());
	OBJECTMGR->AddGarbageObject(pSkillObject);
	
	// 091211 LUJ, ��ų ������Ʈ ���� �������� ġ������ �޸� ������ �ִ�
	//			���� ĳ�������� �޸� ��� ũ�⿡ ������� ����Ͽ� �����
	//			�޸𸮿� ���� ���Ⱑ ���������� �߻��Ѵ�. �̸� ���ϱ� ����
	//			�ϴ� ������ ���� �ʵ��� �Ѵ�. �Ŀ� ���� �����ϵ��� ��ų
	//			������Ʈ ���� ���� �޸� Ǯ�� ���� �����̴�
	SAFE_DELETE(pSkillObject);
}

void cSkillManager::OnReceiveSkillObjectAdd(MSG_SKILLOBJECT_ADD2* pmsg)
{
	CObject* pOperator = OBJECTMGR->GetObject(pmsg->SkillObjectInfo.Operator);

	if(pOperator == NULL)
	{
		return;
	}

	DWORD SkillIdx = pmsg->SkillObjectInfo.SkillIdx;
	cSkillInfo* pSkillInfo = GetSkillInfo(SkillIdx);

	BOOL rt = FALSE;

	if( pOperator == HERO && pmsg->bCreate )
	{
		rt = DoChangeTempSkillObject( &pmsg->SkillObjectInfo );
	}

	DWORD dwTargetID	=	GAMEIN->GetMonsterGuageDlg()->GetWantedTargetID();
	DWORD dwTargetVSID	=	0;

	if( pOperator->GetID() == dwTargetID )
	{
		dwTargetVSID	=	pmsg->SkillObjectInfo.MainTarget.dwMainTargetID;
		GAMEIN->GetMonsterGuageDlg()->SetTargetVersusInfo( dwTargetVSID );
	}

	if( !rt )
	{
		if( pSkillInfo )
		{
			// 100127 ShinJS --- Skill �����ڿ� ����� ���� ���
			if( pmsg->SkillObjectInfo.MainTarget.dwMainTargetID == pmsg->SkillObjectInfo.Operator )
			{
				//  �������� ������ ��ų������ ������ �������� ������.
				const float fRad = VECTORTORAD( pmsg->SkillObjectInfo.Direction );
				pOperator->SetAngle( fRad );
			}
			else
			{
				VECTOR3 MainTargetPos;
				GetMainTargetPos(&pmsg->SkillObjectInfo.MainTarget,&MainTargetPos,NULL);
				MOVEMGR->SetLookatPos(pOperator,&MainTargetPos,0,gCurTime);
			}

			DoCreateSkillObject(
				pmsg->SkillObjectInfo);
		}
		else
		{
			ASSERT( 0 );
		}		
	}
}
void cSkillManager::OnReceiveSkillObjectRemove(MSG_DWORD* pmsg)
{
	cSkillObject* pSObj = GetSkillObject(pmsg->dwData);
	if(pSObj == NULL)
	{
		//ASSERTMSG(0,"SkillObject Remove Failed");
		return;
	}

	if( HERO->GetCurrentSkill() == pSObj->GetID() )
		HERO->SetCurrentSkill( 0 );

	ReleaseSkillObject(pSObj);
}

void cSkillManager::OnReceiveSkillResult(MSG_SKILL_RESULT* pmsg)
{
	cSkillObject* pSObj = GetSkillObject(pmsg->SkillObjectID);
	if(pSObj == NULL)
	{
		OBJECTACTIONMGR->ApplyTargetList(NULL,&pmsg->TargetList,eDamageKind_Front);
		return;
	}

	pSObj->OnReceiveSkillResult(pmsg);
}

void cSkillManager::OnReceiveSkillCancel(MSG_DWORD* pmsg)
{
	cSkillObject* pSObj = GetSkillObject(pmsg->dwData);

	if( !pSObj )
		return;

	pSObj->Cancel();

	if( pmsg->dwObjectID == HEROID )
	{
		HERO->SetSkillCancelDelay( 0 );
		HERO->SetCurrentSkill( 0 );
		HERO->DisableAutoAttack();
		HERO->SetNextAction( 0 );
		HERO->StartSkillProgressBar( 0 );

		switch( HERO->GetState() )
		{
		case eObjectState_SkillStart:
		case eObjectState_SkillSyn:
		case eObjectState_SkillBinding:
		case eObjectState_SkillUsing:
		case eObjectState_SkillDelay:
			{
				OBJECTSTATEMGR->EndObjectState( HERO, HERO->GetState() );
			}
			break;
		}

		if(CItem* const item = ITEMMGR->GetItem(pSObj->GetSkillObjectInfo().usedItem.dwDBIdx))
		{
			item->SetLock(
				FALSE);
		}
	}
}

// 080318 LUJ, ��Ƽ ���� â�� �����ϵ��� ��
void cSkillManager::OnReceiveSkillDelayReset(MSG_DWORD2* pmsg)
{
	cSkillObject* pSObj = GetSkillObject(pmsg->dwData1);

	if( !	pSObj ||
		pSObj->GetSkillInfo()->GetKind() != SKILLKIND_BUFF )
	{
		return;
	}

	cBuffSkillObject* const			pBuffObj		= ( cBuffSkillObject* )pSObj;
	const BUFF_SKILL_INFO* const	buffSkillInfo	= ( ( cBuffSkillInfo* )pSObj->GetSkillInfo() )->GetSkillInfo();

	if( ! buffSkillInfo )
	{
		return;
	}

	// 080122 NYJ - ������ų�� ���۽ð��� �������ش�.
	pBuffObj->SetBeginTime(gCurTime);

	if(buffSkillInfo->DelayTime)
	{
		pBuffObj->GetSkillObjectInfo().remainTime = pmsg->dwData2;
	}

	// 090204 LUJ, �ð��� ���� ������ ���ÿ� �ο��� �� �����Ƿ� �׻� �������ش�
	pBuffObj->SetCount( buffSkillInfo->Count );
	pBuffObj->ResetDelay();

	// 080318 LUJ, ��Ƽ ����â ����
	{
		SKILLOBJECT_INFO objectInfo	= {0};
		objectInfo.SkillIdx	= pBuffObj->GetSkillIdx();
		objectInfo.SkillObjectIdx = pBuffObj->GetID();
		objectInfo.RemainTime = pmsg->dwData2;
		objectInfo.MainTarget.dwMainTargetID = pBuffObj->GetSkillObjectInfo().mainTarget.dwMainTargetID;

		// 080318 LUJ, �ð��� ���� ������ ��� ������ �����̹Ƿ� ������� �ð��� �־�����
		if(0 == objectInfo.RemainTime)
		{
			objectInfo.RemainTime = buffSkillInfo->DelayTime;
		}

		objectInfo.RemainTime += gCurTime;

		GAMEIN->GetPartyDialog()->AddSkill( objectInfo );
	}
}

// 090204 LUJ, ����ΰ� ������ ������ �׻� ī��Ʈ�� ������Ʈ�Ѵ�
void cSkillManager::OnReceiveSkillCount(MSG_DWORD2* pmsg)
{
	cSkillObject* pSObj = GetSkillObject( pmsg->dwData1 );

	if( pSObj && pSObj->GetSkillInfo()->GetKind() == SKILLKIND_BUFF )
	{
		cBuffSkillObject* pBuffObj = ( cBuffSkillObject* )pSObj;

		// 090204 LUJ, ���� ����� ������� ��� ī��Ʈ�� �ٲ���Ѵ�
		if( pBuffObj->GetSkillObjectInfo().mainTarget.dwMainTargetID == HEROID )
		{
			pBuffObj->SetCount( ( WORD )pmsg->dwData2 );
		}
	}
}

// 080318 LUJ, ��Ƽ���� ��ų �߰�/����/�������� ���, ��Ƽ â�� ǥ���ϵ��� ��
void cSkillManager::NetworkMsgParse(BYTE Protocol,void* pMsg)
{
	switch(Protocol)
	{
	case MP_SKILL_SKILLOBJECT_ADD:
		{
			MSG_SKILLOBJECT_ADD2* pmsg = (MSG_SKILLOBJECT_ADD2*)pMsg;
			
			SKILLOBJECT_INFO	objectInfo	= pmsg->SkillObjectInfo;
			cBuffSkillInfo*		skillInfo	= ( cBuffSkillInfo* )( GetSkillInfo( objectInfo.SkillIdx ) );

			if( !skillInfo )
				return;

			SKILLKIND	eSkillKind	=	skillInfo->GetKind();
			// 080318 LUJ, ��Ƽ������ ������ �߰��Ѵ�
			
			CPartyDialog*		dialog		= GAMEIN->GetPartyDialog();

			if(	eSkillKind == SKILLKIND_BUFF )// && dialog )
			{
				if( dialog )
				{
					// 080318 LUJ, �ð��� ���� ������ ��� ������ �����̹Ƿ� ������� �ð��� �־�����
					if( ! objectInfo.RemainTime )
					{
						objectInfo.RemainTime = skillInfo->GetSkillInfo()->DelayTime;
					}

					// 080318 LUJ, ���� �ð��� ������� ���������� ǥ�õȴ�
					objectInfo.RemainTime	+= gCurTime;
					objectInfo.SkillIdx		= skillInfo->GetSkillInfo()->Index;					

					dialog->AddSkill( objectInfo );
				}
				
				//��ų�� ���Ÿ���� ���� ������� ����
				DWORD	dwHeroTargetID	=	GAMEIN->GetMonsterGuageDlg()->GetWantedTargetID();
				DWORD	dwTargetID		=	pmsg->SkillObjectInfo.MainTarget.dwMainTargetID;

				CObject* pTargetObject = OBJECTMGR->GetObject(dwTargetID);

				if( pTargetObject )	
				{
					pTargetObject->InsertBuffSkill( &objectInfo );
					if( dwHeroTargetID == dwTargetID )	
					{
						pTargetObject->UpdateTargetBuffSkill();
					}
				}
			
			}

			OnReceiveSkillObjectAdd(pmsg);
		}
		break;
	case MP_SKILL_SKILLOBJECT_REMOVE:
		{
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

			// 080318 LUJ, ��Ƽ�����Լ� ������ �����ش�
			{
				SKILLOBJECT_INFO objectInfo = { 0 };
				objectInfo.SkillObjectIdx	= pmsg->dwData;

				cSkillObject*	skillObject = GetSkillObject( pmsg->dwData );
				cSkillInfo*		skillInfo	= 0;
				
				if( skillObject )
				{
					skillInfo = skillObject->GetSkillInfo();
					objectInfo.Operator = skillObject->GetSkillObjectInfo().operatorId;
					objectInfo.SkillIdx	= skillObject->GetSkillIdx();
					objectInfo.MainTarget.dwMainTargetID = skillObject->GetSkillObjectInfo().mainTarget.dwMainTargetID;
				}

				CPartyDialog* dialog = GAMEIN->GetPartyDialog();

				if( skillInfo								&&
					skillInfo->GetKind() == SKILLKIND_BUFF )
				{
					if( dialog )
					{
						dialog->RemoveSkill( objectInfo );
					}

					// ��ų�� ���Ÿ���� ���� ������� ����
					const DWORD dwHeroTargetID = GAMEIN->GetMonsterGuageDlg()->GetWantedTargetID();
					const DWORD dwTargetID = skillObject->GetSkillObjectInfo().mainTarget.dwMainTargetID;
					CObject* const pTargetObject = OBJECTMGR->GetObject(objectInfo.MainTarget.dwMainTargetID);

					if( pTargetObject )
					{
						pTargetObject->RemoveBuffSkill( objectInfo.SkillIdx );
						if( dwHeroTargetID == dwTargetID )
						{
							pTargetObject->UpdateTargetBuffSkill();
						}
					}
				}
			}

			OnReceiveSkillObjectRemove(pmsg);
		}
		break;
	case MP_SKILL_SKILL_RESULT:
		{
			MSG_SKILL_RESULT* pmsg = (MSG_SKILL_RESULT*)pMsg;
			OnReceiveSkillResult(pmsg);
		}
		break;
	case MP_SKILL_START_NACK:
		{
			const MSG_DWORD2* message = (MSG_DWORD2*)pMsg;
			OBJECTSTATEMGR->StartObjectState(HERO,eObjectState_None);
			const DWORD	skillIndex = message->dwData1;
			if( HERO->GetCurrentSkill() == skillIndex )
				HERO->SetCurrentSkill( 0 );

			// 080610 LUJ, GM������ ��ų �ߵ� �� ���� ������ �ֿܼ� ��Ÿ������ ��
#ifdef _GMTOOL_
			const SKILL_RESULT	result		= SKILL_RESULT( message->dwData2 );

			cSkillInfo* skillInfo = GetSkillInfo( skillIndex );

			if( ! skillInfo )
			{
				break;
			}

            const char* resultText = "unknown";

			switch( result )
			{
			case SKILL_OPERATOR_INVALUED:
				{
					resultText	= "opeator is invalued";
					break;
				}
			case SKILL_INFO_INVALUED:
				{
					resultText	= "information is invalued";
					break;
				}
			case SKILL_LEVEL_INVALUED:
				{
					resultText	= "level is invalued";
					break;
				}
			case SKILL_STATE_INVALUED:
				{
					resultText	= "state is invalued";
					break;
				}
			case SKILL_TARGET_INVALUED:
				{
					resultText	= "target is invalued";
					break;
				}
			case SKILL_RANGE_NOTENOUGH:
				{
					resultText	= "range is not enough";
					break;
				}
			case SKILL_COOLING:
				{
					resultText	= "state is cooling";
					break;
				}
			case SKILL_FAIL_ETC:
				{
					resultText	= "failed by unknown reason";
					break;
				}
			case SKILL_DESTROY:
				{
					resultText	= "skill is destoryed";
					break;
				}
			}

			cprintf( "%s(%d): %s\n",
				skillInfo->GetName(),
				skillInfo->GetIndex(),
				resultText );
#endif
		}
		break;
	case MP_SKILL_CANCEL_NOTIFY:
		{
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
			OnReceiveSkillCancel(pmsg);
		}
		break;
	case MP_SKILL_CANCEL_NACK:
		{
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
			if( pmsg->dwObjectID == HEROID )
			{
				const float fCancelDelayTime = pmsg->dwData / 1000.f;
				HERO->SetSkillCancelDelay( gCurTime + pmsg->dwData );
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2315 ), fCancelDelayTime );		// %.1f�� �Ŀ� ��ų�� ����Ҽ� �ֽ��ϴ�.
			}
		}
		break;
	case MP_SKILL_DELAY_RESET_NOTIFY:
		{
			MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
			OnReceiveSkillDelayReset(pmsg);
		}
		break;
	case MP_SKILL_START_DESTROY_ACK:
		{
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
			if( pmsg->dwObjectID != HEROID )
				break;

			const DWORD dwItemDBIndex = pmsg->dwData;
			HERO->SetCurrentSkill( 0 );

			// �Һ�������� �ִ� ��ų�� ��� Lock ����
			CItem* pItem = ITEMMGR->GetItem( dwItemDBIndex );
			if( pItem && pItem->IsLocked() )
			{
				pItem->SetLock( FALSE );
			}
		}
		break;

	// 080602 LUJ, ��ų �ߵ� ����
	case MP_SKILL_CONSUME_NACK:
		{
			const MSG_DWORD2*	message		= ( MSG_DWORD2* )pMsg;
			const DWORD			skillIndex	= message->dwData1;
			const DWORD			itemDbIndex	= message->dwData2;

			// 080602 LUJ, �Ҹ�� �������� ��������Ƿ� Ǯ����� �Ѵ�.
			{
				CItem* item = ITEMMGR->GetItem( itemDbIndex );

				if( item )
				{
					item->SetLock( FALSE );
				}
			}

			{
				cActiveSkillInfo* skillInfo = ( cActiveSkillInfo* )GetSkillInfo( skillIndex );

				if( ! skillInfo )
				{
					break;
				}

				const ACTIVE_SKILL_INFO* activeSkillInfo = skillInfo->GetSkillInfo();

				if( ! activeSkillInfo )
				{
					break;
				}

				CHATMGR->AddMsg(
					CTC_SYSMSG,
					"%s%s",
					activeSkillInfo->Name,
					CHATMGR->GetChatMsg( 1543 ) );
			}
		}
		break;
	case MP_SKILL_COUNT_NOTIFY:
		{
			MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
			OnReceiveSkillCount(pmsg);
		}
		break;
	// 100211 ONS ��Ȱ�迭 ��ų ���� ����ڿ��� ��Ȱ�ǻ縦 ���� ó�� �߰�
	case MP_SKILL_RESURRECT_SYN:
		{
			MSG_NAME* pmsg = (MSG_NAME*)pMsg;

			CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject(pmsg->dwObjectID);
			if( !pPlayer ) 
				return;

			// ��Ȱ��ġ ���� â�� �ݴ´�.
			CReviveDialog* pDlg = GAMEIN->GetReviveDialog();
			if( pDlg && pDlg->IsActive() )
			{
				pDlg->SetActive( FALSE );
			}

			// ��Ȱ���θ� ������ �޼����ڽ��� �����Ѵ�.
			WINDOWMGR->MsgBox( MBI_RESURRECT_ASK, MBT_YESNO, CHATMGR->GetChatMsg(2033), pmsg->Name );
		}
		break;

		// 100615 ShinJS --- ���� ������������ ���Ͽ� �������� ���� ���
	case MP_SKILL_BUFF_START_FAIL_NOTIFY:
		{
			MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
			const eBUFF_START_RESULT result = (eBUFF_START_RESULT)pmsg->dwData1;
			const DWORD dwBuffIndex = pmsg->dwData2;
			cBuffSkillInfo* pSkillInfo = (cBuffSkillInfo*)GetSkillInfo( dwBuffIndex );
			if( !pSkillInfo )
				break;

			BUFF_SKILL_INFO* pBuffInfo = pSkillInfo->GetSkillInfo();
			if( !pBuffInfo )
				break;

			switch( result )
			{
			case eBUFF_START_RESULT_BATTLE_RULE_DELETE:
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2292 ), pBuffInfo->Name );
				break;
			case eBUFF_START_RESULT_PEACE_RULE_DELETE:
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2293 ), pBuffInfo->Name );
				break;
			case eBUFF_START_RESULT_MOVE_RULE_DELETE:
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2294 ), pBuffInfo->Name );
				break;
			case eBUFF_START_RESULT_STOP_RULE_DELETE:
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2295 ), pBuffInfo->Name );
				break;
			case eBUFF_START_RESULT_REST_RULE_DELETE:
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2296 ), pBuffInfo->Name );
				break;
			}
		}
		break;
	}
}


void cSkillManager::AddPassive( DWORD SkillIdx )
{
	cActiveSkillInfo* pActive = ( cActiveSkillInfo* )GetSkillInfo( SkillIdx );

	if(0 == pActive)
	{
		return;
	}
	
	if( pActive->GetKind() != SKILLKIND_PASSIVE )
	{
		return;
	}

	for(DWORD cnt = 0; cnt < MAX_BUFF_COUNT; ++cnt)
	{
		const DWORD index = pActive->GetSkillInfo()->Buff[ cnt ];
		cBuffSkillInfo* pSkillInfo = ( cBuffSkillInfo* )GetSkillInfo( index );

		if(0 == pSkillInfo)
		{
			break;
		}

		// 090204 LUJ, Ÿ���� ��Ȯ���Ѵ�
		if( BUFF_SKILL_INFO::Condition::TypeNone != pSkillInfo->GetSkillInfo()->mCondition.mType )
		{
			HERO->AddSpecialSkill( pSkillInfo );
		}
		else if(HERO->IsEnable(*pSkillInfo->GetSkillInfo()))
		{
			pSkillInfo->AddPassiveStatus();
		}
	}
}

void cSkillManager::RemovePassive( DWORD SkillIdx )
{
	cActiveSkillInfo* pActive = ( cActiveSkillInfo* )GetSkillInfo( SkillIdx );

	if(0 == pActive)
	{
		return;
	}
	
	for(DWORD cnt = 0; cnt < MAX_BUFF_COUNT; ++cnt)
	{
		const DWORD index = pActive->GetSkillInfo()->Buff[ cnt ];
		cBuffSkillInfo* pSkillInfo = ( cBuffSkillInfo* )GetSkillInfo( index );

		if(0 == pSkillInfo)
		{
			break;
		}

		// 090204 LUJ, Ÿ���� ��Ȯ���Ѵ�
		if( BUFF_SKILL_INFO::Condition::TypeNone != pSkillInfo->GetSkillInfo()->mCondition.mType )
		{
			HERO->RemoveSpecialSkill( pSkillInfo );
		}
		else if(FALSE == HERO->IsEnable(*pSkillInfo->GetSkillInfo()))
		{
			pSkillInfo->RemovePassiveStatus();
		}
	}
}

DWORD cSkillManager::GetSkillSize( DWORD index ) const
{
	SkillSize::const_iterator it = mSkillSize.find( index );

	return mSkillSize.end() == it ? 0 : it->second;
}


cSkillManager* cSkillManager::GetInstance()
{
	// ����: http://www.codeproject.com/useritems/VC2003MeyersSingletonBug.asp
	//
	// ���� �ζ��� �Լ��� �����ø��� �����ȴ�(�ζ��� �Լ��� ���� ��ġ�� �� ��ũ���� �ʴ´�). 
	// �̷� ���� �̱��� �������� �����ø��� �����ȴ�!
	// �̸� �������� ������ ��ũ ������ ���������Ѵ�. C ���Ͽ� ���� �����ϸ� �ذ�ȴ�.
	static cSkillManager instance;

	return &instance;
}

void cSkillManager::Process()
{
	m_SkillObjectTable.SetPositionHead();

	for(cSkillObject* object = m_SkillObjectTable.GetData();
		0 < object;
		object = m_SkillObjectTable.GetData())
	{
		object->Update();
	}
}

void cSkillManager::SkillCancel( cSkillObject* pSkill )
{
	// �ӽ� ��ü�� ����Ҽ� ����.
    if( pSkill == NULL ||
		pSkill->GetID() == TEMP_SKILLOBJECT_ID )
	{
		return;
	}

	const DWORD dwSkillDelayTime = HERO->GetSkillCancelDelay();
	if( dwSkillDelayTime > gCurTime )
	{
		const float fCancelDelayTime = (dwSkillDelayTime - gCurTime) / 1000.f;
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2315 ), fCancelDelayTime );		// %.1f�� �Ŀ� ��ų�� ����Ҽ� �ֽ��ϴ�.
		return;
	}
	
	// ������ ��� �޼����� ������
	MSG_DWORD msg;
	msg.Category = MP_SKILL;
	msg.Protocol = MP_SKILL_CANCEL_SYN;
	msg.dwObjectID = HEROID;
	msg.dwData = pSkill->GetID();

	NETWORK->Send( &msg, sizeof( msg ) );
}





//=========================================================================
//	NAME : IsUseableBuff_InParty
//	DESC : 071228 LYW 
//		 : ��Ƽ �� ����� �� �ִ� ���� ��ų���� üũ�ϴ� �Լ� �߰�.
//=========================================================================
BOOL cSkillManager::IsUseableBuff_InParty(DWORD dwSkillIdx)
{
	cSkillInfo* pSkillInfo = NULL ;
	pSkillInfo = (cActiveSkillInfo*)GetSkillInfo(dwSkillIdx) ;						// ��Ƽ�� ��ų ������ �޴´�.

	if( !pSkillInfo )																// ��Ƽ�� ��ų ���Ἲ üũ.
	{
		//MessageBox(NULL, "Failed to receive si", "Check bInfo", MB_OK) ;
		return FALSE ;
	}

	DWORD dwBuffSkillIdx  ;
	cBuffSkillInfo* pBuffSkillInfo ;
	BUFF_SKILL_INFO* pBuffInfo ;

	for( int count = 0 ; count < 5 ; ++count )
	{
		dwBuffSkillIdx = 0 ;
		dwBuffSkillIdx = ((cActiveSkillInfo*)pSkillInfo)->GetBuffSkillIdx(count) ;	// ��Ƽ�� ��ų�� ���� �ε����� �޴´�.

		pBuffSkillInfo = NULL ;												
		pBuffSkillInfo =  (cBuffSkillInfo*)GetSkillInfo(dwBuffSkillIdx) ;			// ���� ��ų ������ �޴´�.

		if( !pBuffSkillInfo )														// ���� ��ų ���� ���Ἲ üũ.
		{
			//MessageBox(NULL, "Failed to receive bsi", "Check bInfo", MB_OK) ;
			continue;
		}

		pBuffInfo = NULL ;	
		pBuffInfo = pBuffSkillInfo->GetSkillInfo() ;								// ���� ��ų ���� ����ü�� �޴´�.

		if( !pBuffInfo )															// ���� ��ų ���� ����ü ���Ἲ üũ.
		{	
			//MessageBox(NULL, "Failed to receive bi", "Check bInfo", MB_OK) ;
			return FALSE ;
		}

		if( pBuffInfo->Party == 1 )													// ��Ƽ�� ����� �� ���� �������̸�,
		{
			return FALSE ;															// FALSE ����.
		}
	}

	return TRUE ;																	// ��� ��ο����� ���� ���� ��� �ڵ�.
}


// ��ų�� ���
BOOL cSkillManager::ExcutePetSkillSYN(CHeroPet* pHeroPet,CActionTarget* pTarget,BOOL bSkill)
{
	DWORD SkillNum;
	cSkillInfo* pSkillInfo;

	/// ��ų�� �⺻������ ���
	if(bSkill == FALSE)
	{
		/// �⺻���� ������ �����´�
		SkillNum = GetPetComboSkillIdx(pHeroPet);
		pSkillInfo = GetSkillInfo(SkillNum);
		// 081021 LYW --- SkillManager : �� �� ��ų ���� ������ �߰� ó��.
		//mpSkillInfo = pSkillInfo;
		mpPetSkillInfo = pSkillInfo ;
	}
	else
		// 081021 LYW --- SkillManager : �� �� ��ų ���� ������ �߰� ó��.
		//pSkillInfo = mpSkillInfo;
		pSkillInfo = mpPetSkillInfo;
		
	/// ��ų ������ ���ٸ� ����
	if(pSkillInfo == NULL)
		return FALSE;

	/// ��ų�� �����Ѵ�
	return ExcutePetSkillSYN(pHeroPet,pTarget,pSkillInfo);
}

BOOL cSkillManager::ExcutePetSkillSYN(CHeroPet* pHeroPet,CActionTarget* pTarget,cSkillInfo* pInfo)
{
	CActionTarget target;
	target.CopyFrom(pTarget);
	
	/// ��ų ������ ������ ����
	if(pInfo == NULL)
		return FALSE;

//	if( pInfo->GetKind() == SKILLKIND_BUFF || pInfo->GetKind() == SKILLKIND_PASSIVE )
//		return FALSE;

	cActiveSkillInfo* pSkillInfo = ( cActiveSkillInfo* )pInfo;
	
	/// ���� ���� ������� �Ǵ�
	if(pSkillInfo->IsValidTarget( HERO, pTarget ) == FALSE )
	{
		return FALSE;
	}

	/// ��ų ������ �����´�
	//int SkillLevel = pHero->GetSkillLevel(pSkillInfo->GetIndex());
	int SkillLevel = pSkillInfo->GetIndex() % 100;

	/// ���� �������� �Ǵ�
	if(pSkillInfo->IsExcutablePetSkillState(pHeroPet,SkillLevel,&target) == FALSE)
	{
		/// �⺻ �������� ��ȯ�ϰ� ��ų ������ �Ѵ�
		//pHero->SetCurComboNum(0);
		//printf( "%d\t%s\n", 0, "cSkillManager::ExcuteSkillSYN1", 0 );
		return FALSE;
	}

	/// Ÿ�� ���� - �����ؾ� ��

	/// ��ȭ����� ���
	// 070124 LYW --- PeaceWar : Delete peacewar part.
	/*
	if(PEACEWARMGR->IsPeaceMode(pHero) == TRUE)
		/// �������� �ٲ۴�
		PEACEWARMGR->ToggleHeroPeace_WarMode();
		*/
	
	/// Ÿ���� ��ġ���� ������ ����
	VECTOR3* pTargetPos = target.GetTargetPosition();
	if(pTargetPos == NULL)
		return FALSE;
	
	if(pSkillInfo->IsInRange(*pHeroPet, target))
	{	
		/// �̵� ����
		MOVEMGR->HeroPetMoveStop();
		
		/// �ڽ��� Ÿ���� ��쿡�� ���� ������ �ٲ��� �ʴ´�.
		if( pSkillInfo->GetSkillInfo()->Target != 1 )
			MOVEMGR->SetLookatPos(pHeroPet,pTargetPos,0,gCurTime);
				
		/// �̵� �ൿ�� �ʱ�ȭ
		pHeroPet->SetMovingAction(NULL);

		/// ��ų ���
		return RealExcutePetSkillSYN(pHeroPet,&target,pSkillInfo);
	}

	CAction MoveAction;
	MoveAction.InitSkillAction(pSkillInfo,&target);
	MOVEMGR->SetHeroPetActionMove(&target,&MoveAction);
	return FALSE;
}

BOOL cSkillManager::RealExcutePetSkillSYN(CHeroPet* pHeroPet,CActionTarget* pTarget,cSkillInfo* pInfo)
{
	/// ��ų ������ ������ ����
	if(pInfo == NULL)
		return FALSE;
	
//	if( pInfo->GetKind() == SKILLKIND_BUFF || pInfo->GetKind() == SKILLKIND_PASSIVE )
//		return FALSE;

	cActiveSkillInfo* pSkillInfo = ( cActiveSkillInfo* )pInfo;
	
	/// ��ų ������ �����´�
	//int SkillLevel = pHero->GetSkillLevel(pSkillInfo->GetIndex());
	int SkillLevel = pSkillInfo->GetIndex() % 100;

	/// ���� �������� �Ǵ�
	if(pSkillInfo->IsExcutablePetSkillState(pHeroPet,SkillLevel,pTarget) == FALSE)
	{
		return FALSE;
	}

	// 080602 LUJ, �Ҹ� ������ ������ ����
	ITEMBASE consumeItem;
	ZeroMemory( &consumeItem, sizeof( consumeItem ) );

	// 080602 LUJ, �Ҹ� ����Ʈ/�������� ������� �˻�
	{
		const ACTIVE_SKILL_INFO*	info	= pSkillInfo->GetSkillInfo();
		CHero*						hero	= OBJECTMGR->GetHero();

		if( ! info ||
			! hero )
		{
			return FALSE;
		}

		// 080602 LUJ, ����Ʈ �˻�
		{
			const ACTIVE_SKILL_INFO::ConsumePoint& point = info->mConsumePoint;

			switch( point.mType )
			{
			case ACTIVE_SKILL_INFO::ConsumePoint::TypeFamily:
				{
					CSHFamily* family = hero->GetFamily();

					if( ! family )
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 828 ) );
						return FALSE;
					}

					const CSHFamily::stINFO_EX* info = family->GetEx();

					if( !	info	||
							info->nHonorPoint < point.mValue )
					{
						CHATMGR->AddMsg(
							CTC_SYSMSG,
							CHATMGR->GetChatMsg( 1568 ),
							point.mValue - info->nHonorPoint );
						return FALSE;
					}
					
					break;
				}
			case ACTIVE_SKILL_INFO::ConsumePoint::TypeGuild:
				{
					if( ! hero->GetGuildIdx() )
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 939 ) );
						return FALSE;
					}
					else if( GUILDMGR->GetScore() < point.mValue )
					{
						CHATMGR->AddMsg(
							CTC_SYSMSG,
							CHATMGR->GetChatMsg( 1566 ),
							point.mValue - GUILDMGR->GetScore() );
						return FALSE;
					}

					break;
				}
			case ACTIVE_SKILL_INFO::ConsumePoint::TypeExperience:
				{
					if( hero->GetExpPoint() < EXPTYPE( point.mValue ) )
					{
						CHATMGR->AddMsg(
							CTC_SYSMSG,
							CHATMGR->GetChatMsg( 1567 ),
							EXPTYPE( point.mValue ) - hero->GetExpPoint() );
						return FALSE;
					}

					break;
				}
			case ACTIVE_SKILL_INFO::ConsumePoint::TypePlayerKill:
				{
					if( hero->GetBadFame() < FAMETYPE( point.mValue ) )
					{
						CHATMGR->AddMsg(
							CTC_SYSMSG,
							CHATMGR->GetChatMsg( 1565 ),
							FAMETYPE( point.mValue ) - hero->GetBadFame() );
						return FALSE;
					}

					break;
				}
			}
		}

		// 080602 LUJ, ������ �˻�
		if( info->Money > hero->GetMoney() )
		{
			CHATMGR->AddMsg(
				CTC_SYSMSG,
				CHATMGR->GetChatMsg( 1333 ),
				info->Money - hero->GetMoney() );

			return FALSE;
		}
		// 080602 LUJ, ������ �˻�
		else if( info->mConsumeItem.wIconIdx )
		{
			CInventoryExDialog* inventoryDialog	= GAMEIN->GetInventoryDialog();

			if( ! inventoryDialog )
			{
				return FALSE;
			}

			const POSTYPE inventoryStartPosition = TP_INVENTORY_START;
			const POSTYPE inventoryEndPosition = POSTYPE(TP_INVENTORY_END + TABCELL_INVENTORY_NUM * HERO->Get_HeroExtendedInvenCount());

			for(
				POSTYPE position = inventoryStartPosition;
				inventoryEndPosition > position;
				++position )
			{
				CItem* item = inventoryDialog->GetItemForPos( position );

				if( !	item													||
						item->GetItemIdx()		!= info->mConsumeItem.wIconIdx	||
					(	ITEMMGR->IsDupItem( item->GetItemIdx() )	&&
						item->GetDurability()	< info->mConsumeItem.Durability ) )
				{
					continue;
				}

				consumeItem = item->GetItemBaseInfo();
				break;
			}

			// 080602 LUJ, �Ҹ��� �������� ã�� ����
			if( ! consumeItem.wIconIdx )
			{
				const ITEM_INFO* itemInfo = ITEMMGR->GetItemInfo( info->mConsumeItem.wIconIdx );

				if( ! itemInfo )
				{
					return FALSE;
				}

				CHATMGR->AddMsg(
					CTC_SYSMSG,
					CHATMGR->GetChatMsg( 1543 ),
					itemInfo->ItemName, info->mConsumeItem.Durability );
				
				return FALSE;
			}
		}
	}

	/// ȿ���ߵ� �����̰� ���� ��ų�� Ŭ���̾�Ʈ���� �ٷ� Ÿ�ٸ���Ʈ �ۼ�
	//if( pSkillInfo->GetSkillInfo()->DelayType == 0 )
	{
		VECTOR3* pTargetPos = pTarget->GetTargetPosition();
		if(pTargetPos == NULL)
			return FALSE;

		switch( pSkillInfo->GetSkillInfo()->Area )
		{
		case 1:
			{
				pTarget->SetTargetObjectsInRange(
					pTargetPos,
					pSkillInfo->GetSkillInfo()->AreaData,
					POSTYPE(pSkillInfo->GetSkillInfo()->AreaTarget));
			}
			break;
		}
	}

	float AttackSpeedRate = 1.0f;

	DWORD idx = pInfo->GetIndex();

	if( (idx / 100000) % 10 )
	{
		if( IsUsedPhysicAttackSpeed( *pSkillInfo->GetSkillInfo() ) )
		{
			AttackSpeedRate += ( ( pHeroPet->GetRatePassiveStatus()->PhysicSkillSpeedRate + pHeroPet->GetRateBuffStatus()->PhysicSkillSpeedRate ) * 0.01f );
		}
		else
		{
			float SpeedRate = min( pHeroPet->GetRatePassiveStatus()->MagicSkillSpeedRate + pHeroPet->GetRateBuffStatus()->MagicSkillSpeedRate, 99.f );
			AttackSpeedRate = ( 1 / ( 1 - ( SpeedRate*0.01f ) ) );
			if( (pSkillInfo->GetSkillInfo()->CastingTime / AttackSpeedRate) < MIN_MAGIC_CASTING_TIME )
				AttackSpeedRate = pSkillInfo->GetSkillInfo()->CastingTime / MIN_MAGIC_CASTING_TIME;
		}
	}
	else
	{
		AttackSpeedRate += ( ( pHeroPet->GetRatePassiveStatus()->NormalSpeedRate + pHeroPet->GetRateBuffStatus()->NormalSpeedRate ) * 0.01f );
	}
	
	if( AttackSpeedRate <= 0 )
	{
		AttackSpeedRate = 1.0f;
	}

	// 090309 LUJ, ��� ������ ���� ĳ����
	DWORD time = DWORD( pSkillInfo->GetSkillInfo()->AnimationTime / AttackSpeedRate );
	pHeroPet->GetSkillDelay()->Init( time );
	pHeroPet->GetSkillDelay()->Start();

	//HERO->StartSkillProgressBar( CastingTime );

	/// ���� �ൿ �ʱ�ȭ
	pHeroPet->SetNextAction(NULL);

	pSkillInfo->SendMsgToServer(
		pHeroPet,
		pTarget,
		consumeItem);
	pSkillInfo->SetCooling();

	// 080602 LUJ, �Ҹ�� �������� ��� ó���Ѵ�
	{
		CItem* item = ITEMMGR->GetItem( consumeItem.dwDBIdx );

		if( item )
		{
			item->SetLock( TRUE );
		}
	}

	if( ( ( pInfo->GetIndex() / 100000 ) % 10 ) == 2 )
	{
		pHeroPet->DisableAutoAttack();
	}
	
	mpPetSkillInfo = NULL;
	
	return TRUE;
}

BOOL cSkillManager::OnPetSkillCommand(CHeroPet* pHeroPet,CActionTarget* pTarget,BOOL bSkill)
{
	CBattle* pBattle = BATTLESYSTEM->GetBattle();

	if( pBattle && pBattle->GetBattleKind() == eBATTLE_KIND_GTOURNAMENT )
	{
		if( HERO->GetBattleTeam() == 2 )
			return FALSE;	
	}


	if( pHeroPet->GetState() == eObjectState_SkillSyn ||
		pHeroPet->GetState() == eObjectState_SkillUsing)
		SetNextPetComboSkill(pHeroPet,pTarget,bSkill);
	else
		ExcutePetSkillSYN(pHeroPet,pTarget,bSkill);		//return FALSE ó��....����.. ��������?

	return TRUE;
}

BOOL cSkillManager::OnPetSkillCommand( DWORD idx )
{
	CActionTarget Target;

	cSkillInfo* pSkillInfo = m_SkillInfoTable.GetData( idx );

	if( !pSkillInfo )
	{
		return FALSE;
	}
	else if(SKILLKIND_PASSIVE == pSkillInfo->GetKind() ||
		SKILLKIND_BUFF == pSkillInfo->GetKind())
	{
		return FALSE;
	}

	cActiveSkillInfo* pInfo = ( cActiveSkillInfo* )pSkillInfo;

	// ���� Ÿ�� ���θ� üũ�ؾ� �Ѵ�.

	switch( pInfo->GetSkillInfo()->Target )
	{
	case eSkillTargetKind_None:
		{
			return FALSE;
		}
		break;
	case eSkillTargetKind_OnlyOthers:
		{
			CObject * targetObj = OBJECTMGR->GetSelectedObject();

			if(0 == targetObj)
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(655) );
				return FALSE;
			}
			else if(targetObj->GetID() == gHeroID)
			{
				return FALSE;
			}

			if( targetObj->GetObjectKind() == eObjectKind_Player )
			{
				PKMGR->SetPKStartTimeReset();
			}

			if( pInfo->GetSkillInfo()->Unit == UNITKIND_RESURRECTION )
			{
				if( targetObj->GetObjectKind() != eObjectKind_Player)
				{
					return FALSE;
				}

				if( !targetObj->IsDied() || targetObj->GetState() != eObjectState_Die )
				{
					return FALSE;
				}

				Target.InitActionTarget(targetObj,NULL);
			}
			else
			{
				if( targetObj->IsDied() || targetObj->GetState() == eObjectState_Die )
				{
					return FALSE;
				}

				Target.InitActionTarget(targetObj, 0);
				HEROPET->EnableAutoAttack(&Target);
			}
		}
		break;
	case eSkillTargetKind_OnlySelf:
		{
			Target.InitActionTarget(HEROPET,NULL);	
		}
		break;
	case eSkillTargetKind_AllObject:
		{
			CObject * targetObj = OBJECTMGR->GetSelectedObject();

			if(0 == targetObj )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(655) );
				return FALSE;
			}
			else if( targetObj->IsDied() || targetObj->GetState() == eObjectState_Die )
			{
				return FALSE;
			}

			Target.InitActionTarget(targetObj,NULL);
			HEROPET->EnableAutoAttack(&Target);
		}
		break;
	}

	// 081021 LYW --- SkillManager : �� �� ��ų ���� ������ �߰� ó��.
	//mpSkillInfo = pSkillInfo;	
	mpPetSkillInfo = pSkillInfo;	

	if( idx )
	{
		return OnPetSkillCommand( HEROPET, &Target, TRUE );
	}
	else
	{
		return OnPetSkillCommand( HEROPET, &Target, FALSE );
	}
}


DWORD cSkillManager::GetPetComboSkillIdx(CHeroPet* pHeroPet)
{
	DWORD skillidx = 0;

	switch( pHeroPet->GetType() )
	{
	case ePetType_Basic:
	case ePetType_Physic:	skillidx = PETMGR->GetClosedSkillIndex();	break;
	case ePetType_Magic:
	case ePetType_Support:	skillidx = PETMGR->GetRangeSkillIndex();	break;
	}

	return skillidx;
}

void cSkillManager::SetNextPetComboSkill(CHeroPet* pHeroPet,CActionTarget* pTarget,BOOL bSkill)
{
	cSkillInfo* pNextSkill;
	DWORD NextSkillIdx;

	if(bSkill)
		// 081021 LYW --- SkillManager : �� �� ��ų ���� ������ �߰� ó��.
		//pNextSkill = mpSkillInfo;
		pNextSkill = mpPetSkillInfo;
	else
	{
		NextSkillIdx = GetPetComboSkillIdx(pHeroPet);
		pNextSkill = GetSkillInfo(NextSkillIdx);
	}

	if(pNextSkill == NULL)
		return;
	CAction act;
	act.InitSkillAction( ( cActiveSkillInfo* )pNextSkill, pTarget );
	pHeroPet->SetNextAction(&act);
}

// 081203 LUJ, ��ų �ε����� ��ų ��ũ��Ʈ�� ��ȯ�Ѵ�
const SkillScript& cSkillManager::GetSkillScript( DWORD index ) const
{
	return GAMERESRCMNGR->GetSkillScript( index );
}

void cSkillManager::RecallGuild(const ACTIVE_SKILL_INFO& info)
{
	CGuildDialog* const guildDialog = GAMEIN->GetGuildDlg();

	if(0 == guildDialog)
	{
		return;
	}
	else if(FALSE == guildDialog->IsActive())
	{
#ifdef _GMTOOL_
		CHATMGR->AddMsg(
			CTC_SYSMSG,
			"open guild dialog, select any member. check tip of used skill");
#endif
		return;
	}

	const GUILDMEMBERINFO* const member = guildDialog->GetSelectedMember();

	if(0 == member)
	{
		CHATMGR->AddMsg(
			CTC_SYSMSG,
			CHATMGR->GetChatMsg(1489));
		return;
	}
	else if(member->MemberIdx == gHeroID)
	{
		CHATMGR->AddMsg(
			CTC_SYSMSG,
			CHATMGR->GetChatMsg(1488));
		return;
	}
	else if(FALSE == member->bLogged)
	{
		CHATMGR->AddMsg(
			CTC_SYSMSG,
			CHATMGR->GetChatMsg(211));
		return;
	}

	CMoveManager::TemporaryRecallData recallData = {0};
	recallData.mSkillIndex = info.Index;
	recallData.mTargetPlayerIndex = member->MemberIdx;
	recallData.mMapType = member->mMapType;
	SafeStrCpy(
		recallData.mTargetPlayerName,
		member->MemberName,
		sizeof(recallData.mTargetPlayerName) / sizeof(*recallData.mTargetPlayerName));
	MOVEMGR->SetTemporaryRecall(recallData);

	WINDOWMGR->MsgBox(
		MBI_RECALL_REQUEST,
		MBT_YESNO,
		CHATMGR->GetChatMsg(1753),
		member->MemberName);
}

void cSkillManager::RecallParty(const ACTIVE_SKILL_INFO& info)
{
	CPartyDialog* const partyDialog = GAMEIN->GetPartyDialog();

	if(0 == partyDialog)
	{
		return;
	}
	else if(FALSE == partyDialog->IsActive())
	{
		CHATMGR->AddMsg(
			CTC_SYSMSG,
			"click member in party");
		return;
	}

	const PARTY_MEMBER& member = partyDialog->GetClickedMember();

	if(0 == member.dwMemberID)
	{
		CHATMGR->AddMsg(
			CTC_SYSMSG,
			CHATMGR->GetChatMsg(1489));
		return;
	}
	else if(member.dwMemberID == gHeroID)
	{
		CHATMGR->AddMsg(
			CTC_SYSMSG,
			CHATMGR->GetChatMsg(1488));
		return;
	}
	else if(PARTYMGR->GetMasterID() != gHeroID)
	{
		CHATMGR->AddMsg(
			CTC_SYSMSG,
			CHATMGR->GetChatMsg(1039));
		return;
	}
	else if(FALSE == member.bLogged)
	{
		CHATMGR->AddMsg(
			CTC_SYSMSG,
			CHATMGR->GetChatMsg(211));
		return;
	}

	CMoveManager::TemporaryRecallData recallData = {0};
	recallData.mSkillIndex = info.Index;
	recallData.mTargetPlayerIndex = member.dwMemberID;
	recallData.mMapType = member.mMapType;
	SafeStrCpy(
		recallData.mTargetPlayerName,
		member.Name,
		sizeof(recallData.mTargetPlayerName) / sizeof(*recallData.mTargetPlayerName));
	MOVEMGR->SetTemporaryRecall(recallData);

	WINDOWMGR->MsgBox(
		MBI_RECALL_REQUEST,
		MBT_YESNO,
		CHATMGR->GetChatMsg(1753),
		member.Name);
}


const char* cSkillManager::GetSkillEquipTypeText( const ACTIVE_SKILL_INFO* const pActiveSkillInfo, char* msg, size_t msgSize  )
{
	if( !pActiveSkillInfo || !msg )
		return NULL;

	char txt[MAX_PATH]={0,};

	if( pActiveSkillInfo->mWeaponType == eWeaponType_None &&
		pActiveSkillInfo->mWeaponAnimationType == eWeaponAnimationType_None &&
		pActiveSkillInfo->mArmorType == eArmorType_None )
	{
		return NULL;
	}

	switch( pActiveSkillInfo->mWeaponAnimationType )
	{
	case eWeaponAnimationType_OneHand:
		{
			switch( pActiveSkillInfo->mWeaponType )
			{
			case eWeaponType_Sword:
				{
					// �Ѽ� ��
					sprintf( txt, "%s %s", CHATMGR->GetChatMsg( 658 ), CHATMGR->GetChatMsg( 660 ) );
				}
				break;
			case eWeaponType_Mace:
				{
					// �Ѽ� ���̽�
					sprintf( txt, "%s %s", CHATMGR->GetChatMsg( 658 ), CHATMGR->GetChatMsg( 662 ) );
				}
				break;
			case eWeaponType_Axe:
				{
					// �Ѽ� ����
					sprintf( txt, "%s %s", CHATMGR->GetChatMsg( 658 ), CHATMGR->GetChatMsg( 661 ) );
				}
				break;
			default:
				{
					// �Ѽ� ����
					sprintf( txt, "%s %s", CHATMGR->GetChatMsg( 658 ), CHATMGR->GetChatMsg( 1961 ) );
				}
				break;
			}
		}
		break;
	case eWeaponAnimationType_TwoHand:
		{
			// ��չ���
			switch( pActiveSkillInfo->mWeaponType )
			{
			case eWeaponType_Sword:
				{
					// ��� ��
					sprintf( txt, "%s %s", CHATMGR->GetChatMsg( 659 ), CHATMGR->GetChatMsg( 660 ) );
				}
				break;
			case eWeaponType_Mace:
				{
					// ��� ���̽�
					sprintf( txt, "%s %s", CHATMGR->GetChatMsg( 659 ), CHATMGR->GetChatMsg( 662 ) );
				}
				break;
			case eWeaponType_Axe:
				{
					// ��� ����
					sprintf( txt, "%s %s", CHATMGR->GetChatMsg( 659 ), CHATMGR->GetChatMsg( 661 ) );
				}
				break;
			default:
				{
					// ��� ����
					sprintf( txt, "%s %s", CHATMGR->GetChatMsg( 659 ), CHATMGR->GetChatMsg( 1961 ) );
				}
				break;
			}
		}
		break;
	case eWeaponAnimationType_TwoBlade:
		{
			// �̵���
			sprintf( txt, "%s", CHATMGR->GetChatMsg( 2046 ) );
		}
		break;

	case eWeaponAnimationType_Spear:
		{
			// â
			sprintf( txt, "%s", CHATMGR->GetChatMsg( 2047 ) );
		}
		break;

	default:
		{
			switch( pActiveSkillInfo->mWeaponType )
			{
			case eWeaponType_Bow:
				{
					// Ȱ
					sprintf( txt, "%s", CHATMGR->GetChatMsg( 664 ) );
				}
				break;
			case eWeaponType_Gun:
				{
					// ��
					sprintf( txt, "%s", CHATMGR->GetChatMsg( 2048 ) );
				}
				break;
			case eWeaponType_Dagger:
				{
					// �ܰ�
					sprintf( txt, "%s", CHATMGR->GetChatMsg( 663 ) );
				}
				break;
			case eWeaponType_Staff:
				{
					// ������
					sprintf( txt, "%s", CHATMGR->GetChatMsg( 665 ) );
				}
				break;
			case eWeaponType_Sword:
				{
					// ��
					sprintf( txt, "%s", CHATMGR->GetChatMsg( 660 ) );
				}
				break;
			case eWeaponType_Mace:
				{
					// ���̽�
					sprintf( txt, "%s", CHATMGR->GetChatMsg( 662 ) );
				}
				break;
			case eWeaponType_Axe:
				{
					// ����
					sprintf( txt, "%s", CHATMGR->GetChatMsg( 661 ) );
				}
				break;
			case eWeaponType_BothDagger:
				{
					// ��� ����
					sprintf( txt, "%s", CHATMGR->GetChatMsg( 1924 ) );
				}
				break;
			}

			switch( pActiveSkillInfo->mArmorType )
			{
			case eArmorType_Shield:
				{
					// ����
					sprintf( txt, "%s", CHATMGR->GetChatMsg( 695 ) );
				}
				break;
			case eArmorType_Robe:
				{
					// �κ�
					sprintf( txt, "%s", CHATMGR->GetChatMsg( 692 ) );
				}
				break;
			case eArmorType_Leather:
				{
					// ����Ʈ�Ƹ�
					sprintf( txt, "%s", CHATMGR->GetChatMsg( 693 ) );
				}
				break;
			case eArmorType_Metal:
				{
					// ���Ƹ�
					sprintf( txt, "%s", CHATMGR->GetChatMsg( 694 ) );
				}
				break;
			}
		}
		break;
	}

	SafeStrCpy( msg, txt, msgSize );
	return msg;
}

const char* cSkillManager::GetSkillWeaponTypeText( const BUFF_SKILL_INFO* const pBuffSkillInfo, char* msg, size_t msgSize  )
{
	if( !pBuffSkillInfo || !msg )
		return NULL;

	char txt[MAX_PATH]={0,};

	if( pBuffSkillInfo->mWeaponType == eWeaponType_None &&
		pBuffSkillInfo->mWeaponAnimationType == eWeaponAnimationType_None )
	{
		return NULL;
	}

	switch( pBuffSkillInfo->mWeaponAnimationType )
	{
	case eWeaponAnimationType_Spear:
		{
			// â
			sprintf( txt, "%s", CHATMGR->GetChatMsg( 2047 ) );
		}
		break;
	case eWeaponAnimationType_TwoHand:
		{
			// ��� ����
			sprintf( txt, "%s %s", CHATMGR->GetChatMsg( 659 ), CHATMGR->GetChatMsg( 1961 ) );
		}
		break;
	case eWeaponAnimationType_OneHand:
		{
			// �Ѽ� ����
			sprintf( txt, "%s %s", CHATMGR->GetChatMsg( 658 ), CHATMGR->GetChatMsg( 1961 ) );
		}
		break;
	case eWeaponAnimationType_TwoBlade:
		{
			// �̵���
			sprintf( txt, "%s", CHATMGR->GetChatMsg( 2046 ) );
		}
		break;

	default:
		{
			switch( pBuffSkillInfo->mWeaponType )
			{
			case eWeaponType_Sword:
				{
					// ��
					sprintf( txt, "%s", CHATMGR->GetChatMsg( 660 ) );
				}
				break;
			case eWeaponType_Mace:
				{
					// ���̽�
					sprintf( txt, "%s", CHATMGR->GetChatMsg( 662 ) );
				}
				break;
			case eWeaponType_Axe:
				{
					// ����
					sprintf( txt, "%s", CHATMGR->GetChatMsg( 661 ) );
				}
				break;
			case eWeaponType_Staff:
				{
					// ������
					sprintf( txt, "%s", CHATMGR->GetChatMsg( 665 ) );
				}
				break;
			case eWeaponType_Bow:
				{
					// Ȱ
					sprintf( txt, "%s", CHATMGR->GetChatMsg( 664 ) );
				}
				break;
			case eWeaponType_Gun:
				{
					// ��
					sprintf( txt, "%s", CHATMGR->GetChatMsg( 2048 ) );
				}
				break;
			case eWeaponType_Dagger:
				{
					// �ܰ�
					sprintf( txt, "%s", CHATMGR->GetChatMsg( 663 ) );
				}
				break;
			}
		}
		break;
	}

    SafeStrCpy( msg, txt, msgSize );
	return msg;
}

const char* cSkillManager::GetSkillArmorTypeText( const BUFF_SKILL_INFO* const pBuffSkillInfo, char* msg, size_t msgSize  )
{
	if( !pBuffSkillInfo || !msg )
		return NULL;

	char txt[MAX_PATH]={0,};

	if( pBuffSkillInfo->mArmorType == eArmorType_None )
	{
		return NULL;
	}

	switch( pBuffSkillInfo->mArmorType )
	{
	case eArmorType_Robe:
		{
			// �κ�
			sprintf( txt, "%s", CHATMGR->GetChatMsg( 692 ) );
		}
		break;
	case eArmorType_Leather:
		{
			// ����Ʈ�Ƹ�
			sprintf( txt, "%s", CHATMGR->GetChatMsg( 693 ) );
		}
		break;
	case eArmorType_Metal:
		{
			// ���Ƹ�
			sprintf( txt, "%s", CHATMGR->GetChatMsg( 694 ) );
		}
		break;
	case eArmorType_Shield:
		{
			// ����
			sprintf( txt, "%s", CHATMGR->GetChatMsg( 695 ) );
		}
		break;
	default:
		{
			// ����
			sprintf( txt, "%s", CHATMGR->GetChatMsg( 151 ) );
		}
		break;
	}

	SafeStrCpy( msg, txt, msgSize );
	return msg;
}

const char* cSkillManager::GetConsumePointTypeText( const ACTIVE_SKILL_INFO* const pActiveSkillInfo, char* msg, size_t msgSize  )
{
	if( !pActiveSkillInfo || !msg )
		return NULL;

	char txt[MAX_PATH]={0,};

	if( pActiveSkillInfo->mConsumePoint.mType == ACTIVE_SKILL_INFO::ConsumePoint::TypeNone )
	{
		return NULL;
	}

	switch( pActiveSkillInfo->mConsumePoint.mType )
	{
	case ACTIVE_SKILL_INFO::ConsumePoint::TypeGuild:
		{
			sprintf( txt, "%s%d", CHATMGR->GetChatMsg( 2096 ), pActiveSkillInfo->mConsumePoint.mValue );		// "�Һ� �������Ʈ: "
		}
		break;
	case ACTIVE_SKILL_INFO::ConsumePoint::TypeFamily:
		{
			sprintf( txt, "%s%d", CHATMGR->GetChatMsg( 2097 ), pActiveSkillInfo->mConsumePoint.mValue );		// "�Һ� �йи�����Ʈ: "
		}
		break;
	case ACTIVE_SKILL_INFO::ConsumePoint::TypePlayerKill:
		{
			sprintf( txt, "%s%d", CHATMGR->GetChatMsg( 2098 ), pActiveSkillInfo->mConsumePoint.mValue );		// "�Һ� PK����Ʈ: "
		}
		break;
	case ACTIVE_SKILL_INFO::ConsumePoint::TypeExperience:
		{
			sprintf( txt, "%s%d", CHATMGR->GetChatMsg( 2099 ), pActiveSkillInfo->mConsumePoint.mValue );		// "�Һ� ����ġ: "
		}
		break;
	}

	SafeStrCpy( msg, txt, msgSize );
	return msg;
}

const char* cSkillManager::GetBuffConditionTypeText( const BUFF_SKILL_INFO* const pBuffSkillInfo, char* msg, size_t msgSize  )
{
	if( !pBuffSkillInfo, !msg )
		return NULL;

	char txt[MAX_PATH]={0,};

	struct
	{
		const char* operator() ( BUFF_SKILL_INFO::Condition::Operation eOper )
		{
			switch( eOper )
			{
			case BUFF_SKILL_INFO::Condition::OperationMoreEqual:	return CHATMGR->GetChatMsg( 2056 );	// �̻�
			case BUFF_SKILL_INFO::Condition::OperationLessEqual:	return CHATMGR->GetChatMsg( 2057 );	// ����
			case BUFF_SKILL_INFO::Condition::OperationMore:			return CHATMGR->GetChatMsg( 2058 );	// �ʰ�
			case BUFF_SKILL_INFO::Condition::OperationLess:			return CHATMGR->GetChatMsg( 2059 );	// �̸�
			default:		return "";
			}
		}
	}
	GetConditionOperationTxt;

	if( pBuffSkillInfo->mCondition.mType == BUFF_SKILL_INFO::Condition::TypeNone )
	{
		return NULL;
	}

	switch( pBuffSkillInfo->mCondition.mType )
	{
	case BUFF_SKILL_INFO::Condition::TypeLifePercent:
		{
			sprintf( txt, "%s %.0f%% %s", CHATMGR->GetChatMsg( 164 ), pBuffSkillInfo->mCondition.mValue, GetConditionOperationTxt( pBuffSkillInfo->mCondition.mOperation ) );
		}
		break;
	case BUFF_SKILL_INFO::Condition::TypeLife:
		{
			sprintf( txt, "%s %.0f %s", CHATMGR->GetChatMsg( 164 ), pBuffSkillInfo->mCondition.mValue, GetConditionOperationTxt( pBuffSkillInfo->mCondition.mOperation ) );
		}
		break;
	case BUFF_SKILL_INFO::Condition::TypeLifeMax:
		{
			sprintf( txt, "%s %.0f%% %s", CHATMGR->GetChatMsg( 2060 ), pBuffSkillInfo->mCondition.mValue, GetConditionOperationTxt( pBuffSkillInfo->mCondition.mOperation ) );
		}
		break;
	case BUFF_SKILL_INFO::Condition::TypeLifeRecovery:
		{
			sprintf( txt, "%s %.0f %s", CHATMGR->GetChatMsg( 846 ), pBuffSkillInfo->mCondition.mValue, GetConditionOperationTxt( pBuffSkillInfo->mCondition.mOperation ) );
		}
		break;
	case BUFF_SKILL_INFO::Condition::TypeManaPercent:
		{
			sprintf( txt, "%s %.0f%% %s", CHATMGR->GetChatMsg( 165 ), pBuffSkillInfo->mCondition.mValue, GetConditionOperationTxt( pBuffSkillInfo->mCondition.mOperation ) );
		}
		break;
	case BUFF_SKILL_INFO::Condition::TypeMana:
		{
			sprintf( txt, "%s %.0f %s", CHATMGR->GetChatMsg( 165 ), pBuffSkillInfo->mCondition.mValue, GetConditionOperationTxt( pBuffSkillInfo->mCondition.mOperation ) );
		}
		break;
	case BUFF_SKILL_INFO::Condition::TypeManaMax:
		{
			sprintf( txt, "%s %.0f %s", CHATMGR->GetChatMsg( 2061 ), pBuffSkillInfo->mCondition.mValue, GetConditionOperationTxt( pBuffSkillInfo->mCondition.mOperation ) );
		}
		break;
	case BUFF_SKILL_INFO::Condition::TypeManaRecovery:
		{
			sprintf( txt, "%s %.0f %s", CHATMGR->GetChatMsg( 847 ), pBuffSkillInfo->mCondition.mValue, GetConditionOperationTxt( pBuffSkillInfo->mCondition.mOperation ) );
		}
		break;
	case BUFF_SKILL_INFO::Condition::TypeStrength:
		{
			sprintf( txt, "%s %.0f %s", CHATMGR->GetChatMsg( 157 ), pBuffSkillInfo->mCondition.mValue, GetConditionOperationTxt( pBuffSkillInfo->mCondition.mOperation ) );
		}
		break;
	case BUFF_SKILL_INFO::Condition::TypeIntelligence:
		{
			sprintf( txt, "%s %.0f %s", CHATMGR->GetChatMsg( 160 ), pBuffSkillInfo->mCondition.mValue, GetConditionOperationTxt( pBuffSkillInfo->mCondition.mOperation ) );
		}
		break;
	case BUFF_SKILL_INFO::Condition::TypeDexterity:
		{
			sprintf( txt, "%s %.0f %s", CHATMGR->GetChatMsg( 158 ), pBuffSkillInfo->mCondition.mValue, GetConditionOperationTxt( pBuffSkillInfo->mCondition.mOperation ) );
		}
		break;
	case BUFF_SKILL_INFO::Condition::TypeWisdom:
		{
			sprintf( txt, "%s %.0f %s", CHATMGR->GetChatMsg( 161 ), pBuffSkillInfo->mCondition.mValue, GetConditionOperationTxt( pBuffSkillInfo->mCondition.mOperation ) );
		}
		break;
	case BUFF_SKILL_INFO::Condition::TypeVitality:
		{
			sprintf( txt, "%s %.0f %s", CHATMGR->GetChatMsg( 159 ), pBuffSkillInfo->mCondition.mValue, GetConditionOperationTxt( pBuffSkillInfo->mCondition.mOperation ) );
		}
		break;
	case BUFF_SKILL_INFO::Condition::TypePhysicalAttack:
		{
			sprintf( txt, "%s %.0f %s", CHATMGR->GetChatMsg( 152 ), pBuffSkillInfo->mCondition.mValue, GetConditionOperationTxt( pBuffSkillInfo->mCondition.mOperation ) );
		}
		break;
	case BUFF_SKILL_INFO::Condition::TypePhysicalDefense:
		{
			sprintf( txt, "%s %.0f %s", CHATMGR->GetChatMsg( 153 ), pBuffSkillInfo->mCondition.mValue, GetConditionOperationTxt( pBuffSkillInfo->mCondition.mOperation ) );
		}
		break;
	case BUFF_SKILL_INFO::Condition::TypeMagicalAttack:
		{
			sprintf( txt, "%s %.0f %s", CHATMGR->GetChatMsg( 155 ), pBuffSkillInfo->mCondition.mValue, GetConditionOperationTxt( pBuffSkillInfo->mCondition.mOperation ) );
		}
		break;
	case BUFF_SKILL_INFO::Condition::TypeMagicalDefense:
		{
			sprintf( txt, "%s %.0f %s", CHATMGR->GetChatMsg( 156 ), pBuffSkillInfo->mCondition.mValue, GetConditionOperationTxt( pBuffSkillInfo->mCondition.mOperation ) );
		}
		break;
	case BUFF_SKILL_INFO::Condition::TypeCriticalRate:
		{
			sprintf( txt, "%s %.0f %s", CHATMGR->GetChatMsg( 842 ), pBuffSkillInfo->mCondition.mValue, GetConditionOperationTxt( pBuffSkillInfo->mCondition.mOperation ) );
		}
		break;
	case BUFF_SKILL_INFO::Condition::TypeCriticalDamage:
		{
			sprintf( txt, "%s %.0f %s", CHATMGR->GetChatMsg( 843 ), pBuffSkillInfo->mCondition.mValue, GetConditionOperationTxt( pBuffSkillInfo->mCondition.mOperation ) );
		}
		break;
	case BUFF_SKILL_INFO::Condition::TypeAccuracy:
		{
			sprintf( txt, "%s %.0f %s", CHATMGR->GetChatMsg( 162 ), pBuffSkillInfo->mCondition.mValue, GetConditionOperationTxt( pBuffSkillInfo->mCondition.mOperation ) );
		}
		break;
	case BUFF_SKILL_INFO::Condition::TypeEvade:
		{
			sprintf( txt, "%s %.0f %s", CHATMGR->GetChatMsg( 841 ), pBuffSkillInfo->mCondition.mValue, GetConditionOperationTxt( pBuffSkillInfo->mCondition.mOperation ) );
		}
		break;
	case BUFF_SKILL_INFO::Condition::TypeMoveSpeed:
		{
			sprintf( txt, "%s %.0f %s", CHATMGR->GetChatMsg( 840 ), pBuffSkillInfo->mCondition.mValue, GetConditionOperationTxt( pBuffSkillInfo->mCondition.mOperation ) );
		}
		break;
	}

	SafeStrCpy( msg, txt, msgSize );
	return msg;
}

const char* cSkillManager::GetBuffCountTypeText( const BUFF_SKILL_INFO* const pBuffSkillInfo, char* msg, size_t msgSize , const int nCurrentCount )
{
	if( !pBuffSkillInfo || !msg )
		return NULL;

	char txt[MAX_PATH]={0,};

	if( pBuffSkillInfo->CountType == eBuffSkillCountType_None )
	{
		return NULL;
	}

	int nCount = ( nCurrentCount != 0 ? nCurrentCount : pBuffSkillInfo->Count );

	switch( pBuffSkillInfo->CountType )
	{
	case eBuffSkillCountType_Dead:
		{
			sprintf( txt, "%s: %d", CHATMGR->GetChatMsg( 2077 ), nCount );
		}
		break;
	case eBuffSkillCountType_AnyDamageVolumn:
		{
			sprintf( txt, "%s: %d", CHATMGR->GetChatMsg( 2078 ), nCount );
		}
		break;
	case eBuffSkillCountType_AnyDamage:
		{
			sprintf( txt, "%s: %d", CHATMGR->GetChatMsg( 2079 ), nCount );
		}
		break;
	case eBuffSkillCountType_AnyHitVolumn:
		{
			sprintf( txt, "%s: %d", CHATMGR->GetChatMsg( 2080 ), nCount );
		}
		break;
	case eBuffSkillCountType_AnyHit:
		{
			sprintf( txt, "%s: %d", CHATMGR->GetChatMsg( 2081 ), nCount );
		}
		break;
	case eBuffSkillCountType_PhysicalDamageVolumn:
		{
			sprintf( txt, "%s: %d", CHATMGR->GetChatMsg( 2082 ), nCount );
		}
		break;
	case eBuffSkillCountType_PhysicalDamage:
		{
			sprintf( txt, "%s: %d", CHATMGR->GetChatMsg( 2083 ), nCount );
		}
		break;
	case eBuffSkillCountType_PhysicalHitVolumn:
		{
			sprintf( txt, "%s: %d", CHATMGR->GetChatMsg( 2084 ), nCount );
		}
		break;
	case eBuffSkillCountType_PhysicalHit:
		{
			sprintf( txt, "%s: %d", CHATMGR->GetChatMsg( 2085 ), nCount );
		}
		break;
	case eBuffSkillCountType_MagicalDamageVolumn:
		{
			sprintf( txt, "%s: %d", CHATMGR->GetChatMsg( 2086 ), nCount );
		}
		break;
	case eBuffSkillCountType_MagicalDamage:
		{
			sprintf( txt, "%s: %d", CHATMGR->GetChatMsg( 2087 ), nCount );
		}
		break;
	case eBuffSkillCountType_MagicalHitVolumn:
		{
			sprintf( txt, "%s: %d", CHATMGR->GetChatMsg( 2088 ), nCount );
		}
		break;
	case eBuffSkillCountType_MagicalHit:
		{
			sprintf( txt, "%s: %d", CHATMGR->GetChatMsg( 2089 ), nCount );
		}
		break;
	case eBuffSkillCountType_HealVolumn:
		{
			sprintf( txt, "%s: %d", CHATMGR->GetChatMsg( 2090 ), pBuffSkillInfo->Count );
		}
		break;
	case eBuffSkillCountType_Heal:
		{
			sprintf( txt, "%s: %d", CHATMGR->GetChatMsg( 2091 ), pBuffSkillInfo->Count );
		}
		break;
	case eBuffSkillCountType_BeHealedVolumn:
		{
			sprintf( txt, "%s: %d", CHATMGR->GetChatMsg( 2092 ), pBuffSkillInfo->Count );
		}
		break;
	case eBuffSkillCountType_BeHealed:
		{
			sprintf( txt, "%s: %d", CHATMGR->GetChatMsg( 2093 ), pBuffSkillInfo->Count );
		}
		break;
	}

	SafeStrCpy( msg, txt, msgSize );
	return msg;
}

// �������ڿ����̺� ������ �ʱ�ȭ�Ѵ�.
void cSkillManager::InitToolTipMsgKeyWordTable()
{
	const int dwTableSize = sizeof( ToolTipMsgKeyWordTable ) / sizeof( stToolTipMsgKeyWordData );

	for( int i=0 ; i<dwTableSize ; ++i )
	{
		ToolTipMsgKeyWordTable[i].dwHashCode = GetHashCodeFromTxt( ToolTipMsgKeyWordTable[i].KeyWord );

#ifdef _GMTOOL_
		// ���� ����� ���
		if( m_mapToolTipMsgKeyWord.find( ToolTipMsgKeyWordTable[i].dwHashCode ) != m_mapToolTipMsgKeyWord.end() )
		{
			char txt[MAX_PATH]={0,};
			sprintf( txt, "Tooltip �����[%s]�� �ߺ� �Ǿ� �ֽ��ϴ�", ToolTipMsgKeyWordTable[i].KeyWord );
			MessageBox( NULL, txt, "Warning", MB_OK );
		}
#endif

		m_mapToolTipMsgKeyWord[ ToolTipMsgKeyWordTable[i].dwHashCode ] = (int)ToolTipMsgKeyWordTable[i].eType;
	}
}

// ���ڿ����� �������� ã�´�.
BOOL cSkillManager::GetToolTipMsgKeyWordFromText( std::string& strMsg, std::string& keyword, std::string::size_type& keywordStart, std::string::size_type& keywordSize )
{
	std::string substr;
	keywordSize = 0;
	while( ++keywordSize + keywordStart < strMsg.length() )
	{
		if( strMsg[ keywordSize + keywordStart ] == ' ' ||
			strMsg[ keywordSize + keywordStart ] == '\n' ||
			strMsg[ keywordSize + keywordStart ] == '\t' )
		{
			keywordSize = 0;
			return FALSE;
		}

		if( strMsg[ keywordSize + keywordStart ] == '$' )
		{
			keyword = strMsg.substr( keywordStart, ++keywordSize );
			return TRUE;
		}
	}

	return FALSE;
}

// ���������Type���κ��� ��ȯ�� ���ڿ��� ��ȯ�Ѵ�.
const char* cSkillManager::GetTextFromToolTipMsgKeyWordType( DWORD& dwToolTipIndex, int keywordType, char* txt, size_t txtSize, int prevKeyWordType )
{
	if( !txt )
		return NULL;

	ZeroMemory( txt, txtSize );

	// -> Ÿ�� ��ȯ
	if( (eToolTipMsgKeyWordType)prevKeyWordType != eToolTipMsgKeyWordType_None )
	{
		cSkillInfo* pSkillInfo = SKILLMGR->GetSkillInfo( dwToolTipIndex );
		if( !pSkillInfo )
			return "";

		switch( (eToolTipMsgKeyWordType)prevKeyWordType )
		{
		case eToolTipMsgKeyWordType_ActiveSkill_Buff1:
		case eToolTipMsgKeyWordType_ActiveSkill_Buff2:
		case eToolTipMsgKeyWordType_ActiveSkill_Buff3:
		case eToolTipMsgKeyWordType_ActiveSkill_Buff4:
		case eToolTipMsgKeyWordType_ActiveSkill_Buff5:
			{
				const int nArrayIndex = prevKeyWordType - (int)eToolTipMsgKeyWordType_ActiveSkill_Buff1;
				if( nArrayIndex >= MAX_BUFF_COUNT )
					return "";

				const DWORD dwBuffSkillIndex = ((cActiveSkillInfo*)pSkillInfo)->GetBuffSkillIdx( nArrayIndex );
				dwToolTipIndex = dwBuffSkillIndex;

				return GetTextFromToolTipMsgKeyWordType( dwToolTipIndex, keywordType, txt, txtSize );
			}

		case eToolTipMsgKeyWordType_ActiveSkill_ConsumeItemIndex:
			{
				ACTIVE_SKILL_INFO* pActiveSkillInfo = ((cActiveSkillInfo*)pSkillInfo)->GetSkillInfo();
				if( !pActiveSkillInfo )
					return "";

				dwToolTipIndex = pActiveSkillInfo->mConsumeItem.wIconIdx;
				return GetTextFromToolTipMsgKeyWordType( dwToolTipIndex, keywordType, txt, txtSize );
			}

		case eToolTipMsgKeyWordType_ActiveSkill_RequiredBuff:
			{
				ACTIVE_SKILL_INFO* pActiveSkillInfo = ((cActiveSkillInfo*)pSkillInfo)->GetSkillInfo();
				if( !pActiveSkillInfo )
					return "";

				dwToolTipIndex = pActiveSkillInfo->RequiredBuff;
				return GetTextFromToolTipMsgKeyWordType( dwToolTipIndex, keywordType, txt, txtSize );
			}

		case eToolTipMsgKeyWordType_BuffSkill_EventSkillIndex:
			{
				BUFF_SKILL_INFO* pBuffSkillInfo = ((cBuffSkillInfo*)pSkillInfo)->GetSkillInfo();
				if( !pBuffSkillInfo ||
					pBuffSkillInfo->Status != eStatusKind_EventSkillToVictim ||
					pBuffSkillInfo->Status != eStatusKind_EventSkillToKiller )
					return "";

				dwToolTipIndex  = pBuffSkillInfo->mEventSkillIndex;
				return GetTextFromToolTipMsgKeyWordType( dwToolTipIndex, keywordType, txt, txtSize );
			}

		case eToolTipMsgKeyWordType_BuffSkill_Item1:
		case eToolTipMsgKeyWordType_BuffSkill_Item2:
			{
				BUFF_SKILL_INFO* pBuffSkillInfo = ((cBuffSkillInfo*)pSkillInfo)->GetSkillInfo();
				if( !pBuffSkillInfo )
					return "";

				const int nMaxArraySize = sizeof( pBuffSkillInfo->Item ) / sizeof( *pBuffSkillInfo->Item );
				const int nArrayIndex = prevKeyWordType - (int)eToolTipMsgKeyWordType_BuffSkill_Item1;
				if( nArrayIndex >= nMaxArraySize )
					return "";

				dwToolTipIndex = pBuffSkillInfo->Item[ nArrayIndex ];
				return GetTextFromToolTipMsgKeyWordType( dwToolTipIndex, keywordType, txt, txtSize );
			}

		default:
			return "";
		}
	}


	struct{
		void operator() ( char* txt,  float fTotalSec )
		{
			if( !txt )
				return;

			char time[MAX_PATH]={0,};

			// ��/��/�ʷ� ǥ��
			int hour=0, min=0;
			float sec = fTotalSec;
			if( sec >= 60.f )
			{
				min = (int)(sec / 60.f);
				sec = sec - min * 60.f;
				if( min >= 60.f )
				{
					hour = (int)(min / 60.f);
					min = min - hour * 60;
				}
			}

			if( hour )
			{
				sprintf( time, CHATMGR->GetChatMsg( 1410 ), hour );
				strcat( txt, time );
			}
			if( min )
			{
				sprintf( time, CHATMGR->GetChatMsg( 1411 ), min );
				strcat( txt, time );
			}
			if( sec )
			{
				sprintf( time, CHATMGR->GetChatMsg( 2053 ), sec );
				strcat( txt, time );
			}
		}
	}ConvertHourMinSec;

	
	// struct SKILL_INFO �� ����
	if( keywordType < eToolTipMsgKeyWordType_ActiveSkill )
	{
		cSkillInfo* pSkillInfo = SKILLMGR->GetSkillInfo( dwToolTipIndex );
		if( !pSkillInfo )
			return "";

		switch( (eToolTipMsgKeyWordType)keywordType )
		{
		case eToolTipMsgKeyWordType_Skill_Index:
			{
				sprintf( txt, "%d", pSkillInfo->GetIndex() );
			}
			break;
		case eToolTipMsgKeyWordType_Skill_Name:
			{
				return pSkillInfo->GetName();
			}
			break;
		case eToolTipMsgKeyWordType_Skill_Tooltip:
			{
				sprintf( txt, "%d", pSkillInfo->GetTooltip() );
			}
			break;
		case eToolTipMsgKeyWordType_Skill_Image:
			{
				sprintf( txt, "%d", pSkillInfo->GetImage() );
			}
			break;
		case eToolTipMsgKeyWordType_Skill_Kind:
			{
				switch( pSkillInfo->GetKind() )
				{
				case SKILLKIND_PHYSIC:		return CHATMGR->GetChatMsg( 2100 );		// "��Ƽ��(����)"
				case SKILLKIND_MAGIC:		return CHATMGR->GetChatMsg( 2101 );		// "��Ƽ��(����)"
				case SKILLKIND_PASSIVE:		return CHATMGR->GetChatMsg( 176 );
				case SKILLKIND_ONOFF:		return CHATMGR->GetChatMsg( 2102 );		// "���"
				case SKILLKIND_BUFF:		return CHATMGR->GetChatMsg( 2103 );		// "����"
				}
			}
			break;
		case eToolTipMsgKeyWordType_Skill_Level:
			{
				sprintf( txt, "%d", pSkillInfo->GetLevel() );
			}
			break;
		}
	}
	// struct ACTIVE_SKILL_INFO �� ����
	else if( keywordType < eToolTipMsgKeyWordType_BuffSkill )
	{
		cSkillInfo* pSkillInfo = SKILLMGR->GetSkillInfo( dwToolTipIndex );
		if( !pSkillInfo )
			return "";

		ACTIVE_SKILL_INFO* pActiveSkillInfo = ((cActiveSkillInfo*)pSkillInfo)->GetSkillInfo();
		if( !pActiveSkillInfo )
			return "";

		switch( (eToolTipMsgKeyWordType)keywordType )
		{
		case eToolTipMsgKeyWordType_ActiveSkill_Money:
			{
				return AddComma( pActiveSkillInfo->Money );
			}
			break;
		case eToolTipMsgKeyWordType_ActiveSkill_Life_Percent:
			{
				sprintf( txt, "%.0f%%", pActiveSkillInfo->mLife.mPercent );
			}
			break;
		case eToolTipMsgKeyWordType_ActiveSkill_Life_Plus:
			{
				sprintf( txt, "%d", (int)pActiveSkillInfo->mLife.mPlus );
			}
			break;
		case eToolTipMsgKeyWordType_ActiveSkill_Mana_Percent:
			{
				sprintf( txt, "%.0f%%", pActiveSkillInfo->mMana.mPercent );
			}
			break;
		case eToolTipMsgKeyWordType_ActiveSkill_Mana_Plus:
			{
				sprintf( txt, "%d", (int)pActiveSkillInfo->mMana.mPlus );
			}
			break;
		case eToolTipMsgKeyWordType_ActiveSkill_EquipType:
			{
				if( GetSkillEquipTypeText( pActiveSkillInfo, txt, txtSize ) == NULL )
					return CHATMGR->GetChatMsg( 151 );		// "����"

				return GetSkillEquipTypeText( pActiveSkillInfo, txt, txtSize );
			}
			break;
		case eToolTipMsgKeyWordType_ActiveSkill_OperatorEffect:
			{
				sprintf( txt, "%d", pActiveSkillInfo->OperatorEffect );
			}
			break;
		case eToolTipMsgKeyWordType_ActiveSkill_TargetEffect:
			{
				sprintf( txt, "%d", pActiveSkillInfo->TargetEffect );
			}
			break;
		case eToolTipMsgKeyWordType_ActiveSkill_SkillEffect:
			{
				sprintf( txt, "%d", pActiveSkillInfo->SkillEffect );
			}
			break;
		case eToolTipMsgKeyWordType_ActiveSkill_AnimationTime:
			{
				// �ð�(1/1000��)
				ConvertHourMinSec( txt, pActiveSkillInfo->AnimationTime / 1000.f );
			}
			break;
		case eToolTipMsgKeyWordType_ActiveSkill_CastingTime:
			{
				// �ð�(1/1000��)
				ConvertHourMinSec( txt, pActiveSkillInfo->CastingTime / 1000.f );
			}
			break;
		case eToolTipMsgKeyWordType_ActiveSkill_DelayType:
			{
				switch( pActiveSkillInfo->DelayType )
				{
				case 1:		return CHATMGR->GetChatMsg( 2105 );		// "���ڸ�"
				case 2:		return CHATMGR->GetChatMsg( 2106 );		// "��ǥ�̵�"
				}
			}
			break;
		case eToolTipMsgKeyWordType_ActiveSkill_DelayTime:
			{
				// �ð�(1/1000��)
				ConvertHourMinSec( txt, pActiveSkillInfo->DelayTime / 1000.f );
			}
			break;
		case eToolTipMsgKeyWordType_ActiveSkill_CoolTime:
			{
				// �ð�(1/1000��)
				ConvertHourMinSec( txt, pActiveSkillInfo->CoolTime / 1000.f );
			}
			break;
		case eToolTipMsgKeyWordType_ActiveSkill_Target:
			{
				switch( pActiveSkillInfo->Target )
				{
				case 0:		return CHATMGR->GetChatMsg( 2107 );		// "Ÿ�پ���"
				case 1:		return CHATMGR->GetChatMsg( 2108 );		// "Ÿ���ʿ�"
				case 2:		return CHATMGR->GetChatMsg( 2109 );		// "����Ÿ��"
				case 3:		return CHATMGR->GetChatMsg( 2110 );		// "����Ÿ��"
				case 4:		return CHATMGR->GetChatMsg( 2111 );		// "����&Ÿ��"
				}
			}
			break;
		case eToolTipMsgKeyWordType_ActiveSkill_Range:
			{
				sprintf( txt, "%.1fm", (pActiveSkillInfo->Range / 100.f) );
			}
			break;
		case eToolTipMsgKeyWordType_ActiveSkill_Area:
			{
				switch( pActiveSkillInfo->Area )
				{
				case TARGET_KIND_SINGLE:
				case TARGET_KIND_SINGLE_CIRCLE:
					return CHATMGR->GetChatMsg( 2112 );		// "���ϴ��"

				case TARGET_KIND_MULTI:
				case TARGET_KIND_MULTI_CIRCLE:
				case TARGET_KIND_MULTI_RECT:
				case TARGET_KIND_MULTI_ARC:
					return CHATMGR->GetChatMsg( 2113 );		// "���������"

				case TARGET_KIND_MULTI_MONSTER:
					return CHATMGR->GetChatMsg( 2114 );		// "���� ��� ����"

				case TARGET_KIND_MULTI_ALL:
					return CHATMGR->GetChatMsg( 2115 );		// "����/�÷��̾� ��� ����"
				}
			}
			break;
		case eToolTipMsgKeyWordType_ActiveSkill_AreaTarget:
			{
				switch( pActiveSkillInfo->AreaTarget )
				{
				case eSkillAreaTargetNone:				return CHATMGR->GetChatMsg( 151 );		// "����"
				case eSkillAreaTargetMonster:			return CHATMGR->GetChatMsg( 2116 );		// "����(��)"
				case eSkillAreaTargetParty:				return CHATMGR->GetChatMsg( 2117 );		// "��Ƽ��"
				case eSkillAreaTargetFamily:			return CHATMGR->GetChatMsg( 2118 );		// "�йи���"
				case eSkillAreaTargetGuild:				return CHATMGR->GetChatMsg( 2119 );		// "����"
				case eSkillAreaTargetAll:				return CHATMGR->GetChatMsg( 2120 );		// "�ڽ�����"
				case eSkillAreaTargetAllExceptMe:		return CHATMGR->GetChatMsg( 2121 );		// "�ڽ����� ��� ����"
				}
			}
			break;
		case eToolTipMsgKeyWordType_ActiveSkill_AreaData:
			{
				sprintf( txt, "%.1fm", (pActiveSkillInfo->AreaData / 100.f) );
			}
			break;
		case eToolTipMsgKeyWordType_ActiveSkill_Unit:
			{
				switch( pActiveSkillInfo->Unit )
				{
				case UNITKIND_NONE:						return CHATMGR->GetChatMsg( 151 );		// "����"
				case UNITKIND_PHYSIC_ATTCK:				return CHATMGR->GetChatMsg( 2122 );		// "����������"
				case UNITKIND_MAGIC_ATTCK:				return CHATMGR->GetChatMsg( 2123 );		// "����������"
				case UNITKIND_LIFE_RECOVER:				return CHATMGR->GetChatMsg( 2124 );		// "��"
				case UNITKIND_RESURRECTION:				return CHATMGR->GetChatMsg( 2125 );		// "��Ȱ"
				case UNITKIND_DEBUFF:					return CHATMGR->GetChatMsg( 2126 );		// "��������"
				case UNITKIND_FAKE_DAMAGE:
				case UNITKIND_PHYSIC_FAKE_DAMAGE:
				case UNITKIND_MAGIC_FAKE_DAMAGE:
					return CHATMGR->GetChatMsg( 2127 );		// "��¥������"
				case UNITKIND_DECOY:					return CHATMGR->GetChatMsg( 2128 );		// "���ε�����(��������)"
				case UNITKIND_MANA_RECOVER:				return CHATMGR->GetChatMsg( 2129 );		// "����ȸ��"
				case UNITKIND_RETURN:					return CHATMGR->GetChatMsg( 2130 );		// "����(��ȯ)"
				case UNITKIND_SPELLBREAK:				return CHATMGR->GetChatMsg( 2131 );		// "����극��ũ"
				case UNITKIND_SUMMON:					return CHATMGR->GetChatMsg( 2132 );		// "��ȯ"
				case UNITKIND_RECALL:					return CHATMGR->GetChatMsg( 2133 );		// "�÷��̾� ��ȯ"
				}
			}
			break;
		case eToolTipMsgKeyWordType_ActiveSkill_UnitDataType:
			{
				switch( pActiveSkillInfo->UnitDataType )
				{
				case 0:									return CHATMGR->GetChatMsg( 151 );		// "����"
				case 1:
					{
						switch( pActiveSkillInfo->Unit )
						{
						case UNITKIND_PHYSIC_ATTCK:		return CHATMGR->GetChatMsg( 157 );		// "��"
						case UNITKIND_MAGIC_ATTCK:		return CHATMGR->GetChatMsg( 160 );		// "����"
						}
					}
					break;
				case 2:
					{
						switch( pActiveSkillInfo->Unit )
						{
						case UNITKIND_PHYSIC_ATTCK:		return CHATMGR->GetChatMsg( 2134 );		// "���� �������ݷ�"
						case UNITKIND_MAGIC_ATTCK:		return CHATMGR->GetChatMsg( 2135 );		// "���� �������ݷ�"
						}
					}
					break;
				}
			}
			break;
		case eToolTipMsgKeyWordType_ActiveSkill_UnitData:
			{
				sprintf( txt, "%d", pActiveSkillInfo->UnitData );
			}
			break;
		case eToolTipMsgKeyWordType_ActiveSkill_Success:
			{
				sprintf( txt, "%d%%", (int)(pActiveSkillInfo->Success / 10) );
			}
			break;
		case eToolTipMsgKeyWordType_ActiveSkill_Cancel:
			{
				sprintf( txt, "%d%%", (int)(pActiveSkillInfo->Cancel / 10) );
			}
			break;
		case eToolTipMsgKeyWordType_ActiveSkill_TrainPoint:
			{
				sprintf( txt, "%d", pActiveSkillInfo->TrainPoint );
			}
			break;
		case eToolTipMsgKeyWordType_ActiveSkill_TrainMoney:
			{
				return AddComma( pActiveSkillInfo->TrainMoney );
			}
			break;
		case eToolTipMsgKeyWordType_ActiveSkill_BuffRate1:
		case eToolTipMsgKeyWordType_ActiveSkill_BuffRate2:
		case eToolTipMsgKeyWordType_ActiveSkill_BuffRate3:
		case eToolTipMsgKeyWordType_ActiveSkill_BuffRate4:
		case eToolTipMsgKeyWordType_ActiveSkill_BuffRate5:
			{
				const int nMaxArraySize = sizeof( pActiveSkillInfo->BuffRate ) / sizeof( *pActiveSkillInfo->BuffRate );
				const int nArrayIndex = keywordType - (int)eToolTipMsgKeyWordType_ActiveSkill_BuffRate1;
				if( nArrayIndex >= nMaxArraySize )
					break;

				sprintf( txt, "%d%%", pActiveSkillInfo->BuffRate[ nArrayIndex ] );
			}
			break;
		case eToolTipMsgKeyWordType_ActiveSkill_IsMove:
			{
				return ( pActiveSkillInfo->IsMove ? CHATMGR->GetChatMsg( 2136 ) : CHATMGR->GetChatMsg( 151 ) );
			}
			break;
		case eToolTipMsgKeyWordType_ActiveSkill_RequiredPlayerLevel:
			{
				sprintf( txt, "%d", pActiveSkillInfo->RequiredPlayerLevel );
			}
			break;
		case eToolTipMsgKeyWordType_ActiveSkill_ConsumeItemCount:
			{
				sprintf( txt, "%d", pActiveSkillInfo->mConsumeItem.Durability );
			}
			break;
		case eToolTipMsgKeyWordType_ActiveSkill_ConsumePointType:
			{
				if( GetConsumePointTypeText( pActiveSkillInfo, txt, txtSize ) == NULL )
					return CHATMGR->GetChatMsg( 151 );		// "����"

				return GetConsumePointTypeText( pActiveSkillInfo, txt, txtSize );
			}
			break;
		case eToolTipMsgKeyWordType_ActiveSkill_ConsumePointValue:
			{
				sprintf( txt, "%d", pActiveSkillInfo->mConsumePoint.mValue );
			}
			break;
		case eToolTipMsgKeyWordType_ActiveSkill_IsBattle:
			{
				return ( pActiveSkillInfo->mIsBattle ? CHATMGR->GetChatMsg( 2137 ) : CHATMGR->GetChatMsg( 2138 ) );
			}
			break;
		case eToolTipMsgKeyWordType_ActiveSkill_IsPeace:
			{
				return ( pActiveSkillInfo->mIsPeace ? CHATMGR->GetChatMsg( 2137 ) : CHATMGR->GetChatMsg( 2138 ) );
			}
			break;
		case eToolTipMsgKeyWordType_ActiveSkill_mIsMove:
			{
				return ( pActiveSkillInfo->mIsMove ? CHATMGR->GetChatMsg( 2137 ) : CHATMGR->GetChatMsg( 2138 ) );
			}
			break;
		case eToolTipMsgKeyWordType_ActiveSkill_IsStop:
			{
				return ( pActiveSkillInfo->mIsStop ? CHATMGR->GetChatMsg( 2137 ) : CHATMGR->GetChatMsg( 2138 ) );
			}
			break;
		}
	}
	// BUFF_SKILL_INFO �� ����
	else if( keywordType < eToolTipMsgKeyWordType_Item )
	{
		cSkillInfo* pSkillInfo = SKILLMGR->GetSkillInfo( dwToolTipIndex );
		if( !pSkillInfo )
			return "";

		BUFF_SKILL_INFO* pBuffSkillInfo = ((cBuffSkillInfo*)pSkillInfo)->GetSkillInfo();
		if( !pBuffSkillInfo )
			return "";

		switch( (eToolTipMsgKeyWordType)keywordType )
		{
		case eToolTipMsgKeyWordType_BuffSkill_Money:
			{
				return AddComma( pBuffSkillInfo->Money );
			}
			break;
		case eToolTipMsgKeyWordType_BuffSkill_WeaponType:
			{
				if( GetSkillWeaponTypeText( pBuffSkillInfo, txt, txtSize ) == NULL )
					return CHATMGR->GetChatMsg( 151 );		// "����"

				return GetSkillWeaponTypeText( pBuffSkillInfo, txt, txtSize );
			}
			break;
		case eToolTipMsgKeyWordType_BuffSkill_ArmorType:
			{
				if( GetSkillArmorTypeText( pBuffSkillInfo, txt, txtSize ) == NULL )
					return CHATMGR->GetChatMsg( 151 );		// "����"

				return GetSkillArmorTypeText( pBuffSkillInfo, txt, txtSize );
			}
			break;
		case eToolTipMsgKeyWordType_BuffSkill_DelayTime:
			{
				// �ð�(1/1000��)
				ConvertHourMinSec( txt, pBuffSkillInfo->DelayTime / 1000.f );
			}
			break;
		case eToolTipMsgKeyWordType_BuffSkill_SkillEffect:
			{	
				sprintf( txt, "%d", pBuffSkillInfo->SkillEffect );
			}
			break;
		case eToolTipMsgKeyWordType_BuffSkill_Status:
			{
				switch( pBuffSkillInfo->Status )
				{
				case eStatusKind_None:										return CHATMGR->GetChatMsg( 151 );		// "����"
				case eStatusKind_Str:										return CHATMGR->GetChatMsg( 157 );		// "��"
				case eStatusKind_Dex:										return CHATMGR->GetChatMsg( 158 );		// "��ø"
				case eStatusKind_Vit:										return CHATMGR->GetChatMsg( 159 );		// "ü��"
				case eStatusKind_Int:										return CHATMGR->GetChatMsg( 160 );		// "����"
				case eStatusKind_Wis:										return CHATMGR->GetChatMsg( 161 );		// "����"
				case eStatusKind_All:										return CHATMGR->GetChatMsg( 2139 );		// "��� ����"
				case eStatusKind_PhysicAttack:								return CHATMGR->GetChatMsg( 152 );		// "���� ���ݷ�"
				case eStatusKind_PhysicDefense:								return CHATMGR->GetChatMsg( 153 );		// "���� ����"
				case eStatusKind_MagicAttack:								return CHATMGR->GetChatMsg( 155 );		// "���� ���ݷ�"
				case eStatusKind_MagicDefense:								return CHATMGR->GetChatMsg( 156 );		// "���� ����"
				case eStatusKind_Accuracy:									return CHATMGR->GetChatMsg( 2140 );		// "���� ��ġ"
				case eStatusKind_Avoid:										return CHATMGR->GetChatMsg( 2141 );		// "ȸ�� ��ġ"
				case eStatusKind_CriticalRate:								return CHATMGR->GetChatMsg( 2142 );		// "���� ũ��Ƽ�� �ߵ� Ȯ��"
				case eStatusKind_Range:										return CHATMGR->GetChatMsg( 2143 );		// "�����Ÿ�"
				case eStatusKind_MagicCriticalRate:							return CHATMGR->GetChatMsg( 2144 );		// "���� ũ��Ƽ�� �ߵ� Ȯ��"
				case eStatusKind_CriticalDamage:							return CHATMGR->GetChatMsg( 2145 );		// "���� ũ��Ƽ�� ������"
				case eStatusKind_MoveSpeed:									return CHATMGR->GetChatMsg( 840 );		// "�̵��ӵ�"
				case eStatusKind_Block:										return CHATMGR->GetChatMsg( 2146 );		// "���� �����"
				case eStatusKind_CoolTime:									return CHATMGR->GetChatMsg( 2147 );		// "��Ÿ��"
				case eStatusKind_CastingProtect:							return CHATMGR->GetChatMsg( 2148 );		// "���� ���� Ȯ�� ����"
				case eStatusKind_Hide:										return CHATMGR->GetChatMsg( 2149 );		// "���ڽ�"
				case eStatusKind_Detect:									return CHATMGR->GetChatMsg( 2150 );		// "����Ʈ"
				case eStatusKind_IncreaseDropRate:							return CHATMGR->GetChatMsg( 2151 );		// "�����۽���Ȯ��"
				case eStatusKind_DecreaseManaRate:							return CHATMGR->GetChatMsg( 2152 );		// "�Ҹ� MP"
				case eStatusKind_MaxLife:									return CHATMGR->GetChatMsg( 2060 );		// "�ִ� HP"
				case eStatusKind_MaxMana:									return CHATMGR->GetChatMsg( 2061 );		// "�ִ� MP"
				case eStatusKind_LifeRecoverRate:							return CHATMGR->GetChatMsg( 2153 );		// "HP ȸ����"
				case eStatusKind_ManaRecoverRate:							return CHATMGR->GetChatMsg( 2154 );		// "MP ȸ����"
				case eStatusKind_LifeRecover:								return CHATMGR->GetChatMsg( 2155 );		// "HP ����"
				case eStatusKind_ManaRecover:								return CHATMGR->GetChatMsg( 2156 );		// "MP ����"
				case eStatusKind_Poison:									return CHATMGR->GetChatMsg( 2157 );		// "��"
				case eStatusKind_Paralysis:									return CHATMGR->GetChatMsg( 2158 );		// "����"
				case eStatusKind_Stun:										return CHATMGR->GetChatMsg( 2159 );		// "����"
				case eStatusKind_Slip:										return CHATMGR->GetChatMsg( 2160 );		// "����"
				case eStatusKind_Freezing:									return CHATMGR->GetChatMsg( 2161 );		// "���"
				case eStatusKind_Stone:										return CHATMGR->GetChatMsg( 2162 );		// "��ȭ"
				case eStatusKind_Silence:									return CHATMGR->GetChatMsg( 2163 );		// "���Ϸ���"
				case eStatusKind_BlockAttack:								return CHATMGR->GetChatMsg( 2164 );		// "��������"
				case eStatusKind_God:										return CHATMGR->GetChatMsg( 2165 );		// "������"
				case eStatusKind_Shield:									return CHATMGR->GetChatMsg( 2166 );		// "��ȣ��"
				case eStatusKind_Bleeding:									return CHATMGR->GetChatMsg( 2167 );		// "����"
				case eStatusKind_Burning:									return CHATMGR->GetChatMsg( 2168 );		// "ȭ������"
				case eStatusKind_HolyDamage:								return CHATMGR->GetChatMsg( 2169 );		// "�ż�����"
				case eStatusKind_MoveStop:									return CHATMGR->GetChatMsg( 2170 );		// "Ȧ��"
				case eStatusKind_Attract:									return CHATMGR->GetChatMsg( 2171 );		// "���߰� 1�κ���"
				case eStatusKind_ManaBurn:									return CHATMGR->GetChatMsg( 2172 );		// "MP �� ���� - (��)������"
				case eStatusKind_Reflect:									return CHATMGR->GetChatMsg( 2173 );		// "������ �ݻ�"
				case eStatusKind_Absorb:									return CHATMGR->GetChatMsg( 2174 );		// "������ ���"
				case eStatusKind_DamageToLife:								return CHATMGR->GetChatMsg( 2175 );		// "���� ������ HP��ȯ"
				case eStatusKind_DamageToMana:								return CHATMGR->GetChatMsg( 2176 );		// "���� ������ MP��ȯ"
				case eStatusKind_GetLife:									return CHATMGR->GetChatMsg( 2177 );		// "���� HP"
				case eStatusKind_GetMana:									return CHATMGR->GetChatMsg( 2178 );		// "���� MP"
				case eStatusKind_GetExp:									return CHATMGR->GetChatMsg( 2179 );		// "���� EXP"
				case eStatusKind_GetGold:									return CHATMGR->GetChatMsg( 2180 );		// "���� GOLD"
				case eStatusKind_ProtectExp:								return CHATMGR->GetChatMsg( 2181 );		// "����г�Ƽ��ȣ"
				case eStatusKind_Attrib_None:								return CHATMGR->GetChatMsg( 2182 );		// "�� �Ӽ�"
				case eStatusKind_Attrib_Earth:								return CHATMGR->GetChatMsg( 2183 );		// "�� �Ӽ�"
				case eStatusKind_Attrib_Water:								return CHATMGR->GetChatMsg( 2184 );		// "�� �Ӽ�"
				case eStatusKind_Attrib_Divine:								return CHATMGR->GetChatMsg( 2185 );		// "�� �Ӽ�"
				case eStatusKind_Attrib_Wind:								return CHATMGR->GetChatMsg( 2186 );		// "ǳ �Ӽ�"
				case eStatusKind_Attrib_Fire:								return CHATMGR->GetChatMsg( 2187 );		// "ȭ �Ӽ�"
				case eStatusKind_Attrib_Dark:								return CHATMGR->GetChatMsg( 2188 );		// "�� �Ӽ�"
				case eStatusKind_Sword:										return CHATMGR->GetChatMsg( 660 );		// "��"
				case eStatusKind_Mace:										return CHATMGR->GetChatMsg( 662 );		// "���̽�"
				case eStatusKind_Axe:										return CHATMGR->GetChatMsg( 661 );		// "����"
				case eStatusKind_Staff:										return CHATMGR->GetChatMsg( 665 );		// "������"
				case eStatusKind_Bow:										return CHATMGR->GetChatMsg( 664 );		// "Ȱ"
				case eStatusKind_Gun:										return CHATMGR->GetChatMsg( 2048 );		// "��"
				case eStatusKind_Dagger:									return CHATMGR->GetChatMsg( 663 );		// "�ܰ�"
				case eStatusKind_Spear:										return CHATMGR->GetChatMsg( 2047 );		// "â"
				case eStatusKind_TwoHanded:									sprintf( txt, "%s %s", CHATMGR->GetChatMsg( 659 ), CHATMGR->GetChatMsg( 1961 ) );		break;		// ��� ����
				case eStatusKind_TwoBlade:									return CHATMGR->GetChatMsg( 2046 );		// "�̵���"
				case eStatusKind_OneHanded:									sprintf( txt, "%s %s", CHATMGR->GetChatMsg( 658 ), CHATMGR->GetChatMsg( 1961 ) );		break;		// �Ѽ� ����
				case eStatusKind_RobeArmor:									return CHATMGR->GetChatMsg( 692 );		// "�κ�"
				case eStatusKind_LightArmor:								return CHATMGR->GetChatMsg( 693 );		// "����Ʈ�Ƹ�"
				case eStatusKind_HeavyArmor:								return CHATMGR->GetChatMsg( 694 );		// "���Ƹ�"
				case eStatusKind_ShieldArmor:								return CHATMGR->GetChatMsg( 695 );		// "����"
				case eStatusKind_NormalSpeedRate:							return CHATMGR->GetChatMsg( 2189 );		// "�Ϲ� ���� �ӵ�"
				case eStatusKind_PhysicSkillSpeedRate:						return CHATMGR->GetChatMsg( 2190 );		// "���� ��ų ���� �ӵ�"
				case eStatusKind_MagicSkillSpeedRate:						return CHATMGR->GetChatMsg( 2191 );		// "���� ��ų ���� �ӵ�"
				case eStatusKind_NormalPhysicSkillSpeedRate:				return CHATMGR->GetChatMsg( 2192 );		// "�Ϲ� ���� + ���� ��ų ���� �ӵ�"
				case eStatusKind_ConvertHealthWithGivedDamage:				return CHATMGR->GetChatMsg( 2193 );		// "�� �������� HP�� ��ȯ"
				case eStatusKind_ConvertManaWithGivedDamage:				return CHATMGR->GetChatMsg( 2194 );		// "�� �������� MP�� ��ȯ"
				case eStatusKind_EventSkillToVictim:						return CHATMGR->GetChatMsg( 2195 );		// "��� �� ��ų �ߵ�"
				case eStatusKind_EventSkillToKiller:						return CHATMGR->GetChatMsg( 2196 );		// "�����Ų �ڿ��� ��ų �ߵ�"
				case eStatusKind_EnableTwoBlade:							return CHATMGR->GetChatMsg( 2197 );		// "�̵��� ��� ����"
				case eStatusKind_UnableUseItem:								return CHATMGR->GetChatMsg( 2198 );		// "������ ��� ����"
				case eStatusKind_UnableBuff:								return CHATMGR->GetChatMsg( 2199 );		// "���� ��� ����"
				case eStatusKind_IncreaseExpRate:							return CHATMGR->GetChatMsg( 2200 );		// "����ġ ���� %";
				case eStatusKind_DamageToManaDamage:						return CHATMGR->GetChatMsg( 2201 );		// "�����ǵ�"
				}
			}
			break;
		case eToolTipMsgKeyWordType_BuffSkill_StatusDataType:
			{
				switch( pBuffSkillInfo->StatusDataType )
				{
				case 0:		return CHATMGR->GetChatMsg( 151 );		// "����"
				case 1:		return "���";
				case 2:		return "%%";
				}
			}
			break;
		case eToolTipMsgKeyWordType_BuffSkill_StatusData:
			{
				switch( pBuffSkillInfo->StatusDataType )
				{
				case 0:		
				case 1:	
					{
						sprintf( txt, "%d", (int)abs( pBuffSkillInfo->StatusData ) );
					}
					break;
				case 2:
					{
						sprintf( txt, "%.1f%%", abs( pBuffSkillInfo->StatusData ) );
					}
					break;
				}
					
			}
			break;
		case eToolTipMsgKeyWordType_BuffSkill_ConditonType:
			{
				switch( pBuffSkillInfo->mCondition.mType )
				{
				case BUFF_SKILL_INFO::Condition::TypeNone:							return CHATMGR->GetChatMsg( 151 );		// "����"
				case BUFF_SKILL_INFO::Condition::TypeLifePercent:					return CHATMGR->GetChatMsg( 2202 );		// "���� HP �ۼ�Ʈ"
				case BUFF_SKILL_INFO::Condition::TypeLife:							return CHATMGR->GetChatMsg( 2203 );		// "���� HP ��ġ"
				case BUFF_SKILL_INFO::Condition::TypeLifeMax:						return CHATMGR->GetChatMsg( 2060 );		// "�ִ� HP"
				case BUFF_SKILL_INFO::Condition::TypeLifeRecovery:					return CHATMGR->GetChatMsg( 846 );		// "HP ȸ��"
				case BUFF_SKILL_INFO::Condition::TypeManaPercent:					return CHATMGR->GetChatMsg( 2204 );		// "���� MP �ۼ�Ʈ"
				case BUFF_SKILL_INFO::Condition::TypeMana:							return CHATMGR->GetChatMsg( 2205 );		// "���� MP ��ġ";
				case BUFF_SKILL_INFO::Condition::TypeManaMax:						return CHATMGR->GetChatMsg( 2061 );		// "�ִ� MP"
				case BUFF_SKILL_INFO::Condition::TypeManaRecovery:					return CHATMGR->GetChatMsg( 847 );		// "HP ȸ��"
				case BUFF_SKILL_INFO::Condition::TypeStrength:						return CHATMGR->GetChatMsg( 157 );		// "��"
				case BUFF_SKILL_INFO::Condition::TypeIntelligence:					return CHATMGR->GetChatMsg( 160 );		// "����"
				case BUFF_SKILL_INFO::Condition::TypeDexterity:						return CHATMGR->GetChatMsg( 158 );		// "��ø"
				case BUFF_SKILL_INFO::Condition::TypeWisdom:						return CHATMGR->GetChatMsg( 161 );		// "����"
				case BUFF_SKILL_INFO::Condition::TypeVitality:						return CHATMGR->GetChatMsg( 159 );		// "ü��"
				case BUFF_SKILL_INFO::Condition::TypePhysicalAttack:				return CHATMGR->GetChatMsg( 152 );		// "���� ���ݷ�"
				case BUFF_SKILL_INFO::Condition::TypePhysicalDefense:				return CHATMGR->GetChatMsg( 153 );		// "���� ����"
				case BUFF_SKILL_INFO::Condition::TypeMagicalAttack:					return CHATMGR->GetChatMsg( 155 );		// "���� ���ݷ�"
				case BUFF_SKILL_INFO::Condition::TypeMagicalDefense:				return CHATMGR->GetChatMsg( 156 );		// "���� ����"
				case BUFF_SKILL_INFO::Condition::TypeCriticalRate:					return CHATMGR->GetChatMsg( 842 );		// "ũ��Ƽ�� ��ġ";
				case BUFF_SKILL_INFO::Condition::TypeCriticalDamage:				return CHATMGR->GetChatMsg( 843 );		// "ũ��Ƽ�� ������";
				case BUFF_SKILL_INFO::Condition::TypeAccuracy:						return CHATMGR->GetChatMsg( 162 );		// "����"
				case BUFF_SKILL_INFO::Condition::TypeEvade:							return CHATMGR->GetChatMsg( 162 );		// "����"
				case BUFF_SKILL_INFO::Condition::TypeMoveSpeed:						return CHATMGR->GetChatMsg( 841 );		// "ȸ��"
				}
			}
			break;
		case eToolTipMsgKeyWordType_BuffSkill_ConditionOperation:
			{
				switch( pBuffSkillInfo->mCondition.mOperation )
				{
				case BUFF_SKILL_INFO::Condition::OperationNone:			return CHATMGR->GetChatMsg( 151 );	// "����"
				case BUFF_SKILL_INFO::Condition::OperationMoreEqual:	return CHATMGR->GetChatMsg( 2056 );	// �̻�
				case BUFF_SKILL_INFO::Condition::OperationLessEqual:	return CHATMGR->GetChatMsg( 2057 );	// ����
				case BUFF_SKILL_INFO::Condition::OperationMore:			return CHATMGR->GetChatMsg( 2058 );	// �ʰ�
				case BUFF_SKILL_INFO::Condition::OperationLess:			return CHATMGR->GetChatMsg( 2059 );	// �̸�
				case BUFF_SKILL_INFO::Condition::OperationEqual:		return CHATMGR->GetChatMsg( 2059 );	// ����
				}
			}
			break;
		case eToolTipMsgKeyWordType_BuffSkill_ConditionValue:
			{
				sprintf( txt, "%d", (int)pBuffSkillInfo->mCondition.mValue );
			}
			break;
		case eToolTipMsgKeyWordType_BuffSkill_Count:
			{
				sprintf( txt, "%d", pBuffSkillInfo->Count );
			}
			break;
		case eToolTipMsgKeyWordType_BuffSkill_RuleBattle:
			{
				switch( pBuffSkillInfo->mRuleBattle )
				{
				case eBuffSkillActiveRule_Active:			return CHATMGR->GetChatMsg( 2206 );	// ȿ�� �ߵ�
				case eBuffSkillActiveRule_Delete:			return CHATMGR->GetChatMsg( 2063 ); // ȿ�� ����
				case eBuffSkillActiveRule_Stop:				return CHATMGR->GetChatMsg( 2064 ); // ȿ�� ����
				}
			}
			break;
		case eToolTipMsgKeyWordType_BuffSkill_RulePeace:
			{
				switch( pBuffSkillInfo->mRulePeace )
				{
				case eBuffSkillActiveRule_Active:			return CHATMGR->GetChatMsg( 2206 );	// ȿ�� �ߵ�
				case eBuffSkillActiveRule_Delete:			return CHATMGR->GetChatMsg( 2063 ); // ȿ�� ����
				case eBuffSkillActiveRule_Stop:				return CHATMGR->GetChatMsg( 2064 ); // ȿ�� ����
				}
			}
			break;
		case eToolTipMsgKeyWordType_BuffSkill_RuleMove:
			{
				switch( pBuffSkillInfo->mRuleMove )
				{
				case eBuffSkillActiveRule_Active:			return CHATMGR->GetChatMsg( 2206 );	// ȿ�� �ߵ�
				case eBuffSkillActiveRule_Delete:			return CHATMGR->GetChatMsg( 2063 ); // ȿ�� ����
				case eBuffSkillActiveRule_Stop:				return CHATMGR->GetChatMsg( 2064 ); // ȿ�� ����
				}
			}
			break;
		case eToolTipMsgKeyWordType_BuffSkill_RuleStop:
			{
				switch( pBuffSkillInfo->mRuleStop )
				{
				case eBuffSkillActiveRule_Active:			return CHATMGR->GetChatMsg( 2206 );	// ȿ�� �ߵ�
				case eBuffSkillActiveRule_Delete:			return CHATMGR->GetChatMsg( 2063 ); // ȿ�� ����
				case eBuffSkillActiveRule_Stop:				return CHATMGR->GetChatMsg( 2064 ); // ȿ�� ����
				}
			}
			break;
		case eToolTipMsgKeyWordType_BuffSkill_RuleRest:
			{
				switch( pBuffSkillInfo->mRuleRest )
				{
				case eBuffSkillActiveRule_Active:			return CHATMGR->GetChatMsg( 2206 );	// ȿ�� �ߵ�
				case eBuffSkillActiveRule_Delete:			return CHATMGR->GetChatMsg( 2063 ); // ȿ�� ����
				case eBuffSkillActiveRule_Stop:				return CHATMGR->GetChatMsg( 2064 ); // ȿ�� ����
				}
			}
			break;
		case eToolTipMsgKeyWordType_BuffSkill_Die:
			{
				switch( pBuffSkillInfo->Die  )
				{
				case 0:		return CHATMGR->GetChatMsg( 2071 );		// "�����"
				case 1:		return CHATMGR->GetChatMsg( 2072 );		// "������"
				}
			}
			break;
		case eToolTipMsgKeyWordType_BuffSkill_CountType:
			{
				switch( pBuffSkillInfo->CountType )
				{
				case eBuffSkillCountType_None:					return CHATMGR->GetChatMsg( 151 );		// "����";
				case eBuffSkillCountType_Dead:					return CHATMGR->GetChatMsg( 2077 );		// "ĳ���� ���"
				case eBuffSkillCountType_AnyDamageVolumn:		return CHATMGR->GetChatMsg( 2078 );		// 	"��� �ǰ� ��ġ"
				case eBuffSkillCountType_AnyDamage:				return CHATMGR->GetChatMsg( 2079 );		// "��� �ǰ� ȸ��"
				case eBuffSkillCountType_AnyHitVolumn:			return CHATMGR->GetChatMsg( 2080 );		// "��� ���� ��ġ"
				case eBuffSkillCountType_AnyHit:				return CHATMGR->GetChatMsg( 2081 );		// "��� ���� ȸ��"
				case eBuffSkillCountType_PhysicalDamageVolumn:	return CHATMGR->GetChatMsg( 2082 );		// "���� �ǰ� ��ġ"
				case eBuffSkillCountType_PhysicalDamage:		return CHATMGR->GetChatMsg( 2083 );		// "���� �ǰ� ȸ��"
				case eBuffSkillCountType_PhysicalHitVolumn:		return CHATMGR->GetChatMsg( 2084 );		// "���� ���� ��ġ"
				case eBuffSkillCountType_PhysicalHit:			return CHATMGR->GetChatMsg( 2085 );		// "���� ���� ȸ��"
				case eBuffSkillCountType_MagicalDamageVolumn:	return CHATMGR->GetChatMsg( 2086 );		// "���� �ǰ� ��ġ"
				case eBuffSkillCountType_MagicalDamage:			return CHATMGR->GetChatMsg( 2087 );		// "���� �ǰ� ȸ��"
				case eBuffSkillCountType_MagicalHitVolumn:		return CHATMGR->GetChatMsg( 2088 );		// "���� ���� ��ġ"
				case eBuffSkillCountType_MagicalHit:			return CHATMGR->GetChatMsg( 2089 );		// "���� ���� ȸ��"
				case eBuffSkillCountType_HealVolumn:			return CHATMGR->GetChatMsg( 2090 );		// "���� �� ��ġ"
				case eBuffSkillCountType_Heal:					return CHATMGR->GetChatMsg( 2091 );		// "���� �� ȸ��"
				case eBuffSkillCountType_BeHealedVolumn:		return CHATMGR->GetChatMsg( 2092 );		// "���� ���� ��ġ"
				case eBuffSkillCountType_BeHealed:				return CHATMGR->GetChatMsg( 2093 );		// "���� ���� ȸ��"
				}
			}
			break;
		case eToolTipMsgKeyWordType_BuffSkill_IsEndTime:
			{
				return ( pBuffSkillInfo->IsEndTime ? CHATMGR->GetChatMsg( 2207 ) : CHATMGR->GetChatMsg( 752 ) );
			}
			break;
		case eToolTipMsgKeyWordType_BuffSkill_ItemCount1:
		case eToolTipMsgKeyWordType_BuffSkill_ItemCount2:
			{
				const int nMaxArraySize = sizeof( pBuffSkillInfo->ItemCount ) / sizeof( *pBuffSkillInfo->ItemCount );
				const int nArrayIndex = keywordType - (int)eToolTipMsgKeyWordType_BuffSkill_Item1;
				if( nArrayIndex >= nMaxArraySize )
					break;

				sprintf( txt, "%d", pBuffSkillInfo->ItemCount[ nArrayIndex ] );
			}
			break;
		}
	}
	// Item ������ �̿��ϴ� ���
	else
	{
		const ITEM_INFO* const pItemInfo = ITEMMGR->GetItemInfo( dwToolTipIndex );
		if( !pItemInfo )
			return "";

		switch( (eToolTipMsgKeyWordType)keywordType )
		{
		case eToolTipMsgKeyWordType_Item_Name:
			{
				return pItemInfo->ItemName;
			}
			break;
		}
	}

	return txt;
}

// Script���ڿ����� �������� ��ȯ�Ѵ�.
void cSkillManager::ConvertToolTipMsgKeyWordFormText( DWORD dwToolTipIndex, std::string& strMsg, std::string::size_type startPos, int prevKeyWordType )
{
	std::string::size_type keywordStart = 0;
	while( ( keywordStart = strMsg.find( "$", startPos ) ) != std::string::npos )
	{
		startPos = keywordStart + 1;

		std::string::size_type keywordSize = 0;
		std::string keyword;
		// ����� ����
		if( GetToolTipMsgKeyWordFromText( strMsg, keyword, keywordStart, keywordSize ) == FALSE )
			continue;
		

		// ����� ����� �˻�
		DWORD dwHashCode = GetHashCodeFromTxt( keyword.c_str() );
		std::map< DWORD, int >::const_iterator mapKeyWordIter = m_mapToolTipMsgKeyWord.find( dwHashCode );
		if( mapKeyWordIter != m_mapToolTipMsgKeyWord.end() )
		{
			strMsg.replace( keywordStart, keywordSize, "" );

			char txt[MAX_PATH]={0,};
			const char* convertStr = GetTextFromToolTipMsgKeyWordType( dwToolTipIndex, mapKeyWordIter->second, txt, MAX_PATH, prevKeyWordType );
			if( convertStr == NULL )
				continue;

			strMsg.insert( keywordStart, convertStr );

			// ������ ���� �˻�
			const char* strPtr = "->";
			const size_t nPtrSize = strlen( strPtr );
			std::string ptrstr = strMsg.substr( keywordStart, nPtrSize );
			if( ptrstr.compare( strPtr ) == 0 )
			{
				strMsg.replace( keywordStart, nPtrSize, "" );

				// ������ ����� ��ȯ
				ConvertToolTipMsgKeyWordFormText( dwToolTipIndex, strMsg, keywordStart, mapKeyWordIter->second );
			}
		}

		if( prevKeyWordType != eToolTipMsgKeyWordType_None ) 
			return;
	}
}

BOOL cSkillManager::IsUsedPhysicAttackSpeed( const ACTIVE_SKILL_INFO& activeSkillInfo ) const
{
	// 100624 ShinJS ��ų Unit�� ���� ����/���� ���ݼӵ� �̿뱸��.
	switch( activeSkillInfo.Unit )
	{
		case UNITKIND_PHYSIC_ATTCK:
		case UNITKIND_PHYSIC_FAKE_DAMAGE:
			return TRUE;

		case UNITKIND_NONE:
		case UNITKIND_MAGIC_ATTCK:
		case UNITKIND_LIFE_RECOVER:
		case UNITKIND_RESURRECTION:
		case UNITKIND_DEBUFF:
		case UNITKIND_MANA_RECOVER:
		case UNITKIND_MAGIC_FAKE_DAMAGE:
			return FALSE;
	}

	return TRUE;
}
