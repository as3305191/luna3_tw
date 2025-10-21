#include "stdafx.h"
#include "Hero.h"
#include "GameIn.h"
#include "MHCamera.h"
#include "ObjectManager.h"
#include "MoveManager.h"
#include "ObjectStateManager.h"
#include "ItemManager.h"
#include "CharacterCalcManager.h"
#include "PartyManager.h"
#include "cSkillTreeManager.h"
#include "ObjectGuagen.h"
#include "./Audio/MHAudioManager.h"
#include "StreetStallManager.h"
#include "CheatMsgParser.h"
#include "interface/cWindowManager.h"
#include "ChatManager.h"
#include "WindowIdEnum.h"
#include "ReviveDialog.h"
#include "CharacterDialog.h"
#include "InventoryExDialog.h"
#include "DealDialog.h"
#include "ProgressDialog.h"
#include "mhMap.h"
#include "BattleSystem_Client.h"
#include "NpcScriptDialog.h"
#include "CommonCalcFunc.h"
#include "AppearanceManager.h"
#include "ShoutDialog.h"
#include "GuageDialog.h"
#include "InventoryExDialog.h"
#include "StorageDialog.h"
#include "QuickDlg.h"
#include "../[cc]skill/client/manager/skillmanager.h"
#include "../[cc]skill/client/info/activeskillinfo.h"
#include "../[cc]skill/client/info/buffskillinfo.h"
#include "../[cc]skill/Client/Object/SkillObject.h"
#include "NpcImageDlg.h"
#include "cskilltrainingdlg.h"
#include "cSkillTreeDlg.h"
#include "ChatManager.h"
#include "ObjectBalloon.h"
#include "../hseos/Farm/SHFarmManager.h"
#include "PKManager.h"
#include "MHTimeManager.h"
#include "CWayPointManager.h"
#include "BigMapDlg.h"
#include "MiniMapDlg.h"
#include "TileManager.h"
#include "MovePoint.h"
#include "WorldMapDlg.h"
#include "MonsterGuageDlg.h"
#include "BattleGuageDlg.h"
#include "..\hseos\Family\SHFamily.h"

VECTOR3 gHeroPos;

extern volatile LONG g_PathThreadActivated;

CHero::CHero() :
m_pcsFamily(new CSHFamily),
m_WayPointManager(new CWayPointManager),
m_KeyMove_DistOneStep(1)
{
	//m_NextAction.Clear();
	m_CurComboNum = 0;
	m_maxExpPoint = 0;
	m_StorageNum = 0;	
	m_matchedQuantity = 0;
	m_byExtendedInvenCount = 0;
	m_DisableWASD = 0;
}

CHero::~CHero()
{
	ITEMMGR->RemoveCoolTime( GetID() );

	Release();
	SAFE_DELETE(m_pcsFamily);
	SAFE_DELETE(m_WayPointManager);
}

void CHero::InitHero(HERO_TOTALINFO * pHeroTotalInfo)
{
	mMoveSound = 0;
	m_MovingAction.Clear();
	m_NextAction.Clear();
	m_SkillStartAction.Clear();
	m_bIsAutoAttackMode = TRUE;
	m_bIsAutoAttacking = FALSE;
	memcpy(&m_HeroTotalInfo, pHeroTotalInfo, sizeof(HERO_TOTALINFO));

	m_maxExpPoint = GAMERESRCMNGR->GetMaxExpPoint(GetLevel());
	
	ZeroMemory( &mItemBaseStat, sizeof( PlayerStat ) );
	ZeroMemory( &mItemOptionStat, sizeof( PlayerStat ) );

	// 080313 LUJ, ùù? ùùùùùù ù?ù ù?ù?
	ZeroMemory( &mSetItemStat, sizeof( PlayerStat ) );

	m_dwLastSocietyAction = gCurTime;

	GAMEIN->GetQuickDlg()->SetActiveSheet( 0, 0 );

	memset( &mPassiveStatus, 0, sizeof( Status ) );
	memset( &mBuffStatus, 0, sizeof( Status ) );
	memset( &mRatePassiveStatus, 0, sizeof( Status ) );
	memset( &mRateBuffStatus, 0, sizeof( Status ) );
	memset( &mAbnormalStatus, 0, sizeof( AbnormalStatus ) );

	// desc_hseos_ùùù?ùùù01
	// S ùùù?ùùù ù?ù added by hseos 2007.05.23 2008.01.11:CPlayer ùùùù ù?ù
	ZeroMemory(&m_stMonstermeterInfo, sizeof(m_stMonstermeterInfo));
	mCurrentSkillObjectIndex = 0;


//---KES ùùù ùùùù ùùùù ùùùù 071128
	m_bNoDeffenceFromArmor = FALSE;
	mSkillDelay = 0;
	mSkillCancelDelay = 0;
	m_dwFollowPlayerIdx = 0;
//---------------

	// 090526 ShinJS --- ù?ùùùùù ùù?ù?ùù ù?ùùù ù?ù?
	m_dwInvitePartyByNameLastTime = 0;
	m_dwInvitePartyByNameLimitTime = 0;


	// 100226 ShinJS --- ùùÊ? ùùùù ùùùù ù?ù
	mWeaponEnchantLevel = 0;
	mPhysicDefenseEnchantLevel = 0;
	mMagicDefenseEnchantLevel = 0;
}

void CHero::Process()
{
	CPlayer::Process();
	CAMERA->Process();
	Move_KeyboardInput();

	if(m_MovingAction.HasAction())
	{
		if(m_MovingAction.CheckTargetDistance(&GetCurPosition()) == TRUE)
		{
			// 091026 ShinJS --- ù?ù ùùùùù ùù„ùùùùùù ù?ù?
			ClearWayPoint();

			MOVEMGR->HeroMoveStop();

			m_MovingAction.ExcuteAction(this);
			m_MovingAction.Clear();
		}
		else
		{
			if( m_MovingAction.GetActionKind() == eActionKind_Skill )
			{
				static DWORD dwSkillPersuitTime	= 0;

				if(gCurTime > dwSkillPersuitTime)
				{
					m_MovingAction.ExcuteAction(this);

					const int SKILL_PERSUIT_TICK = 1000;
					dwSkillPersuitTime = gCurTime + SKILL_PERSUIT_TICK;
				}
			}
			else if( MOVEMGR->IsMoving(this) == FALSE )
			{
				m_MovingAction.Clear();
			}
		}
	}
	else if(m_bIsAutoAttacking)
	{
		if(m_NextAction.HasAction() == FALSE)
		{
			if(GetCurComboNum() < MAX_COMBO_NUM || GetCurComboNum() == SKILL_COMBO_NUM)	// ù?ùùùùùùù ù?ù 2ùùùùùù	12/3ùù ?ùù ùùù 3ùùùù 2ùù ù?ù
			{
				if(SKILLMGR->OnSkillCommand(this,&m_AutoAttackTarget,FALSE) == FALSE)
					DisableAutoAttack();
			}
		}
		else
		{
			m_NextAction.ExcuteAction(this);
		}
	}

	// 090204 LUJ, ?ùù ùù?ùù ù?ùùù ù?ùù?ù ?ùù/ùù?ùùùù?ùù
	ProcessSpecialSkill();
	GetPosition(&gHeroPos);
	MAP->SetShadowPivotPos(&gHeroPos);
	AUDIOMGR->SetListenerPosition(
		&gHeroPos,
		GetAngle());

	if(IsDied())
	{
		if(eObjectState_Die != GetState())
		{
			OBJECTSTATEMGR->EndObjectState(
				this,
				GetState());
			OBJECTSTATEMGR->StartObjectState(
				this,
				eObjectState_Die);

			ChangeMotion(
				CHARACTER_MOTION[eCharacterMotion_Die1][GetWeaponAniType()],
				0);
			ChangeBaseMotion(
				CHARACTER_MOTION[eCharacterMotion_Died1][GetWeaponAniType()]);
		}

		PKMGR->SetPKStartTimeReset();
	}

	if(GetState() == eObjectState_SkillUsing)
	{
		if(gCurTime - GetStateStartTime() > 10000)	//ùùù?ù ùùù?? ùùùùù?ùùù ù?ùùù.
		{
			OBJECTSTATEMGR->EndObjectState(this,eObjectState_SkillUsing);
		}
	}

	ProcMonstermeterPlayTime();
	OBJECTMGR->ApplyShadowOption(
		this);

	// 090909 ShinJS --- ùù„ùù ùùùùù ùùùù Ûùù
	switch( m_WayPointManager->GetWMStatus() )
	{
	// ùùùùùùùù ùù
	case eWMStatus_Success:
		{
			// ùù ùùùùùùùù?ùù ùù??ù?ù, ùùù?ù ùùùùù?ù ù?ùù ù?ù
			m_WayPointManager->SetWMStatus(eWMStatus_Inactive);

			// ùùùù ùùùùùùùù?ùù ù?ù
			NextMove();

			// 091204 --- ùù? ùù? ùùùù(ùùùùù?ù ùù„ù? ù?ùù Stopù?ùùùùù ùùùùùù ùù?ùù?ùù ùùùùùùùù. ù?ù ùùùùù?ù ù?ùùù ù?ù.)
			VECTOR3 vecDest = m_WayPointManager->GetFlagDestination();
			CBigMapDlg* pBigMapDlg = GAMEIN->GetBigMapDialog();
			CWorldMapDlg* pWorldMapDlg = GAMEIN->GetWorldMapDialog();
			CBigMapDlg* pWorldToBigMapDlg = (pWorldMapDlg != NULL ? pWorldMapDlg->GetBigMapDlg() : NULL);
			CMiniMapDlg* pMiniMapDlg = GAMEIN->GetMiniMapDialog();
			if( pBigMapDlg )		pBigMapDlg->SetMovePoint( &vecDest );
			if( pWorldToBigMapDlg )	pWorldToBigMapDlg->SetMovePoint( &vecDest );
			if( pMiniMapDlg )		pMiniMapDlg->SetMovePoint( &vecDest );

			MOVEPOINT->SetPoint( &vecDest );
		}
		break;
	// ùùùùùù ùùù
	case eWMStatus_Fail:
		{
			m_WayPointManager->SetWMStatus(eWMStatus_Inactive);
			ClearWayPoint();

			// 091208 ShinJS --- ùù„ùù ùùù?ù ùùùùùù ù?ù ù?ùùùùùùùùù ù?ùù?ù.
			VECTOR3 vecDest = m_WayPointManager->GetFlagDestination();
			VECTOR3 startPos;
			GetPosition( &startPos );

			// ùùùùùù ù?ùùùùù ù?ùùùùùùù ùùù?ù.
			if( MAP->CollisionLine( &startPos, &vecDest, &vecDest, MAP->GetMapNum(), this ) == FALSE )
				break;

			if( MOVEMGR->IsMoving( this ) )
				MOVEMGR->CalcPositionEx( this, gCurTime );

			if ( MAP->CollisionCheck_OneLine_New( &startPos, &vecDest ) == TRUE)
			{
				Move_Simple( &vecDest );
				MOVEPOINT->SetPoint( &vecDest );
			}
		}
		break;
	}
}

// desc_hseos_ùùù?ùùù01
// S ùùù?ùùù ù?ù added by hseos 2007.05.23 2008.01.11:CPlayer ùùùù ù?ù
void CHero::ProcMonstermeterPlayTime()
{
	if (gCurTime > m_stMonstermeterInfo.nPlayTimeTick)
	{
		m_stMonstermeterInfo.nPlayTimeTick = gCurTime + 1000;
		++m_stMonstermeterInfo.nPlayTime;
		++m_stMonstermeterInfo.nPlayTimeTotal;
	}
}
// E ùùù?ùùù ù?ù added by hseos 2007.05.23 2008.01.11:CPlayer ùùùù ù?ù

void CHero::GetHeroTotalInfo(HERO_TOTALINFO * info)
{
	ASSERT(info);
	memcpy(info, &m_HeroTotalInfo, sizeof(HERO_TOTALINFO));
}


