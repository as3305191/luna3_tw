#include "StdAfx.h"
#include "NpcScriptManager.h"
#include "ObjectStateManager.h"
#include "WindowIDEnum.h"
#include "interface/cWindowManager.h"
#include "MHMap.h"
#include "GameIn.h"
#include "cDialogueList.h"
#include "cHyperTextList.h"
#include "cMsgBox.h"
#include "cPage.h"
#include "Npc.h"
#include "DateMatchingDlg.h"
#include "GuildMarkDialog.h"
#include "InventoryExDialog.h"
#include "NpcScriptDialog.h"
#include "StorageDialog.h"
#include "GuildManager.h"
#include "MoveManager.h"
#include "ChatManager.h"
#include "ObjectManager.h"
#include "../hseos/Family/SHFamilyManager.h"
#include "NpcNoticeDlg.h"
#include "../[CC]Header/GameResourceStruct.h"
#include "MHFile.h"

CNpcScriptManager::CNpcScriptManager() :
m_pDialogue(new cDialogueList),
m_pHyperText(new cHyperTextList),
m_pCurSelectedNpc(0)
{
	LoadScript();
}

CNpcScriptManager::~CNpcScriptManager()													// �Ҹ��� �Լ�.
{
	for(NpcMap::iterator npcIterator = mNpcMap.begin();
		mNpcMap.end() != npcIterator;
		++npcIterator)
	{
		PageMap& pageMap = npcIterator->second;

		for(PageMap::iterator pageIterator = pageMap.begin();
			pageMap.end() != pageIterator;
			++pageIterator)
		{
			SAFE_DELETE(pageIterator->second);
		}
	}

	SAFE_DELETE(m_pDialogue);
	SAFE_DELETE(m_pHyperText);
}

CNpcScriptManager* CNpcScriptManager::GetInstance()
{
	static CNpcScriptManager instance;

	return &instance;
}

void CNpcScriptManager::AddScript(DWORD npcIndex, DWORD pageIndex, BYTE emotion)
{
	PageMap& pageMap = mNpcMap[npcIndex];
	
	if(pageMap.end() == pageMap.find(pageIndex))
	{
		if(pageMap.empty())
		{
			mMainPageMap.insert(
				std::make_pair(npcIndex, pageIndex));
		}

        pageMap.insert(
			std::make_pair(pageIndex, new cPage));
	}

	cPage* const page = pageMap[pageIndex];
	page->Init(emotion);
}

cPage* CNpcScriptManager::GetMainPage(DWORD npcIndex)
{
	MainPageMap::iterator iterator = mMainPageMap.find(npcIndex);

	if(mMainPageMap.end() == iterator)
	{
		return 0;
	}

	const DWORD	pageIndex = iterator->second;

	return GetPage(npcIndex, pageIndex);
}

cPage* CNpcScriptManager::GetPage( DWORD dwNpcId, DWORD dwPageId )
{
	NpcMap::iterator npcIterator = mNpcMap.find(dwNpcId);

	if(mNpcMap.end() == npcIterator)
	{
		return 0;
	}

	PageMap& pageMap = npcIterator->second;
	PageMap::iterator pageIterator = pageMap.find(dwPageId);

	return pageMap.end() == pageIterator ? 0 : pageIterator->second;
}

