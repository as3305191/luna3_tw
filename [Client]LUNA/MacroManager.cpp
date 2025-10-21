#include "stdafx.h"
#ifdef _GMTOOL_
#include "GMToolManager.h"
#endif

#include "WindowIdEnum.h"
#include "interface/cWindowManager.h"
#include "../GlobalEventFunc.h"
#include "MacroManager.h"
#include "GameIn.h"
#include "QuickManager.h"
#include "MoveManager.h"
#include "ObjectManager.h"
#include "MainGame.h"
#include "MHCamera.h"
#include "ChatManager.h"
#include "cChatTipManager.h"
#include "input/cIMEWnd.h"
#include "input/UserInput.h"
#include "ShowdownManager.h"
#include "StreetStallManager.h"
#include "ExchangeManager.h"
#include "MouseCursor.h"
#include "FriendManager.h"
#include "QuickDlg.h"
#include "QuickSlotDlg.h"
#include "CharacterDialog.h"
#include "InventoryExDialog.h"
#include "OptionDialog.h"
#include "ReviveDialog.h"
#include "BattleSystem_Client.h"
#include "PKManager.h"
#include "QuestTotaldialog.h"
#include "QuestManager.h"
#include "MiniMapDlg.h"
#include "BigmapDlg.h"
#include "cMsgBox.h"
#include "PartyIconManager.h"
#include "FreeImage/FreeImage.h"
#include "ItemShopDialog.h"
#include "StatusIconDlg.h"
#include "ItemManager.h"
#include "ObjectStateManager.h"
#include "DissolveDialog.h"
#include "Interface/cEditBox.h"
#include "AppearanceManager.h"
#include "ShoutDialog.h"
#include "GTResultDlg.h"
#include "GTScoreInfoDialog.h"
#include "KeySettingTipDlg.h"
#include "cSkillTreeDlg.h"
#include "ExitDialog.h"
#include "cResourceManager.h"
#include "QuestDialog.h"
#include "../FamilyDialog.h"
#include "../[cc]skill/client/manager/skillmanager.h"
#include "../[cc]skill/client/info/activeskillinfo.h"
#include "../DateMatchingDlg.h"
#include "Party.h"
#include "PartyManager.h"
#include "../mhMap.h"
#include "../hseos/Date/SHDateManager.h"
#include "./ChatRoomMgr.h"
#include "FishingManager.h"
#include "FishingDialog.h"
#include "PetInfoDialog.h"
#include "../hseos/Date/SHChallengeZoneClearNo1Dlg.h"			// 090317 ShinJS --- ç���� ���� ���ھ�â�� �ݱ� ���� ȣ��
#include "cHousingMgr.h"
#include "ConductManager.h"										// 100105 ONS �ɱ�/�Ͼ�⸦ ����Ѵ�.
#include "PCRoomManager.h"
#include "ChattingDlg.h"
#include "WorldMapDlg.h"
#include "..\hseos\Family\SHFamily.h"
#include "ProgressDialog.h"

#ifdef _TESTCLIENT_																	// �׽�Ʈ Ŭ���̾�Ʈ���,

#include "TSToolManager.h"
#include "MHMap.h"																	// �� ����� �ҷ��´�.
#include "ObjectManager.h"															// ������Ʈ �Ŵ��� ����� �ҷ��´�.
#include "EFFECT/EffectManager.h"													// ����Ʈ �Ŵ��� �ش��� �ҷ��´�.
#include "GameResourceManager.h"													// ���� ���ҽ� �Ŵ��� �ش��� �ҷ��´�.
#include "interface/cFont.h"														// ��Ʈ �ش��� �ҷ��´�.

void InitTestHero();																// �׽�Ʈ�� ����θ� �ʱ�ȭ �Ѵ�.
void InitTestMonster();																// �׽�Ʈ�� ���͸� �ʱ�ȭ �Ѵ�.
void InitTestCharacters(void);														// �׽�Ʈ�� ĳ���� ��Ʈ�� �ʱ�ȭ �Ѵ�.

#endif																				// �׽�Ʈ Ŭ���̾�Ʈ ó�� ����.


extern BOOL g_bActiveApp;
extern BOOL g_bDisplayFPS;															// �������� ������� ���θ� ���� ����.
extern int g_nServerSetNum;															// ������ ��ȣ�� ���� ����.

GLOBALTON(CMacroManager)

CMacroManager::CMacroManager()														// ��ũ�� �Ŵ��� ������ �Լ�.
{
	ZeroMemory( m_MacroKey, sizeof( m_MacroKey ) );									// ��ũ��Ű ����ü�� ���� �޸� �Ѵ�.
	ZeroMemory( m_DefaultKey, sizeof( m_DefaultKey ) );								// ����ƮŰ ����ü�� ���� �޸� �Ѵ�.

	ZeroMemory( m_MacroPressKey, sizeof( m_MacroPressKey ) );						// ��ũ�� ������Ű�� ���� �޸� �Ѵ�.
	ZeroMemory( m_DefaultPressKey, sizeof( m_DefaultPressKey ) );					// ����Ʈ ������Ű�� ���� �޸� �Ѵ�.

	m_nMacroMode	= MM_CHAT;														// ��ũ�� ��带 ä������ �Ѵ�.
	m_bChatMode		= TRUE;															// ä�ø�带 TRUE�� �����Ѵ�.
	m_bPause		= FALSE;														// ��� FALSE�� �����Ѵ�.
	m_bInVisibleCursor = FALSE;														// Ŀ���� ��¿��θ� FALSE�� �����Ѵ�.

	m_nCurTargetIdx = 0 ;															// �ڵ� Ÿ���� �ε����� 0���� �����Ѵ�.
	m_dwTargetID = 0 ;																// �ڵ� Ÿ���� ���̵� 0���� �����Ѵ�.

	m_nKeyMoveDir = KEY_MOVE_STOP;
	m_bKeyChanged = FALSE;
	m_bWASDMode		= FALSE;
}

CMacroManager::~CMacroManager()														// ��ũ�� �Ŵ��� �Ҹ��� �Լ�.
{
}


void CMacroManager::KeyboardInput_Normal( BOOL bDock, CKeyboard* keyInfo )
{
	extern HWND _g_hWnd;
	if(_g_hWnd != GetFocus())
	{
		return;
	}

	if( !g_bActiveApp || m_bPause )													// ���α׷��� Ȱ��ȭ ���� �ʾҰų�, ������¸�,
	{
		return;																		// �����Ѵ�.							
	}
	
	if(HEROID == 0)																	// ����� ���̵� 0�� ������,
	{
		return;																		// �����Ѵ�.
	}

	if( MAINGAME->GetCurStateNum() != eGAMESTATE_GAMEIN )							// ���� ������ ���°� ���� �� ���°� �ƴϸ�,
	{
		return;																		// �����Ѵ�.
	}

	//if (keyInfo->GetKeyPressed(88) == 1)											//f12�� ������� �ð� ���̾�α׸� ���δ�.
	//{
	//	PlayMacro(ME_SCREENCAPTURE) ;												//ŰŬ���� �������� �۵��ϹǷ� Ű�ٿ�� �������ϰ� Ű���� ��ũ���� ����.
	//}

	bDock = FALSE;																	// ��ŷ���θ� ���� ������ FALSE�� �����Ѵ�.

	if( CIMEWND->IsDocking() )														// IMEWND�� ��ŷ���̶��,
	{
		if( m_nMacroMode == MM_MACRO )												// ��ũ�� ��尡 ����Ű �켱�����,
		{
			bDock = TRUE;															// ��ŷ ������ TRUE�� �����Ѵ�.
		}
		else if (m_nMacroMode == MM_WASD)
		{
			bDock = TRUE;
		}
	}
	
	int nSysKey = MSK_NONE;															// �ý���Ű�� MSK_NONE���� �����Ѵ�.

	if( keyInfo->GetKeyPressed( KEY_MENU ) )										// �޴�Ű�� ���ȴٸ�,
	{
		nSysKey = 0;																// �ý���Ű�� 0���� �����ϰ�,
		nSysKey |= MSK_ALT;															// ALTŰ�� �߰��Ѵ�.
	}
	if( keyInfo->GetKeyPressed( KEY_CONTROL ) )										// ��Ʈ�� Ű�� ���ȴٸ�,
	{
		nSysKey &= ~MSK_NONE;														// �ý��� Ű�� MSK_NONE�� ������ �����Ѵ�.
		nSysKey |= MSK_CTRL;														// �ý��� Ű�� CTRLŰ�� �߰��Ѵ�.
	}
	if( keyInfo->GetKeyPressed( KEY_SHIFT ) )										// ����ƮŰ�� ���ȴٸ�,
	{
		nSysKey &= ~MSK_NONE;														// �ý��� Ű�� MSK_NONE�� ������ �����Ѵ�.
		nSysKey |= MSK_SHIFT;														// �ý��� Ű�� SHIFT�� �߰��Ѵ�.
	}
	
	for( int i = 0 ; i < ME_COUNT ; ++i )
	{
		const sMACRO& macro = m_MacroKey[m_nMacroMode][i];

		if( bDock && !macro.bAllMode )
		{
			continue;
		}
		else if(FALSE == (macro.nSysKey & nSysKey))
		{
			continue;
		}
		else if( macro.bUp )
		{
			if( keyInfo->GetKeyUp( macro.wKey ) )
			{
				PlayMacro( i );
			}
		}
		else if( keyInfo->GetKeyDown( macro.wKey ) )
		{
 			PlayMacro( i );
		}
	}

	CChattingDlg* pChatDlg = GAMEIN->GetChattingDlg();
	if (IsWASDMode() == TRUE && !pChatDlg->GetInputBox()->IsFocus())
	{
		if (keyInfo->GetKeyDown(KEY_W)
			|| keyInfo->GetKeyDown(KEY_S)
			|| keyInfo->GetKeyDown(KEY_A)
			|| keyInfo->GetKeyDown(KEY_D)
			|| keyInfo->GetKeyUp(KEY_W)
			|| keyInfo->GetKeyUp(KEY_S)
			|| keyInfo->GetKeyUp(KEY_A)
			|| keyInfo->GetKeyUp(KEY_D))
		{
			m_bKeyChanged = TRUE;
		}
	}
}

void CMacroManager::KeyboardInput_GMTool( BOOL bDock, CKeyboard* keyInfo )
{
#if defined(_GMTOOL_)
	if( (keyInfo->GetKeyDown(KEY_PADENTER) || (keyInfo->GetKeyDown(KEY_RETURN)))  // ����Ű�� ALTŰ�� ���Ȱų�,
		&& keyInfo->GetKeyPressed(KEY_MENU) )										// �����е��� ����Ű�� ALTŰ�� ��������,
	{
		if( GMTOOLMGR->IsShowing() )												// GM���� ���̰� �ִ»��¶��,
		{
			GMTOOLMGR->ShowGMDialog( FALSE, TRUE );									// GM���� �����.
		}
		else																		// GM���� ������ ���¶��,
		{
			GMTOOLMGR->ShowGMDialog( TRUE, TRUE );									// GM���� ������.
		}
	}
	else if( keyInfo->GetKeyDown(KEY_SUBTRACT) && keyInfo->GetKeyPressed(KEY_MENU) )// ALTŰ�� �����е��� -Ű�� ���ȴٸ�,
	{
		MSGBASE msg;																// �⺻ �޽��� ����ü�� �����Ѵ�.

		msg.Category	= MP_CHEAT;													// ī�װ����� ġƮ�� �����Ѵ�.
		msg.Protocol	= MP_CHEAT_AGENTCHECK_SYN;									// ���������� ������Ʈ üũ ��ũ��  �����Ѵ�.
		msg.dwObjectID	= HEROID;													// ������Ʈ ���̵� ����� ���̵� �����Ѵ�.

		NETWORK->Send( &msg, sizeof(msg) );											// �޽����� �����Ѵ�.
	}
#endif
}

void CMacroManager::KeyboardInput_CheatEnable( BOOL bDock, CKeyboard* keyInfo )
{	
#ifdef _TESTCLIENT_	
	KeyboardInput_Cheat_TestClient( bDock, keyInfo ) ;								// �׽�Ʈ Ŭ���̾�Ʈ �� �� ó���� �Ѵ�.
#else
	KeyboardInput_Cheat_NotTestClient( bDock, keyInfo ) ;							// �׽�Ʈ Ŭ���̾�Ʈ�� �ƴ� �� ó���� �Ѵ�.
#endif // _TESTCLIENT_
}

