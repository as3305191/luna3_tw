#include "stdafx.h"
#include "Player.h"
#include "GameResourceManager.h"
#include "MoveManager.h "
#include "./interface/cWindowManager.h"
#include "AppearanceManager.h"
#include "PartyManager.h"
#include "ItemManager.h"
#include "ObjectManager.h"
//#include "KyungGongManager.h"
#include "StreetStallTitleTip.h"
#include "StreetStallManager.h"
#include "GameIn.h"
#include "ObjectBalloon.h"
//#include "MonsterGuageDlg.h"

#include "MainGame.h"
#include "WindowIdEnum.h"
#include "MapChange.h"
#include "cMsgBox.h"

#include "ObjectStateManager.h"
#include "PKManager.h"
#include "ChatManager.h"
#include "GuildFieldWar.h"

#include "OptionManager.h"
#include "cImageSelf.h"
#include "CharacterDialog.h"
#include "MonsterGuageDlg.h"
#include "cResourceManager.h"
#include "../hseos/Common/SHMath.h"
#include "facialmanager.h"
#include "../hseos/Family/SHFamilyManager.h"
#include "../hseos/Farm/SHFarmManager.h"
#include "../hseos/Date/SHDateManager.h"
#include "MHMap.h"
#include "cHousingMgr.h"
#include "MHCamera.h"
#include "VehicleManager.h"
#include "MonsterTargetDlg.h"

//#include "QuestGroup.h"
CPlayer::CPlayer()
{
	m_pFaceShapeObject		= NULL;
//	m_pQuestGroup = NULL;
	m_ItemUseEffect = -1;

	// 070228 LYW --- Player : Setting boolen value to false.
	m_bStartCheck = FALSE ;

	// 070312 LYW --- Player : Setting boolen value to true of eye part.
	m_bActiveEye  = FALSE ;

	m_bClosed	  = FALSE ;

	// 070503 LYW --- Player : Check emotion kind.
	m_bRunningEmotion = FALSE ;

	// desc_hseos_�йи�01
	// S �йи� �߰� added by hseos 2007.07.17
	m_pcsFamilyEmblem = NULL;
	// E �йи� �߰� added by hseos 2007.07.17

	m_pWingObject = NULL;

	mWeaponEffect[0] = 0;
	mWeaponEffect[1] = 0;
	mWeaponEffect[2] = 0;

	mJobEffect = 0;

	//090522 pdy �߰� 
	m_dwRideFurnitureMotionIndex		 = 0;
	m_dwRideFurnitureID					 = 0;
	m_dwRideFurnitureSeatPos			 = 0;

	//090603 pdy �Ͽ�¡ ž�¹��� ���� m_bRideFurniture �߰� 
	m_bRideFurniture = FALSE;

	bWingsMove = TRUE;
}

CPlayer::~CPlayer()
{
//	if( m_pQuestGroup )
//		delete m_pQuestGroup;

	// desc_hseos_�йи�01
	// S �йи� �߰� added by hseos 2007.07.17
	if (m_pcsFamilyEmblem)
	{
		m_pcsFamilyEmblem->Release();
	}
	SAFE_DELETE(m_pcsFamilyEmblem);
	// E �йи� �߰� added by hseos 2007.07.17
}

// desc_hseos_�йи�01
// S �йи� �߰� added by hseos 2007.08.06
void CPlayer::Release()
{
	// 090427 ShinJS --- ž�������� Player�� ���ڷ� �޾Ƽ� ó��(Release�� ������ ��� �ڱ��ڽ��� ObjectManager�� ObjectTable�� �̹� ���ŵ� ���¿��� ������ ����)
	CVehicle* const pVehicle = (CVehicle*)OBJECTMGR->GetObject( m_dwVehicleID );

	if( pVehicle && pVehicle->GetObjectKind() == eObjectKind_Vehicle )
	{
		pVehicle->GetOff(
			this,
			GetVehicleSeatPos());
	}

	// 090210 LYW --- Player : Player�� Ȱ��ȭ ���� ����/���� ����Ʈ�� �����Ѵ�.
	// ����Ʈ �Ŵ����� ����Ʈ ���̺��� �� Player�� ����Ʈ ������ ���� Ȱ��ȭ ���̱� ������, ������ ���� ��Ų��.
	RemoveEffects() ;

	if (m_pcsFamilyEmblem)
	{
		m_pcsFamilyEmblem->Release();
	}
	SAFE_DELETE(m_pcsFamilyEmblem);

	CObject::Release();
}
// E �йи� �߰� added by hseos 2007.08.06

void CPlayer::InitPlayer(CHARACTER_TOTALINFO* pTotalInfo)
{
	m_CharacterInfo = *pTotalInfo;
	ZeroMemory(&m_MoveInfo, sizeof(m_MoveInfo));

	// 090908 Iros --- Change Part Check.
	ClearChangeParts();

	// 090422 ShinJS --- Ż�� ID, Ż�� ��ȯ/������ �ð� �ʱ�ȭ
	m_dwVehicleID = 0;
	m_dwVehicleSeatPos = 0;
	m_dwLastVehicleSummonTime = 0;
	m_dwLastVehicleGetOffTime = 0;
	
	SetBaseMotion();

	m_pFaceShapeObject = 0;
	ZeroMemory(&m_stFaceAction, sizeof(m_stFaceAction));

	m_stFaceAction.nFaceShapeTimeTick = gCurTime + 3000;
	m_pcsFamilyEmblem = 0;
	m_nFarmMotionTimeTick = 0;
	m_nFarmMotionTime = 0;

	m_IsDrawHeart = FALSE;

	m_pWingObject = NULL;
	ZeroMemory(
		mWeaponEffect,
		sizeof(mWeaponEffect));
	m_dwImmortalTimeOnGTMAP = 0;


	//090603 pdy �Ͽ�¡ ž�¹��� ���� (ž�°��� ���� �ʱ�ȭ)
	m_dwRideFurnitureMotionIndex = 0;
	m_dwRideFurnitureID = 0;										
	m_dwRideFurnitureSeatPos = 0;	
	m_bRideFurniture = FALSE;
}

void CPlayer::GetCharacterTotalInfo(CHARACTER_TOTALINFO* pRtInfo)
{
	memcpy(pRtInfo,&m_CharacterInfo,sizeof(CHARACTER_TOTALINFO));
}


void CPlayer::Process()
{
	// 070503 LYW --- Player : Motice to conduct manager.
	if( GetEngineObject()->IsEndMotion() )
	{
		if( m_bRunningEmotion )
		{
			// desc_hseos_����ý���_01
			// S ����ý��� �߰� added by hseos 2007.09.18
			// APPEARANCEMGR->ShowWeapon( this ) ;
			if (g_csFarmManager.CLI_GetIngCharMotion() == FALSE)
			{
				APPEARANCEMGR->ShowWeapon( this ) ;
			}
			// E ����ý��� �߰� added by hseos 2007.09.18
			m_bRunningEmotion = FALSE ;
		}
	}

	if( GetState() == eObjectState_Die )
	{
		CMonsterGuageDlg* dialog = GAMEIN->GetMonsterGuageDlg();

		if( dialog		&& dialog->GetCurTarget() &&
			this == dialog->GetCurTarget()->GetCurObejct() )
		{
			dialog->SetActive( FALSE );
		}

	}

	// 070228 LYW --- Player : Add check facial time part.
	if( m_bStartCheck )
	{
		if( m_FacialTime.Check() == 0 )
		{
			ReturnFace() ;
		}
	}

	if ( !OPTIONMGR->GetGameOption()->nLODMode ||
		 (OPTIONMGR->GetGameOption()->nLODMode && GetID() == HEROID) )
		ProcessFaceAction();

	ProcFarmMotion();

	CObject::Process();
}

void CPlayer::SetWearedItemIdx(DWORD WearedPosition,DWORD ItemIdx)
{
	ASSERT( sizeof( m_CharacterInfo.WearedItemIdx ) / sizeof( DWORD ) > WearedPosition );

	if (m_CharacterInfo.WearedItemIdx[WearedPosition] == ItemIdx)
		return;

	m_ChangedPartIdx[WearedPosition] = m_CharacterInfo.WearedItemIdx[WearedPosition];
	m_CharacterInfo.WearedItemIdx[WearedPosition] = ItemIdx;

	SetBaseMotion();			// ���Ⱑ �ٲ�� �⺻������ �ٲ��
}

eWeaponType CPlayer::GetWeaponEquipType()
{
	if(const ITEM_INFO* const pItemInfo = ITEMMGR->GetItemInfo(GetWearedItemIdx(eWearedItem_Weapon)))
	{
		return pItemInfo->WeaponType;
	}

	return eWeaponType_None;
}