void CHero::ApplyInterface()
{
#ifndef _TESTCLIENT_
	SetMoney(m_HeroTotalInfo.Money);
	SetLevel(m_CharacterInfo.Level);
	SetMaxLife(m_CharacterInfo.MaxLife);
	SetLife(m_CharacterInfo.Life, 0);
	SetMaxMana(m_HeroTotalInfo.MaxMana);
	SetMana(m_HeroTotalInfo.Mana, 0);
	SetMaxExpPoint(m_maxExpPoint);

	SetExpPoint(m_HeroTotalInfo.ExpPoint);

	SetStrength(m_HeroTotalInfo.Str);
	SetDexterity(m_HeroTotalInfo.Dex);
	SetVitality(m_HeroTotalInfo.Vit);
	SetWisdom(m_HeroTotalInfo.Wis);
	SetIntelligence(m_HeroTotalInfo.Int);

	SetPartyIdx(m_HeroTotalInfo.PartyID);
	SetGuildMemberRank(m_CharacterInfo.PositionInMunpa);
	SetGuildIdx(m_CharacterInfo.MunpaID);
	SetGuildName(m_CharacterInfo.GuildName);
	SetFamilyName(m_CharacterInfo.FamilyName);
	GAMEIN->GetCharacterDialog()->RefreshGuildInfo();
	GAMEIN->GetCharacterDialog()->RefreshFamilyInfo();
	SetBadFame(m_CharacterInfo.BadFame);
	// 070117 LYW --- Add function to setting character face.
	GAMEIN->GetGuageDlg()->MakeHeroFace() ;

	if(cStatic* const nameStatic = (cStatic*)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGENAME))
	{
		nameStatic->SetStaticText(
			m_BaseObjectInfo.ObjectName);
	}
#endif
}

void CHero::SetMaxLife(DWORD maxlife)
{
	CPlayer::SetMaxLife(maxlife);

	DWORD newMaxLife = GetMaxLife();
	if(newMaxLife == 0)
	{
		newMaxLife = 1;
	}
	((CObjectGuagen*)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGELIFE))->SetValue(
		(float)m_CharacterInfo.Life/(float)newMaxLife, 0 );

	char szValue[50] = {0};
	sprintf( szValue, "%d/%d", GetLife(), newMaxLife );

	cStatic* ps = (cStatic *)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGELIFETEXT);
	if( ps )	ps->SetStaticText( szValue );	
}

void CHero::Heal(CObject* pHealer,BYTE HealKind,DWORD HealVal)
{
	CHATMGR->AddMsg(
		CTC_GETMONEY,
		CHATMGR->GetChatMsg(778),
		pHealer ? pHealer->GetObjectName() : GetObjectName(),
		HealVal);
	EFFECTMGR->AddHealNumber(
		HealVal,
		pHealer,
		this,
		HealKind);
	ChangeLife(
		HealVal);
}
void CHero::ChangeLife(int changeval)
{
	// 070419 LYW --- Hero : modified function ChangeLife.
	// 080616 LUJ, ù?ù HPùù ùùùù ù?ù ?ùı?ùùù ù?ù
	DWORD newlife = min( GetMaxLife(), GetLife() + changeval );

	SetLife(newlife);
}

void CHero::SetLife(DWORD life, BYTE type)
{
	CPlayer::SetLife(life);

	if( m_BaseObjectInfo.ObjectBattleState == eObjectBattleState_Battle )
	{
		GAMEIN->GetBattleGuageDlg()->HeroBattleGuageUpdate();
	}

	if(GetState() == eObjectState_Die)
	{
		life = 0;
		GAMEIN->GetBattleGuageDlg()->HeroDie();
	}

//---KES divide 0 ùùùù
	DWORD MaxLife = GetMaxLife();
	if(MaxLife == 0)
	{
		MaxLife = 1;
	}

	((CObjectGuagen*)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGELIFE))->SetValue(
		(float)life/(float)MaxLife, 
		1000);

	char szValue[50] = {0};
	sprintf( szValue, "%d/%d", life, GetMaxLife() );
	cStatic* ps = (cStatic *)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGELIFETEXT);
	if( ps )	ps->SetStaticText( szValue );
}

void CHero::SetMoney(MONEYTYPE Money)
{
	m_HeroTotalInfo.Money = Money;
	if(GAMEIN->GetInventoryDialog())
		GAMEIN->GetInventoryDialog()->SetMoney(Money);

	GAMEIN->GetSkillTrainingDlg()->SetGold();
}

void CHero::SetMaxExpPoint(EXPTYPE dwPoint)
{
	m_maxExpPoint = dwPoint;

	float value = (float)GetExpPoint()/(float)m_maxExpPoint;

	((CObjectGuagen*)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGEEXPPOINT))->SetValue( value, 0 );
	// 070113 LYW --- Delete this line.
	//GAMEIN->GetCharacterDialog()->SetExpPointPercent( value*100 );

	// ùùùùùùù?ù : ùù? ?ùù
	char szValue[50];
	// 070124 LYW --- Modified this line.
	//sprintf( szValue, "%d / %d", GetExpPoint(), dwPoint );
	sprintf( szValue, "%d/%d", GetExpPoint(), dwPoint );
	cStatic* ps = (cStatic *)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGEEXPPOINTTEXT);
	if( ps )	ps->SetStaticText( szValue );
}

void CHero::SetExpPoint(EXPTYPE dwPoint, BYTE type)
{
	m_HeroTotalInfo.ExpPoint = dwPoint;

	// ùùùùùùù?ù ùùùùùù ùùùù
	float value = (float)dwPoint/(float)m_maxExpPoint;
	// 080530 LYW --- Hero : Modified a data type DWORD to EXPTYPE.
	DWORDEX valueTime = (type == 0)? 0 : (2000/m_maxExpPoint)*dwPoint;
	//DWORD valueTime = 0 ;

	// 080604 LYW --- Hero : ùùùù? ùùùùùù Ûùù ùùùù.
	// ùùùù? ùùùùùùùù timeùù ùùùù ù?∞ù ù¥¥?ù ù?ù.
	// typeùù 0ùùùù ùùùùù?ù.
	type = 1 ;

	// 080626 LYW --- Hero : ù?ù ùùùù ùùùù ùù?ùù ùùùù ùùùù ùùùù Define ùùùùùù ùùùùùù.
	//if(GetLevel() == 99 || type == 0 )
	if(GetLevel() == MAX_CHARACTER_LEVEL_NUM || type == 0 )
		((CObjectGuagen*)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGEEXPPOINT))->SetValue( 1, 0 );
	else
		((CObjectGuagen*)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGEEXPPOINT))->SetValue( value, (DWORD)valueTime );

	// ùùùùùùù?ù : ?ùùùù ‚
	CObjectGuagen* ps = (CObjectGuagen *)GAMEIN->GetCharacterDialog()->GetWindowForID(CI_EXPPOINTGUAGE);
	if( ps )
	{
		if(type == 0)
			ps->SetValue( value, 0 );
		else
			ps->SetValue( value, (DWORD)valueTime );
	}	

	// 070113 LYW --- Delete this line.
	//GAMEIN->GetCharacterDialog()->SetExpPointPercent(value*100);
	// ùùùùùùù?ù : ùù? ?ùù

	// 080603 LYW --- Hero : ùùùù? Ûùù (__int32)ùùùù (__int64)ùù ùùùù Ûùù.
	//char szValue[50];
	//if(GetLevel() >= 99)
	//	sprintf( szValue, "[ %s ]", CHATMGR->GetChatMsg(179) );	//"[ ùù ]"
	//else
	//	sprintf( szValue, "%.2f%%", value*100 );

//	#ifdef _CHEATENABLE_	
//	if(CHEATMGR->IsCheatEnable())
//	{
//		char buf1[66];
//		wsprintf(buf1, " %d/%d", dwPoint, GetMaxExpPoint());
//		strcat(szValue, buf1);
//	}
//	#endif

	char szValue[50];
	sprintf( szValue, "%.2f%%", value*100 );

#ifdef _CHEATENABLE_	
	if(CHEATMGR->IsCheatEnable())
	{
		char buf1[66];
		EXPTYPE exp = GetMaxExpPoint() ;
 		sprintf(buf1, " %0.f/%0.f", (float)dwPoint, (float)exp);
		strcat(szValue, buf1);
	}
#endif

	cStatic* ps2 = (cStatic *)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGEEXPPOINTTEXT);
	if( ps2 )	ps2->SetStaticText( szValue );
}

void CHero::SetLevel(LEVELTYPE level)
{
	// ùùùùùùù?ù ‚ ?ùù ùù? ùùùù
	GAMEIN->GetCharacterDialog()->SetLevel(level);

	cStatic* ps = (cStatic *)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGELEVEL);
	char tempStr[ 128 ] = {0, } ;
	sprintf( tempStr, "%d", level ) ;
	if( ps ) ps->SetStaticText( tempStr ) ;

	CPlayer::SetLevel(level);
	ITEMMGR->RefreshAllItem();
}

void CHero::SetSkillPoint(DWORD point)
{
	m_HeroTotalInfo.SkillPoint += point;
	GAMEIN->GetSkillTrainingDlg()->SetSkillPoint();
	GAMEIN->GetSkillTreeDlg()->SetSkillPoint();
}

void CHero::SetMaxMana(DWORD val)
{
	m_HeroTotalInfo.MaxMana = val;

	// ùùùùùùù?ù ùùùùùù ùùùù
	DWORD neMaxMana = GetMaxMana();
	if(neMaxMana == 0)
	{
		neMaxMana = 1;
	}
	((CObjectGuagen*)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGEMANA))->SetValue(
		(float)m_HeroTotalInfo.Mana/(float)neMaxMana, 0 );

	char szValue[50] = {0};
	sprintf( szValue, "%d/%d", GetMana(), neMaxMana );
	cStatic* ps = (cStatic *)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGEMANATEXT);
	if( ps )	ps->SetStaticText( szValue );
}

// 100223 ShinJS --- ùùùù ùùùùùùùù ùùù?ù ùùùùùùùùùù ùù?ùùùùùù ù?ù ùùùù
void CHero::ChangeMana(int changeval)
{
	DWORD newMana = min( GetMaxMana(), GetMana() + changeval );
	SetMana( newMana );
}

