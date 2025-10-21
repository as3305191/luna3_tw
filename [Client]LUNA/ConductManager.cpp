#include "StdAfx.h"
#include "conductmanager.h"
#include "MHFile.h"
#include "ConductInfo.h"
#include "conductbase.h"
#include "Interface/cWindowManager.h"
#include "Interface/cScriptManager.h"
#include "WindowIDEnum.h"
#include "GameIn.h"
#include "cSkillTreeDlg.h"
#include "cJobSkillDlg.h"
#include "interface/cIconGridDialog.h"
#include "ChatManager.h"
#include "ObjectManager.h"
#include "ExchangeManager.h"
#include "StreetStallManager.h"
#include "ShowdownManager.h"
#include "PartyManager.h"
#include "MoveManager.h"
#include "AppearanceManager.h"
#include "input/UserInput.h"
#include "ObjectStateManager.h"
#include "Object.h"
#include "../hseos/Farm/SHFarmManager.h"
#include "PKManager.h"
#include "AutoNoteManager.h"
#include "StoreSearchDlg.h"
#include "FishingDialog.h"
#include "FishingPointDialog.h"
#include "FishingManager.h"
#include "MHMap.h"
#include "CookDlg.h"
#include "cHousingMgr.h"

cConductManager::cConductManager(void)
{
	m_pConductInfo = NULL;

	InitInfoTable();
}

cConductManager::~cConductManager(void)
{
	ReleaseInfoTable() ;
}

void cConductManager::InitInfoTable()
{
	m_ConductInfoTable.Initialize(MAX_CONDUCT_COUNT) ;

	LoadConductInfoList() ;

	// 070528 LYW --- ConductManager : Load emoticon list.
	LoadEmoticonList() ;
}

void cConductManager::ReleaseInfoTable()
{
	m_ConductInfoTable.SetPositionHead() ;

	for(cConductInfo* info = m_ConductInfoTable.GetData();
		0 < info;
		info = m_ConductInfoTable.GetData())
	{
		SAFE_DELETE(
			info);
	}

	m_ConductInfoTable.RemoveAll() ;

	// 070528 LYW --- ConductManager : Add emoticon part.
	EMOTICON* pEmoticon = NULL ;

	PTRLISTPOS pos = m_EmoticonList.GetHeadPosition() ;

	while(pos)
	{
		pEmoticon = (EMOTICON*)m_EmoticonList.GetNext(pos) ;

		if( pEmoticon )
		{
			delete pEmoticon ;
			pEmoticon = NULL ;
		}
	}

	m_EmoticonList.RemoveAll() ;
}

void cConductManager::LoadConductInfoList()
{
	CMHFile	file ;

    char filename[256];
    sprintf(filename, "System/Resource/%s", GAMERESRCMNGR->GetTranslationPath("ConductList.bin")); 
    
	file.Init( filename, "rb" ) ;

	if( file.IsInited() == FALSE )	return ; // ���� ASSERT ó�� �� ��.

	while(1)
	{
		if( file.IsEOF() != FALSE ) break ;

		cConductInfo* pCInfo = new cConductInfo ;
		pCInfo->InitConductInfo(&file) ;

		ASSERT( m_ConductInfoTable.GetData( pCInfo->GetConductIdx() ) == NULL ) ;
		m_ConductInfoTable.Add( pCInfo, pCInfo->GetConductIdx() ) ;
	}

	file.Release() ;
}

// 070528 LYW --- ConductManager : Add function to load emoticon list.
void cConductManager::LoadEmoticonList()
{
	CMHFile	file ;

	file.Init( EMOTICON_FILE_NAME, "rb" ) ;

	if( file.IsInited() == FALSE )	return ; // ���� ASSERT ó�� �� ��.

	while(1)
	{
		if( file.IsEOF() != FALSE ) break ;

		EMOTICON* pEmoticon = new EMOTICON ;
		
		pEmoticon->index = file.GetWord() ;
		strcpy(pEmoticon->command, file.GetString()) ;
		pEmoticon->effectNum = file.GetWord() ;

		m_EmoticonList.AddTail(pEmoticon) ;		
	}

	file.Release() ;
}

