#include "stdafx.h"
#include "GameIn.h"
//#ifdef _CHEATENABLE_
#include "CheatMsgParser.h"
#include "CharacterDialog.h"
#include "MainGame.h"
#include "WindowIdEnum.h"
#include "./Interface/cWindowManager.h"
#include "BattleSystem_client.h"
#include "TutorialDlg.h"
#include "ConductManager.h"
#include "ObjectManager.h"
#include "ItemManager.h"
#include "MoveManager.h"
#include "MHTimeManager.h"
#include "QuickManager.h"
#include "PartyManager.h"
#include "ObjectActionManager.h"
#include "GuildManager.h"
#include "StreetStallManager.h"
#include "StorageManager.h"
#include "FriendManager.h"
#include "NoteManager.h"
#include "ChatManager.h"

// 070228 LYW --- GameIn : Include header file of facial manager.
#include "FacialManager.h"

//050630SW
#include "cChatTipManager.h"
//-
#include "ExchangeManager.h"
#include "NpcScriptManager.h"
#include "./Audio/MHAudioManager.h"
#include "OptionManager.h"
#include "MHCamera.h"
#include "./Input/UserInput.h"
#include "MouseCursor.h"
#include "MiniMapDlg.h"
#include "PartyIconManager.h"
#include "mhMap.h"
#include "cMsgBox.h"

#include "MotionManager.h"
#include "objectstatemanager.h"
#include "CharacterCalcManager.h"
#include "PKManager.h"
#include "GMNotifyManager.h"
#include "ReinforceDlg.h"
#include "NpcScriptDialog.h"
//#include "MixManager.h"
#include "StatusIconDlg.h"
#include "PetStatusIconDialog.h"
#include "CharSelect.h"

#include "Showdownmanager.h"

#include "UserInfoManager.h"
#include "ExitManager.h"
#include "HackCheck.h"
#ifdef _GMTOOL_
#include "GMToolManager.h"
#endif

#include "Quest.h"
#include "QuestManager.h"

#include "MapChange.h"
#include "GuildFieldWar.h"
#include "ItemShopDialog.h"
#include "StorageDialog.h"
#include "GuildTournamentMgr.h"
#include "GuildUnion.h"
#include "GuageDialog.h"
#include "MainSystemDlg.h"
#include "cResourceManager.h"
#include "cScriptManager.h"
#include "Interface/GameNotifyManager.h"
#include "DateMatchingDlg.h"
#include "AutoNoteManager.h"
#include "./ChatRoomMgr.h"
#include "FishingManager.h"
#include "cSkillTreeManager.h"
#include "../[cc]skill/client/manager/skillmanager.h"
#include "cSkillTrainingDlg.h"
#include "FamilyDialog.h"
#include "NpcRecallMgr.h"
#include "DealDialog.h"
#include "PCRoomManager.h"
#include "AppearanceManager.h"

#ifdef _TESTCLIENT_
#include "TSToolManager.h"
#include "AppearanceManager.h"

void InitTestCharacters(void);
void InitTestHero();
void InitTestMonster();
#endif

#include "../hseos/SHMain.h"
#include "../hseos/Farm/SHFarmManager.h"
#include "../hseos/Family/SHFamilyManager.h"
#include "../hseos/Debug/SHDebug.h"
#include "../hseos/Farm/SHFarmBuyDlg.h"
#include "../hseos/Farm/SHFarmUpgradeDlg.h"
#include "../hseos/Farm/SHFarmManageDlg.h"
#include "TutorialManager.h"
#include "cMapMoveDialog.h"
#include "./Interface/cNumberPadDialog.h"
#include "WeatherManager.h"
#include "../hseos/Date/SHDateZoneListDlg.h"
#include "../hseos/Date/SHChallengeZoneListDlg.h"
#include "../hseos/Date/SHDateManager.h"
#include "../hseos/Date/SHChallengeZoneClearNo1Dlg.h"
#include "PetManager.h"
#include "SiegeWarfareMgr.h"
#include "../[CC]SiegeDungeon/SiegeDungeonMgr.h"
#include "LimitDungeonMgr.h"
#include "cCookMgr.h"
#include "VehicleManager.h"
#include "cHousingMgr.h"
#include "Trigger/Manager.h"
#include "DungeonMgr.h"
#include "UiSettingManager.h"
#include "ChannelDialog.h"
#include "ConsignmentMgr.h"
#include "ConsignmentDlg.h"

#include "MonsterGuageDlg.h"

#ifdef _NPROTECT_
#include "NProtectManager.h"
#endif

DWORD g_TempHeroID;
extern HWND _g_hWnd;
extern VECTOR3 gHeroPos;

GLOBALTON(CGameIn)


CGameIn::CGameIn()
{
	m_bGameInAcked	= FALSE;
	m_bInitForGame	= FALSE;
	m_GameInInitKind = eGameInInitKind_Login;

	m_pInventoryDlg		= NULL;
	m_pExchangeDlg		= NULL;
	m_pMixDlg			= NULL;
	m_pDealDlg			= NULL;
	m_pCharDlg			= NULL;
	m_pChattingDlg		= NULL;
	m_pOSChattingDlg	= NULL;
	m_pMainSystemDlg	= NULL;
	m_pDebugDlg	= NULL;
	m_pStallKindSelectDlg	= NULL;
	m_pStreetStallDlg	= NULL;
	m_pNpcScriptDlg		= NULL;
	m_pChatOptionDlg	= NULL;
	m_pOptionDlg		= NULL;
	m_pExitDlg			= NULL;
	m_pMonstermeterDlg	= NULL;
	m_pFarmBuyDlg		= NULL;
	m_pFarmUpgradeDlg	= NULL;
	m_pFarmManageDlg	= NULL;
	m_pDateZoneListDlg			= NULL;
	m_pChallengeZoneListDlg		= NULL;
	m_pChallengeZoneClearNo1Dlg	= NULL;
	m_MoneyDlg			= NULL;
	m_pQuickDlg			= NULL;
	m_pGuageDlg			= NULL;

	m_pMiniMapDlg		= NULL;
	m_pBigMapDlg		= NULL;

	m_pStorageDialog		= NULL;
	m_pNoteDialog		= NULL;
	m_pFriendDialog		= NULL;
	m_pMiniNoteDialogWrite	= NULL;
	m_pMiniNoteDialogRead = NULL;
	m_pMiniFriendDialog	= NULL;
	m_pReviveDialog		= NULL;
	m_pWantNpcDialog	= NULL;

	m_pMonsterGuageDlg	= NULL;
	m_pDissolveDig	= NULL;
	m_pItemShopDialog	= NULL;
	m_MovePoint = 0;
	m_MoveMap	= 0;
	m_DestMoveMap = 0;
	m_GuildIdx	= 0;
	m_SiegeWarMapNum	= 0;

	m_pShoutDlg			= NULL;
	m_pFishingDlg		= NULL;
	m_pFishingGaugeDlg	= NULL;
	m_pFishingPointDlg	= NULL;
	m_pDateMatchingDlg	 = NULL;
	m_pIdentificationDlg = NULL;
	m_pFavorIconDlg		= NULL;
	m_pMatchMyInfoDlg	= NULL;
	m_pMatchPartnerInfoDlg	= NULL;
	m_pMatchRecordDlg	= NULL ;
	ZeroMemory(
		&GameinServerTime,
		sizeof(GameinServerTime));
	m_Clientdate = 0;
	m_Clienttime = 0;

	m_pGameNotifyDlg = NULL;
	m_TutorialDlg = NULL;
	m_HelpDlg = NULL;
	m_TutorialBtnDlg = NULL;
	m_MapMoveDlg = NULL;
	m_pChangeNameDlg = NULL;
	m_pNumberPadDlg = NULL;

	m_pAutoNoteDlg = NULL;
	m_pAutoAnswerDlg = NULL;
	m_pChatRoomMainDlg = NULL;
	m_pChatRoomCreateDlg = NULL;
	m_pChatRoomDlg = NULL;
	m_pChatRoomGuestListDlg	= NULL;
	m_pChatRoomOptionDlg = NULL ;
	m_pChatRoomJoinDlg = NULL ;
	m_pCookDlg = NULL;
	m_pSiegeFlagDlg = NULL;
	m_pFadeDlg = NULL;
	m_pBattleGuageDlg	=	NULL;
	m_bAfterRender = TRUE;
}

CGameIn::~CGameIn()
{

}

#ifdef _TESTCLIENT_
void InitForTestClient(int GameInInitKind)
{
	if( GameInInitKind != eGameInInitKind_MapChange)
	{
		MAINGAME->LoadPack( "data/3dData/character.pak" );
		MAINGAME->LoadPack( "data/3dData/map.pak" );
		MAINGAME->LoadPack( "data/interface/2dImage/image.pak" );
		MAINGAME->LoadPack( "data/3dData/effect.pak" );
		MAINGAME->LoadPack( "data/3dData/monster.pak" );
		MAINGAME->LoadPack( "data/3dData/npc.pak" );
		MAINGAME->LoadPack( "data/interface/2dImage/npcImage.pak" );

		while(TRUE == MAINGAME->IsLoading());

		GAMERESRCMNGR->LoadTestClientInfo();
		APPEARANCEMGR->Init();
		MAP->InitMap(GAMERESRCMNGR->m_TestClientInfo.Map);
	}

	GAMEIN->m_bGameInAcked = TRUE;

	//LOGFILE("OPTIONMGR->ApplySettings()");
	OPTIONMGR->ApplySettings();
}
#endif

BOOL CGameIn::InitForGame(int GameInInitKind)
{
	m_bInitForGame = TRUE;

	WINDOWMGR->CreateGameIn();
	WINDOWMGR->AfterInit();
	USERINFOMGR->LoadUserInfo(eUIK_INTERFACE);
	CHATMGR->SetChattingDlg( GAMEIN->GetChattingDlg() ) ;
	CHATMGR->RestoreChatList();	
	ITEMMGR->ClearCoolTimeInfo();

	// 080411 LYW --- GameIn : ä�ù濡�� ä������ ���ٸ�, ä�� ������ �����Ѵ�.
	CHATROOMMGR->RestoreChat() ;

	QUESTMGR->InitializeQuest();
	PKMGR->Init();
	GUILDUNION->Clear();
	OPTIONMGR->InitForGameIn();
	HOUSINGMGR->Init();

	return TRUE;
}

BOOL CGameIn::Init(void* pInitParam)
{
	int GameInInitKind = *(int*)pInitParam;
	m_GameInInitKind = GameInInitKind;
	m_bGameInAcked = FALSE;

#ifdef _TESTCLIENT_
	InitForTestClient(GameInInitKind);
#endif

	InitForGame(GameInInitKind);
	APPEARANCEMGR->EnbaleReservation();

	if(MAP->GetMapNum() != 0)
	{
		MAP->LoadStaticNpc(MAP->GetMapNum());

		MAP->LoadNpcForQuestDialog() ;
	}


	SHOWDOWNMGR->Init();
	EXCHANGEMGR->Init();
	STREETSTALLMGR->Init();
	FRIENDMGR->Init();
	PARTYMGR->Init();
	GUILDMGR->Init();
	WEATHERMGR->Init();
	FISHINGMGR->Init();
	COOKMGR->Init();
	SKILLTREEMGR->Init();
	CONDUCTMGR->RegistConduct() ;
	CSHMain::Init();

#ifdef _TESTCLIENT_
	InitTestHero();
	InitTestMonster();
	InitTestCharacters();

	if( TSTOOLMGR->IsShowing() )
		TSTOOLMGR->ShowTSDialog( TRUE );
#endif

#ifdef _GMTOOL_
	if( GMTOOLMGR->IsShowing() )
		GMTOOLMGR->ShowGMDialog( TRUE );
#endif

	
	NETWORK->SetCurState(this);

	switch(GameInInitKind)
	{
	case eGameInInitKind_Login:
	case eGameInInitKind_MapChange:
		{
			MSG_DWORD msg;
			msg.Category = MP_USERCONN;
			msg.Protocol = MP_USERCONN_GAMEIN_SYN;
			msg.dwData	 = gChannelNum;
			NETWORK->Send( &msg, sizeof(msg) );
		}
		break;
	case eGameInInitKind_MovePos:
		{
			MSG_DWORD2 msg;
			msg.Category = MP_USERCONN;
			msg.Protocol = MP_USERCONN_GAMEINPOS_SYN;
			msg.dwObjectID = g_TempHeroID;
			msg.dwData1 = gChannelNum;
			msg.dwData2 = m_MovePoint;

			NETWORK->Send( &msg, sizeof(msg) );			
		}
		break;
	case eGameInInitKind_GTEnter:
		{
			MSG_DWORD3 msg;
			msg.Category = MP_GTOURNAMENT;
			msg.Protocol = MP_GTOURNAMENT_BATTLEJOIN_SYN;
			msg.dwObjectID = g_TempHeroID;
			msg.dwData1 = m_GuildIdx;
			msg.dwData2 = 0;
			msg.dwData3 = m_MoveMap;
			NETWORK->Send( &msg, sizeof(msg) );
			
			m_MoveMap = 0;
		}
		break;
	case eGameInInitKind_GTLeave:
		{
			MSGBASE msg;
			msg.Category = MP_GTOURNAMENT;
			msg.Protocol = MP_GTOURNAMENT_LEAVE_SYN;
			msg.dwObjectID = g_TempHeroID;
			NETWORK->Send( &msg, sizeof(msg) );
		}
		break;
	case eGameInInitKind_GTObserverEnter:
		{
			MSG_DWORD3 msg;
			msg.Category = MP_GTOURNAMENT;
			msg.Protocol = MP_GTOURNAMENT_OBSERVERJOIN_SYN;
			msg.dwObjectID = g_TempHeroID;
			msg.dwData1 = m_GuildIdx;
			msg.dwData2 = m_BattleIdx;
			msg.dwData3 = m_MoveMap;
			NETWORK->Send( &msg, sizeof(msg) );
		}
		break;
	case eGameInInitKind_DungeonObserverEnter:
		{
			MSG_WORD3 msg;
			msg.Category = MP_CHEAT;
			msg.Protocol = MP_CHEAT_DUNGEON_OBSERVERJOIN_SYN;
			msg.dwObjectID = g_TempHeroID;
			msg.wData1 = WORD(m_DestMoveMap);
			msg.wData2 = WORD(m_BattleIdx);
			msg.wData3 = WORD(m_MoveMap);
			
			NETWORK->Send( &msg, sizeof(msg) );
		}
		break;
	}

#ifdef _TESTCLIENT_
	g_UserInput.SetInputFocus( TRUE );
	USERINFOMGR->SetMapChage( FALSE );
#endif

	return TRUE;
}