void CMacroManager::KeyboardInput_Cheat_TestClient( BOOL bDock, CKeyboard* keyInfo )
{
#ifdef _TESTCLIENT_	

	if( (keyInfo->GetKeyDown(KEY_PADENTER) || (keyInfo->GetKeyDown(KEY_RETURN)))  // ����Ű�� ALTŰ�� ���Ȱų�,
		&& keyInfo->GetKeyPressed(KEY_MENU) )										// �����е��� ����Ű�� ALTŰ�� ��������,
	{
		if( TSTOOLMGR->IsShowing() )												// GM���� ���̰� �ִ»��¶��,
		{
			TSTOOLMGR->ShowTSDialog( FALSE, TRUE );									// GM���� �����.
		}
		else																		// GM���� ������ ���¶��,
		{
			TSTOOLMGR->ShowTSDialog( TRUE, TRUE );									// GM���� ������.
		}
	}

	if(keyInfo->GetKeyDown(KEY_Z))
		HERO->GetEngineObject()->ChangeMotion(eMotion_Society_Happy, FALSE);

	if(keyInfo->GetKeyDown(KEY_U) && keyInfo->GetKeyPressed(KEY_CONTROL))			// CTRLŰ�� UŰ�� ���ȴٸ�,
	{
		if( GAMEIN->GetChattingDlg()->IsActive() )
		{
			GAMEIN->GetChattingDlg()->SetActive(FALSE) ;
			//GAMEIN->GetOSChattingDlg()->SetActive( FALSE ) ;
		}
		else
		{
			GAMEIN->GetChattingDlg()->SetActive(TRUE) ;
			//GAMEIN->GetOSChattingDlg()->SetActive( TRUE ) ;
		}
	}

	if(keyInfo->GetKeyDown(KEY_1))
	{
		if (keyInfo->GetKeyPressed(KEY_CONTROL)) // CTRLŰ�� 1��Ű�� ���ȴٸ�,
		{
			SKILLMGR->Release();													// ��ų�Ŵ����� �����Ѵ�.
			SKILLMGR->Init();														// ��ų �Ŵ����� �ʱ�ȭ �Ѵ�.
		}
	}

	if(keyInfo->GetKeyDown(KEY_2) && keyInfo->GetKeyPressed(KEY_CONTROL))			// CTRLŰ�� 2��Ű�� ���ȴٸ�,
	{
		EFFECTMGR->RemoveAllEffect();												// ��� ����Ʈ�� ���� �Ѵ�.
		EFFECTMGR->Release();														// ����Ʈ �Ŵ����� �����Ѵ�.
		EFFECTMGR->Init();															// ����Ʈ �Ŵ����� �ʱ�ȭ �Ѵ�.
	}

	if(keyInfo->GetKeyDown(KEY_3) && keyInfo->GetKeyPressed(KEY_CONTROL))			// CTRLŰ�� 3��Ű�� ���ȴٸ�,
	{
		EFFECTMGR->RemoveAllEffect();												// ��� ����Ʈ�� ���� �Ѵ�.

		GAMERESRCMNGR->LoadTestClientInfo();										// �׽�Ʈ Ŭ���̾�Ʈ ������ �ε��Ѵ�.

		OBJECTMGR->RemoveAllObject();												// ��� ������Ʈ�� �����Ѵ�.

		GAMERESRCMNGR->ResetMonsterList();											// ���� ����Ʈ�� �ٽ� �����Ѵ�.
		GAMERESRCMNGR->LoadMonsterList();											// ���� ����Ʈ�� �ε��Ѵ�.

		InitTestHero();																// �׽�Ʈ ����θ� �ʱ�ȭ �Ѵ�.
		InitTestMonster();															// �׽�Ʈ ���͸� �ʱ�ȭ �Ѵ�.
		InitTestCharacters();														// �׽�Ʈ ĳ���͸� �ʱ�ȭ �Ѵ�.
	}

	if(keyInfo->GetKeyDown(KEY_4) && keyInfo->GetKeyPressed(KEY_CONTROL))			// CTRLŰ�� 4��Ű�� ���ȴٸ�,
	{
		MAP->LoadMapDesc(MAP->GetMapNum());											// �� ������ �ε��Ѵ�.
		MAP->ApplyMapDesc();														// �� ������ �����Ѵ�.
	}
	if(keyInfo->GetKeyDown(KEY_ESCAPE))												// ESCŰ�� ���ȴٸ�,
	{
		EFFECTMGR->SetEndFlagAllEffect();											// ��� ����Ʈ�� ���� �÷��׸� �����Ѵ�.
		OBJECTMGR->SetSelectedObject(NULL);											// ���õ� ������Ʈ�� ���� �Ѵ�.
	}

	static BOOL bWireFrame = FALSE;													// ���̾� ������ ���θ� FALSE�� �����Ѵ�.

	if(keyInfo->GetKeyDown(KEY_F12) && keyInfo->GetKeyPressed(KEY_MENU))			// ALTŰ�� F12�� ���ȴٸ�,
	{
		if( !bWireFrame )															// ���̾� ������ ���ΰ� FALSE���,
		{			
			g_pExecutive->GetRenderer()->SetRenderMode(2);							// ������带 ���̾� ���������� �Ѵ�.

			bWireFrame = TRUE;														// ���̾� ������ ���θ� TRUE�� �����Ѵ�.
		}
		else																		// ���̾� ������ ���ΰ� TRUE���,
		{
			g_pExecutive->GetRenderer()->SetRenderMode(0);							// ������带 �ָ���� �Ѵ�.

			bWireFrame = FALSE;														// ���̾� ������ ���θ� FALSE�� �����Ѵ�.
		}
	}

	if(keyInfo->GetKeyDown(KEY_R) && keyInfo->GetKeyPressed(KEY_MENU))				// ALT�� RŰ�� ���ȴٸ�,
	{
		EFFECTMGR->StartEffectProcess(1034,HERO,NULL,0,0);							// 1034 ����Ʈ�� �����Ѵ�.
	}

	if(keyInfo->GetKeyDown(KEY_4) && keyInfo->GetKeyPressed(KEY_MENU))				// ALTŰ�� 4�� Ű�� ���ȴٸ�,
	{
		EFFECTMGR->StartHeroEffectProcess(FindEffectNum("Mall_eff_heal.beff"));		// �� ����Ʈ�� �����Ѵ�.
	}

	if(keyInfo->GetKeyDown(KEY_5) && keyInfo->GetKeyPressed(KEY_MENU))				// ALTŰ�� 5�� Ű�� ���ȴٸ�,
	{
		EFFECTMGR->StartHeroEffectProcess(FindEffectNum("Enchant_daggers01_A1.beff"));			// ����Ʈ�� �����Ѵ�.
	}

	if(keyInfo->GetKeyDown(KEY_7) && keyInfo->GetKeyPressed(KEY_MENU))				// ALTŰ�� 7�� Ű�� ���ȴٸ�,
	{
		EFFECTMGR->StartHeroEffectProcess(eEffect_ShopItem_Revive);					// �������� ��Ȱ ����Ʈ�� �����Ѵ�.
	}

	if(keyInfo->GetKeyDown(KEY_W))													// WŰ�� ���ȴٸ�,
	{
		CActionTarget Target;														// �׼� Ÿ���� �����Ѵ�.

		cSkillInfo* pInfo = NULL ;													// ��ų ������ ���� �����͸� �����Ѵ�.
		
		pInfo = SKILLMGR->GetSkillInfo(GAMERESRCMNGR->m_TestClientInfo.SkillIdx);	// �׽�Ʈ Ŭ���̾�Ʈ ��ų�ε����� ��ų ������ �޴´�.

		if( !pInfo )																// ��ų ������ ��ȿ���� �ʴٸ�,
		{
			return;																	// �����Ѵ�.
		}

		cActiveSkillInfo* pSkillInfo = ( cActiveSkillInfo* )pInfo;					// ��Ƽ�� ��ų ������ �޴´�.

		if(pSkillInfo->GetSkillInfo()->Target == 1)									// ��ų�� Ÿ���� 1�� ������,
		{
			CObject * targetObj = OBJECTMGR->GetSelectedObject();					// ������ Ÿ���� ������Ʈ ������ �޴´�.

			if( targetObj )															// ������Ʈ ������ ��ȿ���� üũ�Ѵ�.
			{
				Target.InitActionTarget(targetObj,NULL);							// Ÿ���� ������� �׼��� �ʱ�ȭ �Ѵ�.
			}
		}
		else if(pSkillInfo->GetSkillInfo()->Target == 0)							// ��ų�� Ÿ���� 0�� ������,
		{
			Target.InitActionTarget(HERO,NULL);										// ����θ� ������� �׼��� �ʱ�ȭ �Ѵ�.
		}
		
		SKILLMGR->OnSkillCommand( HERO, &Target, TRUE );							// ��ų �Ŵ��� ������ �����Ѵ�.
	}

	if(keyInfo->GetKeyDown(KEY_E))													// EŰ�� ���ȴٸ�,
	{
		CObject * targetObj = OBJECTMGR->GetSelectedObject();						// ������ Ÿ���� ������Ʈ ������ �޴´�.

		if( targetObj )																// �������� ������ ��ȿ���� üũ�Ѵ�.
		{
			TARGETSET Target;														// Ÿ�� �� ����ü�� �����Ѵ�.

			Target.pTarget = targetObj;												// Ÿ���� �����Ѵ�.
			memset(&Target.ResultInfo,0,sizeof(RESULTINFO));						// ��� ������ �޸� ���Ѵ�.
			Target.ResultInfo.bCritical = rand()%2 ? TRUE : FALSE;					// ũ��Ƽ�� ���θ� �����Ѵ�.
			Target.ResultInfo.RealDamage = rand() % 50;								// ���� �������� �����Ѵ�.
			Target.ResultInfo.CounterDamage = 0;									// ī���� �������� �����Ѵ�.

			// ����Ʈ�� �����Ѵ�.
			EFFECTMGR->StartEffectProcess(GAMERESRCMNGR->m_TestClientInfo.Effect,HERO,&Target,1,targetObj->GetID(),EFFECT_FLAG_HEROATTACK);
		}
	}

	if(keyInfo->GetKeyDown(KEY_4))													// 4��Ű�� ���ȴٸ�,
	{
		CObject * targetObj = OBJECTMGR->GetSelectedObject();						// Ÿ�� ������Ʈ ������ �޾ƿ´�.

		if( targetObj )																// ������Ʈ ������ ��ȿ���� üũ�Ѵ�.
		{
			VECTOR3 pos;															// ���͸� �����Ѵ�.

			targetObj->GetPosition(&pos);											// Ÿ���� ��ġ�� �޾ƿ´�.
			pos.z -= 3000;															// Z���� �����Ѵ�.

			MOVEMGR->SetMonsterMoveType(targetObj,eMA_WALKAROUND);					// ������ �̵� Ÿ���� eMA_WALKAROUND�� �����Ѵ�.
			MOVEMGR->StartMoveEx(targetObj,gCurTime,&pos);							// �̵��� �����Ѵ�.
		}
	}

	if(keyInfo->GetKeyDown(KEY_5))													// 5��Ű�� ���ôٸ�,
	{
		CObject * targetObj = OBJECTMGR->GetSelectedObject();						// Ÿ�� ������Ʈ ������ �޾ƿ´�.

		if( targetObj )																// ������Ʈ ������ ��ȿ���� üũ�Ѵ�.
		{
			VECTOR3 pos;															// ���͸� �����Ѵ�.

			targetObj->GetPosition(&pos);											// Ÿ���� ��ġ�� �޾ƿ´�.
			pos.z -= 3000;															// Z���� �����Ѵ�.

			MOVEMGR->SetMonsterMoveType(targetObj,eMA_PERSUIT);						// ������ �̵� Ÿ���� eMA_PERSUIT�� �����Ѵ�.
			MOVEMGR->StartMoveEx(targetObj,gCurTime,&pos);							// �̵��� �����Ѵ�.
		}
	}

	if(keyInfo->GetKeyDown(KEY_6))													// 6��Ű�� ���ȴٸ�,
	{
		CObject * targetObj = OBJECTMGR->GetSelectedObject();						// ������ Ÿ�� ������Ʈ ������ �޾ƿ´�.

		if( targetObj )																// ������Ʈ ������ ��ȿ���� üũ�Ѵ�.
		{
			VECTOR3 pos;															// ���͸� �����Ѵ�.

			targetObj->GetPosition(&pos);											// Ÿ���� ��ġ�� �޾ƿ´�.
			pos.z -= 3000;															// Z���� �����Ѵ�.

			MOVEMGR->SetMonsterMoveType(targetObj,eMA_RUNAWAY);						// ������ �̵� Ÿ���� eMA_RUNAWAY�� �����Ѵ�.
			MOVEMGR->StartMoveEx(targetObj,gCurTime,&pos);							// �̵��� �����Ѵ�.
		}
	}

	if(keyInfo->GetKeyDown(KEY_F1) && keyInfo->GetKeyPressed(KEY_CONTROL))			// CTRLŰ�� F1Ű�� ���ȴٸ�,
	{
		g_bDisplayFPS = !g_bDisplayFPS;												// �������� ����Ѵ�.
	}

	if( keyInfo->GetKeyDown(KEY_C) && keyInfo->GetKeyPressed(KEY_CONTROL))	// ALTŰ�� CŰ�� ������,
	{
		if( CAMERA->GetCameraMode() == eCM_EyeView )								// ī�޶� ��尡 EyeView�� ������,
		{
			CAMERA->SetCameraMode( eCM_Free );										// ī�޶� ��带 Free���� �����Ѵ�.

			if( !WINDOWMGR->IsOpendAllWindows() )									// ��� â���� ���� ���� ������,
			{
				WINDOWMGR->ShowAllActivedWindow() ;									// Ȱ��ȭ �� ��� â�� �����ش�.

				WINDOWMGR->SetOpendAllWindows( TRUE ) ;								// ��� �����찡 ���� ������ �����Ѵ�.

				WINDOWMGR->ShowBaseWindow() ;										// �⺻ ��������� �����ش�.
			}

		}
		else																		// ī�޶� ��尡 EyeView�� ���� ������,
		{
			CAMERA->SetCameraMode( eCM_EyeView );									// ī�޶� ��带 EyeView�� �����Ѵ�.

			if( WINDOWMGR->IsOpendAllWindows() )									// ��� �����찡 ���� ������,
			{
				//WINDOWMGR->m_pActivedWindowList->RemoveAll() ;						// Ȱ��ȭ �� â���� ��� �ִ� ����Ʈ�� ��� ����.

				WINDOWMGR->CloseAllWindow() ;										// ��� �����츦 �ݴ´�.

				WINDOWMGR->SetOpendAllWindows( FALSE ) ;							// ��� �����찡 ���� ������ �����Ѵ�.
			}
		}
	}
#endif // _TESTCLIENT_
}

void CMacroManager::KeyboardInput_Cheat_NotTestClient( BOOL bDock, CKeyboard* keyInfo )
{
#if defined(_GMTOOL_)
	if(keyInfo->GetKeyDown(KEY_F1) && keyInfo->GetKeyPressed(KEY_CONTROL))			// CTRLŰ�� F1Ű�� ���ȴٸ�,
	{
		g_bDisplayFPS = !g_bDisplayFPS;												// �������� ����Ѵ�.
	}
	
	if(keyInfo->GetKeyDown(KEY_BACK) && keyInfo->GetKeyPressed(KEY_MENU))			// ALTŰ�� BACK Ű�� ������,
	{
		WINDOWMGR->ToggleShowInterface();											// �������̽��� ��� ��Ų��.
	}

	if(keyInfo->GetKeyDown(KEY_P) && keyInfo->GetKeyPressed(KEY_MENU))				// ALTŰ�� PŰ�� ������,
	{
		m_bInVisibleCursor ^= TRUE;													// Ŀ�� ��¿��θ� �����Ѵ�.
		CURSOR->SetCheatMode( m_bInVisibleCursor );									// Ŀ���� ġƮ���� �����Ѵ�.
	}

	if( keyInfo->GetKeyDown(KEY_C) && keyInfo->GetKeyPressed(KEY_CONTROL))	// ALTŰ�� CŰ�� ������,
	{
		if( CAMERA->GetCameraMode() == eCM_EyeView )								// ī�޶� ��尡 EyeView�� ������,
		{
			CAMERA->SetCameraMode( eCM_Free );										// ī�޶� ��带 Free���� �����Ѵ�.

			if( !WINDOWMGR->IsOpendAllWindows() )									// ��� â���� ���� ���� ������,
			{
				WINDOWMGR->ShowAllActivedWindow() ;									// Ȱ��ȭ �� ��� â�� �����ش�.

				WINDOWMGR->SetOpendAllWindows( TRUE ) ;								// ��� �����찡 ���� ������ �����Ѵ�.

				WINDOWMGR->ShowBaseWindow() ;										// �⺻ ��������� �����ش�.
			}

		}
		else																		// ī�޶� ��尡 EyeView�� ���� ������,
		{
			CAMERA->SetCameraMode( eCM_EyeView );									// ī�޶� ��带 EyeView�� �����Ѵ�.

			if( WINDOWMGR->IsOpendAllWindows() )									// ��� �����찡 ���� ������,
			{
				//WINDOWMGR->m_pActivedWindowList->RemoveAll() ;						// Ȱ��ȭ �� â���� ��� �ִ� ����Ʈ�� ��� ����.

				WINDOWMGR->CloseAllWindow() ;										// ��� �����츦 �ݴ´�.

				WINDOWMGR->SetOpendAllWindows( FALSE ) ;							// ��� �����찡 ���� ������ �����Ѵ�.
			}
		}
	}
#endif
}

void CMacroManager::KeyboardInput( CKeyboard* keyInfo )
{
	BOOL bDock = FALSE;																// ��ŷ���θ� ���� ������ FALSE�� �����Ѵ�.

	KeyboardInput_Normal( bDock, keyInfo ) ;										// �Ϲ����� Ű���� �Է� ó���� �Ѵ�.

#ifdef _GMTOOL_																		// GM�� ó���� �����Ѵ�.

	KeyboardInput_GMTool( bDock, keyInfo ) ;										// GM���� �� Ű���� �Է� ó���� �Ѵ�.

#endif																				// GM�� ó�� ����.


#ifdef _CHEATENABLE_																// ġƮ����� �� ó���� �����Ѵ�.

	KeyboardInput_CheatEnable( bDock, keyInfo ) ;									// ġƮ����� �� Ű���� �Է� ó���� �Ѵ�.

#endif	//_CHEATENABLE_																// ġƮ����� �� ó���� ���� �Ѵ�.

}

void CMacroManager::KeyboardPressInput( CKeyboard* keyInfo )
{
	if( !g_bActiveApp || m_bPause )	return;											// ���α׷��� Ȱ��ȭ ������ �ʰų�, ��������̸� �����Ѵ�.

	if( MAINGAME->GetCurStateNum() != eGAMESTATE_GAMEIN ) return;					// ������ ���°� ���̸� �����Ѵ�.

	if( CIMEWND->IsDocking() )														// ��ŷ�����̸�, 
	{
		return;																		// �����Ѵ�.
	}

	int nSysKey = MSK_NONE;															// �ý��� Ű ���� �⺻���� �����Ѵ�.

	if( keyInfo->GetKeyPressed( KEY_CONTROL ) )		nSysKey |= MSK_CTRL;			// ��Ʈ�� Ű�� ��������, ��Ʈ�� Ű�� �߰��Ѵ�.
	if( keyInfo->GetKeyPressed( KEY_MENU ) )		nSysKey |= MSK_ALT;				// ��Ʈ Ű�� ��������, ��ƮŰ�� �߰��Ѵ�.
	if( keyInfo->GetKeyPressed( KEY_SHIFT ) )		nSysKey |= MSK_SHIFT;			// ����Ʈ Ű�� ��������, ����ƮŰ�� �߰��Ѵ�.

	//090116 pdy Ŭ���̾�Ʈ ����ȭ �۾� ( Show Name On Press Key )
	//SHIFTŰ�������� �̸����̱� �߰� 
	OBJECTMGR->SetPress_ShowNameKey( nSysKey & MSK_SHIFT );								// 090108 ����ƮŰ�� ���ȳ� �����ĸ� ������Ʈ �޴����� ����


	for( int i = 0 ; i < MPE_COUNT ; ++i )											// ��ũ�� ������ �̺�Ʈ ����ŭ ������ ������.
	{
		if( m_MacroPressKey[m_nMacroMode][i].nSysKey & nSysKey						// ���� ��ũ�� Ű�� �ý��� Ű�� Ȯ���ϰ�, ��ũ�� Ű�� ��������,
			&& keyInfo->GetKeyPressed( m_MacroPressKey[m_nMacroMode][i].wKey ) )
		{
			PlayMacroPress( i );													// ��ũ�θ� �����Ѵ�.
		}
	}		

	m_nKeyMoveDir = KEY_MOVE_STOP;
	CProgressDialog* dialog = (CProgressDialog*)WINDOWMGR->GetWindowForID(PROGRESS_DIALOG);
	if (IsWASDMode() == TRUE && !dialog->IsActive())
	{
		if (keyInfo->GetKeyPressed(KEY_A))
		{
			m_nKeyMoveDir |= KEY_MOVE_LEFT;
		}
		if (keyInfo->GetKeyPressed(KEY_D))
		{
			m_nKeyMoveDir |= KEY_MOVE_RIGHT;
		}
		if (keyInfo->GetKeyPressed(KEY_W))
		{
			m_nKeyMoveDir |= KEY_MOVE_UP;
		}
		if (keyInfo->GetKeyPressed(KEY_S))
		{
			m_nKeyMoveDir |= KEY_MOVE_DOWN;
		}
	}
}

