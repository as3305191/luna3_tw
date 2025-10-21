// OptionManager.cpp: implementation of the COptionManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OptionManager.h"
#include "MacroManager.h"
#include "MouseCursor.h"
#include "MousePointer.h"
#include "./Input/UserInput.h"
#include "ChatManager.h"
#include ".\Effect\EffectManager.h"
#include "cChatTipManager.h"
#include "ObjectManager.h"
#include "MHAudioManager.h"
#include "MHCamera.h"
#include "GameIn.h"
#include "./interface/cWindowManager.h"
#include "OptionDialog.h"
#include "./interface/cGuageBar.h"
#include "WindowIDEnum.h"
#include "MHMap.h"
#include "ChattingDlg.h"
#include "TutorialManager.h"
#include "AppearanceManager.h"

GLOBALTON(COptionManager);

COptionManager::COptionManager()
{
	ZeroMemory(
		&m_GameOption,
		sizeof(m_GameOption));
	ZeroMemory(
		&m_SoundInfoBackup,
		sizeof(m_SoundInfoBackup));
	SetDefaultOption();
}

COptionManager::~COptionManager()
{
}

void COptionManager::Init()
{
	LoadGameOption();

	m_bBGMSoundChanged	= FALSE;																			// ��������� �ٲ������ ���θ� FALSE�� �����Ѵ�.
	
	AUDIOMGR->SetBGMSoundOn( m_GameOption.bSoundBGM );														// �ɼ� ���� ���� ��� ������ ON/OFF �� �����Ѵ�.
	
	// 100112 ONS ����ɼ������� ����Ѵ�. ����� �������ٸ� ���� ���������� ���� �����Ͽ�
	// �����Ű�� ������ �ּҹ�ư�� ó���� ����Ͽ� ����ص� �ʿ䰡 �ִ�.
	SoundInfoBackup();

	m_wSendOption		= 0;																				// ��� �ɼ� �÷��� ���� 0���� �����Ѵ�.
	
	m_bShowMemberDamage = TRUE;																				// ��Ƽ�� ������ ���⸦ TRUE�� �����Ѵ�.

	// 071026 LYW --- OptionManager : Setting option of tutorial.
	if( m_GameOption.bShowTutorial )
	{
		TUTORIALMGR->Set_ActiveTutorial(TRUE) ;							// Ʃ�丮���� ��Ȱ��ȭ ���·� �����Ѵ�.
	}
	else
	{
		TUTORIALMGR->Set_ActiveTutorial(FALSE) ;							// Ʃ�丮���� ��Ȱ��ȭ ���·� �����Ѵ�.
	}

	// 080916 KTH -- OptionManager : Setting Option Of ShoutDialog
	if( CHATMGR->GetChattingDlg() )
	{
		CHATMGR->GetChattingDlg()->SetRaceActive(e_CHATTINGDLG_GENERAL_SHOUT, !m_GameOption.bShowGeneralShoutDlg);
	}
}

void COptionManager::SendOptionMsg()
{
	MSG_OPT message;
	ZeroMemory(
		&message,
		sizeof(message));
	message.Category = MP_USERCONN;
	message.Protocol = MP_USERCONN_OPTION_SYN;
	message.dwObjectID = gHeroID;
	message.dwData1 = MAKELPARAM(
		m_GameOption.bNoDeal,
		m_GameOption.bNoParty);
	message.dwData2 = MAKELPARAM(
		m_GameOption.bNoShowdown,
		m_GameOption.mVehicle.mIsNeedAllow);
	message.dweData3 = m_GameOption.mVehicle.mChargedMoney;
	// �� ������ ������Ʈ �������� ȹ���Ͽ� ����ϹǷ� ������ �� ��������.
	message.dwData4 = MAKELPARAM(
		m_GameOption.bNoFriend,
		m_GameOption.bNoWhisper);
	message.dwData5 = m_GameOption.bNoDateMatching;
	message.dwData6 = m_GameOption.dwHideParts;
	message.dwData7 = m_GameOption.bWingsMoveMode;
	NETWORK->Send(
		&message,
		sizeof(message));
}