void CGameIn::ReleaseForGame()
{
	if( !m_bInitForGame ) return;

	while(FALSE == m_pBackupMsgQueue.empty())
	{
		MSGBASE* pMsg = (MSGBASE*)m_pBackupMsgQueue.front();
		m_pBackupMsgQueue.pop();
		if(pMsg)
			SAFE_DELETE(pMsg);
	}

	UISETTINGMGR->UpdateSettingInfo();

	WINDOWMGR->DestroyWindowAll();

	WEATHERMGR->EffectOff();

	m_bInitForGame = FALSE;

	m_pInventoryDlg			= NULL;
	m_pExchangeDlg			= NULL;
	m_pMixDlg				= NULL;
	m_pDealDlg				= NULL;
	m_pCharDlg				= NULL;
	m_pChattingDlg			= NULL;
	m_pOSChattingDlg		= NULL;
	m_pMainSystemDlg		= NULL;
	m_pDebugDlg				= NULL;
// LYJ 051017 ���Գ����� �߰�
	m_pStallKindSelectDlg	= NULL;
	m_pStreetStallDlg		= NULL;
	m_pNpcScriptDlg			= NULL;
	m_pChatOptionDlg		= NULL;
	m_pOptionDlg			= NULL;
	m_pExitDlg				= NULL;
	m_MoneyDlg				= NULL;
	m_pQuickDlg				= NULL;
	m_pGuageDlg				= NULL;

	m_pMiniMapDlg			= NULL;
	m_pBigMapDlg			= NULL;

	m_pStorageDialog			= NULL;
	m_pNoteDialog			= NULL;
	m_pFriendDialog			= NULL;
	m_pMiniNoteDialogWrite	= NULL;
	m_pMiniNoteDialogRead	= NULL;
	m_pMiniFriendDialog		= NULL;
	m_pReviveDialog			= NULL;
	m_pWantNpcDialog		= NULL;

	m_pMonsterGuageDlg		= NULL;

	m_pReinforceDlg			= NULL;

	m_pDissolveDig			= NULL;
	m_pShoutDlg				= NULL;

	 m_pTransmogDlg = NULL;
	 
}

void CGameIn::Release(CGameState* pNextGameState)
{
	APPEARANCEMGR->DisableReservation();
	BATTLESYSTEM->ReleaseBattle();
	STATUSICONDLG->Release();

	CURSOR->SetActive( FALSE );
	CHATMGR->SetChattingDlg( NULL );

	AUDIOMGR->StopAll();
	QUICKMGR->Release();
	ITEMMGR->Release();

	SKILLTREEMGR->Release();
	PKMGR->Release();
	USERINFOMGR->SaveUserInfo( eUIK_USERSTATE | eUIK_INTERFACE | eUIK_USERQUEST );
	EFFECTMGR->RemoveAllEffect();
	OBJECTMGR->RemoveAllObject();
	TRIGGERMGR->RestoreTile();
	MAP->Release();

	GUILDWARMGR->Release();
	PICONMGR->DeleteAllPartyIcon();

	ReleaseForGame();

	HACKCHECK->Release();

#ifdef _GMTOOL_
	GMTOOLMGR->ShowGMDialog( FALSE );
#elif _TESTCLIENT_
	TSTOOLMGR->ShowTSDialog( FALSE );
#endif

	SKILLMGR->ReleaseAllSkillObject();
	CAMERA->Release() ;

	g_pExecutive->UnloadAllPreLoadedGXObject(0);

	g_TempHeroID = gHeroID;
	gHeroID = 0;
}

extern WORD mapnumforMSGBOX = 0;
void CGameIn::Process()
{
	if(!m_bGameInAcked)
		return;

	CHATTIPMGR->Process();
	
	GetMiniMapDialog()->Process();

	EXITMGR->Process();
	ITEMMGR->Process();
	SKILLMGR->Process();
	FISHINGMGR->Process();
	GTMGR->Process();
	SIEGEWARFAREMGR->Process();

	//---KES TEST �ʹ� ���������� ���� �ִ� ���� �ƴұ�?
	// 070121 LYW --- Call function to check potal position.
	if( MOVEMGR->IsMoving(HERO) && !HERO->IsPKMode() && !PKMGR->IsPKLooted() && !HERO->IsDied() )
	{
		// 100211 ShinJS --- Ż�Ϳ� ž������ ��� �����ָ� ��Ż �̵�����
		const BOOL bInvalidVehicleCheck = (HERO->IsGetOnVehicle() && !VEHICLEMGR->IsMaster( HEROID ));
        if( !bInvalidVehicleCheck )
		{
			WORD curMapNum = HERO->GetCharacterTotalInfo()->CurMapNum;
			WORD mapNum = GAMERESRCMNGR->CheckMapChangeForMapID(&gHeroPos, curMapNum);
			mapnumforMSGBOX = GAMERESRCMNGR->CheckMapChange(&gHeroPos, curMapNum);

			if( mapNum != 65535 )
				WINDOWMGR->MsgBox(
				MBI_MAPMOVE_AREYOUSURE,
				MBT_YESNO,
				CHATMGR->GetChatMsg(480),
				GetMapName(mapNum));	
				//g_UserInput.GetHeroMove()->ChangeMap( mapNum );
		}
	}

	CSHMain::MainLoop();
	LIMITDUNGEONMGR->Process();
	HACKCHECK->Process();
	HOUSINGMGR->Process();
	DungeonMGR->Process();
#ifdef _NPROTECT_
	NPROTECTMGR->Check();
#endif
	WEATHERMGR->Process();
	QUESTMGR->Process();
}

void CGameIn::BeforeRender()
{
	MAP->Process(gCurTime);
}

void CGameIn::AfterRender()
{
	if(m_bGameInAcked && m_bAfterRender)
	{
		EFFECTMGR->Process(gCurTime);

		BATTLESYSTEM->Render();
		WINDOWMGR->Render();
		NOTIFYMGR->Render();
		GAMENOTIFYMGR->Render();
#if defined(_GMTOOL_) || defined(_TESTCLIENT_)
		CSHMain::Render();
		MHTIMEMGR->DrawStatus();
		MAP->RenderTileData_Debug();
		WEATHERMGR->RenderDebugState();
#endif
	}
}

// description by hseos S
// ����->Ŭ���̾�Ʈ ��� parsing �� ���� ��� �κ�
// description by hseos E
void CGameIn::NetworkMsgParse(BYTE Category,BYTE Protocol,void* pMsg, DWORD dwMsgSize)
{
	if(NULL == HERO &&
		Category != MP_ITEM &&
		Category != MP_TUTORIAL &&
		Protocol != MP_USERCONN_GAMEIN_ACK)
	{
		AddBackupMsg(pMsg, dwMsgSize);
		return;
	}

	switch(Category)
	{
	case MP_MOVE:			MOVEMGR->NetworkMsgParse(Protocol, pMsg);				break;
	case MP_SKILL:			SKILLMGR->NetworkMsgParse(Protocol, pMsg);				break;
	case MP_CHAR:			CHARCALCMGR->NetworkMsgParse(Protocol, pMsg);			break;
	case MP_USERCONN:		UserConn_NetworkMsgParse( Protocol, pMsg ) ;			break ;
	case MP_HACKCHECK:		HACKCHECK->NetworkMsgParse( Protocol, pMsg );			break;
    case MP_PARTY:			PARTYMGR->NetworkMsgParse(Protocol, pMsg);				break;
	case MP_QUICK:			QUICKMGR->NetworkMsgParse(Protocol, pMsg);				break;
	case MP_ITEM:			ITEMMGR->NetworkMsgParse(Protocol,pMsg);				break;
	case MP_SKILLTREE:		SKILLTREEMGR->NetworkMsgParse(Protocol, pMsg);				break;
	case MP_CHAT:			CHATMGR->NetworkMsgParse(Protocol, pMsg);				break;
	case MP_BATTLE:			BATTLESYSTEM->NetworkMsgParse(Protocol, pMsg);			break;
	case MP_EXCHANGE:		EXCHANGEMGR->NetworkMsgParse(Protocol, pMsg);			break;
	case MP_STREETSTALL:	STREETSTALLMGR->NetworkMsgParse(Protocol, pMsg);		break;
	case MP_STORAGE:			STORAGEMGR->NetworkMsgParse(Protocol, pMsg);				break;
	case MP_FRIEND:			FRIENDMGR->NetworkMsgParse(Protocol, pMsg);				break;
	case MP_NOTE:			NOTEMGR->NetworkMsgParse(Protocol, pMsg);				break;
	// 080107 KTH -- 
	case MP_BOSSMONSTER:	OBJECTMGR->NetworkMsgParse(Protocol, pMsg);				break;
	// 090422 ShinJS --- Ż�� MsgParse�߰�
	case MP_VEHICLE:		VEHICLEMGR->NetworkMsgParse(Protocol, pMsg);	break;
	case MP_QUEST:			
		{
			QUESTMGR->NetworkMsgParse( Protocol, pMsg );
		}
		break;
	case MP_PK:				PKMGR->NetworkMsgParse(Protocol, pMsg);					break;
	case MP_CHEAT:			CHEATMGR->NetworkMsgParse(Category,Protocol,(MSGBASE*)pMsg);	break;	
	case MP_GUILD:			GUILDMGR->NetworkMsgParse(Protocol, pMsg);				break;
	case MP_GUILD_UNION:	GUILDUNION->NetworkMsgParse(Protocol, pMsg);			break;
	case MP_NPC:			NPC_NetworkMsgParse( Protocol, pMsg ) ;					break ;
	case MP_SIGNAL:			SIGNAL_NetworkMsgParse( Protocol, pMsg ) ;				break ;
	case MP_GUILD_WAR:		GUILDWARMGR->NetworkMsgParse( Protocol, pMsg );		break;
	case MP_GTOURNAMENT:	GTMGR->NetworkMsgParse( Protocol, pMsg);			break;
	case MP_FACIAL :		FACIALMGR->NetworkMsgParse( Protocol, pMsg );		break;
	case MP_EMOTION :		CONDUCTMGR->NetworkMsgParse( Protocol, pMsg ); break;
	case MP_FARM :				g_csFarmManager.CLI_ParseAnswerFromSrv(pMsg); break;
	case MP_RESIDENTREGIST :	g_csResidentRegistManager.CLI_ParseAnswerFromSrv(pMsg);	break;
	case MP_FAMILY :			g_csFamilyManager.CLI_ParseAnswerFromSrv(pMsg);	break;
	case MP_DATE:				g_csDateManager.CLI_ParseAnswerFromSrv(pMsg); break;
	case MP_TUTORIAL :			TUTORIALMGR->NetworkMsgParse( Protocol, pMsg); break;
	case MP_AUTONOTE:			AUTONOTEMGR->NetworkMsgParse( Protocol, pMsg); break;
	case MP_CHATROOM :			CHATROOMMGR->NetworkMsgParser(pMsg); break;
	case MP_FISHING:			FISHINGMGR->NetworkMsgParse( Protocol, pMsg); break;
	case MP_PET:				PETMGR->NetworkMsgParse( Protocol, pMsg); break;
	case MP_SIEGEWARFARE :		SIEGEWARFAREMGR->NetworkMsgParse( Protocol, pMsg); break;
  	case MP_RECALLNPC :			NPCRECALLMGR->NetworkMsgParser( Protocol, pMsg); break;
	case MP_LIMITDUNGEON :		LIMITDUNGEONMGR->NetworkMsgParser( Protocol, pMsg); break;
	case MP_COOK:				COOKMGR->NetworkMsgParse(Protocol, pMsg); break;
	case MP_HOUSE:				HOUSINGMGR->NetworkMsgParse(Protocol, pMsg); break;
	case MP_TRIGGER:			TRIGGERMGR->NetworkMsgParse(Protocol, pMsg); break;
	case MP_DUNGEON:			DungeonMGR->NetworkMsgParse(Protocol, pMsg); break;
	case MP_PCROOM:				PCROOMMGR->NetworkMsgParse( Protocol, pMsg ); break;
	case MP_CONSIGNMENT:		CONSIGNMENTMGR->NetworkMsgParse( Protocol, pMsg ); break;
	}
}

void CGameIn::OnDisconnect()
{
	m_bDisconnected = TRUE;
	if( cMsgBox::IsInit() )	//��OAOA��AoCE��a? confirm
	{
		// 070122 LYW --- Modified this line.
		cMsgBox* pMsgBox;
		pMsgBox = WINDOWMGR->MsgBox( MBI_SERVERDISCONNECT, MBT_OK, CHATMGR->GetChatMsg( 212 ) );
		//cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_FAILED_NPC_CHANGE_MAP, MBT_OK, CHATMGR->GetChatMsg( 212 ) );
	}
	else
	{
		MessageBox( NULL, "Disconnected To Server.\nThe Game is Closing.", 0, 0 );
		//MAINGAME->SetGameState(eGAMESTATE_END);
		PostMessage( _g_hWnd, WM_CLOSE, NULL, NULL );
	}
}

void CGameIn::SetLoginTime(SYSTEMTIME Time)
{
	GameinServerTime = Time;
}

SYSTEMTIME	CGameIn::GetLoginTime()
{
	return GameinServerTime;
}

//ĳ���� Ŭ���̾�Ʈ �α��� �ð�
void	CGameIn::GetClientLoginTime(DWORD& date, DWORD& time)
{
	date = m_Clientdate;
	time = m_Clienttime;
}

void	CGameIn::SetClientLoginTime(DWORD date, DWORD time)
{
	m_Clientdate = date;
	m_Clienttime = time;
}


