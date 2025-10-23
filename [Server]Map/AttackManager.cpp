#include "StdAfx.h"
#include "AttackManager.h"
#include "Network.h"
#include "PackedData.h"
#include "ServerSystem.h"
#include "CharMove.h"
#include "UserTable.h"
#include "ObjectStateManager.h"
#include "Monster.h"
#include "PartyManager.h"
#include "Party.h"
#include "Player.h"
#include "GuildManager.h"
#include "pet.h"
#include "petmanager.h"

#include "SiegeWarfareMgr.h"

#include "GuildTournamentMgr.h"
#include "ItemManager.h"

#include "..\[CC]Header\CommonCalcFunc.h"
// 080616 LUJ, ?? ??????? ???????? ??
#include "Event.h"

CAttackManager::CAttackManager()
{
	
	m_PvpPhysicDamageRatio = 80;
	m_PvpMagicDamageRatio = 80;
	m_GTDamageRatio = 10.0f;
}

CAttackManager::~CAttackManager()
{

}

void CAttackManager::sendDieMsg(CObject * pAttacker,CObject* pTarget)
{
	MSG_DWORD2 m2c;
	m2c.Category	= MP_USERCONN;

	if(pTarget->GetObjectKind() & eObjectKind_Monster)
	{
		m2c.Protocol	= MP_USERCONN_MONSTER_DIE;

		m2c.dwObjectID	= pAttacker->GetID();
		m2c.dwData1		= 0;
		m2c.dwData2		= pTarget->GetID();
		
		PACKEDDATA_OBJ->QuickSend(pTarget,&m2c,sizeof(m2c));
	}
	else if(pTarget->GetObjectKind() == eObjectKind_Player)
	{
		m2c.Protocol = MP_USERCONN_CHARACTER_DIE;

		m2c.dwObjectID	= pAttacker->GetID();
		m2c.dwData1		= pAttacker->GetID();
		m2c.dwData2		= pTarget->GetID();
		
		PACKEDDATA_OBJ->QuickSend(pTarget,&m2c,sizeof(m2c));
	}
	else if(pTarget->GetObjectKind() & eObjectKind_Pet)
	{
		// 091110 ONS ? ???????
		m2c.Protocol = MP_USERCONN_PET_DIE;

		m2c.dwObjectID	= pAttacker->GetID();
		m2c.dwData1		= pAttacker->GetID();
		m2c.dwData2		= pTarget->GetID();
		
		PACKEDDATA_OBJ->QuickSend(pTarget,&m2c,sizeof(m2c));
	}
}