void CNpcScriptManager::LoadScript()
{
	CMHFile file;
	file.Init(
		"Data/Script/Npc/Npc_Script.bin",
		"rb");

	DWORD npcIndex = 0;
	DWORD pageIndex = 0;

	while(FALSE == file.IsEOF())
	{
		TCHAR buffer[MAX_PATH] = {0};
		file.GetLine(
			buffer,
			sizeof(buffer) / sizeof(*buffer));

		LPCTSTR seperator = " \t#$";
		LPCTSTR token = _tcstok(
			buffer,
			seperator);

		if(0 == token)
		{
			continue;
		}
		else if(0 == _tcsicmp("npcId", token))
		{
			LPCTSTR textIndex = _tcstok(0, seperator);
			npcIndex = _ttoi(textIndex ? textIndex : "");
		}
		else if(0 == _tcsicmp("pageInfo", token))
		{
			LPCTSTR textPageIndex = _tcstok(0, seperator);
			_tcstok(0, seperator);
			_tcstok(0, seperator);
			LPCTSTR textEmotion = _tcstok(0, seperator);

			pageIndex = _ttoi(textPageIndex ? textPageIndex : "");
			const BYTE emotion = BYTE(_ttoi(textEmotion ? textEmotion : ""));
			AddScript(
				npcIndex,
				pageIndex,
				emotion);
		}
		else if(0 == _tcsicmp("dialogue", token))
		{
			cPage* const page = GetPage(
				npcIndex,
				pageIndex);

			if(0 == page)
			{
				continue;
			}

			for(LPCTSTR textIndex = _tcstok(0, seperator);
				0 != textIndex;
				textIndex = _tcstok(0, seperator))
			{
				page->AddDialogue(_ttoi(textIndex ? textIndex : ""));
			}
		}
		else if(0 == _tcsicmp("hyperlink", token))
		{
			LPCTSTR textLinkIndex = _tcstok(0, seperator);
			LPCTSTR textLinkType = _tcstok(0, seperator);
			LPCTSTR textData = _tcstok(0, seperator);
			LPCTSTR textX = _tcstok(0, seperator);
			LPCTSTR textZ = _tcstok(0, seperator);

			HYPERLINK hyperLink = {0};
			hyperLink.wLinkId = WORD(_ttoi(textLinkIndex ? textLinkIndex : ""));
			hyperLink.wLinkType = LINKTYPE(_ttoi(textLinkType ? textLinkType : "") + 1);
			hyperLink.dwData = _ttoi(textData ? textData : "");
			hyperLink.fXpos = float(_tstof(textX ? textX : ""));
			hyperLink.fZpos = float(_tstof(textZ ? textZ : ""));

			cPage* const page = GetPage(
				npcIndex,
				pageIndex);

			if(page)
			{
				page->AddHyperLink(hyperLink);
			}
		}
	}
}

void CNpcScriptManager::StartNpcScript( CNpc* pNpc )									// NPC ��ũ��Ʈ�� �����ϴ� �Լ�.
{
	if(GetSelectedNpc())
	{
		GAMEIN->GetNpcScriptDialog()->EndDialog();
	}

	VECTOR3 pos = {0};
	HERO->GetPosition(&pos);
	MOVEMGR->SetLookatPos(
		pNpc,
		&pos,
		0,
		gCurTime);
	SetSelectedNpc(pNpc);

	// 090925 ShinJS --- Npc Notice Dlg �� ����ϴ� ���
	if( pNpc->IsUseNoticeDlg() )
	{
		cNpcNoticeDlg* pNpcNoticeDlg = (cNpcNoticeDlg*)WINDOWMGR->GetWindowForID( NND_NOTICEDLG );
		if( pNpcNoticeDlg &&
			pNpcNoticeDlg->OpenDialog( pNpc->GetNpcUniqueIdx(), pNpc->GetNpcListInfo()->ModelNum ) == FALSE )
		{
			if( HERO->GetState() == eObjectState_Deal )
				OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal);
		}

		return;
	}

	// 080402 NYJ --- ���̾�α׸� ���µ� ����������� ó��
	//GAMEIN->GetNpcScriptDialog()->OpenDialog( pNpc->GetNpcUniqueIdx(), pNpc->GetNpcListInfo()->ModelNum );	// ���̾�α׸� ����.
	if(!GAMEIN->GetNpcScriptDialog()->OpenDialog( pNpc->GetNpcUniqueIdx(), pNpc->GetNpcListInfo()->ModelNum ))
	{
		if(GAMEIN->GetNpcScriptDialog()->IsActive())
		{
			NPCSCRIPTMGR->SetSelectedNpc(NULL);
			GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);
		}

		if( HERO->GetState() == eObjectState_Deal )
			OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal);
	}
}