void CHero::SetMana(DWORD val, BYTE type)
{
	CPlayer::SetMana(val);

	if(m_HeroTotalInfo.MaxMana != 0)
	{
		((CObjectGuagen*)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGEMANA))->SetValue(
			(float)val/(float)GetMaxMana(), 
			1000);
	}

	// ùùùùùùù?ù : ùù? ?ùù
	char szValue[50];
	sprintf( szValue, "%d/%d", val, GetMaxMana() );
	cStatic* ps = (cStatic *)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGEMANATEXT);
	if( ps )	ps->SetStaticText( szValue );

	// ùù?ùùùù
	m_HeroTotalInfo.Mana = val;
}
void CHero::SetStrength(DWORD val)
{
	m_HeroTotalInfo.Str = val;

	//SW060906 ùùù?ùùùùù ùùùù
	CHARCALCMGR->CalcCharStats(this);
	//CHARCALCMGR->CalcCharStats(this);

	// ùùùùùùù?ù ‚ ?ùù ùù? ùùùù
	GAMEIN->GetCharacterDialog()->SetStrength();
	// 070411 LYW --- Hero : Modified function SetStrength.
	//GAMEIN->GetCharacterDialog()->SetAttackRate();
	GAMEIN->GetCharacterDialog()->PrintAttackRate();
	GAMEIN->GetCharacterDialog()->PrintAccuracyRate();
	GAMEIN->GetCharacterDialog()->UpdateData();
}
void CHero::SetDexterity(DWORD val)
{
	m_HeroTotalInfo.Dex = val;

	CHARCALCMGR->CalcCharStats(this);
	//CHARCALCMGR->CalcCharStats(this);


	// ùùùùùùù?ù ‚ ?ùù ùù? ùùùù
	GAMEIN->GetCharacterDialog()->SetDexterity();
	// 070411 LYW --- Hero : Modified function SetDexterity.
	//GAMEIN->GetCharacterDialog()->SetAttackRate();
	GAMEIN->GetCharacterDialog()->PrintAccuracyRate();
	GAMEIN->GetCharacterDialog()->PrintEvasionRate();
	GAMEIN->GetCharacterDialog()->PrintCriticalRate();

	GAMEIN->GetCharacterDialog()->UpdateData();
}
void CHero::SetVitality(DWORD val)
{
	m_HeroTotalInfo.Vit = val;

	CHARCALCMGR->CalcCharStats(this);
	//CHARCALCMGR->CalcCharStats(this);

	// ùùùùùùù?ù ‚ ?ùù ùù? ùùùù
	GAMEIN->GetCharacterDialog()->SetVitality();
	// 070411 LYW --- Hero : Modified function SetVitality.
	//GAMEIN->GetCharacterDialog()->SetDefenseRate();
	GAMEIN->GetCharacterDialog()->PrintDefenseRate();

	GAMEIN->GetCharacterDialog()->UpdateData();
}
void CHero::SetWisdom(DWORD val)
{
	m_HeroTotalInfo.Wis = val;

	CHARCALCMGR->CalcCharStats(this);
	//CHARCALCMGR->CalcCharStats(this);

	// ùùùùùùù?ù ‚ ?ùù ùù? ùùùù
	GAMEIN->GetCharacterDialog()->SetWisdom();
	GAMEIN->GetCharacterDialog()->UpdateData();
	GAMEIN->GetCharacterDialog()->PrintMagDefenseRate() ;
	GAMEIN->GetCharacterDialog()->PrintMagicCriticalRate() ;
	GAMEIN->GetCharacterDialog()->UpdateData();
}

// 070411 LYW --- Hero : Add function to setting intelligence to hero.
void CHero::SetIntelligence( DWORD val )
{
	m_HeroTotalInfo.Int = val;

	CHARCALCMGR->CalcCharStats(this);

	// ùùùùùùù?ù ‚ ?ùù ùù? ùùùù
	GAMEIN->GetCharacterDialog()->SetIntelligence();
	GAMEIN->GetCharacterDialog()->UpdateData();
	GAMEIN->GetCharacterDialog()->PrintMagAttackRate() ;
	GAMEIN->GetCharacterDialog()->PrintMagicCriticalRate() ;
	GAMEIN->GetCharacterDialog()->UpdateData();
}

void CHero::SetBadFame( FAMETYPE val )
{
	m_CharacterInfo.BadFame = val;
	GAMEIN->GetCharacterDialog()->SetBadFame( val );	
}

BOOL CHero::StartSocietyAct( int nStartMotion, int nIngMotion, int nEndMotion, BOOL bHideWeapon )
{
	if( GetState() != eObjectState_None && GetState() != eObjectState_Immortal )
	{
		if( GetState() == eObjectState_Society )
		{
			if( m_ObjectState.State_End_Motion != -1 )	//ù?ù ùùùùùù ùùù
			{
				if( m_ObjectState.bEndState == TRUE )
					return FALSE;
				if( gCurTime - GetStateStartTime() < GetEngineObject()->GetAnimationTime(m_ObjectState.State_Start_Motion ) )
					return FALSE;

				OBJECTSTATEMGR->EndObjectState( this, GetState(), GetEngineObject()->GetAnimationTime( m_ObjectState.State_End_Motion ) );
				return TRUE;
			}
		}
		else
		{
			return FALSE;
		}
	}

	m_ObjectState.State_Start_Motion	= (short int)nStartMotion;
	m_ObjectState.State_Ing_Motion		= (short int)nIngMotion;
	m_ObjectState.State_End_Motion		= (short int)nEndMotion;
	m_ObjectState.MotionSpeedRate = 1.0f;

	if( nEndMotion != -1 )
		m_ObjectState.State_End_MotionTime = GetEngineObject()->GetAnimationTime(nEndMotion);
	else
		m_ObjectState.State_End_MotionTime = 0;


	if(m_ObjectState.State_Start_Motion != -1)
	{
		ChangeMotion( m_ObjectState.State_Start_Motion, FALSE );

		if( m_ObjectState.State_Ing_Motion != -1 )
			ChangeBaseMotion( m_ObjectState.State_Ing_Motion );
	}


	if( bHideWeapon )
		APPEARANCEMGR->HideWeapon( this );

	m_BaseObjectInfo.ObjectState	= eObjectState_Society;
	m_ObjectState.State_Start_Time	= gCurTime;
	m_ObjectState.bEndState			= FALSE;

	if( m_ObjectState.State_Ing_Motion == -1 )
		OBJECTSTATEMGR->EndObjectState( this, eObjectState_Society, GetEngineObject()->GetAnimationTime( m_ObjectState.State_Start_Motion ) );

	return TRUE;
}

BOOL CHero::EndSocietyAct()
{
	if( m_ObjectState.State_Ing_Motion != -1 )
	{
		return FALSE;
	}
	return TRUE;
}

void CHero::StartSocietyActForQuest( int nStartMotion, int nIngMotion /* = -1 */, int nEndMotion /* = -1 */, BOOL bHideWeapon /* = FALSE  */ )
{
	m_ObjectState.State_Start_Motion = (short int)nStartMotion;
	m_ObjectState.State_Ing_Motion = (short int)nIngMotion;
	m_ObjectState.State_End_Motion = (short int)nEndMotion;
	m_ObjectState.MotionSpeedRate = 1.0f;

	if( nEndMotion != -1 )
		m_ObjectState.State_End_MotionTime = GetEngineObject()->GetAnimationTime(nEndMotion);
	else
		m_ObjectState.State_End_MotionTime = 0;

	if( m_ObjectState.State_Start_Motion != -1 )
	{
		ChangeMotion( m_ObjectState.State_Start_Motion, FALSE );
		if( m_ObjectState.State_Ing_Motion != -1 )
			ChangeBaseMotion( m_ObjectState.State_Ing_Motion );
	}

	if( bHideWeapon )
		APPEARANCEMGR->HideWeapon( this );
}

#ifdef _GMTOOL_
const char* GetStateName( BYTE state )
{
	static char StateName[eObjectState_MAX][64] = {
	"eObjectState_None",						
	"eObjectState_Enter",						
	"eObjectState_Move",						
	"eObjectState_Ungijosik",					
	"eObjectState_Rest",						
	"eObjectState_Deal",						
	"eObjectState_Exchange",					
	"eObjectState_StreetStall_Owner",			
	"eObjectState_StreetStall_Guest",			
	"eObjectState_PrivateWarehouse",			
	"eObjectState_Munpa",						
	"eObjectState_SkillStart",				
	"eObjectState_SkillSyn",					
	"eObjectState_SkillBinding",				
	"eObjectState_SkillUsing",				
	"eObjectState_SkillDelay",				
	"eObjectState_TiedUp_CanMove",			
	"eObjectState_TiedUp_CanSkill",
	"eObjectState_TiedUp",
	"eObjectState_Die",						
	"eObjectState_BattleReady",
	"eObjectState_Exit",						
	"eObjectState_Immortal",					
	"eObjectState_Society",					
	"eObjectState_ItemUse",
	"eObjectState_TournamentReady",
	"eObjectState_TournamentProcess",
	"eObjectState_TournamentEnd",
	"eObjectState_TournamentDead",
	"eObjectState_Engrave",
	"eObjectState_RestDamage",
	"eObjectState_Connect"
	};

	return StateName[state];
}
#endif

void CHero::OnStartObjectState(EObjectState State)
{
	switch(State)
	{
	case eObjectState_Move:
		{
			if(IsGetOnVehicle())
			{
				break;
			}

			SNDIDX walkSound = 1254;
			SNDIDX runSound = 1255;

			if(const ITEM_INFO* const itemInfo = ITEMMGR->GetItemInfo(GetWearedItemIdx(eWearedItem_Costume_Wing) != NULL && GetMoveType() ? GetWearedItemIdx(eWearedItem_Costume_Wing) : GetWearedItemIdx(eWearedItem_Wing)))
			{
				if(ePartType_Wing == itemInfo->Part3DType)
				{
					walkSound = 1342;
					runSound = 1343;
				}
			}

			mMoveSound = AUDIOMGR->Play(
				eMoveMode_Run == m_MoveInfo.MoveMode ? runSound : walkSound,
				GetID());
			break;
		}
	}

	CPlayer::OnStartObjectState(State);
}

void CHero::OnEndObjectState(EObjectState State)
{
	switch(State)
	{
	case eObjectState_Enter:
		{
		}
		break;
	case eObjectState_Deal:
		{
			HERO->HeroStateNotify(eObjectState_Deal);
			GAMEIN->GetNpcImageDlg()->SetDisable( FALSE );
			//GAMEIN->GetNpcImageDlg()->SetActive( FALSE );
		}
		break;

	case eObjectState_SkillUsing:
	case eObjectState_SkillBinding:
		SKILLMGR->OnComboTurningPoint(this);
		break;

	case eObjectState_SkillSyn:
		{
#ifndef _TESTCLIENT_
//			if(SKILLMGR->GetSkillObject(TEMP_SKILLOBJECT_ID) != NULL)	// ùùùùùùùù ùùùùùù ù?∞ùù
//				OBJECTSTATEMGR->StartObjectState(this,eObjectState_SkillUsing);
//			else				
				SKILLMGR->OnComboTurningPoint(this);
#else
			SKILLMGR->OnComboTurningPoint(this);
#endif
		}
		break;

	case eObjectState_Move:
		{
			AUDIOMGR->Stop(
				mMoveSound);
		}
		break;
	case eObjectState_SkillStart:
		{
			if(m_SkillStartAction.GetActionKind() != eActionKind_Skill_RealExecute)
				break;

			m_SkillStartAction.ExcuteAction(this);
			m_SkillStartAction.Clear();
		}
		break;
	}
	CPlayer::OnEndObjectState(State);
}

//////////////////////////////////////////////////////////////////////////
// ù? ù?ùùù..
void CHero::Die(CObject* pAttacker,BOOL bFatalDamage,BOOL bCritical, BOOL bDecisive)
{
	//	ASSERT(GetLife() == 0);

	WINDOWMGR->CloseAllMsgBox();

	GAMEIN->GetDealDialog()->SetDisable(FALSE);
	GAMEIN->GetDealDialog()->SetActive(FALSE);	// ùùùù‚ ù?ù

	GAMEIN->GetNpcScriptDialog()->SetDisable(FALSE);	//NPC‚ùù ù?ù
	GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);

	// ùùù??ùùù ‚ ù?ù
	{
		CProgressDialog* dialog = ( CProgressDialog* )WINDOWMGR->GetWindowForID( PROGRESS_DIALOG );

		if( dialog->IsActive() )
		{
			dialog->Cancel();
		}
	}

	SetMovingAction(NULL);
	DisableAutoAttack();					// ùùùùùùù ùùùù
	SetLife(0);

	const eWeaponType weapon = GetWeaponEquipType();

	DWORD time = 0;
	if( bFatalDamage )
	{
		time = HERO->GetEngineObject()->GetAnimationTime( CHARACTER_MOTION[ eCharacterMotion_Died2 ][ weapon ] );
	}
	else
	{
		time = HERO->GetEngineObject()->GetAnimationTime( CHARACTER_MOTION[ eCharacterMotion_Died1 ][ weapon ] );
	}
	CAMERA->SetCharState( eCS_Die, time );

	CPlayer::Die(pAttacker,bFatalDamage,bCritical,bDecisive);
	GAMEIN->GetCharacterDialog()->SetAttackRate();
	GAMEIN->GetCharacterDialog()->SetDefenseRate();
	GAMEIN->GetCharacterDialog()->UpdateData();
	// 090907 ShinJS --- ùùùù ùùù WayPointùù ùùùùù?ù
	ClearWayPoint();
}

