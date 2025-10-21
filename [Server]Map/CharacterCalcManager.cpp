#include "stdafx.h"
#include "CharacterCalcManager.h"
#include "Player.h"
#include "CharMove.h"
#include "BossMonster.h"
#include "WeatherManager.h"
#include "../[cc]skill/server/manager/skillmanager.h"
#include "ItemManager.h"
#include "MHFile.h"
#include "FishingManager.h"
#include "pet.h"
#include "MapDBMsgParser.h"
#include "..\[cc]header\GameResourceManager.h"
#include "UserTable.h"
#include "petmanager.h"

// 070410 LYW --- CCharacterCalcManager : Add struct for hp mp determination.
struct HPMP_DETERMINATE
{
	WORD	wClass_Idx ;

	BYTE	byHP_Value ;
	BYTE	byMP_Value ;
} ;


CCharacterCalcManager::CCharacterCalcManager()
{
	LoadHPMPDeterminateList();
	LoadJobSkillList();
}


CCharacterCalcManager::~CCharacterCalcManager()
{
	PTRLISTPOS pos = m_HPMPDeterminateList.GetHeadPosition();

	for(	HPMP_DETERMINATE* pDeterminate;
			(pDeterminate = (HPMP_DETERMINATE*)m_HPMPDeterminateList.GetNext(pos)) != NULL ;
			)
	{
		SAFE_DELETE( pDeterminate );
	}
}


CCharacterCalcManager* CCharacterCalcManager::GetInstance()
{
	static CCharacterCalcManager instance;

	return &instance;
}


void CCharacterCalcManager::StartUpdateLife( CObject* pObject, DWORD plusLife, BYTE recoverInterval, DWORD recoverTime )
{
	DWORD MaxVal = pObject->GetMaxLife();
	DWORD CurVal = pObject->GetLife();
	DWORD DamagedVal = MaxVal-CurVal;
	if(DamagedVal < plusLife)
		plusLife = DamagedVal;

	// 100624 ONS HPùùùùùù?Ûùùùù ?ùù ùùùùù?ù.
	// ùùüùùùù ùùùù ùùùùùù ùùù, ùùù?ùùù ùùùùùù ùùùù HPùù ùùùı?ùùùùùù 
	// ùùùùùù ùùùùù?ù ?ùù ùùùùù?ù ùùùùù?ùùù ùùùù.(MPùù ùùùù)
	if( DamagedVal > 0)
	{
		if( 0 == recoverTime )
		{
			// 100729 ONS ùùù HPùù ùùùùùù?ù?ù ùùù Ûùù ù?ù.
			pObject->SetLifeRecoverDirectly( plusLife );
		}
		else
		{
			YYRECOVER_TIME YYLifeRecoverTime;
			YYLifeRecoverTime.bStart = TRUE;
			YYLifeRecoverTime.count = recoverInterval;											// recoverIntervalùùùù ùùùù ?ùùù?ù
			YYLifeRecoverTime.recoverUnitAmout = plusLife/YYLifeRecoverTime.count;				// ù?ùùù ?ùùùù ùù
			YYLifeRecoverTime.recoverDelayTime = recoverTime/YYLifeRecoverTime.count;			// recoverTimeù?ù ?ùùùù ù?ù?ù
			YYLifeRecoverTime.lastCheckTime = 0;												// Ûùùùù ù?ù ?ùùù?ù.

			pObject->AddLifeRecoverTime( YYLifeRecoverTime );
		}
	}

	UpdateLife( pObject );
}

void CCharacterCalcManager::StartUpdateMana( CPlayer* pPlayer, DWORD plusMana, BYTE recoverInterval, DWORD recoverTime  )
{
	DWORD MaxVal = pPlayer->GetMaxMana();
	DWORD CurVal = pPlayer->GetMana();
	DWORD DamagedVal = MaxVal-CurVal;
	if(DamagedVal < plusMana)
		plusMana = DamagedVal;

	if( DamagedVal > 0)
	{
		if( 0 == recoverTime )
		{
			// 100729 ONS ùùù MPùù ùùùùùù?ù?ù ùùù Ûùù ù?ù.
			pPlayer->SetManaRecoverDirectly( plusMana );
		}
		else
		{
			YYRECOVER_TIME YYManaRecoverTime;
			YYManaRecoverTime.bStart = TRUE;
			YYManaRecoverTime.count = recoverInterval;												// recoverIntervalùùùù ùùùù ?ùùù?ù
			YYManaRecoverTime.recoverUnitAmout = plusMana/YYManaRecoverTime.count;					// ù?ùùù ?ùùùù ùù
			YYManaRecoverTime.recoverDelayTime = recoverTime/YYManaRecoverTime.count;				// recoverTimeù?ù ?ùùùù ù?ù?ù
			YYManaRecoverTime.lastCheckTime = 0;													// Ûùùùù ù?ù ?ùùù?ù.

			pPlayer->AddManaRecoverTime( YYManaRecoverTime );
		}
	}

	UpdateMana( pPlayer );
}
void CCharacterCalcManager::UpdateMana(CPlayer* pPlayer)
{
	if( pPlayer->GetState() == eObjectState_Die )
		return;

	pPlayer->UpdateMana();
}

void CCharacterCalcManager::UpdateLife(CObject* pObject)
{
	if( pObject->GetState() == eObjectState_Die )
		return;

	pObject->UpdateLife();
}

// 070410 LYW --- CStatsCalcManager : Add function to load HP & MP determination file.
BOOL CCharacterCalcManager::LoadHPMPDeterminateList()
{
	CMHFile fp ;

	if( ! fp.Init( "./System/Resource/HPMPdetermination.bin", "rb" ) )
	{
		MessageBox(NULL,"Loading failed!! ./System/Resource/HPMPdetermination.bin",0,0);
		return FALSE;
	}

	while( ! fp.IsEOF() )
	{
		HPMP_DETERMINATE* pDeterminate = new HPMP_DETERMINATE ;

		pDeterminate->wClass_Idx = fp.GetWord() ;
		pDeterminate->byHP_Value = fp.GetByte() ;
		pDeterminate->byMP_Value = fp.GetByte() ;

		m_HPMPDeterminateList.AddTail( pDeterminate ) ;
	}

	return TRUE ;
}

// 081022 KTH -- Load File Job Skill List
BOOL CCharacterCalcManager::LoadJobSkillList()
{
	CMHFile fp;

	if( !fp.Init( "./System/Resource/AddJobSkillList.bin", "rb" ) )
	{
		MessageBox(NULL, "Loading failed!! ./System/Resource/AddJobSkillList.bin", 0, 0);
		return FALSE;
	}

	while( ! fp.IsEOF() )
	{
		char szLine[256];
		fp.GetLine(szLine, sizeof(szLine));
		if (strstr(szLine, "//")) continue;	

		JobSkillInfo stJobSkillInfo;
		memset(&stJobSkillInfo, NULL, sizeof(JobSkillInfo));

		stJobSkillInfo.wClassIndex = fp.GetWord();
		stJobSkillInfo.dwSkillIndex = fp.GetDword();
		stJobSkillInfo.byPlusLevel = fp.GetByte();

		m_JobSkillList.push_back(stJobSkillInfo);
	}

	return TRUE;
}

void CCharacterCalcManager::AddPlayerJobSkill(CPlayer* pPlayer)
{
	//WORD wJobIndex[MAX_JOB_GRADE];
	CHARACTER_TOTALINFO pTotalInfo;
	pPlayer->GetCharacterTotalInfo( &pTotalInfo );

	pPlayer->ClearJobSkill();

	for( WORD grade = pTotalInfo.JobGrade; grade > 0; grade-- )
	{
		int jop = grade;

		if(  jop > 1 )
		{
			jop =  jop-1;
		}

		jop = pTotalInfo.Job[grade - 1];
		
		WORD wJobIndex = WORD( (pTotalInfo.Job[0]*1000) +
			( (pTotalInfo.Race + 1) * 100 ) +
			( grade *10 ) + jop );

		std::list<JobSkillInfo>::iterator BeginItr = m_JobSkillList.begin();

		for( ; BeginItr != m_JobSkillList.end(); BeginItr++ )
		{
			if( BeginItr->wClassIndex == wJobIndex )
			{
				pPlayer->AddJobSkill(BeginItr->dwSkillIndex, BeginItr->byPlusLevel);
			}
		}
	}
}

// 070410 LYW --- CStatsCalcManager : Add function to setting hp & mp determinate.
void CCharacterCalcManager::Init_HPMP_Point(CPlayer * pPlayer)
{
	// Setting class index.
	BYTE byFirst	= pPlayer->GetJop(0) ;
	BYTE bySecond	= pPlayer->GetRacial() + 1 ;	// Have to plus 1 cause of start index of racial is 1. ( from determination.bin )
	BYTE byThird	= pPlayer->GetJopGrade() ;	// Have to minus 1 cause of start index of jop array is 0.( from array of code. 
	BYTE byFourth	= 0;

	if( byThird == 1 )
	{
		byFourth = 1;
	}
	else
	{
		byFourth = pPlayer->GetJop(byThird - 1);
	}

	WORD wClassIdx	= ( byFirst * 1000 ) + ( bySecond * 100 ) + ( byThird * 10 ) + byFourth ;


	// Setting hp & mp determination value.
	PTRLISTPOS pos = m_HPMPDeterminateList.GetHeadPosition() ;

	while( pos )
	{
		HPMP_DETERMINATE* pDeterminate = (HPMP_DETERMINATE*)m_HPMPDeterminateList.GetNext(pos) ;

		if( pDeterminate )
		{
			if( pDeterminate->wClass_Idx == wClassIdx )
			{
				pPlayer->SetHP_Point(pDeterminate->byHP_Value) ;
				pPlayer->SetMP_Point(pDeterminate->byMP_Value) ;

				break ;
			}
		}
	}
}

// 070429 LYW --- CCharacterCalcManager : Add function to return hp&mp determinate.
BOOL CCharacterCalcManager::Get_HPMP_Point(CPlayer* pPlayer, BYTE* hpPoint, BYTE* mpPoint)
{
	// Setting class index.
	BYTE byFirst	= pPlayer->GetJop(0) ;
	BYTE bySecond	= pPlayer->GetRacial() + 1 ;	// Have to plus 1 cause of start index of racial is 1. ( from determination.bin )
	BYTE byThird	= pPlayer->GetJopGrade() ;	// Have to minus 1 cause of start index of jop array is 0.( from array of code. )
	BYTE byFourth	= 0;

	if( byThird == 1 )
	{
		byFourth = 1;
	}
	else
	{
		byFourth = pPlayer->GetJop(byThird - 1);
	}

	WORD wClassIdx	= ( byFirst * 1000 ) + ( bySecond * 100 ) + ( byThird * 10 ) + byFourth ;


	// Setting hp & mp determination value.
	PTRLISTPOS pos = m_HPMPDeterminateList.GetHeadPosition() ;

	while( pos )
	{
		HPMP_DETERMINATE* pDeterminate = (HPMP_DETERMINATE*)m_HPMPDeterminateList.GetNext(pos) ;

		if( pDeterminate )
		{
			if( pDeterminate->wClass_Idx == wClassIdx )
			{
				*hpPoint = pDeterminate->byHP_Value ;
				*mpPoint = pDeterminate->byMP_Value ;

				return TRUE ;
			}
		}
	}

	return FALSE ;
}

void CCharacterCalcManager::CalcMaxLife(CPlayer * pPlayer)
{
	float dwBaseVital = pPlayer->GetRealVitality() * 0.2f;
	float dwCurVital  = pPlayer->GetVitality() * 0.2f;

	LEVELTYPE level = pPlayer->GetLevel() ;

	double dBaseHP			= ( dwCurVital * (10 + (0.05 * (level * pPlayer->GetHP_Point()))) ) ;
	double dIncreaseHP		= ( level - 1 ) *  ( ( dwBaseVital + pPlayer->GetHP_Point() ) * 0.5 );
	double bonusLevelHP		= level * (pPlayer->GetCharacterTotalInfo().Job[0] == 1 ? 140 : 105);  
	

	double dwTotalHP = (dBaseHP + dIncreaseHP + bonusLevelHP) ;

	{
		const PlayerStat::Value& value = pPlayer->GetItemStats().mLife;
		dwTotalHP = ( dwTotalHP + value.mPlus ) * ( 1.0f + value.mPercent );
	}

	{
		const PlayerStat::Value& value = pPlayer->GetSetItemStats().mLife;
		dwTotalHP = ( dwTotalHP + value.mPlus ) * ( 1.0f + value.mPercent );
	}

	float addRateVal = ( 100 + pPlayer->GetRatePassiveStatus()->MaxLife + pPlayer->GetRateBuffStatus()->MaxLife ) / 100.f;
	float addVal = pPlayer->GetPassiveStatus()->MaxLife + pPlayer->GetBuffStatus()->MaxLife;

	dwTotalHP = max( 1.0f, dwTotalHP * addRateVal + addVal );
	
	pPlayer->mMaxLife = (float)dwTotalHP;

	pPlayer->SetMaxLife( ( DWORD )dwTotalHP );
}