void CNpcScriptManager::StartNpcBusiness(DWORD dwData, DWORD dwParam)					// NPC �����Ͻ��� �����ϴ� �Լ�.
{
	if( m_pCurSelectedNpc == NULL && dwParam == 0 )										// ���� ���� �� NPC ������ ���ų�, �Ķ���� ���� 0�� ������,
	{
		ASSERT(0);																		// ASSERT üũ�� �Ѵ�.

		if( HERO->GetState() == eObjectState_Deal )										// HERO�� ���°� �ŷ� ���̸�,
		{
			OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal);					// �ŷ� ���¸� �����Ѵ�.
		}
		
		return;																			// ���� ó���� �Ѵ�.
	}

	if( m_pCurSelectedNpc )																// ���� ���� �� NPC ������ �ִٸ�,
	{
		WORD wJobKind = m_pCurSelectedNpc->GetNpcJob() ;								// NPC ������ �޴´�.

		switch( wJobKind )																// NPC ������ Ȯ���Ѵ�.
		{
		case DEALER_ROLE :			Business_Dealer() ;					break ;			// �������, ���� �����Ѵ�.
		case CHANGGO_ROLE :			Business_Changgo(dwData, dwParam) ;	break ;			// â��������, â�� �����Ͻ��� �����Ѵ�.
		case MUNPA_ROLE :			Business_Guild(dwData) ;			break ;			// ��� ������ �̶��, ��� �����Ͻ��� �����Ѵ�.
		case MAPCHANGE_ROLE :		Business_MapChange(dwData) ;		break ;			// �� ü���� NPC ���, �� ü���� �����Ͻ��� �����Ѵ�.
		case FAMILY_ROLE :			Business_Family(dwData) ;			break ;			// �йи� ������ �̶��, �йи� ���� �����Ͻ��� �����Ѵ�.
		case IDENTIFICATION_ROLE :	Business_Identifycation() ;			break ;			// �ֹε�� ���� ���̶��, �ֹε�� �����Ͻ��� �����Ѵ�.
		case HOUSING_ROLE:			Business_Housing(dwData) ;			break ;
		default :																		// �� ���� ���,
			{
				ASSERT(0);																// ASSERT üũ�� �Ѵ�.

				if( HERO->GetState() == eObjectState_Deal )								// HERO�� ���°� �ŷ����̶��,
				{
					OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal);			// �ŷ� ���¸� ������.
				}

				return;																	// ���� ó���� �Ѵ�.
			}
			break ;
		}
	}

	if( dwParam == eNpcParam_ShowpStorage )												// �Ķ���� ���� eNpcParam_ShowpStorage�� ������,
	{
		Business_Changgo(dwData, dwParam) ;												// â�� �����Ͻ��� �����Ѵ�.
	}

	GAMEIN->GetNpcScriptDialog()->DisActiveEx();										// NPC ��ũ��Ʈ ���̾�α��� DisActiveEx()�� ȣ���Ѵ�.
}

void CNpcScriptManager::Business_Dealer()  												// ���� �����Ͻ�.
{
	MSG_WORD msg;																		// �޽��� ����ü�� �����Ѵ�.
	msg.Category = MP_ITEM;																// ī�װ����� ���������� �����Ѵ�.
	msg.Protocol = MP_ITEM_DEALER_SYN;													// ���������� ������ ���� ��û���� �����Ѵ�.
	msg.dwObjectID = HEROID;															// HERO�� ���̵� �����Ѵ�.
	msg.wData = m_pCurSelectedNpc->GetNpcUniqueIdx();									// NPC ����ũ ���̵� �����Ѵ�.

	NETWORK->Send(&msg, sizeof(msg));													// �޽����� �����Ѵ�.

	GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);										// ��ũ��Ʈ ���̾�α׸� ��Ȱ��ȭ �Ѵ�.
}