// 080910 LUJ, ùùùùùù ùùùù Ûùùùù RESULTINFO ùùùù¸ùù ù?ù Ûùùù?ùùù ùù
void CHero::Damage(CObject* pAttacker,BYTE DamageKind,DWORD Damage,const RESULTINFO& result)
{
	//////////////////////////////////////////////////////////////////////////
	// ?ù?ù ù?ù ù?ù
	VECTOR3 push;
	MATRIX4 my;
	SetVector3(&push,0,0,30);
	SetRotationYMatrix(&my,-m_RotateInfo.Angle.ToRad());
	TransformVector3_VPTR2(&push,&push,&my,1);
	CAMERA->SetPushedVibration(&push,200);
	
	if( pAttacker )
	{
		if( pAttacker->GetObjectKind() == eObjectKind_Player )
		{
			PKMGR->SetPKStartTimeReset();
		}

		// ù?ù ù?ù ùùù?ùùù ùùùùùù ù??ùùù ùùù?ùù?ù. ù?ùùù ùùùùùù ùùùù?ùù ù?ùù?ù
		if(0 == OBJECTMGR->GetSelectedObject() &&
			FALSE == GetBaseMoveInfo()->bMoving)
		{
			VECTOR3 position = pAttacker->GetCurPosition() - GetCurPosition();
			GetEngineObject()->SetDirection(
				&position);
			OBJECTMGR->SetSelectedObject(
				pAttacker);
		}
	}
	
	CPlayer::Damage(pAttacker,DamageKind,Damage,result);
}

void CHero::OnHitTarget(CObject* pMainTarget)
{
	//////////////////////////////////////////////////////////////////////////
	// ùùù?ùùù ùùùùùù ù?ù ùùùùüù?ù ùùùùùù ùùù?ù ùùùù?ùù	
	VECTOR3 targetpos = pMainTarget->GetCurPosition();
	VECTOR3 Curpos = HERO->GetCurPosition();

	float WeaponMinDistance = 100;
	switch(GetWeaponEquipType())
	{
	case eWeaponType_Sword	:	WeaponMinDistance = 150;	break;
	case eWeaponType_Mace	:	WeaponMinDistance = 150;	break;
	case eWeaponType_Axe	:	WeaponMinDistance = 150;	break;
	case eWeaponType_Dagger	:	WeaponMinDistance = 100;	break;
	case eWeaponType_Staff	:	WeaponMinDistance = 150;	break;
	case eWeaponType_Bow	:	WeaponMinDistance = 150;	break;	
	case eWeaponType_Gun	:	WeaponMinDistance = 100;	break;	
		// desc_hseos_ASSERTùùùù_01
		// S ASSERTùùùù ù?ù added by hseos 2007.06.02
		// ..ù?ùùùùù ùùùùùù ùùùù ùùùùùù ù???ù ù?ù
	case eWeaponType_None	:								break;
		// E ASSERTùùùù ù?ù added by hseos 2007.06.02
	default: ASSERT(0);
	}

	if(CalcDistanceXZ(&targetpos,&Curpos) < WeaponMinDistance)
	{
		VECTOR3 pos;	pos.x = 0;	pos.y = 0;	pos.z = 10.f;
		float angle = HERO->GetAngle();
		TransToRelatedCoordinate(&pos,&pos,angle);
		pos = pos + Curpos;
		MOVEMGR->HeroEffectMove(HERO,&pos,0,0);
	}
}


void CHero::ClearTarget(CObject* pObject)
{
	if(m_AutoAttackTarget.GetTargetID() == pObject->GetID())
		DisableAutoAttack();
	if(m_NextAction.GetTargetID() == pObject->GetID())
	{
		m_NextAction.Clear();
	}
}


void CHero::Revive(VECTOR3* pRevivePos)
{
	DisableAutoAttack();

	CAMERA->SetCharState( eCS_Normal, 0 );	//ù??ùù ùùùùùù ùùùù.

	CPlayer::Revive(pRevivePos);
}

DWORD CHero::DoGetAttackRate() 
{
	DWORD wWeaponAttack		= DWORD( mItemStat.mPhysicAttack.mPlus * ( 1.0f + mItemStat.mPhysicAttack.mPercent ) );

	DWORD wEnchantAttack	= GetWeaponEnchantLevel();
	DWORD wLevel			= GetLevel() ;
	float wPassiveSkill		= GetPassiveStatus()->PhysicAttack;
	float fAttackA			= (float)( wWeaponAttack + wEnchantAttack + wLevel + wPassiveSkill + 3) ;
	float wStrengthAll		= ( mCharStat.mStrength.mPlus + mItemStat.mStrength.mPlus + GetBuffStatus()->Str + GetPassiveStatus()->Str ) * ( 1.0f + mItemStat.mStrength.mPercent + GetRateBuffStatus()->Str + GetRatePassiveStatus()->Str/100 );

	{
		const PlayerStat::Value& value = GetSetItemStats().mStrength;

		wStrengthAll =	
			max( 0, ( wStrengthAll + value.mPlus ) * ( 1.0f + value.mPercent ) );
	}

	wStrengthAll *= 0.2f;

	float wMasteryLevel		= 0 ;
	float wMasteryLevelRate = 0 ;

	switch( GetWeaponAniType() )
	{ 
		case eWeaponAnimationType_OneHand:	wMasteryLevel = GetPassiveStatus()->OneHanded; wMasteryLevelRate = GetRatePassiveStatus()->OneHanded;	break;
		case eWeaponAnimationType_TwoHand:	wMasteryLevel = GetPassiveStatus()->TwoHanded; wMasteryLevelRate =  GetRatePassiveStatus()->TwoHanded;	break;
		case eWeaponAnimationType_TwoBlade:	wMasteryLevel = GetPassiveStatus()->TwoBlade; wMasteryLevelRate = GetRatePassiveStatus()->TwoBlade;		break;
	}

	switch( GetWeaponEquipType() )
	{
		case eWeaponType_Sword:		wMasteryLevel += GetPassiveStatus()->Sword; wMasteryLevelRate += GetRatePassiveStatus()->Sword;	break;
		case eWeaponType_Mace:		wMasteryLevel += GetPassiveStatus()->Mace; wMasteryLevelRate += GetRatePassiveStatus()->Mace;	break;
		case eWeaponType_Axe:		wMasteryLevel += GetPassiveStatus()->Axe; wMasteryLevelRate += GetRatePassiveStatus()->Axe;		break;
		case eWeaponType_Dagger:	wMasteryLevel = GetPassiveStatus()->Dagger; wMasteryLevelRate = GetRatePassiveStatus()->Dagger;	break;
		case eWeaponType_Staff:		wMasteryLevel = GetPassiveStatus()->Staff; wMasteryLevelRate = GetRatePassiveStatus()->Staff;	break;
		case eWeaponType_Bow:		wMasteryLevel = GetPassiveStatus()->Bow; wMasteryLevelRate = GetRatePassiveStatus()->Bow;		break;
		case eWeaponType_Gun:		wMasteryLevel = GetPassiveStatus()->Gun; wMasteryLevelRate = GetRatePassiveStatus()->Gun;		break;
	}

	float wAttackBuff		= GetBuffStatus()->PhysicAttack;
	float wAttackBuffRate	= GetRateBuffStatus()->PhysicAttack;
	float wPassiveSkillRate	= GetRatePassiveStatus()->PhysicAttack;
	float fAttackB			= 1000 + (((wStrengthAll -10) * 20) + (wMasteryLevel * 25) + (wAttackBuff * 10)) ;
	float fAttackC			= wPassiveSkillRate + wMasteryLevelRate + wAttackBuffRate;

	DWORD dwTotalAttack		= (DWORD)GetFloatRound( (fAttackA * fAttackB) / 1000 ); 

	{
		const PlayerStat::Value& setValue = GetSetItemStats().mPhysicAttack;
		dwTotalAttack = DWORD(( max( 0, ( dwTotalAttack + setValue.mPlus ) * ( 1.0f + setValue.mPercent ) ) ));
	}

	dwTotalAttack = (DWORD)GetFloatRound( dwTotalAttack * ( 1 + fAttackC / 100 ));

	return dwTotalAttack;
}

DWORD CHero::DoGetDefenseRate() 
{
	DWORD wDefenseAll		= DWORD( mItemStat.mPhysicDefense.mPlus * ( 1.0f + mItemStat.mPhysicDefense.mPercent ) );
	DWORD wEnchantAll		= GetPhysicDefenseEnchantLevel();
	DWORD wLevel			= GetLevel() ;
	float wPassiveSkill		= GetPassiveStatus()->PhysicDefense;

	DWORD Armor = GetWearedItemIdx( eWearedItem_Dress );
	DWORD Boots = GetWearedItemIdx( eWearedItem_Shoes );
	DWORD Glove = GetWearedItemIdx( eWearedItem_Glove );

	ITEM_INFO* pArmor = ITEMMGR->GetItemInfo( Armor );
	ITEM_INFO* pBoots = ITEMMGR->GetItemInfo( Boots );
	ITEM_INFO* pGlove = ITEMMGR->GetItemInfo( Glove );

	m_bNoDeffenceFromArmor = FALSE;

	if( m_CharacterInfo.Job[0] == 2 )
	{
		if( ( pArmor && pArmor->ArmorType == eArmorType_Metal ) ||
			( pBoots && pBoots->ArmorType == eArmorType_Metal ) ||
			( pGlove && pGlove->ArmorType == eArmorType_Metal ) )
		{
			if( GetPassiveStatus()->HeavyArmor * ( 1.0f + GetRatePassiveStatus()->HeavyArmor ) < 0.1f )
			{
				wDefenseAll = 0;
				m_bNoDeffenceFromArmor = TRUE;
			}
		}
	}
	else if( m_CharacterInfo.Job[0] == 3 )
	{
		if( ( pArmor && pArmor->ArmorType == eArmorType_Metal ) ||
			( pBoots && pBoots->ArmorType == eArmorType_Metal ) ||
			( pGlove && pGlove->ArmorType == eArmorType_Metal ) )
		{
			if( GetPassiveStatus()->HeavyArmor * ( 1.0f + GetRatePassiveStatus()->HeavyArmor ) < 0.1f )
			{
				wDefenseAll = 0;
				m_bNoDeffenceFromArmor = TRUE;
			}
		}
		else if( ( pArmor && pArmor->ArmorType == eArmorType_Leather ) ||
				 ( pBoots && pBoots->ArmorType == eArmorType_Leather ) ||
				 ( pGlove && pGlove->ArmorType == eArmorType_Leather ) )
		{
			if( GetPassiveStatus()->LightArmor * ( 1.0f + GetRatePassiveStatus()->LightArmor ) < 0.1f )
			{
				wDefenseAll = 0;
				m_bNoDeffenceFromArmor = TRUE;
			}
		}
	}	

	float wVitalityAll		= ( mCharStat.mVitality.mPlus + mItemStat.mVitality.mPlus + GetBuffStatus()->Vit + GetPassiveStatus()->Vit ) * ( 1.0f + mItemStat.mVitality.mPercent + GetRateBuffStatus()->Vit + GetRatePassiveStatus()->Vit/100);
	
	{
		const PlayerStat::Value& value = GetSetItemStats().mVitality;

		wVitalityAll =	
			max( 0, ( wVitalityAll + value.mPlus ) * ( 1.0f + value.mPercent ) );
	}

	float wMasteryLevel		= 0 ;
	float wMasteryLevelRate = 0 ;
	wVitalityAll *= 0.2f;

	if( pArmor && pBoots && pGlove )
	{
		if( ( pArmor->ArmorType == pBoots->ArmorType ) && ( pArmor->ArmorType == pGlove->ArmorType ) )
		{
			switch( pArmor->ArmorType )
			{
				case eArmorType_Robe:		wMasteryLevel = GetPassiveStatus()->Robe; wMasteryLevelRate = GetRatePassiveStatus()->Robe;				break;
				case eArmorType_Leather:	wMasteryLevel = GetPassiveStatus()->LightArmor; wMasteryLevelRate = GetRatePassiveStatus()->LightArmor;	break;
				case eArmorType_Metal:		wMasteryLevel = GetPassiveStatus()->HeavyArmor; wMasteryLevelRate = GetRatePassiveStatus()->HeavyArmor;	break;
			}
		}
	}

	float wDefenseBuff		= GetBuffStatus()->PhysicDefense;
	float wDefenseBuffRate	= GetRateBuffStatus()->PhysicDefense;
	float wPassiveSkillRate = GetRatePassiveStatus()->PhysicDefense;
	float fDefenseA			= float(wDefenseAll / 2) + wEnchantAll + wLevel + wPassiveSkill ;
	float fValA				= (((float)wVitalityAll - 20) * 10) ;
	float fValB				= (float)(wMasteryLevel * 30) ;
	float fValC				= (float)(wDefenseBuff * 10) ;
	float fDefenseB			= 500 + (fValA + fValB + fValC) ;
	float fDefenseC			= wDefenseBuffRate + wPassiveSkillRate + wMasteryLevelRate;
	float fResult			= (fDefenseA * fDefenseB) / 900;

	float dwTotalDefense = 0.0f ;
	if( fResult <= 0 )
	{
		dwTotalDefense = 0 ;
	}
	else
	{
		dwTotalDefense	= GetFloatRound( fResult ) ;
	}

	{
		const PlayerStat::Value& setValue = GetSetItemStats().mPhysicDefense;
		dwTotalDefense = max( 0, ( dwTotalDefense + setValue.mPlus ) * ( 1.0f + setValue.mPercent ) );
	}

	dwTotalDefense = GetFloatRound( dwTotalDefense * ( 1 + fDefenseC / 100 ));


	return (DWORD)dwTotalDefense;
} 