cConductInfo* cConductManager::GetConductInfo(DWORD conductIdx)
{
	if( conductIdx == 0 )
	{
		YHTPOSITION pos = m_ConductInfoTable.GetPositionHead() ;

		return m_ConductInfoTable.GetDataPos( &pos ) ;
	}
	return (cConductInfo*)m_ConductInfoTable.GetData(conductIdx);
}

void cConductManager::RegistConduct()
{
	cSkillTreeDlg* const pSkillTreeDlg = GAMEIN->GetSkillTreeDlg();
	
	m_ConductInfoTable.SetPositionHead() ;

	for(cConductInfo* info = m_ConductInfoTable.GetData();
		0 < info;
		info = m_ConductInfoTable.GetData())
	{
		cJobSkillDlg* const pJobSkillDlg = (cJobSkillDlg*)pSkillTreeDlg->GetTabSheet(
			BYTE(info->GetConductKind()));
		cIconGridDialog* const pSkillDlg = (cIconGridDialog*)pJobSkillDlg->GetTabSheet(
			0);
		pSkillDlg->AddIcon(
			info->GetConductPos(),
			NewConductIcon(BYTE(info->GetConductIdx())));
	}
}

// 080203 LYW --- ConductManager : �ൿ���� ������ �߰� ó���� ������.
cConductBase* cConductManager::NewConductIcon( BYTE bIndex )
{
	cSkillTreeDlg* pDlg = NULL ;
	pDlg = GAMEIN->GetSkillTreeDlg() ;															// ��ų Ʈ�� ���̾�α׸� �޴´�.

	if( !pDlg )																					// ���̾�α� üũ.
	{
		#ifdef _DEBUG
		MessageBox( NULL, "Failed to receive skill tree dialog.", "NewConductIcon", MB_OK ) ;
		#endif
		return NULL ;															
	}

	CIndexGenerator* pIdxGenerator = NULL ;
	pIdxGenerator = pDlg->GetIndexGenerator() ;													// �ε��� �����⸦ �޴´�.

	if( !pIdxGenerator )																		// �ε��� ������ üũ.
	{
		#ifdef _DEBUG
		MessageBox( NULL, "Failed to receive index generator.", "NewConductIcon", MB_OK ) ;
		#endif
		return NULL ;																
	}

	WORD	wIconSize	= DEFAULT_ICONSIZE ;													// ������ ����� �޴´�.
	DWORD	dwIndex		= pIdxGenerator->GenerateIndex() ;										// �ε����� �Ҵ�޴´�.
	DWORD	dwMaxIndex	= IG_CONDUCT_STARTINDEX + MAX_CONDUCTICON_NUM ;							// �ִ� �ε����� �޴´�.

	if( dwIndex < IG_CONDUCT_STARTINDEX || dwIndex >= dwMaxIndex )								// �ε����� üũ.
	{
		#ifdef _DEBUG
		MessageBox( NULL, "Invalid icon index", "NewConductIcon", MB_OK ) ;
		#endif
		return NULL ;
	}

	cConductInfo* pConductInfo = NULL ;
	pConductInfo = CONDUCTMGR->GetConductInfo((WORD)bIndex) ;									// ������ ������ �޴´�.

	if( !pConductInfo )																			// ������ ���� üũ.
	{
		#ifdef _DEBUG
		MessageBox( NULL, "Failed to receive conduct info.", "NewConductIcon", MB_OK ) ;
		#endif
		return NULL ;
	}

	cImage LowImage ;
	cImage HighImage ;																			// ������ �̹��� ����.

	GetIconImage(bIndex, &LowImage, &HighImage) ;												// �̹����� �޴´�.

	cConductBase* icon	= new cConductBase ;													// �⺻ ������ �����Ѵ�.

	icon->Init(0,0, wIconSize, wIconSize, &LowImage, &HighImage, dwIndex) ;						// �������� ó���Ѵ�.
	icon->SetData((DWORD)bIndex) ;																// �ε��� ������ �����Ѵ�.

	icon->m_pConductInfo = pConductInfo ;														// �ൿ ���� ������ �����Ѵ�.

	SetToolTipIcon( icon ) ;																	// ������ �����Ѵ�.

	WINDOWMGR->AddWindow(icon) ;																// ������ �Ŵ����� �������� �߰��Ѵ�.

	return icon ;																				// �������� �����Ѵ�.
}

