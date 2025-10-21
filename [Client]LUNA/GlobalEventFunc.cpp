#include "stdafx.h"
#include "GlobalEventFunc.h"
#include "MainGame.h"
#include "CharMake.h"
#include "CharSelect.h"
#include "GameIn.h"
#include "MainTitle.h"
#include "cComboBoxEx.h"
#include "ObjectManager.h"
#include "MoveManager.h"
#include "AppearanceManager.h"
#include "WindowIdEnum.h"
#include "interface/cWindowManager.h"
#include "PartyManager.h"
#include "GuildManager.h"
#include "StorageManager.h"
#include "FriendManager.h"
#include "NoteManager.h"
#include "ChatManager.h"
#include "ItemManager.h"
#include "ExchangeManager.h"
#include "StreetStallManager.h"
#include "ShowdownManager.h"
#include "FilteringTable.h"
#include "CharMakeManager.h"
#include "ChannelDialog.h"
#include "CharacterDialog.h"
#include "InventoryExDialog.h"
#include "MixDialog.h"
#include "ProgressDialog.h"
#include "StallKindSelectDlg.h"
#include "StreetBuyStall.h"
#include "BuyRegDialog.h"
#include "StreetStall.h"
#include "BuyItem.h"
#include "cListDialogEx.h"
#include "NpcScriptDialog.h"
#include "MoneyDlg.h"
#include "DealDialog.h"
#include "MiniFriendDialog.h"
#include "FriendDialog.h"
#include "MiniNoteDialog.h"
#include "MiniNoteDialogRead.h"
#include "NoteDialog.h"
#include "OptionDialog.h"
#include "ExchangeDialog.h"
#include "PKLootingDialog.h"
#include "StorageDialog.h"
#include "MiniMapDlg.h"
#include "ExitDialog.h"
#include "ReinforceDlg.h"
#include "QuestDialog.h"
#include "QuestQuickViewDialog.h"
#include "PartyCreateDlg.h"
#include "PartyInviteDlg.h"
#include "QuestManager.h"
#include "GuildDialog.h"
#include "FamilyDialog.h"
#include "FamilyRankDialog.h"
#include "FamilyMarkDialog.h"
#include "FamilyNickNameDialog.h"
#include "MHMap.h"
#include "MHCamera.h"
#include "cMsgBox.h"
#include "cDivideBox.h"
#include "cmoneydividebox.h"
#include "./input/UserInput.h"
#include "./Audio/MHAudioManager.h"
#include "cImeEx.h"
#include "PKManager.h"
#include "ObjectStateManager.h"
#include "ReviveDialog.h"
#include "UserInfoManager.h"
#include "ExitManager.h"
#include "InventoryExDialog.h"
#include "PartyIconManager.h"
#include "ExchangeItem.h"
#include "ItemShopDialog.h"
#include "GuildFieldWarDialog.h"
#include "GuildFieldWar.h"
#include "ShoutDialog.h"
#include "PartyBtnDlg.h"
#include "MonsterGuageDlg.h"
#include "GTStandingDialog.h"
#include "GTBattleListDialog.h"
#include "GTScoreInfoDialog.h"
#include "GuildUnion.h"
#include "GuildUnionMarkMgr.h"
#include "GuageDialog.h"
#include "ReinforceGuideDialog.h"
#include "ServerListDialog.h"
#include "../Interface/cResourceManager.h"
#include "cSkillTreeDlg.h"
#include "ChattingDlg.h"
#include "CharMakeNewDlg.h"
#include "CertificateDlg.h"
#include "MainSystemDlg.h"
#include "NpcScriptManager.h"
#include "ActionTarget.h"
#include "NpcImageDlg.h"
#include "cJobSkillDlg.h"
#include "QuickDlg.h"
#include "cSkillTrainingDlg.h"
#include "DateMatchingDlg.h"
#include "../KeySettingTipDlg.h"
#include "../hseos/ResidentRegist/SHResidentRegistManager.h"
#include "../hseos/family/shfamilymanager.h"
#include "QuestTotalDialog.h"
#include "../hseos/Farm/SHFarmManager.h"
#include "../hseos/Farm/SHFarmBuyDlg.h"
#include "../hseos/Farm/SHFarmUpgradeDlg.h"
#include "../hseos/Farm/SHFarmManageDlg.h"
#include "TutorialManager.h"
#include "TutorialBtnDlg.h"
#include "cMapMoveDialog.h"
#include "../hseos/Date/SHDateZoneListDlg.h"
#include "../hseos/Date/SHChallengeZoneListDlg.h"
#include "../hseos/Date/SHChallengeZoneClearNo1Dlg.h"
#include "./Interface/cNumberPadDialog.h"
#include "AutoNoteDlg.h"
#include "AutoAnswerDlg.h"
#include "StoreSearchDlg.h"
#include "ChatRoomMgr.h"
#include "ChatRoomMainDlg.h"
#include "ChatRoomCreateDlg.h"
#include "ChatRoomJoinDlg.h"
#include "ChatRoomDlg.h"
#include "ChatRoomGuestListDlg.h"
#include "ChatRoomOptionDlg.h"
#include "FishingManager.h"
#include "FishingDialog.h"
#include "FishingPointDialog.h"
#include "petstatedialog.h"
#include "PetResurrectionDialog.h"
#include "../hseos/Farm/FarmAnimalDlg.h"
#include "petweareddialog.h"
#include "../[CC]SiegeDungeon/SiegeDungeonMgr.h"
#include "./NpcRecallMgr.h"
#include "MHMap.h"
#include "GuildTournamentMgr.h"
#include "popupmenudlg.h"
#include "CookDlg.h"
#include "RidePopupMenuDlg.h"
#include "VehicleManager.h"
#include "GradeClassDlg.h"
#include "HousingRevolDlg.h"			//090323 pdy �Ͽ�¡ ��ġ UI�߰�
#include "cHousingWarehouseDlg.h"		//090330 pdy �Ͽ�¡ â�� UI�߰�
#include "cHouseSearchDlg.h"			//090409 pdy �Ͽ�¡ �Ͽ콺 �˻� UI�߰�
#include "cHouseNameDlg.h"				//090410 pdy �Ͽ�¡ �Ͽ콺 �̸����� UI�߰�
#include "cHousingDecoPointDlg.h"		//090414 pdy �Ͽ�¡ �ٹ̱�����Ʈ ���� UI�߰�
#include "cHousingActionPopupMenuDlg.h" //090507 pdy �Ͽ�¡ �׼� �˾��Ŵ� UI�߰�
#include "cHousingMainPointDlg.h"		//090525 pdy �Ͽ�¡ ��������Ʈ UI�߰�
#include "cHousingWarehouseButtonDlg.h"	//090525 pdy �Ͽ�¡ ����â�� ��ư UI�߰�
#include "cHousingDecoModeBtDlg.h"		//090708 pdy �Ͽ�¡ �ٹ̱��� ��ư UI�߰�
#include "cHousingMgr.h"
#include "cHousingStoredIcon.h"
#include "InputNameDlg.h"
#include "cchangenamedialog.h"
#include "BigMapDlg.h"
#include "VideoCaptureManager.h"
#include "AdditionalButtonDlg.h"
#include "ConsignmentDlg.h"
#include "WorldMapDlg.h"
#include "MonsterTargetDlg.h"
#include "TransmogDialog.h"

extern HWND _g_hWnd;

extern int	g_nServerSetNum;
extern ScriptCheckValue g_Check;

FUNC g_mt_func[] =
{
	{MT_LogInOkBtnFunc,"MT_LogInOkBtnFunc"},
	{MT_EditReturnFunc, "MT_EditReturnFunc"},
	{MT_ExitBtnFunc,"MT_ExitBtnFunc"},
	{MT_DynamicBtnFunc,"MT_DynamicBtnFunc"},
	{MT_DynamicEditReturnFunc, "MT_DynamicEditReturnFunc"},

	{CS_BtnFuncCreateChar,"CS_BtnFuncCreateChar"}, 
	{CS_BtnFuncDeleteChar,"CS_BtnFuncDeleteChar"},	 
	{CS_BtnFuncFirstChar,"CS_BtnFuncFirstChar"},	 
	{CS_BtnFuncSecondChar, "CS_BtnFuncSecondChar"},
	{CS_BtnFuncThirdChar,"CS_BtnFuncThirdChar"},
	{CS_BtnFuncFourthchar,"CS_BtnFuncFourthchar"},
	{CS_BtnFuncFifthchar,"CS_BtnFuncFifthchar"},
	{CS_BtnFuncEnter,"CS_BtnFuncEnter"},//��E��c���סˢ�OAO�ˢ碮��oAAU ����Ioo�ˢ碮����E��c��E��c���סˢ� //10
	{CS_FUNC_OkISee,"CS_FUNC_OkISee"},
	{CS_BtnFuncLogOut,"CS_BtnFuncLogOut"},
	// 061218 LYW --- Add callback function to process events from change serverlist button.
	{CS_BtnFuncChangeServer, "CS_BtnFuncChangeServer"},
	// 061219 LYW --- Add callback function to process events from certificate dialog.
	{CTF_CBFunc, "CTF_CBFunc"},

	{MI_CharBtnFunc, "MI_CharBtnFunc"},
	{MI_ExchangeBtnFunc, "MI_ExchangeBtnFunc"},
	{MI_InventoryBtnFunc, "MI_InventoryBtnFunc"},
	{MI_SkillBtnFunc, "MI_SkillBtnFunc"},
	{IN_DlgFunc, "IN_DlgFunc"}, 
	{STD_SkillTreeDlgFunc, "STD_SkillTreeDlgFunc"},
	{CI_DlgFunc, "CI_DlgFunc"},
	{CI_AddExpPoint, "CI_AddExpPoint"},
	// 070111 LYW --- Add callback function to process events.
	{ CI_ActionEventFun, "CI_ActionEventFun" },
	{SO_DlgFunc, "SO_DlgFunc"},

	{BRS_DlgFunc, "BRS_DlgFunc"},
	{BRS_CloseFunc, "BRS_CloseFunc"},
	{BRS_DeleteFunc, "BRS_DeleteFunc"},
	{BRS_TypeListFunc, "BRS_TypeListFunc"},
	{BRS_ItemListFunc, "BRS_ItemListFunc"},
	{BRS_ClassListFunc, "BRS_ClassListFunc"},
	{BRS_REGBtnFunc, "BRS_REGBtnFunc"},

	{BS_DlgFunc, "BS_DlgFunc"},
	{BS_TitleEditBoxFunc, "BS_TitleEditBoxFunc"},
	{BS_SellBtnFunc, "BS_SellBtnFunc"}, 
	{SSI_DlgFunc, "SSI_DlgFunc"},
	{SSI_TitleEditBoxFunc, "SSI_TitleEditBoxFunc"},
	{SSI_BuyBtnFunc, "SSI_BuyBtnFunc"}, 
	{SSI_EditBtnFunc, "SSI_EditBtnFunc"}, 
	{NSI_HyperLinkFunc, "NSI_HyperLinkFunc"},

	{CMI_MoneyOkFunc, "CMI_MoneyOkFunc"},
	{DE_BuyItemFunc, "DE_BuyItemFunc"},//40
	{DE_SellItemFunc, "DE_SellItemFunc"},
	// 070503 LYW --- GlobalEventFunc : Add global event function .
	{ DE_CancelFunc, "DE_CancelFunc" },
	{DE_DlgFunc, "DE_DlgFunc"},
	{DE_CloseHideBtn, "DE_CloseHideBtn"},

	{PA_BtnFunc, "PA_BtnFunc"},

	// 080318 LUJ, ��Ƽ ��� â �̺�Ʈ ó��
	{PartyMemberFunc, "PartyMemberFunc" },

	{QI_QuickDlgFunc, "QI_QuickDlgFunc"},
	{CM_CharMakeBtnFunc, "CM_CharMakeBtnFunc"},
	{CM_CharCancelBtnFunc, "CM_CharCancelBtnFunc"}, 
	{PYO_DlgBtnFunc, "PYO_DlgBtnFunc"},
	{Note_DlgBtnFunc, "Note_DlgBtnFunc"},
	{Friend_DlgFunc, "Friend_DlgFunc"},			//100118 ONS ģ������ ���̾�α׿� PC���������� ����߰�
	{CR_DlgBtnFunc, "CR_DlgBtnFunc"},

	{MNM_DlgFunc,"MNM_DlgFunc"},
	{ITD_DlgFunc,"ITD_DlgFunc"},
	{ITMD_DlgFunc,"ITMD_DlgFunc"},  //60
	{ITR_DlgFunc, "ITR_DlgFunc"}, //

	// 080215 LUJ, �ɼ� ���� â�� ���� �Լ�
	{ApplyOptionDialogFunc, "ApplyOptionDialogFunc"},
	{CHA_DlgBtnFunc, "CHA_DlgBtnFunc"},
	{OTI_DlgFunc, "OTI_DlgFunc"},	//��E��c���סˢ�OAO�ˢ�E?E�ˢ碮��uCA��E��cE
	{EXT_DlgFunc, "EXT_DlgFunc"},	//A�ˢ碮��u��E��c��E�ˡ�aA��E��cE	//
	{XCI_DlgFunc, "XCI_DlgFunc"},	//��E��c����u����I��I����E?A��E��cE
	{PLI_DlgFunc, "PLI_DlgFunc"},
	{CNA_BtnOkFunc, "CNA_BtnOkFunc"},
	{DIVIDEBOX_Func, "DIVIDEBOX_Func"},	//Divide Box
	{MONEYDIVIDEBOX_Func, "MONEYDIVIDEBOX_Func"},	//Divide Box
	{SL_DlgBtnFunc, "SL_DlgBtnFunc" },
	{QUE_QuestDlgFunc, "QUE_QuestDlgFunc"},
	{GD_WarehouseFunc,	"GD_WarehouseFunc"	},
	{GD_DlgFunc,		"GD_DlgFunc"		},
	{GDREVIVAL_DlgFunc,	"GDREVIVAL_DlgFunc"	},
	{GDLEVEL_DlgFunc,	"GDLEVEL_DlgFunc"	},
	{GDRANK_DlgFunc,	"GDRANK_DlgFunc"	},
	{GDMARK_DlgFunc,	"GDMARK_DlgFunc"	},
	{GDCREATE_DlgFunc,	"GDCREATE_DlgFunc"	},
	{GDINVITE_DlgFunc,	"GDINVITE_DlgFunc"	},
	{GDWHRANK_DlgFunc,	"GDWHRANK_DlgFunc"	},
	{GDUnion_DlgFunc,	"GDUnion_DlgFunc"	},
	{GDNICK_DlgFunc,	"GDNICK_DlgFunc"	},

	{PROGRESS_DlgFunc, "PROGRESS_DlgFunc" },

	{FD_DlgFunc, "FD_DlgFunc"},

	// guildfieldwar
	{GFW_DlgFunc, "GFW_DlgFunc" },
	// Guild Tournament
	{GDT_DlgFunc, "GDT_DlgFunc"},
	{CG_DlgFunc, "CG_DlgFunc"},
	// 080916 LUJ, �ռ� â ���� �Լ�
	{ComposeDialogFunc, "ComposeDialogFunc" },
	// 06. 02. ��ȭ ��� �ȳ� �������̽� �߰� - �̿���
	{RFGUIDE_DLGFunc, "RFGUIDE_DLGFunc"},
	{GN_DlgBtnFunc, "GN_DlgBtnFunc"},
	{AN_DlgBtnFunc, "AN_DlgBtnFunc"},
	// 06. 03. ���İ��� - �̿���
	{GNotice_DlgBtnFunc, "GNotice_DlgBtnFunc"},

	//060802 GuildPlusTime - wonju
	{GuildPlusTime_DlgFunc, "GuildPlusTime_DlgFunc"},
	{ CM_CMNCbFunc, "CM_CMNCbFunc" },
	{ MDS_CBFunc, "MDS_CBFunc" },
	{ CharMain_BtnFunc, "CharMain_BtnFunc" },
	{JO_DlgFunc, "JO_DlgFunc"},
	{DMD_DateMatchingDlgFunc, "DMD_DateMatchingDlgFunc"},			// ��Ī ���̾�α� �׽�Ʈ. 2007/03/15 ������
	{DMD_PartnerDlgFunc, "DMD_PartnerDlgFunc"}, 
	{DMD_IDT_Func, "DMD_IDT_Func"}, 
	{DMD_FID_Func, "DMD_FID_Func"},
	{DMD_MID_Func, "DMD_MID_Func"}, 
	{DMD_RID_Func, "DMD_RID_Func"}, 
	{KS_Func, "KS_Func"}, 

	{SkillTrainDlgFunc, "SkillTrainDlgFunc"},
	{FD_GuildDlg,			"FD_GuildDlg"},
	{FARM_BUY_DlgFunc,		"FARM_BUY_DlgFunc"},
	{FARM_UPGRADE_DlgFunc,	"FARM_UPGRADE_DlgFunc"},
	{FARM_MANAGE_DlgFunc ,	"FARM_MANAGE_DlgFunc"},
	{TTR_DlgFunc, "TTR_DlgFunc"}, 
	{TTR_BTNDlgFunc, "TTR_BTNDlgFunc"}, 
	{HD_DlgFunc, "HD_DlgFunc"}, 
	{MapMove_DlgFunc, "MapMove_DlgFunc"}, 
	{DATE_ZONE_LIST_DlgFunc ,		"DATE_ZONE_LIST_DlgFunc"},
	{CHALLENGE_ZONE_LIST_DlgFunc ,	"CHALLENGE_ZONE_LIST_DlgFunc"},
	{CHALLENGE_ZONE_LIST_DlgFunc ,	"CHALLENGE_ZONE_CLEARNO1_DlgFunc"},
	{Change_Name_Func, "Change_Name_Func"}, 
	{NumberPad_DlgFunc, "NumberPad_DlgFunc"}, 
	{AutoNoteDlg_Func, "AutoNoteDlg_Func"},
	{AutoAnswerDlg_Func, "AutoAnswerDlg_Func"},
	{StoreSearchDlg_Func, "StoreSearchDlg_Func"},
	{Shout_DlgBtnFunc, "Shout_DlgBtnFunc"},
	{BodyChangeFunc, "BodyChangeFunc"},
	{ChatRoomMainDlg_Func,		"ChatRoomMainDlg_Func"},
	{ChatRoomCreateDlg_Func,	"ChatRoomCreateDlg_Func"},
	{ChatRoomDlg_Func,			"ChatRoomDlg_Func"}, 
	{ChatRoomGuestListDlg_Func, "ChatRoomGuestListDlg_Func"}, 
	{ChatRoomJoinDlg_Func,		"ChatRoomJoinDlg_Func"}, 
	{ChatRoomOptionDlg_Func,	"ChatRoomOptionDlg_Func"},
	{ChatRoomMainDlg_Func,		"ChatRoomMainDlg_Func"},
	{Fishing_DlgBtnFunc, "Fishing_DlgBtnFunc"},
	{Animal_DlgBtnFunc,	"Animal_DlgBtnFunc"},

	{PET_STATUS_DLG_FUNC, "PET_STATUS_DLG_FUNC"},
	{PET_RES_DLG_FUNC, "PET_RES_DLG_FUNC"},
	
	{PopupMenuDlgFunc, "PopupMenuDlgFunc" },

	{COOK_DLG_FUNC, "COOK_DLG_FUNC"},
	// 090422 ShinJS --- Ż���� ���� �˾� ���̾�α� �߰��۾�
	{RidePopupMenuDlgFunc, "RidePopupMenuDlgFunc" },

	//090323 pdy �Ͽ�¡ ��ġ UI�߰�
	{HousingRevolDlgFunc, "HousingRevolDlgFunc"},

	//090330 pdy �Ͽ�¡ â�� UI�߰�
	{Housing_WarehouseDlg_Func, "Housing_WarehouseDlg_Func" },

	//090409 pdy �Ͽ�¡ �Ͽ콺 �˻� UI�߰�
	{HouseSearchDlgFunc,"HouseSearchDlgFunc"},

	//090410 pdy �Ͽ�¡ �Ͽ콺 �̸����� UI�߰�
	{HouseNameDlgFunc,"HouseNameDlgFunc"},

	//090414 pdy �Ͽ�¡ �ٹ̱�����Ʈ ���� UI�߰�
	{HousingDecoPointDlgFunc,"HousingDecoPointDlgFunc"},
	//090507 pdy �Ͽ�¡ �׼� �˾��Ŵ� UI�߰�
	{HousingActionPopupMenuDlgFunc,"HousingActionPopupMenuDlgFunc"},

	//090525 pdy �Ͽ�¡ ����â�� ��ư UI�߰�
	{HousingWarehouseButtonDlgFunc,"HousingWarehouseButtonDlgFunc"},

	// 090525 ShinJS --- Party �ٸ��ʿ� �ִ� ������ ���� �ʴ�� �̸��� �Է��ϴ� Dialog �߰�
	{InputNameDlgFunc, "InputNameDlgFunc"},

	//090708 pdy �Ͽ�¡ �ٹ̱��� ��ư UI�߰�
	{HousingDecoModeBtDlgFunc,"HousingDecoModeBtDlgFunc"},

	//091012 pdy �Ͽ�¡ �ٹ̱� ����Ʈ ��ư ��ȹ���� 
	{HousingMainPointDlgFunc,"HousingMainPointDlgFunc"},

	//091223 NYJ �Ͽ�¡ �ٸ����湮
	{HousingSearchDlgFunc,"HousingSearchDlgFunc"},

	// 091210 ShinJS --- ��ȭ Dialog �߰�
	{VideoCaptureDlgFunc, "VideoCaptureDlgFunc"},

	// 100111 ONS �ΰ����� ��ư ���̾�α� �߰�
	{AdditionalButtonDlgFunc, "AdditionalButtonDlgFunc"},

	// 100511 ONS �������� ���̾�α� �߰�
	{ChangeClassDlgFunc, "ChangeClassDlgFunc"},
	// 100503 NYJ �ǸŴ���
	{ConsignmentDlgFunc, "ConsignmentDlgFunc"},
	// 100629 ����Ÿ�� ���̾˷α�
	{MonsterTargetDlgFunc,"MonsterTargetDlgFunc"},
	// 100709 ONS ����Ʈ�˸��� ������ư ó�� �߰�
	{QuickView_DlgFunc, "QuickView_DlgFunc"},
	// 100628 ShinJS ����� ���̾�α� �߰�
	{WorldMapDlgFunc, "WorldMapDlgFunc"},
	{BigMapDlgFunc, "BigMapDlgFunc"},
	{ITTD_TransmogDlgFunc,"ITTD_TransmogDlgFunc"},
	{NULL, ""},	//�������ٿ� �̰� �� ���μ���.
};

int FUNCSEARCH(char * funcName)
{
	for(int i = 0 ;; i++)
	{
		if(g_mt_func[i]._func == NULL)
			break;
		if(strcmp(funcName, g_mt_func[i]._funcName)==0)
			return i;
	}

	return -1; 
}

void CS_BtnFuncChangeServer( LONG lId, void* p, DWORD we )
{
	CHARSELECT->BackToMainTitle();
	TITLE->SetServerList();
	CAMERA->ResetInterPolation();
}

void CS_BtnFuncLogOut(LONG lId, void* p, DWORD we)
{
	PostMessage( _g_hWnd, WM_CLOSE, NULL, NULL );
}
void CS_BtnFuncCreateChar(LONG lId, void* p, DWORD we)
{
	if(CHARSELECT->IsFull() == FALSE)
	{
		MAINGAME->SetGameState(eGAMESTATE_CHARMAKE);
		CAMERA->MoveStart(eCP_CHARMAKE);
	}
	else
	{
		CHARSELECT->DisplayNotice(17);
	}
}

void		CS_BtnFuncFirstChar(LONG lId, void* p, DWORD we)
{
	if( we & WE_PUSHDOWN )
		CHARSELECT->SelectPlayer(0);
	else if( we & WE_PUSHUP )
		((cPushupButton*)((cDialog*)p)->GetWindowForID(lId))->SetPush( TRUE );
}
void		CS_BtnFuncSecondChar(LONG lId, void* p, DWORD we)
{
	if( we & WE_PUSHDOWN )
		CHARSELECT->SelectPlayer(1);
	else if( we & WE_PUSHUP )
		((cPushupButton*)((cDialog*)p)->GetWindowForID(lId))->SetPush( TRUE );
}
void		CS_BtnFuncThirdChar(LONG lId, void* p, DWORD we)
{
	if( we & WE_PUSHDOWN )
		CHARSELECT->SelectPlayer(2);
	else if( we & WE_PUSHUP )
		((cPushupButton*)((cDialog*)p)->GetWindowForID(lId))->SetPush( TRUE );
}
void		CS_BtnFuncFourthchar(LONG lId, void* p, DWORD we)
{
	if( we & WE_PUSHDOWN )
		CHARSELECT->SelectPlayer(3);
	else if( we & WE_PUSHUP )
		((cPushupButton*)((cDialog*)p)->GetWindowForID(lId))->SetPush( TRUE );
}
void		CS_BtnFuncFifthchar(LONG lId, void* p, DWORD we)
{
	if( we & WE_PUSHDOWN )
		CHARSELECT->SelectPlayer(4);
	else if( we & WE_PUSHUP )
		((cPushupButton*)((cDialog*)p)->GetWindowForID(lId))->SetPush( TRUE );
}
void CS_BtnFuncEnter(LONG lId, void* p, DWORD we)
{
	if(	CHARSELECT->GetCurSelectedPlayer() == NULL)
		CHARSELECT->DisplayNotice(18);
	else
	{
		CHARSELECT->SendMsgGetChannelInfo();
		CHARSELECT->SetDisablePick(TRUE);
	} 
}

void CS_FUNC_OkISee(LONG lId, void* p, DWORD we)	 
{
	cStatic * CharacterSelectMessageDlg = (cStatic *)WINDOWMGR->GetWindowForIDEx(CSID_MessageBox);
	CharacterSelectMessageDlg->SetActive(FALSE);
}