void CCharacterCalcManager::CalcMaxMana(CPlayer * pPlayer)
{
	float dwBaseWisdom = (float)(pPlayer->GetRealWisdom() * 0.2f);
	float dwCurWisdom  = (float)(pPlayer->GetWisdom() * 0.2f);

	LEVELTYPE level = pPlayer->GetLevel() ;

	double dBaseMP		= ( dwCurWisdom * (6 + (0.05 * (level * pPlayer->GetMP_Point()))) ) ;
	double dIncreaseMP	= ( level - 1 ) * ( ( dwBaseWisdom + pPlayer->GetMP_Point() ) * 0.5 );	

	double dwTotalMP = (DWORD)(dBaseMP + dIncreaseMP) ;

	{
		const PlayerStat::Value& value = pPlayer->GetItemStats().mMana;
		dwTotalMP = ( dwTotalMP + value.mPlus ) * ( 1.0f + value.mPercent );
	}

	{
		const PlayerStat::Value& value = pPlayer->GetSetItemStats().mMana;
		dwTotalMP = ( dwTotalMP + value.mPlus ) * ( 1.0f + value.mPercent );
	}
	
	float addRateVal = ( 100 + pPlayer->GetRatePassiveStatus()->MaxMana + pPlayer->GetRateBuffStatus()->MaxMana ) / 100.f;
	float addVal = pPlayer->GetPassiveStatus()->MaxMana + pPlayer->GetBuffStatus()->MaxMana;


	dwTotalMP = max( 1.0f, dwTotalMP * addRateVal + addVal );
	
	pPlayer->mMaxMana = (float)dwTotalMP;

	pPlayer->SetMaxMana( ( DWORD )dwTotalMP );
}


void CCharacterCalcManager::CalcCharPhyAttackMax( CPlayer* pPlayer )
{
	DWORD wWeaponAttack	= 0;

	{
		const PlayerStat::Value& value = pPlayer->GetItemStats().mPhysicAttack;
		wWeaponAttack += DWORD( value.mPlus * ( 1.0f + value.mPercent ) );
	}

	DWORD wEnchantAttack	= pPlayer->mWeaponEnchantLevel;
	DWORD wLevel			= pPlayer->GetLevel();
	float wPassiveSkill		= pPlayer->GetPassiveStatus()->PhysicAttack;
	float wAttackA			= wWeaponAttack + wEnchantAttack + wLevel + wPassiveSkill + 3;
	float wStrengthAll		= float( pPlayer->GetStrength() );

	wStrengthAll *= 0.2f;

	float wMasteryLevel	= 0 ;
	float wMasteryLevelRate = 0;

	switch( pPlayer->GetWeaponAniType() )
	{ 
		case eWeaponAnimationType_OneHand:	wMasteryLevel = pPlayer->GetPassiveStatus()->OneHanded; wMasteryLevelRate = pPlayer->GetRatePassiveStatus()->OneHanded;	break;
		case eWeaponAnimationType_TwoHand:	wMasteryLevel = pPlayer->GetPassiveStatus()->TwoHanded; wMasteryLevelRate = pPlayer->GetRatePassiveStatus()->TwoHanded;	break;
		case eWeaponAnimationType_TwoBlade:	wMasteryLevel = pPlayer->GetPassiveStatus()->TwoBlade; wMasteryLevelRate = pPlayer->GetRatePassiveStatus()->TwoBlade;	break;
	}

	switch( pPlayer->GetWeaponEquipType() )
	{
		case eWeaponType_Sword:		wMasteryLevel += pPlayer->GetPassiveStatus()->Sword; wMasteryLevelRate += pPlayer->GetRatePassiveStatus()->Sword;	break;
		case eWeaponType_Mace:		wMasteryLevel += pPlayer->GetPassiveStatus()->Mace; wMasteryLevelRate += pPlayer->GetRatePassiveStatus()->Mace;		break;
		case eWeaponType_Axe:		wMasteryLevel += pPlayer->GetPassiveStatus()->Axe; wMasteryLevelRate += pPlayer->GetRatePassiveStatus()->Axe;		break;
		case eWeaponType_Dagger:	wMasteryLevel = pPlayer->GetPassiveStatus()->Dagger; wMasteryLevelRate = pPlayer->GetRatePassiveStatus()->Dagger;	break;
		case eWeaponType_Staff:		wMasteryLevel = pPlayer->GetPassiveStatus()->Staff; wMasteryLevelRate = pPlayer->GetRatePassiveStatus()->Staff;		break;
		case eWeaponType_Bow:		wMasteryLevel = pPlayer->GetPassiveStatus()->Bow; wMasteryLevelRate = pPlayer->GetRatePassiveStatus()->Bow;			break;
		case eWeaponType_Gun:		wMasteryLevel = pPlayer->GetPassiveStatus()->Gun; wMasteryLevelRate = pPlayer->GetRatePassiveStatus()->Gun;			break;
	}

	float wAttackBuff		= pPlayer->GetBuffStatus()->PhysicAttack;
	float wAttackBuffRate	= pPlayer->GetRateBuffStatus()->PhysicAttack;
	float wPassiveSkillRate = pPlayer->GetRatePassiveStatus()->PhysicAttack;
	float wAttackB			= 1000 + (((wStrengthAll -10) * 20) + (wMasteryLevel * 25) + (wAttackBuff * 10));
	float wAttackC			= wAttackBuffRate + wPassiveSkillRate + wMasteryLevelRate;
	float dwTotalAttack		= ((wAttackA * wAttackB) / 1000)  ;

	{
		const PlayerStat::Value& value = pPlayer->GetSetItemStats().mPhysicAttack;
		dwTotalAttack = max( 0, ( dwTotalAttack + value.mPlus ) * ( 1.0f + value.mPercent ) );
	}

	dwTotalAttack = dwTotalAttack * ( 1 + wAttackC / 100 );


	pPlayer->mPhysicAttackMax = dwTotalAttack;
}

void CCharacterCalcManager::CalcCharPhyAttackMin( CPlayer* pPlayer )
{
	float wWeaponAttack  = 0;

	{
		const PlayerStat::Value& value = pPlayer->GetItemStats().mPhysicAttack;
		wWeaponAttack = value.mPlus * ( 1.0f + value.mPercent );
	}

	switch( pPlayer->GetWeaponEquipType() )
	{
	case eWeaponType_Dagger:
	case eWeaponType_Bow:
		{
			wWeaponAttack = wWeaponAttack * 0.9f;
		}
		break;
	case eWeaponType_Sword:
	case eWeaponType_Gun:
		{
			wWeaponAttack = wWeaponAttack * 0.7f;
		}
		break;
	case eWeaponType_Mace:
	case eWeaponType_Axe:
	case eWeaponType_Staff:
	case eWeaponType_BothDagger:
		{
			wWeaponAttack = wWeaponAttack * 0.8f;
		}
		break;
	}

	DWORD wEnchantAttack	= pPlayer->mWeaponEnchantLevel;
	DWORD wLevel			= pPlayer->GetLevel();
	float wPassiveSkill		= pPlayer->GetPassiveStatus()->PhysicAttack;
	float wAttackA			= wWeaponAttack + wEnchantAttack + wLevel + wPassiveSkill + 3;
	float wStrengthAll		= float( pPlayer->GetStrength() );

	wStrengthAll *= 0.2f;

	float wMasteryLevel	= 0 ;
	float wMasteryLevelRate = 0;

	switch( pPlayer->GetWeaponAniType() )
	{ 
		case eWeaponAnimationType_OneHand:	wMasteryLevel = pPlayer->GetPassiveStatus()->OneHanded; wMasteryLevelRate = pPlayer->GetRatePassiveStatus()->OneHanded;	break;
		case eWeaponAnimationType_TwoHand:	wMasteryLevel = pPlayer->GetPassiveStatus()->TwoHanded; wMasteryLevelRate = pPlayer->GetRatePassiveStatus()->TwoHanded;	break;
		case eWeaponAnimationType_TwoBlade:	wMasteryLevel = pPlayer->GetPassiveStatus()->TwoBlade; wMasteryLevelRate = pPlayer->GetRatePassiveStatus()->TwoBlade;	break;
	}

	switch( pPlayer->GetWeaponEquipType() )
	{
		case eWeaponType_Sword:		wMasteryLevel += pPlayer->GetPassiveStatus()->Sword; wMasteryLevelRate += pPlayer->GetRatePassiveStatus()->Sword;	break;
		case eWeaponType_Mace:		wMasteryLevel += pPlayer->GetPassiveStatus()->Mace; wMasteryLevelRate += pPlayer->GetRatePassiveStatus()->Mace;		break;
		case eWeaponType_Axe:		wMasteryLevel += pPlayer->GetPassiveStatus()->Axe; wMasteryLevelRate += pPlayer->GetRatePassiveStatus()->Axe;		break;
		case eWeaponType_Dagger:	wMasteryLevel = pPlayer->GetPassiveStatus()->Dagger; wMasteryLevelRate = pPlayer->GetRatePassiveStatus()->Dagger;	break;
		case eWeaponType_Staff:		wMasteryLevel = pPlayer->GetPassiveStatus()->Staff; wMasteryLevelRate = pPlayer->GetRatePassiveStatus()->Staff;		break;
		case eWeaponType_Bow:		wMasteryLevel = pPlayer->GetPassiveStatus()->Bow; wMasteryLevelRate = pPlayer->GetRatePassiveStatus()->Bow;			break;
		case eWeaponType_Gun:		wMasteryLevel = pPlayer->GetPassiveStatus()->Gun; wMasteryLevelRate = pPlayer->GetRatePassiveStatus()->Gun;			break;
	}

	float wAttackBuff		= pPlayer->GetBuffStatus()->PhysicAttack;
	float wAttackBuffRate	= pPlayer->GetRateBuffStatus()->PhysicAttack;
	float wPassiveSkillRate = pPlayer->GetRatePassiveStatus()->PhysicAttack;
	float wAttackB			= 1000 + (((wStrengthAll -10) * 20) + (wMasteryLevel * 25) + (wAttackBuff * 10));
	float wAttackC			= wAttackBuffRate + wPassiveSkillRate + wMasteryLevelRate;
	float dwTotalAttack		= ((wAttackA * wAttackB) / 1000);

	{
		const PlayerStat::Value value = pPlayer->GetSetItemStats().mPhysicAttack;
		dwTotalAttack = max( 0, ( dwTotalAttack + value.mPlus ) * ( 1.0f + value.mPercent ) );
	}

	dwTotalAttack = dwTotalAttack * ( 1 + wAttackC / 100 );

	pPlayer->mPhysicAttackMin = dwTotalAttack;
}