void CMacroManager::GetMacro( int nMacroMode, sMACRO* pMacro )
{
	memcpy( pMacro, m_MacroKey[nMacroMode], sizeof(sMACRO) * ME_COUNT );			// ���� ��ũ�� ���� ���� ��ũ�� ����ü�� ��ũ�� ������ �����Ѵ�.
}

void CMacroManager::SetMacro( int nMacroMode, sMACRO* pMacro )
{
	memcpy( m_MacroKey[nMacroMode], pMacro, sizeof(sMACRO) * ME_COUNT );			// ���� ��ũ�� ���� ���� ��ũ�� ����ü�� ��� ��ũ�� ����ü�� �����Ѵ�.
}

void CMacroManager::GetDefaultMacro( int nMacroMode, sMACRO* pMacro )
{
	memcpy( pMacro, m_DefaultKey[nMacroMode], sizeof(sMACRO) * ME_COUNT );			// ����Ʈ ��ũ�θ� ���� ��ũ�� ����ü�� �����Ѵ�.
}

void CMacroManager::SetMacroMode( int nMacroMode )
{
	m_nMacroMode = nMacroMode;														// ���ڷ� ���� ��ũ�� ��带 ��� ��ũ�� ��� ������ �����Ѵ�.

	if( m_nMacroMode == MM_CHAT )													// ��ũ�θ�尡 ä�ø��� ���ٸ�,
	{
		m_bChatMode = TRUE;															// ä�ø�� ������ TRUE�� �����Ѵ�.
	}
	else if (m_nMacroMode == MM_WASD)
	{
		m_bChatMode = FALSE;
		m_bWASDMode = TRUE;
	}
	else																			// ��ũ�θ�尡 ä�ø��� ���� �ʴٸ�,
	{
		m_bChatMode = FALSE;														// ä�ø�� ������ FALSE�� �����Ѵ�.
		m_bWASDMode = FALSE;
	}
}

void CMacroManager::LoadUserMacro()
{
	SetDefaultMacro( MM_MACRO );													// ����Ʈ ��ũ�θ� ����Ű �켱 ���� �����Ѵ�.
	SetDefaultMacro( MM_CHAT );														// ����Ʈ ��ũ�θ� ä�� �켱 ���� �����Ѵ�.
	SetDefaultMacro( MM_WASD );
	SetMacroMode( MM_MACRO );														// ��ũ�� ��带 ����Ű �켱 ���� �����Ѵ�.
}


BOOL CMacroManager::LoadMacro( LPCTSTR strPath )
{
	HANDLE	hFile;																	// ���� �ڵ��� �����Ѵ�.

	hFile = CreateFile( strPath, GENERIC_READ, 0, NULL,								// �б���� ���� �н��� ������ �о� �ڵ��� �Ѱ� �޴´�.
						OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

	if( hFile == INVALID_HANDLE_VALUE )												// �ڵ��� �޴µ� �����ߴٸ�,
		return FALSE;																// ����ó���� �Ѵ�.

	DWORD dwRead;																	// ������ ���� ����Ʈ ���� ���Ϲޱ� ���� ��¿� �μ�.
	
	if( !ReadFile( hFile, m_MacroKey,												// ��ũ��Ű ����ü��, ��ũ�� Ű ������ ��ŭ �о���δ�.
		sizeof( m_MacroKey ), &dwRead, NULL ) )										// �о���̴°� �����ϸ�,
	{
		CloseHandle( hFile );														// �ڵ��� �ݴ´�.
		return FALSE;																// FALSE ���� ó���� �Ѵ�.
	}

	if( !ReadFile( hFile, m_MacroPressKey,											// m_MacroPressKey����ü�� m_MacroPressKey ������ ��ŭ �о���δ�.
		sizeof( m_MacroPressKey ), &dwRead, NULL ) )								// �о���̴°� �����ϸ�,
	{
		CloseHandle( hFile );														// �ڵ��� �ݴ´�.
		return FALSE;																// FALSE ���� ó���� �Ѵ�.
	}
	
	CloseHandle( hFile );															// �ڵ��� �ݴ´�.

	return TRUE;																	// TRUE ���� ó���� �Ѵ�.
}

void CMacroManager::SaveUserMacro()
{
	SaveMacro( "./INI/MacroUserSet.mcr" );											// ��ũ�θ� �����Ѵ�.
}

BOOL CMacroManager::SaveMacro( LPCTSTR strPath )
{
	HANDLE	hFile;																	// ���� �ڵ��� �����Ѵ�.

	hFile=CreateFile( strPath, GENERIC_WRITE, 0, NULL,								// ������ �н��� ������� ���� �ڵ��� �޾ƿ´�.
						CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

	if( hFile == INVALID_HANDLE_VALUE )												// ���� �ڵ��� �޴µ� �����ϸ�,
		return FALSE;																// FALSE ���� ó���� �Ѵ�.
	
	DWORD dwWritten;																// ������ �� ����Ʈ ���� ���Ϲޱ� ���� ��¿� �μ�.
	
	if( !WriteFile( hFile, m_MacroKey,												// m_MacroKey ����ü�� ��� ������ m_MacroKey ����ü ������ ��ŭ ����.
		sizeof( m_MacroKey ), &dwWritten, NULL ) )									// ���⿡ �����Ѵٸ�,
	{
		CloseHandle( hFile );														// �ڵ��� �ݴ´�.
		return FALSE;																// FALSE ���� ó���� �Ѵ�.
	}

	if( !WriteFile( hFile, m_MacroPressKey,											// m_MacroPressKey ����ü�� ��� ������ m_MacroPressKey ����ü ������ ��ŭ ����.
		sizeof( m_MacroPressKey ), &dwWritten, NULL ) )								// ���⿡ �����Ѵٸ�,
	{
		CloseHandle( hFile );														// �ڵ��� �ݴ´�.
		return FALSE;																// FALSE ����ó���� �Ѵ�.
	}

	CloseHandle( hFile );															// �ڵ��� �ݴ´�.
	return TRUE;																	// TRUE ���� ó���� �Ѵ�.
}

void CMacroManager::PlayMacro( int nMacroEvent )
{
	if( CAMERA->GetCameraMode() == eCM_EyeView )									// ī�޶� ��尡 eCM_EyeView�� ���ٸ�,
	{
		return;																		// ���� ó���� �Ѵ�.
	}

	switch( nMacroEvent )															// ��ũ�� �̺�Ʈ�� Ȯ���Ѵ�.
	{
		case ME_USE_QUICKITEM_1_01: 												// ���� ������ 1��Ű ���
		case ME_USE_QUICKITEM_1_02: 												// ���� ������ 2��Ű ���
		case ME_USE_QUICKITEM_1_03: 												// ���� ������ 3��Ű ���
		case ME_USE_QUICKITEM_1_04: 												// ���� ������ 4��Ű ���
		case ME_USE_QUICKITEM_1_05: 												// ���� ������ 5��Ű ���
		case ME_USE_QUICKITEM_1_06: 												// ���� ������ 6��Ű ���
		case ME_USE_QUICKITEM_1_07: 												// ���� ������ 7��Ű ���
		case ME_USE_QUICKITEM_1_08: 												// ���� ������ 8��Ű ���
		case ME_USE_QUICKITEM_1_09: 												// ���� ������ 9��Ű ���
		case ME_USE_QUICKITEM_1_10:	PM_UseMainQuickSlotItem(nMacroEvent) ;	break ;	// ���� ������10��Ű ���

		case ME_USE_QUICKITEM_2_01: 												// ���� ������ 1��Ű ��� 
		case ME_USE_QUICKITEM_2_02: 			 									// ���� ������ 2��Ű ��� 
		case ME_USE_QUICKITEM_2_03: 			 									// ���� ������ 3��Ű ��� 
		case ME_USE_QUICKITEM_2_04: 			 									// ���� ������ 4��Ű ��� 
		case ME_USE_QUICKITEM_2_05: 			 									// ���� ������ 5��Ű ��� 
		case ME_USE_QUICKITEM_2_06: 			 									// ���� ������ 6��Ű ��� 
		case ME_USE_QUICKITEM_2_07: 			 									// ���� ������ 7��Ű ��� 
		case ME_USE_QUICKITEM_2_08: 			 									// ���� ������ 8��Ű ��� 
		case ME_USE_QUICKITEM_2_09: 			 									// ���� ������ 9��Ű ��� 
		case ME_USE_QUICKITEM_2_10:	PM_UseSubQuickSlotItem(nMacroEvent) ;	break ;	// ���� ������10��Ű ��� 

		case ME_CHANGE_QUICKSLOT_01: 												// ���� �������� 1������ ������.
		case ME_CHANGE_QUICKSLOT_02: 												// ���� �������� 2������ ������.
		case ME_CHANGE_QUICKSLOT_03: 												// ���� �������� 3������ ������.
		case ME_CHANGE_QUICKSLOT_04: 												// ���� �������� 4������ ������.
		case ME_CHANGE_QUICKSLOT_05: 												// ���� �������� 5������ ������.
		case ME_CHANGE_QUICKSLOT_06: 												// ���� �������� 6������ ������.
		case ME_CHANGE_QUICKSLOT_07: 												// ���� �������� 7������ ������.
		case ME_CHANGE_QUICKSLOT_08:	PM_Change_MainSlotNum(nMacroEvent) ;break ;	// ���� �������� 8������ ������.

		case ME_INCREASE_MAIN_QUICKSLOT_NUM:														// ���� ������ ��ȣ�� ���� ��Ų��.
		case ME_DECREASE_MAIN_QUICKSLOT_NUM: PM_SetSlotPage(QI1_QUICKSLOTDLG, nMacroEvent) ; break ;// ���� ������ ��ȣ�� ���� ��Ų��.

		// 080707 LYW --- MacroManager : Ȯ�� ���� ����Ű�� ���´�.
		//case ME_INCREASE_SUB_QUICKSLOT_NUM: 														// ���� ������ ��ȣ�� ���� ��Ų��.
		//case ME_DECREASE_SUB_QUICKSLOT_NUM:  PM_SetSlotPage(QI2_QUICKSLOTDLG, nMacroEvent) ; break ;// ���� ������ ��ȣ�� ���� ��Ų��.

		case ME_SELECT_MYSELF: HERO->ShowObjectName( TRUE, NAMECOLOR_SELECTED ); OBJECTMGR->SetSelectedObject( HERO ); break ;// �� �ڽ��� �����Ѵ�.

		case ME_SELECT_PARTYMEMBER_1: 												// ù ��° ��Ƽ ����� �����Ѵ�.
		case ME_SELECT_PARTYMEMBER_2: 												// �� ��° ��Ƽ ����� �����Ѵ�.
		case ME_SELECT_PARTYMEMBER_3: 												// �� ��° ��Ƽ ����� �����Ѵ�.
		case ME_SELECT_PARTYMEMBER_4: 												// �� ��° ��Ƽ ����� �����Ѵ�.
		case ME_SELECT_PARTYMEMBER_5: 												// �ټ� ��° ��Ƽ ����� �����Ѵ�.
		case ME_SELECT_PARTYMEMBER_6:	PM_SelectPartyMember(nMacroEvent) ;break ;	// ���� ��° ��Ƽ ����� �����Ѵ�.

		case ME_SELECT_NEAR_TARGET:		PM_SetAutoTarget() ;				break ;	// ���� �����̿� �ִ� Ÿ���� �����Ѵ�.

		case ME_SCREENCAPTURE:					PM_ScreenCapture() ;		break ;	// ��ũ���� ĸ���Ѵ�.

		case ME_TOGGLE_EXITDLG:					PM_Toggle_ExitDlg() ;		break ;	// ���� �޽��� â�� ����.

		case ME_TOGGLE_ALLINTERFACE:			PM_Toggle_AllInterface() ;	break ;	// ��� �������̽��� ����Ѵ�.
		
		case ME_TOGGLE_SKILLDLG:				PM_Toggle_SkillDlg() ;		break ;	// ��ųâ�� ����Ѵ�.

		case ME_TOGGLE_FAMILYDLG:				PM_Toggle_FamilyDlg() ;		break ;	// �йи� â�� ����Ѵ�.

		case ME_TOGGLE_INVENTORYDLG:			PM_Toggle_InventoryDlg() ;  break ;	// �κ��丮�� ����Ѵ�.

		case ME_TOGGLE_HELP_SHOW:				PM_Toggle_ShowKeyTip() ;	break ;	// ������ ����Ѵ�.

		case ME_TOGGLE_BIGMAP:					PM_Toggle_BigMap() ;		break ;	// ����� ����Ѵ�.

		case ME_TOGGLE_WORLDMAP:				PM_Toggle_WorldMap() ;		break ;	// ������� ����Ѵ�.

		case ME_TOGGLE_OPTIONDLG:				PM_Toggle_OptionDlg() ;		break ;	// �ɼ� â�� ����Ѵ�.

		case ME_TOGGLE_GUILDDLG:				PM_Toggle_GuildDlg() ;		break ;	// ��� â�� ����Ѵ�.

		case ME_TOGGLE_QUESTDLG:				PM_Toggle_QuestDlg() ;		break ;	// ����Ʈ â�� ����Ѵ�.

		case ME_TOGGLE_CHARINFODLG:				PM_Toggle_CharInfoDlg() ;	break ;	// ĳ���� ����â�� ����Ѵ�.

		case ME_WALK_RUN:						MOVEMGR->ToggleHeroMoveMode() ;	break ;	// �ȱ�/�ٱ� ��带 ��ȯ�Ѵ�.

		case ME_TOGGLE_CAMERAVIEWMODE:			CAMERA->ToggleCameraViewMode(); break ;	// ī�޶� �� ��带 ��ȯ�Ѵ�.

		case ME_TOGGLE_MATCHINGDLG:				PM_Toggle_MatchingDlg() ;		break ;	// ��Ī â�� ����Ѵ�.

		case ME_TOGGLE_FRIENDDLG:				PM_Toggle_FriendDlg() ;		break ;		// ģ�� ����� ����Ѵ�.

		// 071025 LYW --- MacroManager : Add macro for help dialog.
		case ME_TOGGLE_HELPDLG :				PM_Toggle_HelpDlg() ;		break ;		// ���� â�� ����Ѵ�.

		case ME_FISHING_PULLINGBTN:				GAMEIN->GetFishingDlg()->Fishing_Pullling();	break;
		case ME_TOGGLE_GTRESULT_DLG:			PM_Toggle_GTResultDlg();	break;

		// 080429 LYW --- MacroManager : Add macro for chatroom system.
		case ME_TOGGLE_CHATROOM_LISTDLG :		PM_Toggle_ChatRoomListDlg() ; break ;	// ä�ù� ����Ʈâ�� ����Ѵ�.
		case ME_TOGGLE_CHATROOM_ROOMDLG :		PM_Toggle_ChatRoomDlg() ;	  break ;	// ä�ù��� ��Ŭ�Ѵ�.
		case ME_SETFOCUS_CHATROOM_CHATTING :	PM_SetFocus_ChatRoom_Chat() ; break ;	// ä��â�� ä�ù� ���̷� ����Ʈ �ڽ� ��Ŀ���� �̵���Ų��.


		case ME_PET_INFO : 	PM_Toggle_Pet_Info();	break;

		// 090422 ShinJS --- Ż�� ž��/���� ����
		case ME_VEHICLE_GETONOFF :				PM_Toggle_Vehicle_GetOnOff();	break;			

		// 100105 ONS �ɱ�/�Ͼ�⸦ ����Ѵ�.
		case ME_TOGGLE_RESTMODE:				CONDUCTMGR->OnConductCommand( 1 ) ;	break;

		default : break ;
	}
}

void CMacroManager::PM_Toggle_ExitDlg()
{
	if( HERO->GetCurrentSkill() )
	{
		SKILLMGR->SkillCancel(
			SKILLMGR->GetSkillObject(HERO->GetCurrentSkill()));
	}
	else if(GAMEIN->GetFishingDlg()->IsPushedStartBtn())		// �������̸� ���ø� �ߴ��Ѵ�.
	{
		GAMEIN->GetFishingDlg()->Fishing_Cancel();
	}
	else if(CVehicle* vehicleObject = (CVehicle*)OBJECTMGR->GetObject(VEHICLE_INSTALL_ID))
	{
		if(eObjectKind_Vehicle == vehicleObject->GetObjectKind())
		{
			if(CItem* const item = ITEMMGR->GetItem(vehicleObject->GetUsedItem().dwDBIdx))
			{
				item->SetLock(
					FALSE);
			}

			OBJECTMGR->RemoveVehicle(
				vehicleObject->GetID());
		}

        GAMEIN->GetInventoryDialog()->SetDisable(
			FALSE);
	}
	// 090611 pdy �Ͽ�¡ ���� ��ġ�߿� esc�������� ��ġ ��Ҹ� ������ 
	else if( HOUSINGMGR->IsHousingMap()	&& HOUSINGMGR->IsDoDecoration() )
	{
		HOUSINGMGR->CancelDecoration();
	}
	else if( !WINDOWMGR->CloseAllAutoCloseWindows() )									// �ڵ����� ������ ��������� �ݴ� ���� �����ϸ�,?
	{
		// 080117 LYW --- MacroManager : ç���� �ʿ����� ESCŰ ����� ���´�.
		if (!g_csDateManager.IsChallengeZone(MAP->GetMapNum()))
		{
			CExitDialog * pWindow = GAMEIN->GetExitDialog();							// ���� �޽��� ������ ������ �޾ƿ´�.

			if(pWindow)																	// ���� �޽��� ������ ������ ��ȿ�ϸ�,
			{
				pWindow->SetActive(!pWindow->IsActive()) ;								// ���� �޽��� �����츦 Ȱ��, ��Ȱ��ȭ �Ѵ�.
			}
		}
	}

	// 090317 ShinJS --- ç���� �ʿ��� ���ھ�� ����� LimitTime Ȱ��ȭ�� ���� ó��
	if( g_csDateManager.IsChallengeZone(MAP->GetMapNum()) )
	{		
		CSHChallengeZoneClearNo1Dlg* pDlg = GAMEIN->GetChallengeZoneClearNo1Dlg();
		if( pDlg )
			pDlg->SetOnlyRenderLimitTime( !pDlg->GetOnlyRenderLimitTime() );
	}

	if(CObject* const object = OBJECTMGR->GetObject(m_dwTargetID))															// ������Ʈ ������ ��ȿ�ϴٸ�,
	{
		object->ShowObjectName(
			FALSE,
			NAMECOLOR_DEFAULT);
		OBJECTMGR->SetSelectedObject(
			NULL);
	}
}

void CMacroManager::PM_ScreenCapture()
{
	DIRECTORYMGR->SetLoadMode(eLM_Root);											// �ε� ��带 eLM_Root�� �����Ѵ�.

	CreateDirectory( "ScreenShot", NULL );											// ��ũ�� �� ���丮�� �����Ѵ�.
	
	WORD wSrv = 0, wY = 0, wM = 0, wD = 0, wN = 0;									// ������ ���� �������� �����Ѵ�.
	WORD wHH = 0, wMM = 0, wSS = 0;													// �ð��븦 ���� �������� �����Ѵ�.

	char fname[256];																// ���ϸ��� ���� �ӽ� ���۸� �����Ѵ�.

	SYSTEMTIME ti;																	// �ð� ������ ���� ����ü�� �����Ѵ�.
	GetLocalTime( &ti );															// ���ýð��� �޾ƿ´�.

	HANDLE hFile = CreateFile( "screenshot/ScreenShot.cnt", GENERIC_READ, 0, NULL,	// �б���� ScreenShot.cnt�� �о� �����ڵ��� �Ѱ� �޴´�.
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

	DWORD dwSize = 0;																// ����� ���� ������ �����Ѵ�.

	if( hFile != INVALID_HANDLE_VALUE )												// �ڵ��� �Ѱܹ޴µ� �����ߴٸ�,
	{
		ReadFile( hFile, &wSrv, sizeof(wSrv), &dwSize, NULL );						// ������ ��ȣ�� �޴´�.
		ReadFile( hFile, &wY, sizeof(wY), &dwSize, NULL );							// �⵵�� �޴´�.
		ReadFile( hFile, &wM, sizeof(wM), &dwSize, NULL );							// ���� �޴´�.
		ReadFile( hFile, &wD, sizeof(wD), &dwSize, NULL );							// ������ �޴´�.
		ReadFile( hFile, &wHH, sizeof(wHH), &dwSize, NULL );						// �ð��� �޴´�.
		ReadFile( hFile, &wMM, sizeof(wMM), &dwSize, NULL );						// ���� �޴´�.
		ReadFile( hFile, &wSS, sizeof(wSS), &dwSize, NULL );						// �ʸ� �޴´�.
		ReadFile( hFile, &wN, sizeof(wN), &dwSize, NULL );							// �и� ������ ���� �޴´�.
		
		CloseHandle( hFile );														// ���� �ڵ��� �ݴ´�.

		if( !( wSrv == (WORD)g_nServerSetNum && ti.wYear == wY &&					// �о���� ��������
			ti.wMonth == wM && ti.wDay == wD && ti.wHour == wHH &&					// ���� �ð���� ���� ������,
			ti.wMinute == wMM && ti.wSecond == wSS) )
		{
			wSrv = WORD(g_nServerSetNum);
			wY = ti.wYear;
			wM = ti.wMonth;					// GetLocalTime()���� ���� ������ �������� �����Ѵ�.
			wD = ti.wDay;
			wN = 1; 
			wHH = ti.wHour;
			wMM = ti.wMinute;
			wSS = ti.wSecond;
		}					
	}
	else // �ڵ��� �޴µ� �����ߴٸ�,
	{
		wSrv = WORD(g_nServerSetNum);
		wY = ti.wYear;
		wM = ti.wMonth;						// GetLocalTime()���� ���� ������ �������� �����Ѵ�.
		wD = ti.wDay;
		wN = 1; 
		wHH = ti.wHour;
		wMM = ti.wMinute;
		wSS = ti.wSecond;
	}


	sprintf(fname,"ScreenShot/Luna_%02d_%02d%02d%02d_%02d%02d%02d_%03d.jpg",		// ���ϸ��� ������ ������ �������� ���Ͽ� �����Ѵ�.
		wSrv, wY%100, wM, wD, wHH, wMM, wSS, wN );

	BOOL rt = g_pExecutive->GetRenderer()->CaptureScreen(fname);					// ���ϸ����� ��ũ���� ĸ���Ѵ�.

	if( rt )																		// ĸ���� ����� �����̸�,
	{
		hFile = CreateFile( "screenshot/ScreenShot.cnt", GENERIC_WRITE, 0, NULL,	// ������� ScreenShot.cnt ������ ���� 
						CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );				// ���� �ڵ��� �Ѱ� �޴´�.

		if( hFile != INVALID_HANDLE_VALUE )											// ���� ���⿡ �����ߴٸ�,
		{
			++wN;																	// �и� ������ ���� ������Ų��.

			WriteFile( hFile, &wSrv, sizeof(wSrv), &dwSize, NULL );					// ������ ��ȣ�� ����.
			WriteFile( hFile, &wY, sizeof(wY), &dwSize, NULL );						// ������ ����Ѵ�.
			WriteFile( hFile, &wM, sizeof(wM), &dwSize, NULL );						// ���� ����Ѵ�.
			WriteFile( hFile, &wD, sizeof(wD), &dwSize, NULL );						// ������ ����Ѵ�.
			WriteFile( hFile, &wHH, sizeof(wHH), &dwSize, NULL );					// �ð��� ����Ѵ�.
			WriteFile( hFile, &wMM, sizeof(wMM), &dwSize, NULL );					// ���� ����Ѵ�.
			WriteFile( hFile, &wSS, sizeof(wSS), &dwSize, NULL );					// �ʸ� ����Ѵ�.
			WriteFile( hFile, &wN, sizeof(wN), &dwSize, NULL );						// �и� �ʸ� ����Ѵ�.

			CloseHandle( hFile );													// ���� �ڵ��� �ݴ´�.
		}

		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(387), fname );				// ��ũ���� ĸ�� �ߴٴ� �޽����� ����Ѵ�.
	}
	else																			// ���� ���⿡ �����ߴٸ�,
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(386) );					// ��ũ�� ĸ�� ���� �޽����� ����Ѵ�.
	}

	FIBITMAP* pBitmap = FreeImage_Load(FIF_TARGA, fname, TARGA_DEFAULT);			// ���� �̹��� ������ �ε��Ѵ�.
	
	if(pBitmap)																		// �̹��� ������ ��ȿ�ϸ�,
	{
		DeleteFile(fname);															// ������ �����Ѵ�.

		int len = strlen(fname);													// ���ϸ��� ���̸� ���Ѵ�.

		fname[len-3] = 'j';															// ���ϸ��� jpg�� �߰��Ѵ�.
		fname[len-2] = 'p';
		fname[len-1] = 'g';

		FreeImage_SaveJPEG(pBitmap,fname,JPEG_QUALITYSUPERB);						// jpg �̹����� ������ �Ѵ�.

		FreeImage_Unload(pBitmap);													// �޾ƿ� �̹��� ������ ��ȯ�Ѵ�.
	}
}