DWORD CHero::DoGetMagAttackRate() 
{
	DWORD wMagicAttack		= DWORD( mItemStat.mMagicAttack.mPlus * ( 1.0f + mItemStat.mMagicAttack.mPercent ) );
	DWORD wEnchantAttack	= GetWeaponEnchantLevel() ;
	DWORD wLevel			= GetLevel() ;
	float wPassiveSkill		= GetPassiveStatus()->MagicAttack;
	float wIntelligenceAll	= ( mCharStat.mIntelligence.mPlus + mItemStat.mIntelligence.mPlus + GetBuffStatus()->Int + GetPassiveStatus()->Int ) * ( 1.0f + mItemStat.mIntelligence.mPercent + GetRateBuffStatus()->Int + GetRatePassiveStatus()->Int/100 );

	{
		const PlayerStat::Value& value = GetSetItemStats().mIntelligence;

		wIntelligenceAll =	
			float( max( 0, ( wIntelligenceAll + value.mPlus ) * ( 1.0f + value.mPercent ) ) );
	}

	wIntelligenceAll *= 0.2f; 

	float wAttackBuff		= GetBuffStatus()->MagicAttack;
	float wAttackBuffRate	= GetRateBuffStatus()->MagicAttack;
	float wPassiveSkillRate = GetRatePassiveStatus()->MagicAttack;
	float fAttackA			= (float)(wMagicAttack + wEnchantAttack + wLevel + wPassiveSkill) ;
	float fValA				= ((float)wIntelligenceAll -10) * 35 ;
	float fValB				= (float)(wAttackBuff * 10) ;
	float fAttackB			= 1000 + (fValA + fValB) ;
	float fAttackC			= wAttackBuffRate + wPassiveSkillRate; 
	float fResult			= (fAttackA * fAttackB) / 350 ;
	float dwTotalAttack		= GetFloatRound( fResult ) ;

	{
		const PlayerStat::Value& setValue = GetSetItemStats().mMagicAttack;
		dwTotalAttack = max( 0, ( dwTotalAttack + setValue.mPlus ) * ( 1.0f + setValue.mPercent ) );
	}

	dwTotalAttack = GetFloatRound( (dwTotalAttack)* ( 1 + fAttackC / 100 ));
	dwTotalAttack *= 0.7f;


	return DWORD( dwTotalAttack );
}

DWORD CHero::DoGetMagDefenseRate() 
{
	DWORD wMagDefenseAll	= DWORD( mItemStat.mMagicDefense.mPlus * ( 1.0f + mItemStat.mMagicDefense.mPercent ) );
	DWORD wEnchantAll		= GetMagicDefenseEnchantLevel() ;
	DWORD wLevel			= GetLevel() ;
	float wPassiveSkill		= GetPassiveStatus()->MagicDefense;
	float wWisdomAll		= ( mCharStat.mWisdom.mPlus + mItemStat.mWisdom.mPlus + GetBuffStatus()->Wis + GetPassiveStatus()->Wis ) * ( 1.0f + mItemStat.mWisdom.mPercent + GetRateBuffStatus()->Wis + GetRatePassiveStatus()->Wis/100 );

	{
		const PlayerStat::Value& value = GetSetItemStats().mWisdom;

		wWisdomAll =	
			float( max( 0, ( wWisdomAll + value.mPlus ) * ( 1.0f + value.mPercent ) ) );
	}

	wWisdomAll *= 0.2f;

	float wDefenseBuff		= GetBuffStatus()->MagicDefense;
	float wDefenseBuffRate	= GetRateBuffStatus()->MagicDefense;
	float wPassiveSkillRate	= GetRatePassiveStatus()->MagicDefense;
	float fDefenseA			= float(wMagDefenseAll / 5) + wEnchantAll + wLevel + wPassiveSkill ;
	float fValA				= ((float)wWisdomAll - 20) * 15 ;
	float fValB				= (float)wDefenseBuff * 10 ; 
	float fDefenseB			= 500 + (fValA + fValB) ;
	float fDefenseC			= wDefenseBuffRate + wPassiveSkillRate;
	float fResult			= (fDefenseA * fDefenseB) / 200 ;
	float dwTotalDefense	= GetFloatRound( fResult ) ;

	{
		const PlayerStat::Value& setValue = GetSetItemStats().mMagicDefense;
		dwTotalDefense = max( 0, ( dwTotalDefense + setValue.mPlus ) * ( 1.0f + setValue.mPercent ) );
	}

	dwTotalDefense = GetFloatRound( dwTotalDefense * ( 1 + fDefenseC / 100 ));


	return DWORD( dwTotalDefense );
}

DWORD CHero::DoGetAccuracyRate() 
{
	BYTE byJobOrder			= m_CharacterInfo.Job[0] ;
	DWORD wLevel			= GetLevel() ;
	float wDexterityAll		= ( mCharStat.mDexterity.mPlus + mItemStat.mDexterity.mPlus + GetBuffStatus()->Dex + GetPassiveStatus()->Dex ) * ( 1.0f + mItemStat.mDexterity.mPercent + GetRateBuffStatus()->Dex + GetRatePassiveStatus()->Dex/100 );
	float wStrengthAll		= ( mCharStat.mStrength.mPlus + mItemStat.mStrength.mPlus + GetBuffStatus()->Str + GetPassiveStatus()->Str ) * ( 1.0f + mItemStat.mStrength.mPercent + GetRateBuffStatus()->Str + GetRatePassiveStatus()->Str/100 );
	
	{
		const PlayerStat::Value& value = GetSetItemStats().mDexterity;

		wDexterityAll =	
			float( max( 0, ( wDexterityAll + value.mPlus ) * ( 1.0f + value.mPercent ) ) );
	}

	wDexterityAll *= 0.2f;
	wStrengthAll *= 0.2f;

	DWORD wClassOrderVal	= 0 ;
	float wPassiveSkill		= GetPassiveStatus()->Accuracy;
	float wPassiveSkillRate	= GetRatePassiveStatus()->Accuracy;
	float wBuffSkill		= GetBuffStatus()->Accuracy;
	float wBuffSkillRate	= GetRateBuffStatus()->Accuracy;

	switch( byJobOrder )
	{
		case 1 : wClassOrderVal = 20 ; break ;
		case 2 : wClassOrderVal = 25 ; break ;
		case 3 : wClassOrderVal = 15 ; break ;
		case 4 : wClassOrderVal = 30 ; break ;
	}

	float fValA				= ((float)wDexterityAll / 2) ;
	float fValC				= ((float)wStrengthAll / 4) ;
	float fValB				= (float)(fValA + wLevel + wClassOrderVal + fValC);
	float fValD				= wPassiveSkillRate + wBuffSkillRate;
	float wAccuracy			= GetFloatRound(fValB) ;
	float dwTotalAccuracy	= wAccuracy + wPassiveSkill + wBuffSkill;

	{
		const PlayerStat::Value& value = GetSetItemStats().mAccuracy;
		dwTotalAccuracy = max( 0, ( dwTotalAccuracy + value.mPlus ) * ( 1.0f + value.mPercent ) );
	}

	{
		const PlayerStat::Value& value = mItemStat.mAccuracy;
		dwTotalAccuracy = ( dwTotalAccuracy + value.mPlus ) * ( 1.0f + value.mPercent );
	}

	dwTotalAccuracy = GetFloatRound( dwTotalAccuracy * ( 1 + fValD / 100 ));


	return DWORD( dwTotalAccuracy );
}

DWORD CHero::DoGetEvasionRate() 
{
	BYTE byJobOrder			= m_CharacterInfo.Job[0] ;
	DWORD wLevel			= GetLevel() ;
	float wDexterityAll		= ( mCharStat.mDexterity.mPlus + mItemStat.mDexterity.mPlus + GetBuffStatus()->Dex + GetPassiveStatus()->Dex ) * ( 1.0f + mItemStat.mDexterity.mPercent + GetRateBuffStatus()->Dex + GetRatePassiveStatus()->Dex/100 );

	{
		const PlayerStat::Value& value = GetSetItemStats().mDexterity;

		wDexterityAll =	
			float( max( 0, ( wDexterityAll + value.mPlus ) * ( 1.0f + value.mPercent ) ) );
	}

	wDexterityAll *= 0.2f;

	DWORD wClassOrderVal	= 0 ;
	float wPassiveSkill		= GetPassiveStatus()->Avoid;
	float wPassiveSkillRate	= GetRatePassiveStatus()->Avoid;
	float wBuffSkill		= GetBuffStatus()->Avoid;
	float wBuffSkillRate	= GetRateBuffStatus()->Avoid;

	switch( byJobOrder )
	{
		case 1 : wClassOrderVal = 20 ; break ;
		case 2 : wClassOrderVal = 25 ; break ;
		case 3 : wClassOrderVal = 15 ; break ;
		case 4 : wClassOrderVal = 30 ; break ;
	}

	float fValA				= ((float)wDexterityAll / 2) ;
	float fValB				= (float)(fValA + wLevel + wClassOrderVal);
	float fValC				= wPassiveSkillRate + wBuffSkillRate;
	float wEvasion			= GetFloatRound(fValB) ;
	float dwTotalEvasion	= wEvasion + wPassiveSkill + wBuffSkill;

	{
		const PlayerStat::Value& value = GetSetItemStats().mEvade;
		dwTotalEvasion = max( 0, ( dwTotalEvasion + value.mPlus ) * ( 1.0f + value.mPercent ) );
	}

	{
		const PlayerStat::Value& value = mItemStat.mEvade;
		dwTotalEvasion = ( dwTotalEvasion + value.mPlus ) * ( 1.0f + value.mPercent );
	}

	dwTotalEvasion = GetFloatRound( dwTotalEvasion * ( 1 + fValC / 100 ));

	return DWORD( dwTotalEvasion );
}