void CS_BtnFuncDeleteChar(LONG lId, void* p, DWORD we)
{
	int nChar = CHARSELECT->GetCurSelectedPlayerNum();
	int MaxChar = MAX_CHARACTER_NUM-1;

	const DISPLAY_INFO& dispInfo = GAMERESRCMNGR->GetResolution();
	LONG Xpos = (LONG)(dispInfo.dwWidth/2 - 100) ;	// 100 is half of msgbox's width.
	LONG Ypos = (LONG)(dispInfo.dwHeight/2 - 57) ;	// 57 is half of msgbox's height.

	// 070205 LYW --- Add a conditional sentence.
	if( nChar >= 0 && nChar <= MaxChar )
	{
		cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_DELETECHAR, MBT_YESNO, RESRCMGR->GetMsg( 267 ) );

		if( pMsgBox )
			pMsgBox->SetAbsXY( Xpos, Ypos );
	}
	else
	{
		CHARSELECT->DisplayNotice( 20 );
	}

	CHARSELECT->SetDisablePick( TRUE );
}

// 061219 LYW --- Add function to process events from certificate dialog.
void CTF_CBFunc( LONG lId, void* p, DWORD we )
{
	CHARSELECT->GetCertificateDlg()->OnActionEvent( lId, p, we ) ;
}

extern void	CM_CharMakeBtnFunc(LONG lId, void* p, DWORD we)
{
	cEditBox * editboxName = (cEditBox *)WINDOWMGR->GetWindowForIDEx(CM_IDEDITBOX);
	char* pName = editboxName->GetEditText(); 
	int nLen	= strlen(pName);

	if( nLen == 0 )
	{
		// 070125 LYW --- CharMake : Modified message number.
		//CHARMAKE->DisplayNotice( 11 );
		CHARMAKE->DisplayNotice( 11 );
		return;
	}
	else if( nLen < 4 )
	{
		CHARMAKE->DisplayNotice( 19 );
		return;
	}

	if( FILTERTABLE->IsInvalidCharInclude((unsigned char*)pName) )
	{
		CHARMAKE->DisplayNotice( 14 );
		return;
	}

	if( !FILTERTABLE->IsUsableName(pName) )
	{
		CHARMAKE->DisplayNotice( 14 );
		// 061219 LYW --- Setting to null editbox.
		cEditBox* pEdit = ( cEditBox* )CHARMAKE->GetCharMakeNewDlg()->GetWindowForID( CM_IDEDITBOX ) ;
		pEdit->SetEditText("") ;
		return;
	}

	CHARACTERMAKEINFO msg;
	ZeroMemory(&msg, sizeof(CHARACTERMAKEINFO)) ;
	CHARMAKEMGR->GetCharMakeNewDlg()->ApplyOption( &msg ) ;

	msg.Category = MP_USERCONN;
	msg.Protocol = MP_USERCONN_CHARACTER_MAKE_SYN;
	msg.StandingArrayNum = BYTE(-1);
	strcpy(msg.Name, editboxName->GetEditText());

	NETWORK->Send(&msg,sizeof(msg));

	CHARMAKE->SetDisableDlg( TRUE );
	//	CHARMAKE->GetCharMakeDialog()->SetDisable( TRUE );
}

extern void	CM_CharCancelBtnFunc(LONG lId, void* p, DWORD we)
{
	MSGBASE msg;
	msg.Category = MP_USERCONN;
	msg.Protocol = MP_USERCONN_DIRECTCHARACTERLIST_SYN;
	NETWORK->Send(&msg,sizeof(msg));	

	//
	CAMERA->MoveStart(eCP_CHARSEL);
}

extern char g_szHeroIDName[];
extern char g_CLIENTVERSION[];

//=========================================================================
//	NAME : MT_LogInOkBtnFunc
//	DESC : The function to management action event of all controls in main title. 080109 LYW
//=========================================================================
void MT_LogInOkBtnFunc(LONG lId, void* p, DWORD we)
{
	ScriptCheckValue check;
	SetScriptCheckValue( check );

	if( check.mValue != g_Check.mValue )
	{
#ifndef _GMTOOL_
		// 090120 LUJ, ��ġ ������ ���ʷ� ������
		{
			const char* const	versionFile = "LunaVerInfo.ver";
			FILE* const			file		= fopen( versionFile, "r+" );
			if( file )
			{
				// 090109 LUJ, 4���ڷ� ������ ���� ����� ���Ͽ��� �о�� ���ڿ� ������ ��´�
				const size_t headerSize = 4;
				char header[ headerSize + 1 ] = { 0 };
				fread(
					header,
					headerSize,
					sizeof( *header ),
					file );

				// 090109 LUJ, ��ü ��ġ�� ������ ���� ��ȣ�� �����
				char versionText[ MAX_PATH ] = { 0 };
				sprintf(
					versionText,
					"%s00000000",
					header );
				// 090109 LUJ, ��ġ ���� ���ڿ��� ���Ͽ� ����
				fseek(
					file,
					0,
					SEEK_SET );
				fwrite(
					versionText,
					sizeof( *versionText ),
					strlen( versionText ),
					file );
				fclose( file );
			}
		}

		PostMessage( _g_hWnd, WM_CLOSE, NULL, NULL );
		return ;
#endif
	}

	cDialog * dlg = (cDialog*)p;

	if( !dlg ) return;

	cNumberPadDialog* pPad = ((cNumberPadDialog*)WINDOWMGR->GetWindowForID( MT_LOGINDLG ));

	if( !pPad ) return ;														// ���� �е��� ���Ἲ�� üũ����.

	cEditBox * editboxId = NULL ;												// ���̵� ����Ʈ �ڽ��� �޴´�.
	editboxId = (cEditBox *)dlg->GetWindowForID(MT_IDEDITBOX) ;					

	if( !editboxId ) return ;													// ���̵� ����Ʈ �ڽ��� ���Ἲ�� üũ����.

	cEditBox * editboxPwd = NULL ;												// �н����� ����Ʈ �ڽ��� �޴´�.
	editboxPwd = (cEditBox *)dlg->GetWindowForID(MT_PWDEDITBOX) ;				

	if( !editboxPwd ) return ;													// �н����� ����Ʈ �ڽ��� ���Ἲ�� üũ����.

	cDialog* pIDDlg = NULL ;
	pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG ) ;							// �α��� ���̾�α׸� �޴´�.

	if( !pIDDlg ) return ;														// �α��� ���̾�α��� ���Ἲ�� üũ�Ѵ�.

	cMsgBox* pMsgBox = NULL ;													

	//--------------------------------------------------------------------------

	char * userID = editboxId->GetEditText() ;									// ���̵� �޴´�.
	char * userPWD = editboxPwd->GetEditText() ;								// �н����带 �޴´�.

	//------------------------------------------------------------------------- // ���̵�� ��� ��ȣ�� ����� �Է� �ߴٸ�,
#ifdef _KOR_WEBLAUNCHER_
	if( strlen( pPad->GetNumberStr() ) > 0 )
#else
	if((strcmp(userID, "") != 0) && (strcmp(userPWD, "") != 0))					
#endif
	{
		MSG_LOGIN_SYN msg ;														// �α��� ������ ���� �޽��� ����ü�� �����Ѵ�.
		memset(&msg, 0, sizeof(MSG_LOGIN_SYN)) ;								// �޽��� ����ü�� �ʱ�ȭ �Ѵ�.

		msg.Category = MP_USERCONN ;											// ī�װ����� ���������� �����Ѵ�.
		msg.Protocol = MP_USERCONN_LOGIN_SYN ;

		SafeStrCpy(msg.id, userID, MAX_NAME_LENGTH + 1 ) ;						// ���̵� ����.
		SafeStrCpy(msg.pw, userPWD, MAX_NAME_LENGTH + 1 ) ;						// ��й�ȣ ����.

		// ���� ��й�ȣ �߰� ó��.
		strcpy(msg.strSecurityPW, pPad->GetNumberStr()) ;

		SafeStrCpy(msg.Version,g_CLIENTVERSION, 16 ) ;							// Ŭ���̾�Ʈ ���� ����.

		msg.AuthKey = TITLE->GetDistAuthKey() ;									// ����Ű�� ��´�.
		
		msg.Check.mValue = check.mValue;

		SafeStrCpy(
			msg.mLoginKey,
			TITLE->GetLoginKey().c_str(),
			_countof(msg.mLoginKey));

		NETWORK->Send(&msg,sizeof(msg)) ;										// �޽����� �����Ѵ�.

//---KES �α��� ������ �� �α����� �� �ȵǴ� �� ����
		NETWORK->m_bDisconnecting = FALSE;
//---------------------------------------------

		SafeStrCpy(g_szHeroIDName, userID, MAX_NAME_LENGTH + 1 ) ;				// ���̵� �޴´�.

		pMsgBox = WINDOWMGR->MsgBox( MBI_WAIT_LOGINCHECK,						// �α��� ���̶�� �޽��� �ڽ��� �޴´�.
								 MBT_NOBTN, 
								 RESRCMGR->GetMsg( 265 ) ) ;

		if( pMsgBox )
		{
			float fXpos = pIDDlg->GetAbsX() ;									// X��ǥ�� �޴´�.
			float fYpos = pIDDlg->GetAbsY() + pIDDlg->GetHeight() ;				// Y��ǥ�� �޴´�.

			pMsgBox->SetAbsXY( fXpos, fYpos ) ;									// �޽��� �ڽ��� ��ġ�� �����Ѵ�.
		}

		TITLE->StartWaitConnectToAgent( TRUE ) ;								// ������Ʈ ������ ��ٸ���.

		USERINFOMGR->SetUserID( userID ) ;										// ���� ���̵� �����Ѵ�.
	}
	else	
	{
		cMsgBox* pMsgBox = WINDOWMGR->MsgBox(
			MBI_LOGINEDITCHECK, 
			MBT_OK, 
#ifdef _KOR_WEBLAUNCHER_
			CHATMGR->GetChatMsg(1478)); 	// "��й�ȣ�� �ùٸ��� �Է��� �ּ���."
#else
			CHATMGR->GetChatMsg(208));
#endif


		if( pMsgBox )
		{
			float fXpos = pIDDlg->GetAbsX() ;									// X��ǥ�� �޴´�.
			float fYpos = pIDDlg->GetAbsY() + pIDDlg->GetHeight() ;				// Y��ǥ�� �޴´�.

			pMsgBox->SetAbsXY( fXpos, fYpos ) ;									// �޽��� �ڽ��� ��ġ�� �����Ѵ�.
		}
	}

	editboxId->SetFocusEdit(FALSE) ;											// ���̵�, ��� ����Ʈ �ڽ��� ��Ŀ���� ���� �Ѵ�.
	editboxPwd->SetFocusEdit(FALSE) ;
	dlg->SetDisable( TRUE ) ;													// ���̵�, ��� â�� ��Ȱ��ȭ �Ѵ�.
}

void MT_DynamicBtnFunc(LONG lId, void * p, DWORD we)
{
	cDialog* dlg = (cDialog*)p;

	if( lId == MT_DYNAMICOKBTN )
	{
		cEditBox* editboxPwd = (cEditBox*)dlg->GetWindowForID(MT_DYNAMICPWDEDITBOX);
		char* userPWD = editboxPwd->GetEditText();
		if( strcmp(userPWD, "") != 0 )
		{
			MSG_LOGIN_DYNAMIC_SYN msg;
			msg.Category = MP_USERCONN;
			msg.Protocol = MP_USERCONN_LOGIN_DYNAMIC_SYN;
			msg.AuthKey = TITLE->GetDistAuthKey();
			SafeStrCpy( msg.id, g_szHeroIDName, MAX_NAME_LENGTH + 1 );
			SafeStrCpy( msg.pw, userPWD, MAX_NAME_LENGTH + 1 );
			NETWORK->Send( &msg, sizeof(msg) );

			cMsgBox* pMsgBox = (cMsgBox*)WINDOWMGR->GetWindowForID( MBI_WAIT_LOGINCHECK );
			if( pMsgBox )
			{
				pMsgBox->SetDisable( FALSE );
				pMsgBox->SetActive( TRUE );
				cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
				if( pIDDlg )
				{
					pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );
				}
			}

			TITLE->StartWaitConnectToAgent( TRUE );
		}
		else
		{
			cMsgBox* pMsgBox = WINDOWMGR->MsgBox(MBI_CANTUSEDYNAMIC, MBT_OK, CHATMGR->GetChatMsg(208) ); 
			cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
			if( pIDDlg && pMsgBox )
			{
				pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );		
			}
		}

		editboxPwd->SetFocusEdit(FALSE);
		dlg->SetDisable( TRUE );
	}
	else if( lId == MT_DYNAMICEXITBTN )
	{
		((cEditBox*)dlg->GetWindowForID(MT_DYNAMICPWDEDITBOX))->SetEditText( "" );

		dlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
		((cEditBox*)dlg->GetWindowForID(MT_IDEDITBOX))->SetEditText( "" );
		((cEditBox*)dlg->GetWindowForID(MT_PWDEDITBOX))->SetEditText( "" );

		TITLE->NoDisconMsg();
		TITLE->ShowServerList();
	}
}

void MT_DynamicEditReturnFunc( LONG lId, void* p, DWORD we )
{
	if(we == WE_RETURN)
	{
		if( lId == MT_DYNAMICPWDEDITBOX )
		{
			MT_DynamicBtnFunc( MT_DYNAMICOKBTN, p, we );
		}
	}
}

void MT_EditReturnFunc(LONG lId, void * p, DWORD we)
{
	if(we == WE_RETURN)
	{
		cDialog* dlg = (cDialog*)p;
		cEditBox* pEdit;
		if( lId == MT_IDEDITBOX )
		{
			pEdit = (cEditBox *)dlg->GetWindowForID(MT_IDEDITBOX);
			if( *pEdit->GetEditText() != 0 )
			{
				WINDOWMGR->SetNextEditFocus();
			}
		}
		else if( lId == MT_PWDEDITBOX )
		{
			pEdit = (cEditBox *)dlg->GetWindowForID(MT_PWDEDITBOX);
			if( *pEdit->GetEditText() != 0 )
			{
				MT_LogInOkBtnFunc( MT_OKBTN, p, WE_BTNCLICK );
			}
		}
	}
}

void MT_ExitBtnFunc(LONG lId, void * p, DWORD we)
{
	cDialog* dlg = (cDialog *)p;
	cEditBox* editboxId = (cEditBox *)dlg->GetWindowForID(MT_IDEDITBOX);
	cEditBox* editboxPwd = (cEditBox *)dlg->GetWindowForID(MT_PWDEDITBOX);

	editboxId->SetEditText("");
	editboxPwd->SetEditText("");
	editboxId->SetFocusEdit(TRUE);
	editboxPwd->SetFocusEdit(FALSE);

	TITLE->NoDisconMsg();
	TITLE->ShowServerList();
}

////////////////////////////////////// Main Interface //////////////////////////////////////////
void MI_CharBtnFunc(LONG lId, void* p, DWORD we)
{
	if(we == WE_PUSHUP)
	{
		GAMEIN->GetCharacterDialog()->SetActive(FALSE);
	}
	else if(we == WE_PUSHDOWN)
	{
		GAMEIN->GetCharacterDialog()->UpdateData();
		GAMEIN->GetCharacterDialog()->SetActive(TRUE);
	}
}
void MI_ExchangeBtnFunc(LONG lId, void* p, DWORD we)
{
	static BOOL run = 0;
	if(run)
	{
		MSGBASE msg;
		msg.Category = MP_MOVE;
		msg.Protocol = MP_MOVE_WALKMODE;
		msg.dwObjectID = HEROID;
		NETWORK->Send(&msg,sizeof(msg));
		run = 0;
		MOVEMGR->SetWalkMode(HERO);
	}
	else
	{
		MSGBASE msg;
		msg.Category = MP_MOVE;
		msg.Protocol = MP_MOVE_RUNMODE;
		msg.dwObjectID = HEROID;
		NETWORK->Send(&msg,sizeof(msg));
		run = 1;
		MOVEMGR->SetRunMode(HERO);
	}
}

void MI_InventoryBtnFunc(LONG lId, void* p, DWORD we)
{
	if(we == WE_PUSHUP)
	{
		GAMEIN->GetInventoryDialog()->SetActive(FALSE);
	}
	else if(we == WE_PUSHDOWN)
	{
		GAMEIN->GetInventoryDialog()->SetActive(TRUE);
	}
}

void MI_SkillBtnFunc(LONG lId, void* p, DWORD we)
{
	//MessageBox(NULL,"����Io��E��ci��E��c���סˢ碮��I���ע�IA��E��cE Aa��E��c��E�ˡ�A",NULL,NULL);
	if(we == WE_PUSHUP)
	{
		GAMEIN->GetSkillTreeDlg()->SetActive(FALSE);
	}
	else if(we == WE_PUSHDOWN)
	{
		GAMEIN->GetSkillTreeDlg()->SetActive(TRUE);
	}

}

void IN_DlgFunc(LONG lId, void * p, DWORD we)
{
	if(we == WE_LBTNCLICK || we == WE_LBTNDBLCLICK || we == WE_RBTNDBLCLICK || WE_RBTNCLICK)
	{
		GAMEIN->GetInventoryDialog()->OnActionEvnet(lId, p, we);
	}
}

void STD_SkillTreeDlgFunc(LONG lId, void * p, DWORD we)
{
	if( lId == STD_SKILLTRAININGBTN && we & WE_BTNCLICK )
	{
		GAMEIN->GetSkillTrainingDlg()->OpenDialog();
	}
	else if( lId == CMI_CLOSEBTN )
	{
		GAMEIN->GetSkillTreeDlg()->SetActive( FALSE );
	}
	else if( p )
	{
		( ( cJobSkillDlg* )p )->OnActionEvnet(lId, p, we);
	}
}

void CI_DlgFunc(LONG lId, void * p, DWORD we)
{
	if( lId == CI_CLOSEBTN )
	{
		GAMEIN->GetCharacterDialog()->SetActive( !( GAMEIN->GetCharacterDialog()->IsActive() ) );
		// 070115 LYW --- Close tree dialog.
		GAMEIN->GetCharacterDialog()->SetViewTreeDlg( FALSE ) ;
	}
}
void CI_AddExpPoint(LONG lId, void * p, DWORD we)
{
	BYTE whats=STR_POINT;
	switch(lId)
	{
	case CI_STATUS_BTN_0:
		whats=STR_POINT;
		break;
	case CI_STATUS_BTN_1:
		whats=DEX_POINT;
		break;
	case CI_STATUS_BTN_2:
		whats=VIT_POINT;
		break;
	case CI_STATUS_BTN_3:
		whats=INT_POINT ;
		break;
	case CI_STATUS_BTN_4 :
		whats=WIS_POINT;
		break ;
	}

	if( lId==CI_STATUS_BTN_0 || lId==CI_STATUS_BTN_1 || lId==CI_STATUS_BTN_2 || lId==CI_STATUS_BTN_3 || lId==CI_STATUS_BTN_4 )
		GAMEIN->GetCharacterDialog()->OnAddPoint(whats);
}

// 070111 LYW --- Add function to process events.
void CI_ActionEventFun( LONG lId, void* p, DWORD we )
{
	GAMEIN->GetCharacterDialog()->OnActionEvent( lId, p, we ) ;
}

void SSI_TitleEditBoxFunc(LONG lId, void * p, DWORD we)
{
	if(we == WE_RETURN)
	{
		// A|�ˢ�E�ˢ�ϡ�n�ˢ�E?�ˢ�E��Ec �ˢ�E�ˢ�IeCN �ˢ�E?����Io�ˢ�E?UA����I��I���̡ˢ�E�ˢ�ϡ��ˢ�Ec CO��E��c���סˢ�I..
		STREETSTALLMGR->EditTitle();
	}
}

void SSI_DlgFunc(LONG lId, void * p, DWORD we)
{
	GAMEIN->GetStreetStallDialog()->OnActionEvnet(lId, p, we);
}

void SSI_BuyBtnFunc(LONG lId, void * p, DWORD we)
{
	CStreetStall* pStallDlg = GAMEIN->GetStreetStallDialog();

	if( pStallDlg->GetCurSelectedItemNum() == -1 ) return;

	if( we == WE_BTNCLICK )
	{

		if( !pStallDlg->SelectedItemCheck() ) return;

		// FOR FIND_SS_BUG
		if( ITEMMGR->IsDupItem( pStallDlg->GetCurSelectedItemIdx() ) ) // �ˢ�E?�ˢ�Ii�ˢ碮��uaAI����I��I���̡ˢ碮���ˢ�I �ˢ碮����E��c?��E��cie����Io��E��c���סˢ� �ˢ碮��u�ˢ碮����E��cAIAUAI ��E��c���סˢ碮��I��E��c�ˢ�E?i
		{
			if( pStallDlg->GetCurSelectedItemDur() == 1 )
				STREETSTALLMGR->BuyItem();
			else
				GAMEIN->GetStreetStallDialog()->ShowDivideBox();
		}
		else
		{
			STREETSTALLMGR->BuyItem();
		}
	}
}

void SSI_EditBtnFunc(LONG lId, void * p, DWORD we)
{
	if( GAMEIN->GetStreetStallDialog()->GetCurSelectedItemNum() == -1 ) return;

	CStreetStall* pStall = (CStreetStall*)p;
	if(we == WE_BTNCLICK)
	{

		CExchangeItem* pExItem = pStall->FindItem( pStall->GetCheckDBIdx() );
		if( pExItem == NULL ) return;

		//		if( !GAMEIN->GetStreetStallDialog()->SelectedItemCheck() ) return;
		//���� �ɷ��ֳ� ���ֳ��� ����..
		if( pExItem->IsLocked() == TRUE ) return;

		STREETSTALLMGR->ItemStatus( pExItem, FALSE );
	}
}

// LYJ 051017 ���Գ����� �߰�
void SO_DlgFunc(LONG lId, void * p, DWORD we)
{
	GAMEIN->GetStallKindSelectDialog()->OnActionEvnet(lId, p, we);
}

void BS_TitleEditBoxFunc(LONG lId, void * p, DWORD we)
{
	if(we == WE_RETURN)
	{
		STREETSTALLMGR->EditTitle();
	}
}

void BS_DlgFunc(LONG lId, void * p, DWORD we)
{
	GAMEIN->GetStreetBuyStallDialog()->OnActionEvnet(lId, p, we);
}

void BS_SellBtnFunc(LONG lId, void * p, DWORD we)
{
	CStreetBuyStall* pStallBuyDlg = GAMEIN->GetStreetBuyStallDialog();

	if( pStallBuyDlg->GetCurSelectedItemNum() == -1 ) return;

	if( we == WE_BTNCLICK )
	{

		if( !pStallBuyDlg->SelectedItemCheck() ) return;

		STREETSTALLMGR->BuyItem();
	}
}

void BRS_DlgFunc(LONG lId, void * p, DWORD we)
{
	GAMEIN->GetBuyRegDialog()->OnActionEvnet(lId, p, we);
}

void BRS_CloseFunc(LONG lId, void * p, DWORD we)
{
	MSGBASE msg;
	msg.Category = MP_STREETSTALL;
	msg.dwObjectID = HEROID;
	msg.Protocol = MP_STREETSTALL_UPDATEEND_SYN;

	NETWORK->Send( &msg, sizeof(MSGBASE) );
	GAMEIN->GetBuyRegDialog()->Close();
}

void BRS_DeleteFunc(LONG lId, void * p, DWORD we)
{
	CBuyItem* pItem = (CBuyItem*)GAMEIN->GetStreetBuyStallDialog()->GetCurSelectedItem();
	POSTYPE pos = GAMEIN->GetStreetBuyStallDialog()->GetCurSelectedItemNum();

	//���õ� ���� �������� �־��ٸ� �����Ѵ�
	if( pos != -1 && pItem )
		GAMEIN->GetStreetBuyStallDialog()->FakeDeleteItem(pos);

	GAMEIN->GetBuyRegDialog()->Close();
}


void BRS_TypeListFunc(LONG lId, void * p, DWORD we)
{
	GAMEIN->GetBuyRegDialog()->UpdateType();
}

void BRS_ItemListFunc(LONG lId, void * p, DWORD we)
{
	GAMEIN->GetBuyRegDialog()->UpdateItem();
}

void BRS_ClassListFunc(LONG lId, void * p, DWORD we)
{
	GAMEIN->GetBuyRegDialog()->UpdateClass();
}

void BRS_REGBtnFunc(LONG lId, void * p, DWORD we)
{
	ITEM_INFO* pItem = GAMEIN->GetBuyRegDialog()->GetSelectItemInfo();

	if(!pItem)
		return;

	BUY_REG_INFO RegInfo;

	if( !GAMEIN->GetBuyRegDialog()->GetBuyRegInfo(RegInfo) )
		return;

	GAMEIN->GetStreetBuyStallDialog()->FakeRegistItem(RegInfo, pItem);
	GAMEIN->GetBuyRegDialog()->Close();
}

void NSI_HyperLinkFunc(LONG lId, void * p, DWORD we)
{
	cListDialogEx* pListDlg = (cListDialogEx*)((cDialog*)p)->GetWindowForID( lId );
	int nIdx = pListDlg->GetCurSelectedRowIdx();
	if( nIdx == -1 ) return;

	cNpcScriptDialog* pDlg = GAMEIN->GetNpcScriptDialog() ;

	if( pDlg )
	{
		if( we == WE_BTNCLICK )
		{
			pDlg->OnActionEvent( lId, p, we ) ;
		}

		if( lId == NSI_TOPBTN )
		{
			return;
		}
		else if(ITEM* const item = pListDlg->GetItem(pListDlg->GetCurSelectedRowIdx()))
		{
			pDlg->HyperLinkParser(
				item->string,
				item->dwData);
		}

		WINDOWMGR->SetMouseInputProcessed();	
	}
}

void CMI_MoneyOkFunc(LONG lId, void * p, DWORD we)
{
	GAMEIN->GetMoneyDialog()->OkPushed();
}