void CMacroManager::PM_UseMainQuickSlotItem( int nMacroEvent )
{
	WORD slotNum = USHRT_MAX;

	switch( nMacroEvent )															// ��ũ�� �̺�Ʈ�� Ȯ���Ѵ�.
	{
	case ME_USE_QUICKITEM_1_01:		slotNum = 0 ;	break ;							// ���� ��ȣ�� 0���� �����Ѵ�.
	case ME_USE_QUICKITEM_1_02:		slotNum = 1 ;	break ;							// ���� ��ȣ�� 1���� �����Ѵ�.
	case ME_USE_QUICKITEM_1_03:		slotNum = 2 ;	break ;							// ���� ��ȣ�� 2���� �����Ѵ�.
	case ME_USE_QUICKITEM_1_04:		slotNum = 3 ;	break ;							// ���� ��ȣ�� 3���� �����Ѵ�.
	case ME_USE_QUICKITEM_1_05:		slotNum = 4 ;	break ;							// ���� ��ȣ�� 4���� �����Ѵ�.
	case ME_USE_QUICKITEM_1_06:		slotNum = 5 ;	break ;							// ���� ��ȣ�� 5���� �����Ѵ�.
	case ME_USE_QUICKITEM_1_07:		slotNum = 6 ;	break ;							// ���� ��ȣ�� 6���� �����Ѵ�.
	case ME_USE_QUICKITEM_1_08:		slotNum = 7 ;	break ;							// ���� ��ȣ�� 7���� �����Ѵ�.
	case ME_USE_QUICKITEM_1_09:		slotNum = 8 ;	break ;							// ���� ��ȣ�� 8���� �����Ѵ�.
	case ME_USE_QUICKITEM_1_10:		slotNum = 9 ;	break ;							// ���� ��ȣ�� 9���� �����Ѵ�.
	}

	if(USHRT_MAX != slotNum)
	{
		QUICKMGR->UseQuickItem( GAMEIN->GetQuickDlg()->GetSlotDlg( 0 )->GetCurSheet(), slotNum );
	}
}

void CMacroManager::PM_UseSubQuickSlotItem( int nMacroEvent )
{
	WORD slotNum = USHRT_MAX;
																					
	switch( nMacroEvent )															// ��ũ�� �̺�Ʈ�� Ȯ���Ѵ�.
	{																				
	case ME_USE_QUICKITEM_2_01:		slotNum = 0 ;	break ;							// ���� ��ȣ�� 0���� �����Ѵ�.
	case ME_USE_QUICKITEM_2_02:		slotNum = 1 ;	break ;							// ���� ��ȣ�� 1���� �����Ѵ�.
	case ME_USE_QUICKITEM_2_03:		slotNum = 2 ;	break ;							// ���� ��ȣ�� 2���� �����Ѵ�.
	case ME_USE_QUICKITEM_2_04:		slotNum = 3 ;	break ;							// ���� ��ȣ�� 3���� �����Ѵ�.
	case ME_USE_QUICKITEM_2_05:		slotNum = 4 ;	break ;							// ���� ��ȣ�� 4���� �����Ѵ�.
	case ME_USE_QUICKITEM_2_06:		slotNum = 5 ;	break ;							// ���� ��ȣ�� 5���� �����Ѵ�.
	case ME_USE_QUICKITEM_2_07:		slotNum = 6 ;	break ;							// ���� ��ȣ�� 6���� �����Ѵ�.
	case ME_USE_QUICKITEM_2_08:		slotNum = 7 ;	break ;							// ���� ��ȣ�� 7���� �����Ѵ�.
	case ME_USE_QUICKITEM_2_09:		slotNum = 8 ;	break ;							// ���� ��ȣ�� 8���� �����Ѵ�.
	case ME_USE_QUICKITEM_2_10:		slotNum = 9 ;	break ;							// ���� ��ȣ�� 9���� �����Ѵ�.
	}																				
																					
	if(USHRT_MAX != slotNum)
	{		
		QUICKMGR->UseQuickItem(
			GAMEIN->GetQuickDlg()->GetSlotDlg(1)->GetCurSheet() + MAX_SLOTPAGE,
			slotNum);
	}
}

void CMacroManager::PM_Change_MainSlotNum( int nMacroEvent )
{
	WORD slotNum = USHRT_MAX;
																					
	switch( nMacroEvent )															// ��ũ�� �̺�Ʈ�� Ȯ���Ѵ�.
	{																				
	case ME_CHANGE_QUICKSLOT_01:		slotNum = 0 ;	break ;						// ���� ��ȣ�� 0���� �����Ѵ�.
	case ME_CHANGE_QUICKSLOT_02:		slotNum = 1 ;	break ;						// ���� ��ȣ�� 1���� �����Ѵ�.
	case ME_CHANGE_QUICKSLOT_03:		slotNum = 2 ;	break ;						// ���� ��ȣ�� 2���� �����Ѵ�.
	case ME_CHANGE_QUICKSLOT_04:		slotNum = 3 ;	break ;						// ���� ��ȣ�� 3���� �����Ѵ�.
	case ME_CHANGE_QUICKSLOT_05:		slotNum = 4 ;	break ;						// ���� ��ȣ�� 4���� �����Ѵ�.
	case ME_CHANGE_QUICKSLOT_06:		slotNum = 5 ;	break ;						// ���� ��ȣ�� 5���� �����Ѵ�.
	case ME_CHANGE_QUICKSLOT_07:		slotNum = 6 ;	break ;						// ���� ��ȣ�� 6���� �����Ѵ�.
	case ME_CHANGE_QUICKSLOT_08:		slotNum = 7 ;	break ;						// ���� ��ȣ�� 7���� �����Ѵ�.
	}																				
																					
	if(USHRT_MAX != slotNum)
	{
		if(cQuickSlotDlg* pSlot = (cQuickSlotDlg*)WINDOWMGR->GetWindowForID(QI1_QUICKSLOTDLG))
		{
			pSlot->SelectPage(slotNum);
		}
	}
}