void CGameIn::UserConn_NetworkMsgParse(BYTE Protocol,void* pMsg)
{
	switch(Protocol)
	{
	case MP_USERCONN_OBJECT_REMOVE:				UserConn_Object_Remove( pMsg ) ;							break;
	case MP_USERCONN_MONSTER_DIE:				UserConn_Monster_Die( pMsg ) ;								break;
	case MP_USERCONN_NPC_DIE:					UserConn_Monster_Die( pMsg ) ;								break;
	case MP_USERCONN_CHARACTER_DIE:				UserConn_Character_Die( pMsg ) ;							break;	
	// 091110 ONS �� �ִϸ��̼� �߰� : �׾����� �ִϸ��̼� ó�� �߰�
	case MP_USERCONN_PET_DIE:					UserConn_Pet_Die( pMsg ) ;									break;	
	// 070417 LYW --- GameIn : Add network message down exp and level.
	case MP_USERCONN_CHARACTER_APPLYEXP_NOTICE :	UserConn_ApplyExp( pMsg ) ;								break;
	case MP_USERCONN_CHARACTER_DOWNEXP_NOTICE :		UserConn_DownExp( pMsg ) ;								break;
	case MP_USERCONN_CHARACTER_DOWNLEVEL_NOTICE :	UserConn_DownLevel( pMsg ) ;							break;
	case MP_USERCONN_READY_TO_REVIVE:			UserConn_Ready_To_Revive() ;								break;
	case MP_USERCONN_CHARACTER_REVIVE:			UserConn_Character_Revive( pMsg ) ;							break;		
	case MP_USERCONN_CHARACTER_REVIVE_NACK:		UserConn_Character_Revive_Nack( pMsg ) ;					break;
	case MP_USERCONN_CHARACTER_REVIVE_FLAG_ACK:	UserConn_Revive_Flag(pMsg); break;
	case MP_USERCONN_NPC_ADD:					UserConn_Npc_Add( pMsg ) ;									break;
	case MP_USERCONN_GRIDINIT:					UserConn_GridInit() ;										break;
	case MP_USERCONN_GAMEIN_ACK:				UserConn_GameIn_Ack( pMsg ) ;								break;
	case MP_USERCONN_GAMEIN_NACK:				__asm int 3;												break;
	case MP_USERCONN_CHARACTER_ADD:				UserConn_Character_Add( pMsg ) ;							break;
	case MP_USERCONN_MONSTER_ADD:				UserConn_Monster_Add( pMsg ) ;								break;	
	case MP_USERCONN_BOSSMONSTER_ADD:			UserConn_BossMonster_Add( pMsg ) ;							break;	
	case MP_USERCONN_CHANGEMAP_ACK:				UserConn_ChangeMap_Ack( pMsg ) ;							break;
	// 070918 LYW --- GameIn : Add process to change map through the npc.
	case MP_USERCONN_NPC_CHANGEMAP_ACK :		UserConn_Npc_ChangeMap_Ack( pMsg ) ;						break;
	case MP_USERCONN_NPC_CHANGEMAP_NACK :		UserConn_Npc_ChangeMap_Nack() ;									break;
	case MP_USERCONN_CHEAT_CHANGEMAP_ACK:		UserConn_Cheat_ChangeMap_Ack( pMsg ) ;						break;					
	case MP_USERCONN_CHANGEMAP_NACK:			UserConn_ChangeMap_Nack() ;									break;
	// 090512 ONS Ÿ������ �ű����� ���۸������� �̵� ����
	case MP_USERCONN_DEVILMARKET_CHANGEMAP_NACK:UserConn_DevilMarket_ChangeMap_Nack() ;						break;
	case MP_USERCONN_CHARACTERLIST_ACK:			UserConn_CharacterList_Ack( pMsg ) ;						break;
	case MP_USERCONN_BACKTOCHARSEL_ACK:			MAINGAME->SetGameState( eGAMESTATE_CHARSELECT );			break;
	case MP_USERCONN_BACKTOCHARSEL_NACK:		UserConn_BackToCharSel_Nack() ;								break;
	case MP_USERCONN_MAPDESC:					UserConn_MapDesc( pMsg ) ;									break;
	case MP_USERCONN_SETVISIBLE:				UserConn_SetVisible( pMsg ) ;								break;
	case MP_USERCONN_EVENTITEM_USE:				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(396) );	break;
	case MP_USERCONN_EVENTITEM_USE2:			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(396) );	break;
	case MP_USERCONN_READY_TO_REVIVE_BY_GFW:
		{
			cDialog* dialog = WINDOWMGR->GetWindowForID( GD_REVIVALDLG );
			ASSERT( dialog );
			dialog->SetActive( TRUE );
			break;
		}
	case MP_USERCONN_PET_ADD: UserConn_Pet_Add( pMsg ); break;
	case MP_USERCONN_HEROPET_REMOVE: UserConn_HeroPet_Remove( pMsg ); break;
	// 090316 LUJ, Ż��
	case MP_USERCONN_VEHICLE_ADD: UserConn_Vehicle_Add( pMsg ); break;
	case MP_USERCONN_VEHICLE_REMOVE: UserConn_Vehicle_Remove( pMsg ); break;
	case MP_USERCONN_MONSTER_TAME_ACK:
		{
			const MSG_DWORD* const message = (MSG_DWORD*)pMsg;
			const DWORD ownerObjectIndex = message->dwObjectID;
			const DWORD tamedObjectIndex = message->dwData;

			UserConn_Monster_Tame(
				ownerObjectIndex,
				tamedObjectIndex);
			break;
		}
	case MP_USERCONN_GETSERVERTIME_ACK:
		{
			stTime64t* pmsg = (stTime64t*)pMsg;
			MHTIMEMGR->SetServerTime( pmsg->time64t );
#ifdef _GMTOOL_
			struct tm curTimeWhen = *_localtime64( &pmsg->time64t );
			CHATMGR->AddMsg( CTC_SYSMSG, "server time: %d-%d-%d %02d:%02d:%02d", 
				curTimeWhen.tm_year + 1900,
				curTimeWhen.tm_mon + 1,
				curTimeWhen.tm_mday,
				curTimeWhen.tm_hour,
				curTimeWhen.tm_min,
				curTimeWhen.tm_sec );
#endif
		}
		break;

		// 100507 ShinJS --- ä������ ����
	case MP_USERCONN_CHANNELINFO_ACK:
		{
			MSG_CHANNEL_INFO * pmsg = (MSG_CHANNEL_INFO*)pMsg;

			m_pMiniMapDlg->SetChanelComboBox( pmsg );

			if( pmsg->Count <= 1 )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2223 ) );		// ���ش� ���� ä���� �ϳ��� ���� �ϹǷ� ������ �Ұ��� �մϴ�."
				break;
			}
		}
		break;
	case MP_USERCONN_CHANGE_CHANNEL_IN_GAME_NACK:
		{
			if(cDialog* const dialog = WINDOWMGR->GetWindowForID(CHA_CHANNELDLG))
			{
				dialog->SetActive(
					FALSE);
				CHATMGR->AddMsg(
					CTC_SYSMSG,
					CHATMGR->GetChatMsg(2223));
			}
		}
		break;
	case MP_USERCONN_BILLING_ALERT_ACK:
		{
			const MSG_INT2* const message = (MSG_INT2*)pMsg;
			const int result = message->nData1;
			const int remainedSecond = message->nData2;

			UserConn_Alert(
				result,
				remainedSecond);
		}
		break;
	case MP_USERCONN_BILLING_STOP_ACK:
		{
			if(cWindow* const window = GAMEIN->GetMiniMapDialog()->GetWindowForID(MNM_PCROOM_MALL_BTN))
			{
				window->SetActive(
					FALSE);
			}

			if(cWindow* const window = GAMEIN->GetMiniMapDialog()->GetWindowForID(MNM_PCROOM_PROVIDE_ITEM_BTN))
			{
				window->SetActive(
					FALSE);
			}

			CHATMGR->AddMsg(
				CTC_SYSMSG,
				CHATMGR->GetChatMsg(1347));
		}
		break;
	case MP_USERCONN_OPTION_ACK:
		{
			const MSG_DWORD2* const message = (MSG_DWORD2*)pMsg;
			CPlayer* player = (CPlayer*)OBJECTMGR->GetObject(
				message->dwObjectID);

			if(0 == player)
			{
				break;
			}
			else if(eObjectKind_Player != player->GetObjectKind())
			{
				break;
			}

			player->GetCharacterTotalInfo()->HideFlag = message->dwData1;
			player->SetMoveType(BOOL(message->dwData2));
			APPEARANCEMGR->AddCharacterPartChange(
				player->GetID());
			player->SetBaseMotion();
		}
		break;
	}
}

void CGameIn::UserConn_Alert(int result, int remainedSecond)
{
	switch(result)
	{
	case 1: // ������� ���� �ð�
		{
			const int oneMinute = 60;
			const int oneHour = oneMinute * 60;
			const int oneDay = oneHour * 24;

			// �Ϸ� �̻� �������� ���ϰ� ǥ������ �ʴ´�
			if(const int day = remainedSecond / oneDay)
			{
				CHATMGR->AddMsg(
					CTC_SYSMSG,
					CHATMGR->GetChatMsg(1349));
				break;
			}

			TCHAR text[MAX_PATH] = {0};
			TCHAR buffer[MAX_PATH] = {0};

			if(const int hour = remainedSecond / oneHour)
			{
				_stprintf(
					buffer,
					CHATMGR->GetChatMsg(1410),
					hour);
				_tcscat(
					text,
					buffer);

				remainedSecond = remainedSecond - hour * oneHour;
			}

			if(const int minute = remainedSecond / oneMinute)
			{
				_stprintf(
					buffer,
					_T(" %d%s"),
					minute,
					CHATMGR->GetChatMsg(794));
				_tcscat(
					text,
					buffer);

				remainedSecond = remainedSecond - minute * oneMinute;
			}

			CHATMGR->AddMsg(
				CTC_SYSMSG,
				CHATMGR->GetChatMsg(1348),
				text);
			break;
		}
	case 0: // ���� �ð� ����. ���� ����
	case -1: // ȯ�� ��ġ. ���� ����
	case -2: // �ߺ� �α���. ���� ����
	case -4: // ��Ÿ ���� ����
		{
			CHATMGR->AddMsg(
				CTC_SYSMSG,
				CHATMGR->GetChatMsg(1347));
			break;
		}
	case -3:
		{
			CHATMGR->AddMsg(
				CTC_SYSMSG,
				"You're started for reserved speciality on PC room");
			break;
		}
	default:
		{
			CHATMGR->AddMsg(
				CTC_SYSMSG,
				"Not defined alarm is received");
			break;
		}
	}
}

void CGameIn::UserConn_Monster_Tame(DWORD ownerObjectIndex, DWORD tamedObjectIndex) const
{
	CMonster* const monster = (CMonster*)OBJECTMGR->GetObject(
		tamedObjectIndex);

	if(0 == monster)
	{
		return;
	}
	else if(FALSE == (eObjectKind_Monster & monster->GetObjectKind()))
	{
		return;
	}

	monster->SetOwnerIndex(
		ownerObjectIndex);
}

void CGameIn::UserConn_Object_Remove( void* pMsg )
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

	CObject* pObject = OBJECTMGR->GetObject(pmsg->dwData);
	if(pObject == NULL)
	{
		return;
	}

	// 091026 LUJ, �ڵ� ����
	switch(pObject->GetObjectKind())
	{
	case eObjectKind_Player:
		{
			pObject->OnStartObjectState( eObjectState_Exit );
			break;
		}
	case eObjectKind_Npc:
		{
			const DWORD dwRecallNpcIdx = ( (CNpc*)pObject )->GetNpcTotalInfo()->dwRecallNpcIdx ;

			if(dwRecallNpcIdx)
			{
				NPCRECALLMGR->OnRemoveRecallNpc((CNpc*)pObject);
			}

			break;
		}
	case eObjectKind_Vehicle:
		{
			CVehicle* const vehicle = (CVehicle*)pObject;
			const BOOL isMovableVehicle(0 < vehicle->GetMoveSpeed());

			// 091026 LUJ, ����ΰ� ������ �̵��� Ż���� ���Ž�Ű�� �ʴ´�
			//			�׸��� �̵��� ���� ������Ʈ�� �߰�/���ŵǴµ�, �̰� �ݴ�� �Ǵ� ��찡 �ִ�.
			//			�̷� ���� ������Ʈ�� ���ŵǰ� ��ǥ�� ������Ʈ���� �ʾ� �������� �ʵ��� �ߴ�.
			// 091106 ShinJS, Ż���� ���������� ���(��ȯ����)�� ���ŵɼ� �ֵ��� ����.
			if(gHeroID == vehicle->GetOwnerIndex() &&
				! vehicle->IsDied() &&
				isMovableVehicle)
			{
				return;
			}

			break;
		}
	case eObjectKind_Pet:
		{
			// 091026 LUJ, ����ΰ� ������ ���� ��� �������� �ʰ� ��Ȳ�� ���� ��ġ�� �̵���Ų��
			//			������ �ִ� �ڵ带 Ŭ���̾�Ʈ�� �̵����״�.
			if(HEROPET == pObject)
			{
				CHero* const hero = OBJECTMGR->GetHero();
				VECTOR3 heroPosition = {0};
				hero->GetPosition(&heroPosition);
				VECTOR3 petPosition = {0};
				pObject->GetPosition(&petPosition);

				const float distance = CalcDistance(
					&heroPosition,
					&petPosition);

				if(3000 < distance)
				{
					MOVEMGR->SetHeroPetTarget();
				}

				return;
			}

			break;
		}
	}

	APPEARANCEMGR->CancelAlphaProcess(pObject);
	APPEARANCEMGR->CancelReservation(pObject);
	OBJECTMGR->AddGarbageObject(pObject);
}

void CGameIn::UserConn_Monster_Die( void* pMsg )
{
	MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
	CObject* pObject = OBJECTMGR->GetObject(pmsg->dwData2);
	if(!pObject)
		return;
	// 090316 LUJ, �̹� ���� ��� ó���� �ʿ� ����.
	else if( pObject->IsDied() )
		return;

	pObject->SetDieFlag();

	if(const BOOL isSuicide = (pmsg->dwObjectID == pmsg->dwData2))
	{
		OBJECTACTIONMGR->Die(
			pObject,
			pObject,
			rand() < RAND_MAX / 2,
			rand() < RAND_MAX / 2,
			rand() < RAND_MAX / 2);
	}
}


void CGameIn::UserConn_Character_Die( void* pMsg )
{
	const MSG_DWORD2* const pmsg = (MSG_DWORD2*)pMsg;
	CPlayer* const player = ( CPlayer* )OBJECTMGR->GetObject( pmsg->dwData2 );

	if( 0 == player ||
		player->GetObjectKind() != eObjectKind_Player )
	{
		return;
	}

	// 091214 ShinJS --- HERO�� ���� ��� WayPoint�� �����Ѵ�.
	if( HEROID == player->GetID() )
		HERO->ClearWayPoint();

	// 090316 LUJ, ����� ���� ó���� �Ѵ�
	VEHICLEMGR->GetOff( player->GetID(), player->GetVehicleID(), player->GetVehicleSeatPos() );
	player->SetDieFlag();
}

// 091110 ONS �� �ִϸ��̼� �߰� : �׾����� �ִϸ��̼� ó�� �߰�
void CGameIn::UserConn_Pet_Die( void* pMsg )
{
	MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
	CObject* pObject = OBJECTMGR->GetObject(pmsg->dwData2);
	if(!pObject)
		return;
	// 090316 LUJ, �̹� ���� ��� ó���� �ʿ� ����.
	else if( pObject->IsDied() )
		return;

	pObject->SetDieFlag();
}

