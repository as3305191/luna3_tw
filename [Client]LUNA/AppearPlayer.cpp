#include "Stdafx.h"
#include "ItemManager.h"
#include "GameResourceManager.h"
#include "AppearanceManager.h"
#include "AppearPlayer.h"
#include "Player.h"
#include "cHousingMgr.h"
#include "ChatManager.h"

CAppearPlayer::CAppearPlayer(void)
{
	// ĳ���� ǥ�� ó���� ����� ���� ������ �ʱ�ȭ.
	InitData() ;
}

CAppearPlayer::~CAppearPlayer(void)
{}

void CAppearPlayer::SetForEffect(BOOL isForEffect)
{
	m_IsForEffect = isForEffect ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: SetPlayer
//	DESC		: ��� ǥ�� ��� Player ������ �����Ѵ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: February 05, 2009
//-------------------------------------------------------------------------------------------------
void CAppearPlayer::SetPlayer( CPlayer* pPlayer )
{
	// �Լ� ���ڸ� Ȯ���Ѵ�.
	if( !pPlayer ) return ;


	// Player ������ �����Ѵ�.
	m_pPlayer = pPlayer ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: SetEngineObject
//	DESC		: ��� ǥ�� ��� ���� ������Ʈ ������ �����Ѵ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: February 05, 2009
//-------------------------------------------------------------------------------------------------
void CAppearPlayer::SetEngineObject( CEngineObject* pEngineObject)
{
	// �Լ� ���ڸ� Ȯ���Ѵ�.
	if( !pEngineObject ) return ;


	// ���� ������Ʈ ������ �����Ѵ�.
	m_pEngineObject = pEngineObject ;
}


// @@@ Iros �ּһ��� �����Ǵ� �ڽ��� ���.
void CAppearPlayer::SetBaseAppearPlayer()
{
	// ����� ���� �� Player�� ���� ������Ʈ ������ ��ȿ���� Ȯ���Ѵ�.
	if( !m_pPlayer || !m_pEngineObject ) return ;

	// Ȱ��ȭ ���̴� Effect�� �����Ѵ�.
	m_pPlayer->RemoveEffects() ;

	// �����ֱ� ���θ� ���� �÷��� ����ü�� �ʱ�ȭ �Ѵ�.
	InitShowFlag() ;

	// MOD ��Ʈ ó���� �Ѵ�.
	if( !InitMOD() ) return ;

	MOD_LIST* pModList = &GAMERESRCMNGR->m_ModFileList[ m_pTotalInfo->Race ][ m_pTotalInfo->Gender ] ;

	int BoxHeadNum = 1722;
	int BoxBodyNum = 1723;
	int BoxHandNum = 1724;
	int BoxBootsNum = 1725;

	m_pEngineObject->ChangePart(eAppearPart_Face,"NULL.MOD") ;
	m_pPlayer->SetFaceObject( NULL ) ;

	m_pEngineObject->ChangePart(eAppearPart_Body, pModList->ModFile[ BoxBodyNum ]);
	m_pEngineObject->ChangePart(eAppearPart_Foot, pModList->ModFile[ BoxBootsNum ]);
	m_pEngineObject->ChangePart(eAppearPart_Hand, pModList->ModFile[ BoxHandNum ]);
	m_pEngineObject->ChangePart(eAppearPart_Hair, pModList->ModFile[ BoxHeadNum ]);

	// ������ ó���� �Ѵ�.
	if( !InitScale() ) return ;

	// ��ġ ���� ó���� �Ѵ�.
	if( !InitPos() ) return ;

	APPEARANCEMGR->HideWeapon(m_pPlayer,m_pEngineObject) ;

	if(const ITEM_INFO* const itemInfo = ITEMMGR->GetItemInfo(m_pPlayer->GetWearedItemIdx(eWearedItem_Weapon)))
	{
		int boxRhandWeapon = 1727;

		switch(itemInfo->Part3DType)
		{
		case ePartType_TwoBlade:
			{
				if(eWeaponType_BothDagger == itemInfo->WeaponType)
				{
					CEngineObject* pRightWeapon = new CEngineObject ;
					pRightWeapon->Init(
						pModList->ModFile[boxRhandWeapon],
						NULL,
						eEngineObjectType_Weapon) ;
					m_pEngineObject->AttachWeapon(
						pRightWeapon,
						RIGHT_HAND) ;

					CEngineObject* pLeftWeapon = new CEngineObject ;
					pLeftWeapon->Init(
						pModList->ModFile[boxRhandWeapon],
						NULL,
						eEngineObjectType_Weapon) ;
					m_pEngineObject->AttachWeapon(
						pLeftWeapon,
						LEFT_HAND) ;
				}

				break;
			}
		default:
			{
				CEngineObject* pWeapon = new CEngineObject ;

				if(eWeaponType_Bow == itemInfo->WeaponType)
				{
					int boxBowWeapon = 1728;

					pWeapon->Init(
						pModList->ModFile[boxBowWeapon],
						NULL,
						eEngineObjectType_Weapon) ;
					m_pEngineObject->AttachWeapon(
						pWeapon,
						LEFT_HAND);
				}
				else
				{
					pWeapon->Init(
						pModList->ModFile[boxRhandWeapon],
						NULL,
						eEngineObjectType_Weapon);
					m_pEngineObject->AttachWeapon(
						pWeapon,
						RIGHT_HAND);
				}

				break;
			}
		}
	}

	if(const ITEM_INFO* const itemInfo = ITEMMGR->GetItemInfo(m_pPlayer->GetWearedItemIdx(eWearedItem_Shield)))
	{
		int boxShield = 1726;

		CEngineObject* pShield	= new CEngineObject ;
		pShield->Init(
			pModList->ModFile[boxShield],
			NULL,
			eEngineObjectType_Weapon) ;

		m_pEngineObject->AttachWeapon(
			pShield,
			LEFT_HAND);
	}

	m_pEngineObject->EnableSelfIllumin() ;
}


//-------------------------------------------------------------------------------------------------
//	NAME		: Renewal
//	DESC		: Player�� ����� �����Ѵ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: February 05, 2009
//-------------------------------------------------------------------------------------------------
void CAppearPlayer::RenewalAll()
{
	// ����� ���� �� Player�� ���� ������Ʈ ������ ��ȿ���� Ȯ���Ѵ�.
	if( !m_pPlayer || !m_pEngineObject ) return ;

	// Ȱ��ȭ ���̴� Effect�� �����Ѵ�.
	m_pPlayer->RemoveEffects() ;

	// �����ֱ� ���θ� ���� �÷��� ����ü�� �ʱ�ȭ �Ѵ�.
	InitShowFlag() ;

	// MOD ��Ʈ ó���� �Ѵ�.
	if( !InitMOD() ) return ;

	// �Ӹ�(���) ó���� �Ѵ�.
	if( !InitHair() ) return ;

	// �ٲ�� �κе� ó���� �Ѵ�.
	if( !ChangePart() ) return ;

	// �ٿ����� �κе� ó���� �Ѵ�.
	if( !AttachPart() ) return ;

	// �� ��Ʈ ó���� �Ѵ�.
	if( !InitHead() ) return ;

	// ������ ó���� �Ѵ�.
	if( !InitScale() ) return ;

	// ��ġ ���� ó���� �Ѵ�.
	if( !InitPos() ) return ;

	// ���⸦ ǥ���Ѵ�.
	if( !AppearWeapone() ) return ;

	// HERO ĳ���ʹ� ��ü ���콺 ��ŷ�� �ȵǵ��� �Ѵ�.
	if(m_pPlayer->GetID() == HEROID)
	{
		m_pEngineObject->DisablePick() ;
	}

	// ���� ������Ʈ�� ��ü ����Ʈ ȿ�� ó���� �Ѵ�.
	m_pEngineObject->EnableSelfIllumin() ;

	m_pPlayer->ClearChangeParts();
}

void CAppearPlayer::Renewal(DWORD changeFlag)
{
	// ����� ���� �� Player�� ���� ������Ʈ ������ ��ȿ���� Ȯ���Ѵ�.
	if( !m_pPlayer || !m_pEngineObject ) return ;

	m_pTotalInfo = m_pPlayer->GetCharacterTotalInfo() ;

	// Ȱ��ȭ ���̴� Effect�� �����Ѵ�.
	m_pPlayer->RemoveEffects() ;

	// �����ֱ� ���θ� ���� �÷��� ����ü�� �ʱ�ȭ �Ѵ�.
	InitShowFlag() ;

	if (changeFlag & eCHAPPEAR_MOD)
	{
		// MOD ��Ʈ ó���� �Ѵ�.
		if( !InitMOD() ) return ;

		// �Ӹ�(���) ó���� �Ѵ�.
		if( !InitHair() ) return ;
	}

	if (changeFlag & eCHAPPEAR_MOD || changeFlag & eCHAPPEAR_PART)
	{
		// �ٲ�� �κе� ó���� �Ѵ�.
		if( !ChangePart() ) return ;

		// �ٿ����� �κе� ó���� �Ѵ�.
		if( !AttachPart() ) return ;
	}

	// �� ��Ʈ ó���� �Ѵ�.
	if (changeFlag & eCHAPPEAR_MOD)
	{
		if( !InitHead() ) return ;
	}

	// ������ ó���� �Ѵ�.
	if (changeFlag & eCHAPPEAR_MOD || changeFlag & eCHAPPEAR_SCALE)
	{
		if( !InitScale() ) return ;
	}

	// ��ġ ���� ó���� �Ѵ�.
	if( !InitPos() ) return ;

	if (changeFlag & eCHAPPEAR_WEAPON)
	{
		// ���⸦ ǥ���Ѵ�.
		if( !AppearWeapone() ) return ;
	}

	if (changeFlag & eCHAPPEAR_MOD)
	{
		// HERO ĳ���ʹ� ��ü ���콺 ��ŷ�� �ȵǵ��� �Ѵ�.
		if(m_pPlayer->GetID() == HEROID)
		{
			m_pEngineObject->DisablePick() ;
		}

		// ���� ������Ʈ�� ��ü ����Ʈ ȿ�� ó���� �Ѵ�.
		m_pEngineObject->EnableSelfIllumin() ;
	}

	m_pPlayer->ClearChangeParts();
}





//-------------------------------------------------------------------------------------------------
//	NAME		: InitMOD
//	DESC		: ����� ���� �� Player�� MOD ��Ʈ ó���� �Ѵ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: February 05, 2009
//-------------------------------------------------------------------------------------------------
BYTE CAppearPlayer::InitMOD()
{
	// ������ ������ �޴´�.
	m_pTotalInfo = m_pPlayer->GetCharacterTotalInfo() ;
	if( !m_pTotalInfo ) goto ERREND ;

	ASSERT( m_pTotalInfo->Gender <= eWOMAN ) ;


	// MOD ������ �޴´�. ( ���ҽ� �Ŵ������� mod ���� ����Ʈ�� ���� ��Ų ���� ���� �ʴٰ� ������ )
	MOD_LIST* pModList = &GAMERESRCMNGR->m_ModFileList[ m_pTotalInfo->Race ][ m_pTotalInfo->Gender ] ;
	if( !pModList ) goto ERREND ;


	// �� ���� �ʱ�ȭ
	m_pEngineObject->Init( pModList->BaseObjectFile, m_pPlayer, eEngineObjectType_Character ) ;
	// 090316 LUJ, ���� ������ �ʱ�ȭ�������� ������ ������ �޸𸮸� ����ϰ�, Ŭ���̾�Ʈ�� ���� ����ȴ�
	m_pPlayer->SetWingObject( NULL );

	// ��� ����.
	return TRUE ;


// ������ ���� ó���� �Ѵ�.
ERREND :
	InitData() ;
	return FALSE ;
}


//-------------------------------------------------------------------------------------------------
//	NAME		: InitHair
//	DESC		: ����� ���� �� Player�� ��� ��Ʈ ó���� �Ѵ�.
//	PROGRAMMER	: nsoh
//	DATE		: February 20, 2009
//-------------------------------------------------------------------------------------------------
BYTE CAppearPlayer::InitHair() 
{
	if(0 == m_pTotalInfo)
	{
		InitData();
		return FALSE;
	}

	MOD_LIST& modList = GAMERESRCMNGR->m_HairModList[m_pTotalInfo->Race][m_pTotalInfo->Gender];

	if(modList.MaxModFile <= m_pTotalInfo->HairType)
	{
#ifdef _GMTOOL_
		_tcprintf(
			"[����] ��ϵ��� ���� �Ӹ�(%d)�� �⺻ ǥ���մϴ�",
			m_pTotalInfo->HairType);
#endif
		m_pTotalInfo->HairType = 0;
	}

	m_pEngineObject->ChangePart(
		eAppearPart_Hair,
		modList.ModFile[m_pTotalInfo->HairType]);
	return TRUE;
}


//-------------------------------------------------------------------------------------------------
//	NAME		: InitHead
//	DESC		: ����� ���� �� Player�� �Ӹ� ��Ʈ ó���� �Ѵ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: February 05, 2009
//-------------------------------------------------------------------------------------------------
BYTE CAppearPlayer::InitHead()
{
	if(0 == m_pTotalInfo)
	{
		InitData();
		return FALSE;
	}

	m_pEngineObject->ChangePart(
		eAppearPart_Face,
		"NULL.MOD");
	m_pPlayer->SetFaceObject(
		0);

	if(FALSE == m_stShowFlag.IsShowFace)
	{
		return TRUE;
	}

	MOD_LIST& modList = GAMERESRCMNGR->m_FaceModList[m_pTotalInfo->Race][m_pTotalInfo->Gender];

	CEngineObject* const pFace = new CEngineObject;
	pFace->Init(
		0 == modList.ModFile ? "" : modList.ModFile[0],
		0,
		eEngineObjectType_Weapon);
	m_pEngineObject->AttachDress(
		pFace,
		"Bip01 Head");
	m_pPlayer->SetFaceObject(
		pFace);
	pFace->SetMaterialIndex(
		m_pTotalInfo->FaceType * 2 + 72);
	return TRUE;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: ChangePart
//	DESC		: ����� ���� �� Player�� �ٲ�� �κ� ó���� �Ѵ�.
//				: ChangePart�� ����� ��� Attach�� ������Ʈ�� ��� �����ѵ�
//				: ChangePart�� ����ѵ� �ٽ� Attach�� ����ؾ� �ϹǷ�
//				: Attach�� ChangePart�� ����� ��� ������ �߻��� ������ �ִ�.
//				: ���� ChangePart�� ����ؾ� �ϴ� ���� �������� ���� ó���ѵ�
//				: Attach�� ����ؾ� �ϴ� ������������ ó���Ѵ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: February 05, 2009
//-------------------------------------------------------------------------------------------------
BYTE CAppearPlayer::ChangePart() 
{
	// ĳ���� ��Ż ���� ��ȿ���� Ȯ���Ѵ�.
	if( !m_pTotalInfo ) goto ERREND ;


	// MOD ������ �޴´�. ( ���ҽ� �Ŵ������� mod ���� ����Ʈ�� ���� ��Ų ���� ���� �ʴٰ� ������ )
	MOD_LIST* pModList = &GAMERESRCMNGR->m_ModFileList[ m_pTotalInfo->Race ][ m_pTotalInfo->Gender ] ;
	if( !pModList ) goto ERREND ;

	ITEM_INFO* pInfo = NULL;
	WORD PartType = 0;
	WORD PartModelNum = 0;
	DWORD* WearedItemIdx = m_pTotalInfo->WearedItemIdx;
    ITEM_INFO* transmogInfo = NULL;
    DWORD ITEM_3D = 0;
    DWORD* WearedItemTransmog = m_pTotalInfo->WearedItemTransmog;
	char HideParts[eWearedItem_Max] = {0};
	EncodeHidePartsFlag(
		m_pTotalInfo->HideFlag,
		HideParts);

	// ChangePart�� ����ϴ� �κ�	
	for(int n=0 ;n < eWearedItem_Max ; ++n)
	{
		if (/*m_pPlayer->GetChangePart(n) && */WearedItemIdx[n] && !HideParts[n])
		{
			// ������ ������ �޴´�.
			pInfo = ITEMMGR->GetItemInfo(WearedItemIdx[n]) ;
			if( !pInfo ) continue ;

			// ��Ʈ ������ �޴´�.
			PartType		= pInfo->Part3DType ;
			PartModelNum	= pInfo->Part3DModelNum ;

			// 3D ��Ʈ ������ ������ �����ܸ� ǥ�õǴ� �༮���̴�.
			if( PartType == ePartType_None ) continue ;

			if( PartModelNum == 65535 ) continue ;


            
			if(transmogInfo = ITEMMGR->GetItemInfo(WearedItemTransmog[n])){
				ITEM_3D = transmogInfo->Part3DModelNum;
				PartType = transmogInfo->Part3DType;
			}
			else
				ITEM_3D = 0;



			// ��Ʈ Ÿ���� Ȯ���Ѵ�.
			switch( PartType )
			{
			case ePartType_Hair :
				{
					m_pEngineObject->ChangePart( eAppearPart_Hair, (ITEM_3D != 0 ? pModList->ModFile[ ITEM_3D ] : pModList->ModFile[ PartModelNum ]) ) ;
				}
				break ;

			case ePartType_Body :
			case ePartType_Costume_Body :
				{
					m_pEngineObject->ChangePart( eAppearPart_Body, (ITEM_3D != 0 ? pModList->ModFile[ ITEM_3D ] : pModList->ModFile[ PartModelNum ]) ) ;
				}
				break ;

			case ePartType_Hand :
			case ePartType_Costume_Hand :
				{
					m_pEngineObject->ChangePart( eAppearPart_Hand, (ITEM_3D != 0 ? pModList->ModFile[ ITEM_3D ] : pModList->ModFile[ PartModelNum ]) ) ;
				}
				break ;

			case ePartType_Foot :
			case ePartType_Costume_Foot :
				{
					m_pEngineObject->ChangePart( eAppearPart_Foot, (ITEM_3D != 0 ? pModList->ModFile[ ITEM_3D ] : pModList->ModFile[ PartModelNum ]) ) ;
				}
				break ;

			case ePartType_Head_Panda :
				{
					m_pEngineObject->ChangePart( eAppearPart_Hair, (ITEM_3D != 0 ? pModList->ModFile[ ITEM_3D ] : pModList->ModFile[ PartModelNum ]) ) ;

					m_stShowFlag.IsShowGlasses  = FALSE ;
					m_stShowFlag.IsShowHelmet	= FALSE ;
					m_stShowFlag.IsShowMask		= FALSE ;
				}
				break ;

			case ePartType_Head_Chicken :
				{
					m_pEngineObject->ChangePart( eAppearPart_Hair, (ITEM_3D != 0 ? pModList->ModFile[ ITEM_3D ] : pModList->ModFile[ PartModelNum ]) ) ;
					
					m_stShowFlag.IsShowGlasses  = TRUE ;
					m_stShowFlag.IsShowHelmet	= FALSE ;
					m_stShowFlag.IsShowMask		= TRUE ;
				}
				break ;

			case ePartType_Head_Tights :
				{
					m_pEngineObject->ChangePart( eAppearPart_Hair, (ITEM_3D != 0 ? pModList->ModFile[ ITEM_3D ] : pModList->ModFile[ PartModelNum ]) ) ;

					m_stShowFlag.IsShowGlasses  = TRUE ;
					m_stShowFlag.IsShowHelmet	= TRUE ;
					m_stShowFlag.IsShowMask		= TRUE ;
				}
				break ;

			case ePartType_Head_Bag :
				{
					m_pEngineObject->ChangePart( eAppearPart_Hair, (ITEM_3D != 0 ? pModList->ModFile[ ITEM_3D ] : pModList->ModFile[ PartModelNum ]) ) ;

					m_stShowFlag.IsShowGlasses  = FALSE ;
					m_stShowFlag.IsShowHelmet	= FALSE ;
					m_stShowFlag.IsShowMask		= TRUE ;
				}
				break ;

			case ePartType_Head_Mask :
				{
					m_stShowFlag.IsShowGlasses  = FALSE ;
					m_stShowFlag.IsShowHelmet	= TRUE ; 
					m_stShowFlag.IsShowMask		= FALSE ;
				}
				break ;

			//090506 pdy ���� �߰� 
			case ePartType_Head_CatEar:
				{
					m_stShowFlag.IsShowFace		= TRUE ;
					m_stShowFlag.IsShowGlasses  = TRUE ;
					m_stShowFlag.IsShowHelmet	= TRUE ;
					m_stShowFlag.IsShowMask		= TRUE ;
				}
				break;

			default : break ;

			}
		}
	}

	// ��� ����.
	return TRUE ;

	// ������ ���� ó���� �Ѵ�.
ERREND :
	InitData() ;
	return FALSE ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: AttachPart
//	DESC		: ����� ���� �� Player�� �ٿ��� �ϴ� ��Ʈ ó���� �Ѵ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: February 05, 2009
//-------------------------------------------------------------------------------------------------
BYTE CAppearPlayer::AttachPart() 
{
	// ĳ���� ��Ż ���� ��ȿ���� Ȯ���Ѵ�.
	if( !m_pTotalInfo ) goto ERREND ;


	// MOD ������ �޴´�. ( ���ҽ� �Ŵ������� mod ���� ����Ʈ�� ���� ��Ų ���� ���� �ʴٰ� ������ )
	MOD_LIST* pModList = &GAMERESRCMNGR->m_ModFileList[ m_pTotalInfo->Race ][ m_pTotalInfo->Gender ] ;
	if( !pModList ) goto ERREND ;

	ITEM_INFO* pInfo = NULL;
	WORD PartType = 0;
	WORD PartModelNum = 0;
	DWORD* WearedItemIdx = m_pTotalInfo->WearedItemIdx;


    ITEM_INFO* transmogInfo = NULL;
	DWORD ITEM_3D = 0;
	DWORD* WearedItemTransmog = m_pTotalInfo->WearedItemTransmog;

	char HideParts[eWearedItem_Max] = {0};
	EncodeHidePartsFlag(
		m_pTotalInfo->HideFlag,
		HideParts);

	// Attach�� ����ϴ� �κ�
	for( int n = 0 ; n < eWearedItem_Max ; ++n )
	{
		if(/*m_pPlayer->IsChangePart(n) &&*/ WearedItemIdx[ n ] && !HideParts[ n ])
		{
			pInfo = ITEMMGR->GetItemInfo(WearedItemIdx[ n ]) ;

			/// ������ ���� ����...
			if( !pInfo ) continue ;

			PartType	 = pInfo->Part3DType ;
			PartModelNum = pInfo->Part3DModelNum ;

			/// 3D ��Ʈ ������ ������ �����ܸ� ǥ�õǴ� �༮���̴�.
			if( PartType == ePartType_None ) continue ;

			if( PartModelNum == 65535 ) continue ;

            if(transmogInfo = ITEMMGR->GetItemInfo(WearedItemTransmog[n])){
				ITEM_3D = transmogInfo->Part3DModelNum;
				PartType = transmogInfo->Part3DType; 
			}
			else
				ITEM_3D = 0;

			switch( PartType )
			{
			case ePartType_OneHand :
			case ePartType_TwoHand :
			case ePartType_TwoBlade :
				{
					// 100310 ShinJS --- Ż�� ž�½� ������ �ʵ��� ��.
					if( m_pPlayer->IsGetOnVehicle() )
						break;

					if( m_pPlayer->GetState() != eObjectState_StreetStall_Owner )
					{
						if( !AppearWeapone() ) goto ERREND ;
					}
				}
				break ;

			case ePartType_Shield :
				{
					// 100310 ShinJS --- Ż�� ž�½� ������ �ʵ��� ��.
					if( m_pPlayer->IsGetOnVehicle() )
						break;

					CEngineObject* pShield = NULL ;
					pShield = new CEngineObject ;
					pShield->Init((ITEM_3D != 0 ? pModList->ModFile[ ITEM_3D ] : pModList->ModFile[ PartModelNum ]),NULL,eEngineObjectType_Weapon) ;
					m_pEngineObject->AttachWeapon(pShield,"Lhand") ;	
				}
				break ;

			case ePartType_Helmet :
				{
					if( m_stShowFlag.IsShowHelmet )
					{
						CEngineObject* pHelmet = NULL ;
						pHelmet = new CEngineObject ;
						pHelmet->Init((ITEM_3D != 0 ? pModList->ModFile[ ITEM_3D ] : pModList->ModFile[ PartModelNum ]),NULL,eEngineObjectType_Weapon) ;
						m_pEngineObject->AttachDress(pHelmet,"Bone_head") ;
					}
				}
				break ;

			case ePartType_Head_Panda :
				{
					m_stShowFlag.IsShowFace = FALSE ;
				}
				break ;

			case ePartType_Mask :
				{
					if( m_stShowFlag.IsShowMask )
					{
						CEngineObject* pMask = NULL ;
						pMask = new CEngineObject ;
						pMask->Init((ITEM_3D != 0 ? pModList->ModFile[ ITEM_3D ] : pModList->ModFile[ PartModelNum ]),NULL,eEngineObjectType_Weapon) ;
						m_pEngineObject->AttachDress(pMask,"Bone_head") ;
					}
				}
				break ;

			case ePartType_Glasses :
				{
					if( m_stShowFlag.IsShowGlasses )
					{
						CEngineObject* pGlasses = NULL ;
						pGlasses = new CEngineObject ;
					pGlasses->Init((ITEM_3D != 0 ? pModList->ModFile[ ITEM_3D ] : pModList->ModFile[ PartModelNum ]),NULL,eEngineObjectType_Weapon) ;
						m_pEngineObject->AttachDress(pGlasses,"Bone_head") ;	
					}
				}
				break ;

			case ePartType_Wing :
			case ePartType_Wing2 :
				{
					CEngineObject* pWing = new CEngineObject ;
					pWing->Init((ITEM_3D != 0 ? pModList->ModFile[ ITEM_3D ] : pModList->ModFile[ PartModelNum ]),NULL,eEngineObjectType_Weapon) ;
					m_pEngineObject->AttachDress(pWing,"Bip01 Spine1") ;

					// 091028 Ư�� ��ų���� ������� ���ϰų�, ���� �̵������ �������ʴ� ���� ���� 
					//if( !m_IsForEffect )
					//{
						m_pPlayer->SetWingObject( pWing ) ;
					//}
				}
				break ;
			//100406 pdy �����߰�
			case ePartType_Pelvis:
				{
					CEngineObject* pTail = new CEngineObject ;
					pTail->Init((ITEM_3D != 0 ? pModList->ModFile[ ITEM_3D ] : pModList->ModFile[ PartModelNum ]),NULL,eEngineObjectType_Weapon) ;
					m_pEngineObject->AttachDress(pTail,"Bip01 Pelvis") ;
				}
				break;

			//090506 pdy ���� �߰� 
			case ePartType_Tail:
				{ 
					CEngineObject* pTail = new CEngineObject ;
						pTail->Init((ITEM_3D != 0 ? pModList->ModFile[ ITEM_3D ] : pModList->ModFile[ PartModelNum ]),NULL,eEngineObjectType_Weapon) ;
					m_pEngineObject->AttachDress(pTail,"Bip01") ;
				}
				break ;

			//090506 pdy ���� �߰� 
			case ePartType_Head_CatEar :
				{
					CEngineObject* pEar = new CEngineObject ;
				pEar->Init((ITEM_3D != 0 ? pModList->ModFile[ ITEM_3D ] : pModList->ModFile[ PartModelNum ]),NULL,eEngineObjectType_Weapon) ;
					m_pEngineObject->AttachDress(pEar,"Bone_head") ;
				}
				break;

			case ePartType_Head_Mask :
				{
					CEngineObject* pMask = NULL ;
					pMask = new CEngineObject ;
					pMask->Init((ITEM_3D != 0 ? pModList->ModFile[ ITEM_3D ] : pModList->ModFile[ PartModelNum ]),NULL,eEngineObjectType_Weapon) ;
					m_pEngineObject->AttachDress(pMask,"Bone_head") ;	
				}
				break ;

			default : break ;
			} 
		}
	}

	if(0 == HideParts[eWearedItem_Costume_Head])
	{
		ITEM_INFO*	pHatInfo			=	NULL;
		ITEM_INFO*  pCustume_Head_Info	=	NULL;
		WORD	CustumeHeadPartsNum		=	NULL;	
		pHatInfo			=	ITEMMGR->GetItemInfo(WearedItemIdx[eWearedItem_Hat]);
		pCustume_Head_Info	=	ITEMMGR->GetItemInfo(WearedItemIdx[eWearedItem_Costume_Head]);
			
		if( pHatInfo != NULL && pCustume_Head_Info != NULL )
		{
			if( pHatInfo->Part3DType == ePartType_Head_Panda && pCustume_Head_Info->Part3DType != ePartType_Head_Panda )
			{
				CustumeHeadPartsNum	=	pCustume_Head_Info->Part3DModelNum;


				m_stShowFlag.IsShowFace		=	TRUE;
				m_stShowFlag.IsShowGlasses	=	TRUE;
				m_stShowFlag.IsShowMask		=	TRUE;
				m_stShowFlag.IsShowHelmet	=	FALSE;
			}
		}
	}

	// ���� ����Ʈ�� Ȱ��ȭ ��Ų��.
	m_pPlayer->StartJobEffect() ;

	
	// ��� ����.
	return TRUE ;

	// ������ ���� ó���� �Ѵ�.
ERREND :
	InitData() ;
	return FALSE ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: InitScale
//	DESC		: ����� ���� �� Player�� Scale ó���� �Ѵ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: February 05, 2009
//-------------------------------------------------------------------------------------------------
BYTE CAppearPlayer::InitScale() 
{
	// ĳ���� ��Ż ���� ��ȿ���� Ȯ���Ѵ�.
	if( !m_pTotalInfo ) goto ERREND ;


	//������
	VECTOR3 scale ;	

	if( m_pTotalInfo->Width > 3.0f ) m_pTotalInfo->Width = 3.0f ;
	else if( m_pTotalInfo->Width < 0.3f ) m_pTotalInfo->Width = 0.3f ;

	if( m_pTotalInfo->Height > 3.0f ) m_pTotalInfo->Height = 3.0f ;
	else if( m_pTotalInfo->Height < 0.3f ) m_pTotalInfo->Height = 0.3f ;

	SetVector3( &scale, m_pTotalInfo->Width, m_pTotalInfo->Height, m_pTotalInfo->Width) ;

	m_pEngineObject->SetScale(&scale) ;


	// ��� ����.
	return TRUE ;

	// ������ ���� ó���� �Ѵ�.
ERREND :
	InitData() ;
	return FALSE ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: InitPos
//	DESC		: ����� ���� �� Player�� ��ġ ó���� �Ѵ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: February 05, 2009
//-------------------------------------------------------------------------------------------------
BYTE CAppearPlayer::InitPos() 
{
	// ĳ���� ��Ż ���� ��ȿ���� Ȯ���Ѵ�.
	if( !m_pTotalInfo ) goto ERREND ;


	m_pEngineObject->ApplyHeightField( TRUE ) ;

	MOVE_INFO* moveInfo = m_pPlayer->GetBaseMoveInfo() ;
	if( !moveInfo ) goto ERREND ;

	ROTATEINFO* rotateInfo = m_pPlayer->GetBaseRotateInfo() ;
	if( !rotateInfo ) goto ERREND ;


	m_pPlayer->SetPosition( &moveInfo->CurPosition ) ;

	if(HOUSINGMGR->IsHousingMap())
		m_pPlayer->SetAngle( HOUSINGMGR->GetStartAngle() ) ;
	else
		m_pPlayer->SetAngle( rotateInfo->Angle.ToRad() ) ;

	// ��� ����.
	return TRUE ;

	// ������ ���� ó���� �Ѵ�.
ERREND :
	InitData() ;
	return FALSE ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: AppearWeapone
//	DESC		: ����� ���� �� Player�� ���� ǥ�� ó���� �Ѵ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: February 05, 2009
//-------------------------------------------------------------------------------------------------
BYTE CAppearPlayer::AppearWeapone() 
{
	// 100310 ShinJS --- Ż�� ž�½� ������ �ʵ��� ��.
	if( m_pPlayer->IsGetOnVehicle() )
		return TRUE;

	// ĳ���� ��Ż ���� ��ȿ���� Ȯ���Ѵ�.
	if( !m_pTotalInfo ) goto ERREND ;

	MOD_LIST& pModList = GAMERESRCMNGR->m_ModFileList[ m_pTotalInfo->Race ][ m_pTotalInfo->Gender ] ;
	
	APPEARANCEMGR->HideWeapon(m_pPlayer,m_pEngineObject) ;

	if(const ITEM_INFO* const itemInfo = ITEMMGR->GetItemInfo(m_pPlayer->GetWearedItemIdx(eWearedItem_Weapon)))
	{
        WORD ITEM_3D = 0;

		if(const ITEM_INFO* const transmogInfo = ITEMMGR->GetItemInfo(m_pPlayer->GetWearedItemTransmog(eWearedItem_Weapon)))
			ITEM_3D = transmogInfo->Part3DModelNum;
		else
			ITEM_3D = 0;
            
		switch(itemInfo->Part3DType)
		{
		case ePartType_TwoBlade:
			{
				if(eWeaponType_BothDagger == itemInfo->WeaponType)
				{
					CEngineObject* pRightWeapon = new CEngineObject ;
					pRightWeapon->Init(
						(ITEM_3D != 0 ? pModList.ModFile[ ITEM_3D ] : pModList.ModFile[ itemInfo->Part3DModelNum ]),
						NULL,
						eEngineObjectType_Weapon);
					m_pEngineObject->AttachWeapon(
						pRightWeapon,
						RIGHT_HAND);

					// ������ �޼չ��⸦ modlist���Ͽ��� �����Ͽ� attach��Ų��.
					if(const char* pLModFile = GAMERESRCMNGR->GetDualWeaponLObject((ITEM_3D != 0 ? ITEM_3D : itemInfo->Part3DModelNum), m_pTotalInfo->Race, m_pTotalInfo->Gender))
					{
						CEngineObject* pLeftWeapon = new CEngineObject ;
						pLeftWeapon->Init(
							LPTSTR(pLModFile),
							NULL,
							eEngineObjectType_Weapon) ;
						m_pEngineObject->AttachWeapon(
							pLeftWeapon,
							LEFT_HAND) ;
					}
				}

				break;
			}
		default:
			{
				CEngineObject* pWeapon = new CEngineObject ;
				pWeapon->Init(
					(ITEM_3D != 0 ? pModList.ModFile[ ITEM_3D ] : pModList.ModFile[ itemInfo->Part3DModelNum ]),
					NULL,
					eEngineObjectType_Weapon);

				m_pEngineObject->AttachWeapon(
					pWeapon,
					eWeaponType_Bow == itemInfo->WeaponType ? LEFT_HAND : RIGHT_HAND);

				break;
			}
		}
	}

	if(const ITEM_INFO* const itemInfo = ITEMMGR->GetItemInfo(m_pPlayer->GetWearedItemIdx(eWearedItem_Shield)))
	{
        WORD ITEM_3D = 0;

		if( itemInfo->WeaponAnimation == eWeaponAnimationType_OneHand )
		{
			if(const ITEM_INFO* const transmogInfo = ITEMMGR->GetItemInfo(m_pPlayer->GetWearedItemTransmog(eWearedItem_Shield)))
				ITEM_3D = transmogInfo->Part3DModelNum;
			else
				ITEM_3D = 0;
		}
		else
		{
			if(const ITEM_INFO* const transmogInfo = ITEMMGR->GetItemInfo(m_pPlayer->GetWearedItemTransmog(eWearedItem_Shield)))
				ITEM_3D = transmogInfo->Part3DModelNum;
			else
				ITEM_3D = 0;
		}

		CEngineObject* pShield	= new CEngineObject ;

		pShield->Init(
			(ITEM_3D != 0 ? pModList.ModFile[ ITEM_3D ] : pModList.ModFile[ itemInfo->Part3DModelNum ]),
			NULL,
			eEngineObjectType_Weapon);
		m_pEngineObject->AttachWeapon(
			pShield,
			LEFT_HAND);
	}

	if(m_pTotalInfo->WeaponEnchant >= 3)
	{
		BYTE level = m_pTotalInfo->WeaponEnchant - 3;
        DWORD effectItemIdx = m_pTotalInfo->WearedItemIdx[eWearedItem_Weapon];
		if(m_pTotalInfo->WearedItemTransmog[eWearedItem_Weapon])
		{
			effectItemIdx = m_pTotalInfo->WearedItemTransmog[eWearedItem_Weapon];
		}
        
		if(EnchantEffect* pEffect = ITEMMGR->GetItemEffect(effectItemIdx))
		{
			for( int i = 0; i < 3; i++ )
			{
				if( pEffect->Effect[ level ][ i ] == 0 ) continue ;

				TARGETSET set ;
				set.pTarget = m_pPlayer ;
				m_pPlayer->mWeaponEffect[ i ] = EFFECTMGR->StartEffectProcess( pEffect->Effect[ level ][ i ], m_pPlayer, &set, 0, m_pPlayer->GetID() ) ;
			}
		}
	}

	// ��� ����.
	return TRUE ;

	// ������ ���� ó���� �Ѵ�.
ERREND :
	InitData() ;
	return FALSE ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: InitData
//	DESC		: ��� ���� ó���� �ʿ�� �� �������� �ʱ�ȭ �ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: February 05, 2009
//-------------------------------------------------------------------------------------------------
void CAppearPlayer::InitData()
{
	InitShowFlag() ;

	m_IsForEffect		= FALSE ;

	m_pPlayer			= NULL ;
	m_pEngineObject		= NULL ;

	m_pTotalInfo		= NULL ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: InitShowFlag
//	DESC		: ����� �����ִ� �ʱ�ȭ �÷��׸� �ʱ�ȭ �ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: February 05, 2009
//-------------------------------------------------------------------------------------------------
void CAppearPlayer::InitShowFlag()
{
	memset( &m_stShowFlag, TRUE, sizeof(m_stShowFlag) ) ;
}