/*��E��cioA�ˢ�E��EcA��E��cE*/
void DE_DlgFunc(LONG lId, void * p, DWORD we)
{
	if(we == WE_CLOSEWINDOW)
	{
		CDealDialog* pDlg = GAMEIN->GetDealDialog();

		if( pDlg && pDlg->IsShow() )
		{
			pDlg->SetActive(FALSE);
		}
	}

	if(we == WE_LBTNCLICK)
	{
		GAMEIN->GetDealDialog()->OnSelectedItem();
	}
	else if(/*we == WE_LBTNCLICK || we == WE_RBTNCLICK || we == WE_RBTNDBLCLICK ||*/ we == WE_LBTNDBLCLICK)
	{
		GAMEIN->GetDealDialog()->OnActionEvnet(lId, p, we);
	}
}
void DE_BuyItemFunc(LONG lId, void * p, DWORD we)
{
	GAMEIN->GetDealDialog()->OnBuyPushed();
}
void DE_SellItemFunc(LONG lId, void * p, DWORD we)
{
	GAMEIN->GetDealDialog()->OnSellPushed();
}
// 070503 LYW --- GlobalEventFunc : Add function to cancel sell dialog.
void DE_CancelFunc(LONG lId, void* p, DWORD we)
{
	GAMEIN->GetDealDialog()->SetActive(FALSE) ;
}
void DE_CloseHideBtn(LONG lId, void * p, DWORD we)
{
	GAMEIN->GetDealDialog()->HideDealer();
}

//////////////////////////////////////////////////////////////////////////
//*�ˢ碮����E��cA�ˢ碮����E��c�ˢ碮��uA��E��cE*/
void PA_BtnFunc(LONG lId, void * p, DWORD we) 
{
	switch(lId) 
	{		
	case PA_SECEDEBTN: //A��E��ciA����I�ˢ�I 
		{
			if( HEROID != PARTYMGR->GetMasterID() )
				WINDOWMGR->MsgBox( MBI_PARTY_SECEDE, MBT_YESNO, CHATMGR->GetChatMsg(400));	
			else
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1042)) ;
			}
		}	
		break;

	case PA_FORCEDSECEDEBTN: //��E��c���סˢ碮E��c����IA|A��E��ciA����I�ˢ�I
		{
			PARTYMGR->BanPartyMemberSyn(
				GAMEIN->GetPartyDialog()->GetClickedMemberID());
		}
		break;

	case PA_TRANSFERBTN: //�ˢ�E�ˢ�ϡ��ˢ�Ec�ˢ�E�ˢ�Io��E��c����uC AI�ˢ碮��uc
		{
			if(gHeroID != PARTYMGR->GetMasterID())
			{
				break;
			}

			CPartyDialog* const pPartyDlg = GAMEIN->GetPartyDialog();

			if(0 == pPartyDlg)
			{
				break;
			}
			else if(pPartyDlg->GetClickedMemberID() == gHeroID)
			{
				CHATMGR->AddMsg(
					CTC_SYSMSG,
					CHATMGR->GetChatMsg(1300));
				break;
			}
            
			PARTYMGR->ChangeMasterPartySyn(
				gHeroID,
				pPartyDlg->GetClickedMemberID());
		}
		break;

	case PA_BREAKUPBTN: //C�ˢ碮�����ע�I��E��cie
		{
			if( HEROID != PARTYMGR->GetMasterID() )	return;

			if(PARTYMGR->IsProcessing() == TRUE)
				return;
			WINDOWMGR->MsgBox(MBI_PARTYBREAKUP, MBT_YESNO, CHATMGR->GetChatMsg(651));
		}
		break;

	case PA_ADDPARTYMEMBERBTN: //�ˢ�E�ˢ�ϡ�a����Ioo A����I�ˢ�ϡ���E��c���סˢ�ˢ�E��Ec
		{
			CObject* pObject = OBJECTMGR->GetSelectedObject();
			if(!pObject)
			{
				// 090528 ShinJS --- ���õ� ������Ʈ�� ���� ��� ��Ƽ�ʴ� �̸��Է�â�� ����
				GAMEIN->GetInputNameDlg()->SetActive( TRUE );
				return;
			}
			else 
			{
				if( pObject->GetID() == HEROID )
				{
					CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg( 1269 ) );
					return ;
				}

				if( PARTYMGR->IsPartyMember(pObject->GetID()) )
				{
					CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1038)) ;
					return ;
				}

				if(pObject->GetObjectKind() == eObjectKind_Player)
					PARTYMGR->AddPartyMemberSyn(OBJECTMGR->GetSelectedObject()->GetID());
				else
					CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(18));
			}
		}
		break;
	case PA_CREATEOK:
		{
			GAMEIN->GetPartyCreateDialog()->SetActive(
				FALSE);

			PARTY_OPTION partyOption = ePartyOpt_Damage;

			if(cComboBox* pDistirbute = (cComboBox*)WINDOWMGR->GetWindowForIDEx(PA_CREATECOMBOX))
			{
				if(0 == _tcsicmp(RESRCMGR->GetMsg(434), pDistirbute->GetComboText()))
				{
					partyOption = ePartyOpt_Damage;
				}
				else if(0 == _tcsicmp(RESRCMGR->GetMsg(432), pDistirbute->GetComboText()))
				{
					partyOption = ePartyOpt_Sequence;
				}
			}

			BYTE distributionOption = 0;
			
			if(cComboBox* pDiceGrade = (cComboBox*)WINDOWMGR->GetWindowForIDEx(PA_DICEGRADECOMBOX))
			{
				if(0 == _tcsicmp(RESRCMGR->GetMsg(1194), pDiceGrade->GetComboText()))
				{
					distributionOption = 0;
				}
				else if(0 == _tcsicmp(RESRCMGR->GetMsg(1182), pDiceGrade->GetComboText()))
				{
					distributionOption = 1;
				}
				else if(0 == _tcsicmp(RESRCMGR->GetMsg(1183), pDiceGrade->GetComboText()))
				{
					distributionOption = 2;
				}
				else if(0 == _tcsicmp(RESRCMGR->GetMsg(1184), pDiceGrade->GetComboText()))
				{
					distributionOption = 3;
				}
				else if(0 == _tcsicmp(RESRCMGR->GetMsg(1185), pDiceGrade->GetComboText()))
				{
					distributionOption = 4;
				}
			}

			MSG_BYTE2 message;
			ZeroMemory(
				&message,
				sizeof(message));
			message.Category = MP_PARTY;
			message.Protocol = MP_PARTY_CREATE_SYN;
			message.bData1 = partyOption;
			message.bData2 = distributionOption;
			NETWORK->Send(
				&message,
				sizeof(message));

			PARTYMGR->SetIsProcessing(
				TRUE);
		}
		break;
	case PA_CREATECANCEL:
		{
			GAMEIN->GetPartyCreateDialog()->SetActive(FALSE);
		}
		break;
	case PA_INVITEOK:
		{
			GAMEIN->GetPartyInviteDialog()->SetActive(FALSE);
			PARTYMGR->PartyInviteAccept();
		}
		break;
	case PA_INVITECANCEL:
		{
			PARTYMGR->PartyInviteDeny();
			GAMEIN->GetPartyInviteDialog()->SetActive(FALSE);
		}
		break;
	case PA_WARSUGGESTBTN:
		{
			CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(528));
			return;
		}
		break;
	case PA_DISTRIBUTE:
		{
			if(CPartyBtnDlg* pPartySet = (CPartyBtnDlg*)WINDOWMGR->GetWindowForID( PA_BTNDLG ))
			{
				cComboBox* pDistribute = pPartySet->GetDistribute() ;
				if( !pDistribute ) return ;

				if( we==WE_PUSHDOWN && HEROID!=PARTYMGR->GetMasterID())
				{
					pDistribute->ForceListFold();
					CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1116)) ;
					return ;
				}
				else if( we != WE_COMBOBOXSELECT )
				{
					return;
				}

				PARTYMGR->SetPickupItem(pDistribute->GetCurSelectedIdx()) ;
			}
		}
		break ;

	case PA_DICEGRADE:
		{
			if(CPartyBtnDlg* pPartySet = (CPartyBtnDlg*)WINDOWMGR->GetWindowForID( PA_BTNDLG ))
			{
				cComboBox* pDiceGrade = pPartySet->GetDiceGrade() ;
				if( !pDiceGrade ) return ;

				if( we==WE_PUSHDOWN && HEROID!=PARTYMGR->GetMasterID())
				{
					pDiceGrade->ForceListFold();
					CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1116)) ;
					return ;
				}
				else if( we != WE_COMBOBOXSELECT )
				{
					return;
				}

				PARTYMGR->SetDiceGrade(pDiceGrade->GetCurSelectedIdx());
			}
		}
		break ;
	// 080318 LUJ, ǥ�õ� ��Ƽ���� ���� ��ų �������� ����Ѵ�
	case PA_SKILL_ICON_BUTTON:
		{
			if(CPartyDialog* dialog = GAMEIN->GetPartyDialog())
			{
				dialog->OnActionEvent( lId, p, we );
			}

			break;
		}
	}
}

// 080318 LUJ, ��Ƽ ��� â �̺�Ʈ ó��
void PartyMemberFunc( LONG lId, void* p, DWORD we )
{
	if(CPartyDialog* dialog = GAMEIN->GetPartyDialog())
	{
		dialog->OnActionEvent( lId, p, we );
	}
}


// �ˢ�E�ˢ�IUAaA��E��cE ��E��c���סˢ�u��E��c��E�ˡ�A
void QI_QuickDlgFunc(LONG lId, void * p, DWORD we)
{
	GAMEIN->GetQuickDlg()->OnActionEvnet(lId, p, we);
}

void PYO_DlgBtnFunc(LONG lId, void * p, DWORD we)
{
	switch(lId) 
	{
		// 070326 LYW --- GlobalEventFunc : add close button.
	case PYO_STORAGE_CLOSE :
		{
			GAMEIN->GetStorageDialog()->ShowStorageDlg(FALSE) ;
		}
		break ;
	case PYO_STORAGELISTBTN:
		{
			GAMEIN->GetStorageDialog()->SetStorageListInfo();
			GAMEIN->GetStorageDialog()->ShowStorageMode(eStorageMode_StorageListInfo);
			GAMEIN->GetStorageDialog()->AddStorageMode(eStorageMode_StorageListInfo);
			GAMEIN->GetStorageDialog()->ShowStorageDlg(TRUE) ;

			// ���â�� ����
			{
				cDialog* dialog = WINDOWMGR->GetWindowForID( GD_GUILDDLG );
				ASSERT( dialog );

				dialog->SetActive( FALSE );
			}
		}
		break;
	case PYO_BUYSTORAGEBTN:
		{
			if(HERO->GetStorageNum() >= MAX_BASESTORAGELIST_NUM)
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(535) );
				return;
			}
			// 070324 LYW --- GlobalEventFunc : Modified message of messagebox.
			WINDOWMGR->MsgBox( MBI_STORAGEBUY, MBT_YESNO, CHATMGR->GetChatMsg(531), 
				HERO->GetStorageNum()+1, AddComma( GAMEIN->GetStorageDialog()->GetPrice(HERO->GetStorageNum() ) ) );
		}
		break;

	case PYO_STORAGE1BTN: case PYO_STORAGE2BTN: case PYO_STORAGE3BTN:
	case PYO_STORAGE4BTN: case PYO_STORAGE5BTN:
		{
			int num = lId-PYO_STORAGE1BTN;
			CStorageDialog* pDlg = (CStorageDialog*)GAMEIN->GetStorageDialog();
			if( !pDlg )
			{
				ASSERT(0);
				return;
			}

			cPushupButton* pstoragebtn[TAB_STORAGE_NUM];

			if( HERO->GetStorageNum() == 0 )
			{
				for(int i=0; i<TAB_STORAGE_NUM; ++i)
				{
					pstoragebtn[i] = (cPushupButton*)pDlg->GetWindowForID(PYO_STORAGE1BTN+i);
					pstoragebtn[i]->SetPush(FALSE);
				}

				return;
			}
			for(int i=0; i<TAB_STORAGE_NUM; ++i)
			{
				pstoragebtn[i] = (cPushupButton*)pDlg->GetWindowForID(PYO_STORAGE1BTN+i);
				if(i==num)
				{
					pstoragebtn[i]->SetPush(TRUE);
				}
				else
				{
					pstoragebtn[i]->SetPush(FALSE);
				}
			}
			// ����Ioo�ˢ碮����E��c��E��c���סˢ�ˢ�E?�ˢ�E��Ec �ˢ�Iiu�ˢ�EOo �ˢ碮���ˢ�ϡ��ˢ�E�ˢ�ϡ��ˢ�E?�ˢ碮��IAo�ˢ�E�ˢ�IA �ˢ�E�ˢ�ϡ�����I�ˢ�I�ˢ碮��oA�ˢ�E�ˢ�ϡ��ˢ�E�ˢ�ϡ� �ˢ碮���ˢ�ϡ�?E��E��c����ICN�ˢ�E�ˢ�IU.
			GAMEIN->GetStorageDialog()->ShowStorageMode(eStorageMode_StorageWare1+num);
			GAMEIN->GetStorageDialog()->AddStorageMode(eStorageMode_StorageWare1+num);
		}
		break;

	case PYO_PUTINMONEYBTN:
		{
			STORAGEMGR->PutInOutMoney(1);
		}
		break;
	case PYO_PUTOUTMONEYBTN:
		{
			STORAGEMGR->PutInOutMoney(0);
		}
		break;
	case PYO_NOBUYBTN: case PYO_NOBUYSTORAGEBTN:
		{
			GAMEIN->GetStorageDialog()->ShowStorageDlg(FALSE) ;
		}
		break;
	}
}


void Friend_DlgFunc(LONG lId, void * p, DWORD we)
{
	switch(lId) 
	{
	case FRI_PAGE1BTN: case FRI_PAGE2BTN: case FRI_PAGE3BTN: 
		{
			GAMEIN->GetFriendDialog()->SetFriendList(lId - FRI_PAGE1BTN + 1);
		}
		break;
	//100118 ONS ģ������ ���̾�α׿� PC���������� ����߰�
	case FRI_PCROOM_PAGE1BTN: case FRI_PCROOM_PAGE2BTN: case FRI_PCROOM_PAGE3BTN: 
		{
			GAMEIN->GetFriendDialog()->SetPCRoomMemberList(lId - FRI_PCROOM_PAGE1BTN + 1);
		}
		break;

	case FRI_ADDFRIENDBTN:
		{
			if(GAMEIN->GetMiniFriendDialog()->IsActive())
			{
				GAMEIN->GetMiniFriendDialog()->SetActive(FALSE);
			}
			else
			{
				GAMEIN->GetMiniFriendDialog()->SetActive(TRUE);
				char Name[MAX_NAME_LENGTH+1] = {0};
				CObject* pTarget = OBJECTMGR->GetSelectedObject();
				if(pTarget && (pTarget->GetObjectKind() == eObjectKind_Player))
				{
					//strcpy(Name, OBJECTMGR->GetSelectedObject()->GetObjectName());
					SafeStrCpy(Name, OBJECTMGR->GetSelectedObject()->GetObjectName(), MAX_NAME_LENGTH+1);
					GAMEIN->GetMiniFriendDialog()->SetName(Name);
				}
			}
		}
		break;
	case FRI_DELFRIENDBTN:
		{
			GAMEIN->GetFriendDialog()->CheckedDelFriend();
		}
		break;
	case FRI_ADDOKBTN:
		{		
			cEditBox * pName = (cEditBox *)WINDOWMGR->GetWindowForIDEx(FRI_NAMEEDIT);
			char code[MAX_NAME_LENGTH+1] = {0,};			
			sscanf(pName->GetEditText(),"%s",code);
			strupr(code);
			if(strcmp("",code) == 0)
				return;
			char heroname[MAX_NAME_LENGTH+1] = {0,};
			sscanf(HERO->GetObjectName(), "%s", heroname);
			strupr(heroname);
			if(strcmp(heroname, code) == 0)
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1488) );
				return;
			}				
			FRIENDMGR->AddDelFriendSyn(pName->GetEditText(), MP_FRIEND_ADD_SYN);
			GAMEIN->GetMiniFriendDialog()->SetActive(FALSE);
		}
		break;
	case FRI_ADDCANCELBTN:
		{
			GAMEIN->GetMiniFriendDialog()->SetActive(FALSE);
		}
		break;
	case FRI_SENDNOTEBTN:
		{
			CMiniNoteDialogWrite* dialog = GAMEIN->GetMiniNoteDialogWrite();
			ASSERT( dialog );

			dialog->SetActive( TRUE );

			if( ! strlen( GAMEIN->GetFriendDialog()->GetChkedName() ) )
			{
				dialog->SetMiniNote( "", "", 0 );
			}
			else
			{
				dialog->SetMiniNote(GAMEIN->GetFriendDialog()->GetChkedName(), "", 0);
			}
		}
		break;
	case FRI_SENDWHISPERBTN:
		{
			char temp[MAX_NAME_LENGTH+5];
			if(strcmp(GAMEIN->GetFriendDialog()->GetChkedName(), "") == 0)
				return;
			sprintf(temp,"/%s ",GAMEIN->GetFriendDialog()->GetChkedName());
			//061212 LYW --- Change chatting dialog.
			//cEditBox* pChatBox = GAMEIN->GetChattingDlg()->GetInputBox();
			//070102 LYW --- Delete this code.
			
			cEditBox* pChatBox = GAMEIN->GetChattingDlg()->GetInputBox();
			pChatBox->SetFocusEdit(TRUE);
			pChatBox->SetEditText(temp);


		}
		break;
	//090623 pdy �Ͽ�¡ ����߰� (ģ����� ��ũ �Ͽ콺����) 
	case FRI_FRIENDLIHOUSE1: 
	case FRI_FRIENDLIHOUSE2: 
	case FRI_FRIENDLIHOUSE3:
	case FRI_FRIENDLIHOUSE4:
	case FRI_FRIENDLIHOUSE5:
	case FRI_FRIENDLIHOUSE6:
	case FRI_FRIENDLIHOUSE7:
	case FRI_FRIENDLIHOUSE8:
	case FRI_FRIENDLIHOUSE9:
	case FRI_FRIENDLIHOUSE10:
		{
			if( HERO->GetState() != eObjectState_None || HERO->IsPKMode() )
			{
				WINDOWMGR->MsgBox( MBI_CANTUSE_MAPMOVESCROLL, MBT_OK, CHATMGR->GetChatMsg(789) ) ;
				return ;
			}

			DWORD BtnNum = lId - FRI_FRIENDLIHOUSE1;

			CFriendDialog* Dlg = GAMEIN->GetFriendDialog() ;
		
			cListCtrl* pFriendListLCtl = (cListCtrl*)((cTabDialog*)Dlg->GetTabSheet(0))->GetWindowForID(FRI_FRIENDLISTLCTL);
			cRITEMEx * rItem = (cRITEMEx *)pFriendListLCtl->GetRItem( BtnNum );

			if( ! rItem )
				break;

			// 091019 pdy �Ͽ�¡�ʿ��� �ε��߿� ���� �Ͽ�¡ ��� ������ �߰�
			if( HOUSINGMGR->IsHousingMap() &&	
				HOUSINGMGR->IsHouseInfoLoadOk() == FALSE )
			{
				return;
			}

			//090625 pdy �Ͽ�¡ �˾�â [ģ����� ��ũ �Ͽ콺����]
			char MsgBuf[128] = {0,};
			sprintf(MsgBuf,CHATMGR->GetChatMsg(1890), rItem->pString[0]);
			cMsgBox* pBox = WINDOWMGR->MsgBox( MBI_HOUSE_VISIT_LINK_FRIENDHOME, MBT_YESNO, MsgBuf ) ;	//1890	"%s�� ������ �̵� �Ͻðڽ��ϱ�?"

			//ģ���� �̸��� �ּҰ��� Param���� ����  
			if( pBox )
				pBox->SetParam( (DWORD)rItem->pString[0] );

			OBJECTSTATEMGR->StartObjectState(HERO, eObjectState_Deal) ;
		}
		break;
	}
}

void Note_DlgBtnFunc(LONG lId, void * p, DWORD we)
{
	switch(lId) 
	{
	case NOTE_WRITENOTEBTN:
		{
			if(GAMEIN->GetMiniNoteDialogWrite()->IsActive())
			{
				GAMEIN->GetMiniNoteDialogWrite()->SetActive(FALSE);
			}
			else
			{
				GAMEIN->GetMiniNoteDialogWrite()->SetActive(TRUE);
			}
		}
		break;
	case NOTE_DELNOTEBTN:
		{
			GAMEIN->GetNoteDialog()->CheckedNoteID();
		}
		break;
	case NOTE_SAVENOTEBTN:
		{
			GAMEIN->GetNoteDialog()->SaveNoteID();
		}
		break;

	case NOTE_PAGE1BTN:
	case NOTE_PAGE2BTN:
	case NOTE_PAGE3BTN:
	case NOTE_PAGE4BTN:
	case NOTE_PAGE5BTN:
	case NOTE_PAGE6BTN:
	case NOTE_PAGE7BTN:
	case NOTE_PAGE8BTN:
		{
			NOTEMGR->NoteListSyn( lId - NOTE_PAGE1BTN +1, GAMEIN->GetNoteDialog()->GetMode() );
		}
		break;
		//////////////////////////////////////////////////////////////////////////
		//////MINI NOTE
	case NOTE_MSENDOKBTN:
		{
			cEditBox * pReceiver = (cEditBox *)WINDOWMGR->GetWindowForIDEx(NOTE_MRECEIVEREDIT);
			char pname[MAX_NAME_LENGTH+1] = {0};
			SafeStrCpy(pname, pReceiver->GetEditText(), MAX_NAME_LENGTH+1);
			if(strlen(pname) == 0)
				return;

			cEditBox * pTitle = (cEditBox *)WINDOWMGR->GetWindowForIDEx(NOTE_MWTITLE);
			char ptitle[MAX_NOTE_TITLE+1] = {0,};
			SafeStrCpy(ptitle, pTitle->GetEditText(), MAX_NOTE_TITLE+1);
			int len = strlen(ptitle);
			if(len <= 2)
				return;

			if(len >= MAX_NOTE_TITLE)
			{
				len = MAX_NOTE_TITLE - 1;
				ptitle[len] = 0;
			}

			cTextArea * pNote = (cTextArea *)WINDOWMGR->GetWindowForIDEx(NOTE_MWNOTETEXTREA);
			char FullText[512];
			pNote->GetScriptText(FullText);

			char Note[MAX_NOTE_LENGTH];
			SafeStrCpy(Note, FullText, MAX_NOTE_LENGTH - 1);
			Note[MAX_NOTE_LENGTH - 1] = 0;
			NOTEMGR->WriteNote(Note, ptitle, pname);
			GAMEIN->GetMiniNoteDialogWrite()->SetActive(FALSE);			
		}
		break;
	case NOTE_MSENDCANCELBTN:
		{
			GAMEIN->GetMiniNoteDialogWrite()->SetActive(FALSE);
		}
		break;
	case NOTE_MREPLYBTN:
		{
			char ToName[MAX_NAME_LENGTH+1];

			if( 0 == strcmp(CHATMGR->GetChatMsg(2236), GAMEIN->GetMiniNoteDialogRead()->GetSenderName()) )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(2237) );
				return;
			}

			strcpy(ToName, GAMEIN->GetMiniNoteDialogRead()->GetSenderName());
			if(strlen(ToName) == 0 )
				return;

			GAMEIN->GetMiniNoteDialogWrite()->SetMiniNote(ToName, "", 0);
			GAMEIN->GetMiniNoteDialogWrite()->SetActive(TRUE);
			GAMEIN->GetMiniNoteDialogRead()->SetActive(FALSE);

		}
		break;
	case NOTE_MGETPACKAGEBTN:
		{
			if( GAMEIN->GetMiniNoteDialogRead()->IsActive() )
				GAMEIN->GetMiniNoteDialogRead()->SendGetPackageMsg();
		}
		break;
	case NOTE_MDELETEBTN:
		{
			NOTEMGR->DelNoteSyn(GAMEIN->GetMiniNoteDialogRead()->GetNoteID(), TRUE);
			GAMEIN->GetMiniNoteDialogRead()->SetActive(FALSE);
		}
		break;
	case NOTE_TABBTN1:
		{
			GAMEIN->GetNoteDialog()->SetMode( eMode_NormalNote );
			NOTEMGR->NoteListSyn( 1, eMode_NormalNote );
		}
		break;
	case NOTE_TABBTN2:
		{
			GAMEIN->GetNoteDialog()->SetMode( eMode_StoredNote );
			NOTEMGR->NoteListSyn( 1, eMode_StoredNote );
		}
		break;
	case NOTE_NOTELISTCHK12:
		{
			GAMEIN->GetNoteDialog()->SetChkAll();
		}
		break;
	}
}

// 080313 NYJ --- ��ü��ġ�� �߰�
void Shout_DlgBtnFunc(LONG lId, void * p, DWORD we)
{
	if(cDialog* const dialog = WINDOWMGR->GetWindowForID(SHOUT_SHOUTDLG))
	{
		dialog->OnActionEvent(
			lId,
			p,
			we);
	}
}

//080410 NYJ --- ���ýý���
void Fishing_DlgBtnFunc(LONG lId, void * p, DWORD we)
{
	switch(lId)
	{
	case FISHING_BTN_CLOSE:
		{
			GAMEIN->GetFishingDlg()->SetActive(FALSE);
		}
		break;
	case FISHING_BTN_START:
		{
			// ������ư ���´� ��������� �ݴ�.
			BOOL bLastPushed = !GAMEIN->GetFishingDlg()->IsPushedStartBtn();
			
			if(bLastPushed)
				GAMEIN->GetFishingDlg()->Fishing_Cancel();
			else
				GAMEIN->GetFishingDlg()->Fishing_Start();
				

			GAMEIN->GetFishingDlg()->SetLockStartBtn(TRUE);
		}
		break;

	case FISHING_BTN_CLOSEPOINT:
	case FISHING_BTN_CLOSEPOINTX:
		{
			// ����Ʈ â�ݱ� X��ư
			GAMEIN->GetFishingPointDlg()->SetActive(FALSE);
		}
		break;

	case FISHING_BTN_CHANGEPOINT:
		{
			// ����Ʈ��ȯ
			GAMEIN->GetFishingPointDlg()->SendFishPoint();
		}
		break;

	case FISHING_BTN_CLEARSLOT:
		{
			// �����ʱ�ȭ
			GAMEIN->GetFishingPointDlg()->ClearFishIcon();
		}
		break;
	}
}