void CCharacterCalcManager::CalcCharPhyDefense( CPlayer* pPlayer ) 
{
	float wDefenseAll		= 0;

	{
		const PlayerStat::Value& value = pPlayer->GetItemStats().mPhysicDefense;

		wDefenseAll = value.mPlus * ( 1.0f + value.mPercent );
	}

	float wEnchantAll		= (float)pPlayer->mPhysicDefenseEnchantLevel ;
	float wLevel			= pPlayer->GetLevel() ;
	float wPassiveSkill		= pPlayer->GetPassiveStatus()->PhysicDefense;

	DWORD Armor = pPlayer->GetWearedItemIdx( eWearedItem_Dress );
	DWORD Boots = pPlayer->GetWearedItemIdx( eWearedItem_Shoes );
	DWORD Glove = pPlayer->GetWearedItemIdx( eWearedItem_Glove );

	ITEM_INFO* pArmor = ITEMMGR->GetItemInfo( Armor );
	ITEM_INFO* pBoots = ITEMMGR->GetItemInfo( Boots );
	ITEM_INFO* pGlove = ITEMMGR->GetItemInfo( Glove );

	if( pPlayer->m_HeroCharacterInfo.Job[0] == 2 )
	{
		if( ( pArmor && pArmor->ArmorType == eArmorType_Metal ) ||
			( pBoots && pBoots->ArmorType == eArmorType_Metal ) ||
			( pGlove && pGlove->ArmorType == eArmorType_Metal ) )
		{
			if( pPlayer->GetPassiveStatus()->HeavyArmor * ( 1.0f + pPlayer->GetRatePassiveStatus()->HeavyArmor ) < 1 )
				wDefenseAll = 0.f;
		}
	}
	else if( pPlayer->m_HeroCharacterInfo.Job[0] == 3 )
	{
		if( ( pArmor && pArmor->ArmorType == eArmorType_Metal ) ||
			( pBoots && pBoots->ArmorType == eArmorType_Metal ) ||
			( pGlove && pGlove->ArmorType == eArmorType_Metal ) )
		{
			if( pPlayer->GetPassiveStatus()->HeavyArmor * ( 1.0f + pPlayer->GetRatePassiveStatus()->HeavyArmor ) < 1 )
				wDefenseAll = 0.f;
		}
		else if( ( pArmor && pArmor->ArmorType == eArmorType_Leather ) ||
				 ( pBoots && pBoots->ArmorType == eArmorType_Leather ) ||
				 ( pGlove && pGlove->ArmorType == eArmorType_Leather ) )
		{
			if( pPlayer->GetPassiveStatus()->LightArmor * ( 1.0f + pPlayer->GetRatePassiveStatus()->LightArmor ) < 1 )
				wDefenseAll = 0.f;
		}
	}	


	float wDefenseA			= ( wDefenseAll / 2 ) + wEnchantAll + wLevel + wPassiveSkill ;
	float wVitalityAll		= float( pPlayer->GetVitality() );

	float wMasteryLevel		= 0;
	float wMasteryLevelRate	= 0;
	wVitalityAll *= 0.2f;

	if( pArmor && pBoots && pGlove )
	if( ( pArmor->ArmorType == pBoots->ArmorType ) && ( pArmor->ArmorType == pGlove->ArmorType ) )
	{
		switch( pArmor->ArmorType )
		{
			case eArmorType_Robe:		wMasteryLevel = pPlayer->GetPassiveStatus()->Robe; wMasteryLevelRate = pPlayer->GetRatePassiveStatus()->Robe;				break;
			case eArmorType_Leather:	wMasteryLevel = pPlayer->GetPassiveStatus()->LightArmor; wMasteryLevelRate = pPlayer->GetRatePassiveStatus()->LightArmor;	break;
			case eArmorType_Metal:		wMasteryLevel = pPlayer->GetPassiveStatus()->HeavyArmor; wMasteryLevelRate = pPlayer->GetRatePassiveStatus()->HeavyArmor;	break;
		}
	}

	float wDefenseBuff		= pPlayer->GetBuffStatus()->PhysicDefense;
	float wDefenseBuffRate	= pPlayer->GetRateBuffStatus()->PhysicDefense;
	float wPassiveSkillRate = pPlayer->GetRatePassiveStatus()->PhysicDefense;
	float wDefenseB			= 500 + (((wVitalityAll - 20) * 10) + (wMasteryLevel * 30) + (wDefenseBuff * 10)) ;
	float wDefenseC			= wDefenseBuffRate + wPassiveSkillRate + wMasteryLevelRate;
	float dwTotalDefense	= ((wDefenseA * wDefenseB) / 900)  ;

	{
		const PlayerStat::Value& value = pPlayer->GetSetItemStats().mPhysicDefense;
		dwTotalDefense = max( 0, ( dwTotalDefense + value.mPlus ) * ( 1.0f + value.mPercent ) );
	}

	dwTotalDefense = dwTotalDefense * ( 1 + wDefenseC / 100 );



	pPlayer->mPhysicDefense = dwTotalDefense;
}

void CCharacterCalcManager::CalcCharMagAttack( CPlayer* pPlayer ) 
{
	float wMagicAttack		= 0;

	{
		const PlayerStat::Value& value = pPlayer->GetItemStats().mMagicAttack;
		wMagicAttack = value.mPlus * ( 1.0f + value.mPercent );
	}

	float wEnchantAttack		= (float)pPlayer->mWeaponEnchantLevel ;
	float wLevel				= pPlayer->GetLevel() ;
	float wPassiveSkill			= pPlayer->GetPassiveStatus()->MagicAttack;
	float wAttackA				= wMagicAttack + wEnchantAttack + wLevel + wPassiveSkill ;
	float wIntelligenceAll		= float( pPlayer->GetIntelligence() );
	float wPassiveSkillRate		= pPlayer->GetRatePassiveStatus()->MagicAttack;
	float wAttackBuff			= pPlayer->GetBuffStatus()->MagicAttack;
	float wAttackBuffRate		= pPlayer->GetRateBuffStatus()->MagicAttack;

	wIntelligenceAll *= 0.2f;
	float wAttackB				= 1000 + (((wIntelligenceAll -10) * 35) + (wAttackBuff * 10)) ;
	float wAttackC				= wAttackBuffRate + wPassiveSkillRate;
	float dwTotalAttack			= ((wAttackA * wAttackB) / 350)  ;

	{
		const PlayerStat::Value& value = pPlayer->GetSetItemStats().mMagicAttack;
		dwTotalAttack = max( 0, ( dwTotalAttack + value.mPlus ) * ( 1.0f + value.mPercent ) );
	}

	dwTotalAttack = (dwTotalAttack)* ( 1 + wAttackC / 100 );
	dwTotalAttack *= 0.7f;


	pPlayer->mMagicAttack = dwTotalAttack;
}

void CCharacterCalcManager::CalcCharMagDefense( CPlayer* pPlayer ) 
{
	float wMagDefenseAll	= 0 ;

	{
		const PlayerStat::Value& value = pPlayer->GetItemStats().mMagicDefense;
		wMagDefenseAll = value.mPlus * ( 1.0f + value.mPercent );
	}

	float wEnchantAll		= (float)pPlayer->mMagicDefenseEnchantLevel ;
	float wLevel			= pPlayer->GetLevel() ;
	float wPassiveSkill		= pPlayer->GetPassiveStatus()->MagicDefense;
	float wPassiveSkillRate	= pPlayer->GetRatePassiveStatus()->MagicDefense;
	float wDefenseA			= ( wMagDefenseAll / 5 ) + wEnchantAll + wLevel + wPassiveSkill ;
	float wWisdomAll		= float( pPlayer->GetWisdom() );
	float wDefenseBuff		= pPlayer->GetBuffStatus()->MagicDefense;
	float wDefenseBuffRate	= pPlayer->GetRateBuffStatus()->MagicDefense;

	wWisdomAll *= 0.2f;
	float wDefenseB			= 500 + (((wWisdomAll - 20) * 15) + (wDefenseBuff * 10)) ;
	float wDefenseC			= wDefenseBuffRate + wPassiveSkillRate;
	float dwTotalDefense	= ((wDefenseA * wDefenseB) / 200)  ;

	{
		const PlayerStat::Value& value = pPlayer->GetSetItemStats().mMagicDefense;
		dwTotalDefense = max( 0, ( dwTotalDefense + value.mPlus ) * ( 1.0f + value.mPercent ) );
	}

	dwTotalDefense = dwTotalDefense * ( 1 + wDefenseC / 100 );


	pPlayer->mMagicDefense = dwTotalDefense;
}

void CCharacterCalcManager::CalcCharAccuracy( CPlayer* pPlayer ) 
{
	CHARACTER_TOTALINFO totalInfo ;
	pPlayer->GetCharacterTotalInfo(&totalInfo) ;

	BYTE byJobOrder		= totalInfo.Job[0] ;

	DWORD wLevel		= pPlayer->GetLevel() ;
	float wDexterityAll	= float( pPlayer->GetDexterity() );
	float wStrengthAll	= float( pPlayer->GetStrength() );

	DWORD wClassOrderVal	= 0 ;
	float wPassiveSkill		= pPlayer->GetPassiveStatus()->Accuracy;
	float wPassiveSkillRate = pPlayer->GetRatePassiveStatus()->Accuracy;
	float wBuffSkill		= pPlayer->GetBuffStatus()->Accuracy;
	float wBuffSkillRate	= pPlayer->GetRateBuffStatus()->Accuracy;

	wDexterityAll *= 0.2f;
	wStrengthAll *= 0.2f;

	switch( byJobOrder )
	{
		case 1 : wClassOrderVal = 20 ; break ;
		case 2 : wClassOrderVal = 25 ; break ;
		case 3 : wClassOrderVal = 15 ; break ;
		case 4 : wClassOrderVal = 30 ; break ;
	}

	float wAccuracy		= ( wDexterityAll / 1.4f ) + wLevel + wClassOrderVal + ( wStrengthAll / 4 );
	float wAccuracyA	= wPassiveSkillRate + wBuffSkillRate;

	float dwTotalAccuracy = wAccuracy + wPassiveSkill + wBuffSkill ;

	{
		const PlayerStat::Value& value = pPlayer->GetSetItemStats().mAccuracy;
		dwTotalAccuracy = max( 0, ( dwTotalAccuracy + value.mPlus ) * ( 1.0f + value.mPercent ) );
	}

	{
		const PlayerStat::Value& value = pPlayer->GetItemStats().mAccuracy;
		dwTotalAccuracy = ( dwTotalAccuracy + value.mPlus ) * ( 1.0f + value.mPercent );
	}

	dwTotalAccuracy = dwTotalAccuracy * ( 1 + wAccuracyA / 100 );


	pPlayer->mAccuracy = dwTotalAccuracy;
}

void CCharacterCalcManager::CalcCharEvasion( CPlayer* pPlayer ) 
{
	CHARACTER_TOTALINFO totalInfo ;
	pPlayer->GetCharacterTotalInfo(&totalInfo) ;

	BYTE byJobOrder		= totalInfo.Job[0] ;

	DWORD wLevel		= pPlayer->GetLevel() ;
	float wDexterityAll	= float( pPlayer->GetDexterity() );

	DWORD wClassOrderVal	= 0 ;
	float wPassiveSkill		= pPlayer->GetPassiveStatus()->Avoid;
	float wPassiveSkillRate = pPlayer->GetRatePassiveStatus()->Avoid;
	float wBuffSkill		= pPlayer->GetBuffStatus()->Avoid;
	float wBuffSkillRate	= pPlayer->GetRateBuffStatus()->Avoid;

	wDexterityAll *= 0.2f;

	switch( byJobOrder )
	{
		case 1 : wClassOrderVal = 20 ; break ;
		case 2 : wClassOrderVal = 25 ; break ;
		case 3 : wClassOrderVal = 15 ; break ;
		case 4 : wClassOrderVal = 30 ; break ;
	}

	float wEvasion			= ( wDexterityAll / 2 ) + wLevel + wClassOrderVal;
	float wEvasionA			= wPassiveSkillRate + wBuffSkillRate;

	float dwTotalEvasion	= wEvasion + wPassiveSkill + wBuffSkill;

	{
		const PlayerStat::Value& value = pPlayer->GetSetItemStats().mEvade;
		dwTotalEvasion = max( 0, ( dwTotalEvasion + value.mPlus ) * ( 1.0f + value.mPercent ) );
	}

	{
		const PlayerStat::Value& value = pPlayer->GetItemStats().mEvade;
		dwTotalEvasion = ( dwTotalEvasion + value.mPlus ) * ( 1.0f + value.mPercent );
	}

	dwTotalEvasion = dwTotalEvasion * ( 1 + wEvasionA / 100 );


	pPlayer->mAvoid = dwTotalEvasion;
}

void CCharacterCalcManager::CalcCharCriticalRate( CPlayer* pPlayer ) 
{
	float wDexterityAll	= float( pPlayer->GetDexterity() );

	float wPassiveSkill		= pPlayer->GetPassiveStatus()->CriticalRate;
	float wPassiveSkillRate = pPlayer->GetRatePassiveStatus()->CriticalRate;
	float wBuffSkill		= pPlayer->GetBuffStatus()->CriticalRate;
	float wBuffSkillRate	= pPlayer->GetRateBuffStatus()->CriticalRate;

	wDexterityAll *= 0.2f;

	DWORD wLevel		= pPlayer->GetLevel();
	float wCriticalA	= wBuffSkillRate + wPassiveSkillRate;
	float dwCritical	= 125 + wLevel + ((wDexterityAll - 15) * 7) + wPassiveSkill + wBuffSkill ;

	{
		const PlayerStat::Value& value = pPlayer->GetSetItemStats().mCriticalRate;
		dwCritical = max( 0, ( dwCritical + value.mPlus ) * ( 1.0f + value.mPercent ) );
	}

	{
		const PlayerStat::Value& value = pPlayer->GetItemStats().mCriticalRate;
		dwCritical = ( dwCritical + value.mPlus ) * ( 1.0f + value.mPercent );
	}

	dwCritical = dwCritical * ( 1 + wCriticalA / 100 );


	pPlayer->mCriticalRate = dwCritical;
}


void CCharacterCalcManager::CalcCharCriticalDamage( CPlayer* pPlayer ) 
{
	float wPassiveSkill		= pPlayer->GetPassiveStatus()->CriticalDamage;
	float wPassiveSkillRate = pPlayer->GetRatePassiveStatus()->CriticalDamage;
	float wBuffSkill		= pPlayer->GetBuffStatus()->CriticalDamage;
	float wBuffSkillRate	= pPlayer->GetRateBuffStatus()->CriticalDamage;

	float wCriticalDamageBuff	= (wBuffSkillRate > 0 ? (1-wBuffSkillRate) : 0) ;
	float wCriticalDamagePassive	= (wPassiveSkillRate > 0 ? (1-wPassiveSkillRate) : 0);
	float dwCriticalDamage	= wPassiveSkill + wBuffSkill;

	float	fPercent	= (pPlayer->GetSetItemStats().mCriticalDamage.mPercent > 0 ? 1-pPlayer->GetSetItemStats().mCriticalDamage.mPercent: 0);


	float itemPercent = pPlayer->GetItemStats().mCriticalDamage.mPercent;

	dwCriticalDamage = 1 - (1-wCriticalDamageBuff) * (1-wCriticalDamagePassive) * (1-fPercent) * (1-itemPercent);

	pPlayer->mCriticalDamage = dwCriticalDamage;
}