void cConductManager::GetIconImage(DWORD Conductldx, cImage * pLowImage , cImage * pHighImage)
{
	cConductInfo* pConductInfo = CONDUCTMGR->GetConductInfo(Conductldx);
	if(pConductInfo == NULL)
	{
		return;
	}

	/*if( pConductInfo->GetLowImage() != -1 )
	SCRIPTMGR->GetImage( pConductInfo->GetLowImage(), pLowImage, PFT_CONDUCTPATH );*/
	if( pConductInfo->GetHighImage() != -1 )
	SCRIPTMGR->GetImage( pConductInfo->GetHighImage(), pHighImage, PFT_CONDUCTPATH );
}

void cConductManager::SetToolTipIcon(cConductBase *pConduct)
{
	cConductInfo* pCommConduct = pConduct->m_pConductInfo;
	if(!pCommConduct)	return;

	cImage imgToolTip;
	SCRIPTMGR->GetImage( 0, &imgToolTip, PFT_HARDPATH );
	pConduct->SetToolTip( "", RGBA_MAKE(255, 255, 255, 255), &imgToolTip, TTCLR_ITEM_CANEQUIP );

	char line[64];

	wsprintf(line, "[%s]", pCommConduct->GetConductName() );

	pConduct->AddToolTipLine( line, TTTC_SKILLNAME );
}

// 070220 LYW --- Add function to use conduct icon.
BOOL cConductManager::OnConductCommand( DWORD idx )
{
	// 070620 LYW --- ConductManager : Check die.
	if( HERO->IsDied() ) return FALSE ;

	// 070509 LYW --- ConductManager : Check running emotion.
	if(HERO->IsRunningEmotion()) return FALSE ;

	cConductInfo* pConductInfo = NULL ;
	if( idx == 0 )
	{
		YHTPOSITION pos = m_ConductInfoTable.GetPositionHead() ;

		pConductInfo = m_ConductInfoTable.GetDataPos( &pos ) ;
	}
	else
	{
		pConductInfo = m_ConductInfoTable.GetData( idx );
	}

	if( !pConductInfo )
	{
		return FALSE;
	}

	// 091022 ShinJS --- Ż�� ž�½� ����Ҽ� ���� Action �� ���
	if( pConductInfo->IsInvalidOnVehicle() && 
		HERO->IsGetOnVehicle() )
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1839 ) );
		return FALSE;
	}

	// 090911 ShinJS --- WayPoint Clear
	HERO->ClearWayPoint();

	WORD conductKind = pConductInfo->GetConductKind();

	switch( conductKind )
	{
	case eSkillDlgTab_Action :
		{
			LifeSkillCommand( pConductInfo->GetConductIdx() ) ;
		}
		break ;

	case eSkillDlgTab_Motion :
		{
			// desc_hseos_����ý���_01
			// S ����ý��� �߰� added by hseos 2007.09.18
			// ..���� ��� ���� �� ����
			if (g_csFarmManager.CLI_GetIngCharMotion())	return FALSE;

			if ( eObjectState_HouseRiding == HERO->GetState() )
			{
				//090527 pdy �Ͽ�¡ �ý��۸޼��� ž�»��½� �����ൿ [���]
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1876 ) );	//1876	"ž�� ���¸� ������ �� ����ϼž� �մϴ�."
				return FALSE;
			}
			// E ����ý��� �߰� added by hseos 2007.09.18
			ExpressEmotion( pConductInfo->GetConductIdx() ) ;

			// 080205 LYW --- ConductManager : ����� �����Ѵ�.
			HERO->SetRunningEmotion(TRUE) ;
		}
		break ;

	// 070425 LYW --- ConductManager : Add emoticon part.
	case eSkillDlgTab_Emotiocon :
		{
			EmoticonCommand( pConductInfo->GetConductIdx() ) ;

			// 080205 LYW --- ConductManager : ����� �����Ѵ�.
			HERO->SetRunningEmotion(TRUE) ;
		}
		break ;
	}

	m_pConductInfo = pConductInfo ;

	// 080205 LYW --- ConductManager : ���� ���� �� �������� ����ϵ���, ���� ó��.
	//HERO->SetRunningEmotion(TRUE) ;
	return TRUE ;
}