void CR_DlgBtnFunc(LONG lId, void * p, DWORD we)
{
	if( EXITMGR->IsExitWaiting() )	//�����߿� ��Ȱ �ȵ�
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(643) );
		GAMEIN->GetReviveDialog()->SetActive( FALSE );
		return;
	}

	LEVELTYPE heroLevel = HERO->GetLevel() ;

	if( heroLevel >= 10 )
	{
		switch(lId) 
		{
		case CR_PRESENTSPOT:
			{
				if( SIEGEDUNGEONMGR->IsSiegeDungeon(MAP->GetMapNum()) )
  				{
  					WINDOWMGR->MsgBox(MBI_REVIVE_PRESENTSPOT, MBT_YESNO, CHATMGR->GetChatMsg( 1687 ) );//"1~5%�� ����ġ�� �϶��մϴ�, ���ڸ����� ��Ȱ �Ͻðڽ��ϱ�?") ;

  				}
  				else
  				{
  					WINDOWMGR->MsgBox(MBI_REVIVE_PRESENTSPOT, MBT_YESNO, CHATMGR->GetChatMsg(850)) ;
  				}
			}
			break;

		case CR_TOWNSPOT:
			{
				if( SIEGEDUNGEONMGR->IsSiegeDungeon(MAP->GetMapNum()) )
  				{
  					WINDOWMGR->MsgBox(MBI_REVIVE_TOWNSOPT, MBT_YESNO, CHATMGR->GetChatMsg( 1688 )) ;

  				}
  				else
  				{
  					WINDOWMGR->MsgBox(MBI_REVIVE_TOWNSOPT, MBT_YESNO, CHATMGR->GetChatMsg(851)) ;
  				}
			}
			break;
		}
	}
	else
	{
		switch(lId)
		{
		case CR_PRESENTSPOT :
			{
				MSGBASE msg;
				msg.Category = MP_CHAR_REVIVE;
				msg.Protocol = MP_CHAR_REVIVE_PRESENTSPOT_SYN;
				msg.dwObjectID = HEROID;
				NETWORK->Send(&msg, sizeof(msg));
			}
			break ;

		case CR_TOWNSPOT :
			{
				MSGBASE msg;
				msg.Category = MP_CHAR_REVIVE;
				msg.Protocol = MP_CHAR_REVIVE_VILLAGESPOT_SYN;
				msg.dwObjectID = HEROID;
				NETWORK->Send(&msg, sizeof(msg));
			}
			break ;
		}
	}

	GAMEIN->GetReviveDialog()->SetDisable(TRUE);
}

void OTI_DlgFunc(LONG lId, void * p, DWORD we)
{
	GAMEIN->GetOptionDialog()->OnActionEvent(lId, p, we);
}

void EXT_DlgFunc(LONG lId, void * p, DWORD we)
{
	//A��E��cAA��E��cE��E��c����u�ˢ�E�ˢ�ϡ�Co
	if( we == WE_BTNCLICK )
	{
		switch( lId )
		{
		case EXT_BTN_EXITGAME:
			{
				GAMEIN->GetExitDialog()->SetActive( FALSE ) ;
				WINDOWMGR->MsgBox( MBI_EXIT, MBT_YESNO, RESRCMGR->GetMsg( 263 ) ) ;
			}
			break;
			
		case EXT_BTN_SELECTCHAR:
			{
				WINDOWMGR->MsgBox( MBI_MOVE_TO_CHARSELECT, MBT_YESNO, CHATMGR->GetChatMsg( 1329 ) );
			}
			break;

		case EXT_BTN_OPTION:
			{
				COptionDialog* pDlg = GAMEIN->GetOptionDialog() ;					

				if( pDlg )															
				{
					pDlg->SetActive( TRUE ) ;										
				}
			}
			break;

		case EXT_BTN_EMERGENCY:
			{
				if(MAP->GetMapNum() == GTMAPNUM)
				{
					WINDOWMGR->MsgBox( MBI_GTEMERGENCY_FAIL, MBT_OK, CHATMGR->GetChatMsg( 1656 ) );
					return;
				}

				MSGBASE msg;
				msg.Category	= MP_CHAR;
				msg.Protocol	= MP_CHAR_EMERGENCY_SYN;
				msg.dwObjectID	= HEROID;	
				NETWORK->Send( &msg, sizeof(msg) );
				EXITMGR->SendExitMsg( eEK_CHARSEL );
			}
			break;
		}
	}
}

void XCI_DlgFunc( LONG lId, void* p, DWORD we )
{
	GAMEIN->GetExchangeDialog()->OnActionEvent(lId, p, we);
}

void PLI_DlgFunc( LONG lId, void* p, DWORD we )
{
	if( p )
	{
		if( ((cDialog*)p)->GetType() == WT_PKLOOTINGDLG )
		{
			((CPKLootingDialog*)p)->OnActionEvent( lId, p, we );
		}
	}
}

void CNA_BtnOkFunc( LONG lId, void* p, DWORD we )
{
	if( p )
	{
		((cDialog*)p)->SetActive( FALSE );
	}
	TITLE->SetServerList();
}

void DIVIDEBOX_Func(LONG lId, void * p, DWORD we)
{
	if( we == WE_RETURN )
	{
		((cDivideBox*)p)->ExcuteDBFunc( WE_RETURN );		
	}
}

void MONEYDIVIDEBOX_Func(LONG lId, void * p, DWORD we)
{
	if( we == WE_RETURN )
	{
		((cMoneyDivideBox*)p)->ExcuteDBFunc( WE_RETURN );		
	}
}