// 080703 LUJ, ����� ǥ���� �� �ִ� �ִϸ��̼� Ÿ���� ��ȯ�Ѵ�
eWeaponAnimationType CPlayer::GetWeaponAniType()
{
	const ITEM_INFO* leftInfo	= ITEMMGR->GetItemInfo( GetWearedItemIdx( eWearedItem_Weapon ) );
	const ITEM_INFO* rightInfo	= ITEMMGR->GetItemInfo( GetWearedItemIdx( eWearedItem_Shield ) );

	const eWeaponAnimationType	leftType	= eWeaponAnimationType( leftInfo ? leftInfo->WeaponAnimation : eWeaponAnimationType_None );
	const eWeaponAnimationType	rightType	= eWeaponAnimationType( rightInfo ? rightInfo->WeaponAnimation : eWeaponAnimationType_None );

	// 080703 LUJ, ����� ���Ⱑ �ٸ��� �̵����� �ƴϴ�. �޼տ� ���⸦ �� ����� ��쵵 ���������̴�.
	if( leftType != rightType ||
		leftType == eWeaponAnimationType_None )
	{
		return leftType;
	}

	return eWeaponAnimationType_TwoBlade;
}

void CPlayer::SetBaseMotion()
{
	// 090422 ShinJS --- Ż�Ϳ� ž�����ΰ�� Ż�� Script���� ��������� �о�´�
	if( IsGetOnVehicle() )
	{
		CVehicle* pVehicle = (CVehicle*)OBJECTMGR->GetObject( GetVehicleID() );
		if( !pVehicle || pVehicle->GetObjectKind() != eObjectKind_Vehicle )
			return;

		const WORD wMonsterKind = pVehicle->GetMonsterKind();
		const VehicleScript& script = GAMERESRCMNGR->GetVehicleScript( wMonsterKind );
		if( script.mMonsterKind != wMonsterKind )		return;

		m_StandardMotion = script.mMotion.mMount;

		m_Move_Start_Motion = script.mMotion.mMount;
		m_Move_Ing_Motion = script.mMotion.mMount;
		m_Move_End_Motion = -1;
	}
	else if( eObjectState_HouseRiding == GetState() )
	{
		CFurniture* pFurniture = (CFurniture*)OBJECTMGR->GetObject( GetRideFurnitureID() );
		if( !pFurniture || pFurniture->GetObjectKind() != eObjectKind_Furniture )
			return;

		m_StandardMotion = m_dwRideFurnitureMotionIndex;
		m_Move_Start_Motion = m_dwRideFurnitureMotionIndex;
		m_Move_Ing_Motion = m_dwRideFurnitureMotionIndex;
		m_Move_End_Motion = -1;
	}
	else
	{
		const eWeaponAnimationType WeaponEquipType = GetWeaponAniType();

		if( GetObjectBattleState() )
		{
			m_StandardMotion = CHARACTER_MOTION[ eCharacterMotion_Battle ][ WeaponEquipType ];
		}
		else
		{
			m_StandardMotion = CHARACTER_MOTION[ eCharacterMotion_Standard ][ WeaponEquipType ];
		}

		if(m_MoveInfo.MoveMode == eMoveMode_Run)
		{
			// �ٱ�	
			m_Move_Start_Motion = CHARACTER_MOTION[ eCharacterMotion_Run ][ WeaponEquipType ];
			m_Move_Ing_Motion = CHARACTER_MOTION[ eCharacterMotion_Run ][ WeaponEquipType ];
			m_Move_End_Motion = -1;

			DWORD itemidx = GetWearedItemIdx(eWearedItem_Costume_Wing) != NULL && bWingsMove ? GetWearedItemIdx(eWearedItem_Costume_Wing) : GetWearedItemIdx(eWearedItem_Wing);
			if( itemidx )
			{
				ITEM_INFO* pInfo = ITEMMGR->GetItemInfo( itemidx );
				if(pInfo)
				{
					switch( pInfo->Part3DType )
					{
					case ePartType_Wing:
						{
							m_Move_Start_Motion = 199;
							m_Move_Ing_Motion = 199;
						}
						break;
					}
				}
			}
		}
		else
		{	// �ȱ�
			m_Move_Start_Motion = CHARACTER_MOTION[ eCharacterMotion_Walk ][ WeaponEquipType ];
			m_Move_Ing_Motion = CHARACTER_MOTION[ eCharacterMotion_Walk ][ WeaponEquipType ];
			m_Move_End_Motion = -1;

			DWORD itemidx = GetWearedItemIdx(eWearedItem_Costume_Wing) != NULL && bWingsMove ? GetWearedItemIdx(eWearedItem_Costume_Wing) : GetWearedItemIdx(eWearedItem_Wing);
			if( itemidx )
			{
				ITEM_INFO* pInfo = ITEMMGR->GetItemInfo( itemidx );
				if(pInfo)
				{
					switch( pInfo->Part3DType )
					{
					case ePartType_Wing:
						{
							m_Move_Start_Motion = 199;
							m_Move_Ing_Motion = 199;
						}
						break;
					}
				}
			}
		}
	}
}

BOOL CPlayer::StartSocietyAct( int nStartMotion, int nIngMotion, int nEndMotion, BOOL bHideWeapon )
{
	//��� ���°� �ƴϸ� �ȵȴ�.
	if( GetState() != eObjectState_None && GetState() != eObjectState_Society
		&& GetState() != eObjectState_Immortal ) return FALSE;

	OBJECTSTATEMGR->EndObjectState( this, GetState(), 0 );	//���� ���¸� ������.

	m_ObjectState.State_Start_Motion	= (short int)nStartMotion;
	m_ObjectState.State_Ing_Motion		= (short int)nIngMotion;
	m_ObjectState.State_End_Motion		= (short int)nEndMotion;
	m_ObjectState.MotionSpeedRate = 1.0f;

	if( nEndMotion != -1 )
		m_ObjectState.State_End_MotionTime	= GetEngineObject()->GetAnimationTime(nEndMotion);
	else
		m_ObjectState.State_End_MotionTime = 0;


	if(m_ObjectState.State_Start_Motion != -1)
	{
		ChangeMotion( m_ObjectState.State_Start_Motion, FALSE );

		if( m_ObjectState.State_Ing_Motion != -1 )
			ChangeBaseMotion( m_ObjectState.State_Ing_Motion );
	}

	if(m_ObjectState.State_End_MotionTime)
	m_BaseObjectInfo.ObjectState	= eObjectState_Society;
	m_ObjectState.State_Start_Time	= gCurTime;
	m_ObjectState.bEndState			= FALSE;

	if( bHideWeapon )
		APPEARANCEMGR->HideWeapon( this );

	m_BaseObjectInfo.ObjectState	= eObjectState_Society;
	m_ObjectState.State_Start_Time	= gCurTime;
	m_ObjectState.bEndState			= FALSE;
	
	if( m_ObjectState.State_Ing_Motion == -1 )
		OBJECTSTATEMGR->EndObjectState( this, eObjectState_Society, GetEngineObject()->GetAnimationTime( m_ObjectState.State_Start_Motion ) );

	return TRUE;
}

void CPlayer::SetWingObjectMotion(eWingMotion idx)
{
	if( m_pWingObject )
	{
		m_pWingObject->ChangeMotion( idx, 0 );
	}
}

void CPlayer::SetWingObjectBaseMotion(eWingMotion idx)
{
	if(m_pWingObject)
	{
		m_pWingObject->ChangeBaseMotion(idx);
	}
}