DWORD CHero::DoGetCriticalDamage() 
{
	float wPassiveSkill		= GetPassiveStatus()->CriticalDamage;
	float wPassiveSkillRate = GetRatePassiveStatus()->CriticalDamage;
	float wBuffSkill		= GetBuffStatus()->CriticalDamage;
	float wBuffSkillRate	= GetRateBuffStatus()->CriticalDamage;

	float wCriticalDamageBuff	= (wBuffSkillRate > 0 ? (1-wBuffSkillRate) : 0) ;
	float wCriticalDamagePassive	= (wPassiveSkillRate > 0 ? (1-wPassiveSkillRate) : 0);
	float dwCriticalDamage	= wPassiveSkill + wBuffSkill;

	float	fPercent	= (GetSetItemStats().mCriticalDamage.mPercent > 0 ? 1-GetSetItemStats().mCriticalDamage.mPercent: 0);


	float itemPercent = GetItemStats().mCriticalDamage.mPercent;

	dwCriticalDamage = 1 - (1-wCriticalDamageBuff) * (1-wCriticalDamagePassive) * (1-fPercent) * (1-itemPercent);

	return (DWORD)(dwCriticalDamage * 100);
}

DWORD CHero::DoGetCriticalRate() 
{
	DWORD wLevel			= GetLevel() ;
	float wDexterityAll		= ( mCharStat.mDexterity.mPlus + mItemStat.mDexterity.mPlus + GetBuffStatus()->Dex + GetPassiveStatus()->Dex ) * ( 1.0f + mItemStat.mDexterity.mPercent + GetRateBuffStatus()->Dex + GetRatePassiveStatus()->Dex/100 );

	{
		const PlayerStat::Value& value = GetSetItemStats().mDexterity;

		wDexterityAll =	
			float( max( 0, ( wDexterityAll + value.mPlus ) * ( 1.0f + value.mPercent ) ) );
	}

	wDexterityAll *= 0.2f;

	float wPassiveSkill		= GetPassiveStatus()->CriticalRate;
	float wPassiveSkillRate	= GetRatePassiveStatus()->CriticalRate;
	float wBuffSkill		= GetBuffStatus()->CriticalRate;
	float wBuffSkillRate	= GetRateBuffStatus()->CriticalRate;
	float fValA				= (((float)wDexterityAll - 15) * 7) ;
	float fValB				= (float)(125 + wLevel + fValA + wPassiveSkill + wBuffSkill);
	float fValC				= wPassiveSkillRate + wBuffSkillRate;
	float dwCritical		= GetFloatRound(fValB) ;

	{
		const PlayerStat::Value& value = GetSetItemStats().mCriticalRate;
		dwCritical = max( 0, ( dwCritical + value.mPlus ) * ( 1.0f + value.mPercent ) );
	}

	{
		const PlayerStat::Value& value = mItemStat.mCriticalRate;
		dwCritical = ( dwCritical + value.mPlus ) * ( 1.0f + value.mPercent );
	}

	dwCritical = GetFloatRound( dwCritical * ( 1 + fValC / 100 ));

	return DWORD( dwCritical );
}

DWORD CHero::DoGetMagicCriticalRate()	
{
	DWORD wLevel			= GetLevel() ;
	float wIntelligenceAll	= ( mCharStat.mIntelligence.mPlus + mItemStat.mIntelligence.mPlus + GetBuffStatus()->Int + GetPassiveStatus()->Int ) * ( 1.0f + mItemStat.mIntelligence.mPercent + GetRateBuffStatus()->Int + GetRatePassiveStatus()->Int/100 );

	{
		const PlayerStat::Value& value = GetSetItemStats().mIntelligence;

		wIntelligenceAll =	
			float( max( 0, ( wIntelligenceAll + value.mPlus ) * ( 1.0f + value.mPercent ) ) );
	}

	wIntelligenceAll *= 0.6f; 

	float wPassiveSkill		= GetPassiveStatus()->MagicCriticalRate;
	float wPassiveSkillRate	= GetRatePassiveStatus()->MagicCriticalRate; 
	float wBuffSkill		= GetBuffStatus()->MagicCriticalRate;
	float wBuffSkillRate	= GetRateBuffStatus()->MagicCriticalRate;
	float fValA				= (((float)wIntelligenceAll/* - 15*/)/* * 4*/) ;
	float fValB				= (float)(125 + wLevel + fValA + wPassiveSkill + wBuffSkill);
	float fValC				= wPassiveSkillRate + wBuffSkillRate;
	float dwCritical		= GetFloatRound(fValB) ;

	{
		const PlayerStat::Value& value = GetSetItemStats().mMagicCriticalRate;
		dwCritical = max( 0, ( dwCritical + value.mPlus ) * ( 1.0f + value.mPercent ) );
	}

	{
		const PlayerStat::Value& value = mItemStat.mMagicCriticalRate;
		dwCritical = ( dwCritical + value.mPlus ) * ( 1.0f + value.mPercent );
	}

	return DWORD( floor(fValC + (fValB * 0.1f)) );
}


DWORD CHero::DoGetMoveSpeedRate()	
{
	float	fMoveSpeed = DoGetMoveSpeed() / 4.5f - 100.0f;


	return max( 0, DWORD( floor( 0.5f + fMoveSpeed ) ) );
}

DWORD CHero::DoGetCastingSpeedRate()
{
	float	fPercent			= GetRatePassiveStatus()->MagicSkillSpeedRate
								+ GetRateBuffStatus()->MagicSkillSpeedRate;



	return DWORD( floor( 0.5f + fPercent ) );
}

DWORD CHero::DoGetAttackSpeedRate()		
{
	float	fPercent			= GetRatePassiveStatus()->PhysicSkillSpeedRate
								+ GetRateBuffStatus()->PhysicSkillSpeedRate;

	
	
	return DWORD( floor( 0.5f + fPercent ) );
}

DWORD CHero::DoGetRangeRate()		
{
	const float staticValue = mBuffStatus.Range + mPassiveStatus.Range;
	const float dynamicValue = mRateBuffStatus.Range + mRatePassiveStatus.Range;

	return DWORD(staticValue * (1.0f + dynamicValue));
}

float CHero::DoGetBlockRate()		
{
	const float rate = mRateBuffStatus.Block + mRatePassiveStatus.Block + 1.0f;
	const float value = ( mBuffStatus.Block + mPassiveStatus.Block ) / 100.0f;
	float bonus	= 0;

	switch( GetCharacterTotalInfo()->Job[0] )
	{
	case 1:
		{
			bonus = 10;
			break;
		}
	case 2:
		{
			bonus = 5;
			break;
		}
	case 3:
		{
			bonus = 3;
			break;
		}
	case 4:
		{
			bonus = 3;
			break;
		}
	}


	return bonus + rate;
}

/* ùùùùù?ùùù ùùù∞ùùù ùùùùùùùù ù?ùùù. */
void CHero::HeroStateNotify(BYTE EndState)
{
	MSG_BYTE msg;
	msg.Category = MP_CHAR;
	msg.Protocol = MP_CHAR_STATE_NOTIFY;
	msg.dwObjectID = GetID();
	msg.bData = EndState;

	NETWORK->Send(&msg, sizeof(msg));
}

BYTE CHero::GetManaPercent()
{
	return (BYTE)((GetMana()*100) / GetMaxMana());
}

DWORD CHero::DoGetMaxMana()
{
	return m_HeroTotalInfo.MaxMana;
}

void CHero::ClearGuildInfo()
{
	CPlayer::ClearGuildInfo();
}

char* CHero::GetGuildEntryDate()
{
	return m_HeroTotalInfo.MunpaCanEntryDate;
}

void CHero::SetGuildEntryDate(char* day)
{
	SafeStrCpy(m_HeroTotalInfo.MunpaCanEntryDate, day, 11);
}

char* CHero::GetFamilyEntryDate()
{
	return m_HeroTotalInfo.FamilyCanEntryDate;
}

void CHero::SetFamilyEntryDate(char* day)
{
	SafeStrCpy(m_HeroTotalInfo.FamilyCanEntryDate, day, 11);
}

void CHero::SetGuildName(char* Name)	
{ 
	SafeStrCpy(m_CharacterInfo.GuildName, Name, MAX_GUILD_NAME+1);	
}

char* CHero::GetGuildName()			
{ 
	return m_CharacterInfo.GuildName;
}

void CHero::SetFamilyName(char* Name)	
{ 
	SafeStrCpy(
		m_CharacterInfo.FamilyName,
		Name,
		_countof(m_CharacterInfo.FamilyName));
}

char* CHero::GetFamilyName()			
{ 
	return m_CharacterInfo.FamilyName;
}

BOOL CHero::CanSocietyAction( DWORD curTime )
{
	if( GetState() != eObjectState_None && GetState() != eObjectState_Society &&
		GetState() != eObjectState_Immortal )
		return FALSE;

	DWORD dwElapsedTime = gCurTime - m_dwLastSocietyAction;
	if( dwElapsedTime > 2000 ) //2ùù
	{
		m_dwLastSocietyAction = gCurTime;
		return TRUE;
	}

	return FALSE;
}
void CHero::ProcessSpecialSkill()
{
	if( mSpecialSkillList.empty() )
	{
		return;
	}

	// 090204 LUJ, ?ùùùùùù ùùùù ùùùùù?ù ùù ùùùù ùù?ùù ù?ùùù ùù ùù ù?ù ùùùùùù
	SpecialSkillData specialSkillData = mSpecialSkillList.front();
	mSpecialSkillList.pop_front();

	cBuffSkillInfo* const buffSkillInfo = specialSkillData.mBuffSkillInfo;
	const BOOL validCondition = IsEnable(*(buffSkillInfo->GetSkillInfo()));
	const BOOL isTurnOn	= (! specialSkillData.mIsTurnOn && validCondition);
	const BOOL isTurnOff = (specialSkillData.mIsTurnOn && ! validCondition);

	if( isTurnOn )
	{
		buffSkillInfo->AddBuffStatus();
		specialSkillData.mIsTurnOn = TRUE;
		GAMEIN->GetCharacterDialog()->RefreshInfo();
	}
	else if( isTurnOff )
	{
		buffSkillInfo->RemoveBuffStatus();
		specialSkillData.mIsTurnOn = FALSE;
		GAMEIN->GetCharacterDialog()->RefreshInfo();
	}

	// 090204 LUJ, ùùùù ù?? ùùùù ù?ùù?ù
	mSpecialSkillList.push_back( specialSkillData );
}

DWORD CHero::GetAccuracy()
{
	return mAccuracy;
}

DWORD CHero::GetAvoid()
{
	return mAvoid;
}

DWORD CHero::GetBlock()
{
	return mBlock;
}

DWORD CHero::GetPhysicAttackMax()
{
	return mPhysicAttackMax;
}

DWORD CHero::GetPhysicAttackMin()
{
	return mPhysicAttackMin;
}

DWORD CHero::GetMagicAttack()
{
	return mMagicAttack;
}

DWORD CHero::GetPhysicDefense()
{
	return mPhysicDefense;
}

DWORD CHero::GetMagicDefense()
{
	return mMagicDefense;
}

DWORD CHero::GetCriticalRate()
{
	return mCriticalRate;
}

DWORD CHero::GetCriticalDamage()
{
	return mCriticalDamage;
}

DWORD CHero::GetLifeRecover()
{
	return mLifeRecover;
}

DWORD CHero::GetManaRecover()
{
	return mManaRecover;
}

DWORD	CHero::GetMaxLifeWithItem()
{
	return mMaxLife;
}

DWORD	CHero::GetMaxManaWithItem()
{
	return mMaxMana;
}


void CHero::AddMatch()
{
	++m_matchedQuantity;

	SetHeart( TRUE );
}


void CHero::RemoveMatch()
{
	if( 1 > --m_matchedQuantity )
	{
		SetHeart( FALSE );

		m_matchedQuantity = 0;
	}
}


void CHero::CheckAllHeart()
{
	m_matchedQuantity = 0;
	
	// ùù¸ ùùùù
	{
		CYHHashTable<CObject>& objectTable = OBJECTMGR->GetObjectTable();
		YHTPOSITION position = objectTable.GetPositionHead();

		for(CObject* object = objectTable.GetDataPos( &position );
            0 < object;
			object = objectTable.GetDataPos( &position ))
		{
			if( eObjectKind_Player != object->GetObjectKind() )
			{
				continue;
			}

			CPlayer* player = ( CPlayer* )object;

			if( player->CheckHeart( this ) )
			{
				player->SetHeart( TRUE );

				AddMatch();
			}
			else
			{
				player->SetHeart( FALSE );

				RemoveMatch();
			}
		}
	}
}