// 070418 LYW --- GameIn : Add function to notice apply exp.
void CGameIn::UserConn_ApplyExp( void* pMsg )
{
	MSG_DWORDEX2* pmsg = ( MSG_DWORDEX2* )pMsg ;

	HERO->SetExpPoint( pmsg->dweData1 ) ;

	CHATMGR->AddMsg( CTC_GETEXP, CHATMGR->GetChatMsg(120), (float)pmsg->dweData2 ) ;
}

// 070418 LYW --- GameIn : Add function to notice down exp.
void CGameIn::UserConn_DownExp( void* pMsg )														// ����ġ �϶� �޽����� ó���ϴ� �Լ�.
{
	// 080602 LYW --- Player : ����ġ ��ġ (__int32) ���� (__int64) ������� ���� ó��.
	//MSG_DWORD3* pmsg = ( MSG_DWORD3* )pMsg ;														// �޽����� �޴´�.
	MSG_DWORDEX3* pmsg = ( MSG_DWORDEX3* )pMsg ;														// �޽����� �޴´�.

	if(pmsg->dweData3)																				// dwData3�� TURE�̸�,
	{
		CHATMGR->AddMsg( CTC_GETEXP, CHATMGR->GetChatMsg(848), pmsg->dweData1 ) ;					// ���ڸ� ��Ȱ �߰� ����ġ �϶� �޽����� ����Ѵ�.
	}
	else																							// dwData3�� FALSE�̸�,
	{
		CHATMGR->AddMsg( CTC_GETEXP, CHATMGR->GetChatMsg(849), pmsg->dweData1 ) ;					// ����ġ �϶� �޽����� ����Ѵ�.
	}

	HERO->SetExpPoint(pmsg->dweData2) ;																// �÷��̾��� ����ġ�� �����Ѵ�.
}

// 070418 LYW --- GameIn : Add function to notice down level.
void CGameIn::UserConn_DownLevel( void* pMsg )														// ���� �ٿ� �޽����� ó���ϴ� �Լ�.
{
	// 080602 LYW --- Player : ����ġ ��ġ (__int32) ���� (__int64) ������� ���� ó��.
	//MSG_DWORD4* pmsg = ( MSG_DWORD4* )pMsg ;														// �޽����� �޴´�.
	MSG_DWORDEX4* pmsg = ( MSG_DWORDEX4* )pMsg ;														// �޽����� �޴´�.

	HERO->SetLevel(static_cast<LEVELTYPE>(pmsg->dweData1)) ;																	// �÷��̾��� ������ �����Ѵ�.
	HERO->SetExpPoint(pmsg->dweData2) ;																// �÷��̾��� ����ġ�� �����Ѵ�.

	if(pmsg->dweData4)																				// dwData4�� TRUE �̸�,
	{
		CHATMGR->AddMsg( CTC_GETEXP, CHATMGR->GetChatMsg(848), pmsg->dweData3 ) ;					// ���ڸ� ��Ȱ �߰� ����ġ �϶� �޽����� ����Ѵ�.
	}
	else																							// dwData4�� FALSE�̸�,
	{
		CHATMGR->AddMsg( CTC_GETEXP, CHATMGR->GetChatMsg(849), pmsg->dweData3 ) ;					// ����ġ �϶� �޽����� ����Ѵ�.
	}

	CHATMGR->AddMsg( CTC_GETEXP, CHATMGR->GetChatMsg(657)) ;										// �����ٿ� �޽����� ����Ѵ�.

	// 080310 LUJ, ��ų â�� ������ �����Ѵ�
	{
		cSkillTrainingDlg* dialog = GAMEIN->GetSkillTrainingDlg();

		if( dialog &&
			dialog->IsActive() )
		{
			dialog->OpenDialog();
			dialog->SetSkillInfo();
		}
	}
}

void CGameIn::UserConn_Ready_To_Revive()
{
	// 080808 KTH -- ������ �����̸� ��Ȱ�� �ƴ϶� 5���Ŀ� �̵� �ϵ��� �Ѵ�.
  	if( SIEGEWARFAREMGR->IsSiegeWarfareZone(MAP->GetMapNum() ) )
  	{
  		SIEGEWARFAREMGR->SetRevive(TRUE);
  		SIEGEWARFAREMGR->SetReviveTime();
  		return;
  	}

	cDialog* const reviveDialog = WINDOWMGR->GetWindowForID(CR_DIALOG);

	if(reviveDialog)
	{
		reviveDialog->SetActive(TRUE);
	}
}


void CGameIn::UserConn_Character_Revive( void* pMsg )									// ĳ���� ��Ȱ�� ó���ϴ� �Լ�.
{
	MOVE_POS* pmsg = (MOVE_POS*)pMsg;													// �޽����� �޴´�.
	VECTOR3 pos;																		// ��ġ�� ��Ÿ���� ���͸� �����Ѵ�.

	CObject* pObject = OBJECTMGR->GetObject(pmsg->dwMoverID);							// �޽����� ���̵�� ������Ʈ�� �޴´�.

	if( pObject )																		// ������Ʈ ������ ��ȿ���� üũ�Ѵ�.
	{
		pmsg->cpos.Decompress(&pos);													// ��Ȱ ��ġ ������ �޴´�.

		OBJECTACTIONMGR->Revive(pObject,&pos);											// ������ ��ġ�� ������Ʈ�� ��Ȱ��Ų��.

		if(pObject->GetID() == gHeroID)
		{
			cDialog* const reviveDialog = WINDOWMGR->GetWindowForID(CR_DIALOG);

			if(reviveDialog)
			{
				reviveDialog->SetDisable(FALSE);
				reviveDialog->SetActive(FALSE);
			}
		}
	}
}

void CGameIn::UserConn_Revive_Flag(LPVOID packet)
{
	cDialog* const reviveDialog = WINDOWMGR->GetWindowForID(CR_DIALOG);

	if(0 == reviveDialog)
	{
		return;
	}

	cWindow* const presentSpotButton = reviveDialog->GetWindowForID(CR_PRESENTSPOT);
	cWindow* const townSpotButton = reviveDialog->GetWindowForID(CR_TOWNSPOT);

	if(0 == presentSpotButton ||
		0 == townSpotButton)
	{
		return;
	}

	const DWORD enableColor = RGB(255, 255, 255);
	const DWORD unableColor = RGB(0, 0, 0);	

	// 100111 LUJ, ���ڸ� ��Ȱ ��ư�� �ʱ�ȭ�Ѵ�
	presentSpotButton->SetDisable(FALSE);
	presentSpotButton->SetActive(TRUE);
	presentSpotButton->SetImageRGB(enableColor);
	presentSpotButton->SetToolTip("");
	// 100111 LUJ, �������� ��Ȱ ��ư�� �ʱ�ȭ�Ѵ�
	townSpotButton->SetDisable(FALSE);
	townSpotButton->SetActive(TRUE);
	townSpotButton->SetImageRGB(enableColor);
	townSpotButton->SetToolTip("");

	const MSG_INT* const message = (MSG_INT*)packet;

	switch(message->nData)
	{
	case ReviveFlagHere:
		{
			cImage image;
			SCRIPTMGR->GetImage(
				0,
				&image,
				PFT_HARDPATH);

			townSpotButton->SetImageRGB(unableColor);
			townSpotButton->SetDisable(TRUE);
			townSpotButton->SetToolTip(
				CHATMGR->GetChatMsg(1867),
				RGBA_MAKE(255, 255, 255, 255),
				&image,
				TTCLR_ITEM_CANEQUIP);
			break;
		}
	case ReviveFlagTown:
		{
			cImage image;
			SCRIPTMGR->GetImage(
				0,
				&image,
				PFT_HARDPATH);

			presentSpotButton->SetImageRGB(unableColor);
			presentSpotButton->SetDisable(TRUE);
			presentSpotButton->SetToolTip(
				CHATMGR->GetChatMsg(1867),
				RGBA_MAKE(255, 255, 255, 255),
				&image,
				TTCLR_ITEM_CANEQUIP);
			break;
		}
	}
}

void CGameIn::UserConn_Character_Revive_Nack( void* pMsg )
{
#ifdef _GMTOOL_
	const MSG_BYTE* const message = (MSG_BYTE*)pMsg;

	switch(message->bData)
	{
	case 1:
		{
			CHATMGR->AddMsg(
				CTC_SYSMSG,
				"Error in reviving: you're died already");
			break;
		}
	case 2:
		{
			CHATMGR->AddMsg(
				CTC_SYSMSG,
				"Error in reviving: you're in be looting");
			break;
		}
	case 3:
		{
			CHATMGR->AddMsg(
				CTC_SYSMSG,
				"Error in reviving: your revive flag is invalid");
			break;
		}
	case 4:
		{
			CHATMGR->AddMsg(
				CTC_SYSMSG,
				"Error in reviving: you're in exiting");
			break;
		}
	}
#endif

	cDialog* const reviveDialog = WINDOWMGR->GetWindowForID(CR_DIALOG);

	if(reviveDialog)
	{
		reviveDialog->SetDisable(FALSE);
	}
}


void CGameIn::UserConn_Npc_Add( void* pMsg )
{
	SEND_NPC_TOTALINFO* pmsg = (SEND_NPC_TOTALINFO*)pMsg;
			
	BASEMOVE_INFO moveInfo;
	moveInfo.AddedMoveSpeed = 0.f;
	moveInfo.SetFrom(&pmsg->MoveInfo);
	CAddableInfoIterator iter(&pmsg->AddableInfo);
	BYTE AddInfoKind;
	while((AddInfoKind = iter.GetInfoKind()) != CAddableInfoList::None)
	{
		ySWITCH(AddInfoKind)
			yCASE(CAddableInfoList::MoveInfo)
				COMPRESSEDTARGETPOS tpos;
				iter.GetInfoData(&tpos);
				moveInfo.bMoving = TRUE;
				moveInfo.SetMaxTargetPosIdx(tpos.PosNum);
				ASSERT( tpos.PosNum <= MAX_CHARTARGETPOSBUF_SIZE );
				moveInfo.SetCurTargetPosIdx(0);
				for(BYTE n=0;n<tpos.PosNum;++n)
					tpos.pos[n].Decompress(moveInfo.GetTargetPosition(n));
			yCASE(CAddableInfoList::ShowdownInfo)
			yCASE(CAddableInfoList::StreetStall)
		yENDSWITCH
		
		iter.ShiftToNextData();
	}

	// 100308 pdy �������� �Ľ̵Ǽ��� npc�̸��� KeyWordó���� �ȵǾ��־� Ŭ�󿡼� ��ȯ���ش�. 
	if( pmsg->TotalInfo.dwRecallNpcIdx != 0 )
	{
		CNpcRecallBase* pRecallBase = NPCRECALLMGR->Get_RecallBase( pmsg->TotalInfo.dwRecallNpcIdx ) ; 
		if( pRecallBase )
		{
			SafeStrCpy( pmsg->BaseObjectInfo.ObjectName , pRecallBase->Get_NpcName() , MAX_NPC_NAME_LENGTH+1 ) ;
		}
	}
	else
	{
		char temp[256] = {0,};
		_parsingKeywordString( pmsg->BaseObjectInfo.ObjectName , temp );
		SafeStrCpy( pmsg->BaseObjectInfo.ObjectName , temp , MAX_NPC_NAME_LENGTH+1 ) ;
	}
	
	CNpc* pNpc = OBJECTMGR->AddNpc(&pmsg->BaseObjectInfo,&moveInfo,&pmsg->TotalInfo);
	MOVEMGR->SetAngle(pNpc,pmsg->Angle,0);

	// Npc Add
	QUESTMGR->SetNpcData( pNpc );

	if(pmsg->bLogin)
	{
		TARGETSET set;
		set.pTarget = pNpc;
		//EFFECTMGR->StartEffectProcess(eEffect_MonRegen,pMon,&set,0,pMon->GetID());
		EFFECTMGR->StartEffectProcess(eEffect_NewCharacter,pNpc,&set,0,pNpc->GetID());
	}

	// 091104 ONS NPC�� �߰��ɶ� �ش� ����Ʈ�� ���� �߰��Ѵ�.
	TRIGGERMGR->RegenEffect(pNpc->GetID());

	// 091208 pdy ��ȯ npc ����Ʈ��ũ ���ó�� �߰�
	if( pmsg->TotalInfo.dwRecallNpcIdx )
	{
		NPCRECALLMGR->OnAddRecallNpc(pNpc);
	}

}


void CGameIn::UserConn_GridInit()
{
	DeleteFile("DEBUG.txt");
	m_bGameInAcked = TRUE;

	g_UserInput.GetKeyboard()->ClearKeyState();
	g_UserInput.GetKeyboard()->UpdateKeyboardState();
	g_UserInput.SetInputFocus( TRUE );
	USERINFOMGR->LoadUserInfo( eUIK_USERSTATE );
}