void CPlayer::SetMotionInState(EObjectState State)
{
	const eWeaponAnimationType  WeaponEquipType = GetWeaponAniType();

	switch(State)
	{
	case eObjectState_StreetStall_Owner:
		{
			m_ObjectState.State_Start_Motion	= CHARACTER_MOTION[ eCharacterMotion_RestStart ][ WeaponEquipType ];
			m_ObjectState.State_Ing_Motion		= CHARACTER_MOTION[ eCharacterMotion_Rest ][ WeaponEquipType ];
			m_ObjectState.State_End_Motion		= CHARACTER_MOTION[ eCharacterMotion_RestEnd ][ WeaponEquipType ];
			m_ObjectState.State_End_MotionTime	= GetEngineObject()->GetAnimationTime(eMotion_StreetStall_End);
			m_ObjectState.MotionSpeedRate = 1.0f;

			SetWingObjectMotion( eWingMotion_Idle );
			SetWingObjectBaseMotion( eWingMotion_Idle );
		}
		break;
	case eObjectState_Move:
		{
			m_ObjectState.State_Start_Motion = m_Move_Start_Motion;
			m_ObjectState.State_Ing_Motion = m_Move_Ing_Motion;
			m_ObjectState.State_End_Motion = m_Move_End_Motion;
			m_ObjectState.State_End_MotionTime = GetEngineObject()->GetAnimationTime(m_Move_End_Motion);
			m_ObjectState.MotionSpeedRate = 1.0f;

			if(m_MoveInfo.MoveMode == eMoveMode_Run)
			{
				SetWingObjectMotion( eWingMotion_Run );
				SetWingObjectBaseMotion( eWingMotion_Run );
			}
			else
			{
				SetWingObjectMotion( eWingMotion_Walk );
				SetWingObjectBaseMotion( eWingMotion_Walk );
			}
		}
		break;
	case eObjectState_Immortal:
	case eObjectState_None:
	case eObjectState_Exchange:
	case eObjectState_Deal:
	//case eObjectState_Munpa:
	case eObjectState_PrivateWarehouse:
	case eObjectState_StreetStall_Guest:
	case eObjectState_HouseRiding:								//090526 pdy �Ͽ�¡ Ÿ�� �׼� �߰�
		{
			m_ObjectState.State_Start_Motion = m_StandardMotion;
			
			//YH2DO
//			if( GetState() == eObjectState_SkillSyn ||
//				GetState() == eObjectState_SkillUsing )
//				m_ObjectState.State_Start_Motion = -1;

			if( GetState() == eObjectState_Rest )
			{
				m_ObjectState.State_Start_Motion	= CHARACTER_MOTION[ eCharacterMotion_RestEnd ][ WeaponEquipType ];
			}

			m_ObjectState.State_Ing_Motion = m_StandardMotion;
			m_ObjectState.State_End_Motion = -1;
			m_ObjectState.State_End_MotionTime = 0;
			m_ObjectState.MotionSpeedRate = 1.0f;

			SetWingObjectMotion( eWingMotion_Idle );
			SetWingObjectBaseMotion( eWingMotion_Idle );
		}
		break;
	case eObjectState_Engrave:
		{
			m_ObjectState.State_Start_Motion = 327;
			m_ObjectState.State_Ing_Motion = 328;
			m_ObjectState.State_End_Motion = -1;
			m_ObjectState.State_End_MotionTime = GetEngineObject()->GetAnimationTime(eMotion_Ungijosik_End);
			m_ObjectState.MotionSpeedRate = 1.0f;

			SetWingObjectMotion( eWingMotion_Idle );
			SetWingObjectBaseMotion( eWingMotion_Idle );
		}
		break;

	case eObjectState_Rest :
		{
			m_ObjectState.State_Start_Motion	= CHARACTER_MOTION[ eCharacterMotion_RestStart ][ WeaponEquipType ];
			if( GetState() == eObjectState_Rest )
			{
				m_ObjectState.State_Start_Motion = CHARACTER_MOTION[ eCharacterMotion_Rest ][ WeaponEquipType ];;
			}
			m_ObjectState.State_Ing_Motion		= CHARACTER_MOTION[ eCharacterMotion_Rest ][ WeaponEquipType ];
			m_ObjectState.State_End_Motion		= -1 ;
			m_ObjectState.State_End_MotionTime	= 0 ;
			m_ObjectState.MotionSpeedRate = 1.0f;

			SetWingObjectMotion( eWingMotion_Idle );
			SetWingObjectBaseMotion( eWingMotion_Idle );
		}
		break ;

	default:
		{
			m_ObjectState.State_Start_Motion = -1;
			m_ObjectState.State_Ing_Motion = -1;
			m_ObjectState.State_End_Motion = -1;
			m_ObjectState.State_End_MotionTime = 0;
			m_ObjectState.MotionSpeedRate = 1.0f;
		}
		break;
	}
}

BYTE CPlayer::GetLifePercent()
{
	return (BYTE)((m_CharacterInfo.Life*100) / GetMaxLife());
}

void CPlayer::OnStartObjectState(EObjectState State)
{
	switch(State)
	{
	case eObjectState_Move:
		{

		}
		break;
	case eObjectState_Connect:
		{
			CHero* hero = OBJECTMGR->GetHero();

			if( hero && 
				CheckHeart( hero ) )
			{
				//hero->AddMatch( this );
				hero->AddMatch();

				SetHeart( TRUE );
			}
			// desc_hseos_�йи�01
			// S �йи� �߰� added by hseos 2007.11.21
			// ..���� ǥ��
			if (GetFamilyIdx())
			{
				g_csFamilyManager.CLI_ShowEmblem(this, GetFamilyIdx(), GetCharacterTotalInfo()->nFamilyEmblemChangedFreq);
			}
			// E �йи� �߰� added by hseos 2007.11.21
		}
		break;

	case eObjectState_Exit:
		{
			if( m_IsDrawHeart )
			{
				CHero* hero = OBJECTMGR->GetHero();

				//hero->RemoveMatch( this );
				hero->RemoveMatch();
			}

			// desc_hseos_�йи�01
			// S �йи� �߰� added by hseos 2007.07.26
			// ..���� ����
			if (m_pcsFamilyEmblem)
			{
				m_pcsFamilyEmblem->Release();
				SAFE_DELETE(m_pcsFamilyEmblem);
			}
			// E �йи� �߰� added by hseos 2007.07.26

			//090618 pdy �Ͽ�¡ 
			if( IsRideFurniture() )
				HOUSINGMGR->RideOffPlayerFromFuniture( this );
		}
		break;
	case eObjectState_StreetStall_Owner:
		{
			APPEARANCEMGR->HideWeapon(this);
		}
		break;
	case eObjectState_ItemUse:
		{
			if( m_ItemUseEffect == -1 )
				return;

			EFFECTMGR->StartPlayerEffectProcess( HERO, m_ItemUseEffect );
			m_ItemUseEffect = -1;
		}
		break;
	case eObjectState_Housing:
		{
			if(  gHeroID == GetID() )
			{
				DWORD dwFilterIdx = HOUSINGMGR->GetCameraFilterIdxOnDecoMode() ;
				if( dwFilterIdx == 0 )
					return;

				CAMERA->AttachCameraFilter( dwFilterIdx ) ;
			}
		}
		break;
	}
}

void CPlayer::OnEndObjectState(EObjectState State)
{
	switch(State)
	{
	case eObjectState_Die:
		{
			if(GetID() != HEROID)
				GetEngineObject()->EnablePick();
		}
		break;
	case eObjectState_Housing:
		{
			//RemoveObjectEffect( HOUSE_DECOMODE_EFFECT );
			DWORD dwFilterIdx = HOUSINGMGR->GetCameraFilterIdxOnDecoMode() ;
			if( dwFilterIdx == 0 )
				return;

			CAMERA->DetachCameraFilter( dwFilterIdx );
		}
		break;
	case eObjectState_ItemUse:
		{
			if( m_ItemUseMotion==eMotion_Item_ChangeHair_1 || m_ItemUseMotion==eMotion_Item_ChangeHair_2 || m_ItemUseMotion==eMotion_Item_ChangeHair_3 )
			{
				APPEARANCEMGR->InitAppearance( this );
			}

			//
			if(GetID() != HEROID )
				goto MotionReset;
			if( m_ItemUseMotion==eMotion_Item_Teleport_1 || m_ItemUseMotion==eMotion_Item_Teleport_2 || m_ItemUseMotion==eMotion_Item_Teleport_3 )
			{
				DWORD map = GAMEIN->GetMoveMap();
				// 070122 LYW --- Modified this line.
				//WINDOWMGR->MsgBox( MBI_NOBTNMSGBOX, MBT_NOBTN, CHATMGR->GetChatMsg( 160 ) );
				WINDOWMGR->MsgBox( MBI_NOBTNMSGBOX, MBT_NOBTN, RESRCMGR->GetMsg( 290 ) );
				MAPCHANGE->SetGameInInitKind(eGameInInitKind_MovePos);				
				MAINGAME->SetGameState(eGAMESTATE_MAPCHANGE,&map,4);
			}

MotionReset:
			m_ItemUseMotion = -1;
			m_ItemUseEffect = -1;
		}
		break;
	}
	
	RemoveObjectEffect(STATE_EFFECT_ID);	//????
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// �ൿ �Լ���..
void CPlayer::Die(CObject* pAttacker,BOOL bFatalDamage,BOOL bCritical, BOOL bDecisive) 
{	
	if(pAttacker)
	{
		MOVEMGR->SetLookatPos(this,&pAttacker->GetCurPosition(),0,gCurTime);

		if( HERO )
		if( pAttacker == HERO && HERO->IsPKMode() && !this->IsPKMode() )
//		if( GUILDFIELDWAR->GetEnemyGuildIdx() != 0 )
		{
			PKMGR->SetPKContinueTime( PKMGR->GetPKContinueTime() + 10*60*1000 );
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1239 ), 10 );	//---KES PK 071124
			//msg �������� ��� ���ӽð��� �߰��� 60�ʸ�ŭ �þ���ϴ�.
		}
	}

	//---KES PK 071124
	if( this == HERO && HERO->IsPKMode() )	//������ PK�ð� ����
	{
		//---������ ���� PK��� ���ӽð� ����
		DWORD dwPKContinueTime = PKMGR->GetPKContinueTime();
		if( dwPKContinueTime == 0 ) return;

	    if( dwPKContinueTime > 10*60*1000 )
			dwPKContinueTime -= 10*60*1000;
		else
			dwPKContinueTime = 0;

		PKMGR->SetPKContinueTime( dwPKContinueTime );
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1240 ), 10 );	//---KES PK 071124
		//-----------------------------------
	}


	const eWeaponAnimationType  weapon = GetWeaponAniType();
	WORD DieMotion = CHARACTER_MOTION[ eCharacterMotion_Die1 ][ weapon ];
	WORD DiedMotion = CHARACTER_MOTION[ eCharacterMotion_Died1 ][ weapon ];


	int DieEffect;
	if(bFatalDamage)
	{
		DieEffect = eEffect_Die2;	// ���󰡼� �ױ�
	
		SetWingObjectBaseMotion( eWingMotion_Died1 );
		SetWingObjectMotion( eWingMotion_Die1 );
	
	}
	else
	{
		DieEffect = eEffect_Die1;	// �׳� �ױ�

		SetWingObjectBaseMotion( eWingMotion_Died2 );
		SetWingObjectMotion( eWingMotion_Die2 );
	}

	TARGETSET set;
	set.pTarget = this;
	set.ResultInfo.bCritical = bCritical;
	set.ResultInfo.bDecisive = bDecisive;
	
	EFFECTMGR->StartEffectProcess(DieEffect,pAttacker,&set,1,GetID(),
							EFFECT_FLAG_DIE|EFFECT_FLAG_GENDERBASISTARGET);


	ChangeMotion( DieMotion, 0 );
	ChangeBaseMotion( DiedMotion );

	CObject::Die(pAttacker,bFatalDamage,bCritical,bDecisive);

	// desc_hseos_�󱼱�ü01
	// S �󱼱�ü �߰� added by hseos 2007.06.12
	// ..���� �� �� ������..
	SetFaceShape(GetCharacterTotalInfo()->FaceType*2+1+72);
	// E �󱼱�ü �߰� added by hseos 2007.06.12
}