void CAttackManager::RecoverLife(CObject* pOperator,CObject* pObject, float RecoverLifeVal,RESULTINFO* pResultInfo)
/*{
	DWORD realAddVal = 0;
	DWORD val = 0;	//KES 070918 val; --> val = 0;  (val? ???? ?·?AddLife? ? ?????. RecoverMana()?????)

	// 100219 ShinJS --- ?? ?? ?? 
	// ? ??? = ( ( (?? * 11) + ( ?? * 4 ) + ( ?? * 20 ) ) * ???? ) / 800 + 100
	if( pOperator->GetObjectKind() == eObjectKind_Player )
	{
		CPlayer* pPlayer = (CPlayer*)pOperator;
		val = ( DWORD )( ( ( ( pPlayer->GetWisdom() * 11 ) + ( pPlayer->GetIntelligence() * 4 ) + ( pPlayer->GetLevel() * 20 ) ) * RecoverLifeVal ) / 800.f + 100.f );
	}
	else if( pOperator->GetObjectKind() == eObjectKind_Pet )
	{
		PlayerStat& pStat = ((CPet*)pOperator)->GetStat();
		val = ( DWORD )( ( ( ( pStat.mWisdom.mPlus * 11 ) + ( pStat.mIntelligence.mPlus * 4 ) + ( pOperator->GetLevel() * 20 ) ) * RecoverLifeVal ) / 800.f + 100.f );
	}
	else if(pObject->GetObjectKind() & eObjectKind_Monster)
	{
		val = DWORD(RecoverLifeVal);
	}

	// 080728 KTH -- ??? ?????? ??
	if( SIEGEWARFAREMGR->Is_CastleMap() == FALSE &&
  		SIEGEWARFAREMGR->IsSiegeWarfareZone() &&
  		SIEGEWARFAREMGR->GetState() > eSiegeWarfare_State_Before )
  	{
  		val = DWORD( val * (SIEGEWARFAREMGR->GetDecreaseHealRatio() / 100) );
  		
  		if( val < 1 )
  			val = 1;
  	}

	if(g_pServerSystem->GetMapNum() == GTMAPNUM)
		val = DWORD( val * 0.5f );	// ???? ??? 50%?

	pObject->AddLife( val, &realAddVal, FALSE );
	pResultInfo->HealLife = realAddVal;

	if( pOperator->GetObjectKind() == eObjectKind_Player||
		pOperator->GetObjectKind() == eObjectKind_Pet)
	{
		if(pObject->GetObjectKind() == eObjectKind_Player)
		{
			((CPlayer*)pObject)->AddAggroToMyMonsters(
				int(realAddVal) / 3,
				pOperator->GetID(),
				pResultInfo->mSkillIndex);
		}
		else if(pObject->GetObjectKind() == eObjectKind_Pet)
		{
			((CPet*)pObject)->AddAggroToMyMonsters(
				int(realAddVal) / 3,
				pOperator->GetID(),
				pResultInfo->mSkillIndex);
		}
	}

	// 090204 LUJ, ??? ??? ????? ??????
	pOperator->RemoveBuffCount( eBuffSkillCountType_HealVolumn, val );
	pObject->RemoveBuffCount( eBuffSkillCountType_BeHealedVolumn, val );
*/
{
	DWORD realAddVal = 0;
	DWORD val = 0;	//KES 070918 val; --> val = 0;  (val? ???? ?·?AddLife? ? ?????. RecoverMana()?????)

	if( pOperator->GetObjectKind() == eObjectKind_Player )
	{
		float fPlus = ( ( CPlayer* )pOperator )->GetMagicAttack() / 10;
		val = ( DWORD )( RecoverLifeVal + fPlus );

		//---KES Aggro 070918
		//???? 50%? ???????? (?? ???? ??? ??????? 50%?. ????? ??? ????????? ???)
		//?? ???? ???????????
		//if( val && pObject->GetObjectKind() == eObjectKind_Player )
		//{
		//	((CPlayer*)pObject)->AddAggroToMyMonsters( (int)RecoverLifeVal / 2, (CPlayer*)pOperator );
		//}
		//-------------------
	}
	else if( pOperator->GetObjectKind() == eObjectKind_Pet )
	{
		float fPlus = pOperator->GetMagicAttackPower() / 10;
		val = ( DWORD )( RecoverLifeVal + fPlus );
	}

	if( SIEGEWARFAREMGR->Is_CastleMap() == FALSE &&
  		SIEGEWARFAREMGR->IsSiegeWarfareZone() &&
  		SIEGEWARFAREMGR->GetState() > eSiegeWarfare_State_Before )
  	{
  		val = DWORD( val * (SIEGEWARFAREMGR->GetDecreaseHealRatio() / 100) );
  		
  		if( val < 1 )
  			val = 1;
  	}

	if(g_pServerSystem->GetMapNum() == GTMAPNUM)
		val = DWORD( val * 0.5f );	// ???? ??? 50%?

	pObject->AddLife( val, &realAddVal, FALSE );
	pResultInfo->HealLife = realAddVal;

	//---KES Aggro 071005
	//?? ???? 1/3 ? ????????.
	if(pObject->GetObjectKind() == eObjectKind_Player)
		{
			((CPlayer*)pObject)->AddAggroToMyMonsters(
				int(realAddVal) / 3,
				pOperator->GetID(),
				pResultInfo->mSkillIndex);
		}
		else if(pObject->GetObjectKind() == eObjectKind_Pet)
		{
			((CPet*)pObject)->AddAggroToMyMonsters(
				int(realAddVal) / 3,
				pOperator->GetID(),
				pResultInfo->mSkillIndex);
		}
}

void CAttackManager::RecoverMana(CObject* pOperator,CObject* pObject, float RecoverManaVal,RESULTINFO* pResultInfo)
{
	DWORD realAddVal = 0;
	DWORD val = 0;

	if( pOperator->GetObjectKind() == eObjectKind_Player )
	{
		// ?? ??? = ( ( (?? * 11) + ( ?? * 4 ) + ( ?? * 20 ) ) * ???? ) / 800 + 100
		CPlayer* pPlayer = (CPlayer*)pOperator;
		val = ( DWORD )( ( ( (pPlayer->GetWisdom() * 11) + ( pPlayer->GetIntelligence() * 4 ) + ( pPlayer->GetLevel() * 20 ) ) * RecoverManaVal ) / 800.f + 100.f );
	}
	else if( pOperator->GetObjectKind() == eObjectKind_Pet )
	{
		PlayerStat& pStat = ((CPet*)pOperator)->GetStat();
		val = ( DWORD )( ( ( ( pStat.mWisdom.mPlus * 11 ) + ( pStat.mIntelligence.mPlus * 4 ) + ( pOperator->GetLevel() * 20 ) ) * RecoverManaVal ) / 800.f + 100.f );
	}
	pObject->AddMana( val, &realAddVal );
	pResultInfo->RechargeMana = realAddVal;
}