DWORD CHero::GetVitality() const 
{ 
	char buffer[ MAX_PATH ] = { 0 };
	sprintf( buffer, "%0.1f", mCharStat.mVitality.mPlus + mItemStat.mVitality.mPlus );

	float Result = float( atof( buffer ) );

	const PlayerStat::Value& value = mSetItemStat.mVitality;
	Result += value.mPlus;

	Result += ( mPassiveStatus.Vit + mBuffStatus.Vit );

	float rate = ( mRatePassiveStatus.Vit + mRateBuffStatus.Vit + ( value.mPercent * 100.f ) ) / 100.f + mItemStat.mVitality.mPercent;

	Result += ( mCharStat.mVitality.mPlus * rate );

	if( Result < 0 )
	{
		Result = 0;
	}

	return (DWORD)Result;
}

DWORD CHero::GetWisdom() const
{ 
	char buffer[ MAX_PATH ] = { 0 };
	sprintf( buffer, "%0.1f", mCharStat.mWisdom.mPlus + mItemStat.mWisdom.mPlus );

	float Result = float( atof( buffer ) );

	const PlayerStat::Value& value = mSetItemStat.mWisdom;
	Result += value.mPlus;

	Result += ( mPassiveStatus.Wis + mBuffStatus.Wis );

	float rate = ( mRatePassiveStatus.Wis + mRateBuffStatus.Wis + ( value.mPercent * 100.f ) ) / 100.f + mItemStat.mWisdom.mPercent;

	Result += ( mCharStat.mWisdom.mPlus * rate );
	if( Result < 0 )
	{
		Result = 0;
	}

	return (DWORD)Result;
}

DWORD CHero::GetStrength() const
{
	char buffer[ MAX_PATH ] = { 0 };
	sprintf( buffer, "%0.1f", mCharStat.mStrength.mPlus + mItemStat.mStrength.mPlus );

	float Result = float( atof( buffer ) );

	const PlayerStat::Value& value = mSetItemStat.mStrength;
	Result += value.mPlus;

	Result += ( mPassiveStatus.Str + mBuffStatus.Str );

	float rate = ( mRatePassiveStatus.Str + mRateBuffStatus.Str + ( value.mPercent * 100.f ) ) / 100.f + mItemStat.mStrength.mPercent;

	Result += ( mCharStat.mStrength.mPlus * rate );

	if( Result < 0 )
	{
		Result = 0;
	}

	return (DWORD)Result;
}

DWORD CHero::GetDexterity() const 
{ 
	char buffer[ MAX_PATH ] = { 0 };
	sprintf( buffer, "%0.1f", mCharStat.mDexterity.mPlus + mItemStat.mDexterity.mPlus );

	float Result = float( atof( buffer ) );

	const PlayerStat::Value& value = mSetItemStat.mDexterity;
	Result += value.mPlus;

	Result += ( mPassiveStatus.Dex + mBuffStatus.Dex );

	float rate = ( mRatePassiveStatus.Dex + mRateBuffStatus.Dex + ( value.mPercent * 100.f ) ) / 100.f + mItemStat.mDexterity.mPercent;

	Result += ( mCharStat.mDexterity.mPlus * rate );
	if( Result < 0 )
	{
		Result = 0;
	}

	return (DWORD)Result;
}

DWORD CHero::GetIntelligence() const
{
	char buffer[ MAX_PATH ] = { 0 };
	sprintf( buffer, "%0.1f", mCharStat.mIntelligence.mPlus + mItemStat.mIntelligence.mPlus );

	float Result = float( atof( buffer ) );

	const PlayerStat::Value& value = mSetItemStat.mIntelligence;
	Result += value.mPlus;

	Result += ( mPassiveStatus.Int + mBuffStatus.Int );

	float rate = ( mRatePassiveStatus.Int + mRateBuffStatus.Int + ( value.mPercent * 100.f ) ) / 100.f + mItemStat.mIntelligence.mPercent;

	Result += ( mCharStat.mIntelligence.mPlus * rate );

	if( Result < 0 )
	{
		Result = 0;
	}

	return (DWORD)Result;
}
float CHero::DoGetMoveSpeed()
{
	float Speed = CPlayer::DoGetMoveSpeed();
	
	if( GetAbnormalStatus()->IsMoveStop )
	{
		Speed = 0;
	}

	return Speed;
}

void CHero::StartSkillProgressBar( DWORD time )
{
	GetObjectBalloon()->GetProgressBar()->CLI_Start(CSHFarmManager::FARM_CHAR_MOTION_PROGRESSBAR_TYPE, CSHFarmManager::FARM_CHAR_MOTION_PROGRESSBAR_WIDTH, time);
}
// 071210 LYW --- Hero : HEROùù ?ùù ù?ùù? ùùùù ùùùù ùù ùù?ù?ù ù?ù ù?ù.
void CHero::Set_HeroExtendedInvenCount(BYTE count)
{
	// ùùùùù ?ùù ù?ù ùùùù ùù ùùùù ùùù??ù ù?ù, ùùùù ùùùùùù ùùùù ùù ùùùù ùùùù ùùùù ù?ù ùùùù?ù.
	if( count > 2 ) return ;	

	m_byExtendedInvenCount = count ;
}

void CHero::PassiveSkillCheckForWeareItem()
{
	mPassiveStatus = Status();
	mRatePassiveStatus = Status();

	SKILLTREEMGR->SetPositionHead();

	for(SKILL_BASE* pSkillBase = SKILLTREEMGR->GetSkillBase();
		0 < pSkillBase;
		pSkillBase = SKILLTREEMGR->GetSkillBase())
	{
		const LEVELTYPE skillLevel = min(
			LEVELTYPE(SKILLMGR->GetSkillSize(pSkillBase->wSkillIdx)),
			pSkillBase->Level);
		cActiveSkillInfo* const pSkill = ( cActiveSkillInfo* )SKILLMGR->GetSkillInfo(
			pSkillBase->wSkillIdx - 1 + skillLevel);

		if( pSkill->GetKind() != SKILLKIND_PASSIVE )
			continue;

		for( DWORD cnt = 0; cnt < MAX_BUFF_COUNT; cnt++ )			
		{
			const DWORD index = pSkill->GetSkillInfo()->Buff[ cnt ];
			cBuffSkillInfo* pSkillInfo = ( cBuffSkillInfo* )SKILLMGR->GetSkillInfo( index );

			if(0 == pSkillInfo)
			{
				break;
			}
			else if(FALSE == IsEnable(*(pSkillInfo->GetSkillInfo())))
			{
				continue;
			}

			pSkillInfo->AddPassiveStatus();
		}	
	}
}

// 090204 LUJ, ?ùù ùù?ùù ù?ùù?ù.
void CHero::AddSpecialSkill( cBuffSkillInfo* buffSkillInfo )
{
	// 090204 LUJ, Ûùùùùùù ù?ù ùùùùù?ù. ùùù?ùùù ùù ù?ùù?? ùùù?ù ùùùù ùùùù ùùù ùùùùù?ù
	buffSkillInfo->AddBuffStatus();

	SpecialSkillData specialSkillData = { 0 };
	specialSkillData.mBuffSkillInfo	= buffSkillInfo;
	specialSkillData.mIsTurnOn		= TRUE;
	mSpecialSkillList.push_back( specialSkillData );
}

// 090204 LUJ, ?ùù ùù?ùù ùùùùù?ù
void CHero::RemoveSpecialSkill( cBuffSkillInfo* buffSkillInfo )
{
	SpecialSkillList::iterator it = mSpecialSkillList.end();

	// 090204 LUJ, ùùùùù?ù ù?ù ùùùùùù ùù?ùù „ù¥ù
	for(	;
			mSpecialSkillList.end() != it;
			++it )
	{
		const SpecialSkillData& specialSkillData = *it;

		if( specialSkillData.mBuffSkillInfo == buffSkillInfo )
		{
			break;
		}
	}

	if( mSpecialSkillList.end() == it )
	{
		return;
	}

	const SpecialSkillData& specialSkillData = *it;

	// 090204 LUJ, ùùù?ù ùùùù ùùùùùù ùùùùùù ùù ùùùùù?ù ù?ù ùù ùùùù?ù ù?ù
	if( specialSkillData.mIsTurnOn )
	{
		buffSkillInfo->RemoveBuffStatus();
	}

	mSpecialSkillList.erase( it );
}

void CHero::ClearWayPoint()
{
	m_WayPointManager->ClearAllWayPoint();

	// Big/Mini Mapùù MovePoint ùùùù
	CBigMapDlg* pBigMapDlg = GAMEIN->GetBigMapDialog();
	CWorldMapDlg* pWorldMapDlg = GAMEIN->GetWorldMapDialog();
	CBigMapDlg* pWorldToBigMapDlg = (pWorldMapDlg != NULL ? pWorldMapDlg->GetBigMapDlg() : NULL);
	CMiniMapDlg* pMiniMapDlg = GAMEIN->GetMiniMapDialog();
	if( pBigMapDlg )		pBigMapDlg->ClearMovePoint();
	if( pWorldToBigMapDlg )	pWorldToBigMapDlg->ClearMovePoint();
	if( pMiniMapDlg )		pMiniMapDlg->ClearMovePoint();
}

BOOL CHero::Move_UsePath( VECTOR3* pDestPos, BOOL bSimpleMode, BOOL bMousePointDisplay, BOOL bIsCollisionPosMove, BOOL bKeyboard )
{
	// 50cm üùùùù ù??ù ùùùù ù?ù.
	if(!bKeyboard)
	{
		pDestPos->x = (int(pDestPos->x / TILECOLLISON_DETAIL) + 0.5f) * TILECOLLISON_DETAIL;
		pDestPos->y = 0;
		pDestPos->z = (int(pDestPos->z / TILECOLLISON_DETAIL) + 0.5f) * TILECOLLISON_DETAIL;
	}

	// ùùùùù? ?ùù?ùù ùùùùùù ùùùùùùùù ù?¥ù
	if( InterlockedCompareExchange(&g_PathThreadActivated, 0, 0) == 1 )
	{
		return FALSE;
	}

	VECTOR3 t_StartPos;
	GetPosition( &t_StartPos );

	/// 0. üùùùù ù?ùù?ùùùùùùù ùùù
	if (MAP->CollisionCheck_OneTile( pDestPos ) == FALSE)
	{
		// ù?ùù?ùùùùùùùùùùù ù?ù ùùùù ù?ù
		if( !bIsCollisionPosMove )
			return FALSE;

		// ùùùùùù ù?ùùùùù ù?ùùùùùùù ùùù?ù.
		if( MAP->CollisionLine( &t_StartPos, pDestPos, pDestPos, MAP->GetMapNum(), this ) == FALSE )
			return FALSE;
	}

	/// ùùùù Ûùù
	ClearWayPoint();

	if( MOVEMGR->IsMoving( this ) )
		MOVEMGR->CalcPositionEx( this, gCurTime );

	/// 1. ù?ùùù ùùùù ùüùùù ùù ùù ù??ù ùùùùùùùù ù?ù
	HTR_S(eRT_OneLineCheck);
	BOOL t_CollisionCheck_Oneline = MAP->CollisionCheck_OneLine_New( &t_StartPos, pDestPos );
	HTR_E(eRT_OneLineCheck);
	if ( t_CollisionCheck_Oneline == TRUE)
	{
		Move_Simple( pDestPos );

		if( bMousePointDisplay )
			MOVEPOINT->SetPoint( pDestPos );
	}
	else
	{
		// ùùùù
		if( MOVEMGR->IsMoving( this ) )
			MOVEMGR->HeroMoveStop();

		/// 2. ùù„ùùù ù?ù
		// ùù„ùù ùùùùù? ùùùùù?ù
		// ùùùùù? ùùù?ù ùù ùùùùùùùù?ùù ù?ùù?ù
		// 091204 ShinJS --- 3ùùù ùùù
		if(FALSE == MAP->PathFind(
			&t_StartPos,
			pDestPos,
			m_WayPointManager,
			FALSE))
		{
			return FALSE;
		}
	}

	if(!bKeyboard) m_WayPointManager->SetFlagDestination( *pDestPos );

	return TRUE;
}