void CGameIn::UserConn_GameIn_Ack( void* pMsg )
{
	ASSERT(!m_bGameInAcked);

	SEND_HERO_TOTALINFO * pmsg = (SEND_HERO_TOTALINFO *)pMsg;
	ASSERT(!(pmsg->SendMoveInfo.CurPos.wx == 0 && pmsg->SendMoveInfo.CurPos.wz == 0));

	//ĳ���� ���� �α��νð�, ĳ���� Ŭ���̾�Ʈ �α��νð� ���
	GAMEIN->SetLoginTime(pmsg->ServerTime);
	GAMEIN->SetClientLoginTime(MHTIMEMGR->GetMHDate(), MHTIMEMGR->GetMHTime());

	OBJECTMGR->RegisterHero(pmsg);
	OPTIONMGR->UpdateHideFlagFromDB(pmsg->ChrTotalInfo.HideFlag);
	OPTIONMGR->UpdateMoveTypeFromDB(pmsg->ChrTotalInfo.bWingsMove);
	HERO->SetMoveType(pmsg->ChrTotalInfo.bWingsMove);
	HERO->SetBaseMotion();
	OPTIONMGR->ApplySettings();
	APPEARANCEMGR->InitAppearance(HERO);
	OBJECTSTATEMGR->InitObjectState(HERO);
	GAMEIN->GetMiniMapDialog()->AddHeroIcon(HERO);
	GAMEIN->GetCharacterDialog()->SetPointLeveling(
		TRUE,
		pmsg->HeroTotalInfo.LevelUpPoint);

	EFFECTMGR->StartHeroEffectProcess(eEffect_NewCharacter);
	// 091116 ONS UI ��ġ/Ȱ��ȭ���¿� ������ ������ �����Ѵ�.
	UISETTINGMGR->ApplySettingInfo();
	FRIENDMGR->LogInNotify();
	
	VECTOR2 sidpos;
	sidpos.x = 260;
	sidpos.y = 0;
	STATUSICONDLG->Init(HERO,&sidpos,8);

	QUESTMGR->ProcessNpcMark();

	CAMERA->Init(HERO,30,0,1000);
	CAMERA->SetTargetAngleX(0,29.9f,0);
	HERO->InitializeEye();
	HERO->SetStorageNum( static_cast<BYTE>(pmsg->StorageSize) );

	if( &pmsg->ChrTotalInfo.DateMatching.bIsValid )
	{
		GAMEIN->GetDateMatchingDlg()->UpdateMyInfo(&pmsg->ChrTotalInfo.DateMatching) ;
	}

	g_csFarmManager.Init(MAP->GetMapNum());
	g_csDateManager.Init();

	SIEGEWARFAREMGR->LoadInfo();
	SIEGEDUNGEONMGR->LoadInfo();

	if(GTMAPNUM == MAP->GetMapNum())
	{
		HERO->SetCharacterSize(
			1.0f);
	}
	else if(TRUE == SIEGEWARFAREMGR->IsSiegeWarfareKindMap(MAP->GetMapNum()))
	{
		if(FALSE == SIEGEWARFAREMGR->Is_CastleMap())
		{
			HERO->SetCharacterSize(
				1.0f);
		}

		MSGBASE message;
		ZeroMemory(
			&message,
			sizeof(message));
		message.Category = MP_SIEGEWARFARE;
		message.Protocol = MP_SIEGEWARFARE_STATE_SYN;
		message.dwObjectID = gHeroID;
		NETWORK->Send(
			&message,
			sizeof(message));
	}
	else if( HOUSINGMGR->IsHousingMap() )
	{
		HERO->SetCharacterSize(
			1.0f);

		MSGBASE msg;
		SetProtocol(&msg, MP_HOUSE, MP_HOUSE_INFO_SYN);
  		msg.dwObjectID = gHeroID;
  		NETWORK->Send( &msg, sizeof( msg ) );
		HOUSINGMGR->StartHouseInfoLoading();
	}
	else if( DungeonMGR->IsDungeonMap(MAP->GetMapNum()))
	{
		DungeonMGR->Init();

		MSGBASE msg;
		SetProtocol(&msg, MP_DUNGEON, MP_DUNGEON_INFO_SYN);
  		msg.dwObjectID = gHeroID;
  		NETWORK->Send( &msg, sizeof( msg ) );
	}

	if( m_GameInInitKind == eGameInInitKind_Login )
	{
		MSGBASE msg;
		msg.Category = MP_ITEM;
		msg.Protocol = MP_ITEM_GIFT_EVENT_START;
		msg.dwObjectID = gHeroID;
		NETWORK->Send( &msg, sizeof( msg ) );
	}

	// 100127 ONS ������ ��� ���ӽ��� Ʃ�丮���� ǥ������ �ʵ��� ����.
	if ( (m_GameInInitKind == eGameInInitKind_Login )
		&& (!TUTORIALMGR->IsCompleteTutorial(e_TT_FirstGameIn)) 
		&& (HERO->GetCharacterTotalInfo()->Race != RaceType_Devil) )
	{
		TUTORIALMGR->Start_Specification_Tutorial(e_TT_FirstGameIn) ;
	}
	// 100611 ������ ��ư Ȱ������ �Ǵ�

	m_pMiniMapDlg->ChangeLevelUpBTNState();

	// ä�� ���� ��û
	GAMEIN->GetMiniMapDialog()->RequestChannelInfo();


	ProcessBackupMsg();
}


void CGameIn::UserConn_Character_Add(void* pMsg )
{
	SEND_CHARACTER_TOTALINFO* const pmsg = (SEND_CHARACTER_TOTALINFO*)pMsg;
	BASEMOVE_INFO moveInfo;	
	ZeroMemory(&moveInfo, sizeof(moveInfo));
	moveInfo.SetFrom(&pmsg->MoveInfo);

	for(CAddableInfoIterator iter(&pmsg->AddableInfo);
		CAddableInfoList::MoveInfo == iter.GetInfoKind();
		iter.ShiftToNextData())
	{
		COMPRESSEDTARGETPOS compressedPosition = {0};
		iter.GetInfoData(&compressedPosition);
		moveInfo.SetMaxTargetPosIdx(compressedPosition.PosNum);

		for(BYTE i = 0;i < compressedPosition.PosNum; ++i)
		{
			VECTOR3* const targetPosition = moveInfo.GetTargetPosition(i);
			compressedPosition.pos[i].Decompress(targetPosition);
		}
	}

	moveInfo.bMoving = (0 < moveInfo.GetMaxTargetPosIdx());

	CPlayer* pPlayer = OBJECTMGR->AddPlayerThroughCache(
		&pmsg->BaseObjectInfo,
		&moveInfo,
		&pmsg->TotalInfo,pmsg);

	// 090422 ShinJS --- Ż�� ������ �ְ� ž������ ��� ž��ó��
	const DWORD dwVehicleID = pmsg->mMountedVehicle.mVehicleIndex;
	const DWORD dwVehicleSeatPos = pmsg->mMountedVehicle.mSeatIndex;

	// Player Ż�� ���� ����
	pPlayer->SetVehicleID( dwVehicleID );
	pPlayer->SetVehicleSeatPos( dwVehicleSeatPos );
	pPlayer->SetRideFurnitureID( pmsg->mRiderInfo.dwFurnitureObjectIndex );
	pPlayer->SetRideFurnitureSeatPos(pmsg->mRiderInfo.wSlot);

	if( HOUSINGMGR->IsHousingMap() )
	{
		pPlayer->SetAngle(
			HOUSINGMGR->GetStartAngle());
		pPlayer->SetCharacterSize(
			1.0f);
	}
	else if(GTMAPNUM == MAP->GetMapNum())
	{
		pPlayer->SetCharacterSize(
			1.0f);
	}
	else if(FALSE == SIEGEWARFAREMGR->Is_CastleMap() &&
		TRUE == SIEGEWARFAREMGR->IsSiegeWarfareKindMap(MAP->GetMapNum()))
	{
		pPlayer->SetCharacterSize(
			1.0f);
	}
}

void CGameIn::UserConn_Monster_Add(void* pMsg )
{
	SEND_MONSTER_TOTALINFO* pmsg = (SEND_MONSTER_TOTALINFO*)pMsg;
	if(pmsg->TotalInfo.MonsterKind > 10000)
	{
		ASSERT(0);
	}

	BASEMOVE_INFO moveInfo;
	moveInfo.AddedMoveSpeed = 0.f;
	moveInfo.SetFrom(&pmsg->MoveInfo);
	CAddableInfoIterator iter(&pmsg->AddableInfo);
	BYTE AddInfoKind;
	while((AddInfoKind = iter.GetInfoKind()) != CAddableInfoList::None)
	{
		ySWITCH(AddInfoKind)
			yCASE(CAddableInfoList::MoveInfo)
				COMPRESSEDTARGETPOS tpos;
				iter.GetInfoData(&tpos);
				moveInfo.bMoving = TRUE;
				moveInfo.SetMaxTargetPosIdx(tpos.PosNum);
				ASSERT( tpos.PosNum <= MAX_CHARTARGETPOSBUF_SIZE );
				moveInfo.SetCurTargetPosIdx(0);
				for(BYTE n=0;n<tpos.PosNum;++n)
					tpos.pos[n].Decompress(moveInfo.GetTargetPosition(n));
			yCASE(CAddableInfoList::ShowdownInfo)
			yCASE(CAddableInfoList::StreetStall)
		yENDSWITCH
		
		iter.ShiftToNextData();
	}
	
#ifdef TAIWAN_LOCAL
	//�ӽ÷�! �����̸��� �������� �Ⱥ����ֵ��� ����!
	SafeStrCpy( pmsg->BaseObjectInfo.ObjectName,
		GAMERESRCMNGR->GetMonsterListInfo( pmsg->TotalInfo.MonsterKind )->Name,
		MAX_NAME_LENGTH+1 );
#endif
	
	CMonster* pMon = OBJECTMGR->AddMonster(&pmsg->BaseObjectInfo,&moveInfo,&pmsg->TotalInfo);
						
	//////////////////////////////////////////////////////////////////////////

	if(pmsg->bLogin)
	{
		TARGETSET set;
		set.pTarget = pMon;
		//EFFECTMGR->StartEffectProcess(eEffect_MonRegen,pMon,&set,0,pMon->GetID());
		EFFECTMGR->StartEffectProcess(eEffect_NewCharacter,pMon,&set,0,pMon->GetID());
	}
}


void CGameIn::UserConn_BossMonster_Add(void* pMsg )
{
	SEND_MONSTER_TOTALINFO* pmsg = (SEND_MONSTER_TOTALINFO*)pMsg;
	if(pmsg->TotalInfo.MonsterKind > 10000)
	{
		ASSERT(0);
	}
	
	BASEMOVE_INFO moveInfo;
	moveInfo.AddedMoveSpeed = 0.f;
	moveInfo.SetFrom(&pmsg->MoveInfo);
	CAddableInfoIterator iter(&pmsg->AddableInfo);
	BYTE AddInfoKind;
	while((AddInfoKind = iter.GetInfoKind()) != CAddableInfoList::None)
	{
		ySWITCH(AddInfoKind)
			yCASE(CAddableInfoList::MoveInfo)
				COMPRESSEDTARGETPOS tpos;
				iter.GetInfoData(&tpos);
				moveInfo.bMoving = TRUE;
				moveInfo.SetMaxTargetPosIdx(tpos.PosNum);
				ASSERT( tpos.PosNum <= MAX_CHARTARGETPOSBUF_SIZE );
				moveInfo.SetCurTargetPosIdx(0);
				for(BYTE n=0;n<tpos.PosNum;++n)
					tpos.pos[n].Decompress(moveInfo.GetTargetPosition(n));
			yCASE(CAddableInfoList::ShowdownInfo)
			yCASE(CAddableInfoList::StreetStall)
		yENDSWITCH
		
		iter.ShiftToNextData();
	}
	
#ifdef TAIWAN_LOCAL
	//�ӽ÷�! �����̸��� �������� �Ⱥ����ֵ��� ����!
	SafeStrCpy( pmsg->BaseObjectInfo.ObjectName,
		GAMERESRCMNGR->GetMonsterListInfo( pmsg->TotalInfo.MonsterKind )->Name,
		MAX_NAME_LENGTH+1 );
#endif
	
	CBossMonster* pMon = OBJECTMGR->AddBossMonster(&pmsg->BaseObjectInfo,&moveInfo,&pmsg->TotalInfo);
						
	//////////////////////////////////////////////////////////////////////////

	if(pmsg->bLogin)
	{
		TARGETSET set;
		set.pTarget = pMon;

		switch( pMon->GetMonsterKind() )
		{
		case eBOSSKIND_TARINTUS:
			{
				// 080108 KTH -- ���� �ִϸ��̼��� ������ �ش�.
				EFFECTMGR->StartEffectProcess(
					FindEffectNum("BOSS_Tarintus_Regen.beff"), 
					pMon, NULL, 0, pMon->GetID());
				// 080109 KTH -- ������Ʈ�� ���õ��� �ʵ��� �����Ѵ�.
				pMon->GetEngineObject()->DisablePick();
				// 080109 KTH -- ������ ������ �ð��� ���� �ش�.
				pMon->SetOpenningTime(8000);
				break;
			}
			break;
		case eBOSSKIND_LEOSTEIN:
			{
				EFFECTMGR->StartEffectProcess(
					FindEffectNum("BOSS_LeoStein_Regen.beff"), 
					pMon, NULL, 0, pMon->GetID());
				pMon->GetEngineObject()->DisablePick();
				pMon->SetOpenningTime(3000);
			}
			break;
		case eBOSSKIND_DRAGONIAN:
			{
				EFFECTMGR->StartEffectProcess(
					9507, 
					pMon, NULL, 0, pMon->GetID());
				pMon->GetEngineObject()->DisablePick();
				pMon->SetOpenningTime(7000);
			}
			break;
		default:
			EFFECTMGR->StartEffectProcess(eEffect_NewCharacter,pMon,&set,0,pMon->GetID());
			break;
		}
	}
}


void CGameIn::UserConn_ChangeMap_Ack(void* pMsg )
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	DWORD map = pmsg->dwData;
	WINDOWMGR->MsgBox( MBI_NOBTNMSGBOX, MBT_NOBTN, RESRCMGR->GetMsg( 290 ) );
	MAPCHANGE->SetGameInInitKind(eGameInInitKind_MapChange);
	MAINGAME->SetGameState(eGAMESTATE_MAPCHANGE,&map,4);
	// 070623 LYW --- GameIn : SaveChatList.
	CHATMGR->SaveChatList() ;

	// 080411 LYW --- GameIn : ä�ù濡�� ä�����̾��ٸ�, ä�ó����� �����Ѵ�.
	CHATROOMMGR->SaveChat() ;

	// �δ��ʿ��� �ٸ������� �̵��ϸ� �δ������� ������.
	MAPTYPE mapNum = MAP->GetMapNum();
	if(DungeonMGR->IsDungeonMap(mapNum))
	{
		DungeonMGR->ClearCurDungeon();
	}
}

// 070918 LYW --- GameIn : Add function to change map through the npc.
void CGameIn::UserConn_Npc_ChangeMap_Ack(void* pMsg)
{
	if( !pMsg ) return ;

	cNpcScriptDialog* pDlg = GAMEIN->GetNpcScriptDialog() ;
	if( !pDlg ) return ;

	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg ;

	DWORD dwMapNum = pmsg->dwData ;

	WINDOWMGR->MsgBox( MBI_NOBTNMSGBOX, MBT_NOBTN, RESRCMGR->GetMsg( 290 ) );

	MAPCHANGE->SetGameInInitKind(eGameInInitKind_MapChange);
	MAINGAME->SetGameState(eGAMESTATE_MAPCHANGE,&dwMapNum,4);

	pDlg->EndDialog() ;
	pDlg->SetActive(FALSE) ;

	CHATMGR->SaveChatList() ;

	// 080411 LYW --- GameIn : ä�ù濡�� ä�����̾��ٸ�, ä�ó����� �����Ѵ�.
	CHATROOMMGR->SaveChat() ;
}