extern WORD mapnumforMSGBOX;
void MessageBox_Func( LONG lId, void * p, DWORD we )
{
	switch( lId )
	{
	case MBI_EXIT:
		{
			if( we == MBI_YES )
			{
				if( MAINGAME->GetCurStateNum() != eGAMESTATE_GAMEIN )
				{
					//					MAINGAME->SetGameState( eGAMESTATE_END );
					PostMessage( _g_hWnd, WM_CLOSE, NULL, NULL );
					break;
				}

				int nErrorCode;
				if( ( nErrorCode = EXITMGR->CanExit() ) == eEXITCODE_OK )
				{
					EXITMGR->SendExitMsg( eEK_GAMEEXIT );
				}
				else
				{
					switch( nErrorCode )
					{
					case eEXITCODE_PKMODE:
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1261) );
						break;
					case eEXITCODE_LOOTING:
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1262) );
						break;
					default:
						// 070622 LYW --- GlobalEventFunc : �޽��� ��ȣ ����.
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(219) );
						break;
					}
				}
			}
		}
		break;

	// 080121 LYW --- GlobalEventFunc : ĳ���� ���� ȭ������ �̵�Ȯ�� �޽��� ó��.
	case MBI_MOVE_TO_CHARSELECT :
		{
			if( we == MBI_YES )
			{
				GAMEIN->GetExitDialog()->SetActive( FALSE );
				int nErrorCode;
				if( ( nErrorCode = EXITMGR->CanExit() ) == eEXITCODE_OK )
				{
					EXITMGR->SendExitMsg( eEK_CHARSEL );
				}
				else
				{
					switch( nErrorCode )
					{
					case eEXITCODE_PKMODE:
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1261) );
						break;
					case eEXITCODE_LOOTING:
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1262) );
						break;
					default:
						// 070622 LYW --- GlobalEventFunc : �޽��� ��ȣ ����.
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(219) );
						break;
					}
				}
			}
		}
		break ;

	case MBI_PASSERROR:
		{                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 
			cEditBox* pEditId = (cEditBox*)WINDOWMGR->GetWindowForIDEx( MT_IDEDITBOX );
			cEditBox* pEditPwd = (cEditBox*)WINDOWMGR->GetWindowForIDEx( MT_PWDEDITBOX );
			if( pEditId )
				pEditId->SetFocusEdit( FALSE );
			if( pEditPwd )
			{
				pEditPwd->SetEditText( "" );
				pEditPwd->SetFocusEdit( TRUE );
			}
			cDialog* pDlg1 = (cDialog*)WINDOWMGR->GetWindowForID( MT_LOGINDLG );
			pDlg1->SetDisable( FALSE );

			cDialog* pDlg2 = (cDialog*)WINDOWMGR->GetWindowForID( MT_DYNAMICDLG );

			if( pDlg2 )
				if( pDlg2->IsActive() )				
				{
					pDlg1->SetActive( TRUE );
					pDlg2->SetDisable( FALSE );
					pDlg2->SetActive( FALSE	);
					pEditPwd = (cEditBox*)WINDOWMGR->GetWindowForIDEx( MT_DYNAMICPWDEDITBOX );
					pEditPwd->SetEditText( "" );
					//				pEditPwd->SetFocusEdit( TRUE );
				}
		}
		break;
	case MBI_DISCONNECTED_OVERLAPPEDLOGIN:
		{
			PostMessage( _g_hWnd, WM_CLOSE, NULL, NULL );
		}
		break;
	case MBI_PARTYBREAKUP:
		{
			if(we == MBI_YES)
			{
				PARTYMGR->BreakUpSyn();
			}
		}
		break;
	case MBI_STORAGEBUY:
		{
			if(we == MBI_YES)
			{
				STORAGEMGR->BuyStorageSyn();
			}
		}
		break;
	case MBI_SKILLDELETE:
		{
			if( we == MBI_YES )
			{
				AUDIOMGR->Play(
					67,
					gHeroID);
			}
		}
		break;
	case MBI_MAKESTREETSTALL:
		{
			if(we == MBI_YES)
			{
				STREETSTALLMGR->OpenStreetStall();
			}
			else if( we == MBI_NO )
			{
				STREETSTALLMGR->SetOpenMsgBox(TRUE);
			}
		}
		break;

	case MBI_DELETECHAR:
		{
			if( we == MBI_YES )
			{
				CHARSELECT->GetCertificateDlg()->MakeRandomNumber() ;
				CHARSELECT->GetCertificateDlg()->SetActive(TRUE) ;
			}
			else
			{
				CHARSELECT->SetDisablePick( FALSE );
			}
		}
		break;
	case MBI_CANCEL :
		{
			CHARSELECT->SetDisablePick( FALSE );
		}
		break ;

	case MBI_EXCHANGEAPPLY:
		{
			if(we == MBI_YES)
			{
				EXCHANGEMGR->AcceptExchange( TRUE );
			}
			else if( we == MBI_NO )
			{
				EXCHANGEMGR->AcceptExchange( FALSE );
			}
		}
		break;

	case MBI_EXCHANGEWAITING:
		{
			if(we == MBI_CANCEL)
			{
				EXCHANGEMGR->CancelApply();				
			}
		}
		break;

	case MBI_SELECTLOGINPOINT:
		{
			if(we == MBI_YES)
			{
				cMsgBox* pMsgBox = (cMsgBox*)WINDOWMGR->GetWindowForID( MBI_SELECTLOGINPOINT );				
				CNpc* pNpc = (CNpc*)OBJECTMGR->GetObject( pMsgBox->GetParam() );
				if( pNpc )
				{
					g_UserInput.GetHeroMove()->SelectLoginPoint( pNpc->GetNpcUniqueIdx() );
				}
				else
				{
					//error ��E��c��E�ˡ�I��E��c����u��E��c��E?AI�ˢ碮����E��c��E��cAAI�ˢ碮����E��c�ˢ�Ec�ˢ�E�ˢ�ϡ�| AuAaCO �ˢ碮��uo �ˢ碮��u����I���ע�I�ˢ碮��oA�ˢ�E�ˢ�II�ˢ�E�ˢ�IU.
				}

				if( HERO->GetState() == eObjectState_Deal )
					OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal);
			}
			else if( we == MBI_NO )
			{
				if( HERO->GetState() == eObjectState_Deal )
					OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal);
			}
		}
		break;

	case MBI_LOGINEDITCHECK:
		{

			if( we == MBI_OK )
			{
				cEditBox * pIdEdit =(cEditBox *) WINDOWMGR->GetWindowForIDEx(MT_IDEDITBOX); // pjs ID�ˢ�E?�ˢ�E��Ec �ˢ碮����E��c��E��cAA�ˢ�E?�ˢ碮��o�ˢ碮���ˢ�ϡ� [5/20/2003]
				cEditBox * pPwEdit =(cEditBox *) WINDOWMGR->GetWindowForIDEx(MT_PWDEDITBOX); // pjs ID�ˢ�E?�ˢ�E��Ec �ˢ碮����E��c��E��cAA�ˢ�E?�ˢ碮��o�ˢ碮���ˢ�ϡ� [5/20/2003]

				if( *pIdEdit->GetEditText() == 0 )
					pIdEdit->SetFocusEdit(TRUE);
				else
					pPwEdit->SetFocusEdit(TRUE);
			}
			cDialog* pDlg= (cDialog*)WINDOWMGR->GetWindowForID( MT_LOGINDLG );
			pDlg->SetDisable( FALSE );
		}
		break;
	case MBI_DELETEICONCHECK:
		{
			switch( we )
			{
			case MBI_YES:
				{
					ITEMMGR->SendDeleteItem();
					AUDIOMGR->Play(
						69,
						gHeroID);
					break;
				}
			case MBI_NO:
				{
					GAMEIN->GetInventoryDialog()->SetDisable( FALSE );
					GAMEIN->GetStorageDialog()->SetDisable( FALSE );

					// ���â�� ��� �����ϰ� �Ѵ�.
					{
						cDialog* dialog = WINDOWMGR->GetWindowForID( GD_GUILDDLG );
						ASSERT( dialog );

						dialog->SetDisable( FALSE );
					}

					break;
				}
			}
		}
		break;
	case MBI_DELETEPETICONCHECK:
		{
			switch( we )
			{
			case MBI_YES :
				{
					GAMEIN->GetPetWearedDlg()->SendDelete() ;
					AUDIOMGR->Play(
						69,
						gHeroID);
				}
				break ;

			default : break ;
			}

			GAMEIN->GetPetWearedDlg()->SetDisable( FALSE ) ;
		}
		break;

	case MBI_FRIEND_ADD:
		{
			if(we == MBI_YES)
			{
				FRIENDMGR->FriendInviteAccept();

			}
			else if( we == MBI_NO )
			{
				FRIENDMGR->FriendInviteDeny();
			}
		}
		break;
	case MBI_SHOWDOWNAPPLY:
		{
			if(we == MBI_YES)
			{
				SHOWDOWNMGR->AcceptShowdown( TRUE );
			}
			else if( we == MBI_NO )
			{
				SHOWDOWNMGR->AcceptShowdown( FALSE );
			}
		}
		break;
	case MBI_SHOWDOWNWAITING:
		{
			if(we == MBI_CANCEL)
			{
				SHOWDOWNMGR->CancelApply();
			}
		}
		break;
	case MBI_CHARSELERROR:
		{
			CHARSELECT->SetDisablePick( FALSE );
		}
		break;
	case MBI_CHARMAKEERROR:
		{
			CHARMAKE->SetDisableDlg( FALSE );

			// 070619 LYW --- GlobalEventFunc : Setting To Disable Gender button.
			// ������ ������ �� ������ �ϱ� ���� Disable�� ������ �ش�.
			CHARMAKE->GetCharMakeNewDlg()->GetPushUpButton(e_PB_MAN)->SetDisable(TRUE) ;
			CHARMAKE->GetCharMakeNewDlg()->GetPushUpButton(e_PB_WOMAN)->SetDisable(TRUE) ;
		}
		break;
	case MBI_PARTY_SECEDE:
		{
			if(we == MBI_YES)
			{
				if( GAMEIN->GetGuageDlg()->GetHeroState() == e_CAPTAIN )
				{
					GAMEIN->GetGuageDlg()->SecessionPartyCaptain() ;
				}

				PARTYMGR->DelPartyMemberSyn();
			}
		}
		break;
	case MBI_OVERLAPPEDLOGININOTHERSERVER:
		{
			cDialog* dlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
			if( dlg )
			{
				dlg->SetDisable( FALSE );
				cEditBox* editboxId = (cEditBox *)dlg->GetWindowForID(MT_IDEDITBOX);
				cEditBox* editboxPwd = (cEditBox *)dlg->GetWindowForID(MT_PWDEDITBOX);
				if( editboxId )
				{
					editboxId->SetEditText("");
					editboxId->SetFocusEdit(TRUE);
				}
				if( editboxPwd )
				{
					editboxPwd->SetEditText("");
					editboxPwd->SetFocusEdit(FALSE);
				}
			}
		}
		break;
	case MBI_OVERLAPPEDLOGIN:
		{			
			if(we == MBI_YES)
			{
				MSGBASE msg;
				msg.Category = MP_USERCONN;
				msg.Protocol = MP_USERCONN_FORCE_DISCONNECT_OVERLAPLOGIN;
				NETWORK->Send(&msg,sizeof(msg));

				cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_LOGINERRORCLOSE, MBT_OK, CHATMGR->GetChatMsg(4) );
				cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
				if( pIDDlg && pMsgBox )
				{
					pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );
				}
			}
			else if( we == MBI_NO )
			{
				cDialog* dlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
				if( dlg )
				{
					dlg->SetDisable( FALSE );
					cEditBox* editboxId = (cEditBox *)dlg->GetWindowForID(MT_IDEDITBOX);
					cEditBox* editboxPwd = (cEditBox *)dlg->GetWindowForID(MT_PWDEDITBOX);
					if( editboxId )
					{
						editboxId->SetEditText("");
						editboxId->SetFocusEdit(TRUE);
					}
					if( editboxPwd )
					{
						editboxPwd->SetEditText("");
						editboxPwd->SetFocusEdit(FALSE);
					}
				}				
			}
		}
		break;
	case MBI_SEND_DISCONNECT_OVERLAPPEDLOGIN:
		{

		}
		break;
	case MBI_LOGINERRORCLOSE:
		{
			//			MAINGAME->SetGameState(eGAMESTATE_END);
			cDialog* dlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
			if( dlg )
			{
				dlg->SetDisable( FALSE );
				cEditBox* editboxId = (cEditBox *)dlg->GetWindowForID(MT_IDEDITBOX);
				if( editboxId )
				{
					editboxId->SetEditText("");
					editboxId->SetFocusEdit(TRUE);
				}
				cEditBox* editboxPwd = (cEditBox *)dlg->GetWindowForID(MT_PWDEDITBOX);
				if( editboxPwd )
				{
					editboxPwd->SetEditText("");
					editboxPwd->SetFocusEdit(FALSE);
				}
			}

			TITLE->NoDisconMsg();		//
			TITLE->ShowServerList();
		}
		break;
	case MBI_CANTUSEDYNAMIC:
		{
			cDialog* pDlg = WINDOWMGR->GetWindowForID( MT_DYNAMICDLG );
			if( pDlg )
			{
				pDlg->SetDisable( FALSE );
				((cEditBox*)pDlg->GetWindowForID( MT_DYNAMICPWDEDITBOX ))->SetEditText( "" );
				((cEditBox*)pDlg->GetWindowForID( MT_DYNAMICPWDEDITBOX ))->SetFocusEdit( TRUE );
			}			
		}
		break;
	case MBI_SERVERDISCONNECT:
		{
			//			MAINGAME->SetGameState(eGAMESTATE_END);
			PostMessage( _g_hWnd, WM_CLOSE, NULL, NULL );
		}
		break;
	case MBI_DISTCONNECT_ERROR:
		{
			WINDOWMGR->CloseAllMsgBoxNoFunc();
			TITLE->ShowServerList();
		}
		break;
	case MBI_MAPSERVER_CLOSE:
		{
			CHARSELECT->SetDisablePick( FALSE );
		}
		break;
	case MBI_SELLITEM:
		{
			if( we == MBI_YES )
			{
				GAMEIN->GetDealDialog()->SendSellItemMsg();
			}
			else
			{
				CDealDialog::OnCancelSellItem(0,NULL,0,GAMEIN->GetDealDialog(),NULL);
				//GAMEIN->GetDealDialog()->CancelSellItem();
			}
		}
		break;
	case MBI_BUYITEM:
		{
			if( we == MBI_YES )
			{
				GAMEIN->GetDealDialog()->SendBuyItemMsg();
			}
			else
			{
				GAMEIN->GetDealDialog()->CancelBuyItem();
			}
		}
		break;
	case MBI_PKMODE:
		{
			if( we == MBI_YES )
			{
				PKMGR->SendMsgPKOn();
			}
			else
			{

			}
		}
		break;
	case MBI_NOT_GM:
		{
			//			MAINGAME->SetGameState( eGAMESTATE_END );
			PostMessage( _g_hWnd, WM_CLOSE, NULL, NULL );
		}
		break;

	case MBI_WAIT_LOGINCHECK:
		{
			//�ˢ�E?�ˢ碮��I��E��c����ua�ˢ碮��u��E��c����I �ˢ�E�ˢ�ϡ��ˢ碮���ˢ硧I�ˢ碮��u�ˢ碮��uAo�ˢ�E�ˢ�ϡ�| �ˢ碮���ˢ�ϡ��ˢ�E�ˢ�ϡ�����I��I���̢�E��ci��E��c���סˢ�i ����Ii��Io�ˢ�Ei����o�ˢ碮��u�ˢ碮����E��c�ˢ碮��u��E��c����I A����I��I���̡ˢ�E�ˢ�ϡ��ˢ�Ec? ��E��c��E�ˡ͢�E��cEAI�ˢ�EO��E��c�ˢ��?�ˢ�E�ˢ�IA AI��E��c���סˢ�I�ˢ�Ii�ˢ�Ii A����I��I���̡ˢ�E�ˢ�ϡ��ˢ�Ec �ˢ碮��uE�ˢ�IiE��E��c���סˢ�I �ˢ碮��u�ˢ碮����E��c�ˢ�E�ˢ�IN��E��c���סˢ�ˢ�E��Ec?

			cDialog* dlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
			if( dlg )
			{
				cEditBox* editboxId = (cEditBox *)dlg->GetWindowForID(MT_IDEDITBOX);
				cEditBox* editboxPwd = (cEditBox *)dlg->GetWindowForID(MT_PWDEDITBOX);
				if( editboxId )
				{
					editboxId->SetEditText("");
					editboxId->SetFocusEdit(TRUE);
				}
				if( editboxPwd )
				{
					editboxPwd->SetEditText("");
					editboxPwd->SetFocusEdit(FALSE);
				}
			}

			TITLE->NoDisconMsg();		//�ˢ碮��u�ˢ碮����E��c��E��c��E�ˡ͡ˢ�E��Ec showserverlist�ˢ�E?�ˢ�E��Ec�ˢ碮��u��E��c����I A��E��cE�ˢ碮��uO ����I�ˢ�"��cAA��E��ci�ˢ�EO��E��c�ˢ��? ����I�ˢ�Ao��E��c����ue �ˢ�E�ˢ�ϡ��ˢ碮���ˢ硧I�ˢ碮��u�ˢ碮��uAo �ˢ碮��u����I���ע�I�ˢ�Ii�ˢ�Ii��E��c��E�ˡ�I.
			TITLE->ShowServerList();
			TITLE->StartWaitConnectToAgent( FALSE );
		}
		break;
	case MBI_TIMEOVER_TOCONNECT:
		{
			TITLE->GetServerListDialog()->SetActive( TRUE );
		}
		break;


	case MBI_STREETBUYMSG:
		{
			if( we == MBI_YES )
			{
				STREETSTALLMGR->SendBuyItemMsg();
			}

			// 081202 LUJ, ����� �����Ѵ�
			if(CStreetStall* stallDialog = GAMEIN->GetStreetStallDialog())
			{
				stallDialog->SetDisable( FALSE );
			}
		}
		break;

	case MBI_REGEDITMSG:
		{
			if( we == MBI_YES )
			{
				if(!GAMEIN->GetStreetBuyStallDialog()->IsActive())
					return;

				POSTYPE pos = GAMEIN->GetStreetBuyStallDialog()->GetCurSelectedItemNum();
				BUY_REG_INFO RegInfo = GAMEIN->GetStreetBuyStallDialog()->GetBuyRegInfo(pos);
				GAMEIN->GetBuyRegDialog()->SetRegInfo(RegInfo);
				GAMEIN->GetBuyRegDialog()->Show();	

			}
			else
			{
				MSGBASE msg;
				msg.Category = MP_STREETSTALL;
				msg.dwObjectID = HEROID;
				msg.Protocol = MP_STREETSTALL_UPDATEEND_SYN;

				NETWORK->Send( &msg, sizeof(MSGBASE) );
			}
		}
		break;
	case MBI_STREETSELLMSG:
		{
			CStreetBuyStall* pStall = GAMEIN->GetStreetBuyStallDialog();

			if( we == MBI_YES )
				if(pStall && pStall->IsActive() && STREETSTALLMGR->SendSellItemMsg())
					break;
				else
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1218) );

			if(pStall) GAMEIN->GetStreetBuyStallDialog()->SetData(NULL);

			CItem* pItem = pStall->GetSellItem();

			if(pItem)	pItem->SetLock(FALSE);

			pStall->DelSellItem();
			// 081202 LUJ, ����� �����Ѵ�
			pStall->SetDisable( FALSE );
		}
		break;

	case MBI_QUESTGIVEUP:
		{
			if( we == MBI_YES )
				QUESTMGR->QuestAbandon();
		}
		break;
	case MBI_QUESTSTART:
		{
			if( we == MBI_YES )
				GAMEIN->GetInventoryDialog()->ItemUseForQuestStart();

		}
		break;
	case MBI_GUILD_SECEDE:
		{
			if( we == MBI_YES )
			{
				MSGBASE msg;
				msg.Category = MP_GUILD;
				msg.Protocol = MP_GUILD_SECEDE_SYN;
				msg.dwObjectID = HEROID;
				NETWORK->Send(&msg, sizeof(msg));
				GAMEIN->GetGuildDlg()->SetActive(FALSE);
			}
		}
		break;
	case MBI_GUILD_BREAKUP:
		{
			if( we == MBI_YES )
			{
				if( GUILDWARMGR->IsValid( 3 ) )
				{
					//GUILDMGR->BreakUpGuildSyn();

					if(HERO->GetGuildIdx() == 0)
					{
						char buf[128];
						sprintf(buf, "HOW CAN U SHOW DLG? chridx : %u", HEROID);
						ASSERTMSG(0, buf);
						return;
					}
					if(HERO->GetGuildMemberRank() != GUILD_MASTER)
					{
						char buf[128];
						sprintf(buf, "HOW CAN U BREAKUP? chridx : %u, rank : %d", HEROID, HERO->GetGuildMemberRank());
						ASSERTMSG(0, buf);
						return;
					}
					if( HERO->GetGuildUnionIdx() != 0 )
					{
						CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(591));
						return;
					}
				
					MSGBASE msg;
					msg.Category = MP_GUILD;
					msg.Protocol = MP_GUILD_BREAKUP_SYN;
					msg.dwObjectID = HEROID;
				
					NETWORK->Send(&msg, sizeof(msg));	
				}
			}
			OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal);
		}
		break;
	case MBI_GUILD_BAN:
		{
			CGuildDialog* dialog = GAMEIN->GetGuildDlg();

			if( ! dialog )
			{
				break;
			}

			// 080417 LUJ, �޽��� â ǥ�� ���� �ᰬ���Ƿ� �ݵ�� �ٽ� Ǯ������
			dialog->SetDisable( FALSE );

			if(we == MBI_YES)
			{
				MSG_DWORD msg;
				msg.Category	= MP_GUILD;
				msg.Protocol	= MP_GUILD_DELETEMEMBER_SYN;
				msg.dwObjectID	= HEROID;
				msg.dwData		= dialog->GetKickMemberIndex();
				NETWORK->Send(&msg, sizeof(msg));
			}
		}
		break;
	case MBI_SHOPITEM_LOCK:
		{
			CInventoryExDialog* pInven = GAMEIN->GetInventoryDialog() ;
			if( !pInven )
			{
				return ;
			}
			else
			{
				if( we == MBI_YES ) pInven->ItemUnSealingSync() ;
			}


			// �κ��丮 Disable�� ������ �ش�.
			pInven->SetDisable(FALSE) ;
		}
		break;
	case MBI_REVIVE_PRESENTSPOT :
		{
			if( we == MBI_YES )
			{
				MSGBASE msg;
				msg.Category = MP_CHAR_REVIVE;
				msg.Protocol = MP_CHAR_REVIVE_PRESENTSPOT_SYN;
				msg.dwObjectID = HEROID;
				NETWORK->Send(&msg, sizeof(msg));
			}
			else
			{
				GAMEIN->GetReviveDialog()->SetDisable(FALSE);
			}
		}
		break ;

	case MBI_REVIVE_TOWNSOPT :
		{
			if( we == MBI_YES )
			{
				MSGBASE msg;
				msg.Category = MP_CHAR_REVIVE;
				msg.Protocol = MP_CHAR_REVIVE_VILLAGESPOT_SYN;
				msg.dwObjectID = HEROID;
				NETWORK->Send(&msg, sizeof(msg));
			}
			else
			{
				GAMEIN->GetReviveDialog()->SetDisable(FALSE);
			}
		}
		break ;

		// guildfieldwar -----------------------------------------------------------------
	case MBI_GUILDFIELDWAR_DECLARE_ACCEPT:
		{
			GAMEIN->GetGFWarResultDlg()->SetDisable( FALSE );
			if( we == MBI_YES )
				GUILDWARMGR->SendDeclareAcceptDeny( MP_GUILD_WAR_DECLARE_ACCEPT );
		}
		break;
	case MBI_GUILDFIELDWAR_DECLARE_DENY:
		{
			GAMEIN->GetGFWarResultDlg()->SetDisable( FALSE );
			if( we == MBI_YES )
				GUILDWARMGR->SendDeclareAcceptDeny( MP_GUILD_WAR_DECLARE_DENY );
		}
		break;	
	case MBI_GUILDFIELDWAR_SUGGEST:
		{
			GAMEIN->GetGFWarInfoDlg()->SetDisable( FALSE );
			//			GAMEIN->GetGFWarInfoDlg()->SetActive( FALSE );
			if( we == MBI_YES )
				GUILDWARMGR->SendSuggest();
		}
		break;
	case MBI_GUILDFIELDWAR_SURREND:
		{
			GAMEIN->GetGFWarInfoDlg()->SetDisable( FALSE );
			//			GAMEIN->GetGFWarInfoDlg()->SetActive( FALSE );
			if( we == MBI_YES )
				GUILDWARMGR->SendSurrender();
		}
		break;
	case MBI_GUILDFIELDWAR_SUGGEST_ACCEPTDENY:
		{
			if( we == MBI_YES )
			{
				GUILDWARMGR->SendSuggestAcceptDeny( MP_GUILD_WAR_SUGGESTEND_ACCEPT );
			}
			else if( we == MBI_NO )
			{
				GUILDWARMGR->SendSuggestAcceptDeny( MP_GUILD_WAR_SUGGESTEND_DENY );
			}
		}
		break;
	case MBI_GUILDFIELDWAR_REVIVE:
		{
			if( MBI_YES == we )
			{
				// ��Ȱ ���� â ����
				{
					cDialog* dialog = WINDOWMGR->GetWindowForID( GD_REVIVALDLG );
					ASSERT( dialog );

					dialog->SetActive( FALSE );
				}

				// ��Ȱ ��û
				{
					MSGBASE message;

					message.Category	= MP_CHAR_REVIVE;
					message.Protocol	= MP_CHAR_REVIVE_LOGINSPOT_SYN;
					message.dwObjectID	= HEROID;

					NETWORK->Send( &message, sizeof( message ) );
				}
			}
		}
		break;
		//--------------------------------------------------------------------------------
		// guildunion
	case MBI_UNION_INVITE:
		{
			if( we == MBI_YES )
				GUILDUNION->SendInviteAcceptDeny( MP_GUILD_UNION_INVITE_ACCEPT );
			else if( we == MBI_NO )
				GUILDUNION->SendInviteAcceptDeny( MP_GUILD_UNION_INVITE_DENY );
		}
		break;
	case MBI_UNION_REMOVE:
		{
			if( we == MBI_YES )
				GUILDUNION->SendRemoveGuildUnion();			
		}
		break;
	case MBI_UNION_DESTROY:
		{
			if( we == MBI_YES )
				GUILDUNION->SendDestroyGuildUnion();
			OBJECTSTATEMGR->EndObjectState( HERO, eObjectState_Deal );
		}
		break;
	case MBI_UNION_LEAVE:
		{
			if( we == MBI_YES )
			{
				MSGBASE message;
				message.Category	= MP_GUILD_UNION;
				message.Protocol	= MP_GUILD_UNION_SECEDE_SYN;
				message.dwObjectID	= HEROID;
				
				NETWORK->Send( &message, sizeof( message ) );
			}
		}
		break;
		//----------------------------------------------------------------------------------
	case MBI_PRESENT_NOTIFY:
		{
		}
		break;
		// 070607 LYW --- GlobalEventFunc : Add identification part.
	case MBI_IDENTIFICATION_REGIST_INFO :
		{
			CIdentification* pDlg = GAMEIN->GetIdentificationDlg() ;

			if( we == MBI_YES )
			{
				if( pDlg )
				{
					// check all data is ok.
					CSHResidentRegistManager::stRegistInfo* pRegistInfo = pDlg->GetRegistInfo() ;

					// setting name.
					strcpy(pRegistInfo->szNickName, HERO->GetObjectName()) ;

					// setting gender.
					if( strcmp(pDlg->GetGender()->GetStaticText(), RESRCMGR->GetMsg(508)) == 0 )
					{
						pRegistInfo->nSex = 0 ;
					}
					else
					{
						pRegistInfo->nSex = 1 ;
					}

					// setting age.
					pRegistInfo->nAge = (WORD)atoi(pDlg->GetAge()->GetEditText()) ;
					pRegistInfo->nLocation = pDlg->GetRegion()->GetClickedRowIdx() + 1;
					pRegistInfo->nFavor = 0 ;

					g_csResidentRegistManager.CLI_RequestRegist(pRegistInfo) ;
				}
			}
			else
			{
				pDlg->CancelRegist() ;
			}
		}
		break ;

	case MBI_IDENTIFICATION_ALREADY_HAVE :
		{
			// Deactive npc script dialog.
			if( HERO->GetState() != eObjectState_Die )
				OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal) ;
		}
		break ;

	case MBI_MATCHAT_INVITE :
		{
			// check.
			if( we == MBI_YES )
			{
				// start chatting.
				g_csResidentRegistManager.CLI_StartDateMatchingChat() ;
			}
			else
			{
				// send message.
				g_csResidentRegistManager.CLI_RefuseDateMatchingChat() ;
			}
		}
		break ;

	case MBI_MATCHAT_CANCEL :
		{
			if( we == MBI_CANCEL )
			{
				// cancel invite.
				g_csResidentRegistManager.CLI_CancelDateMatchingChat();
			}
		}
		break ;

	case MBI_END_CHATTING_PARTNERLIST :
		{
			if( we == MBI_YES )
			{
				// end chatting.
				g_csResidentRegistManager.CLI_EndDateMatchingChat() ;

				// return to partner list.
				GAMEIN->GetDateMatchingDlg()->SetUpperDlg(e_PartnerListDlg) ;
			}
			else if( we == MBI_NO )
			{
				GAMEIN->GetDateMatchingDlg()->GetPartnerTabBtn()->SetPush(FALSE) ;
			}
		}
		break ;

	case MBI_END_CHATTING_CLOSE :
		{
			if( we == MBI_YES )
			{
				// end chatting.
				g_csResidentRegistManager.CLI_EndDateMatchingChat() ;

				// close matching dialog.
				GAMEIN->GetDateMatchingDlg()->SetActive(FALSE) ;
			}
		}
		break ;

	case MBI_FAMILY_INVITE_YOU :
		{
			/*enum FAMILY_INVITE_RESULT
			{
				FIR_ACCEPT_MASTER = 0,
					FIR_ACCEPT_PLAYER,
					FIR_DENY_MASTER,
					FIR_DENY_PLAYER,
			};*/
			if( we == MBI_YES )
			{
				g_csFamilyManager.CLI_RequestAnswerToInvite(CSHFamilyManager::FIR_ACCEPT_PLAYER) ;
			}
			else
			{
				g_csFamilyManager.CLI_RequestAnswerToInvite(CSHFamilyManager::FIR_DENY_PLAYER) ;
			}
		}
		break ;

		// ��� ���� �̾� Ȯ�� �޽��� â
	case MBI_GUILD_RETIRE_NOTIFY:
		{
			CGuildDialog* dialog = ( CGuildDialog* )WINDOWMGR->GetWindowForID( GD_GUILDDLG );
			ASSERT( dialog );

			dialog->SetDisable( FALSE );

			const GUILDMEMBERINFO* member = dialog->GetSelectedMember();
			ASSERT( member );

			if( we == MBI_YES )
			{
				MSG_DWORD message;
				message.Category	= MP_GUILD;
				message.Protocol	= MP_GUILD_RETIRE_SYN;
				message.dwObjectID	= HEROID;
				message.dwData		= member->MemberIdx;

				NETWORK->Send( &message, sizeof( message ) );
			}
		}
		break;
	case MBI_GUILD_RETIRE_REQUEST:
		{
			// ��� â���� ��� ������ �ε����� �����´�.
			CHero* hero = OBJECTMGR->GetHero();
			ASSERT( hero );

			MSG_DWORD message;
			message.Category	= MP_GUILD;
			message.dwObjectID	= gHeroID;
			message.dwData		= gHeroID;
			message.Protocol	= ( MBI_YES == we ? MP_GUILD_RETIRE_ACK : MP_GUILD_RETIRE_NACK );

			NETWORK->Send( &message, sizeof( message ) );
		}
		break;
		// ������ ������ ������ �Ҷ� ���� ���ο� ���� Ȯ�� â�� ǥ���Ѵ�.
	case MBI_ITEM_MIX_CONFIRM:
		{
			// ���� ǥ�ð� ���� �� ������ �۾��� ��û�� ���̴�
			if( we == MBI_YES )
			{
				CProgressDialog* dialog = ( CProgressDialog* )WINDOWMGR->GetWindowForID( PROGRESS_DIALOG );
				ASSERT( dialog );

				dialog->Wait( CProgressDialog::eActionMix );
			}
			else
			{
				CMixDialog* dialog = ( CMixDialog* )WINDOWMGR->GetWindowForID( ITMD_MIXDLG );
				ASSERT( dialog );

				dialog->Restore();
			}
		}
		break;
		// ������ ��ȭ �ɼ��� �ִ� ��� �ɼ��� ������ �� ������ ���� ����� ǥ���Ѵ�
	case MBI_ITEM_REINFORCE_CONFIRM:
		{
			// ���� ǥ�ð� ���� �� ������ �۾��� ��û�� ���̴�
			if( we == MBI_YES )
			{
				CProgressDialog* dialog = ( CProgressDialog* )WINDOWMGR->GetWindowForID( PROGRESS_DIALOG );
				ASSERT( dialog );

				dialog->Wait( CProgressDialog::eActionReinforce );
			}
			else
			{
				CReinforceDlg* dialog = ( CReinforceDlg* )WINDOWMGR->GetWindowForID( ITR_REINFORCEDLG );
				ASSERT( dialog );

				dialog->Restore();
			}
		}
		break;
	// desc_hseos_�йи�01
	// S �йи� �߰� added by hseos 2007.10.16
	// ..Ż��
	case MBI_FAMILY_LEAVE:
		{
			if( we == MBI_YES )
			{
				g_csFamilyManager.CLI_RequestLeave();
			}
		}
		break;
	// ..�߹�
	case MBI_FAMILY_EXPEL:
		{
			if( we == MBI_YES )
			{
				g_csFamilyManager.CLI_RequestExpel(HERO->GetFamily()->GetMember(GAMEIN->GetFamilyDlg()->GetSelectedMemberIdx())->Get()->nID);
			}
		}
		break;
	// ..��ü
	case MBI_FAMILY_BREAKUP:
		{
			if( we == MBI_YES )
			{
				g_csFamilyManager.CLI_RequestBreakUp();
			}
		}
		break;

	case MBI_SKILLTRAIN:
		{
			if( we == MBI_YES )
			{
				GAMEIN->GetSkillTrainingDlg()->Training();
			}
			else
			{
				GAMEIN->GetSkillTrainingDlg()->SetDisable( FALSE );
			}
		}
		break;
	// E �йи� �߰� added by hseos 2007.10.16

	// 071201 LYW --- GlovalEventFunc : 
	case MBI_MAPMOVE_AREYOUSURE :
		{
			cMapMoveDialog* pDlg = NULL ;
			pDlg = GAMEIN->GetMapMoveDlg() ;

			if( !pDlg ) return ;
			if (!pDlg->IsActive() && we == MBI_YES)
				g_UserInput.GetHeroMove()->ChangeMap(mapnumforMSGBOX);

			if( we == MBI_YES )
			{
				pDlg->MoveMapSyn() ;
			}
		}
		break ;

	// 071203 LYW --- GlovalEventFunc : ���� ���� ���� ȿ���� ����ϰ� �ٽ� ������ ������ �����.
	case MBI_FORCE_USEBUFFITEM_AREYOUSURE :
	case MBI_USEBUFFITEM_AREYOUSURE :
		{
			CInventoryExDialog* pDlg = NULL ;
			pDlg = GAMEIN->GetInventoryDialog() ;

			if( !pDlg ) return ;

			// 080728 LUJ, �κ��丮 ����� �����Ѵ�
			pDlg->SetDisable( FALSE );

			if( we == MBI_YES )
			{
				CItem* pItem = NULL ;
				// �κ��� ������ ��� �ϰ� ����ϸ�, ���� ���� �� �������� ���� �� ó���� ���ϱ� ������
				// GetCurSelectedItem�� ������� �ʴ´�.
				//pItem = pDlg->GetCurSelectedItem(INV) ;	

				pItem = pDlg->Get_QuickSelectedItem() ;

				if( !pItem ) return ;

				ITEMMGR->UseItem_ItemMallItem(pItem, HERO) ;
			}
		}
		break ;

	// 071204 LYW --- GlovalEventFunc : ���� �ʱ�ȭ Ȯ�� �޽��� ó��.
	case MBI_RESETSTATS_AREYOUSURE :
		{
			CInventoryExDialog* pDlg = NULL ;
			pDlg = GAMEIN->GetInventoryDialog() ;

			if( !pDlg ) return ;

			CItem* pItem = NULL ;
			pItem = pDlg->Get_QuickSelectedItem() ;

			if( !pItem ) return ;

			if( we == MBI_YES )
			{
				ITEMMGR->UseItem_ResetStats(pItem) ;

				pItem->SetLock(FALSE) ;
			}

			pItem->SetLock(FALSE) ;
			pDlg->Set_QuickSelectedItem(NULL) ;
		}
		break ;

	// 071204 LYW --- GlovalEventFunc : ��ų �ʱ�ȭ Ȯ�� �޽��� ó��.
	case MBI_RESETSKILL_AREYOUSURE :
		{
			CInventoryExDialog* pDlg = NULL ;
			pDlg = GAMEIN->GetInventoryDialog() ;

			if( !pDlg ) return ;

			CItem* pItem = NULL ;
			pItem = pDlg->Get_QuickSelectedItem() ;

			if( !pItem ) return ;

			if( we == MBI_YES )
			{
				ITEMMGR->UseItem_ResetSkill(pItem) ;

				pItem->SetLock(FALSE) ;
			}

			pItem->SetLock(FALSE) ;
			pDlg->Set_QuickSelectedItem(NULL) ;
		}
		break ;
	// 080218 KTH -- ResetRegidentRegist
	case MBI_RESET_REGIDENTREGIST :
		{
			CInventoryExDialog* pDlg = NULL;
			pDlg = GAMEIN->GetInventoryDialog() ;

			if( !pDlg ) return;

			CItem* pItem = NULL;
			pItem = pDlg->Get_QuickSelectedItem();

			if( !pItem ) return;

			if( we == MBI_YES )
			{
				ITEMMGR->UseItem_ResetRegistResident(pItem);
				pItem->SetLock(FALSE);
			}
			
			pItem->SetLock(FALSE);
			pDlg->Set_QuickSelectedItem(NULL);
		}
		break;

	// 071205 LYW --- GlovalEventFunc : ����ġ ���� Ȯ�� �޽��� ó��.
	case MBI_INCEXPRATE_AREYOUSURE :
		{
			CInventoryExDialog* pDlg = NULL ;
			pDlg = GAMEIN->GetInventoryDialog() ;

			if( !pDlg ) return ;

			if( we == MBI_YES )
			{
				CItem* pItem = NULL ;
				pItem = pDlg->Get_QuickSelectedItem() ;

				if( !pItem ) return ;

				ITEMMGR->UseItem_ItemMallItem(pItem, HERO) ;

				pDlg->Set_QuickSelectedItem(NULL) ;
			}
		}
		break ;

	// 071205 LYW --- GlovalEventFunc : ����� ���� Ȯ�� �޽��� ó��.
	case MBI_INCDROPRATE_AREYOUSURE :
		{
			CInventoryExDialog* pDlg = NULL ;
			pDlg = GAMEIN->GetInventoryDialog() ;

			if( !pDlg ) return ;

			if( we == MBI_YES )
			{
				CItem* pItem = NULL ;
				pItem = pDlg->Get_QuickSelectedItem() ;

				if( !pItem ) return ;

				ITEMMGR->UseItem_ItemMallItem(pItem, HERO) ;

				pDlg->Set_QuickSelectedItem(NULL) ;
			}
		}
		break ;
	// 071210 LYW --- GlobalEventFunc : �κ��丮 Ȯ�� ��� �޽��� ó��.
	case MBI_EXTENDINVEN_AREYOUSURE :
		{
			CInventoryExDialog* pDlg = NULL ;
			pDlg = GAMEIN->GetInventoryDialog() ;

			if( !pDlg ) return ;

			CItem* pItem = NULL ;
			pItem = pDlg->Get_QuickSelectedItem() ;

			if( !pItem ) return ;

			if( we == MBI_YES )
			{
				ITEMMGR->UseItem_ExtendInventory(pItem) ;
			}

			pItem->SetLock(FALSE) ;
			pDlg->Set_QuickSelectedItem(NULL) ;
		}
		break ;
	// S ����ý��� �߰� added by hseos 2008.01.17
	case MBI_FARM_TAX_PAY_REQUEST:
		{
			if( we == MBI_YES )
			{
				// 091204 ONS �йи� ���̾�α׿� ���弼�� ���� ��� ��ư �߰�
				// ����ʿ����� ���������� �����ǰ� �ֱ� ������ ����ʿ����� ���ο� 
				// Ÿ�ʿ����� ���θ� �����Ͽ� ó���Ѵ�.
				if( CSHFarmManager::ePayTaxMode_FarmNpc == g_csFarmManager.GetPayTaxMode() )
				{
					g_csFarmManager.CLI_RequestPayTax(1);
				}
				else
				{
					g_csFarmManager.CLI_RequestPayTax(3);
				}
			}
		}
		break;
	// E ����ý��� �߰� added by hseos 2008.01.17
		// 080414 LUJ, �ܾ� ���� ������ ���
	case MBI_USEBODYCHANGEITEM:
		{
			cDialog* dialog = WINDOWMGR->GetWindowForID( BODY_CHANGE_DIALOG );

			// 080930 LUJ, �޽��� �ڽ� ó���� �ش� â���� �ϵ��� �Ѵ�
			if( ! dialog )
			{	
				break;
			}

			dialog->OnActionEvent( MBI_USEBODYCHANGEITEM, 0, we );
		}
		break;
	// 080507 KTH -- ��������� ���
	case MBI_FARM_ANIMAL_INSTALL :
		{
			CInventoryExDialog* pDlg = NULL ;
			pDlg = GAMEIN->GetInventoryDialog() ;

			if( !pDlg ) return ;

			CItem* pItem = NULL ;
			pItem = pDlg->Get_QuickSelectedItem() ;

			if( !pItem ) return ;

			if( we == MBI_YES )
			{
				g_csFarmManager.CLI_RequestInstallAnimalInAnimalCage(pItem->GetPosition(), pItem->GetItemIdx());
			}

			pItem->SetLock(FALSE) ;
			pDlg->Set_QuickSelectedItem(NULL) ;
		}
		break ;

	case MBI_CR_SECRETCODE_ERR :
 		{
 			if( we != MBI_OK ) return ;
 
 			CChatRoomCreateDlg* pDlg = NULL ;
 			pDlg = GAMEIN->GetChatRoomCreateDlg() ;
 
 			if( !pDlg ) return ;
 
 			cEditBox* pCodeBox = NULL ;
 			pCodeBox = pDlg->GetCodeBox() ;
 
 			if( !pCodeBox ) return ;
 
 			pCodeBox->SetFocus(TRUE) ;
 			// Ȥ�� ä��â�� ��Ŀ���� ������ �Ѥ�; ���� ����.
 		}
 		break ;

	case MBI_CR_ENDCHAT :														// ä�ù� ���� ��ư ó��.
		{
			if( we == MBI_YES )
			{
				CChatRoomDlg* pDlg = NULL ;										// ä�ù� ������ �޴´�.
				pDlg = GAMEIN->GetChatRoomDlg() ;

				ASSERT(pDlg) ;

				if(!pDlg)
				{
					MessageBox( NULL, "ERR-FRCRD", "GEF-CR_EC", MB_OK) ;
					return ;
				}

				MSG_DWORD msg ;													// ���� ��û �޽����� ������.
				memset(&msg, 0, sizeof(MSG_DWORD)) ;

				msg.Category	= MP_CHATROOM ;
				msg.Protocol	= MP_CHATROOM_OUT_ROOM_SYN ;
				msg.dwObjectID	= HEROID ;

				msg.dwData		= CHATROOMMGR->GetMyRoomOwnerIdx() ;

				NETWORK->Send( &msg, sizeof(MSG_DWORD) ) ;
			}
		}
		break;
	case MBI_PETGRADEUP:
		{
			CInventoryExDialog* pDlg = GAMEIN->GetInventoryDialog();
			if( we == MBI_YES )
			{
				pDlg->PetGradeUp();
			}
			else
			{
				pDlg->SetDisable( FALSE );
			}
		}
		break;
	case MBI_GTENTER_SYN:
		{
			if( we == MBI_YES)
			{
				MSG_DWORD msgMapSyn;
				msgMapSyn.Category = MP_GTOURNAMENT;
				msgMapSyn.Protocol = MP_GTOURNAMENT_MOVETOBATTLEMAP_SYN;
				msgMapSyn.dwObjectID = HEROID;
				msgMapSyn.dwData = 0;
				NETWORK->Send( &msgMapSyn, sizeof(msgMapSyn) );
			}
		}
		break;

		// 080821 LYW --- GlobalEventFunc : ����� �� �̵� ��Ż ó�� �߰�.
  	case MBI_RECALLPORTAL_CHANGEMAP :
  		{
  			if( we == MBI_YES ) 
  			{
  				CObject* pObject = NULL ;
  				pObject = OBJECTMGR->GetSelectedObject() ;
  
  				if( !pObject )
  				{
  					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1689 ) );//"����� ������ �ּ���." ) ;

  					return ;
  				}
  
  				if( pObject->GetObjectKind() != eObjectKind_Npc ||
  					((CNpc*)pObject)->GetNpcJob() != PORTAL_ROLE )
  				{
  					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1690 ) ); //"�ùٸ� ����� �ƴմϴ�." ) ; 
  					return ;
  				}
  
  				MSG_DWORD msg ;
  
  				msg.Category	= MP_RECALLNPC ;
  				msg.Protocol	= MP_RECALLNPC_CTOAM_CHANGEMAP_SYN ;
  				msg.dwObjectID	= HEROID ; 
  				msg.dwData		= NPCRECALLMGR->Get_CurSelectNpcId() ;
  				
  				NETWORK->Send(&msg, sizeof(msg));
  			}
  		}
  		break ;
  
  		// 080922 LYW --- GlobalEventFunc : ��� ����Ʈ�� �̵��ϰڳĴ� ó�� �߰�.
  	case MBI_SIEGEWARFLAG : 
  		{
  			if( we == MBI_YES )
  			{
  				MSGBASE msg ;
  
  				msg.Category	= MP_SIEGEWARFARE ;
  				msg.Protocol	= MP_SIEGEWARFARE_WARPTOAGIT_SYN ;
  
  				msg.dwObjectID	= HEROID ;
  
  				NETWORK->Send( &msg, sizeof(MSGBASE) ) ;
  			}
  		}
  		break;
		// 091127 LUJ, ��ȯ ��ų ���
	case MBI_RECALL_REQUEST:
		{
			if( MBI_NO == we )
			{
				break;
			}

			const CMoveManager::TemporaryRecallData& data = MOVEMGR->GetTemporaryRecallData();

			MSG_NAME_DWORD3 message;
			ZeroMemory(&message, sizeof(message));
			message.Category = MP_SKILL;
			message.Protocol = MP_SKILL_RECALL_SYN;
			message.dwObjectID = gHeroID;
			message.dwData1	= data.mTargetPlayerIndex;
			message.dwData2	= data.mSkillIndex;
			message.dwData3 = data.mMapType;
			SafeStrCpy(
				message.Name,
				data.mTargetPlayerName,
				sizeof(message.Name) / sizeof(*message.Name));
			NETWORK->Send( &message, sizeof( message ) );
		}
		break;

		// 081031 LUJ, ��ȯ �³� ���θ� ó���Ѵ�
	case MBI_RECALL_ASK:
		{
			const CMoveManager::TemporaryRecallData& data = MOVEMGR->GetTemporaryRecallData();

			MSG_DWORD message;
			ZeroMemory( &message, sizeof( message ) );
			message.Category	= MP_MOVE;
			message.Protocol	= ( MBI_YES == we ? MP_MOVE_RECALL_ASK_ACK : MP_MOVE_RECALL_ASK_NACK );
			message.dwObjectID	= HEROID;
			message.dwData		= data.mKey;
			NETWORK->Send( &message, sizeof( message ) );
		}
		break;
		// 090422 ShinJS --- Ż�� ž�� ��û�� ����/�ź� 
	case MBI_VEHICLE_ALLOW_GETON:
		{
			// �ڽ��� Ż�� ������ ���´�
			CVehicle* pVehicle = (CVehicle*)OBJECTMGR->GetObject( HERO->GetVehicleID() );
			if( !pVehicle || pVehicle->GetObjectKind() != eObjectKind_Vehicle )		break;

			cMsgBox* const messageBox = (cMsgBox*)WINDOWMGR->GetWindowForID(
				MBI_VEHICLE_ALLOW_GETON);

			if(0 == messageBox)
			{
				break;
			}

			// ž�� ����
			if( we == MBI_YES )
			{
				MSG_DWORD4 msg;
				ZeroMemory( &msg, sizeof( msg ) );
				msg.Category = MP_VEHICLE;
				msg.Protocol = MP_VEHICLE_MOUNT_ALLOW_SYN;
				msg.dwData1 = pVehicle->GetRequestGetOnPlayerID();
				msg.dwData2 = pVehicle->GetID();
				msg.dwData3 = pVehicle->GetEmptySeatPos(pVehicle->GetRequestGetOnPlayerID());
				msg.dwData4 = messageBox->GetParam();
				NETWORK->Send(
					&msg,
					sizeof(msg));
			}
			// ž�� �ź�
			else
			{
				MSG_DWORD msg;
				ZeroMemory( &msg, sizeof( msg ) );
				msg.Category = MP_VEHICLE;
				msg.Protocol = MP_VEHICLE_MOUNT_REJECT_SYN;
				msg.dwObjectID = HEROID;
				msg.dwData = pVehicle->GetRequestGetOnPlayerID();
				NETWORK->Send( &msg, sizeof( msg ) );
			}
		}
		break;

	// 090422 ShinJS --- Ż�� ž�½� ����� �ʿ��� ��� ž�� �õ� �Ǵ�
	case MBI_VEHICLE_GETON_ASK_PAY_TOLL:
		{
			if( we != MBI_YES )
			{
				break;
			}

			CVehicle* const pVehicle = (CVehicle*)OBJECTMGR->GetObject( VEHICLEMGR->GetRidingVehicle() );

			if(0 == pVehicle)
			{
				break;
			}
			else if(eObjectKind_Vehicle != pVehicle->GetObjectKind())
			{
				break;
			}

			if(gHeroID != pVehicle->GetOwnerIndex())
			{
				CHATMGR->AddMsg(
					CTC_SYSMSG,
					CHATMGR->GetChatMsg(2034),
					pVehicle->GetMasterName());
			}

			cMsgBox* const messageBox = (cMsgBox*)WINDOWMGR->GetWindowForID(
				MBI_VEHICLE_GETON_ASK_PAY_TOLL);

			if(0 == messageBox)
			{
				break;
			}
			else if(WT_MSGBOX != messageBox->GetType())
			{
				break;
			}

			MSG_DWORD2 msg;
			ZeroMemory( &msg, sizeof( msg ) );
			msg.Category = MP_VEHICLE;
			msg.Protocol = MP_VEHICLE_MOUNT_ASK_SYN;
			msg.dwObjectID = gHeroID;
			msg.dwData1 = pVehicle->GetID();
			msg.dwData2 = messageBox->GetParam();
			NETWORK->Send(
				&msg,
				sizeof(msg));
		}
		break;
	// 090415 �Ͽ�¡ �̸����� ���� ���θ� ó���Ѵ� 
	case MBI_HOUSENAME_AREYOUSURE:
		{
			//��Ŷ�� ������. 
			cHouseNameDlg* pNameDlg = GAMEIN->GetHouseNameDlg();
			if( MBI_NO == we )
			{
				pNameDlg->SetDisable(FALSE);
				break;
			}
			//090420 �Ͽ콺���� ��Ŷ�� ������ MP_HOUSE_CREATE_SYN

			MSG_HOUSE_CREATE Message;
			ZeroMemory( &Message, sizeof( Message ) );
			Message.Category = MP_HOUSE ; 
			Message.Protocol = MP_HOUSE_CREATE_SYN ; 
			Message.dwPlayerID = HEROID;
			Message.dwPlayerID = HEROID;
			SafeStrCpy(Message.szHouseName, pNameDlg->GetName() , MAX_HOUSING_NAME_LENGTH+1);

			NETWORK->Send( &Message, sizeof( Message ) );

		}
		break;
	// 090415 �Ͽ�¡ ��ã�� ���� ���θ� ó���Ѵ� . 
	case MBI_HOUSESEACH_AREYOUSURE:
		{
			//��Ŷ�� ������.
			cHouseSearchDlg* pSearchDlg = GAMEIN->GetHouseSearchDlg();
			stVisitInfo* pVisitInfo = pSearchDlg->GetVisitInfo();
			if( MBI_NO == we || ! pVisitInfo )
			{
				GAMEIN->GetInventoryDialog()->SetDisable( FALSE );
				pSearchDlg->SetDisableState(FALSE);
				break;
			}

			pSearchDlg->AddVisitList();

			MSG_HOUSE_VISIT msg;
			ZeroMemory( &msg, sizeof( msg ) );
			msg.Category = MP_HOUSE;
			msg.Protocol = MP_HOUSE_ENTRANCE_SYN;
			msg.dwObjectID = HEROID;
			msg.cKind	= pVisitInfo->m_dwVisitKind;
			msg.dwValue1 = pVisitInfo->m_dwData1;
			msg.dwValue2 = pVisitInfo->m_dwData2;

			SafeStrCpy(msg.Name, pSearchDlg->GetName() , MAX_NAME_LENGTH+1);
			NETWORK->Send(&msg,sizeof(msg));
		}
		break;
	//090618 pdy �Ͽ�¡ ����߰� �������� 
	case MBI_VISIT_RANDOM_AREYOUSURE:
		{
			//��Ŷ�� ������.
			cHouseSearchDlg* pSearchDlg = GAMEIN->GetHouseSearchDlg();
			stVisitInfo* pVisitInfo = pSearchDlg->GetVisitInfo();
			if( MBI_NO == we || ! pVisitInfo )
			{
				GAMEIN->GetInventoryDialog()->SetDisable( FALSE );
				pSearchDlg->SetDisableState(FALSE);
				break;
			}

			MSG_HOUSE_VISIT msg;
			ZeroMemory( &msg, sizeof( msg ) );
			msg.Category = MP_HOUSE;
			msg.Protocol = MP_HOUSE_ENTRANCE_SYN;
			msg.dwObjectID = HEROID;
			msg.cKind	=  eHouseVisitByItemRandom;
			msg.dwValue1 = pVisitInfo->m_dwData1;
			msg.dwValue2 = pVisitInfo->m_dwData2;

			SafeStrCpy(msg.Name, pSearchDlg->GetName() , MAX_NAME_LENGTH+1);
			NETWORK->Send(&msg,sizeof(msg));
		}
		break;
		// 090415 �Ͽ�¡ �ٹ̱� ���ʽ� ��� ���θ� ó���Ѵ� . 
	case MBI_USEDECOBONUS_AREYOUSURE:
		{
			//ó������ ó������ ó������ 
			cHousingDecoPointDlg* pDecoPointDlg = GAMEIN->GetHousingDecoPointDlg();
			pDecoPointDlg->SetDisable(FALSE);
			if( MBI_NO == we )
			{
				break;
			}

			stHouseBonusInfo* pBonusInfo = pDecoPointDlg->GetCurSelectBonusInfo();

			if(! pBonusInfo )
				break;

			HOUSINGMGR->RequestUseDecoBonus(pBonusInfo->dwBonusIndex);
		}
		break;
	case MBI_DECOINSTALL_AREYOUSURE:
		{
			GAMEIN->GetHousingRevolDlg()->SetDisable(FALSE);
			if( MBI_NO == we )
			{
				break;
			}

			//���ʽÿ��� ���� 
			if( HOUSINGMGR->RequestInstallCurDeco() )
				GAMEIN->GetHousingRevolDlg()->SetActive(FALSE);
		}
		break;
	case MBI_HOUSE_EXIT_AREYOUSURE:
		{
			GAMEIN->GetHousingActionPopupMenuDlg()->SetDisable(FALSE);
			if( MBI_NO == we )
			{
				//090618 pdy �Ͽ�¡ ������ �׼� ���� ���� 
				OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal) ;
				break;
			}
			HOUSINGMGR->RequestUseCurAction();
		}
		break;
	case MBI_HOUSE_VOTEHOUSE_AREYOUSURE:
		{
			GAMEIN->GetHousingActionPopupMenuDlg()->SetDisable(FALSE);
			if( MBI_NO == we )
			{
				break;
			}

			HOUSINGMGR->RequestUseCurAction();
		}
		break;
	case MBI_HOUSE_CHANGEMATERIAL_AREYOUSURE:
		{
			CInventoryExDialog* pDlg = GAMEIN->GetInventoryDialog();
			CItem* pItem = pDlg->Get_QuickSelectedItem() ;
			if( !pItem ) return ;

			ITEM_INFO* pInfo = ITEMMGR->GetItemInfo( pItem->GetItemIdx() );
			if( ! pInfo ) return ;

			if( MBI_NO == we )
			{
				pItem->SetLock(FALSE) ;
				HOUSINGMGR->ChangeMaterialFurniture(pInfo->SupplyType , pInfo->SupplyValue, 1 );
			}
			else
			{
				MSG_ITEM_USE_SYN msg;
				msg.Category = MP_ITEM;
				msg.Protocol = MP_ITEM_USE_SYN;
				msg.dwObjectID = HEROID;
				msg.wItemIdx = pItem->GetItemIdx();
				msg.TargetPos = pItem->GetPosition();

				NETWORK->Send(&msg,sizeof(msg));
			}

			pDlg->SetDisable(FALSE);
			pDlg->Set_QuickSelectedItem(NULL) ;
		}
		break;
	case MBI_HOUSE_VISIT_MYHOME_AREYOUSURE:
		{
			CInventoryExDialog* pDlg = GAMEIN->GetInventoryDialog();
			CItem* pItem = pDlg->Get_QuickSelectedItem() ;
			if( !pItem ) return ;

			if( MBI_YES == we )
			{
				MSG_HOUSE_VISIT msg;
				msg.Category = MP_HOUSE;
				msg.Protocol = MP_HOUSE_ENTRANCE_SYN;
				msg.dwObjectID = HEROID;
				msg.cKind	=  eHouseVisitByItem ;
				msg.dwValue1 = pItem->GetItemIdx() ;
				msg.dwValue2 = pItem->GetPosition() ;
				SafeStrCpy(msg.Name, HERO->GetObjectName() , MAX_NAME_LENGTH+1);

				NETWORK->Send(&msg,sizeof(msg));
			}
			else
			{
				pItem->SetLock(FALSE);
				OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal) ;
			}

			pDlg->SetDisable(FALSE);
			pDlg->Set_QuickSelectedItem(NULL) ;
		}
		break;
	case MBI_VISIT_LINK_PLAYER_RCLICK:						//090615 pdy �Ͽ�¡ ĳ���� ��Ŭ�� ��ũ �Ͽ콺���� ��� �߰�
		{
			if( MBI_YES == we )
			{
				cMsgBox* pMsgBox = (cMsgBox*)WINDOWMGR->GetWindowForID( MBI_VISIT_LINK_PLAYER_RCLICK );				
				CObject* pDestObj = OBJECTMGR->GetObject( pMsgBox->GetParam() );

				if( pDestObj )
				{
					BASEOBJECT_INFO DestObjInfo;	
					pDestObj->GetBaseObjectInfo(&DestObjInfo);

					MSG_HOUSE_VISIT msg;
					msg.Category = MP_HOUSE;
					msg.Protocol = MP_HOUSE_ENTRANCE_SYN;
					msg.dwObjectID = HEROID;
					msg.cKind	=  eHouseVisitByLink ;			
					msg.dwValue1 = 0;				
					msg.dwValue2 = DestObjInfo.dwUserID;
					NETWORK->Send(&msg,sizeof(msg));
				}
				else
				{
					OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal) ;
				}
			}
			else
			{
				OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal) ;
			}
		}
		break;
	case MBI_HOUSE_VISIT_LINK_MYHOME_AREYOUSURE:			//090604 pdy static Npc�� ���� �������� �߰� 
		{
			if( MBI_YES == we )
			{
				cMsgBox* pMsgBox = (cMsgBox*)WINDOWMGR->GetWindowForID( MBI_HOUSE_VISIT_LINK_MYHOME_AREYOUSURE );				
				CNpc* pNpc = (CNpc*)OBJECTMGR->GetObject( pMsgBox->GetParam() );

				if( pNpc )
				{
					MSG_HOUSE_VISIT msg;
					msg.Category = MP_HOUSE;
					msg.Protocol = MP_HOUSE_ENTRANCE_SYN;
					msg.dwObjectID = HEROID;
					msg.cKind	=  eHouseVisitByLink ;			//Npc�� ���� ���°��� eHouseVisitByLink
					msg.dwValue1 = pNpc->GetID();				
					msg.dwValue2 = TITLE->GetUserIdx();			//Npc�� ���� ������ ������ �����ε����� ���׾��Ѵ�.
					NETWORK->Send(&msg,sizeof(msg));
				}
				else
				{
					OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal) ;
				}
			}
			else
			{
				OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal) ;
			}
		}
		break;
	case MBI_HOUSE_VISIT_LINK_AREYOUSURE:
		{
			if( MBI_YES == we )
			{
				if(HERO->GetVehicleID())
				{
					CHATMGR->AddMsg(CTC_SYSMSG,  CHATMGR->GetChatMsg(1839) ); //1839	"ž�� ������ �̿� �ÿ��� �ش� ����� ����Ͻ� �� �����ϴ�."
					OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal) ;
					return;
				}

				cMsgBox* pMsgBox = (cMsgBox*)WINDOWMGR->GetWindowForID( MBI_HOUSE_VISIT_LINK_AREYOUSURE );				
				CNpc* pNpc = (CNpc*)OBJECTMGR->GetObject( pMsgBox->GetParam() );
				if( pNpc )
				{
					MSG_HOUSE_VISIT msg;
					msg.Category = MP_HOUSE;
					msg.Protocol = MP_HOUSE_ENTRANCE_SYN;
					msg.dwObjectID = HEROID;
					msg.cKind	=  eHouseVisitByLink ;
					msg.dwValue1 = pNpc->GetID();
					msg.dwValue2 = pNpc->GetNpcTotalInfo()->dwSummonerUserIndex;
					NETWORK->Send(&msg,sizeof(msg));
				}
				else
				{
					OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal) ;
				}
			}
			else 
			{
				OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal) ;
			}
		}
		break;
	case MBI_HOUSE_VISIT_LINK_FRIENDHOME :
		{
			if( MBI_YES == we )
			{
				cMsgBox* pMsgBox = (cMsgBox*)WINDOWMGR->GetWindowForID( MBI_HOUSE_VISIT_LINK_FRIENDHOME );		
				char* pFriendName = (char*)(pMsgBox->GetParam());
				if( pFriendName )
				{
					MSG_HOUSE_VISIT msg;
					msg.Category = MP_HOUSE;
					msg.Protocol = MP_HOUSE_ENTRANCE_SYN;
					msg.dwObjectID = HEROID;
					msg.cKind	=  eHouseVisitByName ;
					msg.dwValue1 = 0;
					msg.dwValue2 = 0;
					SafeStrCpy(msg.Name, pFriendName , MAX_NAME_LENGTH+1);
					NETWORK->Send(&msg,sizeof(msg));
				}
				else
				{
					OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal) ;
				}
			}
			else
			{
				OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal) ;
			}

		}
		break;
	case MBI_HOUSE_EXTEND_AREYOUSURE :
		{
			CInventoryExDialog* pDlg = GAMEIN->GetInventoryDialog();
			CItem* pItem = pDlg->Get_QuickSelectedItem() ;
			if( !pItem ) return ;

			if( MBI_YES == we )
			{
				MSG_ITEM_USE_SYN msg;
				msg.Category = MP_ITEM;
				msg.Protocol = MP_ITEM_USE_SYN;
				msg.dwObjectID = HEROID;
				msg.wItemIdx = pItem->GetItemIdx();
				msg.TargetPos = pItem->GetPosition();

				NETWORK->Send(&msg,sizeof(msg));
			}
			else
			{
				pItem->SetLock(FALSE);
			}

			pDlg->SetDisable(FALSE);
			pDlg->Set_QuickSelectedItem(NULL) ;
		}
		break;
	case MBI_HOUSE_DESTROY_FUTNITURE_AREYOUSURE:
		{
			cHousingWarehouseDlg* pDlg = GAMEIN->GetHousingWarehouseDlg();
			if(! pDlg ) return;

			cHousingStoredIcon* pStoredIcon = pDlg->Get_QuickSelectedIcon();
			if( ! pStoredIcon ) return;

			if( MBI_YES == we )
			{
				HOUSINGMGR->RequestDestroyFuniture(pStoredIcon->GetLinkFurniture());
			}
		}
		break;
	// 091125 ONS ĳ���͸� ����� �˾� �޼��� ���
	case MBI_CHANGENAME_ACK:
		{
			cChangeNameDialog* pDlg = GAMEIN->GetChangeNameDlg();
			if(! pDlg ) return;

			if( MBI_YES == we )
			{
				ITEMMGR->UseItem_ChangeName( pDlg->GetChangeName() ) ;
			}
		}
		break;
	// 091126 ONS �йи� ���̾�α׿� �йи��� �̾� ��� �߰�
	case MBI_FAMILY_TRANSFER:
		{	
			if( MBI_YES == we )
			{
				g_csFamilyManager.CLI_RequestTransfer( HERO->GetFamily()->GetMember(GAMEIN->GetFamilyDlg()->GetSelectedMemberIdx())->Get()->nID );
			}
		}
		break;
	// 100105 ShinJS --- PC�� ������ ���� ��û Ȯ��
	case MBI_PCROOM_PROVIDE_ITEM:
		{
			if( MBI_YES == we )
			{
				// ������ ���� ��û
				MSGBASE msg;
				ZeroMemory( &msg, sizeof(msg) );
				msg.Category = MP_PCROOM;
				msg.Protocol = MP_PCROOM_PROVIDE_ITEM_SYN;
				msg.dwObjectID = HEROID;
				NETWORK->Send( &msg, sizeof(msg) );
			}
		}
		break;
	// 100211 ONS ��Ȱ�迭 ��ų ���� ����ڿ��� ��Ȱ�ǻ縦 ���� ó�� �߰�
	case MBI_RESURRECT_ASK:
		{
			MSGBASE msg;
			ZeroMemory( &msg, sizeof(msg) );
			msg.Category = MP_SKILL;
			if( MBI_YES == we )
			{
				msg.Protocol = MP_SKILL_RESURRECT_ACK;
			}
			else
			{
				msg.Protocol = MP_SKILL_RESURRECT_NACK;

				// ��Ȱ�� �ź��ϸ� ������ ��Ȱ���� ���� ���̾�α׸� Ȱ��ȭ�Ѵ�.
				CReviveDialog* pDlg = GAMEIN->GetReviveDialog();
				if( pDlg && !pDlg->IsActive() )
				{
					pDlg->SetActive( TRUE );
				}
			}
			msg.dwObjectID = HEROID;
			NETWORK->Send( &msg, sizeof(msg) );		
		}
		break;

	case MBI_CONSIGNMENT_CANCEL_CONFIRM:
		{
			if(MBI_YES == we)
			{
				CConsignmentDlg* pDlg = GAMEIN->GetConsignmentDlg();
				if(pDlg && pDlg->GetMode()==CConsignmentDlg::eConsignment_Mode_Regist)
				{
					MSG_CONSIGNMENT_UPDATE msg;
					msg.Category = MP_CONSIGNMENT;
					msg.Protocol = MP_CONSIGNMENT_UPDATE_SYN;
					msg.dwObjectID = HEROID;
					msg.dwConsignmentIndex = pDlg->GetConsignmentIndex();
					msg.wUpdateKind = eConsignmentUPDATEKIND_CancelByUser;
					msg.dwBuyItemIndex = 0;
					msg.dwBuyTotalPrice = 0;
					msg.wBuyDurability = 0;
					
					NETWORK->Send( &msg, sizeof(msg) );
				}
			}
		}
		break;

	case MBI_CONSIGNMENT_BUY_CONFIRM:
		{
			if(MBI_YES == we)
			{
				CConsignmentDlg* pDlg = GAMEIN->GetConsignmentDlg();
				if(pDlg && pDlg->GetMode()==CConsignmentDlg::eConsignment_Mode_Buy)
				{
					DWORD dwConsignmentIndex = pDlg->GetConsignmentIndex();
					DWORD dwBuyItemIndex = pDlg->GetBuyItemIndex(dwConsignmentIndex);
					WORD wBuyDurbility = pDlg->GetBuyDurability(dwConsignmentIndex);
					DWORD dwBuyTotalPrice = pDlg->GetBuyPrice(dwConsignmentIndex) * wBuyDurbility;

					if(!dwConsignmentIndex || !dwBuyItemIndex || !wBuyDurbility || !dwBuyTotalPrice)
						return;

					MSG_CONSIGNMENT_UPDATE msg;
					msg.Category = MP_CONSIGNMENT;
					msg.Protocol = MP_CONSIGNMENT_UPDATE_SYN;
					msg.dwObjectID = HEROID;
					msg.dwConsignmentIndex = dwConsignmentIndex;
					msg.wUpdateKind = eConsignmentUPDATEKIND_Buy;
					msg.dwBuyItemIndex = dwBuyItemIndex;
					msg.wBuyDurability = wBuyDurbility;
					msg.dwBuyTotalPrice = dwBuyTotalPrice;
					NETWORK->Send( &msg, sizeof(msg) );
				}
			}
		}	
		break;
	}
}