// 070220 LYW --- ConductManager : Add function to use life skill.
void cConductManager::LifeSkillCommand( DWORD idx )
{
	if ( eObjectState_HouseRiding == HERO->GetState() )
	{
		//090527 pdy �Ͽ�¡ �ý��۸޼��� ž�»��½� ���ѻ��� [�Ϲݾ׼�]
		//������ ž�½ÿ� �Ϲݾ׼��� �����ѵȴ�
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1876 ) );	//1876	"ž�� ���¸� ������ �� ����ϼž� �մϴ�."
		return;
	}

 	switch( idx )
	{
	case 0 :	// ����
		{
			g_UserInput.GetHeroMove()->AttackSelectedObject() ;
		}
		break ;

	case 1 :	//�ɱ�
		{
			// desc_hseos_����ý���_01
			// S ����ý��� �߰� added by hseos 2007.09.20
			if (g_csFarmManager.CLI_GetIngCharMotion()) return;
			// E ����ý��� �߰� added by hseos 2007.09.20

			// 070502 LYW --- ConductManager : Send rest infor to server.

			CHero* hero = OBJECTMGR->GetHero();
			ASSERT( hero );

			EObjectState IsRestMode = OBJECTSTATEMGR->GetObjectState(hero);
			BOOL bReady = FALSE;

			if( IsRestMode == eObjectState_None || IsRestMode == eObjectState_Move || IsRestMode == eObjectState_Rest )
			{
				bReady = TRUE ;
			}

			if( !bReady )
			{
				return ;
			}

			if( IsRestMode == eObjectState_Rest )
			{
//				MOVEMGR->HeroMoveStop();
				OBJECTSTATEMGR->StartObjectState(hero, eObjectState_None);

				IsRestMode = eObjectState_None;
			}
			else
			{
				MOVEMGR->HeroMoveStop();
				OBJECTSTATEMGR->StartObjectState(hero, eObjectState_Rest);

				IsRestMode = eObjectState_Rest;
			}

			MSG_BYTE msg;
			msg.Category = MP_EMOTION;
			msg.Protocol = MP_EMOTION_SITSTAND_SYN;
			msg.dwObjectID = gHeroID;
			msg.bData = BYTE(IsRestMode);
			NETWORK->Send(&msg, sizeof(MSG_BYTE));
		}
		break ;

	case 2 :	//090204 LYW --- ConductManager : 2�� ����� [ ��Ƽ ���� ] ����̴�.
		{
			//STREETSTALLMGR->ChangeDialogState();
			GAMEIN->GetPartyDialog()->SetActive(TRUE);
		}
		break ;

	case 3 :	//�ŷ�
		{
			
			//090527 pdy �Ͽ�¡ �ý��۸޼��� �ٹ̱� ����Ͻ� ���ѻ��� [�ŷ�]
			if( HERO->GetState() == eObjectState_Housing  )
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg( 1874 ) );	//1874	"�ٹ̱� ��忡�� ����Ͻ� �� �����ϴ�."
				return;
			}

			//SHOWDOWNMGR->ApplyShowdown() ;
			EXCHANGEMGR->ApplyExchange() ;
		}
		break ;

	case 4 :	//����
		{
			// desc_hseos_����ý���_01
			// S ����ý��� �߰� added by hseos 2007.09.20
			if (g_csFarmManager.GetFarmZoneID(MAP->GetMapNum(), NULL)) 
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1792 ) );
				return;
			}

			if (g_csFarmManager.CLI_GetIngCharMotion()) return;
			// E ����ý��� �߰� added by hseos 2007.09.20
			//GAMEIN->GetPartyDialog()->SetActive(TRUE);

			// 080429 NYJ --- ������ ���Ұ�
			if(GAMEIN->GetFishingDlg()->IsPushedStartBtn())
			{
				CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1082 ) );
				return;
			}
			// 080429 NYJ --- ��������Ʈ��ȯ�� ���Ұ�
			else if(GAMEIN->GetFishingPointDlg()->IsActive() )
			{
				CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1082 ) );
				return;
			}

			if( HOUSINGMGR->IsHousingMap() )
			{
				//090527 pdy �Ͽ�¡ �ý��۸޼��� �Ͽ�¡ ���϶� �����ൿ [����]
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1867 ) );	//1867	"�� ���������� ����Ͻ� �� �����ϴ�."
				return;
			}

			STREETSTALLMGR->ChangeDialogState();
		}
		break ;

	case 5 : // ����
		{
			if ( HOUSINGMGR->IsHousingMap() )
			{
				//090527 pdy �Ͽ�¡ �ý��۸޼��� �Ͽ�¡ ���϶� �����ൿ [����]
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1867 ) );	//1867	"�� ���������� ����Ͻ� �� �����ϴ�."
				return;
			}

			SHOWDOWNMGR->ApplyShowdown() ;
		}
		break ;

	case 52 :	// ����
		{
			cDialog* dialog = WINDOWMGR->GetWindowForID( ITMD_MIXDLG );
			ASSERT( dialog );

			dialog->SetActive( ! dialog->IsActive() );
		}
		break;

	case 53 :	// ����
		{
			cDialog* dialog = WINDOWMGR->GetWindowForID( DIS_DISSOLUTIONDLG );
			ASSERT( dialog );

			dialog->SetActive( ! dialog->IsActive() );
		}
		break;

	case 54:		// ��ȭ
		{
			cDialog* dialog = WINDOWMGR->GetWindowForID( ITR_REINFORCEDLG );
			ASSERT( dialog );

			dialog->SetActive( ! dialog->IsActive() );
		}
		break;
	case 55:		// ��æƮ
		{
			cDialog* dialog = WINDOWMGR->GetWindowForID( ITD_UPGRADEDLG	 );
			ASSERT( dialog );

			dialog->SetActive( ! dialog->IsActive() );
		}
		break;