void CGameIn::UserConn_Cheat_ChangeMap_Ack(void* pMsg )
{
	MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
	DWORD map = pmsg->dwData1;

//---KES ä���̵� ����
	gChannelNum = pmsg->dwData2;
	gCheatMove = TRUE;
//--------------------
    

	// 071208 LYW --- GameIn : �� �̵� ��ũ�� ����, �� �̵� �ϸ鼭 �������� ������ ������, 
	// �޽��� ��ȣ�� 700 -> 206���� �����Ѵ�.
	WINDOWMGR->MsgBox( MBI_NOBTNMSGBOX, MBT_NOBTN, CHATMGR->GetChatMsg( 206 ) );
	
	MAPCHANGE->SetGameInInitKind(eGameInInitKind_MapChange);
	MAINGAME->SetGameState(eGAMESTATE_MAPCHANGE,&map,4);
	CHATMGR->SaveChatList();

	// 080411 LYW --- GameIn : ä�ù濡�� ä�����̾��ٸ�, ä�ó����� �����Ѵ�.
	CHATROOMMGR->SaveChat() ;
}


void CGameIn::UserConn_ChangeMap_Nack()
{
	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(388) );
	g_UserInput.SetInputFocus(TRUE);
}

// 090512 ONS Ÿ������ �ű����� ���۸������� �̵� ���� - �޼�����ȣ ���� ���.
void CGameIn::UserConn_DevilMarket_ChangeMap_Nack()
{
	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(388) );
	g_UserInput.SetInputFocus(TRUE);
}

void CGameIn::UserConn_Npc_ChangeMap_Nack()
{
	WINDOWMGR->MsgBox( MBI_FAILED_NPC_CHANGE_MAP, MBT_OK, CHATMGR->GetChatMsg( 56 ) );
	g_UserInput.SetInputFocus(TRUE);

	cNpcScriptDialog* pDlg = GAMEIN->GetNpcScriptDialog() ;

	if( !pDlg ) return ;

	pDlg->EndDialog() ;
	pDlg->SetActive(FALSE) ;
}


void CGameIn::UserConn_CharacterList_Ack(void* pMsg )
{
	SEND_CHARSELECT_INFO* pmsg = (SEND_CHARSELECT_INFO*)pMsg;
	MAPCHANGE->SetGameInInitKind( eGameInInitKind_Login );
	MAINGAME->SetGameState(eGAMESTATE_CHARSELECT, (void *)pmsg, sizeof(SEND_CHARSELECT_INFO));
	//ITEMMGR->DeleteShopItemInfo();
}


void CGameIn::UserConn_BackToCharSel_Nack()
{
	WINDOWMGR->AddListDestroyWindow( WINDOWMGR->GetWindowForID( MBI_NOBTNMSGBOX ) );
	OBJECTSTATEMGR->EndObjectState( HERO, eObjectState_Enter );//AO��A
}


void CGameIn::UserConn_MapDesc( void* pMsg )
{
	MSG_WORD3* pmsg = (MSG_WORD3*)pMsg;
	MAP->SetVillage( pmsg->wData1 );
	PKMGR->SetPKAllow( pmsg->wData2 );
	gChannelNum = (int)pmsg->wData3;

	if (g_csDateManager.IsChallengeZone(MAP->GetMapNum()))
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1221 ) );
		return;
	}

#ifdef TAIWAN_LOCAL					
	if( MAP->GetMapNum() == EVENTMAPNUM || MAP->GetMapNum() == Tournament || 
		MAP->GetMapNum() == QuestRoom )
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 394 ), GetMapName( MAP->GetMapNum() ) );
	else
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 384 ), gChannelNum+1, GetMapName( MAP->GetMapNum() ) );
	}
#else
	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 384 ), GetMapName( MAP->GetMapNum() ), gChannelNum+1 );
#endif

	if( PKMGR->IsPKAllow() )
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 383 ) );
	}

	// 06. 03. ���İ��� - �̿���
//	if(HERO->GetGuildIdx() && GUILDMGR->GetGuildNotice())
//		CHATMGR->AddMsg(CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(395), GUILDMGR->GetGuildNotice());
}


void CGameIn::UserConn_SetVisible( void* pMsg )
{
	MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
	CObject* pObj = OBJECTMGR->GetObject(pmsg->dwData1);
	if( !pObj ) // AI��I������������ NULL�ƨ� AuA��(E��AI��a!)
	{
		ASSERT(0);
		return;
	}
	
	if(pObj->GetObjectKind() != eObjectKind_Player)
	{
		ASSERT(0);
		return;
	}

	CPlayer* ppla = (CPlayer*)pObj;
	if(pmsg->dwData2)
	{
#ifdef _GMTOOL_
		if( MAINGAME->GetUserLevel() <= eUSERLEVEL_GM )
			ppla->GetEngineObject()->SetAlpha( 1.0f );
		else
#endif						
			ppla->GetEngineObject()->Show();
		
		ppla->GetCharacterTotalInfo()->bVisible = true;
	}
	else
	{
#ifdef _GMTOOL_
		if( MAINGAME->GetUserLevel() <= eUSERLEVEL_GM )
			ppla->GetEngineObject()->SetAlpha( 0.3f );
		else
#endif						
			ppla->GetEngineObject()->HideWithScheduling();

		ppla->GetCharacterTotalInfo()->bVisible = false;
	}
}


void CGameIn::NPC_NetworkMsgParse(BYTE Protocol,void* pMsg)
{
	switch(Protocol)
	{
	case MP_NPC_SPEECH_ACK:
		{
			Npc_Speech_Ack( pMsg ) ;
			// 071021 LYW --- GameIn : Check tutorial condition.
			TUTORIALMGR->Check_NpcTalk(pMsg) ;								// ��ȭ�� ������ npc�� üũ�ϱ� ���� �Լ�.
		}
		break;
	case MP_NPC_SPEECH_NACK:
		{
			Npc_Speech_Nack( pMsg ) ;
		}
		break;
	case MP_NPC_CLOSEBOMUL_ACK:
		{
			Npc_CloseBomul_Ack() ;
		}
		break;
	case MP_NPC_DIE_ACK:
		{
			Npc_Die_Ack( pMsg ) ;
		}
		break;
		
	case MP_NPC_DOJOB_NACK:
		{
			Npc_DoJob_Nack() ;			
		}
		break;

	case MP_NPC_CHECK_HACK_ACK:
		{
			MSG_DWORD3* pmsg = (MSG_DWORD3*)pMsg;
			NPCSCRIPTMGR->StartNpcBusiness(pmsg->dwData3);
		}
		break;
	case MP_NPC_CHECK_HACK_NACK:
		{
			if( HERO->GetState() == eObjectState_Deal )
				OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal);

			MSG_DWORD *pmsg = (MSG_DWORD*)pMsg;
			if(pmsg->dwData == 271) // bomul
			{
				NPCSCRIPTMGR->SetSelectedNpc(NULL);
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(391) );
			}

			// 081009 LYW --- GameIn : ���� �ҽ� ����
			if(pmsg->dwData == 135 || pmsg->dwData == 136)
			{
				GAMEIN->GetNpcScriptDialog()->EndDialog();
				GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);
				NPCSCRIPTMGR->SetSelectedNpc(NULL);
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1705));
			}
		}
		break;

	// 090227 ShinJS --- �̵�NPC�� ���� ó��
	case MP_NPC_READYTOMOVE_NOTIFY:
		{
			// �̵�NPC �̵��غ� �޼���
			MSG_WORD* pmsg = (MSG_WORD*)pMsg;

			CNpc* pNpc = (CNpc*)OBJECTMGR->GetObject( STATICNPC_ID_START + pmsg->wData );
			// �ش� NPC ID�� �����ϴ°�� �̵��غ� ����
			if( pNpc )
				pNpc->SetReadyToMove( TRUE );

			// NPC �̸� ���ϱ�
			char* strNPCName = "";
			if( pNpc )
			{
				strNPCName = pNpc->GetObjectName();
			}
			else
			{
				STATIC_NPCINFO* pStaticNpcInfo = GAMERESRCMNGR->GetMoveNpcInfo( pmsg->wData );
				if( pStaticNpcInfo )
					strNPCName = pStaticNpcInfo->NpcName;
			}

			CDealDialog* pDealDlg = GAMEIN->GetDealDialog();
			cNpcScriptDialog* pNpcScriptDlg	= GAMEIN->GetNpcScriptDialog();

			// DealDialog�� Ȱ��ȭ �Ǿ� �ְ� ������ �̵�NPC�� ��� �۾�â�� �ݴ´�.
			if( pDealDlg )
			if( pDealDlg->IsActive() && pDealDlg->GetDealerIdx() == pmsg->wData )
			{
				cDialog* pBuyDivideBox	= WINDOWMGR->GetWindowForID( DBOX_BUY );			// ���� �춧 ���� �Է�â
				cDialog* pSellDivideBox	= WINDOWMGR->GetWindowForID( DBOX_SELL );			// ���� �ȶ� ���� �Է�â
				cDialog* pBuyMsgBox		= WINDOWMGR->GetWindowForID( MBI_BUYITEM );			// ���� �춧 MSG BOX
				cDialog* pSellMsgBox	= WINDOWMGR->GetWindowForID( MBI_SELLITEM );		// ���� �ȶ� MSG BOX
				

				if( pBuyDivideBox )
				{
					pBuyDivideBox->SetActive( FALSE );
					WINDOWMGR->AddListDestroyWindow( pBuyDivideBox );
				}
				if( pSellDivideBox )
				{
					pSellDivideBox->SetActive( FALSE );
					WINDOWMGR->AddListDestroyWindow( pSellDivideBox );
				}
				if( pBuyMsgBox )
				{
					pBuyMsgBox->SetActive( FALSE );
					WINDOWMGR->AddListDestroyWindow( pBuyMsgBox );
				}
				if( pSellMsgBox )
				{
					pSellMsgBox->SetActive( FALSE );
					WINDOWMGR->AddListDestroyWindow( pSellMsgBox );
				}

				((cDialog*)GAMEIN->GetInventoryDialog())->SetDisable( FALSE );
				((cDialog*)GAMEIN->GetInventoryDialog())->SetActive( FALSE );				// �κ��丮�� �ݴ´�
				pDealDlg->SetDisable( FALSE );
				pDealDlg->SetActive( FALSE );												// �Ǹ�â�� �ݴ´�

				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1910), strNPCName );		// �̵��غ��۾����� ���� �ŷ��ߴܸ޼����� ����Ѵ�
			}

			// ScriptDialog�� Ȱ��ȭ �Ǿ� �ְ� �̵�NPC�� ��� �ݴ´�.
			if( pNpcScriptDlg )
			if( pNpcScriptDlg->IsActive() && pNpcScriptDlg->GetCurNpcIdx() == pmsg->wData)
			{
				pNpcScriptDlg->SetActive( FALSE );											// NPC Script â�� �ݴ´�

				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1910), strNPCName );		// �̵��غ��۾����� ���� �ŷ��ߴܸ޼����� ����Ѵ�
			}

		}
		break;

	case MP_NPC_REMOVE_MOVENPC_SYN:
		{
			// �̵�NPC ����
			MSG_WORD* pmsg = (MSG_WORD*)pMsg;

			// ���õǾ� �ִ� NPC�� ������ �̵�NPC�� ���
			if( OBJECTMGR->GetSelectedObjectID() == STATICNPC_ID_START + pmsg->wData )
			{
				CObject* pObject = OBJECTMGR->GetSelectedObject();
				if( pObject )
				{
					pObject->OnDeselected();
					pObject->ShowObjectName( FALSE, NAMECOLOR_DEFAULT );		// ������Ʈ �̸��� ����Ʈ �������� �����Ѵ�.
				}
				OBJECTMGR->SetSelectedObject( NULL ) ;							// ������Ʈ ������ ���� �Ѵ�.
			}

			
			OBJECTMGR->RemoveNPC( STATICNPC_ID_START + pmsg->wData );			// �̵�NPC�� �����Ѵ�
		}
		break;

	case MP_NPC_ADD_MOVENPC_SYN:
		{
			// �̵�NPC �߰�
			MSG_DWORD4* pmsg = (MSG_DWORD4*)pMsg;

			WORD wMapNum		= LOWORD( pmsg->dwData1 );						// NPC Map ��ȣ
			WORD wDir			= HIWORD( pmsg->dwData1 );						// NPC ����
			WORD wNpcIdx		= LOWORD( pmsg->dwData2 );						// NPC Index
			WORD wNpcKindIdx	= HIWORD( pmsg->dwData2 );						// NPC ����
			WORD wXPos			= LOWORD( pmsg->dwData3 );						// ��ġ
			WORD wZPos			= HIWORD( pmsg->dwData3 );
			BOOL bReadyToMove	= BOOL( pmsg->dwData4 );

			CNpc* pNpc = (CNpc*)OBJECTMGR->GetObject( STATICNPC_ID_START + wNpcIdx );
			// �ش� NPC ID�� �����ϴ°�� (����׽� �ѹ��� �������� �߰������͸� ���� ���)
			if( pNpc )
			{
				BASEMOVE_INFO* pminfo = pNpc->GetBaseMoveInfo();
				if( !pminfo )	break;

				pminfo->CurPosition.x = wXPos;										// �̵������� ��ġ�� �����Ѵ�.
				pminfo->CurPosition.z = wZPos;

				pNpc->SetAngle( DEGTORAD( float(wDir) ) );							// ������ �����Ѵ�.
				pNpc->SetReadyToMove( FALSE );										// �̵��� �غ��۾� �ʱ�ȭ

				MOVEMGR->InitMove( pNpc, pminfo );
			}
			else
			{
				// �̵�NPC �߰�
				NPC_TOTALINFO tinfo;												// npc ��Ż ������ ���� ����ü�� �����Ѵ�.
				BASEOBJECT_INFO binfo;												// �⺻ ������Ʈ ������ ���� ����ü�� �����Ѵ�.
				BASEMOVE_INFO minfo;												// �⺻ �̵� ������ ���� ����ü�� �����Ѵ�.

				binfo.dwObjectID = STATICNPC_ID_START + wNpcIdx;
				binfo.ObjectState = eObjectState_None;
				tinfo.MapNum = wMapNum;
				minfo.bMoving = FALSE;
				minfo.KyungGongIdx = 0;
				minfo.MoveMode = eMoveMode_Run;
				tinfo.NpcKind = wNpcKindIdx;										// ��Ż ������ npc ������ �����Ѵ�.

				// �⺻ ������ npc �̸��� �����Ѵ�.
                STATIC_NPCINFO* pStaticNpcInfo = GAMERESRCMNGR->GetMoveNpcInfo( wNpcIdx );
				if( pStaticNpcInfo )
					SafeStrCpy( binfo.ObjectName, pStaticNpcInfo->NpcName, MAX_NPC_NAME_LENGTH+1 );
				else
					binfo.ObjectName[0] = 0;

				tinfo.NpcUniqueIdx = wNpcIdx;										// ��Ż ������ npc ����ũ �ε����� �����Ѵ�.

				NPC_LIST* pInfo = GAMERESRCMNGR->GetNpcInfo( tinfo.NpcKind );		// npc ������ �����Ͽ� npc ����Ʈ ������ �޴´�.
				if(pInfo)															// npc ����Ʈ ������ ��ȿ�ϸ�,
				{
					tinfo.NpcJob = pInfo->JobKind;									// ��Ż ������ npc ������ �����Ѵ�.
				}
				else																// npc ����Ʈ ������ ��ȿ���� ������,
				{
					tinfo.NpcJob = 0;												// ������ 0���� �����Ѵ�.
				}

				//jop�� 0�̸� �̸��� ������.
				if( tinfo.NpcJob == 0 )												// ������ 0�̸�,
				{
					binfo.ObjectName[0] = 0;										// �̸��� �����.
				}

				minfo.CurPosition.x = wXPos;										// �̵������� ��ġ�� �����Ѵ�.
				minfo.CurPosition.y = 0;
				minfo.CurPosition.z = wZPos;

				pNpc = OBJECTMGR->AddNpc(&binfo,&minfo,&tinfo);						// NPC �߰�

				pNpc->SetAngle( DEGTORAD( float(wDir) ) );							// ������ �����Ѵ�.
			}

			pNpc->SetReadyToMove( bReadyToMove );									// �̵��غ���� ����

			TARGETSET set;
			set.pTarget = pNpc;
			EFFECTMGR->StartEffectProcess(eEffect_NewCharacter,pNpc,&set,0,pNpc->GetID());	// ĳ���� ���� ����Ʈ�� �����Ѵ�
		}
		break;
	case MP_NPC_PUT_CHAT_ACK:
		{
			MSG_NAME_DWORD2* const message = (MSG_NAME_DWORD2*)pMsg;
			LPCTSTR fileName = message->Name;
			const DWORD textIndex = message->dwData2;

			cTutorialDlg* const dialog = (cTutorialDlg*)WINDOWMGR->GetWindowForID(TUTORIAL_DLG);

			if(0 == dialog)
			{
				break;
			}

			dialog->SetNpcImage(fileName);
			dialog->SetText(
				CHATMGR->GetChatMsg(textIndex));
			dialog->SetInvisibleSecond(5);
			dialog->SetActive(TRUE);
		}
		break;
	}
}