void CCharacterCalcManager::CalcCharShieldDefense(CPlayer* pPlayer)
{
	CItemSlot* slot = pPlayer->GetSlot( eItemTable_Weared );
	
	if( ! slot )
	{
		return;
	}
	
	const ITEMBASE*	itemBase = slot->GetItemInfoAbs( TP_WEAR_START + eWearedItem_Shield );
	
	if( ! itemBase )
	{
		return;
	}
	
	const ITEM_INFO* itemInfo = ITEMMGR->GetItemInfo( itemBase->wIconIdx );
	
	if( !	itemInfo ||
			itemInfo->Part3DType != ePartType_Shield )
	{
		pPlayer->SetShieldDefence( 0 );
		return;
	}
	
	const ITEM_OPTION& itemOption = ITEMMGR->GetOption( *itemBase );
	
	float defenseValue	= float( itemInfo->PhysicDefense ) + itemOption.mReinforce.mPhysicDefence;
	float defenseRate	= 1.0f;	
	
	{
		const ITEM_OPTION::Drop& drop = itemOption.mDrop;
	
		for( DWORD i = 0; i < sizeof( drop.mValue ) / sizeof( *drop.mValue ); ++i )
		{
			const ITEM_OPTION::Drop::Value& dropValue = drop.mValue[ i ];
	
			switch( dropValue.mKey )
			{
			case ITEM_OPTION::Drop::KeyPlusPhysicalDefence:
				{
					defenseValue += dropValue.mValue;
					break;
				}
			case ITEM_OPTION::Drop::KeyPercentPhysicalDefence:
				{
					defenseRate += dropValue.mValue;
					break;
				}
			}
		}
	}	
	
	{
		const EnchantScript* const script = g_CGameResourceManager.GetEnchantScript( itemOption.mEnchant.mIndex );
	
		if( script && itemOption.mEnchant.mLevel )
		{
			for(	EnchantScript::Ability::const_iterator it = script->mAbility.begin();
					script->mAbility.end() != it;
					++it )
			{
				switch( EnchantScript::eType( *it ) )
				{
				case ReinforceScript::eTypePhysicDefence:
					{
						defenseValue += GetBonusEnchantValue( *itemInfo, itemOption, itemInfo->PhysicDefense );
						break;
					}
				}
			}
		}
	}
	
	pPlayer->SetShieldDefence( DWORD( defenseValue * defenseRate ) );
}

void CCharacterCalcManager::CalcCharMagicalCriticalRate( CPlayer* pPlayer ) 
{
	float wIntelligenceAll	= float( pPlayer->GetIntelligence() );

	float wPassiveSkill		= pPlayer->GetPassiveStatus()->MagicCriticalRate;
	float wPassiveSkillRate = pPlayer->GetRatePassiveStatus()->MagicCriticalRate;
	float wBuffSkill		= pPlayer->GetBuffStatus()->MagicCriticalRate;
	float wBuffSkillRate	= pPlayer->GetRateBuffStatus()->MagicCriticalRate;

	wIntelligenceAll *= 0.6f;

	DWORD wLevel		= pPlayer->GetLevel();
	float dwCritical	= 125 + wLevel + (wIntelligenceAll) + wPassiveSkill + wBuffSkill ;

	float wCriticalA =  wBuffSkillRate + wPassiveSkillRate;

	{
		const PlayerStat::Value& value = pPlayer->GetSetItemStats().mMagicCriticalRate;
		dwCritical = max( 0, ( dwCritical + value.mPlus ) * ( 1.0f + value.mPercent ) );
	}

	{
		const PlayerStat::Value& value = pPlayer->GetItemStats().mMagicCriticalRate;
		dwCritical = ( dwCritical + value.mPlus ) * ( 1.0f + value.mPercent );
	}

	dwCritical = (dwCritical * 0.1f) +  wCriticalA;

	pPlayer->mMagicCriticalRate = dwCritical;
}

void CCharacterCalcManager::CalcCharMagicalCriticalDamage( CPlayer* pPlayer ) 
{
	float wPassiveSkill		= pPlayer->GetPassiveStatus()->MagicCriticalDamage;
	float wPassiveSkillRate = pPlayer->GetRatePassiveStatus()->MagicCriticalDamage;
	float wBuffSkill		= pPlayer->GetBuffStatus()->MagicCriticalDamage;
	float wBuffSkillRate	= pPlayer->GetRateBuffStatus()->MagicCriticalDamage;

	float wCriticalDamageA	= wBuffSkillRate + wPassiveSkillRate;
	float dwCriticalDamage	= wPassiveSkill + wBuffSkill;

	{
		const PlayerStat::Value& value = pPlayer->GetSetItemStats().mMagicCriticalDamage;
		dwCriticalDamage = max( 0, ( dwCriticalDamage + value.mPlus ) * ( 1.0f + value.mPercent ) );
	}

	{
		const PlayerStat::Value& value = pPlayer->GetItemStats().mMagicCriticalDamage;
		dwCriticalDamage = ( dwCriticalDamage + value.mPlus ) * ( 1.0f + value.mPercent );
	}

	dwCriticalDamage = dwCriticalDamage * ( 1 + wCriticalDamageA / 100 );

	pPlayer->mMagicCriticalDamage = dwCriticalDamage;
}

void CCharacterCalcManager::ProcessLife(CPlayer * pPlayer)
{
	if(pPlayer->GetState() == eObjectState_Die)
		return;

	DWORD life = pPlayer->GetLife();
	DWORD maxlife = pPlayer->GetMaxLife();
	LEVELTYPE level = pPlayer->GetLevel();

	if( life == maxlife )
	{
		return;
	}
	
	DWORD curTime = 0;
	if(!pPlayer->m_LifeRecoverTime.bStart)
	{
		curTime = gCurTime;
	}
	else
	{
		curTime = pPlayer->m_LifeRecoverTime.lastCheckTime = gCurTime;
		pPlayer->m_LifeRecoverTime.bStart = FALSE;
		return;
	}

	General_LifeCount(pPlayer, curTime, level, life, maxlife);
}

void CCharacterCalcManager::General_LifeCount(CPlayer * pPlayer, DWORD curTime, LEVELTYPE level, DWORD life, DWORD maxlife)
{
	DWORD TimeLength = 3000;

	if(curTime - pPlayer->m_LifeRecoverTime.lastCheckTime > TimeLength)
	{
		float dwVitalityAll		= pPlayer->GetVitality() * 0.2f;
		float dwOtherOptionVal	= pPlayer->GetPassiveStatus()->LifeRecoverRate + pPlayer->GetBuffStatus()->LifeRecoverRate ;

		DWORD dwUpLife			= 0;

		{
			const PlayerStat::Value& value = pPlayer->GetItemStats().mRecoveryLife;

			dwUpLife = DWORD( value.mPlus * ( 1.0f + value.mPercent ) );
		}

		if( dwOtherOptionVal <= 0 ) dwOtherOptionVal = 0 ;
		
		if(pPlayer->GetState() == eObjectState_Rest)
		{
			dwUpLife += (DWORD)( dwVitalityAll*3 + dwOtherOptionVal + 1000 ) ;
		}
		else
		{
			dwUpLife += (DWORD)( dwVitalityAll/2 + dwOtherOptionVal ) ;
		}

		{
			const PlayerStat::Value& value = pPlayer->GetSetItemStats().mRecoveryLife;

			dwUpLife = DWORD( max( 0, ( dwUpLife + value.mPlus ) * ( 1.0f + value.mPercent ) ) );
		}

		if( dwUpLife <= 0 )
		{
			dwUpLife = 1 ;
		}

		DWORD dwCalcLife = life + dwUpLife ;
				
		pPlayer->SetLife(dwCalcLife);
		pPlayer->m_LifeRecoverTime.lastCheckTime = curTime;
	}
}

void CCharacterCalcManager::ProcessPetLife(CPet * pPet)
{
	if(pPet->GetState() == eObjectState_Die)
		return;

	DWORD life = pPet->GetLife();
	DWORD maxlife = pPet->GetMaxLife();
	LEVELTYPE level = pPet->GetLevel();

	if( life == maxlife )
	{
		return;
	}
	
	DWORD curTime = 0;
	if(!pPet->m_LifeRecoverTime.bStart)
	{
		curTime = gCurTime;
	}
	else
	{
		curTime = pPet->m_LifeRecoverTime.lastCheckTime = gCurTime;
		pPet->m_LifeRecoverTime.bStart = FALSE;
		return;
	}

	General_PetLifeCount(pPet, curTime, level, life, maxlife);
}

void CCharacterCalcManager::General_PetLifeCount(CPet * pPet, DWORD curTime, LEVELTYPE level, DWORD life, DWORD maxlife)
{
	DWORD TimeLength = 3000;

	if(curTime - pPet->m_LifeRecoverTime.lastCheckTime > TimeLength)
	{
		float dwVitalityAll		= pPet->mPetStat.mVitality.mPlus * 0.2f;
		float dwOtherOptionVal	= pPet->GetBuffStatus()->LifeRecoverRate ;

		DWORD dwUpLife			= 0;

		if( dwOtherOptionVal <= 0 ) dwOtherOptionVal = 0 ;
		
		dwUpLife += (DWORD)( dwVitalityAll/2 + dwOtherOptionVal ) ;

		if( dwUpLife <= 0 )
		{
			dwUpLife = 1 ;
		}

		DWORD dwCalcLife = life + dwUpLife ;
				
		pPet->SetLife(dwCalcLife);
		pPet->m_LifeRecoverTime.lastCheckTime = curTime;
	}
}

void CCharacterCalcManager::General_LifeCountBoss(CBossMonster * pBoss, DWORD curTime, DWORD life, DWORD maxlife)
{
	// 	ùùùù ?ùù (5ù?ù) ¸ùù : ùù¸ùù 1%
	if(curTime - pBoss->m_LifeRecoverTime.lastCheckTime > 5000)
	{
		pBoss->SetLife(life+(DWORD)(maxlife*0.001f), TRUE);
		pBoss->m_LifeRecoverTime.lastCheckTime = curTime;
	}
}

void CCharacterCalcManager::ProcessLifeBoss(CBossMonster * pBoss)
{
	if(pBoss->GetState() == eObjectState_Die)
		return;

	DWORD life = pBoss->GetLife();
	DWORD maxlife = pBoss->GetMaxLife();

	if(life == maxlife) return;
	
	DWORD curTime = 0;
	if(!pBoss->m_LifeRecoverTime.bStart)
	{
		curTime = gCurTime;
	}
	else
	{
		curTime = pBoss->m_LifeRecoverTime.lastCheckTime = gCurTime;
		pBoss->m_LifeRecoverTime.bStart = FALSE;
		return;
	}

	General_LifeCountBoss(pBoss, curTime, life, maxlife);
}

void CCharacterCalcManager::ProcessMana(CPlayer * pPlayer)
{
	if(pPlayer->GetState() == eObjectState_Die)
		return;
	
	DWORD energy = pPlayer->GetMana();
	DWORD maxEnergy = pPlayer->GetMaxMana();
	LEVELTYPE level = pPlayer->GetLevel();

	if(energy == maxEnergy) return;

	DWORD curTime = 0;
	if(!pPlayer->m_ManaRecoverTime.bStart)
	{
		curTime = gCurTime;
	}
	else
	{
		curTime = pPlayer->m_ManaRecoverTime.lastCheckTime = gCurTime;
		pPlayer->m_ManaRecoverTime.bStart = FALSE;
		return;
	}

	General_ManaCount(pPlayer, curTime, level, energy, maxEnergy);
}