//---KES AUTONOTE
	case 59:
		{
			AUTONOTEMGR->ToggleAutoNote();
		}
		break;
//---------------

	case 64:
		{
			PKMGR->ToggleHeroPKMode();
		}
		break;

//---KES �����˻�
	case 65:
		{
			if( GAMEIN->GetStoreSearchDlg()->IsActive() )
				GAMEIN->GetStoreSearchDlg()->SetActive( FALSE );
			else
				GAMEIN->GetStoreSearchDlg()->SetActive( TRUE );
		}
		break;
//---------------

//---KES ���󰡱�
	case 66:
		{
			CObject* pObject = OBJECTMGR->GetSelectedObject();
			if( pObject )
			if( pObject->GetObjectKind() == eObjectKind_Player )
			{
				if( HERO )
					HERO->SetFollowPlayer( pObject->GetID() );
			}
		}
		break;
//---------------

// 080410 NYJ --- ����
	case 67:
		{	
			if(!HERO)
				break;

			CFishingDialog* pDlg = NULL ;
			pDlg = GAMEIN->GetFishingDlg();

			if( !pDlg) return ;

			if(!pDlg->IsActive())						// ����â�� ���������� â����
				pDlg->SetActive(TRUE) ;
		}
		break;

	case 68:
		{
			if(!HERO)
				break;

			if( GAMEIN->GetCookDlg()->IsActive() )
				GAMEIN->GetCookDlg()->SetActive( FALSE );
			else
				GAMEIN->GetCookDlg()->SetActive( TRUE );
		}
		break;
	 case 74:
		{
			cDialog* dialog = WINDOWMGR->GetWindowForID( ITTD_TRANSMOGDLG );
			ASSERT( dialog );

			dialog->SetActive( ! dialog->IsActive() );
		}
		break;
	}
}

// 070220 LYW --- ConductManager : Add function to use express emotion.
void cConductManager::ExpressEmotion( DWORD idx )
{
	switch( HERO->GetState() )
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
		return;
	}

	// 090422 ShinJS --- Ż�Ϳ� ž�����ΰ�� �������� �ʴ´�
	if( HERO->IsGetOnVehicle() )
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1839 ) );
		return;
	}

	cConductInfo* pCInfo = m_ConductInfoTable.GetData(idx) ;

	if(0 == pCInfo)
	{
		return;
	}

	APPEARANCEMGR->HideWeapon(HERO);
	MOVEMGR->HeroMoveStop();
	//HERO->ChangeEmotion( (int)motionNum ) ;