void MNM_DlgFunc(LONG lId, void * p, DWORD we)
{
	switch(lId)
	{
	case MNM_PARTYDISPLAY_ONOFF:
		break;
	case MNM_WORLDMAPBTN:
		{
			cDialog* pDlg = GAMEIN->GetWorldMapDialog();
			if( pDlg )
			{
				pDlg->SetActive( !pDlg->IsActive() );
			}
		}
		break;
	case MNM_BIGMAPBTN:
		{
			cDialog* pDlg = GAMEIN->GetBigMapDialog();
			if( pDlg )
			{
				pDlg->SetActive( !pDlg->IsActive() );
			}
		}
		break;

	// 091209 ShinJS --- ��ã��� ��ư �߰�
	case MNM_PATHFIND_BTN:
		{
			// ��ã�� ����
			CMiniMapDlg* pMiniMapDlg = (CMiniMapDlg*)p;
			if( pMiniMapDlg && we == WE_BTNCLICK )
				pMiniMapDlg->Move_UsePath();
		}
		break;

		// 091209 ShinJS --- �����۸� ��ư
	case MNM_ITEMMALL_BTN:
		{
			if(GAMEIN->GetItemShopDialog()->IsActive())
			{
				GAMEIN->GetItemShopDialog()->SetActive( FALSE );
				// 100223 ONS �κ��丮�� �Բ� �ݴ´�.
				GAMEIN->GetInventoryDialog()->SetActive( FALSE );
				return;
			}

			// 091105 pdy ����/��æƮ/��ȭ/���� ���ΰ�� �����۸� ���� �Ұ��� 
			BOOL isOpen = FALSE;
			{
				cDialog* mixDialog			= WINDOWMGR->GetWindowForID( ITMD_MIXDLG );
				cDialog* enchantDialog		= WINDOWMGR->GetWindowForID( ITD_UPGRADEDLG );
				cDialog* reinforceDialog	= WINDOWMGR->GetWindowForID( ITR_REINFORCEDLG );
				cDialog* dissoloveDialog	= WINDOWMGR->GetWindowForID( DIS_DISSOLUTIONDLG );

				ASSERT( mixDialog && enchantDialog && reinforceDialog && dissoloveDialog );

				isOpen =	mixDialog->IsActive()		||
					enchantDialog->IsActive()	||
					reinforceDialog->IsActive()	||
					dissoloveDialog->IsActive();
			}

			if( isOpen == TRUE )
			{
				CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1082 ) ) ;
				return;
			}

			GAMEIN->GetItemShopDialog()->SetActive( TRUE );
			GAMEIN->GetInventoryDialog()->SetActive( TRUE );

			MSGBASE msg;																

			msg.Category = MP_ITEM;														
			msg.Protocol = MP_ITEM_SHOPITEM_INFO_SYN;									
			msg.dwObjectID = gHeroID;
			NETWORK->Send(&msg, sizeof(msg));
		}
		break;

		// 091209 ShinJS --- ä�ù� ��ư
	case MNM_CHATROOM_BTN:
		{
			CHATROOMMGR->ToggleChatRoomMainDlg();
		}
		break;

		// 091209 ShinJS --- ��ȭ ����(��ȭ Dialog ����) ��ư
	case MNM_VIDEO_REC_START_BTN:
		{
			// ��ȭ Dialog �� ����
			cDialog* pVideoCaptureDlg = WINDOWMGR->GetWindowForID( VIDEOCAPTURE_DLG );
			if( !pVideoCaptureDlg )
				break;

			pVideoCaptureDlg->SetActive( !pVideoCaptureDlg->IsActive() );
		}
		break;

		// 091209 ShinJS --- ��ȭ ���� ��ư
	case MNM_VIDEO_REC_STOP_BTN:
		{
			CMiniMapDlg* pMiniMapDlg = (CMiniMapDlg*)p;
			if( !pMiniMapDlg )
				break;

			pMiniMapDlg->ShowVideoCaptureStartBtn( TRUE );

			VIDEOCAPTUREMGR->CaptureStop();
		}
		break;

		// 091209 ShinJS --- PC�� ����
	case MNM_PCROOM_MALL_BTN:
		{
			if( GAMEIN->GetDealDialog()->IsActive() )
			{
				// �̹� PC�� ������ ���� ���
				if( GAMEIN->GetDealDialog()->IsActivePointInfo() )
				{
					// 100223 ONS ������ �����ִ°�� �ݴ´�.
					GAMEIN->GetDealDialog()->ShowDealDialog( FALSE );
					break;
				}
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1077 ) );		// ������ ���� ��������
				break;
			}

			if( HERO->GetState() == eObjectState_Die)
				break;
			
			cDialog* mixDialog			= WINDOWMGR->GetWindowForID( ITMD_MIXDLG );
			cDialog* enchantDialog		= WINDOWMGR->GetWindowForID( ITD_UPGRADEDLG );
			cDialog* reinforceDialog	= WINDOWMGR->GetWindowForID( ITR_REINFORCEDLG );
			cDialog* dissoloveDialog	= WINDOWMGR->GetWindowForID( DIS_DISSOLUTIONDLG );

			const BOOL isOpen = mixDialog->IsActive()		||
								enchantDialog->IsActive()	||
								reinforceDialog->IsActive()	||
								dissoloveDialog->IsActive();

			if( isOpen )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1104 ) );
				return;
			}

			MSGBASE msg;
			ZeroMemory( &msg, sizeof(msg) );
			msg.Category = MP_PCROOM;
			msg.Protocol = MP_PCROOM_OPEN_MALL_SYN;
			msg.dwObjectID = HEROID;
			NETWORK->Send( &msg, sizeof(msg) );
		}
		break;
	
		// 100105 ShinJS --- PC�� ������ ���� ��û
	case MNM_PCROOM_PROVIDE_ITEM_BTN:
		{
			// ������ ���� ���θ� ���� MsgBox ����
			WINDOWMGR->MsgBox( MBI_PCROOM_PROVIDE_ITEM, MBT_YESNO, CHATMGR->GetChatMsg( 2016 ) );
		}
		break;
	// 100611 ��������ư ó��
	case MNM_LEVELUP_BTN:
		{
			const HERO_TOTALINFO*	heroInfo	=	HERO->GetHeroTotalInfo();

			cPushupButton* pLevelUpButton	=	(cPushupButton*)GAMEIN->GetMiniMapDialog()->GetWindowForID( MNM_LEVELUP_BTN );

			if( pLevelUpButton )
			{
				if( heroInfo->LevelUpPoint >= MINIMUM_LEVELUP_POINT )
					GAMEIN->GetCharacterDialog()->SetActive( TRUE );
				
				if( heroInfo->SkillPoint >= MINIMUM_SKILL_POINT )
					GAMEIN->GetSkillTreeDlg()->SetActive( TRUE );		

				pLevelUpButton->SetPush( TRUE );
				pLevelUpButton->SetDisable( TRUE );
			}		
		}
		break;
	case MNM_CHANNEL_COMBOBOX:
		{
			if( we == WE_PUSHDOWN )
				GAMEIN->GetMiniMapDialog()->RequestChannelInfo();

			if( we != WE_COMBOBOXSELECT )
				break;

			GAMEIN->GetMiniMapDialog()->ChangeChannel();
		}
		break;

	}
}