void CCharacterCalcManager::General_ManaCount(CPlayer* pPlayer, DWORD curTime, LEVELTYPE level, DWORD energy, DWORD maxEnergy)
{
	DWORD TimeLength = 5000;

	if(curTime - pPlayer->m_ManaRecoverTime.lastCheckTime > TimeLength)
	{
		float dwWisdomAll		= pPlayer->GetWisdom() * 0.2f;
		float dwOtherOptionVal	= pPlayer->GetPassiveStatus()->ManaRecoverRate + pPlayer->GetBuffStatus()->ManaRecoverRate ;
		DWORD dwUpMana			= 0;

		{
			const PlayerStat::Value& value = pPlayer->GetItemStats().mRecoveryMana;

			dwUpMana = DWORD( value.mPlus * ( 1.0f + value.mPercent ) );
		}

		if( dwOtherOptionVal <= 0 ) dwOtherOptionVal = 0 ;
		
		if( pPlayer->GetState() == eObjectState_Rest)
		{
			dwUpMana += (DWORD)( dwWisdomAll*2 + dwOtherOptionVal + 60) ;
		}
		else
		{
			dwUpMana += (DWORD)( dwWisdomAll/2+ dwOtherOptionVal ) ;
		}

		{
			const PlayerStat::Value& value = pPlayer->GetSetItemStats().mRecoveryMana;

			dwUpMana = DWORD( max( 0, ( dwUpMana + value.mPlus ) * ( 1.0f + value.mPercent ) ) );
		}

		if( dwUpMana <= 0 )  
		{
			dwUpMana = 1 ;
		}

		DWORD dwCalcMana = energy + dwUpMana ;
				
		pPlayer->SetMana(dwCalcMana);
		pPlayer->m_ManaRecoverTime.lastCheckTime = curTime;
	}
}


void CCharacterCalcManager::ProcessPetMana(CPet * pPet)
{
	if(pPet->GetState() == eObjectState_Die)
		return;
	
	DWORD energy = pPet->GetMana();
	DWORD maxEnergy = pPet->GetMaxMana();
	LEVELTYPE level = pPet->GetLevel();

	if(energy == maxEnergy) return;

	DWORD curTime = 0;
	if(!pPet->m_ManaRecoverTime.bStart)
	{
		curTime = gCurTime;
	}
	else
	{
		curTime = pPet->m_ManaRecoverTime.lastCheckTime = gCurTime;
		pPet->m_ManaRecoverTime.bStart = FALSE;
		return;
	}

	General_PetManaCount(pPet, curTime, level, energy, maxEnergy);
}

void CCharacterCalcManager::General_PetManaCount(CPet* pPet, DWORD curTime, LEVELTYPE level, DWORD energy, DWORD maxEnergy)
{
	DWORD TimeLength = 5000;

	if(curTime - pPet->m_ManaRecoverTime.lastCheckTime > TimeLength)
	{
		float dwWisdomAll		= pPet->mPetStat.mWisdom.mPlus * 0.2f;
		float dwOtherOptionVal	= pPet->GetBuffStatus()->ManaRecoverRate ;
		DWORD dwUpMana			= 0;

		if( dwOtherOptionVal <= 0 ) dwOtherOptionVal = 0 ;
		
		dwUpMana += (DWORD)( dwWisdomAll/2+ dwOtherOptionVal ) ;

		if( dwUpMana <= 0 )  
		{
			dwUpMana = 1 ;
		}

		DWORD dwCalcMana = energy + dwUpMana ;
				
		pPet->SetMana(dwCalcMana);
		pPet->m_ManaRecoverTime.lastCheckTime = curTime;
	}
}

void CCharacterCalcManager::Initialize( CPlayer* player )
{
	player->ResetSetItemStatus();

	PlayerStat& stat = player->GetItemStats();
	ZeroMemory( &stat, sizeof( stat ) );

	typedef std::map< const SetScript*, DWORD > SetItemSize;
	SetItemSize									setItemSize;

	float* pFishItemRate = player->GetFishItemRate();
	
	for(POSTYPE part = TP_WEAR_START ; part < TP_WEAR_END ; ++part )
	{
		const ITEMBASE* item = ITEMMGR->GetItemInfoAbsIn(player, part);

		if(0 == item)
		{
			continue;
		}

		const ITEM_INFO * info = ITEMMGR->GetItemInfo(item->wIconIdx);

		if( ! info )
		{
			continue;
		}
		else if(TP_WEAR_START + eWearedItem_Shield == part)
		{
			const BOOL isNoShield = (info->Part3DType != ePartType_Shield);
			const BOOL isUnableTwoBlade = (player->GetHeroTotalInfo()->bUsingTwoBlade == FALSE);

			if(isNoShield &&
				isUnableTwoBlade)
			{
				continue;
			}
		}
		{
			const SetScript* script = GAMERESRCMNGR->GetSetScript( item->wIconIdx );

			if( script )
			{
				++setItemSize[ script ];

				{
					const SetScript::Element& itemElement = GAMERESRCMNGR->GetItemElement( info->ItemIdx );

					for(	SetScript::Element::Skill::const_iterator skill_it = itemElement.mSkill.begin();
							itemElement.mSkill.end() != skill_it;
							++skill_it )
					{
						const DWORD	skillIndex	= skill_it->first;
						const BYTE	skillLevel	= skill_it->second;

						player->AddSetSkill(
							skillIndex,
							skillLevel );
					}

					SetPlayerStat(
						player->GetSetItemStats(),
						itemElement.mStat,
						SetValueTypeAdd );
				}
			}
		}		

		const ITEM_OPTION& option = ITEMMGR->GetOption( *item );
		AddStat( *info, stat );
		AddStat( *info, ITEMMGR->GetOption( *item ), stat );

		switch( info->EquipType )
		{
		case eEquipType_Weapon:
			{
				if( TP_WEAR_START + eWearedItem_Shield == part )
				{
					player->mWeaponEnchantLevel = (DWORD)((player->mWeaponEnchantLevel + option.mEnchant.mLevel) / 2.f);
				}
				else
				{
					player->mWeaponEnchantLevel += option.mEnchant.mLevel;
				}
			}
			break;
		case eEquipType_Armor:
			{
				switch( info->EquipSlot )
				{
				case eWearedItem_Dress:
				case eWearedItem_Hat:
				case eWearedItem_Glove:
				case eWearedItem_Shoes:
				case eWearedItem_Belt:
				case eWearedItem_Band:
					{
						player->mPhysicDefenseEnchantLevel += option.mEnchant.mLevel;
					}
					break;
				}
			}
			break;
		case eEquipType_Accessary:
			{
				switch( info->EquipSlot )
				{
				case eWearedItem_Ring1:
				case eWearedItem_Ring2:
				case eWearedItem_Necklace:
				case eWearedItem_Earring1:
				case eWearedItem_Earring2:
					{
						player->mMagicDefenseEnchantLevel += option.mEnchant.mLevel;
					}
					break;
				}
			}
			break;
		}

		// ùùùùùù ùùùùùù ùùùù ùùù˘ùùùùù?ùù ùùù
		stFishingRate* pFishingRate = FISHINGMGR->GetFishingUtilityRate(info->ItemIdx);
		if(pFishingRate)
		{
			int i;
			for(i=0; i<MAX_FISHITEM; i++)
			{
				if(pFishingRate->FishList[i].nGrade < 0)
					continue;

				pFishItemRate[pFishingRate->FishList[i].nGrade] += pFishingRate->FishList[i].fRate;

				if(99.0f < pFishItemRate[pFishingRate->FishList[i].nGrade])
					pFishItemRate[pFishingRate->FishList[i].nGrade] = 99.0f;
			}
		}

	}

	// ùù? ù?ù ùùùù. ùùùù ù?ùùùù ùùùùùù?ùù ùùùùù?ù ù?ù
	for(
		SetItemSize::const_iterator it = setItemSize.begin();
		setItemSize.end() != it;
		++it )
	{
		const SetScript*	script	= it->first;
		const DWORD			size	= it->second;

		for( DWORD i = 1; i <= size; ++i )
		{
			AddSetItemStats( player, *script, i );
		}
	}

	CalcCharStats( player );
}


void CCharacterCalcManager::CalcCharStats(CPlayer* player)
{
	PlayerStat& char_stats = player->GetCharStats();
	ZeroMemory( &char_stats, sizeof( char_stats ) );

	HERO_TOTALINFO pHeroInfo;
	player->GetHeroTotalInfo( &pHeroInfo );
	
	char_stats.mDexterity.mPlus		= float( pHeroInfo.Dex );
	char_stats.mStrength.mPlus		= float( pHeroInfo.Str );
	char_stats.mVitality.mPlus		= float( pHeroInfo.Vit  );
	char_stats.mIntelligence.mPlus	= float( pHeroInfo.Int  );
	char_stats.mWisdom.mPlus		= float( pHeroInfo.Wis  );

	Init_HPMP_Point( player ) ;
	CalcMaxLife( player ) ;
	CalcMaxMana( player ) ;
	CalcCharPhyAttackMax( player ) ;
	CalcCharPhyAttackMin( player ) ;
	CalcCharPhyDefense( player ) ;
	CalcCharMagAttack( player ) ;
	CalcCharMagDefense( player ) ;
	CalcCharAccuracy( player ) ;
	CalcCharEvasion( player ) ;
	CalcCharCriticalRate( player ) ;
	CalcCharCriticalDamage( player ) ;
	CalcCharMagicalCriticalRate( player ) ;
	CalcCharMagicalCriticalDamage( player ) ;
	CalcCharShieldDefense( player );
}


void CCharacterCalcManager::AddSetItemStats( CPlayer* player, const SetScript& script, int setItemSize )
{
	// ùù? ùùùùùùùù ùùùùùù ùùùù ùùùù?ù. ùù?ùù ùùùùù?ù ùùùùùù ù?ùùùù ù?ù, ùùùùù ùù? ùùùùùù ùùùùùù ùùùù ù?ùùù ùùù?ù ù?ùù?ù.

	// ùù? ùù?ùù?ùù ù?ùùù ùùùùùù ùùùùù?ù ùù ùùùùùùùù ùùùù
	// ùù? ùù?ùù?ùù ù?ùùù ù?ù?ù ùù? ùùùùùù ùùùù ùùùùùù ùùùùù?ù ù?ù ù?ù

	typedef CPlayer::SetItemLevel SetItemLevel;

	CPlayer::SetItemLevel& setLevel = player->GetSetItemLevel();

	int& level = setLevel[ &script ];

	ASSERT( size_t( level ) < script.mAbility.size() );
	SetScript::Ability::const_iterator it = script.mAbility.begin();
	std::advance( it, level );

	for(	;
		script.mAbility.end() != it;
		++it, ++level )
	{
		const int size = it->first;

		if( size > setItemSize )
		{
			break;
		}

		// ù?ùùù ù?ùùùùù
		{
			const SetScript::Element&	element = it->second;

			// 080313 LUJ, ùù? ùùùùùù ùùù?ù Ûùù
			AddStat( element.mStat, player->GetSetItemStats() );

			for(	SetScript::Element::Skill::const_iterator inner = element.mSkill.begin();
				element.mSkill.end() != inner;
				++inner )
			{
				const DWORD index	= inner->first;
				const BYTE	level	= BYTE( inner->second );

				player->AddSetSkill( index, level );
			}
		}
	}

	if( 0 == level )
	{
		setLevel.erase( &script );
	}
}

void CCharacterCalcManager::RemoveSetItemStats( CPlayer* player, const SetScript& script, int setItemSize )
{
	// ù?ù ù˜ùù?? ùùùù ùù? ùù?ùù?ùù ù?ùùù ùùùùùù ùùùùù¥ù.
	// ùùùùùù ù?ùùùù ùùùùù?ù ù?ùùù ùùùù. ùùùùùù ù?ù ùù
	// ?ùùùù ùù? ù?ùùù ùùùùùù ùùùù
	// ùù?ùù ùù?ùùùùùù ùù? ùùùù ùù˚

	typedef CPlayer::SetItemLevel SetItemLevel;

	CPlayer::SetItemLevel& setLevel = player->GetSetItemLevel();

	// ù?ù ùù?ùù?ùù ùùùùù ùùùùùù ùùù?ù ùù ùùùùù ùù? ù?ùùù ùùù?ù ùù
	if( setLevel.end() == setLevel.find( &script ) )
	{
		return;
	}

	CPlayer::SetItemLevel::iterator level_it = setLevel.find( &script );

	if( setLevel.end() == level_it )
	{
		return;
	}

	// 080602 LUJ, ùùùù ùùùù ùùùùùù ù???ù ùùùù ù?ù ùù ùùùùù
	int& level = level_it->second;

	if( level > int( script.mAbility.size() ) )
	{
		return;
	}

	SetScript::Ability::const_iterator it = script.mAbility.begin();
	std::advance( it, level - 1 );

	for( ;; )	
	{
		const int size = it->first;

		if( size <= setItemSize )
		{
			break;
		}

		// ù?ùùù ùùùùùùùù
		{
			const SetScript::Element&	element = it->second;
			
			// 080313 LUJ, ùù? ùùùùùù ùùù?ù Ûùù
			RemoveStat( element.mStat, player->GetSetItemStats() );

			for(
				SetScript::Element::Skill::const_iterator inner = element.mSkill.begin();
				element.mSkill.end() != inner;
				++inner )
			{
				player->RemoveSetSkill(
					inner->first,
					inner->second);
			}
		}

		--level;

        if (it == script.mAbility.begin())
        {
            break; 
        }
        --it;
	}


	if( ! level )
	{
		setLevel.erase( &script );
	}
}