// 080910 LUJ, ������ ���� ó���� RESULTINFO ����ü�� �޾� ó���ϵ��� ��
void CPlayer::Damage(CObject* pAttacker,BYTE DamageKind,DWORD Damage,const RESULTINFO& result)
{
	// desc_hseos_������ȭ����01
	// S ������ȭ���� �ּ�ó�� added by hseos 2007.08.02
	SetObjectBattleState( eObjectBattleState_Battle );
	if (pAttacker)
		pAttacker->SetObjectBattleState( eObjectBattleState_Battle );
	// E ������ȭ���� �ּ�ó�� added by hseos 2007.08.02

	if(Damage != 0)
	{
		if( GetState() == eObjectState_None )	
		{
			if(pAttacker)
				MOVEMGR->SetLookatPos(this,&pAttacker->GetCurPosition(),0,gCurTime);
		}

		if( GetState() == eObjectState_Rest && m_ObjectState.bEndState == FALSE &&
			GetEngineObject()->GetCurMotion() != eObjectState_Rest)
		{	// ��������̸� ��������� ������ ����
			//ChangeMotion(1,FALSE);
			//ChangeMotion(eObjectState_RestDamage,FALSE);
			OBJECTSTATEMGR->StartObjectState(this, eObjectState_None);

			
			MSG_BYTE msg ;

			msg.Category	= MP_EMOTION ;
			msg.Protocol	= MP_EMOTION_SITSTAND_SYN ;
			msg.dwObjectID	= gHeroID ;
			msg.bData		= eObjectState_None ;

			NETWORK->Send(&msg, sizeof(MSG_BYTE));
			
		}
		else if(GetState() == eObjectState_None)
		{
			const eWeaponAnimationType  weapon = GetWeaponAniType();

			switch(DamageKind) {
			case eDamageKind_Front:
				ChangeMotion( CHARACTER_MOTION[ eCharacterMotion_FrontDamage ][ weapon ], FALSE );
				break;
			}

			SetWingObjectMotion( eWingMotion_Hit );
		}
		
		int DamageEffect = -1;
	
		if( result.bCritical )
		{
			DamageEffect = eEffect_CriticalDamage;
		}
		else
		{
			DamageEffect = eEffect_NormalDamage;
		}

		if(DamageEffect != -1)
		{
			TARGETSET set;
			set.pTarget = this;
			set.ResultInfo = result;
			
			EFFECTMGR->StartEffectProcess(DamageEffect,pAttacker,&set,1,GetID(),
											EFFECT_FLAG_GENDERBASISTARGET);
		}
	}
	
	if( pAttacker == HERO || pAttacker == HEROPET ||this == HERO )	//�ڽ��� ������ ��ȣ�� ǥ�� //KES 040801
	{
		EFFECTMGR->AddDamageNumber(Damage,pAttacker,this,DamageKind,result);
	}
	else if( pAttacker )
	{
		if( OPTIONMGR->IsShowMemberDamage() )
		if( PARTYMGR->IsPartyMember( pAttacker->GetID() ) )
		{
			EFFECTMGR->AddDamageNumber(Damage,pAttacker,this,DamageKind,result);
		}
	}

	// 080616 LUJ, Ŭ���̾�Ʈ�� HP �ս��� ���� ó���ϱ� ������ ������ ������ �߻��� �� �ִ�
	//				5�� ��ų�� ���� �������� HP/MP�� ��ȯ�ϴ� ����� �߰��Ǿ��� �����̴�.
	//				���� HP �ս��� �߻��ϸ� ������ HP ������ ��û�ؼ� ������Ʈ�ϵ��� �Ѵ�
	// 090507 LUJ, ��û/��� ������Ʈ ����� HP�� �����ϵ��� ����
	if(GetID() == gHeroID ||
		pAttacker == HERO)
	{
		MSG_DWORD message;
		ZeroMemory( &message, sizeof( message ) );
		message.Category = MP_CHAR;
		message.Protocol = MP_CHAR_LIFE_GET_SYN;
		message.dwObjectID = gHeroID;
		message.dwData = GetID();
		NETWORK->Send( &message, sizeof( message ) );
	}
	else
	{
		SetLife(
			GetLife() < Damage ? 0 : GetLife() - Damage);
	}
}

// 100223 ShinJS --- ���� ������ �߰�
void CPlayer::ManaDamage(CObject* pAttacker,BYTE DamageKind,DWORD Damage,const RESULTINFO& result)
{
	SetObjectBattleState( eObjectBattleState_Battle );
	if (pAttacker)
		pAttacker->SetObjectBattleState( eObjectBattleState_Battle );

	if(Damage != 0)
	{
		if( GetState() == eObjectState_None )	
		{
			if(pAttacker)
				MOVEMGR->SetLookatPos(this,&pAttacker->GetCurPosition(),0,gCurTime);
		}

		if( GetState() == eObjectState_Rest && m_ObjectState.bEndState == FALSE &&
			GetEngineObject()->GetCurMotion() != eObjectState_Rest)
		{
			OBJECTSTATEMGR->StartObjectState(this, eObjectState_None);
			
			MSG_BYTE msg ;
			msg.Category	= MP_EMOTION ;
			msg.Protocol	= MP_EMOTION_SITSTAND_SYN ;
			msg.dwObjectID	= gHeroID ;
			msg.bData		= eObjectState_None ;

			NETWORK->Send(&msg, sizeof(MSG_BYTE));
		}
		else if(GetState() == eObjectState_None)
		{
			const eWeaponAnimationType  weapon = GetWeaponAniType();

			switch(DamageKind) 
			{
			case eDamageKind_Front:
				ChangeMotion( CHARACTER_MOTION[ eCharacterMotion_FrontDamage ][ weapon ], FALSE );
				break;
			}

			SetWingObjectMotion( eWingMotion_Hit );
		}
		
		int DamageEffect = -1;
	
		if( result.bCritical )
		{
			DamageEffect = eEffect_CriticalDamage;
		}
		else
		{
			DamageEffect = eEffect_NormalDamage;
		}

		if(DamageEffect != -1)
		{
			TARGETSET set;
			set.pTarget = this;
			set.ResultInfo = result;
			
			EFFECTMGR->StartEffectProcess(DamageEffect,pAttacker,&set,1,GetID(),
											EFFECT_FLAG_GENDERBASISTARGET);
		}
	}
	
	if( pAttacker == HERO || pAttacker == HEROPET ||this == HERO )	//�ڽ��� ������ ��ȣ�� ǥ�� //KES 040801
	{
		EFFECTMGR->AddManaDamageNumber(Damage,pAttacker,this,DamageKind,result);
	}
	else if( pAttacker )
	{
		if( OPTIONMGR->IsShowMemberDamage() )
		if( PARTYMGR->IsPartyMember( pAttacker->GetID() ) )
		{
			EFFECTMGR->AddManaDamageNumber(Damage,pAttacker,this,DamageKind,result);
		}
	}

	// Mana ����
	if( this == HERO )
	{
		DWORD mana = HERO->GetMana();
		mana = (mana < Damage ? 0 : mana - Damage);
		HERO->SetMana( mana );

		MSGBASE msg;
		ZeroMemory( &msg, sizeof( msg ) );
		msg.Category = MP_CHAR;
		msg.Protocol = MP_CHAR_MANA_GET_SYN;
		msg.dwObjectID = HEROID;
		NETWORK->Send( &msg, sizeof( msg ) );
	}
}