void CAttackManager::PhysicAttack( CObject* pAttacker, CObject* pTarget, RESULTINFO* pDamageInfo, float Accuracy, float AddDamage, WORD AddType )
{
	pDamageInfo->Clear();

	if( !pAttacker || !pTarget )
		return;

	if( pTarget->GetAbnormalStatus()->IsGod )
	{
		return;
	}
	

	float Avoid = 0;
	float Attack = 1;
	float Defense = 1;
	float Shield = 0;
	float Block = 0;
	float CriticalRate = 0;
	float CriticalDamage = 0;
	float Strength = 0;
	float Dexterity = 0;
	WORD ALevel = pAttacker->GetLevel();
	WORD DLevel = pTarget->GetLevel();
	BOOL IsSpecial = 0;
	float vitality = 0;

	if( pAttacker->GetObjectKind() == eObjectKind_Player )
	{
		CPlayer* pPlayer = (CPlayer*)pAttacker;
		
		Accuracy += pPlayer->GetAccuracy();

		int min = ( int )( pPlayer->GetPhysicAttackMin() );
		int max = ( int )( pPlayer->GetPhysicAttackMax() );

		if( max > min )
		{
			Attack = float(random( min, max ));
		}
		else
		{
			Attack = (float)min;
		}

		CriticalRate = pPlayer->GetCriticalRate();
		CriticalDamage = pPlayer->GetCriticalDamage();
		Strength = (float)pPlayer->GetStrength();
		Dexterity = (float)pPlayer->GetDexterity();
	}
	else if( pAttacker->GetObjectKind() & eObjectKind_Monster )
	{
		CMonster* pMonster = (CMonster*)pAttacker;
		const BASE_MONSTER_LIST& pInfo = pMonster->GetSMonsterList();

		float accuracy = pInfo.Accuracy + pMonster->GetBuffStatus()->Accuracy;
		if( accuracy > 0 )
		{
			Accuracy = accuracy;
		}

		float min = pInfo.AttackPhysicalMin + pMonster->GetBuffStatus()->PhysicAttack;
		float max = pInfo.AttackPhysicalMax + pMonster->GetBuffStatus()->PhysicAttack;

		if( min < 0 ) min = 0;
		if( max < 0 ) max = 0;

		Attack = random( min, max );
		Attack *= 1.2f;
		
		float criticalRate = pInfo.CriticalPercent + pMonster->GetBuffStatus()->CriticalRate;

		if( criticalRate )
		{
			CriticalRate = criticalRate;
		}
	}
	else if( pAttacker->GetObjectKind() == eObjectKind_Pet )
	{
		CPet* pPet = ( CPet* )pAttacker;

		PlayerStat& pStat = pPet->GetStat();

		Accuracy += pStat.mAccuracy.mPlus;

		int min = ( int )( pStat.mPhysicAttack.mPlus );
		int max = ( int )( pStat.mPhysicAttack.mPlus );

		if( max > min )
		{
			Attack = float(random( min, max ));
		}
		else
		{
			Attack = (float)min;
		}

		//Attack *= PETMGR->GetFriendlyPenalty( pPet );

		CriticalRate = pStat.mCriticalRate.mPlus;
		CriticalDamage = pStat.mCriticalDamage.mPlus;
		Dexterity = pStat.mDexterity.mPlus;
	}
	else
	{
		ASSERT(0);
	}

	if( pTarget->GetObjectKind() == eObjectKind_Player )
	{
		CPlayer* pPlayer = (CPlayer*)pTarget;
		
		Avoid = pPlayer->GetAvoid();
		Defense = pPlayer->GetPhysicDefense();
		Block = pPlayer->GetBlock();
		Shield		= (float)pPlayer->GetShieldDefense();
		vitality	= (float)pPlayer->GetVitality();

		if( pAttacker->GetObjectKind() == eObjectKind_Player )
		{
			Defense = Defense;
			CPlayer* pAttackPlayer = (CPlayer*)pAttacker;
			if( pAttackPlayer->IsPKMode())
				pAttackPlayer->SetPKStartTimeReset();

			if( pPlayer->IsPKMode() )
				pPlayer->SetPKStartTimeReset();
		}
	}
	else if( pTarget->GetObjectKind() & eObjectKind_Monster )
	{
		CMonster* pMonster = (CMonster*)pTarget;
		const BASE_MONSTER_LIST& pInfo = pMonster->GetSMonsterList();

		IsSpecial = pInfo.SpecialType;
		
		float avoid = pInfo.Avoid + pMonster->GetBuffStatus()->Avoid;
		if( avoid > 0 )
			Avoid = avoid;
		float defense = pInfo.PhysicalDefense + pMonster->GetBuffStatus()->PhysicDefense;

		if( g_pServerSystem->GetMapNum() == Distorted_crevice)
		{

		}
		if( defense > 0 )
		{
			defense = random( ( defense * 0.8f ), ( defense * 1.1f ) );
			Defense = defense;
		}
	}
	else if( pTarget->GetObjectKind() == eObjectKind_Pet )
	{
		CPet* pPet = (CPet*)pTarget;
		
		PlayerStat& pStat = pPet->GetStat();

		Avoid = pStat.mEvade.mPlus;
		Defense = pStat.mPhysicDefense.mPlus;
		Block = 0;
		Shield = 0;

		//Defense *= PETMGR->GetFriendlyPenalty( pPet );
		if( pAttacker->GetObjectKind() == eObjectKind_Player )
		{
			CPlayer* pAttackPlayer = (CPlayer*)pAttacker;
			if( pAttackPlayer->IsPKMode())
				pAttackPlayer->SetPKStartTimeReset();
		}
	}
	else
	{
		ASSERT(0);
	}

	if( pTarget->GetObjectKind() == eObjectKind_Player && pAttacker->GetObjectKind() == eObjectKind_Player )
	{
		float ACC = 260 - ( ( Avoid - Accuracy ) * 13 );

		if( ACC < 260 )
		{
			if( ACC < ( ( rand() % 260 ) + 1 ) )
			{
				pDamageInfo->bDodge = TRUE;
				pDamageInfo->RealDamage = 0;

				return;
			}
		}
	}
	else
	{
		//float ACC = 750 - ( ( Avoid - Accuracy ) * 13 );
		float ACC = 750 - ( ( Avoid - ( Accuracy - 10 ) ) * 13 );

		if( ACC < 1000 )
		{
			if( ACC < ( ( rand() % 1000 ) + 1 ) )
			{
				pDamageInfo->bDodge = TRUE;
				pDamageInfo->RealDamage = 0;

				return;
			}
		}
	}

	float damage = 1;
	
	float cdReduction = (Defense / Attack * 100) * 1.7f;
	if(cdReduction > 100)
		cdReduction = 100;

	/*float pdefReduction = (((Defense*1.7f)+Shield) / Attack);
	if(pdefReduction > 1)
		pdefReduction = 1;*/

	if( Attack > ( Defense + Shield ) )
	{
		damage = Attack - ( Defense + Shield );
		//damage = Attack - (Attack * pdefReduction) + 1;
	}

  if( DLevel > ALevel )
	{
		damage = float(damage * ( 1 + ( ( ALevel - DLevel ) * 0.03 ) ));
	}

	if( 0 < Shield )
	{	
		const float random = float( rand() % 100 + 1);

		if( Block > random )
		{
			damage					= damage * ( 0.6f - ( vitality * 0.00025f ) ) - ( Shield * 2 );
			pDamageInfo->bBlock		= TRUE;
			CriticalRate			= 0;
		}
	}

	

	if(( pAttacker->GetObjectKind() == eObjectKind_Player && pTarget->GetObjectKind() == eObjectKind_Monster ))
	{
		if( AddType == 1 )
		{
			damage += AddDamage;
		}
		else if( AddType == 2 )
		{	
			damage *= ( ( 100 + AddDamage ) / 100.f );
		}
		else if( AddType == 3 )
		{
			damage *= ( ( AddDamage - 250 ) / 100.f );
		}
	}
	else { 
		if( AddType == 1 )
		{
			damage += AddDamage;
		}
		else if( AddType == 2 )
		{
			damage = ( damage ) * ( ( 1000.f + AddDamage + Strength ) / 1000.f );
			//damage += ( ( 100 + AddDamage ) / 100.f );
		}
		else if( AddType == 3 )
		{
			damage = ( damage ) * ( ( 1000.f + AddDamage + Strength ) / 1000.f );
			//damage += ( ( AddDamage - 250 ) / 100.f );
		}


		//damage *= 1.1f;
	}


	CriticalDamage = CriticalDamage - (CriticalDamage * cdReduction/100);
	float critMultiplier = 2 - (2 * (cdReduction*0.3)/100);
	//float critMultiplier = 2 ;

	if( CriticalRate && CriticalRate < 1500 )
	{
		if( CriticalRate >= ( ( rand() % 1500 ) + 1 ) )
		{
			damage *= ( critMultiplier + ( Dexterity * 0.0005f ));
			damage = damage + (damage * (CriticalDamage));
			//damage *= ( 2 + ( Dexterity * 0.0005f ) + ((CriticalDamage)*0.003f));
			//damage = float(( damage * ( 150 + ( Dexterity * 0.3 ) ) * ( 100 + CriticalDamage ) ) / 10000);
			pDamageInfo->bCritical = TRUE;
		}
	}
	else if( CriticalRate >= 1500 )
	{
		damage *= ( critMultiplier + ( Dexterity * 0.0005f ));
		
		damage = damage + (damage * (CriticalDamage));

		//damage *= ( 2 + ( Dexterity * 0.0005f ) + ((CriticalDamage)*0.003f));
		//damage = float(( damage * ( 150 + ( Dexterity * 0.3 ) ) * ( 100 + CriticalDamage ) ) / 10000);
		pDamageInfo->bCritical = TRUE;
	}

	switch( IsSpecial )
	{
		case 1: damage = 1; break;		
	}

	if( pTarget->GetObjectKind() == eObjectKind_Player && pAttacker->GetObjectKind() == eObjectKind_Player )
	{

		damage *= (this->GetPvpPhysicDamageRatio() / 100);

		if(g_pServerSystem->GetMapNum() == GTMAPNUM)

			damage *= 0.45f;
		else if(g_pServerSystem->GetMapNum() == DesolatedGorge)
			damage *= 0.45f;
	
		else if(g_pServerSystem->GetMapNum() == Zakandia_Outpost)
			damage *= 0.45f;

		else if(g_pServerSystem->GetMapNum() == Advanced_Dungeon)
			damage *= 0.45f;
		else if( SIEGEWARFAREMGR->Is_CastleMap() == FALSE &&
  		SIEGEWARFAREMGR->IsSiegeWarfareZone())
			damage *= 0.45f;
		else 		
			damage *= 0.8f;

	}

	if( AddType == 69 )
	{
		damage = 1;
	}

	if( damage < 1 )
	{
		damage = 1;
	}

	if( damage > 1 && ( pTarget->GetObjectKind() == eObjectKind_Player && pAttacker->GetObjectKind() == eObjectKind_Monster ) )
	{
		damage = damage + ( damage * 1.5f );
	}

	//if( pTarget->GetAbnormalStatus()->IsAbsorb )
	//{
	//	pTarget->RemoveBuffCount( eBuffSkillCountType_AnyDamageVolumn, int( damage ) );
	//	damage = 1;
	//}

	pDamageInfo->RealDamage = (DWORD)damage;
	


	{
		float count = Attack;

		switch( AddType )
		{
		case 1:
			{
				count += AddDamage;
				break;
			}
		case 2:
			{
				count *= ( 1.0f + AddDamage / 100.0f );
				break;
			}
		case 3:
			{
				count *= ( 1.0f + AddDamage / 100.0f );
				break;
			}
		}

		pAttacker->RemoveBuffCount( eBuffSkillCountType_PhysicalHitVolumn, int( count ) );
		pTarget->RemoveBuffCount( eBuffSkillCountType_PhysicalDamageVolumn, int( count ) );
	}
}