void CNpcScriptManager::Business_Changgo(DWORD dwData, DWORD dwParam)					// â�� �����Ͻ�.
{
	// 080513 KTH -- NPC ��ũ��Ʈ�� ���� �־�߸� ���� �ֵ��� ���� (defense hack)
	if( !GAMEIN->GetNpcScriptDialog()->IsActive() )
		return;

	if(dwData == 0)																		// �����͸� Ȯ���Ѵ�.
	{	
		if(GAMEIN->GetStorageDialog()->IsItemInit())									// â�� �������� ������ �ʱ�ȭ�� �Ǿ�����,
		{
			if( !GAMEIN->GetInventoryDialog()->IsActive() )								// �κ��丮 ���̾�αװ� Ȱ��ȭ ���̸�,
			{
				GAMEIN->GetStorageDialog()->ShowStorageDlg(TRUE) ;							// â�� ���̾�α׸� Ȱ��ȭ �Ѵ�.
				GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);
			}
			else
			{
				GAMEIN->GetStorageDialog()->SetActive(TRUE) ;							// â�� ���̾�α׸� Ȱ��ȭ �Ѵ�.
				GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);
			}

			if(HERO->GetStorageNum() == 0)												// ������ â���� ���ٸ�,
			{

				GAMEIN->GetStorageDialog()->ShowStorageMode(eStorageMode_NoWare);		// â���� ���ٴ� ���� �����Ѵ�.

				GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);							// â�� ���̾�α׸� ��Ȱ��ȭ �Ѵ�.
			}
			else																		// ������ â���� �ִٸ�,
			{
				GAMEIN->GetStorageDialog()->ShowStorageMode(eStorageMode_StorageWare1);	// â�� 1�� Ȱ��ȭ �� ���� �Ѵ�.
				GAMEIN->GetStorageDialog()->AddStorageMode(eStorageMode_StorageWare1);	// â�� ��带 �߰��Ѵ�.

				GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);							// ��ũ��Ʈ ���̾�α׸� ��Ȱ��ȭ �Ѵ�.
			}
		}
		else																			// â���� �������� ������ �ʱ�ȭ�� ���� �ʾҴٸ�,
		{
			if(HERO->GetStorageNum() == 0)												// ������ â���� ������,
			{
				Business_Changgo(
				1,
				0);
				return;
			}
			else																		// ������ â���� �ִٸ�,
			{
				MAP->SetVillage(TRUE) ;													// ������ �����Ѵ�.

				if( dwParam == eNpcParam_ShowpStorage || MAP->IsVillage() == TRUE )		// â�� npc �̰�, ������ ������,
				{						
					MSG_WORD2 msg;														// �޽��� ����ü�� �����Ѵ�.
					msg.Category = MP_ITEM;												// ī�װ����� ���������� �����Ѵ�.
					msg.Protocol = MP_ITEM_STORAGE_ITEM_INFO_SYN;						// ���������� ������ â��, ������ ���� ��ũ�� �����Ѵ�.
					msg.dwObjectID = HEROID;											// HERO ���̵� �����Ѵ�.
					
					if( dwParam==eNpcParam_ShowpStorage )								// NPC�� â�� NPC �̸�,
					{
						msg.wData1=79 ;													// �����͸� 79�� �����Ѵ�.
					}
					else																// NPC�� â�� NPC�� �ƴϸ�,
					{
						msg.wData1=m_pCurSelectedNpc->GetNpcUniqueIdx();				// �����͸� NPC ����ũ �ε����� �����Ѵ�.
					}
					//dwParam==eNpcParam_ShowpStorage ? msg.wData1=79 : msg.wData1=m_pCurSelectedNpc->GetNpcUniqueIdx();

					msg.wData2 = eNpcParam_ShowpStorage;								// ������ 2�� NPC �����ͷ� �����Ѵ�.

					NETWORK->Send(&msg, sizeof(msg));									// �޽����� �����Ѵ�.
				}
				else																	// NPC�� â�� NPC�� �ƴϰų�, ������ �ƴϸ�,
					return;																// ���� ó���� �Ѵ�.
			}
		}

		OBJECTSTATEMGR->StartObjectState(HERO, eObjectState_Deal);						// HERO�� ���¸� �ŷ� ���·� �����Ѵ�.
		GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);
	}	
	else if(dwData == 1)																// ����ǥ/����
	{
		GAMEIN->GetStorageDialog()->SetStorageListInfo();								// â�� ���̾�α��� ����Ʈ ������ �����Ѵ�.
		GAMEIN->GetStorageDialog()->ShowStorageDlg(TRUE) ;								// â�� ���̾�α��� â�� ���̾�α׸� �����ش�.

		GAMEIN->GetStorageDialog()->ShowStorageMode(eStorageMode_StorageListInfo);		// â�� ����Ʈ ������ â�� ��带 �����Ѵ�.
		GAMEIN->GetStorageDialog()->AddStorageMode(eStorageMode_StorageListInfo);		// â�� ���̾�α׿� â�� ��带 �߰��Ѵ�.

		GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);									// ��ũ��Ʈ ���̾�α׸� ��Ȱ��ȭ �Ѵ�.
	}
	else if(dwData == 2)																// ��� â��
	{
		GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);

		if( ! HERO->GetGuildIdx() )
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 325 ) );
			return;
		}

		const CGuildManager::LevelSetting* setting = GUILDMGR->GetLevelSetting( GUILDMGR->GetLevel() );
		ASSERT( setting );

		if( !	setting ||
			!	setting->mWarehouseSize )
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 613 ) );
			return;
		}
		
		MSGBASE message;

		message.Category	= MP_GUILD;
		message.Protocol	= MP_GUILD_OPEN_WAREHOUSE_SYN;
		message.dwObjectID	= HEROID;

		NETWORK->Send( &message, sizeof( message ) );
	}
	else if(dwData == 3)																// �����۸�â��
	{
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

		MSGBASE msg;																	// �޽��� ����ü�� �����Ѵ�.
		msg.Category = MP_ITEM;															// ī�װ����� ���������� �����Ѵ�.
		msg.Protocol = MP_ITEM_SHOPITEM_INFO_SYN;										// ���������� ������ �� ���� ��ũ�� �����Ѵ�.
		msg.dwObjectID = gHeroID;
		NETWORK->Send(&msg, sizeof(msg));												// �޽����� �����Ѵ�.

		GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);									// ��ũ��Ʈ ���̾�α׸� ��Ȱ��ȭ �Ѵ�.
		OBJECTSTATEMGR->StartObjectState(HERO, eObjectState_Deal);						// HERO�� ���¸� �ŷ� ���·� �����Ѵ�.
	}
}

