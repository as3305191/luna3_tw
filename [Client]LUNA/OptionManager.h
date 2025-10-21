#pragma once

#define OPTIONMGR USINGTON(COptionManager)
#define OPTIONVERSION	0x00070716

class COptionDialog;
class cGuageBar;

struct sGAMEOPTION
{
	BOOL bNoDeal;															// �ŷ� �ź�.
	BOOL bNoParty;															// ��Ƽ �ź�.
	BOOL bNoFriend;															// ģ����� �ź�.
	BOOL bNoShowdown;														// ���� �ź�.

	BOOL bNameMunpa;														// ���� �̸� ǥ��.
	BOOL bNameParty;														// ��Ƽ�� �̸� ǥ��.
	BOOL bNameOthers;														// �ٸ� ĳ���� �̸� ǥ��.
	BOOL bNoMemberDamage;													// ��Ƽ�� ������ ����.
	BOOL bNoGameTip;														// ���� �� ����.

	// 090116 ShinJS --- ĳ����/�� ���̱� �ɼ� �߰�
	BOOL bHideOtherPlayer;													// ĳ���� ���̱�
	BOOL bHideOtherPet;														// �� ���̱�
	BOOL bHideStreetStallTitle;												// ���� Ÿ��Ʋ �Ⱥ��̱�

	int nMacroMode;															// ä�ÿ켱���/����Ű�켱���.
	int nWASDMode;

	BOOL bNoWhisper;														// �ӼӸ� �ź�.
	BOOL bNoChatting;														// �Ϲ�ä�� ����.
	BOOL bNoBalloon;														// ��ǳ�� ����.
	//BOOL bAutoHide;															// �޽��� ǥ��â �ڵ� ����.
	BOOL bNoPartyChat ;														// ��Ƽ ä�� ����.
	BOOL bNoFamilyChat ;													// �йи� ä�� ����.
	BOOL bNoShoutChat;														// ��ġ�� ����.
	BOOL bNoGuildChat;														// ��� ä�� ����.
	BOOL bNoAllianceChat;													// ����ä�� ����.
	BOOL bNoSystemMsg;														// �ý��� �޽��� ����.
	BOOL bNoExpMsg;															// ����ġ/����ġ ȹ��޽��� ����.
	BOOL bNoItemMsg;														// ��/������ ȹ��޽��� ����.

	int nSightDistance;														// �þ� �Ÿ� ��.

	BOOL bGraphicCursor;													// �׷��� Ŀ�� ����?

	BOOL bShadowHero;														// �׸��� ǥ�� - �ڽ�.
	BOOL bShadowMonster;													// �׸��� ǥ�� - ����/NPC.
	BOOL bShadowOthers;														// �׸��� ǥ�� - �ٸ�ĳ����.

	BOOL bAutoCtrl;															// ���� ȭ�� �ڵ�����.
	int nLODMode;															// �׷��� ���� ��.
	int nEffectMode;														// ����Ʈ ǥ�ð�.
	int nEffectSnow;														// �� ����Ʈ ����.

	BOOL bSoundBGM;															// ������� ON/OFF.
	BOOL bSoundEnvironment;													// ȿ���� ON/OFF.

	int	nVolumnBGM;															// ������� ����.
	int nVolumnEnvironment;													// ȿ���� ����.

	BOOL bAmbientMax;														//SW060904 ĳ��ȯ��ݻ簪 ���� �ɼ� �߰�

	// desc_hseos_�ɼ�01
	// S �ɼ� �߰� added by hseos 2007.06.19
	int	nChatLineNum;
	int nCurChatMode;
	// E �ɼ� �߰� added by hseos 2007.06.19

	// 071025 LYW === OptionManager : Add tutorial option.
	BOOL bShowTutorial ;													// Ʃ�丮�� ���̱� ���θ� ��� ����.