void CPlayer::SetLife(DWORD life, BYTE type)
{
	CObject::SetLife( life, type );
	m_CharacterInfo.Life = life; 

	
	if(GetState() == eObjectState_Die)
		life = 0;

	if( !(GAMEIN->GetMonsterGuageDlg()->GetCurTarget()) )
		return;

	if( GAMEIN->GetMonsterGuageDlg())
	{
		if( GAMEIN->GetMonsterGuageDlg()->GetWantedTargetID() == GetID() )
		{
			GAMEIN->GetMonsterGuageDlg()->SetMonsterLife( life, DoGetMaxLife() );
		}
	}
}

void CPlayer::Heal(CObject* pHealer,BYTE HealKind,DWORD HealVal)
{
	if(pHealer)
	{
		if( GetObjectBattleState() == eObjectBattleState_Battle )
		{
			pHealer->SetObjectBattleState(
				eObjectBattleState_Battle);
		}

		const BOOL bIsParty = OPTIONMGR->IsShowMemberDamage() && PARTYMGR->IsPartyMember(
			pHealer->GetID());
		
		if(const BOOL bIsShowHealNumber = (bIsParty || GetID() == gHeroID || pHealer->GetID() == gHeroID))
		{
			EFFECTMGR->AddHealNumber(
				HealVal,
				pHealer,
				this,
				HealKind);
		}
	}

	DWORD life = GetLife() + HealVal;
	SetLife(life);
}

void CPlayer::Recharge(CObject* pHealer,BYTE RechargeKind,DWORD RechargeVal)
{
	// desc_hseos_������ȭ����01
	// S ������ȭ���� �ּ�ó�� added by hseos 2007.08.02
	if( GetObjectBattleState() == eObjectBattleState_Battle )
	{
		if( pHealer )
			pHealer->SetObjectBattleState( eObjectBattleState_Battle );
	}
	// E ������ȭ���� �ּ�ó�� added by hseos 2007.08.02

	DWORD mana = GetMana() + RechargeVal;
	SetMana(mana);
}

//---KES �����˻� 2008.3.11
void CPlayer::ShowStreetStallTitle( BOOL bShow, char* strTitle, DWORD dwColor )
{
	if( !m_pObjectBalloon )									return;
	if( !(m_bObjectBalloonBits & ObjectBalloon_Title ) )	return;

	if( STREETSTALLMGR->IsSearchWordIn( strTitle ) )
		m_pObjectBalloon->ShowStreetStallTitle( bShow, strTitle, RGB_HALF(255,255,0), RGB_HALF(100,100,255) );
	else
		m_pObjectBalloon->ShowStreetStallTitle( bShow, strTitle, RGB_HALF(10,10,10), RGB_HALF(255,255,255) );
}

void CPlayer::SetStreetStallTitle( char* strTitle )
{
	if( m_pObjectBalloon )
	if( m_pObjectBalloon->GetSSTitleTip() )
	{
		if( STREETSTALLMGR->IsSearchWordIn( strTitle ) )
		{
			m_pObjectBalloon->GetSSTitleTip()->SetFGColor( RGB_HALF(255,255,0) );
			m_pObjectBalloon->GetSSTitleTip()->SetBGColor( RGB_HALF(100,100,255) );
		}
		else
		{
			m_pObjectBalloon->GetSSTitleTip()->SetFGColor( RGB_HALF(10,10,10) );
			m_pObjectBalloon->GetSSTitleTip()->SetBGColor( RGB_HALF(255,255,255) );		
		}

		m_pObjectBalloon->GetSSTitleTip()->SetTitle( strTitle );		
	}
}

void CPlayer::ShowStreetBuyStallTitle( BOOL bShow, char* strTitle, DWORD dwColor )
{
	if( !m_pObjectBalloon )									return;
	if( !(m_bObjectBalloonBits & ObjectBalloon_Title ) )	return;

	if( STREETSTALLMGR->IsSearchWordIn( strTitle ) )
		m_pObjectBalloon->ShowStreetBuyStallTitle( bShow, strTitle, RGB_HALF(255,255,0), RGB_HALF(255,100,100) );
	else
		m_pObjectBalloon->ShowStreetBuyStallTitle( bShow, strTitle, RGB_HALF(10,10,10), RGB_HALF(255,255,255) );
}

void CPlayer::SetStreetBuyStallTitle( char* strTitle )
{
	if( m_pObjectBalloon )
	if( m_pObjectBalloon->GetSBSTitleTip() )
	{
		if( STREETSTALLMGR->IsSearchWordIn( strTitle ) )
		{
			m_pObjectBalloon->GetSBSTitleTip()->SetFGColor( RGB_HALF(255,255,0) );
			m_pObjectBalloon->GetSBSTitleTip()->SetBGColor( RGB_HALF(255,100,100) );
		}
		else
		{
			m_pObjectBalloon->GetSBSTitleTip()->SetFGColor( RGB_HALF(10,10,10) );
			m_pObjectBalloon->GetSBSTitleTip()->SetBGColor( RGB_HALF(255,255,255) );		
		}

		m_pObjectBalloon->GetSBSTitleTip()->SetTitle( strTitle );
	}
}

void CPlayer::RefreshStreetStallTitle()
{
	if( !m_pObjectBalloon )									return;

	CStreetStallTitleTip* pSS = m_pObjectBalloon->GetSSTitleTip();
	if( pSS )
	if( pSS->IsShow() )
	{
		if( STREETSTALLMGR->IsSearchWordIn( pSS->GetTitle() ) )
		{
			pSS->SetFGColor( RGB_HALF(255,255,0) );
			pSS->SetBGColor( RGB_HALF(100,100,255) );
		}
		else
		{
			pSS->SetFGColor( RGB_HALF(10,10,10) );
			pSS->SetBGColor( RGB_HALF(255,255,255) );
		}
	}

	CStreetStallTitleTip* pSB = m_pObjectBalloon->GetSBSTitleTip();
	if( pSB )
	if( pSB->IsShow() )
	{
		if( STREETSTALLMGR->IsSearchWordIn( pSB->GetTitle() ) )
		{
			pSB->SetFGColor( RGB_HALF(255,255,0) );
			pSB->SetBGColor( RGB_HALF(255,100,100) );
		}
		else
		{
			pSB->SetFGColor( RGB_HALF(10,10,10) );
			pSB->SetBGColor( RGB_HALF(255,255,255) );
		}		
	}
}

float CPlayer::DoGetMoveSpeed()
{
	CHero*	hero	= HERO;
	float	Speed	= 0;

	// 100604 ShinJS --- �̵��� Ż�Ϳ� ž������ ���
	CVehicle* pVehicle = (CVehicle*)OBJECTMGR->GetObject( GetVehicleID() );
	if( pVehicle && 
		pVehicle->GetObjectKind() == eObjectKind_Vehicle &&
		IsGetOnVehicle() &&
		pVehicle->DoGetMoveSpeed() > 0.f )
	{
		return pVehicle->DoGetMoveSpeed();
	}

	Speed = float( m_MoveInfo.MoveMode == eMoveMode_Run ? RUNSPEED : WALKSPEED );

	if( this == hero )
	{		
		// 080630 LUJ, ��Ʈ ���������� ȹ��� �̵� �ӵ� ���ʽ��� ����ǵ��� ��
		float addrateval	= ( hero->GetRateBuffStatus()->MoveSpeed + hero->GetRatePassiveStatus()->MoveSpeed ) / 100.f + hero->GetItemBaseStats().mMoveSpeed.mPercent + hero->GetItemOptionStats().mMoveSpeed.mPercent + hero->GetSetItemStats().mMoveSpeed.mPercent;
		float addval		= hero->GetBuffStatus()->MoveSpeed + hero->GetPassiveStatus()->MoveSpeed + hero->GetItemBaseStats().mMoveSpeed.mPlus + hero->GetItemOptionStats().mMoveSpeed.mPlus + hero->GetSetItemStats().mMoveSpeed.mPlus;

		m_MoveInfo.AddedMoveSpeed = Speed * addrateval + addval;
	}

	Speed += m_MoveInfo.AddedMoveSpeed;

	return max( 1, Speed );
}