void CNpcScriptManager::Business_Guild(DWORD dwData)  									// ��� �����Ͻ�.
{
	switch( dwData )																	// �����͸� Ȯ���Ѵ�.
	{
	case 0:																				// ��带 �����Ϸ��� �Ѵٸ�,
		{
			if(HERO->GetGuildIdx())														// HERO�� ��� �ε����� ��ȿ�ϸ�,
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(327));					// �̹� ��尡 �ִٴ� �޽����� ����Ѵ�.
				GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);							// NPC ��ũ��Ʈ ���̾�α׸� ��Ȱ��ȭ �Ѵ�.

				return;																	// ���� ó���� �Ѵ�.
			}

			cDialog* pDlg = (cDialog*)GAMEIN->GetGuildCreateDlg();						// ��� ���� ���̾�α� ������ �޾ƿ´�.

			if( pDlg )																	// ��� ���� ���̾�α� ������ ��ȿ���� üũ�Ѵ�.
			{
				pDlg->SetActive(TRUE);													// ��� ���� ���̾�α׸� Ȱ��ȭ �Ѵ�.
			}

			GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);								// NPC ��ũ��Ʈ ���̾�α׸� �ݴ´�.
		}
		break;

	case 1:																				// ��带 �����Ϸ��� �Ѵٸ�,
		{
			if(HERO->GetGuildIdx() == 0)												// HERO�� ��� �ε����� ��ȿ���� ������,
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(325));					// ��忡 ���ԵǾ� ���� �ʴٴ� �޽����� ����Ѵ�.
				GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);							// NPC ��ũ��Ʈ ���̾�α׸� ��Ȱ��ȭ �Ѵ�.

				return;																	// ���� ó���� �Ѵ�.
			}
			else if(HERO->GetGuildMemberRank() == GUILD_MASTER)							// HERO�� ������ ��� �����Ϳ� ���ٸ�,
			{
				WINDOWMGR->MsgBox( MBI_GUILD_BREAKUP, MBT_YESNO, CHATMGR->GetChatMsg( 329 ) );	// �г�Ƽ�� �ް� �ȴٴ� ����â�� ����.
				GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);							// NPC ��ũ��Ʈ ���̾�α׸� �ݴ´�.

				return ;																// ���� ó���� �Ѵ�.
			}
			else																		// HERO�� ������ ��� �����Ͱ� �ƴ϶��,
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(323));					// ������ ���ٴ� �޽����� ����Ѵ�.
				GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);							// NPC ��ũ��Ʈ ���̾�α׸� �ݴ´�.

				return;																	// ���� ó���� �Ѵ�.
			}
		}
		break;

	case 2:																				// ��� ������ ����.
		{
			if(HERO->GetGuildIdx() == 0)												// HERO�� ��� �ε����� üũ�Ѵ�.
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(325));					// ��忡 ���ԵǾ� ���� �ʴٴ� �޽����� ����Ѵ�.
			}
			else if(HERO->GetGuildMemberRank() != GUILD_MASTER)							// HERO�� ������ ��� �����Ϳ� ���� �ʴٸ�,
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(323));					// ������ ���ٴ� �޽����� ����Ѵ�.
			}
			else
			{
				cDialog* const guildLevelUpDialog = WINDOWMGR->GetWindowForID(GD_LEVELUPDLG);

				if(guildLevelUpDialog)
				{
					guildLevelUpDialog->SetActive(TRUE);
				}
			}	

			GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);
		}
		break;

	case 3:																				// ��� ��ũ ���.
		{
			if( HERO->GetGuildIdx() )
			{
				GAMEIN->GetGuildMarkDlg()->ShowGuildMark();
				//GAMEIN->GetGuildMarkDlg()->SetActive( TRUE );
			}
			else
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(325));
			}

			GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);
		}
		break;
	// ���� ��ũ ���
	case 4:
		{
			GAMEIN->GetGuildMarkDlg()->ShowGuildUnionMark();

			GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);
		}
		break;
	}
}