void CGameIn::Npc_Speech_Ack( void* pMsg )
{
	MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;

	CNpc* pNpc = (CNpc*)OBJECTMGR->GetObject(pmsg->dwData1);

	if( !pNpc )
	{
		ASSERT(0);
		if( HERO->GetState() == eObjectState_Deal )
			OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal);
		return;
	}


	//090604 pdy �Ͽ�¡ NPC��ũ �̵� �߰� ��ȭâ�̾ƴ� �˾�â�� �ߴ� Ư�� ������ ���� ó���ϰ� ���� 
	char szBuf[128] = {0,};
	switch( pNpc->GetNpcJob() )
	{
		case HOUSING_RANK1_ROLE:
		case HOUSING_RANK2_ROLE:
		case HOUSING_RANK3_ROLE:
			{
				// 091019 pdy �Ͽ�¡�ʿ��� �ε��߿� ���� �Ͽ�¡ ��� ������ �߰�
				if( HOUSINGMGR->IsHousingMap() &&	
					HOUSINGMGR->IsHouseInfoLoadOk() == FALSE )
				{
					return;
				}

				//090527 pdy �Ͽ�¡ �˾�â [��Ŀ�� ��ũ����]
				BASEOBJECT_INFO NpcBaseInfo;
				pNpc->GetBaseObjectInfo(&NpcBaseInfo);

				cMsgBox* pBox = WINDOWMGR->MsgBox( MBI_HOUSE_VISIT_LINK_AREYOUSURE, MBT_YESNO, CHATMGR->GetChatMsg(1903) , NpcBaseInfo.ObjectName );	//1903	"%s ��Ŀ ������ �̵��Ͻðڽ��ϱ�?"
				
				if( pBox )
					pBox->SetParam( pNpc->GetID() );

				OBJECTSTATEMGR->StartObjectState(HERO, eObjectState_Deal);
			}
			return;
		case HOUSING_PORTAL_ROLE:
			{
				// 091019 pdy �Ͽ�¡�ʿ��� �ε��߿� ���� �Ͽ�¡ ��� ������ �߰�
				if( HOUSINGMGR->IsHousingMap() &&	
					HOUSINGMGR->IsHouseInfoLoadOk() == FALSE )
				{
					return;
				}

				sprintf(szBuf,CHATMGR->GetChatMsg(1890), pNpc->GetNpcTotalInfo()->szSummonerName );	//1890	"%s�� ������ �̵� �Ͻðڽ��ϱ�?"

				//090604 pdy �Ͽ�¡ �˾�â [��ȯ��ŻNPC ��ũ����]
				cMsgBox* pBox = WINDOWMGR->MsgBox( MBI_HOUSE_VISIT_LINK_AREYOUSURE, MBT_YESNO, szBuf );
				if( pBox )
					pBox->SetParam( pNpc->GetID() );

				OBJECTSTATEMGR->StartObjectState(HERO, eObjectState_Deal);
			}
			return;

		case NOTE_ROLE:
			{
				cDialog* dialog = WINDOWMGR->GetWindowForID( NOTE_NOTEDLG );
				ASSERT( dialog );

				if( dialog->IsActive() )
				{
					dialog->SetActive( FALSE );
				}
				// ���� â�� ������ �ϴ� ������ ����� ��û�ؾ��Ѵ�.
				else
				{
					NOTEMGR->NoteListSyn( 1, eMode_NormalNote );
				}

				OBJECTSTATEMGR->StartObjectState(HERO, eObjectState_Deal);

				/*
				// ��ü���� ĳ���������� ������ ��������.
				VECTOR3 pos = {0};
				HERO->GetPosition(&pos);
				MOVEMGR->SetLookatPos(pNpc, &pos, 0, gCurTime);
				*/
			}
			return;

		case CONSIGNMENT_ROLE:
			{
				CConsignmentDlg* pDlg = GAMEIN->GetConsignmentDlg();
				if( pDlg && !pDlg->IsActive())
				{
					pDlg->SetActive(TRUE);
					pDlg->SetMode(CConsignmentDlg::eConsignment_Mode_Buy);
				}
				else
				{
					pDlg->SetActive(FALSE);
				}

				OBJECTSTATEMGR->StartObjectState(HERO, eObjectState_Deal);

				VECTOR3 pos = {0};
				HERO->GetPosition(&pos);
				MOVEMGR->SetLookatPos(pNpc, &pos, 0, gCurTime);
			}
			return;
	}


	switch( pNpc->GetNpcKind() )
	{
	case 30:
		{
			cMsgBox* pBox = WINDOWMGR->MsgBox( MBI_SELECTLOGINPOINT, MBT_YESNO, CHATMGR->GetChatMsg(381), pNpc->GetObjectName() );
			if( pBox )
				pBox->SetParam( pNpc->GetID() );
		}
		break;
	default:
		{
			NPCSCRIPTMGR->StartNpcScript( pNpc );
		}
		break;
	}
}


void CGameIn::Npc_Speech_Nack( void* pMsg )
{
	if( HERO->GetState() == eObjectState_Deal )
		OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal);

	MSG_DWORD *pmsg = (MSG_DWORD*)pMsg;
	if(pmsg->dwData == 271) // bomul
	{
		NPCSCRIPTMGR->SetSelectedNpc(NULL);
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(391) );
	}
}


void CGameIn::Npc_CloseBomul_Ack()
{
	if(GetNpcScriptDialog()->IsActive())
	{
		NPCSCRIPTMGR->SetSelectedNpc(NULL);
		GetNpcScriptDialog()->SetActive(FALSE);						
	}
}


void CGameIn::Npc_Die_Ack( void* pMsg )
{
	MSG_DWORD * pmsg = (MSG_DWORD*)pMsg;
	CNpc* pNpc = NPCSCRIPTMGR->GetSelectedNpc();
	if(pNpc)
	{
		if(pNpc->GetID() == pmsg->dwData)
		{
			if(GetNpcScriptDialog()->IsActive())
			{
				NPCSCRIPTMGR->SetSelectedNpc(NULL);
				GetNpcScriptDialog()->SetActive(FALSE);						
			}
		}
	}
}


void CGameIn::Npc_DoJob_Nack()
{
	NPCSCRIPTMGR->SetSelectedNpc(NULL);
	GetNpcScriptDialog()->SetActive(FALSE);						
	if( HERO->GetState() == eObjectState_Deal )
		OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal);
}


void CGameIn::SIGNAL_NetworkMsgParse(BYTE Protocol,void* pMsg)
{
	switch(Protocol)
	{
	case MP_SIGNAL_COMMONUSER:
		{
			Signal_CommonUser( pMsg ) ;
		}
		break;
	case MP_SIGNAL_USER_ADD_ITEM:
		{
			Signal_UserAddItem( pMsg ) ;
		}
		break;
	case MP_SIGNAL_SHOWDOWN_RESULT:	//AO��A
		{
			Signal_ShowDown_Result( pMsg ) ;
		}
		break;				
	}
}


void CGameIn::Signal_CommonUser( void* pMsg )
{
	TESTMSG* const data = (TESTMSG*)pMsg;
	LPCTSTR keyword = _T("SystemMsg.bin\'");
	// 091019 LUJ, ���ڿ��� ������ ������ ���۵Ǹ� �ý��� �޽����� �����Ѵ�
	const BOOL isSystemMessage = (0 == _tcsnicmp(data->Msg, keyword, _tcslen(keyword)));

	if(isSystemMessage)
	{
		LPCTSTR token = data->Msg + _tcslen(keyword);
		const DWORD messageIndex = _ttoi(token ? token : "");
		CHATMGR->AddMsg(
			CTC_OPERATOR,
			CHATMGR->GetChatMsg(messageIndex));
	}
	else
	{
		CHATMGR->AddMsg(
			CTC_OPERATOR,
			data->Msg);
	}
}
// 081114 KTH -- ������ ȹ���� �˸�
void CGameIn::Signal_UserAddItem( void* pMsg )
{
	MSG_USER_ADD_ITEM* msg = (MSG_USER_ADD_ITEM*)pMsg;
	char MessageBuff[MAX_PATH] = { 0, };

	ITEM_INFO* pItemInfo = ITEMMGR->GetItemInfo(msg->dwIconIdx);
	char	szItemName[33];
	_parsingKeywordString(pItemInfo->ItemName, szItemName);

	//sprintf(MessageBuff, "[%s]���� [%s] �������� ȹ�� �Ͽ����ϴ�.", msg->CharacterName, szItemName);
	wsprintf(MessageBuff, CHATMGR->GetChatMsg(1267), msg->CharacterName, szItemName);

	CHATMGR->AddMsg( CTC_OPERATOR, MessageBuff );
}

void CGameIn::Signal_ShowDown_Result( void* pMsg )
{
	MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
	CObject* pWinner = OBJECTMGR->GetObject(pmsg->dwData1);
	CObject* pLoser = OBJECTMGR->GetObject(pmsg->dwData2);
	if( pWinner == NULL || pLoser == NULL ) return;
	
	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(389),
	pWinner->GetObjectName(), pLoser->GetObjectName(), pWinner->GetObjectName() );
}

void CGameIn::UserConn_Pet_Add( void* pMsg )
{
	MSG_PET_ADD* pmsg = (MSG_PET_ADD*)pMsg;

	BASEMOVE_INFO moveInfo;
	moveInfo.AddedMoveSpeed = 0.f;
	moveInfo.SetFrom(&pmsg->MoveInfo);
	CAddableInfoIterator iter(&pmsg->AddableInfo);
	BYTE AddInfoKind;
	while((AddInfoKind = iter.GetInfoKind()) != CAddableInfoList::None)
	{
		ySWITCH(AddInfoKind)
			yCASE(CAddableInfoList::MoveInfo)
			COMPRESSEDTARGETPOS tpos;
		iter.GetInfoData(&tpos);
		moveInfo.bMoving = TRUE;
		moveInfo.SetMaxTargetPosIdx(tpos.PosNum);
		ASSERT( tpos.PosNum <= MAX_CHARTARGETPOSBUF_SIZE );
		moveInfo.SetCurTargetPosIdx(0);
		for(BYTE n=0;n<tpos.PosNum;++n)
			tpos.pos[n].Decompress(moveInfo.GetTargetPosition(n));
		yENDSWITCH

			iter.ShiftToNextData();
	}

	CPet* pPet = NULL;

	if(pmsg->MasterIdx == gHeroID)
	{
		if( pmsg->IsLogin ||
			0 == HEROPET)
		{
			// 091214 ONS �� ���̵��� ��ȯ���� �޼����� ������� �ʵ��� ó��.
			if(pmsg->IsSummoned)
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1574) );
			}
			pPet = ( CPet* )OBJECTMGR->AddHeroPet( &( pmsg->BaseObjectInfo ), &moveInfo, &( pmsg->PetObjectInfo ), pmsg->ItemInfo );
		}
	}
	else
	{
		pPet = OBJECTMGR->AddPet( pmsg->MasterIdx, &( pmsg->BaseObjectInfo ), &moveInfo, &( pmsg->PetObjectInfo ), pmsg->ItemInfo );
	}

	// 091214 ONS �� ���̵��� ����Ʈ�� ������� �ʵ��� ó��.
	if( pmsg->IsLogin && pmsg->IsSummoned )
	{
		TARGETSET set;
		set.pTarget = pPet;
		EFFECTMGR->StartEffectProcess(PETMGR->GetSummonEffect(),pPet,&set,0,pPet->GetID());
	}
}

void CGameIn::UserConn_HeroPet_Remove( void* pMsg )
{
	MSG_DWORD_WORD* pmsg = (MSG_DWORD_WORD*)pMsg;

	CObject* pPet = OBJECTMGR->GetObject(pmsg->dwData);
	CObject* pMaster = OBJECTMGR->GetObject(pmsg->dwObjectID);

	if( pPet == NULL )
	{
		return;
	}

	if( pMaster == HERO )
	{
		// 091214 ONS �� ���̵��� ���ΰ��� �޼����� ������� �ʵ��� ó��.
		if(pmsg->wData)
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1575) );
		}
		OBJECTMGR->RemoveHeroPet();
		return;
	}

	APPEARANCEMGR->CancelAlphaProcess( pPet );
	APPEARANCEMGR->CancelReservation( pPet );
	OBJECTMGR->AddGarbageObject( pPet );
}