void ITD_DlgFunc(LONG id, void * p, DWORD we)
{
	// send event o CEnchantDialog
	cDialog* dialog = WINDOWMGR->GetWindowForID( ITD_UPGRADEDLG	 );
	ASSERT( dialog );

	dialog->OnActionEvent( id, p, we );
}

void ITMD_DlgFunc(LONG lId, void * p, DWORD we)
{
	// send event to CMixDialog
	cDialog* dialog = WINDOWMGR->GetWindowForID( ITMD_MIXDLG );
	ASSERT( dialog );

	dialog->OnActionEvent( lId, p, we );

}


void ITR_DlgFunc(LONG lId, void * p, DWORD we)
{
	// send event to CReinforceDlg
	cDialog* dialog = WINDOWMGR->GetWindowForID( ITR_REINFORCEDLG );
	ASSERT( dialog );

	dialog->OnActionEvent( lId, p, we );
}


void ApplyOptionDialogFunc( LONG lId, void * p, DWORD we )
{
	cDialog* dialog = WINDOWMGR->GetWindowForID( APPLY_OPTION_DIALOG );
	ASSERT( dialog );

	dialog->OnActionEvent( lId, p, we );
}

// 080916 LUJ, �ռ� â �̺�Ʈ �Լ�
void ComposeDialogFunc( LONG lId, void * p, DWORD we )
{
	cDialog* dialog = ( cDialog* )GAMEIN->GetComposeDialog();
	
	if( ! dialog )
	{
		return;
	}

	dialog->OnActionEvent( lId, p, we );
}

void CHA_DlgBtnFunc(LONG lId, void * p, DWORD we)
{
	switch(lId)
	{
	case CHA_CONNNECTOK:
		{
			CHARSELECT->GetChannelDialog()->OnConnect();	
		}
		break;
	case CHA_CONNNECTCANCEL:
		{
			CHARSELECT->GetChannelDialog()->SetActive(FALSE);
			
			if( CHARSELECT->GetChannelDialog()->GetState() == CChannelDialog::eState_CharSelect )
			{
				CHARSELECT->SetDisablePick(FALSE);
			}
		}
		break;
	}
}

void SL_DlgBtnFunc( LONG lId, void* p, DWORD we )
{
	CServerListDialog* pDlg = (CServerListDialog*)p;
	switch( lId )
	{
	case SL_BTN_CONNECT:
		TITLE->ConnectToServer( pDlg->GetSelectedIndex() );
		break;

	case SL_BTN_EXIT:
		//		MAINGAME->SetGameState( eGAMESTATE_END );
		PostMessage( _g_hWnd, WM_CLOSE, NULL, NULL );
		break;
	}
}

void QUE_QuestDlgFunc( LONG lId, void* p, DWORD we )
{
	CQuestDialog* pDlg = (CQuestDialog*)p;

	switch(lId)
	{
		//case QUE_JOURNALBTN:
		//	JOURNALMGR->GetListSyn();
		//	break;
	case QUE_ITEM_GRID:
		//		pDlg->GetPositionForXYRef();
		break;
	case QUE_PAGE1BTN:
		pDlg->RefreshQuestItem(0);
		break;
	case QUE_PAGE2BTN:
		pDlg->RefreshQuestItem(1);
		break;
	case QUE_PAGE3BTN:
		pDlg->RefreshQuestItem(2);
		break;
	case QUE_PAGE4BTN:
		pDlg->RefreshQuestItem(3);
		break;
	case QUE_PAGE5BTN:
		pDlg->RefreshQuestItem(4);
		break;

		// 070125 LYW --- Quest : Modified this line.
	case QUE_PAGE6BTN :
		{
			pDlg->RefreshQuestItem(5);
		}
		break ;

	case QUE_NOTICEBTN :
		{
			//QUE_RegistQuestNotice(TRUE);	// 080305 NYJ --- ����Ʈ�ڵ��˸��� ����� ���� �Ʒ��ڵ带 �Լ��� ó����. (���� ������ ����)
			/**/
			DWORD QuestIdx = 0 ;																			// ����Ʈ �ε����� ���� ������ �����ϰ� 0���� �����Ѵ�.

			CQuestDialog* pDlg = NULL ;																		// ����Ʈ ���̾�α� ������ ���� �����͸� �����ϰ� nulló���� �Ѵ�.
			pDlg = GAMEIN->GetQuestTotalDialog()->GetQuestDlg() ;											// ����Ʈ ���̾�α� ������ �޴´�.

			if( pDlg )																						// ����Ʈ ���̾�α� ������ ��ȿ�ϴٸ�,
			{
				QuestIdx = pDlg->GetSelectedQuestID() ;														// ���� �� ����Ʈ �ε����� �޴´�.
			}

			if( QuestIdx < 1 )																				// ����Ʈ �ε����� 1���� ������,
			{
				WINDOWMGR->MsgBox( MBI_QUEST_DELETE, MBT_OK, CHATMGR->GetChatMsg( 1301 ) );

				return ;																					// ���� ó���� �Ѵ�.
			}
			else
			{
				CQuestQuickViewDialog* pQuickDlg = GAMEIN->GetQuestQuickViewDialog() ;

				if( pQuickDlg )
				{
					pQuickDlg->RegistQuest(QuestIdx) ;
					// 100601 ONS ����Ʈ �˸��� ���â�� ������Ʈ�Ѵ�.
					pQuickDlg->ResetQuickViewDialog();

					CQuestDialog* pQuestDlg = GAMEIN->GetQuestDialog() ;

					if(pQuestDlg)
					{
						pQuestDlg->RefreshQuestList() ;	
					}
				}
			}
			/**/
		}
		break ;

	case QUE_GIVEUPBTN :
		{
			DWORD QuestIdx = 0 ;																			// ����Ʈ �ε����� ���� ������ �����ϰ� 0���� �����Ѵ�.

			CQuestDialog* pDlg = NULL ;																		// ����Ʈ ���̾�α� ������ ���� �����͸� �����ϰ� nulló���� �Ѵ�.
			pDlg = GAMEIN->GetQuestTotalDialog()->GetQuestDlg() ;											// ����Ʈ ���̾�α� ������ �޴´�.

			if( pDlg )																						// ����Ʈ ���̾�α� ������ ��ȿ�ϴٸ�,
			{
				QuestIdx = pDlg->GetSelectedQuestID() ;														// ���� �� ����Ʈ �ε����� �޴´�.
			}

			if( QuestIdx < 1 )																				// ����Ʈ �ε����� 1���� ������,
			{
				WINDOWMGR->MsgBox( MBI_QUEST_DELETE, MBT_OK, CHATMGR->GetChatMsg( 1302 ) );

				return ;																					// ���� ó���� �Ѵ�.
			}
			else
			{
				WINDOWMGR->MsgBox( MBI_QUESTGIVEUP, MBT_YESNO, CHATMGR->GetChatMsg( 233 ) );
			}
		}
		break ;
		// 100414 ONS �Ϲݺ���/���ú��� ��ưó���߰�
	case QUE_BTN_REQUITAL_FIX:
		{
			if( !pDlg ) return;
			pDlg->SetRequitalType( (BYTE)eQuestExecute_TakeItem );
			pDlg->SetActiveSelectRequital( FALSE );
		}
		break;
	case QUE_BTN_REQUITAL_SELECT:
		{
			if( !pDlg ) return;
			pDlg->SetRequitalType( (BYTE)eQuestExecute_TakeSelectItem );
			pDlg->SetActiveSelectRequital( TRUE );
		}
		break;
	}
}

void GDRANK_DlgFunc( LONG id, void* p, DWORD we )
{
	// send event to CGuildRankDialog
	cDialog* dialog = WINDOWMGR->GetWindowForID( GDR_RANKDLG );
	ASSERT( dialog );

	dialog->OnActionEvent( id, p, we );
}


void GDLEVEL_DlgFunc( LONG id, void* p, DWORD we )
{
	// send event to CGuildLevelUpDialog
	cDialog* dialog = WINDOWMGR->GetWindowForID( GD_LEVELUPDLG );
	ASSERT( dialog );

	dialog->OnActionEvent( id, p, we );
}


void GDMARK_DlgFunc( LONG id, void* p, DWORD we )
{
	// send event to CGuildMarkDialog
	cDialog* dialog = WINDOWMGR->GetWindowForID( GDM_MARKREGISTDLG );
	ASSERT( dialog );

	dialog->OnActionEvent( id, p, we );
}


void GD_WarehouseFunc( LONG id, void* p, DWORD we )
{
	// send event to CGuildWarehouseDialog
	cDialog* dialog = WINDOWMGR->GetWindowForID( GDW_WAREHOUSEDLG );
	ASSERT( dialog );

	dialog->OnActionEvent( id, p, we );
}


void GDCREATE_DlgFunc(LONG id, void* p, DWORD we )
{
	// send event to CGuildCreateDialog
	cDialog* dialog = WINDOWMGR->GetWindowForID( GD_CREATEDLG );
	ASSERT( dialog );

	dialog->OnActionEvent( id, p, we );
}


void GDINVITE_DlgFunc( LONG id, void* p, DWORD we )
{
	// send event to CGuildInviteDialog
	cDialog* dialog = WINDOWMGR->GetWindowForID( GD_INVITEDlG );
	ASSERT( dialog );

	dialog->OnActionEvent( id, p, we );
}


void GDWHRANK_DlgFunc( LONG id, void* p, DWORD we )
{
	// send event to CGuildWarehouseRankDialog
	cDialog* dialog = WINDOWMGR->GetWindowForID( GDWHR_RANKDLG );
	ASSERT( dialog );

	dialog->OnActionEvent( id, p, we );
}


void GDREVIVAL_DlgFunc( LONG id, void* p, DWORD we )
{
	// send evenCGuildRevivalDialog
	cDialog* dialog = WINDOWMGR->GetWindowForID( GD_REVIVALDLG );
	ASSERT( dialog );

	dialog->OnActionEvent( id, p, we );
}


void GDUnion_DlgFunc( LONG id, void* p, DWORD we )
{
	// send event to CGuildUnionCreateDialog
	cDialog* dialog = WINDOWMGR->GetWindowForID( GDU_CREATEDLG );
	ASSERT( dialog );

	dialog->OnActionEvent( id, p, we );
}


void GDNICK_DlgFunc( LONG id, void* p, DWORD we )
{
	// send event to CGuildUnionCreateDialog
	cDialog* dialog = WINDOWMGR->GetWindowForID( GD_NICKNAMEDLG );
	ASSERT( dialog );

	dialog->OnActionEvent( id, p, we );
}


void GD_DlgFunc( LONG id, void* p, DWORD we )
{
	// send event to cGuildDialog
	cDialog* dialog = WINDOWMGR->GetWindowForID( GD_GUILDDLG );
	ASSERT( dialog );

	dialog->OnActionEvent( id, p, we );
}


void FD_DlgFunc( LONG lId, void* p, DWORD we )
{
	switch(lId)
	{
	case FDM_REGISTOKBTN:
		{
			cEditBox* pMarkName = (cEditBox*)WINDOWMGR->GetWindowForIDEx(FDM_NAMEEDIT);

			if(strcmp(pMarkName->GetEditText(), "") == 0)
				break;

			if (g_csFamilyManager.CLI_RequestRegistEmblem(pMarkName->GetEditText()))
			{
				GAMEIN->GetFamilyMarkDlg()->SetActive(FALSE);			
			}
			else
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(549));
			}
		}
		break;
	case FD_CCREATEOKBTN:
		{
			if(HERO->GetFamilyIdx())
			{
				// 100309 ONS �йи� �ߺ���û�� �޼��� ����
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(697));
				return;
			}
			cEditBox * pFamilyName = (cEditBox *)WINDOWMGR->GetWindowForIDEx(FD_CNAME);
			if(strcmp(pFamilyName->GetEditText(), "") == 0)
				return;

			g_csFamilyManager.CLI_RequestCreateFamily(pFamilyName->GetEditText());
		}
		break;
	case FD_GIVEMEMBERNICK:
		{
			GAMEIN->GetFamilyNickNameDlg()->SetActive(TRUE);
		}
		break;
	case FD_NREGISTOKBTN:
		{
			CFamilyDialog* const pDlg = GAMEIN->GetFamilyDlg();

			if(0 == pDlg)
			{
				break;
			}
			else if(pDlg->GetSelectedMemberIdx() < 0)
			{
				break;
			}

			CFamilyNickNameDialog* const pChangeDlg = GAMEIN->GetFamilyNickNameDlg();

			if(0 == pChangeDlg)
			{
				break;
			}

			CSHFamilyMember* const pMember = HERO->GetFamily()->GetMember(
				pDlg->GetSelectedMemberIdx());

			if(0 == pMember)
			{
				break;
			}
			else if(cEditBox* pEditBox = pChangeDlg->GetEditBox())
			{
				g_csFamilyManager.CLI_RequestChangeNickname(
					pMember->Get()->nID,
					pEditBox->GetEditText());
			}

			pChangeDlg->SetActive(
				FALSE);
		}
		break;
	}
}

// desc_hseos_����ý���_01
// S ����ý��� �߰� added by hseos 2007.08.24	2007.09.10	2007.10.23
void FARM_BUY_DlgFunc( LONG lId, void* p, DWORD we )
{
	switch(lId)
	{
	case FARM_BUY_OKBTN:
		{
			g_csFarmManager.CLI_RequestBuyFarmToSrv();
		}
		break;
	case FARM_BUY_CANCELBTN:
		{
			GAMEIN->GetFarmBuyDlg()->SetActive(FALSE);
		}
		break;
	}
}

void FARM_UPGRADE_DlgFunc( LONG lId, void* p, DWORD we )
{
	switch(lId)
	{
	case FARM_UPGRADE_OKBTN:
		{
			g_csFarmManager.CLI_RequestUpgradeFarm((CSHFarmZone::FARM_ZONE)GAMEIN->GetFarmUpgradeDlg()->GetFarmZone(),
													GAMEIN->GetFarmUpgradeDlg()->GetFarmID(),
													GAMEIN->GetFarmUpgradeDlg()->GetFarmUpgradeKind());
		}
		break;
	case FARM_UPGRADE_CANCELBTN:
		{
			GAMEIN->GetFarmUpgradeDlg()->SetActive(FALSE);
		}
		break;
	}
}

void FARM_MANAGE_DlgFunc( LONG lId, void* p, DWORD we )
{
	GAMEIN->GetFarmManageDlg()->OnActionEvent(lId, p, we);
}
// E ����ý��� �߰� added by hseos 2007.08.24	2007.09.10	2007.10.23

// desc_hseos_����Ʈ ��_01
// S ����Ʈ �� �߰� added by hseos 2007.11.14  2007.11.15	2008.01.24
void DATE_ZONE_LIST_DlgFunc( LONG lId, void* p, DWORD we )
{
	GAMEIN->GetDateZoneListDlg()->OnActionEvent(lId, p, we);
}

void CHALLENGE_ZONE_LIST_DlgFunc( LONG lId, void* p, DWORD we )
{
	GAMEIN->GetChallengeZoneListDlg()->OnActionEvent(lId, p, we);
}

void CHALLENGE_ZONE_CLEARNO1_DlgFunc( LONG lId, void* p, DWORD we )
{
	GAMEIN->GetChallengeZoneClearNo1Dlg()->OnActionEvent(lId, p, we);
}
// E ����Ʈ �� �߰� added by hseos 2007.11.14  2007.11.15	2008.01.24

// guildfieldwar
void GFW_DlgFunc( LONG lId, void* p, DWORD we )
{
	switch( lId )
	{
		// declare
	case GFW_DECLARE_OKBTN:
		{
			cEditBox* pName = (cEditBox*)WINDOWMGR->GetWindowForIDEx( GFW_DECLARE_EDITBOX );
			cEditBox* pMoney = (cEditBox*)WINDOWMGR->GetWindowForIDEx( GFW_MONEY_EDITBOX );
			GUILDWARMGR->SendDeclare( pName->GetEditText(), RemoveComma(pMoney->GetEditText()) );
		}
		break;
	case GFW_DECLARE_CANCELBTN:
		{
			GAMEIN->GetGFWarDeclareDlg()->SetActive( FALSE );
		}
		break;
	case GFW_MONEY_CHK:
		{
			GAMEIN->GetGFWarDeclareDlg()->ShowMoneyEdit();
		}
		break;

		// result
	case GFW_RESULT_OKBTN:
		{
			if( GUILDWARMGR->IsValid( 4 ) )
			{
				GAMEIN->GetGFWarResultDlg()->SetDisable( TRUE );
				WINDOWMGR->MsgBox( MBI_GUILDFIELDWAR_DECLARE_ACCEPT, MBT_YESNO, CHATMGR->GetChatMsg( 957 ) );
			}
		}
		break;
	case GFW_RESULT_CANCELBTN:
		{
			GAMEIN->GetGFWarResultDlg()->SetDisable( TRUE );
			WINDOWMGR->MsgBox( MBI_GUILDFIELDWAR_DECLARE_DENY, MBT_YESNO, CHATMGR->GetChatMsg( 958 ) );
		}
		break;
	case GFW_RESULT_CONFIRMBTN:
		{
			GAMEIN->GetGFWarResultDlg()->SetActive( FALSE );
		}
		break;
	// ���� ����
	case GFW_INFO_SUGGEST_OKBTN:
		{
			CGFWarInfoDlg* dialog = GAMEIN->GetGFWarInfoDlg();
			ASSERT( dialog );

			const GUILDINFO* pInfo = GUILDWARMGR->GetEnemyFromListIndex( GAMEIN->GetGFWarInfoDlg()->GetSelectedListIdx() );

			if( ! pInfo )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 895 ) );
				break;
			}
			else if( 	pInfo->UnionIdx &&
					!	pInfo->mIsUnionMaster )
			{
				const MSG_GUILD_LIST::Data* data = GUILDWARMGR->GetUnionMasterEnemy( pInfo->UnionIdx );

				if( data )
				{
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1270 ), data->GuildName );
				}

				break;
			}

			WINDOWMGR->MsgBox( MBI_GUILDFIELDWAR_SUGGEST, MBT_YESNO, CHATMGR->GetChatMsg( 983 ), pInfo->GuildName );
			dialog->SetDisable( TRUE );
		}
		break;
	// �׺� ����
	case GFW_INFO_SURREND_OKBTN:
		{
			CGFWarInfoDlg* dialog = GAMEIN->GetGFWarInfoDlg();
			ASSERT( dialog );

			const GUILDINFO* pInfo = GUILDWARMGR->GetEnemyFromListIndex( GAMEIN->GetGFWarInfoDlg()->GetSelectedListIdx() );

			if( ! pInfo )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 894 ) );
				break;
			}
			else if( 	pInfo->UnionIdx &&
					!	pInfo->mIsUnionMaster )
			{
				const MSG_GUILD_LIST::Data* data = GUILDWARMGR->GetUnionMasterEnemy( pInfo->UnionIdx );

				if( data )
				{
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1270 ), data->GuildName );
				}
				
				break;
			}

			WINDOWMGR->MsgBox( MBI_GUILDFIELDWAR_SURREND, MBT_YESNO, CHATMGR->GetChatMsg( 984 ), pInfo->GuildName );
			dialog->SetDisable( TRUE );
		}
		break;
	case GFW_GDU_REMOVE_OKBTN:	// guildunion remove
		{
			const char* pName = GUILDUNION->GetNameFromListIndex( GAMEIN->GetGFWarInfoDlg()->GetSelectedListIdx() );
			if( pName )
			{
				WINDOWMGR->MsgBox( MBI_UNION_REMOVE, MBT_YESNO, CHATMGR->GetChatMsg( 985 ), pName );
			}
			// 080128 LYW --- GlobalEventFunc : [MPȸ��] ��� ����.
			/*else
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 847 ) );
			}*/
		}
		break;
	case GFW_INFO_CANCELBTN:
		{
			GAMEIN->GetGFWarInfoDlg()->SetActive( FALSE );
		}
		break;

		// warinfo
	case GW_INFOCLOSEBTN:
		{
			GAMEIN->GetGuildWarInfoDlg()->SetActive( FALSE );
		}
		break;
	}
}