void CNpcScriptManager::Business_MapChange(DWORD dwData)  								// �� ü���� �����Ͻ�.
{
	if(dwData == 0)																		// �����Ͱ� 0�� ������,
	{
		MSG_DWORD msg;																	// �޽��� ����ü�� �����Ѵ�.
		msg.Category = MP_NPC;															// ī�װ����� NPC �� �����Ѵ�.
		msg.Protocol = MP_NPC_DOJOB_SYN;												// ���������� NPC ���� ���� ��û���� �����Ѵ�.
		msg.dwObjectID = HEROID;														// HERO ���̵� �����Ѵ�.
		msg.dwData = m_pCurSelectedNpc->GetID();										// ���� ���� �� NPC�� ���̵� �����Ѵ�.

		NETWORK->Send(&msg, sizeof(msg));												// �޽����� �����Ѵ�.
	}
	else																				// �����Ͱ� 0�� �ƴϸ�,
	{
		if(GAMEIN->GetNpcScriptDialog()->IsActive())									// NPC ��ũ��Ʈ ���̾�αװ� ����������,
		{
			NPCSCRIPTMGR->SetSelectedNpc(NULL);											// ���� ���� �� NPC�� �����Ѵ�.
			GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);								// NPC ��ũ��Ʈ ���̾�α׸� �ݴ´�.
		}
		
		if( HERO->GetState() == eObjectState_Deal )										// HERO ���°� �ŷ����̸�,
		{
			OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal);					// �ŷ� ���¸� ������.
		}
	}
}