void CCharacterCalcManager::AddItem( CPlayer* player, const ITEMBASE& item )
{
	PlayerStat& stat = player->GetItemStats();
	float* pFishItemRate = player->GetFishItemRate();
	

	const ITEM_INFO * info = ITEMMGR->GetItemInfo( item.wIconIdx );

	if( ! info )
	{
		return;
	}

	const ITEM_OPTION& option = ITEMMGR->GetOption( item );
	AddStat( *info, stat );
	AddStat( *info, ITEMMGR->GetOption( item ), stat );

	switch( info->EquipType )
	{
	case eEquipType_Weapon:
		{
			const ITEMBASE* weaponItem = ITEMMGR->GetItemInfoAbsIn( player, TP_WEAR_START + eWearedItem_Weapon );
			const ITEMBASE* shieldItem = ITEMMGR->GetItemInfoAbsIn( player, TP_WEAR_START + eWearedItem_Shield );
			
			const ITEM_INFO* weaponItemInfo	= ( weaponItem ? ITEMMGR->GetItemInfo( weaponItem->wIconIdx ) : 0 );
			const ITEM_INFO* shieldItemInfo	= ( shieldItem ? ITEMMGR->GetItemInfo( shieldItem->wIconIdx ) : 0 );

			DWORD weaponEnchantLevel = 0;

			if( weaponItemInfo != NULL )
			{
				const ITEM_OPTION& weaponItemOption = ITEMMGR->GetOption( *weaponItem );
				weaponEnchantLevel = weaponItemOption.mEnchant.mLevel;
				
			}

			if( shieldItemInfo != NULL && shieldItemInfo->EquipType == eEquipType_Weapon )
			{
				const ITEM_OPTION& shieldItemOption = ITEMMGR->GetOption( *shieldItem );
				weaponEnchantLevel = (DWORD)max((weaponEnchantLevel + shieldItemOption.mEnchant.mLevel) / 2.f, 0);
			}

			player->mWeaponEnchantLevel = weaponEnchantLevel;
		}
		break;
	case eEquipType_Armor:
		{
			switch( info->EquipSlot )
			{
			case eWearedItem_Dress:
			case eWearedItem_Hat:
			case eWearedItem_Glove:
			case eWearedItem_Shoes:
			case eWearedItem_Belt:
			case eWearedItem_Band:
				{
					player->mPhysicDefenseEnchantLevel += option.mEnchant.mLevel;
				}
				break;
			}
		}
		break;
	case eEquipType_Accessary:
		{
			switch( info->EquipSlot )
			{
			case eWearedItem_Ring1:
			case eWearedItem_Ring2:
			case eWearedItem_Necklace:
			case eWearedItem_Earring1:
			case eWearedItem_Earring2:
				{
					player->mMagicDefenseEnchantLevel += option.mEnchant.mLevel;
				}
				break;
			}
		}
		break;
	}

	{
		const SetScript* setScript = GAMERESRCMNGR->GetSetScript( item.wIconIdx );

		if( setScript )
		{
			AddSetItemStats( player, *setScript, ITEMMGR->GetSetItemSize( player, setScript ) );

			{
				const SetScript::Element& itemElement = GAMERESRCMNGR->GetItemElement( info->ItemIdx );

				for(	SetScript::Element::Skill::const_iterator skill_it = itemElement.mSkill.begin();
						itemElement.mSkill.end() != skill_it;
						++skill_it )
				{
					const DWORD	skillIndex	= skill_it->first;
					const BYTE	skillLevel	= skill_it->second;

					player->AddSetSkill(
						skillIndex,
						skillLevel );
				}

				SetPlayerStat(
					player->GetSetItemStats(),
					itemElement.mStat,
					SetValueTypeAdd );
			}
		}
	}

	// TODO : ùùùùùù ùùùù
	player->PassiveSkillCheckForWeareItem();
	CalcCharStats( player );	

	// ùùùùùù ùùùùùù ùùùù ùùù˘ùùùùù?ùù ùùù
	stFishingRate* pFishingRate = FISHINGMGR->GetFishingUtilityRate(info->ItemIdx);
	if(pFishingRate)
	{
		int i;
		for(i=0; i<MAX_FISHITEM; i++)
		{
			if(pFishingRate->FishList[i].nGrade < 0)
				continue;

			pFishItemRate[pFishingRate->FishList[i].nGrade] += pFishingRate->FishList[i].fRate;

			if(99.0f < pFishItemRate[pFishingRate->FishList[i].nGrade])
				pFishItemRate[pFishingRate->FishList[i].nGrade] = 99.0f;
		}
	}

	
}


void CCharacterCalcManager::RemoveItem( CPlayer* player, const ITEMBASE& item )
{
	const ITEM_INFO * info = ITEMMGR->GetItemInfo( item.wIconIdx );

	if( ! info )
	{
		return;
	}

	PlayerStat& stat = player->GetItemStats();
	float* pFishItemRate = player->GetFishItemRate();
	

	const ITEM_OPTION& option = ITEMMGR->GetOption( item );
	RemoveStat( *info, stat );
	RemoveStat( *info, ITEMMGR->GetOption( item ), stat );

	switch( info->EquipType )
	{
	case eEquipType_Weapon:
		{
			const ITEMBASE* weaponItem = ITEMMGR->GetItemInfoAbsIn( player, TP_WEAR_START + eWearedItem_Weapon );
			const ITEMBASE* shieldItem = ITEMMGR->GetItemInfoAbsIn( player, TP_WEAR_START + eWearedItem_Shield );
			
			const ITEM_INFO* weaponItemInfo	= ( weaponItem ? ITEMMGR->GetItemInfo( weaponItem->wIconIdx ) : 0 );
			const ITEM_INFO* shieldItemInfo	= ( shieldItem ? ITEMMGR->GetItemInfo( shieldItem->wIconIdx ) : 0 );

			DWORD weaponEnchantLevel = 0;

			if( weaponItemInfo != NULL )
			{
				const ITEM_OPTION& weaponItemOption = ITEMMGR->GetOption( *weaponItem );
				weaponEnchantLevel = weaponItemOption.mEnchant.mLevel;
				
			}

			if( shieldItemInfo != NULL && shieldItemInfo->EquipType == eEquipType_Weapon )
			{
				const ITEM_OPTION& shieldItemOption = ITEMMGR->GetOption( *shieldItem );
				weaponEnchantLevel = (DWORD)max((weaponEnchantLevel + shieldItemOption.mEnchant.mLevel) / 2.f, 0);
			}

			player->mWeaponEnchantLevel = weaponEnchantLevel;
		}
		break;
	case eEquipType_Armor:
		{
			switch( info->EquipSlot )
			{
			case eWearedItem_Dress:
			case eWearedItem_Hat:
			case eWearedItem_Glove:
			case eWearedItem_Shoes:
			case eWearedItem_Belt:
			case eWearedItem_Band:
				{
					player->mPhysicDefenseEnchantLevel -= option.mEnchant.mLevel;
				}
				break;
			}
		}
		break;
	case eEquipType_Accessary:
		{
			switch( info->EquipSlot )
			{
			case eWearedItem_Ring1:
			case eWearedItem_Ring2:
			case eWearedItem_Necklace:
			case eWearedItem_Earring1:
			case eWearedItem_Earring2:
				{
					player->mMagicDefenseEnchantLevel -= option.mEnchant.mLevel;
				}
				break;
			}
		}
		break;
	}

	{
		const SetScript* setScript = GAMERESRCMNGR->GetSetScript( item.wIconIdx );

		if( setScript )
		{
			RemoveSetItemStats( player, *setScript, ITEMMGR->GetSetItemSize( player, setScript ) );

			{
				const SetScript::Element& itemElement = GAMERESRCMNGR->GetItemElement( info->ItemIdx );

				for(	SetScript::Element::Skill::const_iterator skill_it = itemElement.mSkill.begin();
						itemElement.mSkill.end() != skill_it;
						++skill_it )
				{
					player->RemoveSetSkill(
						skill_it->first,
						skill_it->second);
				}

				SetPlayerStat(
					player->GetSetItemStats(),
					itemElement.mStat,
					SetValueTypeRemove );
			}
		}
	}

	// TODO : ùùùùùù ùùùù
	player->PassiveSkillCheckForWeareItem();
	CalcCharStats( player );
	
	// ùùùùùù ùùùùùù ùùùù ùùù˘ùùùùù?ùù ùùù
	stFishingRate* pFishingRate = FISHINGMGR->GetFishingUtilityRate(info->ItemIdx);
	if(pFishingRate)
	{
		int i;
		for(i=0; i<MAX_FISHITEM; i++)
		{
			if(pFishingRate->FishList[i].nGrade < 0)
				continue;

			pFishItemRate[pFishingRate->FishList[i].nGrade] -= pFishingRate->FishList[i].fRate;

			if(pFishItemRate[pFishingRate->FishList[i].nGrade] < 0.0f)
				pFishItemRate[pFishingRate->FishList[i].nGrade] = 0.0f;
		}
	}

	
}


void CCharacterCalcManager::AddStat( const ITEM_INFO& info, PlayerStat& stat )
{
	stat.mPhysicAttack.mPlus	+= float( info.PhysicAttack );
	stat.mMagicAttack.mPlus		+= float( info.MagicAttack );
	stat.mPhysicDefense.mPlus	+= float( info.PhysicDefense );
	stat.mMagicDefense.mPlus	+= float( info.MagicDefense );
	stat.mStrength.mPlus		+= float( info.ImprovementStr );
	stat.mDexterity.mPlus		+= float( info.ImprovementDex );
	stat.mVitality.mPlus 		+= float( info.ImprovementVit );
	stat.mWisdom.mPlus 			+= float( info.ImprovementWis );
	stat.mIntelligence.mPlus	+= float( info.ImprovementInt );
	stat.mLife.mPlus 			+= float( info.ImprovementLife );
	stat.mMana.mPlus 			+= float( info.ImprovementMana );
}