#ifdef _TESTCLIENT_
	HERO->GetEngineObject()->ChangeMotion(
		pCInfo->GetMotionIdx(),
		FALSE);
#else
	SendToServer(
		pCInfo->GetMotionIdx());
#endif
}	

// 070425 LYW --- ConductManager : Add function to use emoticon.
void cConductManager::EmoticonCommand( DWORD idx )
{
	int emoticonNum = -1 ;

	PTRLISTPOS pos = m_EmoticonList.GetHeadPosition();
	while(pos)
	{
		EMOTICON* pEmoticon = (EMOTICON*)m_EmoticonList.GetNext(pos);
		if (idx == pEmoticon->index)
		{
			emoticonNum = pEmoticon->effectNum;
			break;
		}
	}
	if( emoticonNum > -1 )
	{
		HERO->PlayEmoticon_Syn(
			BYTE(emoticonNum));
	}
}

void cConductManager::PetEmoticonCommand( DWORD idx )
{
	int emoticonNum = -1 ;

	PTRLISTPOS pos = m_EmoticonList.GetHeadPosition();
	while(pos)
	{
		EMOTICON* pEmoticon = (EMOTICON*)m_EmoticonList.GetNext(pos);
		if (idx == pEmoticon->index)
		{
			emoticonNum = pEmoticon->effectNum;
			break;
		}
	}
	if( emoticonNum > -1 )
	{
		TARGETSET set;
		set.pTarget = HEROPET;
			
		EFFECTMGR->StartEffectProcess(emoticonNum,HEROPET,&set,0,HEROPET->GetID());
	}
}

// 070228 LYW --- ConductManager : Add function to send message to server.
void cConductManager::SendToServer( int motionNum )
{
	MSG_INT data ;

	data.Category	= MP_EMOTION ;
	data.Protocol	= MP_EMOTION_NORMAL ;
	data.dwObjectID	= HEROID ;
	data.nData		= motionNum ;
	
	NETWORK->Send(&data, sizeof(MSG_INT));
}

// 070228 LYW --- ConductManager : Add function to parsing message.
void cConductManager::NetworkMsgParse( BYTE Protocol, void* pMsg )
{
	switch( Protocol )
	{
	case MP_EMOTION_NORMAL :
		{
			MSG_INT* data = ( MSG_INT* )pMsg ;

			CPlayer* pPlayer = ( CPlayer* )OBJECTMGR->GetObject( data->dwObjectID ) ;

			if( !pPlayer ) return ;

			// 070619 LYW --- ConductManager : Stop Player.
			MOVEMGR->MoveStop((CObject*)pPlayer, &(pPlayer->GetCurPosition())) ;

			// 070503 LYW --- ConductManager : Check emotion kind.
			APPEARANCEMGR->HideWeapon( pPlayer ) ;
			pPlayer->SetRunningEmotion(TRUE) ;

			pPlayer->ChangeEmotion( data->nData ) ;
		}
		break ;

	// 070502 LYW --- ConductManager : Add message to setting rest mode part.
	case MP_EMOTION_SITSTAND_ACK :
		{
			const MSG_BYTE* pmsg = ( MSG_BYTE* )pMsg ;

			if( gHeroID != pmsg->dwObjectID )
			{
				OBJECTSTATEMGR->StartObjectState(
					OBJECTMGR->GetObject(pmsg->dwObjectID),
					EObjectState(pmsg->bData));
			}
		}
		break ;

	// 070502 LYW --- ConductManager : Add message to setting ing motion.
	case MP_EMOTION_ING_MOTION_ACK :
		{
			MSG_INT* pmsg = ( MSG_INT* )pMsg ;

			if( gHeroID != pmsg->dwObjectID )
			{
				CPlayer* pPlayer = ( CPlayer* )OBJECTMGR->GetObject( pmsg->dwObjectID ) ;

				if( !pPlayer ) return ;
			}
		}
		break ;

	case MP_PLAY_EMOTICON_ACK :
		{
			MSG_INT* data = ( MSG_INT* )pMsg ;

			CPlayer* pPlayer = ( CPlayer* )OBJECTMGR->GetObject( data->dwObjectID ) ;

			if( !pPlayer ) return ;

			pPlayer->PlayEmoticon(data->nData) ;
		}
		break ;
	}
}