void CNpcScriptManager::Business_Family(DWORD dwData)  									// �йи� �����Ͻ�.
{
	if( dwData == 0 )																	// �йи��� �����Ϸ��� �ϸ�,
	{
		if(GAMEIN->GetNpcScriptDialog()->IsActive())									// NPC��ũ��Ʈ ���̾�ΰ� ����������,
		{
			NPCSCRIPTMGR->SetSelectedNpc(NULL);											// ���� �� NPC�� �����Ѵ�.
			GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);								// NPC ��ũ��Ʈ ���̾�α׸� �ݴ´�.
		}

		cDialog* const familyCreateDialog = WINDOWMGR->GetWindowForID(FD_CREATEDLG);

		if(familyCreateDialog)
		{
			familyCreateDialog->SetActive(TRUE);
		}
	}
	else if( dwData == 1 )																// �йи��� ��ü�Ϸ��� �ϸ�,
	{
		if(GAMEIN->GetNpcScriptDialog()->IsActive())									// NPC ��ũ��Ʈ�� ���� �ִٸ�,
		{
			NPCSCRIPTMGR->SetSelectedNpc(NULL);											// ���� �� NPC�� ���� �Ѵ�.
			GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);								// NPC ��ũ��Ʈ�� �ݴ´�.
		}				

   		WINDOWMGR->MsgBox( MBI_FAMILY_BREAKUP, MBT_YESNO, CHATMGR->GetChatMsg( 1144 ), int((CSHFamilyManager::LEAVE_PENALTY_EXP_DOWN_RATE[CSHFamilyManager::FLK_BREAKUP]+0.001)*100), CSHFamilyManager::LEAVE_PENALTY_REJOIN_TIME[CSHFamilyManager::FLK_BREAKUP] );
	}
	else if(dwData == 2 )																// �ֹε���� �߱� �������� �ϸ�,
	{
		const DATE_MATCHING_INFO& dateInfo = HERO->GetDateMatchingInfo() ;				// ����Ʈ ��Ī ������ �޴´�.

		if(dateInfo.bIsValid)
		{
			WINDOWMGR->MsgBox( MBI_IDENTIFICATION_ALREADY_HAVE, MBT_OK, CHATMGR->GetChatMsg( 830 ) );

			if(cNpcScriptDialog* pNpcDlg = GAMEIN->GetNpcScriptDialog())
			{
				pNpcDlg->SetActive(FALSE) ;
			}
		}
		else
		{
			if(CIdentification* pIdentificationDlg = GAMEIN->GetIdentificationDlg())
			{
				pIdentificationDlg->SetActive(TRUE);
				GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);
			}

			if(CFavorIcon* pFavorIconDlg = GAMEIN->GetFavorIconDlg())
			{
				pFavorIconDlg->SetActiveRecursive(
					TRUE);
				GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);
			}
		}
	}
}