BOOL CHero::Move_Simple( VECTOR3* pTargetPos )
{
	m_MoveInfo.InitTargetPosition();
	m_MoveInfo.SetTargetPosition( 0, *pTargetPos );
	m_MoveInfo.SetMaxTargetPosIdx( 1 );

	MOVE_ONETARGETPOS msg;
	msg.Category = MP_MOVE;
	msg.Protocol = MP_MOVE_ONETARGET;
	msg.dwObjectID = HEROID;
	msg.dwMoverID = HEROID;
	msg.SetStartPos( &m_MoveInfo.CurPosition );
	msg.SetTargetPos( m_MoveInfo.GetTargetPosition(0) );

	NETWORK->Send(&msg,msg.GetSize());

	MOVEMGR->StartMoveEx( this, NULL, gCurTime );

	MACROMGR->CheckToggleState( this );


	// 091208 ShinJS --- ùùùùùùù?? ûùù ùùùù ùùù ?ù?ù ù?ùùù?ùù.
	CVehicle* pVehicle = (CVehicle*)OBJECTMGR->GetObject( GetVehicleID() );
	if( pVehicle && pVehicle->GetObjectKind() == eObjectKind_Vehicle )
	{
		if( pVehicle->GetOwnerIndex() == GetID() &&
			IsGetOnVehicle() )
		{
			pVehicle->StartMove( pTargetPos );
		}
	}
	return TRUE;
}

BOOL CHero::NextMove()
{
	VECTOR3 t_WayPoint;
	if ( m_WayPointManager->PopWayPoint(&t_WayPoint) == TRUE )
	{
		if( !Move_Simple( &t_WayPoint ) )
		{
			ClearWayPoint();
			return FALSE;
		}

		return TRUE;
	}

	ClearWayPoint();
	return FALSE;
}

float CHero::GetBonusRange() const
{
	const float staticValue = mBuffStatus.Range + mPassiveStatus.Range;
	const float dynamicValue = mRateBuffStatus.Range + mRatePassiveStatus.Range;

	return staticValue * (1.0f + dynamicValue);
}

BOOL CHero::IsNoEquip(eArmorType armorType, eWeaponType weaponType, eWeaponAnimationType weaponAnimationType)
{
	switch(armorType)
	{
	case eArmorType_Robe:
	case eArmorType_Leather:
	case eArmorType_Metal:
		{
			DWORD Armor = GetWearedItemIdx( eWearedItem_Dress );
			DWORD Boots = GetWearedItemIdx( eWearedItem_Shoes );
			DWORD Glove = GetWearedItemIdx( eWearedItem_Glove );

			ITEM_INFO* pArmor = ITEMMGR->GetItemInfo( Armor );
			ITEM_INFO* pBoots = ITEMMGR->GetItemInfo( Boots );
			ITEM_INFO* pGlove = ITEMMGR->GetItemInfo( Glove );

			if( !pArmor || !pBoots || !pGlove )
				return TRUE;
			else
			{
				if( ( pArmor->ArmorType != armorType ) || ( pBoots->ArmorType != armorType ) || ( pGlove->ArmorType != armorType ) )
					return TRUE;
			}

			break;
		}
	case eArmorType_Shield:
		{
			DWORD Shield = GetWearedItemIdx( eWearedItem_Shield );
			ITEM_INFO* pShield = ITEMMGR->GetItemInfo( Shield );

			if(!pShield || (pShield && (pShield->WeaponAnimation == eWeaponAnimationType_OneHand || pShield->WeaponAnimation == eWeaponAnimationType_Dagger)))
				return TRUE;

			break;
		}
	}

	if(eWeaponType_None != weaponType)
	{
		if(GetWeaponEquipType() != weaponType)
		{
			return TRUE;
		}
	}

	switch( weaponAnimationType )
	{
	case eWeaponAnimationType_None:
		break;
	case eWeaponAnimationType_OneHand:
		{
			switch( GetWeaponAniType() )
			{
			case eWeaponAnimationType_OneHand:
			case eWeaponAnimationType_Dagger:
			case eWeaponAnimationType_TwoBlade:
				break;
			default:
				return TRUE;
			}
		}
		break;

	case eWeaponAnimationType_TwoHand:
		{
			switch( GetWeaponAniType() )
			{
			case eWeaponAnimationType_TwoHand:
			case eWeaponAnimationType_Staff:
			case eWeaponAnimationType_Bow:
			case eWeaponAnimationType_Gun:
			case eWeaponAnimationType_Spear:
				break;
			default:
				return TRUE;
			}
		}
		break;

	default:
		{
			if( GetWeaponAniType() != weaponAnimationType )
				return TRUE;
		}
		break;
	}

	return FALSE;
}

BOOL CHero::IsEnable(const BUFF_SKILL_INFO& info)
{
	if(IsNoEquip(
		info.mArmorType,
		info.mWeaponType,
		info.mWeaponAnimationType))
	{
		return FALSE;
	}

	float checkValue = 0;
	const BUFF_SKILL_INFO::Condition& condition = info.mCondition;

	switch( condition.mType )
	{
	case BUFF_SKILL_INFO::Condition::TypeLifePercent:
		{
			checkValue = float(GetLifePercent());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeLife:
		{
			checkValue = float(GetLife());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeLifeMax:
		{
			checkValue = float(GetMaxLife());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeLifeRecovery:
		{
			checkValue = float(GetLifeRecover());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeManaPercent:
		{
			checkValue = float(GetManaPercent());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeMana:
		{
			checkValue = float(GetMana());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeManaMax:
		{
			checkValue = float(GetMaxMana());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeManaRecovery:
		{
			checkValue = float(GetManaRecover());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeStrength:
		{
			checkValue = float(GetStrength());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeIntelligence:
		{
			checkValue = float(GetIntelligence());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeDexterity:
		{
			checkValue = float(GetDexterity());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeWisdom:
		{
			checkValue = float(GetWisdom());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeVitality:
		{
			checkValue = float(GetVitality());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypePhysicalAttack:
		{
			checkValue = float(GetPhysicAttackMax());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeMagicalAttack:
		{
			checkValue = float(GetMagicAttack());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypePhysicalDefense:
		{
			checkValue = float(GetPhysicDefense());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeMagicalDefense:
		{
			checkValue = float(GetMagicDefense());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeCriticalRate:
		{
			checkValue = float(GetCriticalRate());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeCriticalDamage:
		{
			checkValue = float(GetCriticalDamage());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeAccuracy:
		{
			checkValue = float(GetAccuracy());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeEvade:
		{
			checkValue = float(GetAvoid());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeMoveSpeed:
		{
			checkValue = float(GetMoveSpeed());
			break;
		}
	}

	BOOL isEnable = TRUE;

	switch(condition.mOperation)
	{
	case BUFF_SKILL_INFO::Condition::OperationMore:
		{
			isEnable = (checkValue > condition.mValue);
			break;
		}
	case BUFF_SKILL_INFO::Condition::OperationMoreEqual:
		{
			isEnable = (checkValue >= condition.mValue);
			break;
		}
	case BUFF_SKILL_INFO::Condition::OperationLess:
		{
			isEnable = (checkValue < condition.mValue);
			break;
		}
	case BUFF_SKILL_INFO::Condition::OperationLessEqual:
		{
			isEnable = (checkValue <= condition.mValue);
			break;
		}
	case BUFF_SKILL_INFO::Condition::OperationEqual:
		{
			isEnable = (checkValue == condition.mValue);
			break;
		}
	}

	return isEnable;
}


void CHero::SetObjectBattleState(eObjectBattleState eBattleState)
{
	CPlayer::SetObjectBattleState( eBattleState );

	cBattleGuageDlg* pBattleGuageDlg	=	GAMEIN->GetBattleGuageDlg();

	if( !pBattleGuageDlg )
		return;
	
	if( eBattleState == eObjectBattleState_Battle )
	{
		pBattleGuageDlg->StartBattleSignal();
	}
	else if( eBattleState == eObjectBattleState_Peace )
	{
		pBattleGuageDlg->EndBattleSignal();
	}
}

void CHero::SetFamily(CSHFamily* family)
{
	m_pcsFamily->Set(
		family);
}

void CHero::Move(int nKeyMoveDir)
{
	CDealDialog* pDialogDlg = GAMEIN->GetDealDialog();

	if (pDialogDlg->IsActive())
	{
		MOVEMGR->HeroMoveStop();
		return;
	}

	DisableAutoAttack();
	SetNextAction(0);
	VECTOR3 Depan = CAMERA->GetForwardVector();
	VECTOR3 Belakang = Depan * -1;
	VECTOR3 Kiri, Kanan;
	Kiri.x = Depan.z * -1;
	Kiri.y = 0;
	Kiri.z = Depan.x;
	Kanan.x = Depan.z;
	Kanan.y = 0;
	Kanan.z = Depan.x * -1;
	VECTOR3 stPos;

	DWORD TickByFps = GAMERESRCMNGR->m_GameDesc.FramePerSec;
	float tSpeed = float(GetMoveSpeed());
	float mSpeed = 0;

	if(m_MoveInfo.MoveMode == eMoveMode_Run)
	{
		if(tSpeed >= RUNSPEED && TickByFps<60)
			mSpeed = tSpeed / 3.0f;
		if(tSpeed >= RUNSPEED && TickByFps>=60)
			mSpeed = tSpeed / 6.0f;
		if(mSpeed < 160)
			mSpeed = 160;
	}
	else if(m_MoveInfo.MoveMode == eMoveMode_Walk)
	{
		if(tSpeed >= WALKSPEED && TickByFps<60)
			mSpeed = tSpeed / 4.0f;
		if(tSpeed >= WALKSPEED && TickByFps>=60)
			mSpeed = tSpeed / 8.0f;
		if(mSpeed < 100)
			mSpeed = 100;
	}

	float mDistance = mSpeed + m_KeyMove_DistOneStep;

	if (nKeyMoveDir)
	{
		HERO->GetPosition(&stPos);
		if (nKeyMoveDir & KEY_MOVE_LEFT)	stPos = stPos + Kiri		* mDistance;
		if (nKeyMoveDir & KEY_MOVE_RIGHT)	stPos = stPos + Kanan		* mDistance;
		if (nKeyMoveDir & KEY_MOVE_UP)		stPos = stPos + Depan		* mDistance;
		if (nKeyMoveDir & KEY_MOVE_DOWN)	stPos = stPos + Belakang	* mDistance;
		MOVEMGR->SetHeroTarget(&stPos, FALSE, TRUE);
	}
}

void CHero::DisableWASD(int val)
{
	if(val != 0)
	{
		m_DisableWASD = TRUE;
	}
	else
	{
		m_DisableWASD = FALSE;
	}
}

void CHero::Move_KeyboardInput()
{
	int nKeyMoveDir = MACROMGR->GetKeyMoveDir();
	
	if( MACROMGR->IsKeyChanged() )
	{
		MACROMGR->ClearKeyChanged();

		if (nKeyMoveDir != KEY_MOVE_STOP)
		{
			ClearWayPoint();

			m_MovingAction.ExcuteAction(this);
			m_MovingAction.Clear();

			Move(nKeyMoveDir);
		}
		else
		{
			MOVEMGR->HeroMoveStop();			
		}
	}
	else if( CMHTimeManager::GetInstance()->CheckTimer500() )
	{
		if (nKeyMoveDir != KEY_MOVE_STOP)
		{
			Move(nKeyMoveDir);
		}
	}
}