void COptionManager::SetGameOption( sGAMEOPTION* pOption, BOOL bSaveToFile )
{
	if( m_GameOption.bSoundBGM != pOption->bSoundBGM )														// �Է� ���� ������� ����, ����� ���� ���� ������,
	{
		m_bBGMSoundChanged = TRUE;																			// ��������� �ٲ������ ���θ� TRUE�� �����Ѵ�.
	}

	m_GameOption = *pOption;																				// ����� �ɼ� ����ü�� �Է¹��� ����ü�� �����Ѵ�.

	// 100112 ONS ���� �������� ���������� �����ϸ� ��ȿ�����̹Ƿ� ����.
	if( bSaveToFile )
	{
		SaveGameOption();																						// �ɼ��� �����Ѵ�.
	}
}

BOOL COptionManager::LoadGameOption()
{
	HANDLE	hFile = CreateFile( "./system/GameOption.opt", GENERIC_READ, 0, NULL,
						OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

	if( hFile == INVALID_HANDLE_VALUE )																		// �ڵ� �ޱ⿡ �����ϸ�,
		return FALSE;																						// ���� ó���� �Ѵ�.

	DWORD dwRead = 0;
	DWORD dwVersion = 0;

	if( !ReadFile( hFile, &dwVersion, sizeof(DWORD), &dwRead, NULL ) )										// ���� �ڵ��� ������ �����͸� �д´�. �����ϸ�,
	{
		CloseHandle( hFile );																				// ���� �ڵ��� �ݴ´�.
		return FALSE;																						// ���� ó���� �Ѵ�.
	}
	if( dwVersion != OPTIONVERSION )																		// ������ �־��� ���� ���� ������,
	{
		CloseHandle( hFile );																				// ���� �ڵ��� �ݴ´�.
		return FALSE;																						// ���� ó���� �Ѵ�.
	}

	ReadFile(
		hFile,
		&m_GameOption,
		sizeof(m_GameOption),
		&dwRead,
		0);
	CloseHandle(
		hFile);
	return TRUE;
}

void COptionManager::SaveGameOption()
{
	HANDLE	hFile;																							// ���� �ڵ��� �����Ѵ�.

	hFile=CreateFile( "./system/GameOption.opt", GENERIC_WRITE, 0, NULL,									// ������ ���Ϸ� �ڵ��� �޴´�.(������)
					  CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

	if( hFile == INVALID_HANDLE_VALUE )																		// �ڵ� �ޱ⿡ �����ϸ�,
		return;																								// ���� ó���� �Ѵ�.
	
	DWORD dwWritten;																						// ������ ����� ����Ʈ ���� ���Ϲޱ� ���� ����.
	DWORD dwVersion = OPTIONVERSION ;																		// ������ �����Ѵ�.
	WriteFile( hFile, &dwVersion, sizeof(DWORD), &dwWritten, NULL );										// ������ ����Ѵ�.
	WriteFile( hFile, &m_GameOption, sizeof(m_GameOption), &dwWritten, NULL );								// �ɼǱ���ü ��ŭ ����Ѵ�.
	
	CloseHandle( hFile );																					// �ڵ��� �ݴ´�.
}

void COptionManager::ApplySettings()
{
	ApplySound() ;																							// ���� ��Ʈ�� �����Ѵ�.
	// 100112 ONS �ɼ������ ���������� ����Ѵ�.
	SoundInfoBackup();
	
	ApplyGraphics() ;																						// �׷��� ��Ʈ�� �����Ѵ�.

	ApplyChatting() ;																						// ä�� ��Ʈ�� �����Ѵ�.

	ApplyEnvironmemt() ;																					// ȯ�� ��Ʈ�� �����Ѵ�.
}

void COptionManager::ApplySound()
{
	if( m_bBGMSoundChanged )																				// ��������� ���濩�ΰ� TRUE �̸�,
	{
		AUDIOMGR->SetBGMSoundOn( m_GameOption.bSoundBGM );													// �ɼǿ� ��� ������ ��������� ON/OFF �����Ѵ�.

		if( m_GameOption.bSoundBGM )																		// ������� �ɼǰ��� ON�̸�, 
		{
			AUDIOMGR->PlayLastBGM();																		// ����� �Ŵ����� ������ ��������� �÷��� ��Ų��.
		}
		else																								// ������� ���ǰ��� OFF �̸�, 
		{
			AUDIOMGR->StopBGM();																			// ����� �Ŵ����� ��������� ���� ��Ų��.
		}

		m_bBGMSoundChanged = FALSE;																			// ��������� ���濩�θ� FALSE�� �����Ѵ�.
	}

	if( m_GameOption.bSoundBGM )	//bgm�� play���� �ƴҶ� ���������ϸ� ������.							// ��������� �÷��� ���̸�,
	{
		AUDIOMGR->SetBGMVolume( (float)m_GameOption.nVolumnBGM / 100.0f );									// �ɼ��� BGM Volume�� ������� �ؼ� ������� ������ �����Ѵ�.
	}
	
	AUDIOMGR->SetEffectSoundOn( m_GameOption.bSoundEnvironment );											// �ɼ��� ���� ���� ���� ����Ʈ ���带 ON/OFF �Ѵ�.

	if( m_GameOption.bSoundEnvironment )																	// �ɼ��� ���� ���� TRUE �̸�,
	{
		AUDIOMGR->SetSoundVolume( (float)m_GameOption.nVolumnEnvironment / 100.0f ) ;						// �ɼ��� �������� ������� �ؼ� ���� ������ �����Ѵ�.
	}
	else																									// �ɼ��� ���尪�� FALSE �̸�,
	{
		AUDIOMGR->StopAll();																				// ��� �����Ų��.
	}
}

void COptionManager::ApplyGraphics()
{
	CURSOR->SetActive( !m_GameOption.bGraphicCursor );														// �ɼ��� Ŀ�� ���� ���� Ŀ���� ON/OFF �Ѵ�.

	CAMERA->SetSightDistanceRate( (float)m_GameOption.nSightDistance );										// �þ� �Ÿ����� �����Ѵ�.

	DWORD dwShadowOption = eOO_SHADOW_NONE;																	// �׸��� �ɼ��� ���� ������ ���� �� �ʱ�ȭ �Ѵ�.

	if( m_GameOption.bShadowHero )		dwShadowOption |= eOO_SHADOW_ONLYHERO;								// �ڽ��� üũ�Ǿ� ������ �߰��Ѵ�.
	if( m_GameOption.bShadowMonster )	dwShadowOption |= eOO_SHADOW_MONSTER;								// ���ͳ� NPC�� üũ�Ǿ� ������ �߰��Ѵ�.
	if( m_GameOption.bShadowOthers )	dwShadowOption |= eOO_SHADOW_OTHERCHARACTER;						// �ٸ�ĳ���Ͱ� üũ�Ǿ� ������ �߰��Ѵ�.

	OBJECTMGR->SetShadowOption( dwShadowOption );															// ������Ʈ �Ŵ����� �ɼ��� �����Ѵ�.

	//SW ��ȹ, �׷��� ������ 0������ �ּ� ���ؽ����� ���⼭�� 2 ������ �ּ��̴�.
	// @@@ Iros LOD Option ���� ó��
	OBJECTMGR->SetLodMode(m_GameOption.nLODMode);
	g_pExecutive->SetModelLODUsingMode( m_GameOption.nLODMode, 2 ) ;										// LOD ����带 �����Ѵ�.
	//Effect
	OBJECTMGR->SetEffectOption( m_GameOption.nEffectMode ) ;												// ����Ʈ ��带 �����Ѵ�.
}

void COptionManager::ApplyChatting()
{	
	sChatOption ChatOption;

	ChatOption.bOption[CTO_NOWHISPERMSG]	= m_GameOption.bNoWhisper ;										// �ӼӸ� �źθ� �����Ѵ�.
	ChatOption.bOption[CTO_NOCHATMSG]		= m_GameOption.bNoChatting ;									// �Ϲ�ä�� ���⸦ �����Ѵ�.
	ChatOption.bOption[CTO_NOSHOUTMSG]		= m_GameOption.bNoShoutChat ;									// ��ġ�� ���⸦ �����Ѵ�.
	ChatOption.bOption[CTO_NOPARTYMSG]		= m_GameOption.bNoPartyChat ;									// ��Ƽ ä�� ���⸦ �����Ѵ�.
	ChatOption.bOption[CTO_NOFAMILYMSG]		= m_GameOption.bNoFamilyChat ;									// �йи� ä�� ���⸦ �����Ѵ�.
	ChatOption.bOption[CTO_NOGUILDMSG]		= m_GameOption.bNoGuildChat ;									// ��� ä�� ���⸦ �����Ѵ�.
	ChatOption.bOption[CTO_NOALLIANCEMSG]	= m_GameOption.bNoAllianceChat ;								// ���� ä�� ���⸦ �����Ѵ�.
	ChatOption.bOption[CTO_NOSYSMSG]		= m_GameOption.bNoSystemMsg ;									// �ý��� �޽��� ���⸦ �����Ѵ�.
	ChatOption.bOption[CTO_NOEXPMSG]		= m_GameOption.bNoExpMsg ;										// ����ġ ȹ�� �޽��� ���⸦ �����Ѵ�.
	ChatOption.bOption[CTO_NOITEMMSG]		= m_GameOption.bNoItemMsg ;										// ������ ȹ�� �޽��� ���⸦ �����Ѵ�.

	// 080916 KTH -- OptionManager ShoutListDialog Setting
	if( CHATMGR->GetChattingDlg() )
	{
		CHATMGR->GetChattingDlg()->SetRaceActive(e_CHATTINGDLG_GENERAL_SHOUT, !m_GameOption.bShowGeneralShoutDlg);
	}

	CHATMGR->SetOption(
		ChatOption);
}

void COptionManager::ApplyEnvironmemt()
{
	DWORD dwOverInfoOption = eOO_NAME_CLICKEDONLY ;	//---default											// ���� ���� �ɼ��� ���� ���� ����.

	if( m_GameOption.bNameOthers )	dwOverInfoOption |= eOO_NAME_WHOLE ;									// �ٸ� ĳ���� �̸�ǥ�ø� �߰��Ѵ�.
	if( m_GameOption.bNoMemberDamage )	dwOverInfoOption |= eOO_DAMAGE_MEMBERS ;							// ��Ƽ�� ������ ���⸦ �߰��Ѵ�.
	if( m_GameOption.bNameParty )	dwOverInfoOption |= eOO_NAME_PARTY ;									// ��Ƽ�� �̸� ǥ�ø� �߰��Ѵ�.
	if( m_GameOption.bNameMunpa )	dwOverInfoOption |= eOO_NAME_MUNPA ;									// ���� �̸� ǥ�ø� �߰��Ѵ�.
	if( m_GameOption.bNoBalloon )	dwOverInfoOption |= eOO_CHAT_NOBALLOON ;								// ��ǳ���� TRUE�� �߰��Ѵ�.

	// 090116 ShinJS --- �ɼ� ������׿� ĳ����/��/������ ���̱�/�Ⱥ��̱� ����
	if( m_GameOption.bHideOtherPlayer )			dwOverInfoOption |= eOO_HIDE_PLAYER;
	if( m_GameOption.bHideOtherPet ) 			dwOverInfoOption |= eOO_HIDE_PET;
	if( m_GameOption.bHideStreetStallTitle )	dwOverInfoOption |= eOO_HIDE_STREETSTALLTITLE;

	OBJECTMGR->SetOverInfoOption( dwOverInfoOption ) ;														// ������Ʈ �Ŵ����� ���� ���� �ɼ��� �����Ѵ�.

	CHATTIPMGR->SetActive( m_GameOption.bNoGameTip ) ;														// ���� �� ���⸦ �����Ѵ�.

	OPTIONMGR->SetMemberDemageActive( m_GameOption.bNoMemberDamage ) ;										// ��Ƽ�� ������ ���⸦ �����Ѵ�.

	if (m_GameOption.nWASDMode != 0)
		MACROMGR->SetMacroMode( 2 ) ;
	else
		MACROMGR->SetMacroMode(m_GameOption.nMacroMode);
}

void COptionManager::CancelSettings()
{
	//���� ���� �־��� �͵� ����� ��������
	// 100112 ONS �ɼ� ���� ��ҽÿ� ����(ȿ����, �������)������ �ǵ�����.
	m_GameOption.nVolumnBGM			=	m_SoundInfoBackup.m_nVolumnBgm;
	m_GameOption.nVolumnEnvironment	=	m_SoundInfoBackup.m_nVolumnEnvironment;
	ApplySound();
}

void COptionManager::SetDefaultOption()
{
	m_GameOption.bNoDeal 			= FALSE;																// �ŷ� �ź�.
	m_GameOption.bNoParty 			= FALSE;																// ��Ƽ �ź�.
	m_GameOption.bNoFriend 			= FALSE;																// ģ����� �ź�.
	m_GameOption.bNoShowdown 		= FALSE;																// ���� �ź�.
	m_GameOption.bNoDateMatching	= FALSE;
	m_GameOption.bIsOnBattleGuage	= TRUE;																	// ���������� ��¿���	

	m_GameOption.bNameMunpa 		= TRUE;																	// ���� �̸� ǥ��.
	m_GameOption.bNameParty 		= TRUE;																	// ��Ƽ�� �̸� ǥ��.
	m_GameOption.bNameOthers 		= TRUE;																	// �ٸ� ĳ���� �̸� ǥ��.
	m_GameOption.bNoMemberDamage 	= TRUE;																	// ��Ƽ�� ������ ����.
	m_GameOption.bNoGameTip 		= TRUE;																	// ���� �� ����.

	// 090116 ShinJS --- ĳ����/�� ���̱� �ʱ�ȭ, ���� �Ⱥ��̱� �ʱ�ȭ
	m_GameOption.bHideOtherPlayer 	= FALSE;																// ĳ���� ���̱�
	m_GameOption.bHideOtherPet		= FALSE;																// �� ���̱�
	m_GameOption.bHideStreetStallTitle	= FALSE;															// ����Ÿ��Ʋ �Ⱥ��̱�
	// 100322 ONS ����Ʈ�˸��� �ڵ���� ���� �ʱ�ȭ
	m_GameOption.bAutoQuestNotice		= TRUE;

	m_GameOption.nMacroMode 		= 1 ;																	// ä�ÿ켱���/����Ű�켱���.
	m_GameOption.nWASDMode	 		= 0 ;																	// ä�ÿ켱���/����Ű�켱���.

	m_GameOption.bNoWhisper			= FALSE;																// �ӼӸ� �ź�.
	m_GameOption.bNoChatting		= FALSE;																// �Ϲ�ä�� ����.
	m_GameOption.bNoBalloon			= FALSE;																// ��ǳ�� ����.
	m_GameOption.bNoPartyChat		= FALSE ;																// ��Ƽ ä�� ����.
	m_GameOption.bNoFamilyChat		= FALSE ;																// �йи� ä�� ����.
	m_GameOption.bNoShoutChat		= FALSE;																// ��ġ�� ����.
	m_GameOption.bNoGuildChat		= FALSE;																// ���ä�� ����.
	m_GameOption.bNoAllianceChat	= FALSE;																// ����ä�� ����.

	m_GameOption.bNoSystemMsg		= FALSE;																// �ý��۸޽��� ����.
	m_GameOption.bNoExpMsg			= FALSE;																// ����ġ.����ġ ȹ��޽��� ����.
	m_GameOption.bNoItemMsg			= FALSE;																// ��/������ ȹ��޽��� ����.

	m_GameOption.nSightDistance		= 155;//m_SigntGBMaxValue-50;													// �þ߰Ÿ� ������.
	m_GameOption.bGraphicCursor		= FALSE;																// Ŀ�� �¿���?
	m_GameOption.bShadowHero		= TRUE;																	// �׸��� ǥ�� �ڽ�.
	m_GameOption.bShadowMonster		= TRUE;																	// �׸��� ǥ�� ����/NPC.
	m_GameOption.bShadowOthers		= TRUE;																	// �׸��� ǥ�� �ٸ�ĳ����.

	m_GameOption.bAutoCtrl			= FALSE;																// ���� ȭ�� �ڵ� ����.
	m_GameOption.bAmbientMax		= TRUE;																	// ĳ���� ���?
	m_GameOption.nLODMode			= 0;																	// �׷��� ����.
	m_GameOption.nEffectMode		= 0;																	// ����Ʈ ���.
	m_GameOption.nEffectSnow        = 0;																	// �� ����Ʈ ����.

	// 090429 ShinJS --- ������� ����Ǵ� ���
	if( !m_GameOption.bSoundBGM )
		m_bBGMSoundChanged = TRUE;

	m_GameOption.bSoundBGM = TRUE;
	m_GameOption.bSoundEnvironment = TRUE;
	m_GameOption.nVolumnBGM	= 100;
	m_GameOption.nVolumnEnvironment	= 100;
	m_GameOption.bShowTutorial = TRUE;
	m_GameOption.dwHideParts = 0;
	m_GameOption.bShowGeneralShoutDlg = FALSE ;
	m_GameOption.mVehicle.mIsNeedAllow = TRUE;
	m_GameOption.bWingsMoveMode = TRUE;
}

void COptionManager::ApplyGraphicSetting()
{
	CAMERA->SetSightDistanceRate( (float)m_GameOption.nSightDistance );										// ī�޶��� �þ߰Ÿ��� �����Ѵ�.
	g_pExecutive->SetModelLODUsingMode( m_GameOption.nLODMode, 2 );											// LOD����带 �����Ѵ�.
	OBJECTMGR->SetShadowOption( m_dwShdowOption );															// �׸��� �ɼ��� �����Ѵ�.
	OBJECTMGR->SetEffectOption( m_GameOption.nEffectMode );													// ����Ʈ �ɼ��� �����Ѵ�.
}

void COptionManager::InitForGameIn()
{
	m_pOptionDlg = (COptionDialog*)WINDOWMGR->GetWindowForID( OTI_TABDLG );									// �ɼ� ���̾�α� ������ �޴´�.
	cDialog* pDlg = (cDialog*)m_pOptionDlg->GetTabSheet(2);													// ����° ��Ʈ�� �޴´�.
	m_pCammaGB = (cGuageBar*)(pDlg->GetWindowForID( OTI_GB_GAMMA ));										// ��� ���� �������� ������ �޴´�.
	m_pSightGB = (cGuageBar*)(pDlg->GetWindowForID( OTI_GB_SIGHT ));										// �þ߰Ÿ� ���� �������� ������ �޴´�.
	m_SigntGBMinValue = m_pSightGB->GetMinValue();															// �þ߰Ÿ� �ּҰ��� �޴´�.
	m_SigntGBMaxValue = m_pSightGB->GetMaxValue();															// �þ߰Ÿ� �ִ밪�� �޴´�.
	m_dwShdowOption = eOO_SHADOW_NONE;																		// �׸��� �ɼ��� �ƹ��͵� ���°����� �����Ѵ�.
	m_dwLastCheckTime = m_dwCheck = m_dwFrameAverage = m_dwCountFrame = 0;									// �ð����� �������� 0���� �����Ѵ�.
	m_nCurOptionState = HIGH;																				// ���� �ɼ� ���¸� HIGH�� �����Ѵ�.
}

void COptionManager::SoundInfoBackup()
{
	m_SoundInfoBackup.m_nVolumnBgm			= m_GameOption.nVolumnBGM;
	m_SoundInfoBackup.m_nVolumnEnvironment	= m_GameOption.nVolumnEnvironment;
}