void CNpcScriptManager::Business_Identifycation()  										// �ֹε�� �����Ͻ�.
{
	const DATE_MATCHING_INFO& dateInfo = HERO->GetDateMatchingInfo() ;					// ����Ʈ ��Ī ������ �޴´�.

	if( dateInfo.bIsValid )															// �̹� �߱� �޾Ҵٸ�,
	{
		WINDOWMGR->MsgBox( MBI_IDENTIFICATION_ALREADY_HAVE, MBT_OK, CHATMGR->GetChatMsg( 830 ) );

		if(cNpcScriptDialog* pNpcDlg = GAMEIN->GetNpcScriptDialog())
		{
			pNpcDlg->SetActive(FALSE);
		}
	}
	else
	{
		if(CIdentification* pIdentificationDlg = GAMEIN->GetIdentificationDlg())
		{
			pIdentificationDlg->SetActive(TRUE);
			GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);
		}


		if(CFavorIcon* pFavorIconDlg = GAMEIN->GetFavorIconDlg())
		{
			pFavorIconDlg->SetActiveRecursive(TRUE);
			GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);
		}
	}	
}

// �Ͽ�¡ �����Ͻ�.
void CNpcScriptManager::Business_Housing(DWORD dwData) 														
{
	// ������Ÿ�Կ� �°� ó������ 
	switch( dwData )
	{
	case 0:														// �ŷ��ϱ� 	
		{
			// 091007 pdy �Ͽ콺 �����ε� �ŷ��ϱⰡ �����ϰԲ� �߰�
			MSG_WORD msg;																		// �޽��� ����ü�� �����Ѵ�.
			msg.Category = MP_ITEM;																// ī�װ����� ���������� �����Ѵ�.
			msg.Protocol = MP_ITEM_DEALER_SYN;													// ���������� ������ ���� ��û���� �����Ѵ�.
			msg.dwObjectID = HEROID;															// HERO�� ���̵� �����Ѵ�.
			msg.wData = m_pCurSelectedNpc->GetNpcUniqueIdx();									// NPC ����ũ ���̵� �����Ѵ�.

			NETWORK->Send(&msg, sizeof(msg));													// �޽����� �����Ѵ�.

			GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);										// ��ũ��Ʈ ���̾�α׸� ��Ȱ��ȭ �Ѵ�.
		}
		break;
	case 1:													 // ���� �����Ϸ��� �Ѵٸ�,												
		{
			const DATE_MATCHING_INFO& dateInfo = HERO->GetDateMatchingInfo() ;

			//090527 pdy �Ͽ�¡ �ý��۸޼��� �ֹε���� �̺����� [�� ����]
			if(! dateInfo.bIsValid )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1859 ) );	//1859	"�ֹε������ �߱� ���� �����̽��ϴ�."
				GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);	
				return;
			}

			//090527 pdy �Ͽ�¡ �ý��۸޼��� ž�»��½� �����ൿ [���]
			{
				cDialog* const houseNameDialog = WINDOWMGR->GetWindowForID(HOUSE_NAMEDLG);

				if(houseNameDialog)
				{
					houseNameDialog->SetActive(TRUE);
				}
			}

			GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);	

		}
		break;
	case 2:													// ������ �湮�ϰ� �ʹٸ�																			
		{
			//090604 pdy �Ͽ�¡ Static Npc�� ���� ������ũ���� �߰� 
			CNpc* pNpc = NPCSCRIPTMGR->GetSelectedNpc();

			if(pNpc) 
			{
				//090604 pdy �Ͽ�¡ �˾�â [StaticNpc ������ũ����]
				cMsgBox* pBox = WINDOWMGR->MsgBox( MBI_HOUSE_VISIT_LINK_MYHOME_AREYOUSURE, MBT_YESNO, CHATMGR->GetChatMsg(1897) );	//1897	"������ �̵��Ͻðڽ��ϱ�?"
				pBox->SetParam( pNpc->GetID() );
				OBJECTSTATEMGR->StartObjectState(HERO, eObjectState_Deal) ;
			}
	
			GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);	
		}
		break;
	}
}