	// 080916 KTH -- OptionManager : Add Shout Dialog Option.
	BOOL bShowGeneralShoutDlg;
	DWORD dwHideParts;
	struct Vehicle
	{
		BOOL mIsNeedAllow;
		MONEYTYPE mChargedMoney;
	}
	mVehicle;

	// 100322 ONS ����Ʈ�˸��� �ڵ���� ����
	BOOL bAutoQuestNotice;	

	// 100629 ONS ����Ʈ��Ī �źα�� �߰�
	BOOL bNoDateMatching;

	// 100726 ���� ������ (HERO / MONSTER ) ��¿���
	BOOL bIsOnBattleGuage;

	BOOL bWingsMoveMode;
};

// 100112 ONS ���� ������, ��ҹ�ư ������ ��� ������ �������� �ǵ����� ���� ����
struct sSoundInfo
{
	int		m_nVolumnBgm;
	int		m_nVolumnEnvironment;
};

class COptionManager  
{
protected:

	sGAMEOPTION		m_GameOption;											// ���� �ɼ��� ��� �ִ� ����ü.

	sSoundInfo		m_SoundInfoBackup;										// 100112 ONS �ɼ� ���� ��ҽ� ���������� �ǵ������ ����.

	BOOL			m_bBGMSoundChanged;										// ��� ������ �ٲ������ ���θ� ��� ����.
	WORD			m_wSendOption;											// �ɼ� ���¸� ���� �� ����ϴ� �÷��� ����.

	BOOL			m_bShowMemberDamage ;

	COptionDialog*	m_pOptionDlg;											// �ɼ� ���̾�α� ������.

	cGuageBar*		m_pCammaGB;		//���� ������ �ȵ�						// ���� ��������.
	cGuageBar*		m_pSightGB;												// �þ߰Ÿ� ��������.

	LONG			m_SigntGBMinValue;										// �þ߰Ÿ� �ּ� ��.
	LONG			m_SigntGBMaxValue;										// �þ߰Ÿ� �ִ� ��.

	DWORD			m_dwShdowOption;										// �׸��� �ɼǰ��� ���� ����.

	enum {LOWEST=1,LOW,MID,HIGH,STANDARD_FRAME=15,NICE_FRAME=20};			// �ɼ� ���� ������ ���� �̳�.

	DWORD			m_dwLastCheckTime;										// ������ üũ �ð�.
	DWORD			m_dwCheck;												// üũ ��.
	DWORD			m_dwFrameAverage;										// ��� ������ ��.
	DWORD			m_dwCountFrame;											// ������ ī��Ʈ.
	int				m_nCurOptionState;										// ���� �ɼ� ����.
	
protected:
	BOOL LoadGameOption();

public:
	
	COptionManager();														// ������ �Լ�.
	virtual ~COptionManager();												// �Ҹ��� �Լ�.
	void Init();
	void SaveGameOption();
	void SetDefaultOption();
	void ApplySettings();
	void ApplySound();
	void ApplyGraphics();
	void ApplyChatting();
	void ApplyEnvironmemt();
	void CancelSettings();
	void SendOptionMsg();

	sGAMEOPTION* GetGameOption() { return &m_GameOption; }
	void SetGameOption( sGAMEOPTION* pOption, BOOL bSaveToFile = TRUE );
	void SetMemberDemageActive(BOOL bDo) { m_bShowMemberDamage = bDo; }
	BOOL IsShowMemberDamage() const { return m_bShowMemberDamage; }
	void ApplyGraphicSetting();
	void InitForGameIn();
	void Set_ShowTutorial(BOOL val) { m_GameOption.bShowTutorial = val; }
	BOOL Is_ShowTutorial() const { return m_GameOption.bShowTutorial; }
	void UpdateHideFlagFromDB(DWORD hideFlag) { m_GameOption.dwHideParts = hideFlag; }
	void UpdateMoveTypeFromDB(BOOL bValue) { m_GameOption.bWingsMoveMode = bValue; }
	void SoundInfoBackup();
};

EXTERNGLOBALTON(COptionManager);