void CPlayer::SetPKMode( BOOL bPKMode )
{
	m_CharacterInfo.bPKMode = bPKMode ? true : false;	
}

DWORD CPlayer::DoGetMaxLife()
{
	return m_CharacterInfo.MaxLife;
}

void CPlayer::SetBadFame(FAMETYPE val)
{
	m_CharacterInfo.BadFame = val;
}

void CPlayer::SetGuildIdxRank(DWORD GuildIdx, BYTE Rank)
{ 
	m_CharacterInfo.MunpaID = GuildIdx;
	m_CharacterInfo.PositionInMunpa = Rank;
}

void CPlayer::SetGuildMarkName(MARKNAMETYPE MarkName)
{
	m_CharacterInfo.MarkName = MarkName;
}

void CPlayer::SetNickName(char* pNickName)
{
	SafeStrCpy(m_CharacterInfo.NickName, pNickName, MAX_GUILD_NICKNAME+1);
}

MARKNAMETYPE CPlayer::GetGuildMarkName()
{
	return m_CharacterInfo.MarkName;
}

void CPlayer::SetGuildIdx(DWORD GuildIdx)
{
	m_CharacterInfo.MunpaID = GuildIdx;
}

void CPlayer::ClearGuildInfo()
{
	m_CharacterInfo.MunpaID = 0;
	m_CharacterInfo.PositionInMunpa = GUILD_NOTMEMBER;
	m_CharacterInfo.MarkName = 0;
	SafeStrCpy(m_CharacterInfo.NickName, "", MAX_GUILD_NICKNAME+1);
	SafeStrCpy(m_CharacterInfo.GuildName, "", MAX_GUILD_NAME+1);
}

char* CPlayer::GetNickName()
{
	return m_CharacterInfo.NickName;
}

void CPlayer::SetGuildName(char* GuildName)
{
	SafeStrCpy(m_CharacterInfo.GuildName, GuildName, MAX_GUILD_NAME+1);
}

char* CPlayer::GetGuildName()
{
	return m_CharacterInfo.GuildName;
}

// 070415 LYW --- PLAYER : Active functions to setting and return stage.
//void CPlayer::SetStage( BYTE Stage )
void CPlayer::SetStage( BYTE grade, BYTE index )
{
	//m_CharacterInfo.Stage = Stage;
	//m_CharacterInfo.JobGrade = Stage ;
	m_CharacterInfo.JobGrade = grade ;
	m_CharacterInfo.Job[grade - 1] = index ;
}

void CPlayer::SetCharChangeInfo( CHARACTERCHANGE_INFO* pInfo )
{
	m_CharacterInfo.Gender = pInfo->Gender;
	m_CharacterInfo.HairType = pInfo->HairType;
	m_CharacterInfo.FaceType = pInfo->FaceType;
	m_CharacterInfo.Height = pInfo->Height;
	m_CharacterInfo.Width = pInfo->Width;

	APPEARANCEMGR->AddCharacterPartChange( GetID() );
}

//---KES CHEAT CHANGESIZE
void CPlayer::SetCharacterSize( float fSize )
{
	m_CharacterInfo.Width = m_CharacterInfo.Height = fSize ;

	APPEARANCEMGR->AddCharacterPartChange( GetID() );

	const float playerDefaultSize = 180;
	m_pObjectBalloon->SetTall(
		LONG(playerDefaultSize * fSize));
}
//-----------------------

void CPlayer::SetGTName(DWORD NameType)
{
	if( m_pObjectBalloon )
		m_pObjectBalloon->SetGTName(NameType, GetGuildName());
}

void CPlayer::ChangeFace( BYTE faceNum )
{
	m_EngineObject.SetMaterialIndex( faceNum ) ;

	m_FacialTime.Init(5000);
	m_FacialTime.Start();

	m_bStartCheck = TRUE ;

	// 070312 LYW --- Player : Stop close eye.
	m_bActiveEye = FALSE ;
}

void CPlayer::ReturnFace()
{
	m_EngineObject.SetMaterialIndex( 0x01 ) ;

	m_bStartCheck = FALSE ;

	// 070312 LYW --- Player : Start close eye.
	InitializeEye() ;
}

// 070528 LYW --- Player : Add function to syn to server play emoticon.
void CPlayer::PlayEmoticon_Syn(BYTE EmoticonNum)
{
#ifdef _TESTCLIENT_
	PlayEmoticon( EmoticonNum );
	return;
#endif
	MSG_INT data ;

	data.Category	= MP_EMOTION ;
	data.Protocol	= MP_EMOTICON_SYN ;
	data.dwObjectID	= HEROID ;
	data.nData		= EmoticonNum ;

	NETWORK->Send( &data, sizeof(MSG_INT) ) ;
}

void CPlayer::PlayEmoticon(int nEmoticonNum)
{
	EFFECTMGR->StartPlayerEffectProcess(this, nEmoticonNum) ;
}

// 070312 LYW --- Player : Add function for eye part.
void CPlayer::CloseEye() 
{
	m_EngineObject.SetMaterialIndex( 0x02 ) ;

	m_bClosed = TRUE ;
}

void CPlayer::InitializeEye()
{
	m_EngineObject.SetMaterialIndex( 0x01 ) ;

	m_EyeTime.End();
	m_EyeTime.Init(5000);
	m_EyeTime.Start();

	m_bActiveEye = TRUE ;
	m_bClosed	 = FALSE ;
}

// 070312 LYW --- Player : Add function to return MOD_LIST.
MOD_LIST* CPlayer::GetModList() 
{
	BYTE gender = m_CharacterInfo.Gender;
	BYTE race	= m_CharacterInfo.Race;

	return &GAMERESRCMNGR->m_FaceModList[race][gender];
}

void CPlayer::SetFamilyMarkName(MARKNAMETYPE MarkName)
{
	m_CharacterInfo.FamilyMarkName = MarkName;
}

void CPlayer::SetFamilyNickName(char* pNickName)
{
	SafeStrCpy(m_CharacterInfo.FamilyNickName, pNickName, MAX_GUILD_NICKNAME+1);
}

MARKNAMETYPE CPlayer::GetFamilyMarkName()
{
	return m_CharacterInfo.MarkName;
}

void CPlayer::SetFamilyIdx(DWORD FamilyIdx)
{
	m_CharacterInfo.FamilyID = FamilyIdx;
}

void CPlayer::ClearFamilyInfo()
{
	m_CharacterInfo.FamilyID = 0;
	m_CharacterInfo.MarkName = 0;
	m_CharacterInfo.FamilyNickName[ 0 ]	= 0;
	m_CharacterInfo.FamilyName[ 0 ]		= 0;
}

char* CPlayer::GetFamilyNickName()
{
	return m_CharacterInfo.FamilyNickName;
}

void CPlayer::SetFamilyName(char* FamilyName)
{
	SafeStrCpy( m_CharacterInfo.FamilyName, FamilyName, sizeof( m_CharacterInfo.FamilyName ) );
}

char* CPlayer::GetFamilyName()
{
	return m_CharacterInfo.FamilyName;
}

// desc_hseos_�󱼱�ü01
// S �󱼱�ü �߰� added by hseos 2007.06.07
void CPlayer::SetFaceAction(int nAction, int nShape)
{ 
	if (nAction == FACE_ACTION_KIND_EYE_BLINK)
	{
		m_stFaceAction.nFaceShapeState = PLAYER_FACE_SHAPE_EYE_OPEN;
	}

	m_stFaceAction.nFaceActionKind = nAction; 
	m_stFaceAction.nFaceShapeTimeTick = gCurTime;
	SetFaceShape(nShape);
}