void CGameIn::UserConn_Vehicle_Add( void* pMsg )
{
	SEND_VEHICLE_TOTALINFO* const message = ( SEND_VEHICLE_TOTALINFO* )pMsg;
	BASEMOVE_INFO moveInfo;
	ZeroMemory( &moveInfo, sizeof(BASEMOVE_INFO) );
	moveInfo.SetFrom(&message->MoveInfo);
	CAddableInfoIterator iter(&message->AddableInfo);
	BYTE AddInfoKind;
	while((AddInfoKind = iter.GetInfoKind()) != CAddableInfoList::None)
	{
		ySWITCH(AddInfoKind)
			yCASE(CAddableInfoList::MoveInfo)
				COMPRESSEDTARGETPOS tpos;
				iter.GetInfoData(&tpos);
				moveInfo.bMoving = TRUE;
				moveInfo.SetMaxTargetPosIdx(tpos.PosNum);
				ASSERT( tpos.PosNum <= MAX_CHARTARGETPOSBUF_SIZE );
				moveInfo.SetCurTargetPosIdx(0);
				for(BYTE n=0;n<tpos.PosNum;++n)
					tpos.pos[n].Decompress(moveInfo.GetTargetPosition(n));
		yENDSWITCH

		iter.ShiftToNextData();
	}

	CVehicle* pVehicle = OBJECTMGR->AddVehicle( &message->BaseObjectInfo, &moveInfo, &message->TotalInfo );

	if(0 == pVehicle)
	{
		return;
	}

	pVehicle->SetInstall( TRUE );
	pVehicle->SetMasterName( message->MasterName );

	const ITEMBASE* const pItemBase = ITEMMGR->GetItemInfoAbsIn( 0, message->usedItemPosition );
	if( pItemBase != NULL )
	{
		ICONBASE usedItem = { 0 };
		usedItem.wIconIdx = pItemBase->wIconIdx;
		usedItem.Position = message->usedItemPosition;
		usedItem.dwDBIdx = pItemBase->dwDBIdx;

		pVehicle->SetUsedItem( usedItem );
	}

	// 090316 LUJ, ž�� ó��
	{
		const DWORD seatMaxSize = sizeof( message->mSeat ) / sizeof( *( message->mSeat ) );

		for( DWORD seatIndex = 0; seatIndex < seatMaxSize; ++seatIndex )
		{
			const DWORD playerIndex = message->mSeat[ seatIndex ];
			CPlayer* const player = ( CPlayer* )OBJECTMGR->GetObject( playerIndex );

			if( 0 == player || player->GetObjectKind() != eObjectKind_Player )
			{
				continue;
			}

			pVehicle->GetOn( player, seatIndex + 1 );
		}
	}

	if(message->bLogin)
	{
		TARGETSET set;
		set.pTarget = pVehicle;
		EFFECTMGR->StartEffectProcess(eEffect_MonRegen,pVehicle,&set,0,pVehicle->GetID());
	}
}

void CGameIn::UserConn_Vehicle_Remove( void* pMsg )
{
	const MSG_DWORD2* const message = ( MSG_DWORD2* )pMsg;
	const DWORD playerIndex = message->dwData1;
	const DWORD vehicleIndex = message->dwData2;

	VEHICLEMGR->UnSummon( playerIndex, vehicleIndex );
	UserConn_Monster_Die( pMsg );
}

#ifdef _TESTCLIENT_

void InitTestHero()
{
	DWORD id = 1;
	{	// AOAI�Ʃ�
		SEND_HERO_TOTALINFO info;
		memset(&info,0,sizeof(info));
		info.BaseObjectInfo.dwObjectID = id++;
		strcpy( info.BaseObjectInfo.ObjectName, "DARK");
		VECTOR3 pos;
		pos.x = GAMERESRCMNGR->m_TestClientInfo.x;
		pos.z = GAMERESRCMNGR->m_TestClientInfo.z;
		info.SendMoveInfo.CurPos.Compress(&pos);
		info.ChrTotalInfo.CurMapNum = GAMERESRCMNGR->m_TestClientInfo.Map;
		info.ChrTotalInfo.CurMapNum = GAMERESRCMNGR->m_TestClientInfo.Map;
		info.ChrTotalInfo.Race = GAMERESRCMNGR->m_TestClientInfo.Race;
		info.ChrTotalInfo.Gender = GAMERESRCMNGR->m_TestClientInfo.Gender;
		info.ChrTotalInfo.JobGrade = 1;
		info.ChrTotalInfo.Job[0] = GAMERESRCMNGR->m_TestClientInfo.Job;
		info.ChrTotalInfo.Life = 1000;
		info.ChrTotalInfo.HairType = GAMERESRCMNGR->m_TestClientInfo.HairType;
		info.ChrTotalInfo.FaceType = GAMERESRCMNGR->m_TestClientInfo.FaceType;
		info.ChrTotalInfo.MaxLife = 1000;
		info.HeroTotalInfo.Mana = 1000;
		info.HeroTotalInfo.MaxMana = 1000;
		// 080626 LYW --- Hero : �ִ� ���� ���� ��ġ�� ���� ���� ���� Define ������ ������.
		//info.ChrTotalInfo.Level = 99;
		info.ChrTotalInfo.Level = MAX_CHARACTER_LEVEL_NUM;
		info.ChrTotalInfo.Height = 1;
		info.ChrTotalInfo.Width = 1;
		info.ChrTotalInfo.WearedItemIdx[eWearedItem_Weapon] = GAMERESRCMNGR->m_TestClientInfo.WeaponIdx;
		info.ChrTotalInfo.WearedItemIdx[eWearedItem_Dress] = GAMERESRCMNGR->m_TestClientInfo.DressIdx;
		info.ChrTotalInfo.WearedItemIdx[eWearedItem_Hat] = GAMERESRCMNGR->m_TestClientInfo.HatIdx;
		info.ChrTotalInfo.WearedItemIdx[eWearedItem_Shoes] = GAMERESRCMNGR->m_TestClientInfo.ShoesIdx;
		info.ChrTotalInfo.WearedItemIdx[eWearedItem_Shield] = GAMERESRCMNGR->m_TestClientInfo.ShieldIdx;
		info.ChrTotalInfo.bVisible = true;
		BASEOBJECT_INFO obinfo;
		BASEMOVE_INFO moinfo;

		memset( &obinfo, 0, sizeof(obinfo) );		
		memset( &moinfo, 0, sizeof(moinfo) );

		obinfo.dwObjectID = 100;
		obinfo.dwUserID = 5572;
		sprintf( obinfo.ObjectName, "pet-01" );
		obinfo.BattleID = gChannelNum+2;

		moinfo.CurPosition = pos;
		moinfo.CurPosition.x -= 150;
		moinfo.bMoving = true;

		OBJECTMGR->RegisterHero(&info);
		GAMEIN->GetMiniMapDialog()->AddHeroIcon(HERO);

		CObject * pObj = (CObject*)OBJECTMGR->GetHero();
		pObj->SetPosition(&pos);

		CAMERA->Init(pObj,30,0,1000);
		CAMERA->SetTargetAngleX(0,29.9f,0);

		// 070313 LYW --- GameIn : Initialize hero's eye.
		HERO->InitializeEye() ;

		VECTOR2 sidpos;
		sidpos.x = 140;
		sidpos.y = 0;
		STATUSICONDLG->Init(HERO,&sidpos,5);

	//	CPetBase* pPet = OBJECTMGR->AddPet( &obinfo, &moinfo );
	//	HERO->SetPet( pPet );
	}
}


void InitTestMonster()
{
	DWORD id = 10;
	float x,z;
	x = GAMERESRCMNGR->m_TestClientInfo.x + 25;
	z = GAMERESRCMNGR->m_TestClientInfo.z + 500 + 25;


	DWORD temp =0;
	for(int n=0;n<GAMERESRCMNGR->m_TestClientInfo.MonsterNum;++n)
	{
		SEND_MONSTER_TOTALINFO info;
		memset(&info,0,sizeof(info));
		info.BaseObjectInfo.dwObjectID = id++;
		info.BaseObjectInfo.BattleID = gChannelNum+1;
		sprintf(info.BaseObjectInfo.ObjectName,"Monster%02d",temp++);
		VECTOR3 pos;
		pos.x = x;
		pos.z = z;
		info.MoveInfo.CurPos.Compress(&pos);
		BASEMOVE_INFO bminfo;
		bminfo.SetFrom(&info.MoveInfo);
		info.TotalInfo.MonsterKind = GAMERESRCMNGR->m_TestClientInfo.MonsterKind;
		g_pMonster = OBJECTMGR->AddMonster(&info.BaseObjectInfo,&bminfo,&info.TotalInfo);
		
		x+= 100;
	}

	temp =0;
	for(n=0;n<GAMERESRCMNGR->m_TestClientInfo.BossMonsterNum;++n)
	{
		SEND_MONSTER_TOTALINFO info;
		memset(&info,0,sizeof(info));
		info.BaseObjectInfo.dwObjectID = id++;
		sprintf(info.BaseObjectInfo.ObjectName,"BossMonster%02d",temp++);
		VECTOR3 pos;
		pos.x = x;
		pos.z = z;
		info.MoveInfo.CurPos.Compress(&pos);
		BASEMOVE_INFO bminfo;
		bminfo.SetFrom(&info.MoveInfo);
		info.TotalInfo.MonsterKind = GAMERESRCMNGR->m_TestClientInfo.BossMonsterKind;
		g_pBossMonster = OBJECTMGR->AddBossMonster(&info.BaseObjectInfo,&bminfo,&info.TotalInfo);
	//	g_pBossMonster->LoadBossMonster(GAMERESRCMNGR->m_TestClientInfo.BossInfofile);
		
		x+= 100;
	}

	x = GAMERESRCMNGR->m_TestClientInfo.x + 25;
	z -= 200;
	int lastnp = 0;
	for(n=0;n<GAMERESRCMNGR->m_TestClientInfo.CharacterNum;++n)
	{
		SEND_CHARACTER_TOTALINFO cinfo;
		memset(&cinfo,0,sizeof(cinfo));
		cinfo.BaseObjectInfo.dwObjectID = id++;
		sprintf(cinfo.BaseObjectInfo.ObjectName,"Character%02d",temp++);
		VECTOR3 pos;
		pos.x = x;
		pos.z = z;
		cinfo.MoveInfo.CurPos.Compress(&pos);
		BASEMOVE_INFO bminfo;
		bminfo.SetFrom(&cinfo.MoveInfo);
		cinfo.TotalInfo.bVisible = 1;
		cinfo.TotalInfo.Height = 1;
		cinfo.TotalInfo.Width = 1;
		cinfo.TotalInfo.bVisible = 1;
		cinfo.TotalInfo.Gender = 1;
		cinfo.TotalInfo.WearedItemIdx[eWearedItem_Dress] = 0;
		cinfo.TotalInfo.WearedItemIdx[eWearedItem_Weapon] = 0;
		OBJECTMGR->AddPlayer(&cinfo.BaseObjectInfo,&bminfo,&cinfo.TotalInfo);
		
		x+= 100;
		if(n/50 != lastnp)
		{
			lastnp = n/50;
			x = GAMERESRCMNGR->m_TestClientInfo.x + 25;
			z -= 200;
		}
	}
}

//trustpak
void InitTestCharacters(void)
{	
	DWORD id = 1000;
	DWORD temp =0;

	//for(n=0;n<GAMERESRCMNGR->m_TestClientInfo.CharacterNum;++n)
	DWORD n = 0;
	for (n = 0; n < GAMERESRCMNGR->m_TestClientInfo.dwTestCharacterNo;n++)
	{
		SEND_CHARACTER_TOTALINFO cinfo;
		memset(&cinfo,0,sizeof(cinfo));
		cinfo.BaseObjectInfo.dwObjectID = id++;
		sprintf(cinfo.BaseObjectInfo.ObjectName,"Test Character%02d",temp++);
		
		VECTOR3 pos;
		pos.x = GAMERESRCMNGR->m_TestClientInfo.aTestCharacterInfo[n].fX;
		pos.y = GAMERESRCMNGR->m_TestClientInfo.aTestCharacterInfo[n].fY;
		pos.z = GAMERESRCMNGR->m_TestClientInfo.aTestCharacterInfo[n].fZ;
		cinfo.MoveInfo.CurPos.Compress(&pos);

		BASEMOVE_INFO bminfo;
		bminfo.SetFrom(&cinfo.MoveInfo);
		cinfo.TotalInfo.bVisible = 1;
		cinfo.TotalInfo.Height = 1;
		cinfo.TotalInfo.Width = 1;
		cinfo.TotalInfo.bVisible = 1;
		cinfo.TotalInfo.WearedItemIdx[eWearedItem_Dress] = 23000 + rand()%150;
		cinfo.TotalInfo.WearedItemIdx[eWearedItem_Weapon] = 11000 + rand()%150;		
		cinfo.TotalInfo.Gender = GAMERESRCMNGR->m_TestClientInfo.aTestCharacterInfo[n].byGender;

		OBJECTMGR->AddPlayer(&cinfo.BaseObjectInfo,&bminfo,&cinfo.TotalInfo);		
		CObject * pObj = OBJECTMGR->GetObject(cinfo.BaseObjectInfo.dwObjectID);
		pObj->SetAngle(GAMERESRCMNGR->m_TestClientInfo.aTestCharacterInfo[n].fAngle);		
	}	
}

#endif

// 080916 LUJ, �ռ� â ��ȯ
CComposeDialog* CGameIn::GetComposeDialog()
{
	return ( CComposeDialog* )( WINDOWMGR->GetWindowForID( COMPOSE_DIALOG ) );
}

void CGameIn::AddBackupMsg(void* pMsg, DWORD dwMsgSize)
{
	char* pBackupMsg = new char[dwMsgSize];
	if(pBackupMsg)
	{
		ZeroMemory(pBackupMsg, dwMsgSize);
		memcpy(pBackupMsg, pMsg, dwMsgSize);
		m_pBackupMsgQueue.push(pBackupMsg);
	}
}

void CGameIn::ProcessBackupMsg()
{
#ifdef _GMTOOL_
	FILE* fp = fopen("PacketLog.txt", "wt");
	char buf[256] = {0,};
#endif

	while(FALSE == m_pBackupMsgQueue.empty())
	{
		MSGBASE* pMsg = (MSGBASE*)m_pBackupMsgQueue.front();
		m_pBackupMsgQueue.pop();
		if(pMsg)
		{
#ifdef _GMTOOL_
			if(fp)
			{
				sprintf(buf, "Category:[%02d] , Protocol:[%03d]\n", pMsg->Category, pMsg->Protocol);
				OutputDebugStringA(buf);
				fprintf(fp, buf);
				fflush(fp);
			}
#endif

			NetworkMsgParse(pMsg->Category, pMsg->Protocol, pMsg, 0);
			SAFE_DELETE_ARRAY(pMsg);
		}
	}

#ifdef _GMTOOL_
	if(fp)
		fclose(fp);
#endif
}