void CMacroManager::PM_SetSlotPage( DWORD dwSlotID, int nMacroEvent )
{
	cQuickSlotDlg* pSlot = ( cQuickSlotDlg* )WINDOWMGR->GetWindowForID( dwSlotID ) ;// ���� ������ ���´�.

	if(pSlot)																		// ���� ������ ��ȿ���� üũ�Ѵ�.
	{
		WORD curPage = pSlot->GetCurSheet() ;										// ���� �������� �޾ƿ´�.

		switch( nMacroEvent )														// ��ũ�� �̺�Ʈ�� Ȯ���Ѵ�.
		{
		case ME_INCREASE_MAIN_QUICKSLOT_NUM :										// �������� ������Ű�� �̺�Ʈ���,
		case ME_INCREASE_SUB_QUICKSLOT_NUM :
			{
				if( curPage >= MAX_SLOTPAGE-1 )										// �������� ������ ���������,
				{
					curPage = 0 ;													// ������ �������� �����Ѵ�.
				}
				else																// �׷��� �������,
				{
					curPage = curPage+1 ;											// �������� �ϳ� ������Ų��.
				}
			}
			break ;

		case ME_DECREASE_MAIN_QUICKSLOT_NUM :										// �������� ���ҽ�Ű�� �̺�Ʈ���,
		case ME_DECREASE_SUB_QUICKSLOT_NUM :
			{
				if( curPage <= 0 )													// �������� �ּ� ���������,
				{
					curPage = MAX_SLOTPAGE-1 ;										// ù �������� �����Ѵ�.
				}
				else																// �׷��� �������,
				{
					curPage = curPage-1 ;											// �������� �ϳ� ���ҽ�Ų��.
				}
			}
			break ;
		}

		pSlot->SelectPage( curPage );												// �������� �����Ѵ�.
	}
}

void CMacroManager::PM_SelectPartyMember( int nMacroEvent )
{
	DWORD dwMemberID = 0;

	if(PARTYMGR->GetParty().GetMasterIdx() == HEROID)
	{
		switch(nMacroEvent)
		{
		case ME_SELECT_PARTYMEMBER_1: dwMemberID = PARTYMGR->GetParty().GetMemberID(1);	break;
		case ME_SELECT_PARTYMEMBER_2: dwMemberID = PARTYMGR->GetParty().GetMemberID(2);	break;
		case ME_SELECT_PARTYMEMBER_3: dwMemberID = PARTYMGR->GetParty().GetMemberID(3);	break;
		case ME_SELECT_PARTYMEMBER_4: dwMemberID = PARTYMGR->GetParty().GetMemberID(4);	break;
		case ME_SELECT_PARTYMEMBER_5: dwMemberID = PARTYMGR->GetParty().GetMemberID(5);	break;
		case ME_SELECT_PARTYMEMBER_6: dwMemberID = PARTYMGR->GetParty().GetMemberID(6);	break;
		}
	}
	else
	{
		DWORD dwMemberOrder[MAX_PARTY_LISTNUM] = {0};
		int orderCount = 0;

		for(int count = 0; count < _countof(dwMemberOrder); ++count)
		{
			DWORD dwCurMemberID = PARTYMGR->GetParty().GetMemberID(count);

			if(dwCurMemberID == gHeroID)
			{
				continue;
			}

			dwMemberOrder[orderCount] = dwCurMemberID;
			++orderCount;
		}

		switch(nMacroEvent)
		{
		case ME_SELECT_PARTYMEMBER_1: dwMemberID = dwMemberOrder[0]; break;
		case ME_SELECT_PARTYMEMBER_2: dwMemberID = dwMemberOrder[1]; break;
		case ME_SELECT_PARTYMEMBER_3: dwMemberID = dwMemberOrder[2]; break;
		case ME_SELECT_PARTYMEMBER_4: dwMemberID = dwMemberOrder[3]; break;
		case ME_SELECT_PARTYMEMBER_5: dwMemberID = dwMemberOrder[4]; break;
		case ME_SELECT_PARTYMEMBER_6: dwMemberID = dwMemberOrder[5]; break;
		}
	}

	if(dwMemberID)
	{
		GAMEIN->GetPartyDialog()->SetClickedMemberID(dwMemberID);
	}
}
void CMacroManager::PM_Toggle_AllInterface()
{
	// 071205 LUJ	NPC �̹��� â���� ��ü������ ��� â�� ����Ѵ�. ���� �ش� â�� �������� ��
	//				��� ������ �����ϸ� �ܵ����� ǥ�õǾ���� NPC â ���� �ٸ� â���� ���ļ� 
	//				ǥ�õǴ� ������ �߻��Ѵ�
	{
		cDialog* dialog = WINDOWMGR->GetWindowForID( NSI_SCRIPTDLG );
		ASSERT( dialog );

		if( dialog->IsActive() )
		{
			return;
		}
	}

	if( WINDOWMGR->IsOpendAllWindows() )											// ��������� �����ִ� ���¶��,
	{
		//WINDOWMGR->m_pActivedWindowList->RemoveAll() ;								// �����ִ� �����츦 ����ִ� ����Ʈ�� ��� ����.
		//WINDOWMGR->CloseAllWindow() ;												// ��� �����츦 �ݴ´�.
		
		//090119 pdy Window Hide Mode
		WINDOWMGR->HideAllWindow() ;												//��� �����츦 �����.											

		WINDOWMGR->SetOpendAllWindows( FALSE ) ;									// ��� �����찡 �����ִ� ���·� �����Ѵ�.
	}
	else																			// ��������� �����ִ� ���¶��,
	{
		//WINDOWMGR->ShowAllActivedWindow() ;											// Ȱ��ȭ �� ��� ��������� �����ش�.

		//090119 pdy Window Hide Mode
		WINDOWMGR->UnHideAllActivedWindow();										// ������ִ� ������ ��������� UnHide�Ѵ�.

		WINDOWMGR->SetOpendAllWindows( TRUE ) ;										// ��� �����찡 �����ִ� ���·� �����Ѵ�.

		WINDOWMGR->ShowBaseWindow() ;												// �⺻������ ���̴� ��������� �����ش�.
	}
}

void CMacroManager::PM_Toggle_SkillDlg()
{
	cSkillTreeDlg* pDlg = GAMEIN->GetSkillTreeDlg() ;								// ��ųâ ������ �޾ƿ´�.

	if( pDlg )																		// ��ųâ ������ ��ȿ���� üũ�Ѵ�.
	{
		pDlg->SetActive( !pDlg->IsActive() );										// ��ųâ�� Ȱ��, ��Ȱ��ȭ �Ѵ�.
	}
}

void CMacroManager::PM_Toggle_FamilyDlg()
{
	CSHFamily* pFamily = HERO->GetFamily() ;	

	if( pFamily && pFamily->Get()->nMasterID != 0 )
	{
		CFamilyDialog* pDlg = GAMEIN->GetFamilyDlg() ;

		if( pDlg )
		{
			if( pDlg->IsActive() )
			{
				pDlg->SetActive(FALSE) ;
			}
			else
			{
				pDlg->UpdateAllInfo() ;
				pDlg->SetActive(TRUE) ;
			}
		}
	}
	else
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 369 ));
	}
}

void CMacroManager::PM_Toggle_InventoryDlg()
{
	CInventoryExDialog* pDlg = GAMEIN->GetInventoryDialog() ;						// �κ��丮 ���̾�α� ������ �޾ƿ´�.

	if( !pDlg ) return ;															// ���̾�α� ������ ��ȿ���� üũ�Ѵ�.
	
	pDlg->SetActive( !pDlg->IsActive() ) ;											// ���̾�α׸� Ȱ��, ��Ȱ��ȭ �Ѵ�.
}

void CMacroManager::PM_Toggle_ShowKeyTip()
{
	if(CKeySettingTipDlg* pDlg = GAMEIN->GetKeySettingTipDlg())
	{
		pDlg->SetMode(
			WORD(m_nMacroMode));
		pDlg->SetActive(
			! pDlg->IsActive());
	}
}

void CMacroManager::PM_Toggle_BigMap()
{
	CBigMapDlg* pDlg = GAMEIN->GetBigMapDialog();
	if( pDlg )
	{
		pDlg->SetActive( !pDlg->IsActive() );
	}
}

void CMacroManager::PM_Toggle_WorldMap()
{
	CWorldMapDlg* pDlg = GAMEIN->GetWorldMapDialog();
	if( pDlg )
	{
		pDlg->SetActive( !pDlg->IsActive() );
	}
}

void CMacroManager::PM_Toggle_OptionDlg()
{
	COptionDialog* pDlg = GAMEIN->GetOptionDialog() ;								// �ɼ� ���̾�α� ������ �޾ƿ´�.

	if( pDlg )																		// ���̾�α� ������ ��ȿ���� üũ�Ѵ�.
	{
		pDlg->SetActive( !pDlg->IsActive() ) ;										// ���̾�α׸� Ȱ��, ��Ȱ��ȭ �Ѵ�.
	}
}

void CMacroManager::PM_Toggle_GuildDlg() 
{
	// CGuildDialog;
	cDialog* dialog = WINDOWMGR->GetWindowForID( GD_GUILDDLG );
	ASSERT( dialog );

	dialog->SetActive( ! dialog->IsActive() );
}

void CMacroManager::PM_Toggle_QuestDlg()
{
	CQuestTotalDialog* pDlg = GAMEIN->GetQuestTotalDialog() ;						// ����Ʈ ���̾�α� ������ �޾ƿ´�.

	if( pDlg )																		// ���̾�α� ������ ��ȿ���� üũ�Ѵ�.
	{
		pDlg->SetActive( !pDlg->IsActive() ) ;										// ���̾�α׸� Ȱ��, ��Ȱ��ȭ �Ѵ�.

		CQuestDialog* pQuestDialog = GAMEIN->GetQuestDialog() ;

		if( pQuestDialog )
		{
			if( pQuestDialog->GetQuestCount() > 0 )
			{
				DWORD dwQuestIdx = 0 ;
				dwQuestIdx = pQuestDialog->GetSelectedQuestID() ;

				if( dwQuestIdx < 1 ) 
				{
					DWORD questIndex = pQuestDialog->GetSelectedQuestIDFromTree( 0 );
					pQuestDialog->SetSelectedQuestIdx( questIndex );
				}
				pQuestDialog->RefreshQuestList();
			}
		}
	}
}

void CMacroManager::PM_Toggle_CharInfoDlg()
{
	CCharacterDialog* pDlg = GAMEIN->GetCharacterDialog() ;

	if( pDlg )																		// ���̾�α� ������ ��ȿ���� üũ�Ѵ�.
	{
		pDlg->RefreshFamilyInfo() ;													// �йи� ������ ���ΰ�ħ �Ѵ�.
		pDlg->SetActive( !pDlg->IsActive() ) ;										// ���̾�α׸� Ȱ��, ��Ȱ��ȭ �Ѵ�.
	}
}

void CMacroManager::PM_Toggle_MatchingDlg()
{
	const DATE_MATCHING_INFO& dateInfo = HERO->GetDateMatchingInfo() ;				// ��Ī ������ �޾ƿ´�.

	if(!dateInfo.bIsValid)															// ��Ī ������ ��ȿ���� ������,
	{
		CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg( 1284 ) );
	}
	else																			// ��Ī ������ ��ȿ�ϴٸ�,
	{
		CDateMatchingDlg* pDlg = GAMEIN->GetDateMatchingDlg() ;						// ��Ī ���̾�α� ������ �޾ƿ´�.

		if( pDlg )																	// ���̾�α� ������ ��ȿ���� üũ�Ѵ�.
		{
			if( pDlg->IsActive() )													// ���̾�αװ� ���� �ִٸ�,
			{
				CDateMatchingChatDlg* pChatDlg = NULL ;								// ��Ī ä�� ���̾�α� ������ ���� ������ ������ �����Ѵ�.

				pChatDlg = GAMEIN->GetDateMatchingDlg()->GetChatingDlg() ;			// ��Ī ���̾�α� ������ �޾ƿ´�.

				if( pChatDlg )														// ���̾�α� ������ ��ȿ���� üũ�Ѵ�.
				{
					if( pChatDlg->IsOnChatMode() )									// ä�� ���̶��,
					{
						// notice end chatting.
						WINDOWMGR->MsgBox( MBI_END_CHATTING_CLOSE, MBT_YESNO, CHATMGR->GetChatMsg( 1304 ) );
					}
					else															// ä�� ���� �ƴ϶��,
					{
						pDlg->SetActive(FALSE) ;									// ���̾�α׸� ��Ȱ��ȭ �Ѵ�.
					}
				}
			}
			else																	// ä�� ���̾�α״� ��Ȱ��ȭ ���̶��,
			{
				pDlg->GetPartnerListDlg()->SettingControls() ;						// ��Ʈ�� ����Ʈ�� ����ѵ��� �����Ѵ�.
				//pDlg->GetPartnerListDlg()->UpdateInfo() ;
				pDlg->SetActive(TRUE) ;												// ä�� ���̾�α׸� Ȱ��ȭ �Ѵ�.
			}
		}
	}
}

void CMacroManager::PM_SetAutoTarget()
{
	cDialog* const dialog = WINDOWMGR->GetWindowForID(
		NSI_SCRIPTDLG);

	if(0 == dialog)
	{
		return;
	}
	else if(dialog->IsActive())
	{
		return;
	}

	SHORT_RECT screenRect = {0};
	WORD value = 0;
	g_pExecutive->GetRenderer()->GetClientRect(
		&screenRect,
		&value,
		&value);

	RECT rect = {0};
	rect.left = screenRect.left;
	rect.top = screenRect.top;
	rect.right = screenRect.right;
	rect.bottom = screenRect.bottom;

	const size_t maxFindSize = 200;
	PICK_GXOBJECT_DESC descriptionArray[maxFindSize] = {0};
	const DWORD pickedSize = g_pExecutive->GXOGetMultipleObjectWithScreenCoordRect(
		descriptionArray,
		_countof(descriptionArray),
		&rect,
		0,
		PICK_TYPE_PER_COLLISION_MESH);

	typedef DWORD ObjectIndex;
	typedef std::list< ObjectIndex > ObjectIndexContainer;
	static ObjectIndexContainer objectIndexContainer;
	static PICK_GXOBJECT_DESC previousDescriptionArray[maxFindSize] = {0};
	static BOOL isFirstTarget = TRUE;

	if(0 == memcmp(descriptionArray, previousDescriptionArray, sizeof(descriptionArray)))
	{
		isFirstTarget = ! isFirstTarget;
	}
	else
	{
		isFirstTarget = TRUE;

		memcpy(
			previousDescriptionArray,
			descriptionArray,
			sizeof(descriptionArray));

		VECTOR3 heroPosition = {0};
		CHero* const hero = OBJECTMGR->GetHero();
		hero->GetPosition(
			&heroPosition);

		typedef float Distance;
		typedef std::map< Distance, ObjectIndex > DistanceContainer;
		DistanceContainer distanceContainer;

		for(DWORD i = pickedSize; i-- > 0;)
		{
			const PICK_GXOBJECT_DESC& description = descriptionArray[i];

			CEngineObject* const engineObject = (CEngineObject*)g_pExecutive->GetData(
				description.gxo);

			if(0 == engineObject)
			{
				continue;
			}

			CObject* const object = (CObject*)engineObject->GetBaseObject();

			if(0 == object)
			{
				continue;
			}
			else if(gHeroID == object->GetID())
			{
				continue;
			}
			else if(eObjectKind_Monster == object->GetObjectKind())
			{
				CMonster* const monsterObject = (CMonster*)object;

				if(const BOOL isPuppy = (2 == monsterObject->GetSInfo()->SpecialType))
				{
					continue;
				}
			}

			if(eObjectBattleState_Battle == hero->GetObjectBattleState())
			{
				if(FALSE == (eObjectKind_Monster & object->GetObjectKind()))
				{
					continue;
				}
				else if(eObjectKind_Vehicle == object->GetObjectKind())
				{
					continue;
				}
			}

			VECTOR3 objectPosition = {0};
			object->GetPosition(
				&objectPosition);

			Distance distance = CalcDistance(
				&heroPosition,
				&objectPosition);

			while(distanceContainer.end() != distanceContainer.find(distance))
			{
				distance += 0.001f;
			}

			distanceContainer[distance] = object->GetID();
		}

		objectIndexContainer.clear();

		CObject* pTargetObject	=	NULL;

		for(DistanceContainer::const_iterator iterator = distanceContainer.begin();
			distanceContainer.end() != iterator;
			++iterator)
		{
			const ObjectIndex objectIndex = iterator->second;

			// 100803 �÷��̾�, Ż��, �� ����
			pTargetObject	=	OBJECTMGR->GetObject( objectIndex );
			if( pTargetObject )
			{
				if( pTargetObject->GetObjectKind() != eObjectKind_Player &&
					pTargetObject->GetObjectKind() != eObjectKind_Vehicle &&
					pTargetObject->GetObjectKind() != eObjectKind_Pet )
				{
					objectIndexContainer.push_back(objectIndex);
				}
			}

			if(1 < objectIndexContainer.size())
			{
				break;
			}
		}
	}

	CObject* const heroObject = OBJECTMGR->GetObject(
		gHeroID);

	if(0 == heroObject)
	{
		return;
	}

	ObjectIndex targetObjectIndex = 0;

	if(objectIndexContainer.empty())
	{
		OBJECTMGR->SetSelectedObject(
			0);
		targetObjectIndex = 0;
	}
	else if(1 == objectIndexContainer.size() ||
		isFirstTarget)
	{
		targetObjectIndex = (*objectIndexContainer.begin());
	}
	else
	{
		targetObjectIndex = *(++objectIndexContainer.begin());
	}

	CObject* const targetObject = OBJECTMGR->GetObject(
		targetObjectIndex);

	if(0 == targetObject)
	{
		return;
	}

	OBJECTMGR->SetSelectedObject(
		targetObject);

	targetObject->ShowObjectName(
		TRUE,
		RGB_HALF(255, 141, 39));

	// Ÿ���� �Ĵٺ��� �ڵ�. ���� ���� �̵� �߿� ���� �翬�� ����ϴ�
	if(FALSE == heroObject->IsDied() &&
		FALSE == heroObject->GetBaseMoveInfo()->bMoving)
	{
		VECTOR3 targetObjectPosition = {0};
		targetObject->GetPosition(
			&targetObjectPosition);

		VECTOR3 heroPosition = {0};
		heroObject->GetPosition(
			&heroPosition);

		VECTOR3 position = targetObjectPosition - heroPosition;
		heroObject->GetEngineObject()->SetDirection(
			&position);
	}
}