void GDT_DlgFunc(LONG lId, void * p, DWORD we)
{
	switch( lId )
	{
	case GTENTRYEDIT_SYN:
	case GTENTRYCHANGE:
	// 090824 ONS ��Ʈ�� ��ϴ��̾�α׿� X��ư ��� �߰�.
	case GTENTRYEDIT_CLOSE:
		{
			cDialog* dialog = WINDOWMGR->GetWindowForID( GTENTRYEDITDLG );
			ASSERT( dialog );

			dialog->OnActionEvent( lId, p, we );
		}
		break;

	case GT_RESULTOK:
		{
			cDialog* dialog = WINDOWMGR->GetWindowForID( GTRESULTDLG );
			ASSERT( dialog );

			dialog->SetActive(FALSE);
		}
		break;
	}
}

void CG_DlgFunc(LONG lId, void *p, DWORD we)
{
	GAMEIN->GetGuageDlg()->OnActionEvent(lId, p, we);
}

// 06. 02. ��ȭ ��� �ȳ� �������̽� �߰� - �̿���
void RFGUIDE_DLGFunc(LONG lId, void *p, DWORD we)
{
	GAMEIN->GetReinforceGuideDlg()->OnActionEvent(lId, p, we);
}

void GN_DlgBtnFunc(LONG id, void *p, DWORD we)
{
	//GAMEIN->GetGuildNoteDlg()->OnActionEvnet( id, p, we);

	// send event to CGuildNoteDlg
	cDialog* dialog = WINDOWMGR->GetWindowForID( GUILDNOTE_DLG );
	ASSERT( dialog );

	dialog->OnActionEvent( id, p, we );
}

void AN_DlgBtnFunc(LONG id, void *p, DWORD we)
{
	//GAMEIN->GetUnionNoteDlg()->OnActionEvnet(id, p, we);

	// send event to CUnionNoteDlg
	cDialog* dialog = WINDOWMGR->GetWindowForID( ALLYNOTE_DLG );
	ASSERT( dialog );

	dialog->OnActionEvent( id, p, we );
}

// 06. 03. ���İ��� - �̿���
void GNotice_DlgBtnFunc(LONG id, void *p, DWORD we)
{
	//GAMEIN->GetGuildNoticeDlg()->OnActionEvnet(id, p, we);

	// send to event CGuildNoticeDlg
	cDialog* dialog = WINDOWMGR->GetWindowForID( GUILDNOTICE_DLG );
	ASSERT( dialog );

	dialog->OnActionEvent( id, p, we );
}

void GuildPlusTime_DlgFunc(LONG id, void* p, DWORD we)
{
	// GAMEIN->GetGuildPlusTimeDlg()->OnActionEvnet(id,p,we);

	cDialog* dialog = WINDOWMGR->GetWindowForID( GP_TIME );
	ASSERT( dialog );

	dialog->OnActionEvent( id, p, we );
}

// 061215 LYW --- Add function to call callback function.
void CM_CMNCbFunc( LONG lId, void* p, DWORD we)
{
	CHARMAKEMGR->GetCharMakeNewDlg()->OnActionEvent( lId, p, we ) ;
}

// 061219 LYW --- Add callback function to processing event from controls of main system dialog.
void MDS_CBFunc( LONG lId, void* p, DWORD we )
{
	GAMEIN->GetMainSystemDlg()->OnActionEvent( lId, p, we ) ;
}

void CharMain_BtnFunc( LONG lId, void* p, DWORD we )
{
	GAMEIN->GetGuageDlg()->OnActionEvent( lId, p, we ) ;
}

void JO_DlgFunc(LONG lId, void* p, DWORD we)
{
	cDialog* dialog = WINDOWMGR->GetWindowForID( JOIN_OPTIONDLG );
	ASSERT( dialog );

	dialog->OnActionEvent( lId, p, we );
}


// Date Matching Dialog Procedure
// 2007/03/15	������

void DMD_DateMatchingDlgFunc(LONG lId, void* p, DWORD we)
{
	GAMEIN->GetDateMatchingDlg()->OnActionEvent(lId, p, we);
	_asm nop;
}

// 070607 LYW --- GlovalEventFunc : Add function to call callback function of partner idalog.
void DMD_PartnerDlgFunc( LONG lId, void* p, DWORD we )
{
	GAMEIN->GetDateMatchingDlg()->GetPartnerListDlg()->OnActionEvent(lId, p, we) ;
}

// 070606 LYW --- GlovalEventFunc : Add function to call callback function of identification dialog.
void DMD_IDT_Func(LONG lId, void* p, DWORD we)
{
	GAMEIN->GetIdentificationDlg()->OnActionEvent(lId, p, we) ;
	_asm nop ;
}

// 070607 LYW --- GlovalEventFunc : Add function to call callback function of favor icon dialog.
void DMD_FID_Func(LONG lId, void* p, DWORD we)
{
	GAMEIN->GetFavorIconDlg()->OnActionEvent(lId, p, we) ;
	_asm nop ;
}

// 070607 LYW --- GlovalEventFunc : Add function to call callback function of my info dialog.
void DMD_MID_Func(LONG lId, void* p, DWORD we)
{
	CDateMatchingInfoDlg* pDlg = GAMEIN->GetMatchMyInfoDlg() ;

	if( !pDlg ) return ;

	switch(lId)
	{
	case DMD_CHANGE_MY_INTRODUCTION_BTN :
		{
			cTextArea* pTextArea = pDlg->GetIntroduce() ;
			if( pTextArea )
			{
				pTextArea->SetReadOnly(FALSE);
				pTextArea->SetFocusEdit(TRUE);
			}
		}
		break;
	case DMD_SAVE_MY_INTRODUCTION_BTN :
		{
			cTextArea* pTextArea = pDlg->GetIntroduce() ;

			if( pTextArea )
			{
				// check text.
				char tempBuf[MAX_INTRODUCE_LEN+1] = {0, } ;
				pTextArea->GetScriptText(tempBuf) ;

				g_csResidentRegistManager.CLI_RequestUpdateIntroduction(tempBuf) ;
			}
		}
		break;
	}
}

// 070607 LYW --- GlovalEventFunc : Add function to call callback function of record info dialog.
void DMD_RID_Func(LONG lId, void* p, DWORD we)
{
	GAMEIN->GetMatchRecordDlg()->OnActionEvent(lId, p, we) ;
}

// 070618 LYW --- GlovalEventFunc : Add function to call callback function of key setting dialog.
void KS_Func(LONG lId, void* p, DWORD we)
{
	GAMEIN->GetKeySettingTipDlg()->OnActionEvent(lId, p, we) ;
}

void SkillTrainDlgFunc( LONG lId, void* p, DWORD we )
{
	GAMEIN->GetSkillTrainingDlg()->OnActionEvent(lId, p, we);
}

// desc_hseos_�йи�01
// S �йи� �߰� added by hseos 2007.07.02
void FD_GuildDlg(LONG lId, void* p, DWORD we)
{
	GAMEIN->GetFamilyDlg()->OnActionEvent(lId, p, we);
}
// E �йи� �߰� added by hseos 2007.07.02


void PROGRESS_DlgFunc( LONG id, void* p, DWORD we )
{
	cDialog* dialog = WINDOWMGR->GetWindowForID( PROGRESS_DIALOG );
	ASSERT( dialog );

	dialog->OnActionEvent( id, p, we );
}


// 071018 LYW --- GlovalEventFunc : Add event function for tutorial dialog.
void TTR_DlgFunc( LONG id, void* p, DWORD we )
{
	cDialog* dialog = WINDOWMGR->GetWindowForID( TUTORIAL_DLG );
	ASSERT( dialog );

	if( we != WE_BTNCLICK ) return ;

	dialog->OnActionEvent( id, p, we );
}


// 071023 LYW --- GlovalEventFunc : Add event function for tutorial button dialog.
void TTR_BTNDlgFunc( LONG id, void* p, DWORD we )
{
	cDialog* dialog = WINDOWMGR->GetWindowForID( TUTORIALBTN_DLG ) ;
	ASSERT( dialog ) ;

	if( we != WE_BTNCLICK ) return ;

	dialog->OnActionEvent( id, p, we ) ;
}

// 071024 LYW --- GlovalEventFunc : Add event function for helper dialog.
void HD_DlgFunc(LONG lId, void* p, DWORD we )
{
	cDialog* dialog = WINDOWMGR->GetWindowForID( HELP_DLG ) ;
	ASSERT( dialog ) ;

	if( we & WE_CHECKED )													// üũ �ڽ� üũ �̺�Ʈ�� �߻��ߴٸ�,
	{
		if( lId != HD_SHOWTUTORIAL ) return ;								// üũ �ڽ� ���̵�� ��ġ���� ������, return ó���� �Ѵ�.

		TUTORIALMGR->Set_ActiveTutorial(TRUE) ;								// Ʃ�丮���� Ȱ��ȭ ���·� �����Ѵ�.
		TUTORIALMGR->Get_TutorialBtnDlg()->SetActive(TRUE) ;				// Ʃ�丮�� ��ư�� Ȱ��ȭ �Ѵ�.

		OPTIONMGR->Set_ShowTutorial(TRUE) ;									// Ʃ�丮�� ���̱⸦ true�� �����Ѵ�.
		OPTIONMGR->SaveGameOption();
	}
	else if( we & WE_NOTCHECKED )											// üũ �ڽ� üũ Ǯ�� �̺�Ʈ�� �߻��ߴٸ�,
	{
		if( lId != HD_SHOWTUTORIAL ) return ;								// üũ �ڽ� ���̵�� ��ġ���� ������, return ó���� �Ѵ�.

		TUTORIALMGR->Set_ActiveTutorial(FALSE) ;							// Ʃ�丮���� ��Ȱ��ȭ ���·� �����Ѵ�.
		TUTORIALMGR->Get_TutorialBtnDlg()->SetActive(FALSE) ;				// Ʃ�丮�� ��ư�� ��Ȱ��ȭ �Ѵ�.

		OPTIONMGR->Set_ShowTutorial(FALSE) ;								// Ʃ�丮�� ���̱⸦ false�� �����Ѵ�.
		OPTIONMGR->SaveGameOption();
	}

	if( we & WE_BTNCLICK ) 
	{
		dialog->OnActionEvent( lId, p, we ) ;
	}
}





// 071130 LYW --- GlovalEventFunc : Add event function for map move dialog.
void MapMove_DlgFunc(LONG lId, void* p, DWORD we )
{
	cDialog* dialog = WINDOWMGR->GetWindowForID( MAPMOVE_DLG ) ;
	ASSERT( dialog ) ;

	if( we & WE_BTNCLICK ) 
	{
		dialog->OnActionEvent( lId, p, we ) ;
	}
}





//=========================================================================
//	NAME : Change_Name_Func
//	DESC : Add event function for change name dialog. 071228
//=========================================================================
void Change_Name_Func(LONG lId, void* p, DWORD we )
{
	cDialog* dialog = WINDOWMGR->GetWindowForID( CHANGENAME_DLG ) ;
	ASSERT( dialog ) ;

	if( we & WE_BTNCLICK ) 
	{
		dialog->OnActionEvent( lId, p, we ) ;
	}
}


//=========================================================================
//	NAME : NumberPad_DlgFunc
//	DESC : Add event function for number pad dialog. 080109 LYW
//=========================================================================
void NumberPad_DlgFunc(LONG lId, void* p, DWORD we )
{
	cDialog* pDlg = NULL ;
	pDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG ) ;

	ASSERT( pDlg ) ;

	if( we & WE_BTNCLICK )
	{
		pDlg->OnActionEvent( lId, p, we ) ;
	}
}


//---KES AUTONOTE
void AutoNoteDlg_Func( LONG lId, void* p, DWORD we )
{
	if( GAMEIN->GetAutoNoteDlg() )
		GAMEIN->GetAutoNoteDlg()->OnActionEvent(lId, p, we);
}

void AutoAnswerDlg_Func( LONG lId, void* p, DWORD we )
{
	if( GAMEIN->GetAutoAnswerDlg() )
		GAMEIN->GetAutoAnswerDlg()->OnActionEvent(lId, p, we);
}
//---------------

//---KES �����˻�
void StoreSearchDlg_Func( LONG lId, void* p, DWORD we )
{
	if( GAMEIN->GetStoreSearchDlg() )
		GAMEIN->GetStoreSearchDlg()->OnActionEvent(lId, p, we);
}
//---------------

// 080414 LUJ, �ܾ� ���� â
void BodyChangeFunc( LONG lId, void* p, DWORD we )
{
	cDialog* dialog = WINDOWMGR->GetWindowForID( BODY_CHANGE_DIALOG );

	if( dialog )
	{
		dialog->OnActionEvent( lId, p, we );
	}
}


// 080403 LYW --- GlovalEventFunc : Add event functions for chatroom system.
void ChatRoomMainDlg_Func(LONG lId, void* p, DWORD we )
{
    ASSERT(p) ;

	if(!p)
	{
		MessageBox( NULL, "ERR-IMP", "CRMDF", MB_OK) ;
		return ;
	}

	CChatRoomMainDlg* pDlg = NULL ;
 	pDlg = GAMEIN->GetChatRoomMainDlg() ;

	ASSERT(pDlg) ;

 	if(!pDlg)
 	{
 		MessageBox(	NULL, "ERR-FRD", "CRMDF", MB_OK) ;
 		return ;
 	}
 
 	pDlg->OnActionEvent(lId, p, we) ;
 }

//=========================================================================
//	NAME : ChatRoomCreateDlg_Func - 080312 : LYW
//	DESC : The functino to pass event to create dialog.
//=========================================================================
void ChatRoomCreateDlg_Func(LONG lId, void* p, DWORD we )
{
	ASSERT(p) ;

	if(!p)
	{
		MessageBox( NULL, "ERR-IMP", "CRCDF", MB_OK) ;
		return ;
	}

	CChatRoomCreateDlg* pDlg = NULL ;
	pDlg = GAMEIN->GetChatRoomCreateDlg() ;											// Receive create dialog.

	ASSERT(pDlg) ;

 	if(!pDlg)
 	{
 		MessageBox(	NULL, "ERR-FRD", "CRCDF", MB_OK) ;
 		return ;
 	}

	pDlg->OnActionEvent(lId, p, we) ;												// Pass event to create dialog.
}





//=========================================================================
//	NAME : ChatRoomDlg_Func - 080312 : LYW
//	DESC : The functino to pass event to chatting dialog.
//=========================================================================
void ChatRoomDlg_Func(LONG lId, void* p, DWORD we )
{
	ASSERT(p) ;

	if(!p)
	{
		MessageBox( NULL, "ERR-IMP", "CRDF", MB_OK) ;
		return ;
	}

	CChatRoomDlg* pDlg = NULL ;
	pDlg = GAMEIN->GetChatRoomDlg() ;												// Receive chatring dialog.

	ASSERT(pDlg) ;

 	if(!pDlg)
 	{
 		MessageBox(	NULL, "ERR-FRD", "CRDF", MB_OK) ;
 		return ;
 	}

	pDlg->OnActionEvent(lId, p, we) ;												// Pass event to chatring dialog.
}





//=========================================================================
//	NAME : ChatRoomGuestListDlg_Func - 080312 : LYW
//	DESC : The functino to pass event to guest list dialog.
//=========================================================================
void ChatRoomGuestListDlg_Func(LONG lId, void* p, DWORD we ) 
{
	ASSERT(p) ;

	if(!p)
	{
		MessageBox( NULL, "ERR-IMP", "CRGLDF", MB_OK) ;
		return ;
	}

	CChatRoomGuestListDlg* pDlg = NULL ;
	pDlg = GAMEIN->GetChatRoomGuestListDlg() ;											// Receive guestList dialog.

	ASSERT(pDlg) ;

 	if(!pDlg)
 	{
 		MessageBox(	NULL, "ERR-FRD", "CRGLDF", MB_OK) ;
 		return ;
 	}

	pDlg->OnActionEvent(lId, p, we) ;												// Pass event to guestList dialog.
}





//=========================================================================
//	NAME : ChatRoomJoinDlg_Func - 080312 : LYW
//	DESC : The functino to pass event to join dialog.
//=========================================================================
void ChatRoomJoinDlg_Func(LONG lId, void* p, DWORD we ) 
{
	ASSERT(p) ;

	if(!p)
	{
		MessageBox( NULL, "ERR-IMP", "CRJDF", MB_OK) ;
		return ;
	}

	CChatRoomJoinDlg* pDlg = NULL ;
	pDlg = GAMEIN->GetChatRoomJoinDlg() ;											// Receive join dialog.

	ASSERT(pDlg) ;

 	if(!pDlg)
 	{
 		MessageBox(	NULL, "ERR-FRD", "CRJDF", MB_OK) ;
 		return ;
 	}

	pDlg->OnActionEvent(lId, p, we) ;												// Pass event to join dialog.
}





//=========================================================================
//	NAME : ChatRoomOptionDlg_Func - 080312 : LYW
//	DESC : The functino to pass event to option dialog.
//=========================================================================
void ChatRoomOptionDlg_Func(LONG lId, void* p, DWORD we ) 
{
	ASSERT(p) ;

	if(!p)
	{
		MessageBox( NULL, "ERR-IMP", "CRODF", MB_OK) ;
		return ;
	}

	CChatRoomOptionDlg* pDlg = NULL ;
	pDlg = GAMEIN->GetChatRoomOptionDlg() ;											// Receive option dialog.

	ASSERT(pDlg) ;

 	if(!pDlg)
 	{
 		MessageBox(	NULL, "ERR-FRD", "CRODF", MB_OK) ;
 		return ;
 	}

	pDlg->OnActionEvent(lId, p, we) ;												// Pass event to option dialog.
}

void Animal_DlgBtnFunc(LONG lId, void* p, DWORD we )
{
	ASSERT(p);

	if(!p)
	{
		MessageBox(NULL, "ERR-IMP", "CRLDF", MB_OK);
		return;
	}
	CAnimalDialog* pDlg = NULL;
	pDlg = GAMEIN->GetAnimalDialog();

	ASSERT(pDlg);

	if(!pDlg)
	{
		MessageBox( NULL, "ERR-FRD", "CRLDF", MB_OK );
		return;
	}

	GAMEIN->GetAnimalDialog()->OnActionEvent( lId, p, we );
	//pDlg->OnActionEvent( lId, p, we);
}

void PET_STATUS_DLG_FUNC(LONG lId, void* p, DWORD we )
{
	GAMEIN->GetPetStateDlg()->OnActionEvent(lId, p, we);
}

void PET_RES_DLG_FUNC(LONG lId, void* p, DWORD we )
{
	GAMEIN->GetPetResurrectionDlg()->OnActionEvent(lId, p, we);
}

void PopupMenuDlgFunc(LONG lId, void* p, DWORD we )
{
	GAMEIN->GetPopupMenuDlg()->OnActionEvent(lId, p, we);
}

void COOK_DLG_FUNC(LONG lId, void* p, DWORD we )
{
	GAMEIN->GetCookDlg()->OnActionEvent(lId, p, we);
}

// 090422 ShinJS --- Ż���� ���� �˾� ���̾�α� �߰��۾�
void RidePopupMenuDlgFunc(LONG lId, void* p, DWORD we )
{
	GAMEIN->GetRidePopupMenuDlg()->OnActionEvent(lId, p, we);
}

//090323 pdy �Ͽ�¡ ��ġ UI�߰�
void HousingRevolDlgFunc(LONG lId, void* p, DWORD we )
{
	GAMEIN->GetHousingRevolDlg()->OnActionEvent(lId, p, we);
}

//090330 pdy �Ͽ�¡ â�� UI�߰�
void Housing_WarehouseDlg_Func(LONG lId, void* p, DWORD we ) 
{
	GAMEIN->GetHousingWarehouseDlg()->OnActionEvent(lId, p, we);
}

//090409 pdy �Ͽ�¡ �Ͽ콺 �˻� UI�߰�
void HouseSearchDlgFunc(LONG lId, void* p, DWORD we )
{
	GAMEIN->GetHouseSearchDlg()->OnActionEvent(lId, p, we);
}

//090410 pdy �Ͽ�¡ �Ͽ콺 �̸����� UI�߰�
void HouseNameDlgFunc(LONG lId, void* p, DWORD we )
{
	GAMEIN->GetHouseNameDlg()->OnActionEvent(lId, p, we);
}

//090414 pdy �Ͽ�¡ �ٹ̱�����Ʈ ���� UI�߰�
void HousingDecoPointDlgFunc(LONG lId, void* p, DWORD we )
{
	GAMEIN->GetHousingDecoPointDlg()->OnActionEvent(lId, p, we);
}

//090507 pdy �Ͽ�¡ �׼� �˾��Ŵ� UI�߰�
void HousingActionPopupMenuDlgFunc(LONG lId, void* p, DWORD we )
{
	GAMEIN->GetHousingActionPopupMenuDlg()->OnActionEvent(lId, p, we);
}

//090525 pdy �Ͽ�¡ ����â�� ��ư UI�߰�
void HousingWarehouseButtonDlgFunc(LONG lId, void* p, DWORD we )
{
	GAMEIN->GetHousingWarehouseButtonDlg()->OnActionEvent(lId, p, we);
}

// 090525 ShinJS --- Party �ٸ��ʿ� �ִ� ������ ���� �ʴ�� �̸��� �Է��ϴ� Dialog �߰�
void InputNameDlgFunc(LONG lId, void* p, DWORD we )
{
	GAMEIN->GetInputNameDlg()->OnActionEvent(lId, p, we);
}

//090708 pdy �Ͽ�¡ �ٹ̱��� ��ư UI�߰�
void HousingDecoModeBtDlgFunc(LONG lId, void* p, DWORD we )
{
	GAMEIN->GetHousingDecoModeBtDlg()->OnActionEvent(lId, p, we);
}

//091012 pdy �Ͽ�¡ �ٹ̱� ����Ʈ ��ư ��ȹ����
void HousingMainPointDlgFunc(LONG lId, void* p, DWORD we )
{
	GAMEIN->GetHousingMainPointDlg()->OnActionEvent(lId, p, we);
}

//091223 NYJ �Ͽ�¡ �ٸ����湮
void HousingSearchDlgFunc(LONG lId, void* p, DWORD we ) 
{
	GAMEIN->GetHouseSearchDlg()->OnActionEvent(lId, p, we);
}

// 091210 ShinJS --- ��ȭ Dialog �߰�
void VideoCaptureDlgFunc(LONG lId, void* p, DWORD we )
{
	cDialog* pVideoCaptureDlg = (cDialog*)p;
	if( !pVideoCaptureDlg )
		return;

	switch( lId )
	{
		// ��ȭ ����
	case VIDEOCAPTURE_START_BTN:
		{
			pVideoCaptureDlg->SetActive( FALSE );

			CMiniMapDlg* pMiniMapDlg = GAMEIN->GetMiniMapDialog();
			if( !pMiniMapDlg )
				break;

			pMiniMapDlg->ShowVideoCaptureStartBtn( FALSE );

			VIDEOCAPTUREMGR->CaptureStart();

			// 100111 ONS �ش�ʿ� �̴ϸ��� ������� ȭ�� �»�ܿ� [��ȭ��]�̹����� ����Ѵ�.
			if( !pMiniMapDlg->HaveMinimap() )
			{
				cAdditionalButtonDlg* pAdditionBtnDlg = GAMEIN->GetAdditionalButtonDlg();
				if( !pAdditionBtnDlg )
					break;

				pAdditionBtnDlg->SetActive(TRUE);
				pAdditionBtnDlg->ShowRecordStartBtn( FALSE );
			}
		}
		break;
		// ��ȭ ���
	case VIDEOCAPTURE_CANCEL_BTN:
		{
			pVideoCaptureDlg->SetActive( FALSE );
		}
		break;
	}
}

// 100111 ONS �ΰ����� ��ư ���̾�α� �߰�
void AdditionalButtonDlgFunc( LONG lId, void* p, DWORD we )
{
	if(cDialog* const dialog = WINDOWMGR->GetWindowForID(ADDITIONAL_BUTTON_DLG))
	{
		dialog->OnActionEvent(
			lId,
			p,
			we);
	}
}

// 100511 ONS �������� ���̾�α� �߰�
void ChangeClassDlgFunc( LONG lId, void* p, DWORD we )
{
	if(cDialog* const dialog = WINDOWMGR->GetWindowForID(CHANGE_CLASS_DLG))
	{
		dialog->OnActionEvent(
			lId,
			p,
			we);
	}
}

void ConsignmentDlgFunc(LONG lId, void * p, DWORD we)
{
	if(cDialog* const dialog = WINDOWMGR->GetWindowForID(CONSIGNMENT_DLG))
	{
		dialog->OnActionEvent(
			lId,
			p,
			we);
	}
}

// 100629 ���Ͱ����� ���̾˷α�
void MonsterTargetDlgFunc( LONG lId, void * p, DWORD we)
{
	if(CMonsterGuageDlg* const pMonsterGuageDlg = (CMonsterGuageDlg*)WINDOWMGR->GetWindowForID( CG_PLAYERGUAGEDLG ) )
	{
		if( cDialog*  pMonsterTargetDialog = pMonsterGuageDlg->GetCurTarget() )
		{
			pMonsterTargetDialog->OnActionEvent( 
				lId,
				p,
				we);
		}
	}
}

void ITTD_TransmogDlgFunc(LONG lId, void * p, DWORD we)
{
	cDialog* dialog = WINDOWMGR->GetWindowForID( ITTD_TRANSMOGDLG );
	ASSERT( dialog );

	dialog->OnActionEvent( lId, p, we );
}



// 100709 ONS ����Ʈ�˸��� ������ư ó�� �߰�
void QuickView_DlgFunc(LONG lId, void * p, DWORD we)
{
	if(cDialog* const dialog = WINDOWMGR->GetWindowForID(QUE_QUICKVIEWDLG))
	{
		dialog->OnActionEvent(
			lId,
			p,
			we);
	}
}

void WorldMapDlgFunc(LONG lId, void * p, DWORD we)
{
	if( cDialog* const dialog = WINDOWMGR->GetWindowForID( WORLDMAP_DLG ) )
	{
		dialog->OnActionEvent( lId, p, we );
	}
}

void BigMapDlgFunc(LONG lId, void * p, DWORD we)
{
	if( p == NULL )
		return;

	((cDialog*)p)->OnActionEvent( lId, p, we );
}