void CPlayer::ProcessFaceAction()
{
	switch(m_stFaceAction.nFaceActionKind)
	{
	// �� ������
	case FACE_ACTION_KIND_EYE_BLINK:
		{
 			if (this->IsDied()) break;

			int nRndTime = 0;
			switch(m_stFaceAction.nFaceShapeState)
			{
				// �� �� ����
				case PLAYER_FACE_SHAPE_EYE_OPEN:
					// ���� �ڿ��������� ���� +-������ ����
					nRndTime = CSHMath::GetRandomNum(0, CFacialManager::PLAYER_FACE_EYE_BLINK_OPEN_ADD_RND_TIME);
					if (CSHMath::GetRandomNum(0,1)) nRndTime *= -1;
					// ǥ�� �ð� üũ
					if (gCurTime - m_stFaceAction.nFaceShapeTimeTick > CFacialManager::PLAYER_FACE_EYE_BLINK_OPEN_TIME+nRndTime)
					{
						m_stFaceAction.nFaceShapeState = PLAYER_FACE_SHAPE_EYE_CLOSE;
						m_stFaceAction.nFaceShapeTimeTick = gCurTime;
						SetFaceShape(GetCharacterTotalInfo()->FaceType*2+1+72);
					}
					break;
				// �� ���� ����
				case PLAYER_FACE_SHAPE_EYE_CLOSE: 
					// ���� �ڿ��������� ���� +-������ ����
					nRndTime = CSHMath::GetRandomNum(0, CFacialManager::PLAYER_FACE_EYE_BLINK_CLOSE_ADD_RND_TIME);
					if (CSHMath::GetRandomNum(0,1)) nRndTime *= -1;
					// ǥ�� �ð� üũ
 					if (gCurTime - m_stFaceAction.nFaceShapeTimeTick > CFacialManager::PLAYER_FACE_EYE_BLINK_CLOSE_TIME+nRndTime)
					{
						m_stFaceAction.nFaceShapeState = PLAYER_FACE_SHAPE_EYE_OPEN;
						m_stFaceAction.nFaceShapeTimeTick = gCurTime;
						SetFaceShape(GetCharacterTotalInfo()->FaceType*2+72);
					}
					break;
			}
		}
		break;
	// ä��â ���ɾ�
	case FACE_ACTION_KIND_EYE_CHATTING_CMD:
		if (gCurTime - m_stFaceAction.nFaceShapeTimeTick > CFacialManager::PLAYER_FACE_SHAPE_TIME)
		{
			SetFaceAction(FACE_ACTION_KIND_EYE_BLINK, GetCharacterTotalInfo()->FaceType*2+72);
		}
		break;
	}
}
// E �󱼱�ü �߰� added by hseos 2007.06.07

// desc_hseos_����ý���_01
// S ����ý��� �߰� added by hseos 2007.09.19
// 091117 pdy ���� ��ǽ� ���� ����Ʈ�� ������� ���� ����
VOID CPlayer::StartFarmMotion(int nMotionNum, DWORD nMotionTime)
{
	APPEARANCEMGR->HideWeapon(this);
	HEFFPROC nEff = EFFECTMGR->StartEffectProcess(nMotionNum, this, NULL, 0, 0);

	if (gHeroID == GetID()) g_csFarmManager.CLI_SetIngCharMotion(nEff);

	m_nFarmMotionTimeTick = gCurTime;
	m_nFarmMotionTime = nMotionTime;
	// ĳ���� �̸� �Ʒ� ���α׷��� ����
	GetObjectBalloon()->GetProgressBar()->CLI_Start(CSHFarmManager::FARM_CHAR_MOTION_PROGRESSBAR_TYPE, CSHFarmManager::FARM_CHAR_MOTION_PROGRESSBAR_WIDTH, nMotionTime);
}

// 091117 pdy ���� ��ǽ� ���� ����Ʈ�� ������� ���� ����
VOID CPlayer::ProcFarmMotion()
{
	if (m_nFarmMotionTime &&
  		gCurTime - m_nFarmMotionTimeTick > m_nFarmMotionTime)
	{
		APPEARANCEMGR->ShowWeapon( this ) ;

		if (gHeroID == GetID()) g_csFarmManager.CLI_SetIngCharMotion(NULL);

		m_nFarmMotionTimeTick = NULL;
		m_nFarmMotionTime = NULL;
	}
}
// E ����ý��� �߰� added by hseos 2007.09.19

BOOL CPlayer::CheckHeart( CPlayer* opponent )
{
	const DATE_MATCHING_INFO&	myData			= GetDateMatchingInfo();
	const DATE_MATCHING_INFO&	opponentData	= opponent->GetDateMatchingInfo();
	const CHARACTER_TOTALINFO*	opponentInfo	= opponent->GetCharacterTotalInfo();

	if(		opponent == this						||
		!	opponentInfo->bVisible					||
		!	myData.bIsValid							||
		!	opponentData.bIsValid					||
			myData.byGender	== opponentData.byGender	)
    {
        return FALSE;
    }

    int score = 100;

    // ���� üũ
    {
        const int ageGap = abs( ( int )myData.dwAge - ( int )opponentData.dwAge );

        score -= ageGap;
    }

    // ���� ���� üũ
    {
        const int areaGap = ( myData.dwRegion == opponentData.dwRegion ) ? 0 : 10;

        score -= areaGap;
    }

    // ���� ��: �ϳ��� ����Ʈ, �ϳ��� �¿� �ְ� ����Ʈ�� �ݺ��ڷ� ������. �¿� ���� ������ �����Ѵ�.
    // ��ȣ ���� ��
    {
        const size_t size = sizeof( myData.pdwGoodFeeling ) / sizeof( DWORD );

        std::set< DWORD > opponentFavorite;
        opponentFavorite.insert( opponentData.pdwGoodFeeling, opponentData.pdwGoodFeeling + size );

        for( size_t i = 0; i < size; ++i )
        {
			bool		isFound		= false;				
			const DWORD myFavorite	= myData.pdwGoodFeeling[ i ];

			for( size_t j = 0; j < size; ++j )
			{
				const DWORD opponentFavorite = opponentData.pdwGoodFeeling[ j ];

				if( myFavorite == opponentFavorite )
				{
					isFound = true;
					break;
				}
			}

			if( ! isFound )
			{
				const int favoriteGap = 10;

				score -= favoriteGap;
			}
        }
    }

    // ���� ���� ��
    {
        const size_t size = sizeof( myData.pdwBadFeeling ) / sizeof( DWORD );

        std::set< DWORD > opponentFavorite;
        opponentFavorite.insert( opponentData.pdwBadFeeling, opponentData.pdwBadFeeling + size );

        for( size_t i = 0; i < size; ++i )
        {
			bool		isFound		= false;				
			const DWORD myFavorite	= myData.pdwBadFeeling[ i ];

			for( size_t j = 0; j < size; ++j )
			{
				const DWORD opponentFavorite = opponentData.pdwBadFeeling[ j ];

				if( myFavorite == opponentFavorite )
				{
					isFound = true;
					break;
				}
			}

			if( ! isFound )
			{
				const int favoriteGap = 10;

				score -= favoriteGap;
			}
        }
    }

	const int matchScore = CSHDateManager::NOTICE_HEART_MATCHING_POING;

    return matchScore <= score;
}


const DATE_MATCHING_INFO& CPlayer::GetDateMatchingInfo() const
{
	return m_CharacterInfo.DateMatching;
}


void CPlayer::SetHeart( BOOL isHeart )
{
	if(	isHeart )
	{
		if( ! m_IsDrawHeart )
		{
			OBJECTEFFECTDESC desc(20);
			
			AddObjectEffect( HEART_EFFECT, &desc, 1 );

			m_IsDrawHeart = TRUE;
		}
	}
	else
	{
		RemoveObjectEffect( HEART_EFFECT );

		m_IsDrawHeart = FALSE;
	}
}


const CPlayer::SetItemLevel& CPlayer::GetSetItemLevel() const
{
	return mSetItemLevel;
}


CPlayer::SetItemLevel& CPlayer::GetSetItemLevel()
{
	return mSetItemLevel;
}