void CMacroManager::PM_Toggle_FriendDlg()
{
	cDialog* dialog = WINDOWMGR->GetWindowForID( FRI_FRIENDDLG );
	ASSERT( dialog );

	if( dialog->IsActive() )
	{
		dialog->SetActive( FALSE );
	}
	else
	{
		FRIENDMGR->FriendListSyn( 1 );
	}
}


void CMacroManager::PM_Toggle_HelpDlg()
{
	cDialog* pDlg = WINDOWMGR->GetWindowForID(HELP_DLG) ;
	ASSERT( pDlg ) ;

	pDlg->SetActive( !pDlg->IsActive() ) ;
}



void CMacroManager::PM_Toggle_GTResultDlg()
{
#ifdef _GMTOOL_
	CGTScoreInfoDialog* pScoreDlg = (CGTScoreInfoDialog*)WINDOWMGR->GetWindowForID(GTSCORETIMEDLG) ;
	if(!pScoreDlg)		return;

	CGTResultDlg* pResultDlg = (CGTResultDlg*)WINDOWMGR->GetWindowForID(GTRESULTDLG) ;
	if(!pResultDlg)		return;

	if(pScoreDlg->IsBattle())
	{
		if(pResultDlg->IsActive())
			pResultDlg->SetActive(FALSE);
		else
			pResultDlg->SetResult();
	}
#endif
}


// 080429 LYW --- MacroManager : Add functions to toggle chatroom part.
void CMacroManager::PM_Toggle_ChatRoomListDlg()
{
	cDialog* pDlg = NULL ;
	pDlg = WINDOWMGR->GetWindowForID(CRMD_DLG) ;

	ASSERT(pDlg) ;

	if(!pDlg)
	{
		CHATROOMMGR->Throw_Error("Failed to receive crmd!!", __FUNCTION__) ;
		return ;
	}

	//pDlg->SetActive(!pDlg->IsActive()) ;
	if(pDlg->IsActive())
	{
		pDlg->SetActive(FALSE) ;
	}
	else
	{
		CHATROOMMGR->ToggleChatRoomMainDlg() ;
	}
}





void CMacroManager::PM_Toggle_ChatRoomDlg()
{
	if(!CHATROOMMGR->GetMyRoomOwnerIdx())
	{
		CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1498)) ;
		return ;
	}

	// ä�ù� ���̾�α�.
	cDialog* pDlg = NULL ;
	pDlg = WINDOWMGR->GetWindowForID(CRD_DLG) ;

	ASSERT(pDlg) ;

	if(!pDlg)
	{
		CHATROOMMGR->Throw_Error("Failed to receive crd!!", __FUNCTION__) ;
		return ;
	}

	pDlg->SetActive(!pDlg->IsActive()) ;

	// ������ â.
	cDialog* pGuestDlg = NULL ;
	pGuestDlg = WINDOWMGR->GetWindowForID(CRGLD_DLG) ;

	ASSERT(pGuestDlg) ;

	if(!pGuestDlg)
	{
		CHATROOMMGR->Throw_Error("Failed to receive crd!!", __FUNCTION__) ;
		return ;
	}

	if(pGuestDlg->IsActive()) pGuestDlg->SetActive(FALSE) ;
	//pGuestDlg->SetActive(!pDlg->IsActive()) ;

	// �ɼ� â.
	cDialog* pOptionDlg = NULL ;
	pOptionDlg = WINDOWMGR->GetWindowForID(CROD_DLG) ;

	ASSERT(pOptionDlg) ;

	if(!pOptionDlg)
	{
		CHATROOMMGR->Throw_Error("Failed to receive crd!!", __FUNCTION__) ;
		return ;
	}

	if(pOptionDlg->IsActive()) pOptionDlg->SetActive(FALSE) ;
	//pOptionDlg->SetActive(!pDlg->IsActive()) ;
}





void CMacroManager::PM_SetFocus_ChatRoom_Chat()
{
	// ä�ù� ���������� üũ.
	if(!CHATROOMMGR->GetMyRoomOwnerIdx())
	{
		CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1498)) ;
		return ;
	}



	// ä�ù� ���̾�α�.
	cDialog* pDlg = NULL ;
	pDlg = WINDOWMGR->GetWindowForID(CRD_DLG) ;

	ASSERT(pDlg) ;

	if(!pDlg)
	{
		CHATROOMMGR->Throw_Error("Failed to receive crd!!", __FUNCTION__) ;
		return ;
	}

	// ä�ù��� ��Ȱ��ȭ �Ǿ��ִٸ�, return ó���� �Ѵ�.
	if(!pDlg->IsActive()) return ;



	// ä�ù��� ����Ʈ�ڽ� �ޱ�.
	cEditBox* roomBox = NULL ;
	roomBox = CHATROOMMGR->GetChatRoomInputBox() ;

	ASSERT(roomBox) ;

	if(!roomBox)
	{
		CHATROOMMGR->Throw_Error("Failed to receive creb!!", __FUNCTION__) ;
		return ;
	}

	cEditBox* chatBox = GAMEIN->GetChattingDlg()->GetInputBox();

	if(roomBox->IsFocus())
	{
		roomBox->SetFocus(FALSE) ;
		roomBox->SetFocusEdit(FALSE) ;
		CIMEWND->SetDockingEx(roomBox->GetIME(), FALSE) ;
		chatBox->SetFocus(TRUE) ;
		chatBox->SetFocusEdit(TRUE) ;
		CIMEWND->SetDockingEx(chatBox->GetIME(), TRUE) ;
	}
	else if(chatBox->IsFocus())
	{
		chatBox->SetFocus(FALSE) ;
		chatBox->SetFocusEdit(FALSE) ;
		CIMEWND->SetDockingEx(chatBox->GetIME(), FALSE) ;
		roomBox->SetFocus(TRUE) ;
		roomBox->SetFocusEdit(TRUE) ;
		CIMEWND->SetDockingEx(roomBox->GetIME(), TRUE) ;
	}
	else
	{
		roomBox->SetFocus(TRUE) ;
		roomBox->SetFocusEdit(TRUE) ;
		CIMEWND->SetDockingEx(roomBox->GetIME(), TRUE) ;
	}
}


void CMacroManager::PM_Toggle_Pet_Info()
{
	if( !HEROPET )
		return;

	CPetInfoDialog* pDlg = GAMEIN->GetPetInfoDlg();

	if( !pDlg )
	{
		return;
	}

	if( pDlg->IsActive() )
	{
		pDlg->SetActive( FALSE );
	}
	else
	{
		pDlg->SetActive( TRUE );
	}
}

// 090422 ShinJS --- Ż�� ž��/����
void CMacroManager::PM_Toggle_Vehicle_GetOnOff()
{
	// 091214 ShinJS --- ž�� ���� ����
	// ž�� ����
	if( HERO->IsGetOnVehicle() )
	{
		// ����
		// 090316 LUJ, ������ ��û�Ѵ�
		MSGBASE message;
		ZeroMemory( &message, sizeof( message ) );
		message.Category = MP_VEHICLE;
		message.Protocol = MP_VEHICLE_DISMOUNT_SYN;
		message.dwObjectID = HEROID;
		NETWORK->Send( &message, sizeof( message ) );
	}
	// ��ž�� ����
	else
	{
		// ž��
		// Ż�� ���� ���� Ȯ��
		const DWORD dwVehicleID = HERO->GetVehicleID();
		CVehicle* pVehicle = (CVehicle*)OBJECTMGR->GetObject( dwVehicleID );
		if( !pVehicle || 
			pVehicle->GetObjectKind() != eObjectKind_Vehicle ||
			pVehicle->GetOwnerIndex() != gHeroID)
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1829 ) );
			return;
		}

		DWORD dwEmptySeatPos = pVehicle->CanGetOn( HERO );

		// ž�� �Ұ����� ���
		if( dwEmptySeatPos == 0 )
		{
			return;
		}

		MSG_DWORD message;
		ZeroMemory( &message, sizeof( message ) );
		message.Category = MP_VEHICLE;
		message.Protocol = MP_VEHICLE_MOUNT_ASK_SYN;
		message.dwObjectID = HEROID;
		message.dwData = pVehicle->GetID();
		NETWORK->Send( &message, sizeof( message ) );
	}
}

void CMacroManager::PlayMacroPress( int nMacroEvent )
{
	float Rate = 1.f;																// ȸ�� ��.

	if( g_UserInput.GetKeyboard()->GetKeyPressed(KEY_RSHIFT) )						// ������ ����Ʈ�� ���ȴٸ�,
	{
		Rate = 0.3f;																// ȸ�� ���� 0.3f�� �����Ѵ�.
	}

	switch( nMacroEvent )															// ���ڷ� ���� ��ũ�� �̺�Ʈ�� Ȯ���Ѵ�.
	{
		case MPE_CAMERAMOVE_ZOOMIN:													// ���� ��ũ�ζ��,
			{

#ifdef _GMTOOL_																		// GM���� �� ó���� �����Ѵ�.
				if( CAMERA->GetCameraMode() == eCM_EyeView )						// ī�޶� ��尡 eCM_EyeView�� ���ٸ�,
				{
					CAMERA->Fly( 0.8f * gTickTime*Rate*5 );							// �ɹ̶��� fly �Լ��� �����Ѵ�.				
				}
				else
#endif
					CAMERA->ZoomDistance( 0, -0.8f * gTickTime*Rate, 0 );
			}
			break;

		case MPE_CAMERAMOVE_ZOOMOUT:												// �� �ƿ� ��ũ�ζ��,
			{

#ifdef _GMTOOL_																		// GM�� ó���� �����Ѵ�.

				if( CAMERA->GetCameraMode() == eCM_EyeView )						// ī�޶� ��尡 eCM_EyeView�� ���ٸ�,
				{
					CAMERA->Fly( -0.8f * gTickTime*Rate*5 );							// �־��� ������ ī�޶��� Fly �Լ��� �����Ѵ�.
				}
				else
#endif
					CAMERA->ZoomDistance( 0, 0.8f * gTickTime*Rate, 0 );				// �־��� ������ ī�޶� �� �Ÿ��� �����Ѵ�.
			}
			break;

		case MPE_CAMERAMOVE_UP:														// ī�޶� �� ��ũ�ζ��,
			CAMERA->MouseRotate(
				0,
				int(0.3f * gTickTime * Rate));
			break;

		case MPE_CAMERAMOVE_DOWN:													// ī�޶� �ٿ� ��ũ�ζ��,
			CAMERA->MouseRotate(
				0,
				int(-0.3f * gTickTime*Rate));
			break;

		case MPE_CAMERAMOVE_RIGHT:													// ī�޶� ����Ʈ ��ũ�ζ��,
			CAMERA->MouseRotate(
				int(-0.3f * gTickTime*Rate),
				0);
			break;

		case MPE_CAMERAMOVE_LEFT:													// ī�޶� ����Ʈ ��ũ�ζ��,
			CAMERA->MouseRotate(
				int(0.3f * gTickTime*Rate),
				0);
			break;

	}
}

void CMacroManager::ForceSetFocus()
{
	// �������� ������ _g_hWnd�� ��Ŀ���� �ǵ��� ���� ���ϴ� ���
	// �ٽ� �������ش�.
	extern HWND _g_hWnd;
	SetFocus(_g_hWnd);
}