void CAttackManager::MagicAttack( CObject* pAttacker, CObject* pTarget, RESULTINFO* pDamageInfo, float Accuracy, float AddDamage, WORD AddType )
{
	DWORD skillIdx = 0;
	if(pDamageInfo->mSkillIndex)
	{
		skillIdx = pDamageInfo->mSkillIndex;
	}
	pDamageInfo->Clear();

	if( !pAttacker || !pTarget )
		return;

	if( pTarget->GetAbnormalStatus()->IsGod )
	{
		return;
	}
	

	

	

	float Attack = 1;
	float Defense = 1;
	WORD  AttackerLevel = pAttacker->GetLevel();
	WORD  TargetLevel = pTarget->GetLevel();
	float  FailValue = 1;
	float  CriticalValue = 1;
	WORD  IsSpecial = 0;
	float Int = 1;
	float CriticalRate = pAttacker->GetBuffStatus()->MagicCriticalRate;
	float CriticalDamage = pAttacker->GetBuffStatus()->MagicCriticalDamage;
	float wisdom = 0;
	float wisdomTarget = 0;
	float castingProtectValue	= pAttacker->GetBuffStatus()->CastingProtect;
	float castingProtectRate	= pAttacker->GetRateBuffStatus()->CastingProtect + 1.0f;

	if( AttackerLevel > TargetLevel )
	{
		CriticalValue = (float)(AttackerLevel - TargetLevel);
	}
	else if( TargetLevel > AttackerLevel )
	{
		FailValue = (float)(TargetLevel - AttackerLevel);
	}

	if( pAttacker->GetObjectKind() == eObjectKind_Player )
	{
		CPlayer* pPlayer = (CPlayer*)pAttacker;
		
		Attack = pPlayer->GetMagicAttack();
		Int = (float)pPlayer->GetIntelligence();
		wisdom = (float)pPlayer->GetWisdom();

		CriticalRate		= pPlayer->GetMagicCriticalRate();
		CriticalDamage		= pPlayer->GetMagicCriticalDamage();
		castingProtectValue	+= 	pPlayer->GetPassiveStatus()->CastingProtect;
		castingProtectRate	+= 	pPlayer->GetRatePassiveStatus()->CastingProtect;
	}
	else if( pAttacker->GetObjectKind() & eObjectKind_Monster )
	{
		CMonster* pMonster = (CMonster*)pAttacker;
		const BASE_MONSTER_LIST& pInfo = pMonster->GetSMonsterList();

		float attack = pInfo.AttackMagicalMax + pMonster->GetBuffStatus()->MagicAttack;
		if( attack > 0 )
		{
			Attack = attack;
		}
	}
	else if( pAttacker->GetObjectKind() == eObjectKind_Pet )
	{
		CPet* pPet = (CPet*)pAttacker;
		
		PlayerStat& pStat = pPet->GetStat();

		Attack = pStat.mMagicAttack.mPlus;
		Int = pStat.mIntelligence.mPlus;

		//Attack *= PETMGR->GetFriendlyPenalty( pPet );
		CriticalRate += pPet->GetPassiveStatus().MagicCriticalRate;
	}
	else
	{
		ASSERT(0);
	}

	if( pTarget->GetObjectKind() == eObjectKind_Player )
	{
		CPlayer* pPlayer = (CPlayer*)pTarget;
		wisdomTarget = (float)pPlayer->GetWisdom();
		Defense = pPlayer->GetMagicDefense();

		if( pAttacker->GetObjectKind() == eObjectKind_Player )
		{
			
			CPlayer* pAttackPlayer = (CPlayer*)pAttacker;
			if( pAttackPlayer->IsPKMode() )
				pAttackPlayer->SetPKStartTimeReset();

			if( pPlayer->IsPKMode())
				pPlayer->SetPKStartTimeReset();

		}
	}
	else if( pTarget->GetObjectKind() & eObjectKind_Monster )
	{
		CMonster* pMonster = (CMonster*)pTarget;
		const BASE_MONSTER_LIST& pInfo = pMonster->GetSMonsterList();
		
		IsSpecial = pInfo.SpecialType;

		float defense = pInfo.MagicalDefense + pMonster->GetBuffStatus()->MagicDefense;
		if( defense > 0 )
		{
			Defense = defense;
		}
	}
	else if( pTarget->GetObjectKind() == eObjectKind_Pet )
	{
		CPet* pPet = (CPet*)pTarget;
		
		PlayerStat& pStat = pPet->GetStat();

		Defense = pStat.mMagicDefense.mPlus;

		//Defense *= PETMGR->GetFriendlyPenalty( pPet );
		if( pAttacker->GetObjectKind() == eObjectKind_Player )
		{
			CPlayer* pAttackPlayer = (CPlayer*)pAttacker;
			if( pAttackPlayer->IsPKMode() )
				pAttackPlayer->SetPKStartTimeReset();
		}
	}
	else
	{
		ASSERT(0);
	}

	if( Accuracy < 100 )
	{
		if( Accuracy < ( ( rand() % 100 ) + 1 ) )
		{
			pDamageInfo->bDodge = TRUE;
			pDamageInfo->RealDamage = 0;

			return;
		}
	}

	float damage = 1;
	if( pTarget->GetObjectKind() == eObjectKind_Player && pAttacker->GetObjectKind() == eObjectKind_Player )
	{
		float damageTemp = Attack / 1.85f;

		float dmgReduction = (( Defense ) / damageTemp);
		if(dmgReduction > 1)
			dmgReduction = 1;

		if( (damageTemp) > ( Defense ) )
		{
			damage = damageTemp - (damageTemp * dmgReduction) + 1;
		}

		if( pTarget->GetLevel() > pAttacker->GetLevel() )
		{
			damage = float(damage * ( 1 + ( ( pAttacker->GetLevel() - pTarget->GetLevel() ) * 0.03 ) ));
		}

		if( AddType == 1 )
		{
			damage = AddDamage + Int;
			//damage *= ( ( 100 + AddDamage ) / 100.f );
		}
		else if( AddType == 2 )
		{
			//damage *= ( ( 100 + AddDamage ) / 100.f );
			damage = (damage) * ( ( 1000.f + AddDamage + Int) / 1000.f );
			//damage = ( (damage) ) + AddDamage + Int;
		}
	} else {
		float dem = ( Attack * 1.75f ) - ( Defense * 4.0f );
		float x = ( float )( 100 + pAttacker->GetLevel() - pTarget->GetLevel() );
		float y = 100 + ( ( dem / Defense ) * 25 );
		float z = float(1 - ( ( pTarget->GetLevel() - pAttacker->GetLevel() ) * 0.01 ));

		if( ( AddDamage * x * y ) > 10000 )
		{
			damage = ( AddDamage * x * y * z ) / 10000;
			damage = damage * 1.55f;
		}
	}

	


	{
		castingProtectValue 	*= castingProtectRate; 
		const float random = float( rand() % 100 + 1);
		const int failureRate	= int( max( 0, 10000.0f - ( 1000.0f * ( 1.0f - ( ( wisdom * 0.0005f ) + ( 0.002f * FailValue ) + ( castingProtectValue * 0.01f ) ) ) ) ) );
		const int random2		= rand() % 10000;

		if( CriticalRate > random || CriticalRate >= 100)
		{
			damage *= ( 1.8f + ( Int * 0.0005f ) );
			pDamageInfo->bCritical = TRUE;
		} 
		else if( random2 > failureRate )
		{
			damage *= 0.65f;
		}


		/*castingProtectValue 	*= castingProtectRate;
		const int successRate	= int( max( 0, ( Int * 2.5f ) + ( 20.0f * CriticalValue ) + ( CriticalRate * 5 ) ) );
		
		const int random		= rand() % 10000;
		
		if( random < successRate )
		{
			damage *= ( 1.8f + ( Int * 0.0005f ) );
			pDamageInfo->bCritical = TRUE;
		}
		else if( random > failureRate )
		{
			damage *= 0.65f;
		}*/
	}

	switch( IsSpecial )
	{ 
	case 1: damage = 1; break;		
	}

	if( pTarget->GetObjectKind() == eObjectKind_Player && pAttacker->GetObjectKind() == eObjectKind_Player )
	{

		/*{
			float pTargetCalc = ( ( 1 + wisdomTarget ) * 0.02f ) + ( Defense * 0.1f );
			float pAttackCalc = ( ( 1 + Int ) * 0.02f ) + ( Attack * 0.1f );
			float pFilteredFinal = 1 - ( ( pTargetCalc - pAttackCalc ) * 0.001f );
			damage *= pFilteredFinal;
		}*/

		damage *= (this->GetPvpMagicDamageRatio() / 100);

		if(g_pServerSystem->GetMapNum() == GTMAPNUM)
			//damage *= (this->GetGTDamageRatio() / 100);
			damage *= 0.45f;
	
		else if(g_pServerSystem->GetMapNum() == DesolatedGorge)
			damage *= 0.45f;
		else if(g_pServerSystem->GetMapNum() == Zakandia_Outpost)
			damage *= 0.45f;
	
		else if(g_pServerSystem->GetMapNum() == Advanced_Dungeon)
			damage *= 0.45f; 
	
		else if( SIEGEWARFAREMGR->Is_CastleMap() == FALSE &&
  		SIEGEWARFAREMGR->IsSiegeWarfareZone())
			
			damage *= 0.45f;
		else 		
			damage *= 0.8f;
		
	}

	if( AddType == 69 )
	{
		damage = 1;
	}

	if( damage < 1 )
	{
		damage = 1;
	}
	
	if( damage > 1 && ( pTarget->GetObjectKind() == eObjectKind_Player && pAttacker->GetObjectKind() == eObjectKind_Monster ) )
	{
		damage *= 3;
	}


	
	pDamageInfo->RealDamage = (DWORD)damage;




	{
		float count = Attack;

		switch( AddType )
		{
		case 1:
			{
				count += AddDamage;
				break;
			}
		case 2:
			{
				count *= ( 1.0f + AddDamage / 100.0f );
				break;
			}
		}

		pAttacker->RemoveBuffCount( eBuffSkillCountType_MagicalHitVolumn, int( count ) );
		pTarget->RemoveBuffCount( eBuffSkillCountType_MagicalDamageVolumn, int( count ) );
	}
}