// 080319 LUJ, ù?ù ùùÊ? ùù?ùù ù?ùù?ù
// 080320 LUJ, ù?ùùù ùùùù ùùùù ùùùùùùùùùù ùù
void CCharacterCalcManager::AddStat( const ITEM_INFO& info, const ITEM_OPTION& option, PlayerStat& stat )
{
	const ITEM_OPTION::Reinforce&	reinforce	= option.mReinforce;
	const ITEM_OPTION::Mix&			mix			= option.mMix;

	stat.mPhysicAttack.mPlus	+= float( reinforce.mPhysicAttack );
	stat.mPhysicDefense.mPlus	+= float( reinforce.mPhysicDefence );
	stat.mMagicAttack.mPlus		+= float( reinforce.mMagicAttack );
	stat.mMagicDefense.mPlus	+= float( reinforce.mMagicDefence );
	stat.mStrength.mPlus		+= float( reinforce.mStrength		+ mix.mStrength );
	stat.mDexterity.mPlus		+= float( reinforce.mDexterity		+ mix.mDexterity );
	stat.mVitality.mPlus		+= float( reinforce.mVitality		+ mix.mVitality );
	stat.mWisdom.mPlus			+= float( reinforce.mWisdom			+ mix.mWisdom );
	stat.mIntelligence.mPlus	+= float( reinforce.mIntelligence	+ mix.mIntelligence );
	stat.mLife.mPlus			+= float( reinforce.mLife );
	stat.mMana.mPlus			+= float( reinforce.mMana );
	stat.mRecoveryLife.mPlus	+= float( reinforce.mLifeRecovery );
	stat.mRecoveryMana.mPlus	+= float( reinforce.mManaRecovery );
	stat.mCriticalRate.mPlus	+= float( reinforce.mCriticalRate );
	stat.mCriticalDamage.mPlus	+= float( reinforce.mCriticalDamage );
	stat.mAccuracy.mPlus		+= float( reinforce.mAccuracy );
	stat.mMoveSpeed.mPlus		+= float( reinforce.mMoveSpeed );
	stat.mEvade.mPlus			+= float( reinforce.mEvade );

	const ITEM_OPTION::Drop& drop = option.mDrop;

	for( DWORD i = 0; i < sizeof( drop.mValue ) / sizeof( *drop.mValue ); ++i )
	{
		const ITEM_OPTION::Drop::Value& value = drop.mValue[ i ];

		// 080410 LUJ, ù?ù ùùùùùù ùùù?ù ùùùù ù?ùùù ùùù?ù ù?ùùù ùùùùù?ù
		switch( value.mKey )
		{
		case ITEM_OPTION::Drop::KeyPlusStrength:
			{
				stat.mStrength.mPlus = Round( stat.mStrength.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusIntelligence:
			{
				stat.mIntelligence.mPlus = Round( stat.mIntelligence.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusDexterity:
			{
				stat.mDexterity.mPlus = Round( stat.mDexterity.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusWisdom:
			{
				stat.mWisdom.mPlus = Round( stat.mWisdom.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusVitality:
			{
				stat.mVitality.mPlus = Round( stat.mVitality.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusPhysicalAttack:
			{
				stat.mPhysicAttack.mPlus = Round( stat.mPhysicAttack.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusPhysicalDefence:
			{
				stat.mPhysicDefense.mPlus = Round( stat.mPhysicDefense.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusMagicalAttack:
			{
				stat.mMagicAttack.mPlus = Round( stat.mMagicAttack.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusMagicalDefence:
			{
				stat.mMagicDefense.mPlus = Round( stat.mMagicDefense.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusCriticalRate:
			{
				stat.mCriticalRate.mPlus = Round( stat.mCriticalRate.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusCriticalDamage:
			{
				stat.mCriticalDamage.mPlus = Round( stat.mCriticalDamage.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusAccuracy:
			{
				stat.mAccuracy.mPlus = Round( stat.mAccuracy.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusEvade:
			{
				stat.mEvade.mPlus = Round( stat.mEvade.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusMoveSpeed:
			{
				stat.mMoveSpeed.mPlus = Round( stat.mMoveSpeed.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusLife:
			{
				stat.mLife.mPlus = Round( stat.mLife.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusMana:
			{
				stat.mMana.mPlus = Round( stat.mMana.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusLifeRecovery:
			{
				stat.mRecoveryLife.mPlus = Round( stat.mRecoveryLife.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusManaRecovery:
			{
				stat.mRecoveryMana.mPlus = Round( stat.mRecoveryMana.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentStrength:
			{
				stat.mStrength.mPercent = Round( stat.mStrength.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentIntelligence:
			{
				stat.mIntelligence.mPercent = Round( stat.mIntelligence.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentDexterity:
			{
				stat.mDexterity.mPercent = Round( stat.mDexterity.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentWisdom:
			{
				stat.mWisdom.mPercent = Round( stat.mWisdom.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentVitality:
			{
				stat.mVitality.mPercent = Round( stat.mVitality.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentPhysicalAttack:
			{
				stat.mPhysicAttack.mPercent = Round( stat.mPhysicAttack.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentPhysicalDefence:
			{
				stat.mPhysicDefense.mPercent = Round( stat.mPhysicDefense.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentMagicalAttack:
			{
				stat.mMagicAttack.mPercent = Round( stat.mMagicAttack.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentMagicalDefence:
			{
				stat.mMagicDefense.mPercent = Round( stat.mMagicDefense.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentCriticalRate:
			{
				stat.mCriticalRate.mPercent = Round( stat.mCriticalRate.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentCriticalDamage:
			{
				stat.mCriticalDamage.mPercent = Round( stat.mCriticalDamage.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentAccuracy:
			{
				stat.mAccuracy.mPercent = Round( stat.mAccuracy.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentEvade:
			{
				stat.mEvade.mPercent = Round( stat.mEvade.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentMoveSpeed:
			{
				stat.mMoveSpeed.mPercent = Round( stat.mMoveSpeed.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentLife:
			{
				stat.mLife.mPercent = Round( stat.mLife.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentMana:
			{
				stat.mMana.mPercent = Round( stat.mMana.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentLifeRecovery:
			{
				stat.mRecoveryLife.mPercent = Round( stat.mRecoveryLife.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentManaRecovery:
			{
				stat.mRecoveryMana.mPercent = Round( stat.mRecoveryMana.mPercent + value.mValue, 3 );
				break;
			}
		}
	}

	const EnchantScript* script = g_CGameResourceManager.GetEnchantScript(
		option.mEnchant.mIndex);

	if(script && option.mEnchant.mLevel)
	{
		for(EnchantScript::Ability::const_iterator it = script->mAbility.begin();
			script->mAbility.end() != it;
			++it)
		{
			switch( EnchantScript::eType( *it ) )
			{
			case ReinforceScript::eTypeStrength:
				{
					stat.mStrength.mPlus += GetBonusEnchantValue( info, option, info.ImprovementStr );
					break;
				}
			case ReinforceScript::eTypeDexterity:
				{
					stat.mDexterity.mPlus += GetBonusEnchantValue( info, option, info.ImprovementDex );
					break;
				}
			case ReinforceScript::eTypeVitality:
				{
					stat.mVitality.mPlus += GetBonusEnchantValue( info, option, info.ImprovementVit );
					break;
				}
			case ReinforceScript::eTypeWisdom:
				{
					stat.mWisdom.mPlus += GetBonusEnchantValue( info, option, info.ImprovementWis );
					break;
				}
			case ReinforceScript::eTypeIntelligence:
				{
					stat.mIntelligence.mPlus += GetBonusEnchantValue( info, option, info.ImprovementInt );
					break;
				}
			case ReinforceScript::eTypeLife:
				{
					stat.mLife.mPlus += GetBonusEnchantValue( info, option, info.ImprovementLife );
					break;
				}
			case ReinforceScript::eTypeMana:
				{
					stat.mMana.mPlus += GetBonusEnchantValue( info, option, info.ImprovementMana );
					break;
				}
			case ReinforceScript::eTypePhysicAttack:
				{
					stat.mPhysicAttack.mPlus += GetBonusEnchantValue( info, option, info.PhysicAttack );
					break;
				}
			case ReinforceScript::eTypePhysicDefence:
				{
					stat.mPhysicDefense.mPlus += GetBonusEnchantValue( info, option, info.PhysicDefense );
					break;
				}
			case ReinforceScript::eTypeMagicAttack:
				{
					stat.mMagicAttack.mPlus += GetBonusEnchantValue( info, option, info.MagicAttack );
					break;
				}
			case ReinforceScript::eTypeMagicDefence:
				{
					stat.mMagicDefense.mPlus += GetBonusEnchantValue( info, option, info.MagicDefense );
					break;
				}
			}
		}
	}
}


void CCharacterCalcManager::AddStat( const PlayerStat& srcStat, PlayerStat& destStat )
{
	destStat.mPhysicAttack.mPercent		+= srcStat.mPhysicAttack.mPercent;
	destStat.mPhysicAttack.mPlus		+= srcStat.mPhysicAttack.mPlus;

	destStat.mMagicAttack.mPercent		+= srcStat.mMagicAttack.mPercent;
	destStat.mMagicAttack.mPlus			+= srcStat.mMagicAttack.mPlus;

	destStat.mPhysicDefense.mPercent	+= srcStat.mPhysicDefense.mPercent;
	destStat.mPhysicDefense.mPlus		+= srcStat.mPhysicDefense.mPlus;

	destStat.mMagicDefense.mPercent		+= srcStat.mMagicDefense.mPercent;
	destStat.mMagicDefense.mPlus		+= srcStat.mMagicDefense.mPlus;

	destStat.mStrength.mPercent			+= srcStat.mStrength.mPercent;
	destStat.mStrength.mPlus			+= srcStat.mStrength.mPlus;

	destStat.mDexterity.mPercent 		+= srcStat.mDexterity.mPercent;
	destStat.mDexterity.mPlus 			+= srcStat.mDexterity.mPlus;

	destStat.mVitality.mPercent			+= srcStat.mVitality.mPercent;
	destStat.mVitality.mPlus			+= srcStat.mVitality.mPlus;

	destStat.mIntelligence.mPercent 	+= srcStat.mIntelligence.mPercent;
	destStat.mIntelligence.mPlus 		+= srcStat.mIntelligence.mPlus;

	destStat.mWisdom.mPercent 			+= srcStat.mWisdom.mPercent;
	destStat.mWisdom.mPlus 				+= srcStat.mWisdom.mPlus;

	destStat.mLife.mPercent				+= srcStat.mLife.mPercent;
	destStat.mLife.mPlus				+= srcStat.mLife.mPlus;

	destStat.mMana.mPercent				+= srcStat.mMana.mPercent;
	destStat.mMana.mPlus				+= srcStat.mMana.mPlus;

	destStat.mRecoveryLife.mPercent	+= srcStat.mRecoveryLife.mPercent;
	destStat.mRecoveryLife.mPlus		+= srcStat.mRecoveryLife.mPlus;

	destStat.mRecoveryMana.mPercent		+= srcStat.mRecoveryMana.mPercent;
	destStat.mRecoveryMana.mPlus		+= srcStat.mRecoveryMana.mPlus;

	destStat.mAccuracy.mPercent			+= srcStat.mAccuracy.mPercent;
	destStat.mAccuracy.mPlus			+= srcStat.mAccuracy.mPlus;

	destStat.mEvade.mPercent			+= srcStat.mEvade.mPercent;
	destStat.mEvade.mPlus				+= srcStat.mEvade.mPlus;

	destStat.mCriticalRate.mPercent		+= srcStat.mCriticalRate.mPercent;
	destStat.mCriticalRate.mPlus		+= srcStat.mCriticalRate.mPlus;

	if(destStat.mCriticalDamage.mPercent == 0)
	{
		destStat.mCriticalDamage.mPercent	= (1 - srcStat.mCriticalDamage.mPercent);
	}
	else 
	{
		destStat.mCriticalDamage.mPercent	*= (1 - srcStat.mCriticalDamage.mPercent);
	}
	
	destStat.mCriticalDamage.mPlus		= srcStat.mCriticalDamage.mPlus;

	destStat.mMoveSpeed.mPercent		+= srcStat.mMoveSpeed.mPercent;
	destStat.mMoveSpeed.mPlus			+= srcStat.mMoveSpeed.mPlus;
}


void CCharacterCalcManager::RemoveStat( const ITEM_INFO& info, PlayerStat& stat )
{
	stat.mPhysicAttack.mPlus	-= float( info.PhysicAttack );
	stat.mMagicAttack.mPlus		-= float( info.MagicAttack );
	stat.mPhysicDefense.mPlus	-= float( info.PhysicDefense );
	stat.mMagicDefense.mPlus	-= float( info.MagicDefense );
	stat.mStrength.mPlus		-= float( info.ImprovementStr );
	stat.mDexterity.mPlus		-= float( info.ImprovementDex );
	stat.mVitality.mPlus 		-= float( info.ImprovementVit );
	stat.mWisdom.mPlus 			-= float( info.ImprovementWis );
	stat.mIntelligence.mPlus	-= float( info.ImprovementInt );
	stat.mLife.mPlus 			-= float( info.ImprovementLife );
	stat.mMana.mPlus 			-= float( info.ImprovementMana );
}


// 080319 LUJ, ù?ù ùùÊ? ùù?ùù ùùùùùù
// 080320 LUJ, ùùùùùù ù?ù ùùÊ? ùù?ùù ù?ùùù ùùùùùù
void CCharacterCalcManager::RemoveStat( const ITEM_INFO& info, const ITEM_OPTION& option, PlayerStat& stat )
{
	const ITEM_OPTION::Reinforce&	reinforce	= option.mReinforce;
	const ITEM_OPTION::Mix&			mix			= option.mMix;

	stat.mPhysicAttack.mPlus	-= float( reinforce.mPhysicAttack );
	stat.mPhysicDefense.mPlus	-= float( reinforce.mPhysicDefence );
	stat.mMagicAttack.mPlus		-= float( reinforce.mMagicAttack );
	stat.mMagicDefense.mPlus	-= float( reinforce.mMagicDefence );
	stat.mStrength.mPlus		-= float( reinforce.mStrength		+ mix.mStrength );
	stat.mDexterity.mPlus		-= float( reinforce.mDexterity		+ mix.mDexterity );
	stat.mVitality.mPlus		-= float( reinforce.mVitality		+ mix.mVitality );
	stat.mWisdom.mPlus			-= float( reinforce.mWisdom			+ mix.mWisdom );
	stat.mIntelligence.mPlus	-= float( reinforce.mIntelligence	+ mix.mIntelligence );
	stat.mLife.mPlus			-= float( reinforce.mLife );
	stat.mMana.mPlus			-= float( reinforce.mMana );
	stat.mRecoveryLife.mPlus	-= float( reinforce.mLifeRecovery );
	stat.mRecoveryMana.mPlus	-= float( reinforce.mManaRecovery );
	stat.mCriticalRate.mPlus	-= float( reinforce.mCriticalRate );
	stat.mCriticalDamage.mPlus	-= float( reinforce.mCriticalDamage );
	stat.mAccuracy.mPlus		-= float( reinforce.mAccuracy );
	stat.mMoveSpeed.mPlus		-= float( reinforce.mMoveSpeed );
	stat.mEvade.mPlus			-= float( reinforce.mEvade );

	const ITEM_OPTION::Drop& drop = option.mDrop;

	for( DWORD i = 0; i < sizeof( drop.mValue ) / sizeof( *drop.mValue ); ++i )
	{
		const ITEM_OPTION::Drop::Value& value = drop.mValue[ i ];

		// 080410 LUJ, ù?ù ùùùùùù ùùù?ù ùùùù ù?ùùù ùùù?ù ù?ùùù ùùùùù?ù
		switch( value.mKey )
		{
		case ITEM_OPTION::Drop::KeyPlusStrength:
			{
				stat.mStrength.mPlus = Round( stat.mStrength.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusIntelligence:
			{
				stat.mIntelligence.mPlus = Round( stat.mIntelligence.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusDexterity:
			{
				stat.mDexterity.mPlus = Round( stat.mDexterity.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusWisdom:
			{
				stat.mWisdom.mPlus = Round( stat.mWisdom.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusVitality:
			{
				stat.mVitality.mPlus = Round( stat.mVitality.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusPhysicalAttack:
			{
				stat.mPhysicAttack.mPlus = Round( stat.mPhysicAttack.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusPhysicalDefence:
			{
				stat.mPhysicDefense.mPlus = Round( stat.mPhysicDefense.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusMagicalAttack:
			{
				stat.mMagicAttack.mPlus = Round( stat.mMagicAttack.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusMagicalDefence:
			{
				stat.mMagicDefense.mPlus = Round( stat.mMagicDefense.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusCriticalRate:
			{
				stat.mCriticalRate.mPlus = Round( stat.mCriticalRate.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusCriticalDamage:
			{
				stat.mCriticalDamage.mPlus = Round( stat.mCriticalDamage.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusAccuracy:
			{
				stat.mAccuracy.mPlus = Round( stat.mAccuracy.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusEvade:
			{
				stat.mEvade.mPlus = Round( stat.mEvade.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusMoveSpeed:
			{
				stat.mMoveSpeed.mPlus = Round( stat.mMoveSpeed.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusLife:
			{
				stat.mLife.mPlus = Round( stat.mLife.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusMana:
			{
				stat.mMana.mPlus = Round( stat.mMana.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusLifeRecovery:
			{
				stat.mRecoveryLife.mPlus = Round( stat.mRecoveryLife.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusManaRecovery:
			{
				stat.mRecoveryMana.mPlus = Round( stat.mRecoveryMana.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentStrength:
			{
				stat.mStrength.mPercent = Round( stat.mStrength.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentIntelligence:
			{
				stat.mIntelligence.mPercent = Round( stat.mIntelligence.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentDexterity:
			{
				stat.mDexterity.mPercent = Round( stat.mDexterity.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentWisdom:
			{
				stat.mWisdom.mPercent = Round( stat.mWisdom.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentVitality:
			{
				stat.mVitality.mPercent = Round( stat.mVitality.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentPhysicalAttack:
			{
				stat.mPhysicAttack.mPercent = Round( stat.mPhysicAttack.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentPhysicalDefence:
			{
				stat.mPhysicDefense.mPercent = Round( stat.mPhysicDefense.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentMagicalAttack:
			{
				stat.mMagicAttack.mPercent = Round( stat.mMagicAttack.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentMagicalDefence:
			{
				stat.mMagicDefense.mPercent = Round( stat.mMagicDefense.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentCriticalRate:
			{
				stat.mCriticalRate.mPercent = Round( stat.mCriticalRate.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentCriticalDamage:
			{
				stat.mCriticalDamage.mPercent = Round( stat.mCriticalDamage.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentAccuracy:
			{
				stat.mAccuracy.mPercent = Round( stat.mAccuracy.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentEvade:
			{
				stat.mEvade.mPercent = Round( stat.mEvade.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentMoveSpeed:
			{
				stat.mMoveSpeed.mPercent = Round( stat.mMoveSpeed.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentLife:
			{
				stat.mLife.mPercent = Round( stat.mLife.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentMana:
			{
				stat.mMana.mPercent = Round( stat.mMana.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentLifeRecovery:
			{
				stat.mRecoveryLife.mPercent = Round( stat.mRecoveryLife.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentManaRecovery:
			{
				stat.mRecoveryMana.mPercent = Round( stat.mRecoveryMana.mPercent - value.mValue, 3 );
				break;
			}
		}
	}

	const EnchantScript* script = g_CGameResourceManager.GetEnchantScript(
		option.mEnchant.mIndex);

	if(script && option.mEnchant.mLevel)
	{
		for(EnchantScript::Ability::const_iterator it = script->mAbility.begin();
			script->mAbility.end() != it;
			++it)
		{
			switch( EnchantScript::eType( *it ) )
			{
			case ReinforceScript::eTypeStrength:
				{
					stat.mStrength.mPlus -= GetBonusEnchantValue( info, option, info.ImprovementStr );
					break;
				}
			case ReinforceScript::eTypeDexterity:
				{
					stat.mDexterity.mPlus -= GetBonusEnchantValue( info, option, info.ImprovementDex );
					break;
				}
			case ReinforceScript::eTypeVitality:
				{
					stat.mVitality.mPlus -= GetBonusEnchantValue( info, option, info.ImprovementVit );
					break;
				}
			case ReinforceScript::eTypeWisdom:
				{
					stat.mWisdom.mPlus -= GetBonusEnchantValue( info, option, info.ImprovementWis );
					break;
				}
			case ReinforceScript::eTypeIntelligence:
				{
					stat.mIntelligence.mPlus -= GetBonusEnchantValue( info, option, info.ImprovementInt );
					break;
				}
			case ReinforceScript::eTypeLife:
				{
					stat.mLife.mPlus -= GetBonusEnchantValue( info, option, info.ImprovementLife );
					break;
				}
			case ReinforceScript::eTypeMana:
				{
					stat.mMana.mPlus -= GetBonusEnchantValue( info, option, info.ImprovementMana );
					break;
				}
			case ReinforceScript::eTypePhysicAttack:
				{
					stat.mPhysicAttack.mPlus -= GetBonusEnchantValue( info, option, info.PhysicAttack );
					break;
				}
			case ReinforceScript::eTypePhysicDefence:
				{
					stat.mPhysicDefense.mPlus -= GetBonusEnchantValue( info, option, info.PhysicDefense );
					break;
				}
			case ReinforceScript::eTypeMagicAttack:
				{
					stat.mMagicAttack.mPlus -= GetBonusEnchantValue( info, option, info.MagicAttack );
					break;
				}
			case ReinforceScript::eTypeMagicDefence:
				{
					stat.mMagicDefense.mPlus -= GetBonusEnchantValue( info, option, info.MagicDefense );
					break;
				}
			}
		}
	}
}


void CCharacterCalcManager::RemoveStat( const PlayerStat& srcStat, PlayerStat& destStat )
{
	destStat.mPhysicAttack.mPercent		-= srcStat.mPhysicAttack.mPercent;
	destStat.mPhysicAttack.mPlus		-= srcStat.mPhysicAttack.mPlus;

	destStat.mMagicAttack.mPercent		-= srcStat.mMagicAttack.mPercent;
	destStat.mMagicAttack.mPlus			-= srcStat.mMagicAttack.mPlus;

	destStat.mPhysicDefense.mPercent	-= srcStat.mPhysicDefense.mPercent;
	destStat.mPhysicDefense.mPlus		-= srcStat.mPhysicDefense.mPlus;

	destStat.mMagicDefense.mPercent		-= srcStat.mMagicDefense.mPercent;
	destStat.mMagicDefense.mPlus		-= srcStat.mMagicDefense.mPlus;

	destStat.mStrength.mPercent			-= srcStat.mStrength.mPercent;
	destStat.mStrength.mPlus			-= srcStat.mStrength.mPlus;

	destStat.mDexterity.mPercent 		-= srcStat.mDexterity.mPercent;
	destStat.mDexterity.mPlus 			-= srcStat.mDexterity.mPlus;

	destStat.mVitality.mPercent			-= srcStat.mVitality.mPercent;
	destStat.mVitality.mPlus			-= srcStat.mVitality.mPlus;

	destStat.mIntelligence.mPercent 	-= srcStat.mIntelligence.mPercent;
	destStat.mIntelligence.mPlus 		-= srcStat.mIntelligence.mPlus;

	destStat.mWisdom.mPercent 			-= srcStat.mWisdom.mPercent;
	destStat.mWisdom.mPlus 				-= srcStat.mWisdom.mPlus;

	destStat.mLife.mPercent				-= srcStat.mLife.mPercent;
	destStat.mLife.mPlus				-= srcStat.mLife.mPlus;

	destStat.mMana.mPercent				-= srcStat.mMana.mPercent;
	destStat.mMana.mPlus				-= srcStat.mMana.mPlus;

	destStat.mRecoveryLife.mPercent	-= srcStat.mRecoveryLife.mPercent;
	destStat.mRecoveryLife.mPlus		-= srcStat.mRecoveryLife.mPlus;

	destStat.mRecoveryMana.mPercent		-= srcStat.mRecoveryMana.mPercent;
	destStat.mRecoveryMana.mPlus		-= srcStat.mRecoveryMana.mPlus;

	destStat.mAccuracy.mPercent			-= srcStat.mAccuracy.mPercent;
	destStat.mAccuracy.mPlus			-= srcStat.mAccuracy.mPlus;

	destStat.mEvade.mPercent			-= srcStat.mEvade.mPercent;
	destStat.mEvade.mPlus				-= srcStat.mEvade.mPlus;

	destStat.mCriticalRate.mPercent		-= srcStat.mCriticalRate.mPercent;
	destStat.mCriticalRate.mPlus		-= srcStat.mCriticalRate.mPlus;


	destStat.mCriticalDamage.mPercent	= destStat.mCriticalDamage.mPercent / ( 1 - srcStat.mCriticalDamage.mPercent);
	destStat.mCriticalDamage.mPlus		-= srcStat.mCriticalDamage.mPlus;

	destStat.mMagicCriticalRate.mPercent		-= srcStat.mMagicCriticalRate.mPercent;
	destStat.mMagicCriticalRate.mPlus			-= srcStat.mMagicCriticalRate.mPlus;

	destStat.mMagicCriticalDamage.mPercent	-= srcStat.mMagicCriticalDamage.mPercent;
	destStat.mMagicCriticalDamage.mPlus		-= srcStat.mMagicCriticalDamage.mPlus;

	destStat.mMoveSpeed.mPercent		-= srcStat.mMoveSpeed.mPercent;
	destStat.mMoveSpeed.mPlus			-= srcStat.mMoveSpeed.mPlus;
}

BOOL CCharacterCalcManager::IsInvalidEquip( CPlayer* pPlayer ) const
{
	DWORD Armor = pPlayer->GetWearedItemIdx( eWearedItem_Dress );
	DWORD Boots = pPlayer->GetWearedItemIdx( eWearedItem_Shoes );
	DWORD Glove = pPlayer->GetWearedItemIdx( eWearedItem_Glove );

	ITEM_INFO* pArmor = ITEMMGR->GetItemInfo( Armor );
	ITEM_INFO* pBoots = ITEMMGR->GetItemInfo( Boots );
	ITEM_INFO* pGlove = ITEMMGR->GetItemInfo( Glove );

	if( pPlayer->m_HeroCharacterInfo.Job[0] == 2 )
	{
		if( ( pArmor && pArmor->ArmorType == eArmorType_Metal ) ||
			( pBoots && pBoots->ArmorType == eArmorType_Metal ) ||
			( pGlove && pGlove->ArmorType == eArmorType_Metal ) )
		{
			if( pPlayer->GetPassiveStatus()->HeavyArmor < 1 )
				return TRUE;
		}
	}
	else if( pPlayer->m_HeroCharacterInfo.Job[0] == 3 )
	{
		if( ( pArmor && pArmor->ArmorType == eArmorType_Metal ) ||
			( pBoots && pBoots->ArmorType == eArmorType_Metal ) ||
			( pGlove && pGlove->ArmorType == eArmorType_Metal ) )
		{
			if( pPlayer->GetPassiveStatus()->HeavyArmor < 1 )
				return TRUE;
		}
		else if( ( pArmor && pArmor->ArmorType == eArmorType_Leather ) ||
				 ( pBoots && pBoots->ArmorType == eArmorType_Leather ) ||
				 ( pGlove && pGlove->ArmorType == eArmorType_Leather ) )
		{
			if( pPlayer->GetPassiveStatus()->LightArmor < 1 )
				return TRUE;
		}
	}

	return FALSE;
}

void CCharacterCalcManager::ArrangeCharLevelPoint(CPlayer * pPlayer, WORD type)
{
	if(pPlayer->GetPlayerLevelUpPoint() <= 0)
	{
		pPlayer->SetPlayerLevelUpPoint(0);
		return;
	}

	pPlayer->SetPlayerLevelUpPoint(
		pPlayer->GetPlayerLevelUpPoint() - 1);

	switch(type)
	{
	case STR_POINT:
		{
			pPlayer->AddStrength(1);
			break;
		}
	case WIS_POINT:
		{
			pPlayer->AddWisdom(1);
			break;
		}
	case DEX_POINT:
		{
			pPlayer->AddDexterity(1);
			break;
		}
	case VIT_POINT:
		{
			pPlayer->AddVitality(1);
			break;
		}
	case INT_POINT :
		{
			pPlayer->AddIntelligence(1);
			break;
		}
	}

	InsertLogCharacter(
		pPlayer->GetID(),
		pPlayer->GetLevel(),
		pPlayer->GetHeroTotalInfo());
}