void CMacroManager::Init()
{

	sMACRO DefaultMacro[MM_COUNT][ME_COUNT] = {
		{
			{ MSK_NONE,		KEY_F1,	FALSE, FALSE },									// ���� ������ 1��Ű ���
			{ MSK_NONE,		KEY_F2,	FALSE, FALSE },									// ���� ������ 2��Ű ���
			{ MSK_NONE,		KEY_F3,	FALSE, FALSE },									// ���� ������ 3��Ű ���
			{ MSK_NONE,		KEY_F4,	FALSE, FALSE },									// ���� ������ 4��Ű ���
			{ MSK_NONE,		KEY_F5,	FALSE, FALSE },									// ���� ������ 5��Ű ���
			{ MSK_NONE,		KEY_F6,	FALSE, FALSE },									// ���� ������ 6��Ű ���
			{ MSK_NONE,		KEY_F7,	FALSE, FALSE },									// ���� ������ 7��Ű ���
			{ MSK_NONE,		KEY_F8,	FALSE, FALSE },									// ���� ������ 8��Ű ���
			{ MSK_NONE,		KEY_F9,	FALSE, FALSE },									// ���� ������ 9��Ű ���
			{ MSK_NONE,		KEY_F10,FALSE, FALSE },									// ���� ������10��Ű ���

			{ MSK_CTRL,		KEY_F1,	FALSE, FALSE },									// ���� ������ 1��Ű ��� 
			{ MSK_CTRL,		KEY_F2,	FALSE, FALSE },									// ���� ������ 2��Ű ��� 
			{ MSK_CTRL,		KEY_F3,	FALSE, FALSE },									// ���� ������ 3��Ű ��� 
			{ MSK_CTRL,		KEY_F4,	FALSE, FALSE },									// ���� ������ 4��Ű ��� 
			{ MSK_CTRL,		KEY_F5,	FALSE, FALSE },									// ���� ������ 5��Ű ��� 
			{ MSK_CTRL,		KEY_F6,	FALSE, FALSE },									// ���� ������ 6��Ű ��� 
			{ MSK_CTRL,		KEY_F7,	FALSE, FALSE },									// ���� ������ 7��Ű ��� 
			{ MSK_CTRL,		KEY_F8,	FALSE, FALSE },									// ���� ������ 8��Ű ��� 
			{ MSK_CTRL,		KEY_F9,	FALSE, FALSE },									// ���� ������ 9��Ű ��� 
			{ MSK_CTRL,		KEY_F10,FALSE, FALSE },									// ���� ������10��Ű ��� 

			{ MSK_ALT,		KEY_1,	FALSE, FALSE },									// ���� �������� 1������ ������.
			{ MSK_ALT,		KEY_2,	FALSE, FALSE },									// ���� �������� 2������ ������.
			{ MSK_ALT,		KEY_3,	FALSE, FALSE },									// ���� �������� 3������ ������.
			{ MSK_ALT,		KEY_4,	FALSE, FALSE },									// ���� �������� 4������ ������.
			{ MSK_ALT,		KEY_5,	FALSE, FALSE },									// ���� �������� 5������ ������.
			{ MSK_ALT,		KEY_6,	FALSE, FALSE },									// ���� �������� 6������ ������.
			{ MSK_ALT,		KEY_7,	FALSE, FALSE },									// ���� �������� 7������ ������.
			{ MSK_ALT,		KEY_8,	FALSE, FALSE },									// ���� �������� 8������ ������.

			{ MSK_ALT,		KEY_OEM_3, FALSE, FALSE },								// ���� ������ ��ȣ�� ���� ��Ų��.
			{ MSK_CTRL,		KEY_OEM_3, FALSE, FALSE },								// ���� ������ ��ȣ�� ���� ��Ų��.

			{ MSK_ALT | MSK_SHIFT,	KEY_OEM_3, FALSE, FALSE },						// ���� ������ ��ȣ�� ���� ��Ų��.
			{ MSK_CTRL | MSK_SHIFT,	KEY_OEM_3, FALSE, FALSE },						// ���� ������ ��ȣ�� ���� ��Ų��.

			{ MSK_ALT,		KEY_F1,	FALSE, FALSE },									// �� �ڽ��� �����Ѵ�.

			{ MSK_ALT,		KEY_F2,	FALSE, FALSE },									// ù ��° ��Ƽ ����� �����Ѵ�.
			{ MSK_ALT,		KEY_F3,	FALSE, FALSE },									// �� ��° ��Ƽ ����� �����Ѵ�.
			{ MSK_ALT,		KEY_F4,	FALSE, FALSE },									// �� ��° ��Ƽ ����� �����Ѵ�.
			{ MSK_ALT,		KEY_F5,	FALSE, FALSE },									// �� ��° ��Ƽ ����� �����Ѵ�.
			{ MSK_ALT,		KEY_F6,	FALSE, FALSE },									// �ټ� ��° ��Ƽ ����� �����Ѵ�.
			{ MSK_ALT,		KEY_F7,	FALSE, FALSE },									// ���� ��° ��Ƽ ����� �����Ѵ�.

			{ MSK_NONE,		KEY_TAB,	FALSE, FALSE },								// ���� �����̿� �ִ� Ÿ���� �����Ѵ�.

			{ MSK_ALT,		KEY_F12,	FALSE, FALSE },								// ��ũ���� ĸ���Ѵ�.

			{ MSK_NONE,		KEY_ESCAPE,	FALSE, FALSE },								// ���� �޽��� â�� ����.

			{ MSK_CTRL,		KEY_Z,	FALSE, FALSE },									// ��� �������̽��� ����Ѵ�.

			{ MSK_ALT,		KEY_S,	FALSE, FALSE },									// ��ųâ�� ����Ѵ�.

			{ MSK_ALT,		KEY_F,	FALSE, FALSE },									// �йи� â�� ����Ѵ�.

			{ MSK_ALT,		KEY_I,	FALSE, FALSE },									// �κ��丮�� ����Ѵ�.

			{ MSK_ALT,		KEY_H,	FALSE, FALSE },									// ������ ����Ѵ�.

			{ MSK_ALT,		KEY_M,	FALSE, FALSE },									// �̴ϸ��� ����Ѵ�.

			{ MSK_ALT,		KEY_W,	FALSE, FALSE },									// ����� ����Ѵ�.

			{ MSK_ALT,		KEY_O,	FALSE, FALSE },									// �ɼ� â�� ����Ѵ�.

			{ MSK_ALT,		KEY_G,	FALSE, FALSE },									// ��� â�� ����Ѵ�.

			{ MSK_ALT,		KEY_Q,	FALSE, FALSE },									// ����Ʈ â�� ����Ѵ�.

			{ MSK_ALT,		KEY_C,	FALSE, FALSE },									// ĳ���� ����â�� ����Ѵ�.

			{ MSK_ALT,		KEY_R,	FALSE, FALSE },									// �ȱ�/�ٱ� ��带 ��ȯ�Ѵ�.

			{ MSK_ALT,		KEY_INSERT,	FALSE, FALSE },								// ī�޶� �� ��带 ��ȯ�Ѵ�.

			{ MSK_ALT,		KEY_D,	FALSE, FALSE },									// ��Ī â�� ����Ѵ�.
			{ MSK_ALT,		KEY_B,	FALSE, FALSE },									// ģ�� â�� ����Ѵ�.

			// 071025 LYW --- MacroManager : Add toggle key for help dialog.
			{ MSK_ALT,		KEY_T,	FALSE, FALSE },									// ���� â�� ����Ѵ�.
			{ MSK_CTRL,		KEY_CONTROL,	FALSE, FALSE },							// ���� �����ư
			{ MSK_NONE,		KEY_SPACE,		FALSE, FALSE },							// �����ʸ�Ʈ ���â ���

			// 080429 LYW --- MacroManager : Add toggle key for chatroom system.
			{ MSK_NONE,		KEY_HOME, FALSE, FALSE },								// ä�ù� ����Ʈâ�� ����Ѵ�.
			{ MSK_CTRL,		KEY_HOME, FALSE, FALSE },								// ä�ù��� ����Ѵ�.
			{ MSK_CTRL,		KEY_END,  TRUE, FALSE },								// ä�ù�� ä��â ������ ��Ŀ���� �̵���Ų��.

			{ MSK_ALT,		KEY_P,	FALSE, FALSE },									// ������â
			{ MSK_ALT,		KEY_U, FALSE, FALSE },									// 090422 ShinJS --- Ż�� ž�� ����Ű
			{ MSK_NONE,		KEY_Z, FALSE, FALSE },									// 100105 ONS �ɱ�/�Ͼ�⸦ ����Ѵ�.
		},
		{			
			{ MSK_NONE,		KEY_1,	FALSE, FALSE },									// ���� ������ 1��Ű ���
			{ MSK_NONE,		KEY_2,	FALSE, FALSE },									// ���� ������ 2��Ű ���
			{ MSK_NONE,		KEY_3,	FALSE, FALSE },									// ���� ������ 3��Ű ���
			{ MSK_NONE,		KEY_4,	FALSE, FALSE },									// ���� ������ 4��Ű ���
			{ MSK_NONE,		KEY_5,	FALSE, FALSE },									// ���� ������ 5��Ű ���
			{ MSK_NONE,		KEY_6,	FALSE, FALSE },									// ���� ������ 6��Ű ���
			{ MSK_NONE,		KEY_7,	FALSE, FALSE },									// ���� ������ 7��Ű ���
			{ MSK_NONE,		KEY_8,	FALSE, FALSE },									// ���� ������ 8��Ű ���
			{ MSK_NONE,		KEY_9,	FALSE, FALSE },									// ���� ������ 9��Ű ���
			{ MSK_NONE,		KEY_0, FALSE, FALSE },									// ���� ������10��Ű ���

			{ MSK_CTRL,		KEY_1,	FALSE, FALSE },									// ���� ������ 1��Ű ��� 
			{ MSK_CTRL,		KEY_2,	FALSE, FALSE },									// ���� ������ 2��Ű ��� 
			{ MSK_CTRL,		KEY_3,	FALSE, FALSE },									// ���� ������ 3��Ű ��� 
			{ MSK_CTRL,		KEY_4,	FALSE, FALSE },									// ���� ������ 4��Ű ��� 
			{ MSK_CTRL,		KEY_5,	FALSE, FALSE },									// ���� ������ 5��Ű ��� 
			{ MSK_CTRL,		KEY_6,	FALSE, FALSE },									// ���� ������ 6��Ű ��� 
			{ MSK_CTRL,		KEY_7,	FALSE, FALSE },									// ���� ������ 7��Ű ��� 
			{ MSK_CTRL,		KEY_8,	FALSE, FALSE },									// ���� ������ 8��Ű ��� 
			{ MSK_CTRL,		KEY_9,	FALSE, FALSE },									// ���� ������ 9��Ű ��� 
			{ MSK_CTRL,		KEY_0, FALSE, FALSE },									// ���� ������10��Ű ��� 

			{ MSK_ALT,		KEY_1,	FALSE, FALSE },									// ���� �������� 1������ ������.
			{ MSK_ALT,		KEY_2,	FALSE, FALSE },									// ���� �������� 2������ ������.
			{ MSK_ALT,		KEY_3,	FALSE, FALSE },									// ���� �������� 3������ ������.
			{ MSK_ALT,		KEY_4,	FALSE, FALSE },									// ���� �������� 4������ ������.
			{ MSK_ALT,		KEY_5,	FALSE, FALSE },									// ���� �������� 5������ ������.
			{ MSK_ALT,		KEY_6,	FALSE, FALSE },									// ���� �������� 6������ ������.
			{ MSK_ALT,		KEY_7,	FALSE, FALSE },									// ���� �������� 7������ ������.
			{ MSK_ALT,		KEY_8,	FALSE, FALSE },									// ���� �������� 8������ ������.

			{ MSK_NONE,		KEY_OEM_3, FALSE, FALSE },								// ���� ������ ��ȣ�� ���� ��Ų��.
			{ MSK_CTRL,		KEY_OEM_3, FALSE, FALSE },								// ���� ������ ��ȣ�� ���� ��Ų��.

			{ MSK_NONE | MSK_SHIFT,	KEY_OEM_3, FALSE, FALSE },						// ���� ������ ��ȣ�� ���� ��Ų��.
			{ MSK_CTRL | MSK_SHIFT,	KEY_OEM_3, FALSE, FALSE },						// ���� ������ ��ȣ�� ���� ��Ų��.

			{ MSK_NONE,		KEY_F1,	FALSE, FALSE },									// �� �ڽ��� �����Ѵ�.

			{ MSK_NONE,		KEY_F2,	FALSE, FALSE },									// ù ��° ��Ƽ ����� �����Ѵ�.
			{ MSK_NONE,		KEY_F3,	FALSE, FALSE },									// �� ��° ��Ƽ ����� �����Ѵ�.
			{ MSK_NONE,		KEY_F4,	FALSE, FALSE },									// �� ��° ��Ƽ ����� �����Ѵ�.
			{ MSK_NONE,		KEY_F5,	FALSE, FALSE },									// �� ��° ��Ƽ ����� �����Ѵ�.
			{ MSK_NONE,		KEY_F6,	FALSE, FALSE },									// �ټ� ��° ��Ƽ ����� �����Ѵ�.
			{ MSK_NONE,		KEY_F7,	FALSE, FALSE },									// ���� ��° ��Ƽ ����� �����Ѵ�.

			{ MSK_NONE,		KEY_TAB,	FALSE, FALSE },								// ���� �����̿� �ִ� Ÿ���� �����Ѵ�.

			{ MSK_NONE,		KEY_F12,	FALSE, FALSE },								// ��ũ���� ĸ���Ѵ�.

			{ MSK_NONE,		KEY_ESCAPE,	FALSE, FALSE },								// ���� �޽��� â�� ����.

			{ MSK_CTRL,		KEY_Z,	FALSE, FALSE },									// ��� �������̽��� ����Ѵ�.

			{ MSK_NONE,		KEY_S,	FALSE, FALSE },									// ��ųâ�� ����Ѵ�.

			{ MSK_NONE,		KEY_F,	FALSE, FALSE },									// �йи� â�� ����Ѵ�.

			{ MSK_NONE,		KEY_I,	FALSE, FALSE },									// �κ��丮�� ����Ѵ�.

			{ MSK_NONE,		KEY_H,	FALSE, FALSE },									// ������ ����Ѵ�.

			{ MSK_NONE,		KEY_M,	FALSE, FALSE },									// �̴ϸ��� ����Ѵ�.

			{ MSK_NONE,		KEY_W,	FALSE, FALSE },									// ����� ����Ѵ�.

			{ MSK_NONE,		KEY_O,	FALSE, FALSE },									// �ɼ� â�� ����Ѵ�.

			{ MSK_NONE,		KEY_G,	FALSE, FALSE },									// ��� â�� ����Ѵ�.

			{ MSK_NONE,		KEY_Q,	FALSE, FALSE },									// ����Ʈ â�� ����Ѵ�.

			{ MSK_NONE,		KEY_C,	FALSE, FALSE },									// ĳ���� ����â�� ����Ѵ�.

			// 100216 ONS �ȱ�/�ٱ� Alt+R�� ����
			{ MSK_ALT,		KEY_R,	FALSE, FALSE },									// �ȱ�/�ٱ� ��带 ��ȯ�Ѵ�.

			{ MSK_NONE,		KEY_INSERT,	FALSE, FALSE },								// ī�޶� �� ��带 ��ȯ�Ѵ�.

			{ MSK_NONE,		KEY_D,	FALSE, FALSE },									// ��Ī â�� ����Ѵ�.													
			{ MSK_NONE,		KEY_B,	FALSE, FALSE },									// ģ�� â�� ����Ѵ�.

			// 071025 LYW --- MacroManager : Add toggle key for help dialog.
			{ MSK_NONE,		KEY_T,	FALSE, FALSE },									// ���� â�� ����Ѵ�.

			{ MSK_CTRL,		KEY_CONTROL,	FALSE, FALSE },							// ���� �����ư
			{ MSK_NONE,		KEY_SPACE,		FALSE, FALSE },							// �����ʸ�Ʈ ���â ���
			

			// 080429 LYW --- MacroManager : Add toggle key for chatroom system.
			{ MSK_NONE,		KEY_HOME, FALSE, FALSE },								// ä�ù� ����Ʈâ�� ����Ѵ�.
			{ MSK_CTRL,		KEY_HOME, FALSE, FALSE },								// ä�ù��� ����Ѵ�.
			{ MSK_CTRL,		KEY_END,  TRUE, FALSE },								// ä�ù�� ä��â ������ ��Ŀ���� �̵���Ų��.
			
			{ MSK_NONE,		KEY_P,	FALSE, FALSE },									// ������â
			{ MSK_NONE,		KEY_U, FALSE, FALSE },									// 090422 ShinJS --- Ż�� ž�� ����Ű
			{ MSK_NONE,		KEY_Z, FALSE, FALSE },									// 100105 ONS �ɱ�/�Ͼ�⸦ ����Ѵ�.
		},
		{
			{ MSK_NONE, KEY_1, FALSE, FALSE },									// ���� ������ 1��Ű ���
			{ MSK_NONE, KEY_2, FALSE, FALSE },									// ���� ������ 2��Ű ���
			{ MSK_NONE, KEY_3, FALSE, FALSE },									// ���� ������ 3��Ű ���
			{ MSK_NONE, KEY_4, FALSE, FALSE },									// ���� ������ 4��Ű ���
			{ MSK_NONE, KEY_5, FALSE, FALSE },									// ���� ������ 5��Ű ���
			{ MSK_NONE, KEY_6, FALSE, FALSE },									// ���� ������ 6��Ű ���
			{ MSK_NONE, KEY_7, FALSE, FALSE },									// ���� ������ 7��Ű ���
			{ MSK_NONE, KEY_8, FALSE, FALSE },									// ���� ������ 8��Ű ���
			{ MSK_NONE, KEY_9, FALSE, FALSE },									// ���� ������ 9��Ű ���
			{ MSK_NONE, KEY_0, FALSE, FALSE },									// ���� ������10��Ű ���

			{ MSK_CTRL, KEY_1, FALSE, FALSE },									// ���� ������ 1��Ű ��� 
			{ MSK_CTRL, KEY_2, FALSE, FALSE },									// ���� ������ 2��Ű ��� 
			{ MSK_CTRL, KEY_3, FALSE, FALSE },									// ���� ������ 3��Ű ��� 
			{ MSK_CTRL, KEY_4, FALSE, FALSE },									// ���� ������ 4��Ű ��� 
			{ MSK_CTRL, KEY_5, FALSE, FALSE },									// ���� ������ 5��Ű ��� 
			{ MSK_CTRL, KEY_6, FALSE, FALSE },									// ���� ������ 6��Ű ��� 
			{ MSK_CTRL, KEY_7, FALSE, FALSE },									// ���� ������ 7��Ű ��� 
			{ MSK_CTRL, KEY_8, FALSE, FALSE },									// ���� ������ 8��Ű ��� 
			{ MSK_CTRL, KEY_9, FALSE, FALSE },									// ���� ������ 9��Ű ��� 
			{ MSK_CTRL, KEY_0, FALSE, FALSE },									// ���� ������10��Ű ��� 

			{ MSK_ALT, KEY_1, FALSE, FALSE },									// ���� �������� 1������ ������.
			{ MSK_ALT, KEY_2, FALSE, FALSE },									// ���� �������� 2������ ������.
			{ MSK_ALT, KEY_3, FALSE, FALSE },									// ���� �������� 3������ ������.
			{ MSK_ALT, KEY_4, FALSE, FALSE },									// ���� �������� 4������ ������.
			{ MSK_ALT, KEY_5, FALSE, FALSE },									// ���� �������� 5������ ������.
			{ MSK_ALT, KEY_6, FALSE, FALSE },									// ���� �������� 6������ ������.
			{ MSK_ALT, KEY_7, FALSE, FALSE },									// ���� �������� 7������ ������.
			{ MSK_ALT, KEY_8, FALSE, FALSE },									// ���� �������� 8������ ������.

			{ MSK_NONE, KEY_OEM_3, FALSE, FALSE },								// ���� ������ ��ȣ�� ���� ��Ų��.
			{ MSK_CTRL, KEY_OEM_3, FALSE, FALSE },								// ���� ������ ��ȣ�� ���� ��Ų��.

			{ MSK_NONE | MSK_SHIFT, KEY_OEM_3, FALSE, FALSE },						// ���� ������ ��ȣ�� ���� ��Ų��.
			{ MSK_CTRL | MSK_SHIFT, KEY_OEM_3, FALSE, FALSE },						// ���� ������ ��ȣ�� ���� ��Ų��.

			{ MSK_NONE, KEY_F1, FALSE, FALSE },									// �� �ڽ��� �����Ѵ�.

			{ MSK_NONE, KEY_F2, FALSE, FALSE },									// ù ��° ��Ƽ ����� �����Ѵ�.
			{ MSK_NONE, KEY_F3, FALSE, FALSE },									// �� ��° ��Ƽ ����� �����Ѵ�.
			{ MSK_NONE, KEY_F4, FALSE, FALSE },									// �� ��° ��Ƽ ����� �����Ѵ�.
			{ MSK_NONE, KEY_F5, FALSE, FALSE },									// �� ��° ��Ƽ ����� �����Ѵ�.
			{ MSK_NONE, KEY_F6, FALSE, FALSE },									// �ټ� ��° ��Ƽ ����� �����Ѵ�.
			{ MSK_NONE, KEY_F7, FALSE, FALSE },									// ���� ��° ��Ƽ ����� �����Ѵ�.

			{ MSK_NONE, KEY_TAB, FALSE, FALSE },								// ���� �����̿� �ִ� Ÿ���� �����Ѵ�.

			{ MSK_NONE, KEY_F12, FALSE, FALSE },								// ��ũ���� ĸ���Ѵ�.

			{ MSK_NONE, KEY_ESCAPE, FALSE, FALSE },								// ���� �޽��� â�� ����.

			{ MSK_CTRL, KEY_Z, FALSE, FALSE },									// ��� �������̽��� ����Ѵ�.

			{ MSK_NONE, KEY_K, FALSE, FALSE },									// ��ųâ�� ����Ѵ�.

			{ MSK_NONE, KEY_F, FALSE, FALSE },									// �йи� â�� ����Ѵ�.

			{ MSK_NONE, KEY_I, FALSE, FALSE },									// �κ��丮�� ����Ѵ�.

			{ MSK_NONE, KEY_H, FALSE, FALSE },									// ������ ����Ѵ�.

			{ MSK_NONE, KEY_M, FALSE, FALSE },									// �̴ϸ��� ����Ѵ�.

			{ MSK_NONE, KEY_N, FALSE, FALSE },									// ����� ����Ѵ�.

			{ MSK_NONE, KEY_O, FALSE, FALSE },									// �ɼ� â�� ����Ѵ�.

			{ MSK_NONE, KEY_G, FALSE, FALSE },									// ��� â�� ����Ѵ�.

			{ MSK_NONE, KEY_Q, FALSE, FALSE },									// ����Ʈ â�� ����Ѵ�.

			{ MSK_NONE, KEY_C, FALSE, FALSE },									// ĳ���� ����â�� ����Ѵ�.

			// 100216 ONS �ȱ�/�ٱ� Alt+R�� ����
			{ MSK_ALT, KEY_R, FALSE, FALSE },									// �ȱ�/�ٱ� ��带 ��ȯ�Ѵ�.

			{ MSK_NONE, KEY_INSERT, FALSE, FALSE },								// ī�޶� �� ��带 ��ȯ�Ѵ�.

			{ MSK_NONE, KEY_H, FALSE, FALSE },									// ��Ī â�� ����Ѵ�.													
			{ MSK_NONE, KEY_B, FALSE, FALSE },									// ģ�� â�� ����Ѵ�.
			// 071025 LYW --- MacroManager : Add toggle key for help dialog.
			{ MSK_NONE, KEY_T, FALSE, FALSE },									// ���� â�� ����Ѵ�.

			{ MSK_CTRL, KEY_CONTROL, FALSE, FALSE },							// ���� �����ư
			{ MSK_NONE, KEY_SPACE, FALSE, FALSE },							// �����ʸ�Ʈ ���â ���


			// 080429 LYW --- MacroManager : Add toggle key for chatroom system.
			{ MSK_NONE, NULL, FALSE, FALSE },								// ä�ù� ����Ʈâ�� ����Ѵ�.
			{ MSK_CTRL, NULL, FALSE, FALSE },								// ä�ù��� ����Ѵ�.
			{ MSK_CTRL, KEY_END, TRUE, FALSE },								// ä�ù�� ä��â ������ ��Ŀ���� �̵���Ų��.

			{ MSK_NONE, KEY_P, FALSE, FALSE },									// ������â
			{ MSK_NONE, KEY_U, FALSE, FALSE },									// 090422 ShinJS --- Ż�� ž�� ����Ű
			{ MSK_NONE, KEY_Z, FALSE, FALSE },									// 100105 ONS �ɱ�/�Ͼ�⸦ ����Ѵ�.
		}
	};

	sMACRO DefaultPressMacro[MM_COUNT][MPE_COUNT] = {
		{
			{ MSK_NONE,		KEY_PGUP,	FALSE, FALSE },								//	MPE_CAMERAMOVE_ZOOMIN
			{ MSK_NONE,		KEY_PGDN,	FALSE, FALSE },								//	MPE_CAMERAMOVE_ZOOMOUT

			{ MSK_NONE,		KEY_UP,		FALSE, FALSE },								//	MPE_CAMERAMOVE_UP
			{ MSK_NONE,		KEY_DOWN,	FALSE, FALSE },								//	MPE_CAMERAMOVE_DOWN
			{ MSK_NONE,		KEY_RIGHT,	FALSE, FALSE },								//	MPE_CAMERAMOVE_RIGHT
			{ MSK_NONE,		KEY_LEFT,	FALSE, FALSE },								//	MPE_CAMERAMOVE_LEFT

			{ MSK_NONE,		KEY_W,		FALSE, FALSE },								//	MPE_MOVE_UP
			{ MSK_NONE,		KEY_S,		FALSE, FALSE },								//	MPE_MOVE_DOWN
			{ MSK_NONE,		KEY_D,		FALSE, FALSE },								//	MPE_MOVE_RIGHT
			{ MSK_NONE,		KEY_A,		FALSE, FALSE },								//	MPE_MOVE_LEFT	

			{ MSK_NONE,		KEY_UP,		FALSE, FALSE },								//	MPE_MOVE_UP
			{ MSK_NONE,		KEY_DOWN,	FALSE, FALSE },								//	MPE_MOVE_DOWN
			{ MSK_NONE,		KEY_RIGHT,	FALSE, FALSE },								//	MPE_MOVE_RIGHT
			{ MSK_NONE,		KEY_LEFT,	FALSE, FALSE },								//	MPE_MOVE_LEFT
		},
		{
			{ MSK_NONE,		KEY_PGUP,	FALSE, FALSE },								//	MPE_CAMERAMOVE_ZOOMIN
			{ MSK_NONE,		KEY_PGDN,	FALSE, FALSE },								//	MPE_CAMERAMOVE_ZOOMOUT

			{ MSK_NONE,		KEY_UP,		FALSE, FALSE },								//	MPE_CAMERAMOVE_UP
			{ MSK_NONE,		KEY_DOWN,	FALSE, FALSE },								//	MPE_CAMERAMOVE_DOWN
			{ MSK_NONE,		KEY_RIGHT,	FALSE, FALSE },								//	MPE_CAMERAMOVE_RIGHT
			{ MSK_NONE,		KEY_LEFT,	FALSE, FALSE },								//	MPE_CAMERAMOVE_LEFT		

			{ MSK_NONE,		KEY_W,		FALSE, FALSE },								//	MPE_MOVE_UP
			{ MSK_NONE,		KEY_S,		FALSE, FALSE },								//	MPE_MOVE_DOWN
			{ MSK_NONE,		KEY_D,		FALSE, FALSE },								//	MPE_MOVE_RIGHT
			{ MSK_NONE,		KEY_A,		FALSE, FALSE },								//	MPE_MOVE_LEFT	

			{ MSK_NONE,		KEY_UP,		FALSE, FALSE },								//	MPE_MOVE_UP
			{ MSK_NONE,		KEY_DOWN,	FALSE, FALSE },								//	MPE_MOVE_DOWN
			{ MSK_NONE,		KEY_RIGHT,	FALSE, FALSE },								//	MPE_MOVE_RIGHT
			{ MSK_NONE,		KEY_LEFT,	FALSE, FALSE },								//	MPE_MOVE_LEFT	
		},
		{
			{ MSK_NONE, KEY_PGUP, FALSE, FALSE },								//	MPE_CAMERAMOVE_ZOOMIN
			{ MSK_NONE, KEY_PGDN, FALSE, FALSE },								//	MPE_CAMERAMOVE_ZOOMOUT

			{ MSK_NONE, KEY_UP, FALSE, FALSE },								//	MPE_CAMERAMOVE_UP//Beyond SEA2
			{ MSK_NONE, KEY_DOWN, FALSE, FALSE },								//	MPE_CAMERAMOVE_DOWN
			{ MSK_NONE, KEY_RIGHT, FALSE, FALSE },								//	MPE_CAMERAMOVE_RIGHT
			{ MSK_NONE, KEY_LEFT, FALSE, FALSE },								//	MPE_CAMERAMOVE_LEFT

			{ MSK_NONE, KEY_W, FALSE, FALSE },								//	MPE_MOVE_UP
			{ MSK_NONE, KEY_S, FALSE, FALSE },								//	MPE_MOVE_DOWN
			{ MSK_NONE, KEY_D, FALSE, FALSE },								//	MPE_MOVE_RIGHT
			{ MSK_NONE, KEY_A, FALSE, FALSE },								//	MPE_MOVE_LEFT

			{ MSK_NONE, KEY_UP, FALSE, FALSE },								//	MPE_MOVE_UP
			{ MSK_NONE, KEY_DOWN, FALSE, FALSE },								//	MPE_MOVE_DOWN
			{ MSK_NONE, KEY_RIGHT, FALSE, FALSE },								//	MPE_MOVE_RIGHT
			{ MSK_NONE, KEY_LEFT, FALSE, FALSE },								//	MPE_MOVE_LEFT
		}
	};

	memcpy( m_DefaultKey, DefaultMacro, sizeof( DefaultMacro ) );
	memcpy( m_DefaultPressKey, DefaultPressMacro, sizeof( DefaultPressMacro ) );

	LoadUserMacro();
}