void CPlayer::RemoveEffects()
{
	EFFECTMGR->ForcedEndEffect( mWeaponEffect[0] ) ;
	EFFECTMGR->ForcedEndEffect( mWeaponEffect[1] ) ;
	EFFECTMGR->ForcedEndEffect( mWeaponEffect[2] ) ;

	mWeaponEffect[0] = 0 ;
	mWeaponEffect[1] = 0 ;
	mWeaponEffect[2] = 0 ;


	// ���� ����Ʈ�� ���� ���� ��Ų��.
	EFFECTMGR->ForcedEndEffect( mJobEffect ) ;

	mJobEffect = 0 ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: StartJobEffect
//	DESC		: Player�� ���� ����Ʈ�� �����ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: February 10, 2009
//-------------------------------------------------------------------------------------------------
void CPlayer::StartJobEffect()
{
	// Ȱ��ȭ ���� ���� ����Ʈ�� ������ �����Ѵ�.
	EndJobEffect() ;


	// ���� ����Ʈ�� Ȱ��ȭ �Ѵ�.
	WORD		PartType	  = 0 ;
	WORD		PartModelNum  = 0 ;
	WORD		ITEM_3D		  = 0 ;
	ITEM_INFO*	pInfo		  = NULL ;
	    ITEM_INFO*	transmogInfo  = NULL;
	DWORD*		WearedItemIdx = m_CharacterInfo.WearedItemIdx ;
    DWORD*		WearedItemTransmog = m_CharacterInfo.WearedItemTransmog ;

	char HideParts[eWearedItem_Max] = {0};
	EncodeHidePartsFlag(
		m_CharacterInfo.HideFlag,
		HideParts);

	for( int n = 0 ; n < eWearedItem_Max ; ++n )
	{
		if(!WearedItemIdx[ n ] || HideParts[ n ]) continue ;

		pInfo = ITEMMGR->GetItemInfo(WearedItemIdx[ n ]) ;
		if( !pInfo ) continue ;

		PartType	 = pInfo->Part3DType ;
		PartModelNum = pInfo->Part3DModelNum ;

		// 3D ��Ʈ ������ ������ �����ܸ� ǥ�õǴ� �༮���̴�.
		if( PartType == ePartType_None ) continue ;

		if( PartModelNum == 65535 ) continue ;

		if( PartType != ePartType_Job_Effect ) continue ;

		 if(transmogInfo = ITEMMGR->GetItemInfo(WearedItemTransmog[ n ]))
			ITEM_3D = transmogInfo->Part3DModelNum;
		else
			ITEM_3D = 0;



		TARGETSET set ;
		memset( &set, 0, sizeof(TARGETSET) ) ;

		set.pTarget = this ;

		mJobEffect = EFFECTMGR->StartEffectProcess( PartModelNum, this, &set, 0, GetID() ) ;

		break ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: EndJobEffect
//	DESC		: Player�� ���� ����Ʈ�� �����ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: February 10, 2009
//-------------------------------------------------------------------------------------------------
void CPlayer::EndJobEffect()
{
	if( mJobEffect )
	{
		EFFECTMGR->ForcedEndEffect( mJobEffect ) ;
	}
}

// 090204 LUJ, Ư�� ��ġ�� ���� �ִϸ��̼� Ÿ���� ��ȯ�Ѵ�
eWeaponAnimationType CPlayer::GetWeaponAniType( EWEARED_ITEM wearType )
{
	const ITEM_INFO* const itemInfo = ITEMMGR->GetItemInfo( GetWearedItemIdx( wearType ) );

	return itemInfo ? eWeaponAnimationType( itemInfo->WeaponAnimation ) : eWeaponAnimationType_None;
}

// 090204 LUJ, ������ ���� �� ���� ��ȯ
eArmorType CPlayer::GetArmorType( EWEARED_ITEM wearType )
{
	const ITEM_INFO* const itemInfo = ITEMMGR->GetItemInfo( GetWearedItemIdx( wearType ) );

	return itemInfo ? eArmorType( itemInfo->ArmorType ) : eArmorType_None;
}

// 090908 IROS -- Player : Change Part Check.
DWORD CPlayer::GetChangePart(DWORD WearedPosition)
{
	ASSERT( WearedPosition < eWearedItem_Max);

	return m_ChangedPartIdx[WearedPosition];
}

void CPlayer::ClearChangeParts()
{
	ZeroMemory(&m_ChangedPartIdx, sizeof(m_ChangedPartIdx));
}

void CPlayer::SetWearedItemTransmog(DWORD WearedPosition,DWORD ItemIdx)
{
	ASSERT( sizeof( m_CharacterInfo.WearedItemTransmog ) / sizeof( DWORD ) > WearedPosition );

	if (m_CharacterInfo.WearedItemTransmog[WearedPosition] == ItemIdx)
		return;

	m_CharacterInfo.WearedItemTransmog[WearedPosition] = ItemIdx;
}



// 090423 ShinJS --- Ż���� ��ȯ ������ �������� �Ǵ�
BOOL CPlayer::CanSummonVehicle()
{
	// Player ���� �˻�
	switch( GetState() )
	{
	case eObjectState_Rest:
	case eObjectState_Exchange:
	case eObjectState_StreetStall_Owner:
	case eObjectState_StreetStall_Guest:
	case eObjectState_PrivateWarehouse:
	case eObjectState_Munpa:
	case eObjectState_SkillStart:
	case eObjectState_SkillSyn:
	case eObjectState_SkillBinding:
	case eObjectState_SkillUsing:
	case eObjectState_SkillDelay:
	case eObjectState_TiedUp_CanMove:
	case eObjectState_TiedUp_CanSkill:
	case eObjectState_TiedUp:
	case eObjectState_Die:
	case eObjectState_Fishing:
	case eObjectState_FishingResult:
	case eObjectState_Cooking:
	case eObjectState_ItemUse:
	case eObjectState_Society:
	case eObjectState_Exit:
	case eObjectState_Deal:
		return FALSE;
	}

	// �ٸ� Ż���� �̹� ��ȯ�Ǿ� �ִ� ��� ��ȯ ����
	if( GetVehicleID() )
	{
		CVehicle* pVehicle = (CVehicle*)OBJECTMGR->GetObject( GetVehicleID() );
		if( !pVehicle || pVehicle->GetObjectKind() != eObjectKind_Vehicle )
			return FALSE;

		// Ż���� Master�� ���
		if( pVehicle->GetOwnerIndex() == GetID() )
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1837 ) );							
		}
		// Ż���� ������ �ʾ����� �ٸ� Player�� Ż�Ϳ� ž������ ���
		else
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1838 ) );
		}

		return FALSE;
	}

	// 091013 ShinJS --- �ð��˻� ���� ���� Ȯ��
    if( VEHICLEMGR->IsCheckTime() )
	{
		// Ż�� ��ȯ/���� ��Ÿ���� ������� ������� ��ȯ ����
		const DWORD dwSummonElapsedTime = gCurTime - GetLastVehicleSummonTime();
		if( dwSummonElapsedTime != gCurTime && dwSummonElapsedTime < CVehicle::eSummonLimitTime )
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1835 ), float(CVehicle::eSummonLimitTime - dwSummonElapsedTime)/1000.0f );
			return FALSE;
		}
	}

	// ������ ��ȯ ����
	if( GetObjectBattleState() == eObjectBattleState_Battle )
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1817 ) );
		return FALSE;
	}

	// �Ͽ콺���� ��ȯ ����
	if( HOUSINGMAP == MAP->GetMapNum())
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1867 ) );
		return FALSE;
	}

	return TRUE;
}

// 090423 ShinJS --- Ż���� ���� ������ �������� �Ǵ�
BOOL CPlayer::CanSealVehicle()
{
	// Player ���� �˻�
	switch( GetState() )
	{
	case eObjectState_Rest:
	case eObjectState_Exchange:
	case eObjectState_StreetStall_Owner:
	case eObjectState_StreetStall_Guest:
	case eObjectState_PrivateWarehouse:
	case eObjectState_Munpa:
	case eObjectState_SkillStart:
	case eObjectState_SkillSyn:
	case eObjectState_SkillBinding:
	case eObjectState_SkillUsing:
	case eObjectState_SkillDelay:
	case eObjectState_TiedUp_CanMove:
	case eObjectState_TiedUp_CanSkill:
	case eObjectState_TiedUp:
	case eObjectState_Die:
	case eObjectState_Fishing:
	case eObjectState_FishingResult:
	case eObjectState_Cooking:
	case eObjectState_ItemUse:
	case eObjectState_Society:
	case eObjectState_Exit:
	case eObjectState_BattleReady:
	case eObjectState_Deal:
		return FALSE;
	}

	CVehicle* pVehicle = (CVehicle*)OBJECTMGR->GetObject( GetVehicleID() );
	if( !pVehicle || pVehicle->GetObjectKind() != eObjectKind_Vehicle )
		return FALSE;

	VECTOR3 vecPlayerPos, vecVehiclePos;
	GetPosition( &vecPlayerPos );
	pVehicle->GetPosition( &vecVehiclePos );
	const float fDist = CalcDistanceXZ( &vecPlayerPos, &vecVehiclePos );
	const VehicleScript& script = GAMERESRCMNGR->GetVehicleScript( pVehicle->GetMonsterKind() );
	if( script.mMonsterKind != pVehicle->GetMonsterKind() )
		return FALSE;

	// Ż�Ͱ� �Ÿ� Ȯ��	
	if( fDist > script.mCondition.mSummonRange )
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1821 ) );
		return FALSE;
	}

	// Ż�� ��ȯ/���� ��Ÿ���� ������� ������� ���� ����
	// 091013 ShinJS --- �ð��˻� ���� ���� Ȯ��
    if( VEHICLEMGR->IsCheckTime() )
	{
		const DWORD dwSummonElapsedTime = gCurTime - GetLastVehicleSummonTime();
		if( dwSummonElapsedTime != gCurTime && dwSummonElapsedTime < CVehicle::eSummonLimitTime )
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1835 ), float(CVehicle::eSummonLimitTime - dwSummonElapsedTime)/1000.0f );
			return FALSE;
		}
	}

	return TRUE;
}

void CPlayer::SetObjectBattleState(eObjectBattleState eBattleState)
{
	CObject::SetObjectBattleState(eBattleState);
}