void CAttackManager::SpecialAttack( CObject* pAttacker, CObject* pTarget, RESULTINFO* pDamageInfo, float Damage, WORD Type, BOOL bFixed )
{
	pDamageInfo->Clear();

	if( !pAttacker || !pTarget || pTarget->GetObjectKind() == eObjectKind_Pet )
		return;



	if( pTarget->GetAbnormalStatus()->IsGod )
	{
		return;
	}

	float damage = 0.f;

	BOOL IsSpecial = FALSE;

	if( pTarget->GetObjectKind() & eObjectKind_Monster )
	{
		CMonster* pMonster = (CMonster*)pTarget;
		const BASE_MONSTER_LIST& pInfo = pMonster->GetSMonsterList();

		IsSpecial = pInfo.SpecialType;
	}

	if( Damage < 0 )
	{
		float fAttackDexterity = 0.f;
		float fAttackIntelligense = 0.f;
		float fTargetVitality = 0.f;
		float fTargetWisdom = 0.f;

		switch( pAttacker->GetObjectKind() )
		{
		case eObjectKind_Player:
			{
				fAttackDexterity = (float)((CPlayer*)pAttacker)->GetDexterity();
				fAttackIntelligense = (float)((CPlayer*)pAttacker)->GetIntelligence();
			}
			break;
		case eObjectKind_Pet:
			{
				PlayerStat& pStat = ((CPet*)pTarget)->GetStat();
				fAttackDexterity = (float)(pStat.mDexterity.mPlus);
				fAttackIntelligense = (float)(pStat.mIntelligence.mPlus);
			}
			break;
		case eObjectKind_Monster:
			{
				CPlayer* const playerowner = (CPlayer*)g_pUserTable->FindUser(pAttacker->GetOwnerIndex());
				if (playerowner && playerowner->GetObjectKind() == eObjectKind_Player)
				{
					fAttackDexterity = (float)playerowner->GetDexterity();
					fAttackIntelligense = (float)playerowner->GetIntelligence();
				}
			}
			break;
		}

		switch( pTarget->GetObjectKind() )
		{
		case eObjectKind_Player:
			{
				fTargetWisdom = (float)((CPlayer*)pTarget)->GetWisdom();
				fTargetVitality = (float)((CPlayer*)pTarget)->GetVitality();
			}
			break;
		case eObjectKind_Pet:
			{
				PlayerStat& pStat = ((CPet*)pTarget)->GetStat();
				fTargetVitality = (float)(pStat.mVitality.mPlus);
				fTargetWisdom = (float)(pStat.mWisdom.mPlus);
			}
			break;
		case eObjectKind_Monster:
			{
				CObject* const playerowneridx = ((CMonster*)pTarget)->GetTObject();
				if (playerowneridx)
				{
					CPlayer* const playerowner = (CPlayer*)g_pUserTable->FindUser(playerowneridx->GetID());
					if (playerowner && playerowner->GetObjectKind() == eObjectKind_Player)
					{
						if (pAttacker != playerowner)
						{
							fTargetVitality = (float)playerowner->GetVitality();
							fTargetWisdom = (float)playerowner->GetWisdom();
						}
					}
				}
			}
			break;
		}

		float baseDamage = -Damage;
		damage = -Damage;

	
		
		if(!bFixed) damage = (-Damage * (1 + (((fAttackDexterity * 10.0f) + (fAttackIntelligense * 15.0f) - (fTargetVitality*2.5f) - (fTargetWisdom*5.0f))) / 3400.f));
		
		
		if(!bFixed && damage < 0)
		{
			damage = -Damage;
		}
		switch( IsSpecial )
		{
		case 1: damage = 1; break; 	 	
		}

		if( pTarget->GetObjectKind() == eObjectKind_Player && pAttacker->GetObjectKind() == eObjectKind_Player )
		{
			if(!bFixed)
			{
			
	/*			if(g_pServerSystem->GetMapNum() == GTMAPNUM)
					damage *= 0.45f;
				else if(g_pServerSystem->GetMapNum() == Arcane_Siege_War)
					damage *= 0.60f;
				else if(g_pServerSystem->GetMapNum() == DesolatedGorge)
					damage *= 0.60f;
				else if(g_pServerSystem->GetMapNum() == Zakandia_Outpost)
					damage *= 0.60f;
				else if( SIEGEWARFAREMGR->Is_CastleMap() == FALSE &&
  				SIEGEWARFAREMGR->IsSiegeWarfareZone() &&
  				SIEGEWARFAREMGR->GetState() > eSiegeWarfare_State_Reset )
					damage *= 0.45f;
				else 		*/
					damage *= 0.8f;
			}

			CPlayer* pAttackPlayer = (CPlayer*)pAttacker;
			if( pAttackPlayer->IsPKMode() )
				pAttackPlayer->SetPKStartTimeReset();

			CPlayer* pTargetPlayer = (CPlayer*)pTarget;
			if( pTargetPlayer->IsPKMode()  )
				pTargetPlayer->SetPKStartTimeReset();
		}

		//if( pTarget->GetAbnormalStatus()->IsAbsorb )
		//{
		//	pTarget->RemoveBuffCount( eBuffSkillCountType_AnyDamageVolumn, int( damage ) );
		//	damage = 1;
		//}

		pDamageInfo->RealDamage = (DWORD)damage;

		if( pTarget->HasEventSkill( eStatusKind_DamageToManaDamage ) )
		{
			pTarget->Execute( CTakeDamageEvent( pAttacker, *pDamageInfo, eStatusKind_DamageToManaDamage ) );

			pTarget->ManaDamage( pAttacker, pDamageInfo );
		}

		DWORD newLife = pTarget->Damage(pAttacker,pDamageInfo);

		if(newLife == 0)
		{
			pTarget->Execute( CDieEvent( pAttacker, pTarget ) );
			pAttacker->Execute( CKillEvent( pAttacker, pTarget ) );

			ATTACKMGR->sendDieMsg(pAttacker,pTarget);
			pTarget->Die(pAttacker);
		}
	}
	else 
	{
		RecoverLife( pAttacker, pTarget, Damage, pDamageInfo );
	}
}


// 090204 LUJ, ??? MP? ????
void CAttackManager::BurnMana( CObject* offenseObject, CObject* defenseObject, const BUFF_SKILL_INFO* buffSkillInfo )
{
	if( ! offenseObject || ! defenseObject || ! buffSkillInfo )
	{
		return;
	}
	else if( defenseObject->GetAbnormalStatus()->IsGod )
	{
		return;
	}

	SKILL_INFO::Value damage = { 0 };

	// 090204 LUJ, ??? ?? ?? ??? ???
	switch( buffSkillInfo->StatusDataType )
	{
	case BUFF_SKILL_INFO::StatusTypeAdd:
		{
			damage.mPlus += buffSkillInfo->StatusData;
			break;
		}
	case BUFF_SKILL_INFO::StatusTypePercent:
		{
			damage.mPlus += defenseObject->GetMaxMana() * buffSkillInfo->StatusData / 100.0f;
			break;
		}
	}

	// 090204 LUJ, MP ??? ????
	{
		const float manaDamage	= damage.mPlus * ( 1.0f + damage.mPercent );
		const float currentMana = float( defenseObject->GetMana() ) + manaDamage;

		defenseObject->SetMana( DWORD( max( 0, currentMana ) ) );
	}
}