void CMacroManager::SetDefaultMacro( int nMacroMode )
{
	memcpy( m_MacroKey[nMacroMode], m_DefaultKey[nMacroMode], sizeof( sMACRO ) * ME_COUNT );

	memcpy( m_MacroPressKey[nMacroMode], m_DefaultPressKey[nMacroMode], sizeof( sMACRO ) * MPE_COUNT );
}


int CMacroManager::IsUsableKey( CKeyboard* keyInfo, WORD* wKey, BOOL bUserMode )
{
	static WORD wUsableSysKey[] =
	{
		KEY_CONTROL,
		KEY_MENU,
		KEY_SHIFT
	};

	static WORD wUsableCombineKey[] =
	{
		KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0,
		KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J, KEY_K, KEY_L, KEY_M,
		KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,
		KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6,
		KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12,

		KEY_EQUALS, KEY_SEMICOLON, KEY_OEM_3, KEY_SLASH, KEY_LBRACKET, KEY_RBRACKET, KEY_BACKSLASH,
		KEY_APOSTROPHE, KEY_MINUS, KEY_EQUALS, /*KEY_OEM_102,*/ KEY_COMMA, KEY_PERIOD, 

		KEY_SPACE, KEY_BACK, KEY_INSERT, KEY_DELETE, KEY_HOME, KEY_END, KEY_PGUP, KEY_PGDN,
		KEY_TAB, KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT,

		KEY_PAD0, KEY_PAD1, KEY_PAD2, KEY_PAD3, KEY_PAD4, KEY_PAD5, KEY_PAD6, KEY_PAD7, KEY_PAD8, KEY_PAD9,
		KEY_MULTIPLY, KEY_ADD, KEY_SUBTRACT, KEY_DECIMAL, KEY_DIVIDE
	};
	
	static WORD wUsableKey[] =
	{
		KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6,
		KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12,

		KEY_SPACE, KEY_BACK, KEY_INSERT, KEY_DELETE, KEY_HOME, KEY_END, KEY_PGUP, KEY_PGDN,
		KEY_TAB, KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT,
	};

	static BOOL bSysKeyDown = FALSE;
	int i;

	if( ( keyInfo->GetKeyDown( KEY_TAB ) || keyInfo->GetKeyDown( KEY_F4 ) )
		  && keyInfo->GetKeyPressed( KEY_MENU ) )
	{
		return 0;
	}


	if( bSysKeyDown || bUserMode == 1 )
	{
		for( i = 0 ; i < sizeof( wUsableCombineKey ) / sizeof( WORD ) ; ++i )
		{
			if( keyInfo->GetKeyDown( wUsableCombineKey[i] ) )
			{
				*wKey = wUsableCombineKey[i];
				return 1;
			}
		}
	}
	else
	{
		for( i = 0 ; i < sizeof( wUsableKey ) / sizeof( WORD ) ; ++i )
		{
			if( keyInfo->GetKeyDown( wUsableKey[i] ) )
			{
				*wKey = wUsableKey[i];
				return 1;
			}
		}
	}

	for( i = 0 ; i < sizeof( wUsableSysKey ) / sizeof( WORD ) ; ++i )
	{
		if( keyInfo->GetKeyPressed( wUsableSysKey[i] ) )
		{
			*wKey = wUsableSysKey[i];
			bSysKeyDown = TRUE;
			return 2;
		}
	}

	*wKey = 0x0000;
	bSysKeyDown = FALSE;
	return 0;
}

void CMacroManager::CheckToggleState( CObject* player )
{
	if( !	player			||
			player != HERO	||
			WINDOWMGR->IsOpendAllWindows() )
	{
		return;
	}

	// 071204 LUJ	��ũ���� �Կ����� �������̽��� ��� ���ε� �̵��� �õ��� ���
	//				���� ���¿����� �̿� ������ �������̽��� �ݾƾ� �Ѵ�

	// �ŷ�â
	struct
	{
		void operator() ( cDialog* dialog )
		{
			cPtrList& list = WINDOWMGR->m_pActivedWindowList;

			if( ! dialog)
			{
				return;
			}

			void* key = ( void* )( dialog->GetID() );

			PTRLISTPOS	position = list.Find( key );

			list.RemoveAt( position );
		}
	}
	RemoveSavedWindow;

	RemoveSavedWindow( ( cDialog* )GAMEIN->GetStorageDialog() );
	RemoveSavedWindow( ( cDialog* )GAMEIN->GetGuildWarehouseDlg() );
	RemoveSavedWindow( ( cDialog* )GAMEIN->GetExchangeDialog() );
	RemoveSavedWindow( ( cDialog* )GAMEIN->GetNpcScriptDialog() );
	RemoveSavedWindow( ( cDialog* )GAMEIN->GetStreetBuyStallDialog() );
	RemoveSavedWindow( ( cDialog* )GAMEIN->GetStreetStallDialog() );
	RemoveSavedWindow( ( cDialog* )GAMEIN->GetDealDialog() );
}
