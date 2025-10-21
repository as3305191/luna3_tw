#include "stdafx.h"
#include "QuestManager.h"
#include "Interface/GameNotifyManager.h"
#include "MHFile.h"
#include "..\[CC]Quest\QuestScriptLoader.h"
#include "..\[CC]Quest\QuestString.h"
#include "..\[CC]Quest\QuestInfo.h"
#include "..\[CC]Quest\SubQuestInfo.h"
#include "..\[CC]Quest\QuestExecute.h"
#include "..\[CC]Quest\QuestNpc.h"
#include "..\[CC]Quest\QuestNpcData.h"
#include "..\[CC]Quest\QuestNpcScript.h"
#include "Quest.h"
#include "QuestUpdater.h"
#include "ObjectManager.h"
#include "ChatManager.h"
#include "QuestItemManager.h"
#include "UserInfoManager.h"
#include "GameIn.h"
#include "ItemManager.h"
#include "InventoryExDialog.h"
#include "WindowIdEnum.h"
#include "./Interface/cWindowManager.h"
#include "cDivideBox.h"
#include "cMsgBox.h"
#include "QuickManager.h"
#include "Npc.h"
#include "MHMap.h"
#include "MiniMapDlg.h"
#include "QuestDialog.h"
#include "QuestQuickViewDialog.h"
#include "StatusIconDlg.h"
#include "NpcScriptDialog.h"
#include "NpcImageDlg.h"
#include "QuestTotalDialog.h"
#include "OptionManager.h"
#include "WorldMapDlg.h"
#include "BigMapDlg.h"

GLOBALTON(CQuestManager);																// ����Ʈ �Ŵ����� �۷ι� ������ ��.

CQuestManager::CQuestManager()															// ������ �Լ�.
{
	m_QuestInfoTable.Initialize(MAX_QUEST);												// �ִ� ����Ʈ ����ŭ(200) ����Ʈ ���� ���̺��� �ʱ�ȭ �Ѵ�.
	m_QuestStringTable.Initialize(200);													// �ִ� ����Ʈ ����ŭ(200) ����Ʈ ��Ʈ�� ���̺��� �ʱ�ȭ �Ѵ�.
	m_QuestTable.Initialize(MAX_QUEST);													// �ִ� ����Ʈ ����ŭ(200) ����Ʈ ���̺��� �ʱ�ȭ �Ѵ�.
	m_QuestNpcTable.Initialize(100);													// 100�� ��ŭ ����Ʈ npc ���̺��� �ʱ�ȭ �Ѵ�.

	m_QuestUpdater = new CQuestUpdater;													// ����Ʈ �������͸� ���� �Ҵ��Ѵ�.

	ZeroMemory(
		m_pNpcArray,
		sizeof(m_pNpcArray));
	m_dwMaxNpc = 0;																		// �ִ� npc�� ���� ������ 0���� �ʱ�ȭ �Ѵ�.
	
	m_dwLastMsg = 0;
	ZeroMemory(
		m_QuickViewQuestID,
		sizeof(m_QuickViewQuestID));
}

CQuestManager::~CQuestManager()															// �Ҹ��� �Լ�.
{
	Release();
	SAFE_DELETE(
		m_QuestUpdater);
}

void CQuestManager::Release()															// ���� �Լ�.
{
	{
		m_QuestInfoTable.SetPositionHead();

		for(CQuestInfo* pQuestInfo = m_QuestInfoTable.GetData();
			0 < pQuestInfo;
			pQuestInfo = m_QuestInfoTable.GetData())
		{
			SAFE_DELETE(
				pQuestInfo);
		}

		m_QuestInfoTable.RemoveAll();
	}

	{
		m_QuestStringTable.SetPositionHead();

		for(CQuestString* pQuestString = m_QuestStringTable.GetData();
			0 < pQuestString;
			pQuestString = m_QuestStringTable.GetData())
		{
			SAFE_DELETE(
				pQuestString);
		}

		m_QuestStringTable.RemoveAll();
	}

	{
		m_QuestTable.SetPositionHead();

		for(CQuest* pQuest = m_QuestTable.GetData();
			0 < pQuest;
			pQuest = m_QuestTable.GetData())
		{
			SAFE_DELETE(
				pQuest);
		}

		m_QuestTable.RemoveAll();
	}

	{
		m_QuestNpcTable.SetPositionHead();

		for(QUESTNPCINFO* pData = m_QuestNpcTable.GetData();
			0 < pData;
			pData = m_QuestNpcTable.GetData())
		{
			SAFE_DELETE(
 				pData);
		}

		m_QuestNpcTable.RemoveAll();
	}

	m_dwLastMsg = 0;
}

BOOL CQuestManager::LoadQuestScript( char* strFileName )								// ����Ʈ ��ũ��Ʈ�� �ε��ϴ� �Լ�.
{
	CMHFile file;																		// ���� ������ �����Ѵ�.

	if( !file.Init( strFileName, "rb") )												// ���� �����̸���, ���� ��忡 ���� ������ ����, ����� �޴´�.
	{																					// �����ߴٸ�,
		return FALSE;																	// FALSE�� �����Ѵ�.
	}

	char Token[256];																	// �ӽ� ���۸� �����Ѵ�.

	DWORD dwQuestNum = 0;																// ����Ʈ ��ȣ�� ���� ������ ����, 0���� �ʱ�ȭ �Ѵ�.

	int nMapNum = 0 ;

	DWORD dwNpcIdx = 0 ;

	while( !file.IsEOF() )																// ������ ���� �ƴ� ���� while���� ������.
	{
		file.GetString( Token );														// ��Ʈ���� �о� �ӽ� ���ۿ� ��´�.

		if( strcmp(Token,"$QUEST") == 0 )												// �о���� ��Ʈ���� $QUEST�� ���ٸ�,
		{
			dwQuestNum = file.GetDword();												// ����Ʈ ��ȣ�� �о���δ�.

			nMapNum = file.GetInt() ;													// �� ��ȣ�� �о���δ�.

			dwNpcIdx = file.GetDword() ;												// npc �ε����� �о���δ�.

			CQuestInfo* pQuestInfo = CQuestScriptLoader::LoadQuestInfo( &file, dwQuestNum ) ;

			if( pQuestInfo )
			{
				pQuestInfo->SetMapNum(nMapNum) ;
				pQuestInfo->SetNpcIdx(dwNpcIdx) ;
			}

			m_QuestInfoTable.Add( pQuestInfo, dwQuestNum );	// ����Ʈ ���� ���̺��� ����Ʈ�� ��´�.
		}
	}

	CreateAllQuestFromScript();															// ��ũ��Ʈ�� ���� ��� ����Ʈ�� �����Ѵ�.

	return TRUE;																		// TRUE�� �����Ѵ�.
}

BOOL CQuestManager::LoadQuestString(char* strFileName)									// ����Ʈ ��Ʈ���� �ε��ϴ� �Լ�.
{
	CMHFile file;																		// ���� ������ �����Ѵ�.

	if( !file.Init( strFileName, "rb") )												// ���� �����̸���, ���� ��忡 ���� ������ ����, ����� �޴´�.
	{																					// �����ߴٸ�,
		return FALSE;																	// FALSE�� �����Ѵ�.
	}

	char Token[256];																	// �ӽ� ���۸� �����Ѵ�.
	
	while( !file.IsEOF() )																// ������ ���� �ƴѵ��� while���� ������.
	{
		file.GetString(Token);															// ��Ʈ���� �о� �ӽ� ���ۿ� ��´�.

		if(strcmp(Token,"$SUBQUESTSTR") == 0)											// �о���� ��Ʈ���� $SUBQUESTSTR�� ���ٸ�,
		{
			DWORD Key_1 = file.GetDword();												// Ű1�� �о���δ�.
			DWORD Key_2 = file.GetDword();												// Ű2�� �о���δ�.
			DWORD Key = 0;

			COMBINEKEY(Key_1, Key_2, Key);												// Ű ������ Ű1, Ű2�� �����Ͽ� �޴´�.
			CQuestString* pQuestString = CQuestScriptLoader::LoadQuestString(&file);	// ����Ʈ ��Ʈ���� �ε��Ͽ� ����Ʈ ��Ʈ�� �����ͷ� �޴´�.

			ASSERT( pQuestString && "Why is it null?" );								// ����Ʈ ��Ʈ�� ������ ��ȿ�ؾ� �Ѵ�. assertó��.
			ASSERT( ! m_QuestStringTable.GetData( Key ) && "This memory'll be overwrite. It's reason of memory leak" );	// ����Ʈ ��Ʈ�� ���̺��� �˻��Ѵ�.
			
			m_QuestStringTable.Add(pQuestString, Key);									// ����Ʈ ��Ʈ�� ���̺��� ����Ʈ ��Ʈ�� ������ ��´�.
			pQuestString->SetIndex(Key_1, Key_2);										// ����Ʈ ��Ʈ���� Ű1, Ű2�� �ε����� �����Ѵ�.
		}
	}

	//
	m_QuestItemMGR.QuestItemInfoLoad();													// ����Ʈ ������ �Ŵ����� ����Ʈ ������ ������ �ε��Ѵ�.

	return TRUE;																		// TRUE�� �����Ѵ�.
}

void CQuestManager::LoadQuestNpc( char* strFileName )									// ����Ʈ npc�� �ε��ϴ� �Լ�.
{
	CMHFile file;																		// ���� ������ �����Ѵ�.

	if( !file.Init( strFileName, "rb") )												// ���� �����̸���, ���� ��忡 ���� ������ ����, ����� �޴´�.
	{																					// �����ߴٸ�,
		return ;																		// ���� ó���� �Ѵ�.
	}
	
	while( !file.IsEOF() )																// ������ ���� �ƴѵ��� while���� ������.
	{
		QUESTNPCINFO* pData = new QUESTNPCINFO;											// ����Ʈ npc������ ���� �޸𸮸� �Ҵ��ϰ�, �����ͷ� �޴´�.
		pData->dwObjIdx = 0;															// ������Ʈ �ε����� 0���� �����Ѵ�.
		pData->wMapNum = file.GetWord();												// �� ��ȣ�� �޴´�.
		pData->wNpcKind = file.GetWord();												// npc ������ �޴´�.
//		file.GetString( pData->sName );
		SafeStrCpy( pData->sName, file.GetString(), MAX_NAME_LENGTH+1 );				// npc �̸��� �޴´�.
		pData->wNpcIdx = file.GetWord();												// npc �ε����� �޴´�.
		pData->vPos.x = file.GetFloat();												// npc ��ġ�� �޴´�.
		pData->vPos.y = 0;
		pData->vPos.z = file.GetFloat();
		pData->fDir = file.GetFloat();													// npc ����?�� �޴´�.

		m_QuestNpcTable.Add( pData, pData->wNpcIdx );									// npc ���̺��� npc������ ��´�.
	}
}

void CQuestManager::CreateAllQuestFromScript()											// ��ũ��Ʈ�� ���� ��� ����Ʈ�� �����ϴ� �Լ�.
{
	m_QuestInfoTable.SetPositionHead();

	for(CQuestInfo* pQuestInfo = m_QuestInfoTable.GetData();
		0 < pQuestInfo;
		pQuestInfo = m_QuestInfoTable.GetData())
	{
		CQuest* pQuest = new CQuest( pQuestInfo );										// ����Ʈ ������ ���� ����Ʈ �޸𸮸� �Ҵ��ϰ�, ����Ʈ �����ͷ� �޴´�.
		m_QuestTable.Add( pQuest, pQuestInfo->GetQuestIdx() );							// ����Ʈ ���̺��� �߰��� �Ѵ�.

		// 100719 ShinJS NPC�� ���� ����Ʈ ��Ͽ� �߰�
		const DWORD dwNpcUniqueIdx = pQuestInfo->GetNpcIdx();
		const DWORD dwQuestIdx = pQuestInfo->GetQuestIdx();

		NpcHasQuestList& hasQuestList = m_NpcHasQuestList[ dwNpcUniqueIdx ];
		hasQuestList.insert( dwQuestIdx );

		pQuest->AddNpcHasQuest();
	}
}

void CQuestManager::InitializeQuest()													// ����Ʈ�� �ʱ�ȭ �ϴ� �Լ�.
{
	m_QuestTable.SetPositionHead();

	for(CQuest* pQuest = m_QuestTable.GetData();
		0 < pQuest;
		pQuest = m_QuestTable.GetData())
	{
		pQuest->Initialize();

		CQuestInfo* pQuestInfo = m_QuestInfoTable.GetData( pQuest->GetQuestIdx() );
		if( !pQuestInfo )
			continue;

		// 100405 ShinJS --- �ð����� ����Ʈ ���
		if( (WORD)pQuestInfo->GetMapNum() == MAP->GetMapNum() &&
			pQuest->HasDateLimit() )
		{
			pQuest->InitDateTime();
			m_setDateLimitQuest.insert( pQuest->GetQuestIdx() );
		}
	}

	m_dwMaxNpc = 0;
	m_dwLastMsg = 0;
}

void CQuestManager::SetMainQuestData( DWORD dwQuestIdx, DWORD dwSubQuestFlag, DWORD dwData, __time64_t regisTime, BYTE bCheckType, DWORD dwCheckTime )
{
	if( dwQuestIdx == 0 )																// ����Ʈ �ε����� 0�� ���ٸ�, // Newbieguide
	{
		//m_NewbieGuide.SetFlagData( dwSubQuestFlag, dwData, dwTime );					// �ʺ��� ���̵��� �÷��� �����͸� �����Ѵ�.
		return;																			// ���� ó���� �Ѵ�.
	}
	else																				// ����Ʈ �ε����� 0�� �ƴ϶��,
	{
		CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );							// ����Ʈ ���̺����� ����Ʈ �ε����� ����Ʈ ������ �޴´�.

		if( !pQuest ) return ;															// ����Ʈ ������ ��ȿ�ϴٸ�,

		pQuest->SetMainQuestData( dwSubQuestFlag, dwData, regisTime, bCheckType, dwCheckTime );	// ���� ����Ʈ �����͸� �����Ѵ�.

		if( pQuest->IsQuestComplete() ) return ;
		m_QuestDialog->StartQuest(dwQuestIdx) ;
	}
}

void CQuestManager::SetSubQuestData( DWORD dwQuestIdx, DWORD dwSubQuestIdx, DWORD dwData, DWORD dwTime )
{
	CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );								// ����Ʈ ���̺����� ���� ����Ʈ �ε����� ����Ʈ ������ �޴´�.

	if( !pQuest ) return ;																// ����Ʈ ������ ��ȿ���� üũ�Ѵ�.

	pQuest->SetSubQuestData( dwSubQuestIdx, dwData, dwTime );						// ����Ʈ�� ���� ����Ʈ�� �����Ѵ�.

	AddQuestNpc( pQuest->GetSubQuestInfo( dwSubQuestIdx ) );						// ����Ʈ npc�� �߰��Ѵ�.

	m_QuestDialog->RefreshQuestList() ;
}

void CQuestManager::AddMapTree(int nMapNum)												// �� Ʈ���� �߰��ϴ� �Լ�.
{	
	CQuestTotalDialog * pTotalDialog = GAMEIN->GetQuestTotalDialog() ;					// ����Ʈ ��Ż ���̾�α� ������ �޴´�.

	if( pTotalDialog )																	// ��Ż ���̾�α� ������ ��ȿ�ϸ�,
	{
		CQuestDialog* pQuestDlg = pTotalDialog->GetQuestDlg() ;							// ����Ʈ ���̾�α� ������ �޴´�.

		if( pQuestDlg )																	// ����Ʈ ���̾�α� ������ ��ȿ�ϸ�,
		{
			cPtrList& pTreeList = pQuestDlg->GetTreeList() ;										// Ʈ�� ����Ʈ�� �޴´�.			

			BOOL bResult = FALSE ;													// �˻� ����� ���� ������ �����ϰ� FALSE ó���� �Ѵ�.

			int nTreeCount = pTreeList.GetCount() ;								// Ʈ�� ī��Ʈ�� �޴´�.

			QuestTree* pTree = NULL ;												// Ʈ�� ������ ���� �����͸� �����ϰ� NULL ó���� �Ѵ�.

			PTRLISTPOS pTreePos = NULL ;											// Ʈ���� ��ġ ������ ���� �����͸� �����ϰ� NULLó���� �Ѵ�.

			for( int count = 0 ; count < nTreeCount ; ++count )						// Ʈ�� ī��Ʈ ��ŭ FOR���� ������.
			{
				pTreePos = pTreeList.FindIndex(count) ;							// ī��Ʈ�� �ش��ϴ� ��ġ ������ �޴´�.

				if( pTreePos )														// Ʈ�� ��ġ ������ ��ȿ�ϴٸ�,
				{
					pTree = (QuestTree*)pTreeList.GetAt(pTreePos) ;				// ��ġ�� �ش��ϴ� Ʈ�� ������ �޴´�.

					if( pTree )														// Ʈ�� ������ ��ȿ�ϴٸ�,
					{
						if( pTree->nMapNum == nMapNum )								// ���� ����Ʈ�� �� ��ȣ�� Ʈ���� �� ��ȣ�� ���ؼ� ������,
						{
							bResult = TRUE ;										// ����� TRUE�� �����Ѵ�.

							break ;													// for���� ����������.
						}
					}
				}
			}

			if( bResult == FALSE )													// ����� FALSE�� ������,
			{
				pQuestDlg->AddQuestTree(nMapNum) ;
			}
		}
	}
}

void CQuestManager::DeleteMapTree(int nMapNum)
{
	CQuestTotalDialog * pTotalDialog = GAMEIN->GetQuestTotalDialog() ;					// ����Ʈ ��Ż ���̾�α� ������ �޴´�.

	if( pTotalDialog )																	// ��Ż ���̾�α� ������ ��ȿ�ϸ�,
	{
		CQuestDialog* pQuestDlg = pTotalDialog->GetQuestDlg() ;							// ����Ʈ ���̾�α� ������ �޴´�.

		if( pQuestDlg )																	// ����Ʈ ���̾�α� ������ ��ȿ�ϸ�,
		{
			cPtrList& pTreeList = pQuestDlg->GetTreeList() ;										// Ʈ�� ����Ʈ�� �޴´�.			

			int nTreeCount = pTreeList.GetCount() ;								// Ʈ�� ī��Ʈ�� �޴´�.

			QuestTree* pTree = NULL ;												// Ʈ�� ������ ���� �����͸� �����ϰ� NULL ó���� �Ѵ�.

			PTRLISTPOS pTreePos = NULL ;											// Ʈ���� ��ġ ������ ���� �����͸� �����ϰ� NULLó���� �Ѵ�.

			for( int count = 0 ; count < nTreeCount ; ++count )						// Ʈ�� ī��Ʈ ��ŭ FOR���� ������.
			{
				pTreePos = pTreeList.FindIndex(count) ;							// ī��Ʈ�� �ش��ϴ� ��ġ ������ �޴´�.

				if( pTreePos )														// Ʈ�� ��ġ ������ ��ȿ�ϴٸ�,
				{
					pTree = (QuestTree*)pTreeList.GetAt(pTreePos) ;				// ��ġ�� �ش��ϴ� Ʈ�� ������ �޴´�.

					if( pTree )														// Ʈ�� ������ ��ȿ�ϴٸ�,
					{
						if( pTree->nMapNum == nMapNum )								// ���� ����Ʈ�� �� ��ȣ�� Ʈ���� �� ��ȣ�� ���ؼ� ������,
						{
							if( pTree->list.GetCount() == 0 )						// Ʈ���� ī��Ʈ�� 0���̸�,
							{
								pTreeList.RemoveAt(pTreePos) ;						// ���� Ʈ���� �����Ѵ�.
							}
						}
					}
				}
			}
		}
	}
}

void CQuestManager::AddQuestTitle(int nMapNum, DWORD dwQuestIdx)						// ����Ʈ ���̾�α׿� ����Ʈ ������ �߰��ϴ� �Լ�.
{
	if( dwQuestIdx < 1 )																// ����Ʈ �ε����� 1���� ������,
	{
		return ;																		// ���� ó���� �Ѵ�.
	}

	int nKey = 0 ;																		// ��Ʈ�� Ű ���� ���� ������ �����ϰ� 0���� �����Ѵ�.

	COMBINEKEY(dwQuestIdx, 0, nKey) ;													// ����Ʈ �ε����� 0���� Ű ���� �����Ѵ�.
	
	CQuestString* pQuestString = NULL ;													// ����Ʈ ��Ʈ�� ������ ���� �����͸� �����ϰ� null ó���� �Ѵ�.
	pQuestString = m_QuestStringTable.GetData(nKey) ;									// �Ĺ����� Ű ������ ����Ʈ ��Ʈ���� �޴´�.

	if( !pQuestString )																	// ����Ʈ ��Ʈ�� ������ ��ȿ���� �ʴٸ�,
	{
		return ;																		// ����ó���� �Ѵ�.
	}

	CQuestDialog* pQuestDlg = NULL ;													// ����Ʈ ���̾�α� ������ ���� �����͸� �����ϰ� nulló���� �Ѵ�.
	pQuestDlg = GAMEIN->GetQuestDialog() ;											// ����Ʈ ���̾�α� ������ �޴´�.

	if( pQuestDlg )
	{
		pQuestDlg->AddQuestTitle(pQuestString, nMapNum) ;
	}
}

void CQuestManager::DeleteQuestTitle(int nMapNum, DWORD dwQuestIdx)						// ����Ʈ ����Ʈ���� ����Ʈ ������ �����ϴ� �Լ�.
{
	if( dwQuestIdx < 1 )																// ����Ʈ �ε����� 1���� ������,
	{
		ASSERTMSG(0, "CQuestManager::AddQuestTitle\n����Ʈ ������ �����ϴ� ��, ����Ʈ �ε����� 1���� ���� ���� �Ѿ�Դ�.!!!") ;	// ASSERT ���� ó��.

		return ;																		// ���� ó���� �Ѵ�.
	}

	int nKey = 0 ;																		// ��Ʈ�� Ű ���� ���� ������ �����ϰ� 0���� �����Ѵ�.

	COMBINEKEY(dwQuestIdx, 0, nKey) ;													// ����Ʈ �ε����� 0���� Ű ���� �����Ѵ�.
	
	CQuestString* pQuestString = NULL ;													// ����Ʈ ��Ʈ�� ������ ���� �����͸� �����ϰ� null ó���� �Ѵ�.
	pQuestString = m_QuestStringTable.GetData(nKey) ;									// �Ĺ����� Ű ������ ����Ʈ ��Ʈ���� �޴´�.

	if( !pQuestString )																	// ����Ʈ ��Ʈ�� ������ ��ȿ���� �ʴٸ�,
	{
		ASSERTMSG(0, "CQuestManager::AddQuestTitle\n����Ʈ ������ �����ϴ� �� ����Ʈ ��Ʈ�� ������ ��ȿ���� �ʴ�.") ;	// ASSERT ���� ó��.

		return ;																		// ����ó���� �Ѵ�.
	}

	CQuestTotalDialog * pTotalDialog = NULL ;											// ����Ʈ ��Ż ������ ���� �����͸� �����ϰ� null ó���� �Ѵ�.
	pTotalDialog = GAMEIN->GetQuestTotalDialog() ;										// ����Ʈ ��Ż ���̾�α� ������ �޴´�.

	if( !pTotalDialog )
	{
		ASSERTMSG(0, "CQuestManager::AddQuestTitle\n����Ʈ ������ �����ϴ� �� ����Ʈ ��Ż ������ ��ȿ���� �ʴ�.") ;	// ASSERT ���� ó��.
		
		return ;																		// ���� ó���� �Ѵ�.
	}

	CQuestDialog* pQuestDlg = NULL ;													// ����Ʈ ���̾�α� ������ ���� �����͸� �����ϰ� nulló���� �Ѵ�.
	pQuestDlg = pTotalDialog->GetQuestDlg() ;											// ����Ʈ ���̾�α� ������ �޴´�.

	if( !pQuestDlg )
	{
		ASSERTMSG(0, "CQuestManager::AddQuestTitle\n����Ʈ ������ �����ϴ� �� ����Ʈ ���̾�α� ������ ���� ���ߴ�.") ;	// ASSERT ���� ó��.
		
		return ;																		// ���� ó���� �Ѵ�.
	}

	cPtrList& pTreeList = pQuestDlg->GetTreeList() ;												// Ʈ�� ����Ʈ�� �޴´�.			
	BOOL bResult = FALSE ;																// �˻� ����� ���� ������ �����ϰ� FALSE ó���� �Ѵ�.
	QuestTree* pTree = NULL ;															// Ʈ�� ������ ���� �����͸� �����ϰ� NULL ó���� �Ѵ�.
	PTRLISTPOS pTreePos = NULL ;														// Ʈ���� ��ġ ������ ���� �����͸� �����ϰ� NULLó���� �Ѵ�.
	PTRLISTPOS pDeletePos = NULL ;														// ������ ����Ʈ ��Ʈ�� ��ġ�� ��Ÿ���� �����͸� �����ϰ� NULL ó���� �Ѵ�.

	for(int count = 0 ; count < pTreeList.GetCount(); ++count)
	{
		pTreePos = pTreeList.FindIndex(count) ;										// ī��Ʈ�� �ش��ϴ� ��ġ ������ �޴´�.

		if( pTreePos )																	// Ʈ�� ��ġ ������ ��ȿ�ϴٸ�,
		{
			pTree = (QuestTree*)pTreeList.GetAt(pTreePos) ;							// ��ġ�� �ش��ϴ� Ʈ�� ������ �޴´�.

			if( pTree )																	// Ʈ�� ������ ��ȿ�ϴٸ�,
			{
				if( pTree->nMapNum == nMapNum )											// ���� ����Ʈ�� �� ��ȣ�� Ʈ���� �� ��ȣ�� ���ؼ� ������,
				{
					int nQuestCount = pTree->list.GetCount() ;							// ����Ʈ ���� ī��Ʈ�� �޴´�.

					CQuestString* pDelete = NULL ;										// ���� �� ����Ʈ ��Ʈ���� ���� �����͸� �����ϰ� null ó���� �Ѵ�.

					for( int count = 0 ; count < nQuestCount ; ++count )				// ����Ʈ ���� �� ��ŭ for���� ������.
					{
						pDeletePos = pTree->list.FindIndex(count) ;						// ī��Ʈ�� �ش��ϴ� ���� ��ġ�� �޴´�.

						if( pDeletePos )												// ���� ��ġ ������ ��ȿ�ϸ�,
						{
							pDelete = (CQuestString*)pTree->list.GetAt(pDeletePos) ;	// ���� �� ��Ʈ�� ������ �޴´�.

							if( pDelete )												// ��Ʈ�� ������ ��ȿ�ϸ�,
							{
								if( pDelete->GetKey() == pQuestString->GetKey() )		// Ʈ���� ��Ʈ����, ������ ��Ʈ���� Ű ���� ������,
								{
									bResult = TRUE ;									// ��� ���� TRUE�� �����Ѵ�.

									//pTree->list.RemoveAt(pDeletePos) ;					// Ʈ������ ��Ʈ���� �����Ѵ�.
									CQuestString* pDeleteString = (CQuestString*)pTree->list.GetAt(pDeletePos) ;

									if( pDeleteString )
									{
										pTree->list.Remove(pDeleteString) ;
										delete pDeleteString ;
									}
								}
							}
						}
					}

					break ;																// for���� ����������.
				}
			}
		}
	}

#ifdef _GMTOOL_
	if( bResult == FALSE )
	{
		OutputDebugString("CQuestManager::AddQuestTitle\n���� �ʿ� ����Ʈ�� �������� ����\n");
	}
#endif
}


void CQuestManager::StartQuest( DWORD dwQuestIdx, DWORD dwSubQuestFlag, DWORD dwData, __time64_t registTime )
{
	CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );								// ����Ʈ ���̺����� ���� ����Ʈ �ε����� ����Ʈ ������ �޴´�.

	pQuest->StartQuest( dwSubQuestFlag, dwData, registTime );							// ����Ʈ�� �����Ѵ�.

	m_QuestDialog->StartQuest(dwQuestIdx) ;

	int Key = 0;																	// Ű���� ���� ������ �����Ѱ�, 0���� �����Ѵ�.
	COMBINEKEY( dwQuestIdx, 0, Key );												// ����Ʈ �ε����� 0�� �����Ͽ� Ű ���� �޴´�.

	// title
	CQuestString* pQString = m_QuestStringTable.GetData(Key);						// Ű ������ ����Ʈ ��Ʈ�� ������ �޴´�.

	if( pQString == NULL )															// ����Ʈ ��Ʈ�� ������ ��ȿ���� �ʴٸ�,
	{
		return;																		// ���� ó���� �Ѵ�.
	}
	cPtrList* p = pQString->GetTitle();												// ����Ʈ �����͸� �����Ͽ�, ������ �޴´�.
	ITEM* string = (ITEM*)p->GetHead();										// ����Ʈ�� ��带 ����Ʈ ��Ʈ�� �����Ϳ� �޴´�.

	CHATMGR->AddMsg( CTC_QUEST, CHATMGR->GetChatMsg(221), string->string );			// ����Ʈ�� �����Ѵٴ� �޽����� ����Ѵ�.
}

void CQuestManager::EndQuest( DWORD dwQuestIdx, DWORD dwSubQuestFlag, DWORD dwData, __time64_t registTime )
{
	CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );								// ����Ʈ ���̺����� ���� ����Ʈ �ε����� ����Ʈ ������ �޴´�.

	if( !pQuest ) return ;

	if( pQuest->IsTimeCheck() )															// �ð� üũ�� �ʿ��� ����Ʈ���,
	{
		STATUSICONDLG->RemoveQuestTimeIcon( HERO, eStatusIcon_QuestTime );				// ����Ʈ �ð� �������� �����Ѵ�.
	}

	pQuest->EndQuest( dwSubQuestFlag, dwData, registTime );								// ����Ʈ�� �����Ѵ�.

	m_QuestDialog->EndQuest(dwQuestIdx) ;

	m_QuickView->DeleteQuest(dwQuestIdx) ;

	DWORD Key = 0;																	// Ű ���� ���� ������ �����Ѵ�.
	COMBINEKEY(dwQuestIdx, 0, Key);													// ����Ʈ �ε����� 0�� �����Ͽ� Ű���� �޴´�.

	// title
	CQuestString* pQString = m_QuestStringTable.GetData(Key);						// Ű ������ ����Ʈ ��Ʈ�� ���̺����� ��Ʈ�� ������ �޴´�.

	if( pQString == NULL )															// ����Ʈ ��Ʈ�� ������ ��ȿ���� üũ�Ѵ�.
	{
		return;																		// ������ ��ȿ���� ������ ���� ó���� �Ѵ�.
	}

	cPtrList* p = pQString->GetTitle();												// ����Ʈ �����ͷ� ������ �޴´�.

	ITEM* string = (ITEM*)p->GetHead();										// ����Ʈ�� ��带 �޴´�.

	CHATMGR->AddMsg( CTC_QUEST, CHATMGR->GetChatMsg(222), string->string );			// ����Ʈ �޽����� ä��â�� ����Ѵ�.
}

void CQuestManager::StartSubQuest( DWORD dwQuestIdx, DWORD dwSubQuestIdx, DWORD dwSubQuestFlag, DWORD dwSubData, DWORD dwSubTime )
{
	CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );								// ����Ʈ ���̺����� �ε����� �ش��ϴ� ����Ʈ ������ �޴´�.

	if( !pQuest ) return ;																// ����Ʈ ������ ��ȿ���� üũ�Ѵ�.

	pQuest->StartSubQuest( dwSubQuestIdx, dwSubQuestFlag, dwSubData, dwSubTime );	// ���� ����Ʈ�� �����Ѵ�.
	AddQuestNpc( pQuest->GetSubQuestInfo( dwSubQuestIdx ) );						// ����Ʈ NPC�� �߰��Ѵ�.

	m_QuestDialog->RefreshQuestList() ;
	m_QuickView->ResetQuickViewDialog();
}

void CQuestManager::EndSubQuest( DWORD dwQuestIdx, DWORD dwSubQuestIdx, DWORD dwSubQuestFlag, DWORD dwSubData, DWORD dwSubTime )
{
	CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );								// ����Ʈ �ε����� ����Ʈ ���̺����� ����Ʈ ������ �޴´�.

	if( !pQuest ) return ;																// ����Ʈ ������ ��ȿ���� üũ�Ѵ�.

	pQuest->EndSubQuest( dwSubQuestIdx, dwSubQuestFlag, dwSubData, dwSubTime );			// ���� ����Ʈ�� ���� �Ѵ�.

	m_QuestDialog->RefreshQuestList() ;
}

void CQuestManager::UpdateSubQuestData( DWORD dwQuestIdx, DWORD dwSubQuestIdx, DWORD dwMaxCount, DWORD dwSubData, DWORD dwSubTime )
{
	CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );								// ����Ʈ �ε�����, ����Ʈ���̺����� ����Ʈ ������ �޴´�.

	if( !pQuest ) return ;																// ����Ʈ ������ ��ȿ���� üũ�Ѵ�.

	pQuest->UpdateSubQuestData( dwSubQuestIdx, dwMaxCount, dwSubData, dwSubTime );		// ���� ����Ʈ ������ ������Ʈ �Ѵ�.

	m_QuestDialog->RefreshQuestList() ;
	m_QuickView->ResetQuickViewDialog();
}

void CQuestManager::QuestItemAdd( DWORD dwItemIdx, DWORD dwCount )						// ����Ʈ �������� �߰��ϴ� �Լ�.
{
	QUEST_ITEM_INFO* pData = m_QuestItemMGR.GetQuestItem( dwItemIdx );					// ����Ʈ ������ �Ŵ������� ������ �ε�����, ����Ʈ ������ ������ �޴´�.

	if( !pData ) return;																// ������ ������ ��ȿ���� ������, ���� ó���� �Ѵ�.

	m_QuestUpdater->QuestItemAdd( pData, dwCount );										// ����Ʈ �������Ϳ� ����Ʈ �������� �߰��Ѵ�.

	CHATMGR->AddMsg( CTC_QUEST, CHATMGR->GetChatMsg(224), pData->ItemName );			// ä��â��, ������ ���� ����Ѵ�.
}

void CQuestManager::QuestItemDelete( DWORD dwItemIdx )									// ����Ʈ ������ ���� �Լ�.
{
	// itemname
	QUEST_ITEM_INFO* pData = m_QuestItemMGR.GetQuestItem( dwItemIdx );					// ������ �ε�����, ����Ʈ ������ �Ŵ������� ����Ʈ ������ ������ �޴´�.

	if( !pData ) return;																// ������ ������ ��ȿ���� ������, ���� ó���� �Ѵ�.

	CHATMGR->AddMsg( CTC_QUEST, CHATMGR->GetChatMsg(225), pData->ItemName );			// ä��â�� �����۸��� �޽����� �����Ͽ� �߰��Ѵ�.

	m_QuestUpdater->QuestItemDelete( dwItemIdx );										// ����Ʈ �������Ϳ��� ����Ʈ �������� �����Ѵ�.
}

void CQuestManager::QuestItemUpdate( DWORD dwType, DWORD dwItemIdx, DWORD dwCount )		// ����Ʈ �������� ������Ʈ �ϴ� �Լ�.
{
	m_QuestUpdater->QuestItemUpdate( dwType, dwItemIdx, dwCount);						// ����Ʈ �������Ϳ��� ����Ʈ �������� ������Ʈ �Ѵ�.

	// itemname
	QUEST_ITEM_INFO* pData = m_QuestItemMGR.GetQuestItem( dwItemIdx );					// ���� ������ �ε����� ����Ʈ ������ �Ŵ������� ����Ʈ ������ ������ �޴´�.

	if( !pData ) return;																// ������ ������ ��ȿ���� ������, ����ó���� �Ѵ�.

	m_QuestUpdater->QuestItemAdd( pData, dwCount );										// ����Ʈ �������Ϳ� �������� �߰��Ѵ�.

	CHATMGR->AddMsg( CTC_QUEST, CHATMGR->GetChatMsg(224), pData->ItemName );			// ä��â�� ������ �̸��� �޽����� �����Ͽ� ����Ѵ�.
}

QUEST_ITEM_INFO* CQuestManager::GetQuestItem( DWORD dwItemIdx )
{
	return m_QuestItemMGR.GetQuestItem( dwItemIdx );
}

BOOL CQuestManager::IsQuestComplete( DWORD dwQuestIdx )									// ����Ʈ �Ϸ� ���θ� ��ȯ�ϴ� �Լ�.
{
	CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );								// ���� �ε����� ����Ʈ ���̺����� ����Ʈ ������ �޴´�.

	if( pQuest )																		// ����Ʈ ������ ��ȿ���� üũ�Ѵ�.
	{
		if( pQuest->IsQuestComplete() )													// ����Ʈ�� �Ϸ� �Ǿ��ٸ�,
		{
			return TRUE;																// TRUE�� �����Ѵ�.
		}
		else																			// ����Ʈ�� �Ϸ���� �ʾҴٸ�,
		{
			return FALSE;																// FALSE�� �����Ѵ�.
		}
	}

	return FALSE;																		// FALSE�� �����Ѵ�.
}

void CQuestManager::DeleteQuest( DWORD dwQuestIdx, BOOL bCheckTime )					// ����Ʈ�� �����ϴ� �Լ�.
{
	CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );								// ���� �ε����� ����Ʈ ���̺����� ����Ʈ ������ �޴´�.

	if( !pQuest ) return ;																// ����Ʈ ������ ��ȿ���� üũ�Ѵ�.

	if( pQuest->IsQuestComplete() )
	{
		if( pQuest->HasDateLimit() )
		{
			// ����Ʈ ����/�ʱ�ȭ
			pQuest->Initialize();
			m_QuestUpdater->DeleteQuest( dwQuestIdx );
			m_QuestDialog->EndQuest( dwQuestIdx );
			m_QuickView->DeleteQuest( dwQuestIdx );

			// ��ȿ �ð� ���°� ����� ��츦 �˷��ش�.
			NotifyChangedDateTime( dwQuestIdx );
		}

		return;
	}

	pQuest->Initialize();																// ����Ʈ�� �ʱ�ȭ �Ѵ�.
	m_QuestUpdater->DeleteQuest( dwQuestIdx );											// ����Ʈ �������Ϳ��� ����Ʈ�� �����Ѵ�.

	m_QuestDialog->EndQuest(dwQuestIdx) ;
	m_QuickView->DeleteQuest(dwQuestIdx) ;

	int Key = 0;																	// Ű ���� ���� ������ �����ϰ�, 0���� �����Ѵ�.
	COMBINEKEY( dwQuestIdx, 0, Key );												// ����Ʈ �ε����� 0���� Ű���� �޴´�.

	CQuestString* pQString = m_QuestStringTable.GetData(Key);						// Ű ������ ����Ʈ ��Ʈ�� ���̺����� ����Ʈ ��Ʈ�� ������ �޴´�.

	if( pQString == NULL )	return;													// ����Ʈ ������ ��ȿ���� ������, ���� ó���� �Ѵ�.

	cPtrList* p = pQString->GetTitle();												// ����Ʈ �����͸� �����ϰ�, ��Ʈ���� ������ �޴´�.
	ITEM* string = (ITEM*)p->GetHead();										// ����Ʈ �����͸� ���� �����Ͽ� ��Ʈ�� ������ ������ �޴´�.

	if( pQuest->IsTimeCheck() && bCheckTime )										// ����Ʈ�� �ð��� üũ�Ѵٸ�,
	{
		CHATMGR->AddMsg( CTC_QUEST, CHATMGR->GetChatMsg(295), string->string );		// ����ð� �ʰ��� ����Ǿ��ٴ� �޽����� ����Ѵ�.

		char msg[256] = {0, };														// �ӽ� ���۸� �����Ѵ�.
		CompositString( msg, CHATMGR->GetChatMsg(295), string->string );				// ���� �޽����� �����.

		STATUSICONDLG->RemoveQuestTimeIcon( HERO, eStatusIcon_QuestTime );			// �ð�üũ �������� �����.
	}
	else																			// �ð�üũ�� ���� �ʴ´ٸ�,
	{
		CHATMGR->AddMsg( CTC_QUEST, CHATMGR->GetChatMsg(223), string->string );		// ����Ʈ�� �����ߴٴ� �޽����� ä��â�� ����Ѵ�.

		char msg[256] = {0, };														// �ӽ� ���۸� �����Ѵ�.
		CompositString( msg, CHATMGR->GetChatMsg(223), string->string );				// ���� �޽����� �����.

		STATUSICONDLG->RemoveQuestTimeIcon( HERO, eStatusIcon_QuestTime );			// ����Ʈ �ð� �������� �����Ѵ�.
	}
}

BOOL CQuestManager::IsNpcRelationQuest( DWORD dwNpcIdx )								// NPC�� ����� ����Ʈ���� ��ȯ�ϴ� �Լ�.
{
	m_QuestTable.SetPositionHead();

	for(CQuest* pQuest = m_QuestTable.GetData();
		0 < pQuest;
		pQuest = m_QuestTable.GetData())
	{
		if( pQuest->IsNpcRelationQuest( dwNpcIdx ) )									// npc �ε����� ����Ʈ�� npc�� ����� ����Ʈ���� üũ�Ѵ�.
		{
			return TRUE;																// npc�� ������ ������, TRUE�� �����Ѵ�.
		}
	}

	return FALSE;																		// FALSE�� �����Ѵ�.
}

DWORD CQuestManager::GetNpcScriptPage( DWORD dwNpcIdx )									// NPC ��ũ��Ʈ �������� �����ϴ� �Լ�.
{
	m_QuestTable.SetPositionHead();

	for(CQuest* pQuest = m_QuestTable.GetData();
		0 < pQuest;
		pQuest = m_QuestTable.GetData())
	{
		const DWORD dwPage = pQuest->GetNpcScriptPage( dwNpcIdx );

		if( dwPage )																	// ������ ������ ��ȿ�ϴٸ�,
		{
			return dwPage;																// �������� �����Ѵ�.
		}
	}

	return 0;																			// 0�� �����Ѵ�.
}

void CQuestManager::SendNpcTalkEvent( DWORD dwNpcIdx, DWORD dwNpcScriptIdx, DWORD dwRequitalIndex, DWORD dwRequitalCount )			// npc ��ȭ �̺�Ʈ�� ������ �Լ�.
{
	// 100414 ONS �޼����� ���ú��������� �߰��Ѵ�.
	SEND_QUEST_REQUITAL_IDX Msg;																	// ����Ʈ ������ ���� �޽��� ����ü�� �����Ѵ�.

	ZeroMemory( &Msg, sizeof(Msg) );
	Msg.Category = MP_QUEST;															// ī�װ����� ����Ʈ�� �����Ѵ�.
	Msg.Protocol = MP_QUESTEVENT_NPCTALK;												// ���������� ����Ʈ �̺�Ʈ �� npc ��ȭ�� �����Ѵ�.
	Msg.dwObjectID = gHeroID;
	Msg.MainQuestIdx = (WORD)dwNpcIdx;													// ���� ����Ʈ �ε����� �����Ѵ�.
	Msg.SubQuestIdx = (WORD)dwNpcScriptIdx;												// ���� ����Ʈ �ε����� �����Ѵ�.
	Msg.dwRequitalIdx = dwRequitalIndex;
	Msg.dwRequitalCount = dwRequitalCount;

	NETWORK->Send( &Msg, sizeof(Msg) );													// ���� �� ������ �޽����� �����Ѵ�.
}

void CQuestManager::SendNpcTalkEventForEvent( DWORD dwNpcIdx, DWORD dwNpcScriptIdx )	// �̺�Ʈ�� ���� npc��ȭ �̺�Ʈ�� ������ �Լ�.
{
	SEND_QUEST_IDX Msg;																	// ����Ʈ ������ ���� �޽��� ����ü�� �����Ѵ�.

	Msg.Category = MP_QUEST;															// ī�װ����� ����Ʈ�� �����Ѵ�.
	Msg.Protocol = MP_QUESTEVENT_NPCTALK_SYN;											// ���������� ����Ʈ �̺�Ʈ �� npc ��ȭ ��û���� �����Ѵ�.
	Msg.dwObjectID = gHeroID;
	Msg.MainQuestIdx = (WORD)dwNpcIdx;													// npc �ε����� �����Ѵ�.
	Msg.SubQuestIdx = (WORD)dwNpcScriptIdx;												// npc ��ũ��Ʈ �ε����� �����Ѵ�.

	NETWORK->Send( &Msg, sizeof(Msg) );													// ���� �� ������ �޽����� �����Ѵ�.
}

void CQuestManager::SetNpcData( CNpc* pNpc )											// npc �����͸� �����ϴ� �Լ�.
{
	if( m_dwMaxNpc > MAX_NPCARRAY-1 )													// �ִ� npc �ε����� 31���� ū�� üũ�Ѵ�.
	{
		return;																			// ũ�ٸ�, ����ó���� �Ѵ�.
	}

	m_pNpcArray[m_dwMaxNpc] = pNpc;														// npc ������ ��� �迭 �ε����� npc ������ ��´�.

	++m_dwMaxNpc;																		// �ִ� npc ��ȣ�� ������Ų��.
}

void CQuestManager::SetNpcIDName(WORD wIdx, stNPCIDNAME* pInfo)							// npc �̸��� ���̵� �����ϴ� �Լ�.
{
	if( wIdx > MAX_NPCIDNAME-1 )
	{
		ASSERTMSG(0, "����ƽ NPC ������ 100���� �Ѿ���, MAX_NPCIDNAME ��ġ�� �÷��ֽÿ�.") ;
		return ;
	}

	m_stNPCIDNAME[wIdx].wId = pInfo->wId ;
	strcpy(m_stNPCIDNAME[wIdx].name, pInfo->name) ;
}

stNPCIDNAME* CQuestManager::GetNpcIdName(DWORD npcIdx)
{
	if( npcIdx < 0 || npcIdx > MAX_NPCIDNAME-1 )
	{
		return NULL ;
	}

	for( int count = 0 ; count < MAX_NPCIDNAME ; ++count )
	{
		if( m_stNPCIDNAME[count].wId == (WORD)npcIdx )
		{
			return &m_stNPCIDNAME[count] ;
		}
	}

	return NULL ;
}

QUESTNPCINFO* CQuestManager::GetQusetNpcInfo(DWORD npcIdx)
{
	return m_QuestNpcTable.GetData( npcIdx );
}



void CQuestManager::ProcessNpcMark()
{
	GAMEIN->GetMiniMapDialog()->UpdateQuestMark();
	GAMEIN->GetWorldMapDialog()->GetBigMapDlg()->UpdateQuestMark();
}

eQUEST_MARK CQuestManager::GetQuestMarkState( DWORD dwUniqueNpcIdx, DWORD dwNpcObjectIdx )
{
	std::map< DWORD, NpcHasQuestList >::const_iterator questOwnedByNpcIter = m_NpcHasQuestList.find( dwUniqueNpcIdx );

	if( questOwnedByNpcIter == m_NpcHasQuestList.end() )
		return eQM_NONE;

	eQUEST_MARK mark = eQM_NONE;

	STATIC_NPCINFO* pStaticNpcInfo = GAMERESRCMNGR->GetStaticNpcInfo( (WORD)dwUniqueNpcIdx );
	if( !pStaticNpcInfo )
		return eQM_NONE;

	const WORD wNpcKind = pStaticNpcInfo->wNpcJob;
	NPC_LIST* pNpcList = GAMERESRCMNGR->GetNpcInfo( wNpcKind );
	if( !pNpcList )
		return eQM_NONE;

	const NpcHasQuestList& hasQuestList = questOwnedByNpcIter->second;

	for( NpcHasQuestList::const_iterator hasQuestListIter = hasQuestList.begin() ; hasQuestListIter != hasQuestList.end() ; ++hasQuestListIter )
	{
		const DWORD dwQuestIdx = *hasQuestListIter;
		CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );
		if( !pQuest )
			continue;

		switch( pQuest->GetNpcMarkType( dwUniqueNpcIdx ) )						// NPC�� ��ũ Ÿ���� Ȯ���Ѵ�.
		{
			// ��ũ Ÿ���� !���
		case eQM_EXCLAMATION:
			{
				if( pQuest->HasDateLimit() &&
					mark != eQM_EXCLAMATION )
				{
					mark = eQM_EXCLAMATION_HAS_TIMEQUEST;
				}
				else
				{
					mark = eQM_EXCLAMATION;
				}
			}
			break;
			// ��ũ Ÿ���� ?���
		case eQM_QUESTION:
			{
				if( pQuest->HasDateLimit() &&
					mark != eQM_EXCLAMATION &&
					mark != eQM_EXCLAMATION_HAS_TIMEQUEST &&
					mark != eQM_QUESTION )
				{
					mark = eQM_QUESTION_HAS_TIMEQUEST;
				}
				else if( mark != eQM_EXCLAMATION &&
						 mark != eQM_EXCLAMATION_HAS_TIMEQUEST )
				{
					mark = eQM_QUESTION;
				}
			}
			break;
		}
	}

	CNpc* pNpc = (CNpc*)OBJECTMGR->GetObject( dwNpcObjectIdx );
	if( pNpc != NULL && pNpc->GetObjectKind() == eObjectKind_Npc )
	{
		OBJECTEFFECTDESC desc;
		DWORD dwEffectID = 0;
		
		switch( mark )
		{
		case eQM_EXCLAMATION:
			{
				desc.SetDesc( 17 );
				dwEffectID = NPCMARK_1_ID;
			}
			break;
		case eQM_QUESTION:
			{
				desc.SetDesc( 18 );
				dwEffectID = NPCMARK_2_ID;
			}
			break;
		case eQM_EXCLAMATION_HAS_TIMEQUEST:
			{
				desc.SetDesc( 9525 );
				dwEffectID = NPCMARK_3_ID;
			}
			break;
		case eQM_QUESTION_HAS_TIMEQUEST:
			{
				desc.SetDesc( 9526 );
				dwEffectID = NPCMARK_4_ID;
			}
			break;
		}

		if( pNpc->IsNpcMark( dwEffectID ) )
			return mark;

		pNpc->RemoveObjectEffect( NPCMARK_1_ID );	// !
		pNpc->RemoveObjectEffect( NPCMARK_2_ID );	// ?
		pNpc->RemoveObjectEffect( NPCMARK_3_ID );	// �ð����� !
		pNpc->RemoveObjectEffect( NPCMARK_4_ID );	// �ð����� ?

		desc.Position.x = 0.f;
		desc.Position.y = float( pNpcList->Tall ) + 50.0f;
		desc.Position.z = 0.f;
		pNpc->AddObjectEffect( dwEffectID, &desc, 1, HERO );
	}

	return mark;
}

void CQuestManager::NetworkMsgParse( BYTE Protocol, void* pMsg )									// ����Ʈ ��Ʈ��ũ �޽��� �Ľ��Լ�.
{
	if(!pMsg) return ;																				// ���ڷ� �Ѿ�� �޽��� ������ ��ȿ���� ������, return ó���� �Ѵ�.

	switch(Protocol)																				// �������� ������ Ȯ���Ѵ�.
	{
	case MP_QUEST_MAINDATA_LOAD:				Quest_Maindata_Load( pMsg ) ;			break;		// ���� ������ �ε� ó���� �Ѵ�.
	case MP_QUEST_SUBDATA_LOAD:					Quest_Subdata_Load( pMsg ) ;			break;		// ���� ������ �ε� ó���� �Ѵ�.
	case MP_QUEST_ITEM_LOAD:					Quest_Item_Load( pMsg ) ;				break;		// ������ �ε� ó���� �Ѵ�.
	case MP_QUEST_START_ACK:					Quest_Start_Ack( pMsg ) ;				break;		// ����Ʈ ���� ó���� �Ѵ�.
	case MP_SUBQUEST_START_ACK:					SubQuest_Start_Ack( pMsg ) ;			break;		// ���� ����Ʈ ���� ó���� �Ѵ�.
	case MP_QUEST_END_ACK:						Quest_End_Ack( pMsg )  ;				break;		// ����Ʈ ���� ó���� �Ѵ�.
	case MP_SUBQUEST_END_ACK:					SubQuest_End_Ack( pMsg ) ;				break;		// ���� ����Ʈ ���� ó���� �Ѵ�.
	case MP_SUBQUEST_UPDATE:					SubQuest_Update( pMsg ) ;				break;		// ���� ����Ʈ ������Ʈ ó���� �Ѵ�.
	case MP_QUEST_DELETE_ACK:					Quest_Delete_Ack( pMsg ) ;				break;		// ����Ʈ ������ ó���Ѵ�.
	case MP_QUESTITEM_GIVE:						QuestItem_Give( pMsg ) ;				break;		// ����Ʈ �������� �ִ� ó���� �Ѵ�.
	case MP_QUESTITEM_TAKE:						QuestItem_Take( pMsg ) ;				break;		// ����Ʈ �������� �����ϴ� ó���� �Ѵ�.
	case MP_QUESTITEM_UPDATE:					QuestItem_Update( pMsg ) ;				break;		// ����Ʈ ������ ������Ʈ ó���� �Ѵ�.
	case MP_QUEST_TAKEITEM_ACK:					Quest_TakeItem_Ack( pMsg ) ;			break;		// ����Ʈ ������ ���� ó���� �Ѵ�.
	case MP_QUEST_TAKEMONEY_ACK:				Quest_Takemoney_Ack( pMsg ) ;			break;		// ����Ʈ �Ӵ� ȹ�� ó���� �Ѵ�.
	case MP_QUEST_TAKEEXP_ACK:					Quest_TakeExp_Ack( pMsg ) ;				break;		// ����Ʈ ����ġ ȹ�� ó���� �Ѵ�.
	case MP_QUEST_TAKESEXP_ACK:					Quest_TakesExp_Ack( pMsg ) ;			break;		// ����Ʈ ����ġ ȹ�� ó���� �Ѵ�.
	case MP_QUEST_GIVEITEM_ACK:					Quest_GiveItem_Ack( pMsg ) ;			break;		// ����Ʈ ������ �й� ó���� �Ѵ�.
	// 071026 LYW --- QuestManager : Add process part of give stack item.
	case MP_QUEST_GIVEITEMS_ACK :				Quest_GiveItems_Ack( pMsg ) ;			break ;		// ����Ʈ ���� ������ �й� ó���� �Ѵ�.
	case MP_QUEST_GIVEMONEY_ACK:				Quest_GiveMoney_Ack( pMsg ) ;			break;		// ����Ʈ �Ӵ� �й� ó���� �Ѵ�.
	case MP_QUEST_TIME_LIMIT:					Quest_Time_Limit( pMsg ) ;				break;		// ����Ʈ �ð� ���� ó���� �Ѵ�.
	case MP_QUEST_REGIST_CHECKTIME:				Quest_Regist_Checktime( pMsg ) ;		break;		// ����Ʈ �ð� üũ ��� ó���� �Ѵ�.
	case MP_QUEST_UNREGIST_CHECKTIME:			Quest_Unregist_Checktime( pMsg ) ;		break;		// ����Ʈ �ð� üũ Ż�� ó���� �Ѵ�.
	case MP_QUESTEVENT_NPCTALK_ACK:				QuestEvent_NpcTalk_Ack( pMsg ) ;		break;		// ����Ʈ �̺�Ʈ npc ��ȭ ���� ó���� �Ѵ�.
	case MP_QUESTEVENT_NPCTALK_NACK:			QuestEvent_NpcTalk_Nack( pMsg ) ;		break;		// ����Ʈ �̺�Ʈ npc ��ȭ ���� ó���� �Ѵ�.
	case MP_QUEST_EXECUTE_ERROR:				Quest_Execute_Error( pMsg ) ;			break;		// ����Ʈ ���� ó���� �Ѵ�.
	case MP_QUEST_ERROR_EXT :					Quest_Error_Ext( pMsg ) ;				break;		// ����Ʈ ��Ÿ ���� ó���� �Ѵ�.
	case MP_QUEST_ERROR_FULLQUEST :				Quest_Error_FullQuest( pMsg ) ;			break ;		// ����Ʈ ����� �� á�ٴ� ���� �޽����� ó���Ѵ�.

	// 071028 LYW --- QuestManager : Add process part of error when failed give item.
	case MP_QUEST_GIVEITEM_ERR :				Quest_GiveItem_Error( pMsg ) ;			break ;		// ����Ʈ ������ �ݳ� ���� ó���� �Ѵ�.

	default:																			break;		// ����Ʈ ó��.
	}

	GAMEIN->GetNpcScriptDialog()->RefreshQuestLinkPage() ;
}

void CQuestManager::QuestAbandon()																	// ����Ʈ ���� ó�� �Լ�.
{
	DWORD QuestIdx = 0 ;																			// ����Ʈ �ε����� ���� ������ �����ϰ� 0���� �����Ѵ�.

	CQuestDialog* pDlg = NULL ;																		// ����Ʈ ���̾�α� ������ ���� �����͸� �����ϰ� nulló���� �Ѵ�.
	pDlg = GAMEIN->GetQuestTotalDialog()->GetQuestDlg() ;											// ����Ʈ ���̾�α� ������ �޴´�.

	if( pDlg )																						// ����Ʈ ���̾�α� ������ ��ȿ�ϴٸ�,
	{
		QuestIdx = pDlg->GetSelectedQuestID() ;														// ���� �� ����Ʈ �ε����� �޴´�.

		pDlg->SetQuestCount();
	}

	if( IsQuestComplete(QuestIdx) )																	// ����Ʈ �ε����� �Ϸ��� ����Ʈ���� üũ�Ѵ�.
	{
		return;																						// �Ϸ��� ����Ʈ���, ����ó���� �Ѵ�.
	}

	SEND_QUEST_IDX msg;																				// ����Ʈ ������ ���� ����ü�� �����Ѵ�.

	msg.Category = MP_QUEST;																		// ī�װ����� ����Ʈ�� �����Ѵ�.
	msg.Protocol = MP_QUEST_DELETE_SYN;																// ���������� ����Ʈ �輼 ��ũ�� �����Ѵ�.
	msg.dwObjectID = gHeroID;
	msg.MainQuestIdx = (WORD)QuestIdx;																// ����Ʈ �ε����� �����Ѵ�.

	NETWORK->Send(&msg, sizeof(msg));																// ���õ� ������ �����Ѵ�.

	CQuestInfo* pQuestInfo =NULL ;																	// ����Ʈ ������ ���� �����͸� �����ϰ� null ó���� �Ѵ�.
	pQuestInfo = GetQuestInfo(QuestIdx) ;															// ����Ʈ �ε����� ����Ʈ ������ �޴´�.

	if(!pQuestInfo)																					// ����Ʈ ������ ��ȿ���� ������,
	{
		ASSERTMSG(0, "����Ʈ �����ϴ� �������� ����Ʈ ������ �ε����� ���Ͽ����ϴ�.!") ;			// ASSERT ���� üũ.
		return  ;																					// ���� ó���� �Ѵ�.
	}

	int nMapNum = pQuestInfo->GetMapNum() ;															// ����Ʈ ������ �ִ� �� ��ȣ�� �޴´�.

	DeleteQuestTitle(nMapNum, QuestIdx);
	DeleteMapTree(nMapNum);
}

void CQuestManager::QuestAbandon( DWORD dwQuestIdx )												// ����Ʈ �����Լ�( ����Ʈ �ε��� ���� )
{
	if( IsQuestComplete(dwQuestIdx) )																// ���� ����Ʈ �ε����� ����Ʈ�� �Ϸ� �ߴ��� üũ�Ѵ�.
	{
		return;																						// �Ϸ��� ����Ʈ���, ���� ó���� �Ѵ�.
	}

	SEND_QUEST_IDX msg;																				// ����Ʈ ������ ���� ����ü�� �����Ѵ�.

	msg.Category = MP_QUEST;																		// ī�װ����� ����Ʈ�� �����Ѵ�.
	msg.Protocol = MP_QUEST_DELETE_SYN;																// ���������� ����Ʈ ���� ��ũ�� �����Ѵ�.
	msg.dwObjectID = gHeroID;
	msg.MainQuestIdx = (WORD)dwQuestIdx;															// ����Ʈ �ε����� �����Ѵ�.

	NETWORK->Send(&msg, sizeof(msg));																// ���õ� ������ �����Ѵ�.

	//test
	m_QuestUpdater->DeleteQuest( dwQuestIdx );														// ����Ʈ ���������� ����Ʈ ���� �Լ��� ȣ���Ѵ�.
}

void CQuestManager::AddQuestNpc( CSubQuestInfo* pSubQuestInfo )										// ����Ʈ npc�� �߰��ϴ� �Լ�.
{
	if( !pSubQuestInfo ) return;																	// ���� ����Ʈ ������ ������, ����ó���� �Ѵ�.

	cPtrList& pNpcList = pSubQuestInfo->GetQuestNpcList();
	PTRLISTPOS npos = pNpcList.GetHeadPosition();

	while( npos )																					// npc ����Ʈ�� ��ġ ������ ��� �����Ͱ� ��ȿ�� ���� while���� ������.
	{
		CQuestNpc* pNpc = (CQuestNpc*)pNpcList.GetNext( npos );

		if( pNpc )																					// npc ������ ��ȿ�ϴٸ�,
		{
			cPtrList& pList = pNpc->GetNpcDataList();
			PTRLISTPOS pos = pList.GetHeadPosition();

			while( pos )																		// ��ġ ������ ��ȿ�� ���� while���� ������.
			{
				CQuestNpcData* pData = (CQuestNpcData*)pList.GetNext( pos );					// ��ġ�� �ش��ϴ� ����Ʈ npc ������ ������ �޴´�.

				if( pData )																		// ������ ������ ��ȿ���� üũ�Ѵ�.
				{
					QUESTNPCINFO* pInfo = m_QuestNpcTable.GetData( pData->GetNpcIdx() );		// ������ ������ npc �ε����� �����Ͽ� ����Ʈ npc ������ �޴´�.

					if( pInfo )																	// npc ������ ��ȿ���� üũ�Ѵ�.
					{
						MAP->AddQuestNpc( pInfo );												// �ʿ� ����Ʈ npc�� �߰��Ѵ�.
					}
				}
			}
		}
	}
}

BOOL CQuestManager::IsQuestStarted( DWORD dwQuestIdx )												// ����Ʈ�� ���۵Ǿ����� ���θ� �����ϴ� �Լ�.
{
	CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );											// ���� ����Ʈ �ε�����, ����Ʈ ������ �޴´�.

	if( pQuest )																					// ����Ʈ ������ ��ȿ���� üũ�Ѵ�.
	{
		if( pQuest->IsSubQuestComplete( 0 ) )														// ����Ʈ�� ���� ����Ʈ�� �Ϸ� �Ǿ��ٸ�,
		{
			return FALSE;																			// FALSE�� �����Ѵ�.
		}
		else																						// �׷��� ������,
		{
			return pQuest->CanItemUse( dwQuestIdx );												// �������� ����� �� �ִ��� ���θ� �����Ѵ�.
		}
	}
	return FALSE;																					// FALSE�� �����Ѵ�.
}

char* CQuestManager::GetQuestTitle( DWORD dwQuestIdx )												// ����Ʈ Ÿ��Ʋ�� �����ϴ� �Լ�.
{
	CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );											// ���� ����Ʈ �ε����� ����Ʈ ������ �޴´�.

	if( !pQuest )	return NULL;																	// ����Ʈ ������ ��ȿ���� ������, NULL�� �����Ѵ�.

	//
	int Key = 0;																					// Ű ���� ���� ������ �����ϰ� 0���� �����Ѵ�.

	COMBINEKEY( dwQuestIdx, 0, Key );																// ����Ʈ �ε����� 0�� �����Ͽ� Ű���� �޴´�.

	CQuestString* pQString = m_QuestStringTable.GetData(Key);										// Ű ������ ����Ʈ ��Ʈ�� ������ �޴´�.

	if( pQString == NULL )	return NULL;															// ����Ʈ ��Ʈ�� ������ ��ȿ���� ������, NULL�� �����Ѵ�.

	cPtrList* p = pQString->GetTitle();																// ����Ʈ ��Ʈ���� Ÿ��Ʋ�� ����Ʈ �����ͷ� �޴´�.

	QString* string = (QString*)p->GetHead();														// ����Ʈ�� �ص带 ��Ʈ�� �����ͷ� �޴´�.

	return string->Str;																				// ��Ʈ�� �������� ��Ʈ���� �����Ѵ�.
}

CQuestString* CQuestManager::GetQuestString(DWORD dwMainQuestIdx, DWORD dwSubQuestIdx)				// ����Ʈ ��ȣ�� ���� �� ��ȣ�� ����Ʈ ��Ʈ���� ��ȯ�ϴ� �Լ�.
{
	int nCombineKey = 0 ;																			// �Ĺ��� Ű ���� ���� ������ �����ϰ� 0���� �����Ѵ�.

	COMBINEKEY( dwMainQuestIdx, dwSubQuestIdx, nCombineKey) ;										// ������ ��ȣ�� ������ ��ȣ�� �Ĺ��� Ű�� �����.

	CQuestString* pQuestString = m_QuestStringTable.GetData(nCombineKey) ;							// �Ĺ��� Ű�� ����Ʈ ��Ʈ�� ���̺����� ����Ʈ ��Ʈ���� �޴´�.

	return pQuestString ;																			// ����Ʈ ��Ʈ���� ��ȯ�Ѵ�.
}

void CQuestManager::QuestTimeLimit( DWORD dwQuestIdx, DWORD dwTime )								// ����Ʈ�� �ð��� �����ϴ� �Լ�.
{
	CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );											// ���� �ε����� ����Ʈ ������ �޴´�.

	if( !pQuest )	return;																			// ����Ʈ ������ ��ȿ���� ������ ���� ó���� �Ѵ�.

	//
	stTIME time;																					// �ð� ������ ���� ����ü�� �����Ѵ�.
	time.value = dwTime;																			// ���� �ð����� �����Ѵ�.

	int Key = 0;																					// Ű ���� ���� ������ �����ϰ� 0���� �����Ѵ�.

	COMBINEKEY( dwQuestIdx, 0, Key );																// ����Ʈ �ε����� 0�� �����Ͽ� Ű ���� �޴´�.

	CQuestString* pQString = m_QuestStringTable.GetData(Key);										// ���� Ű ���� ������ ����Ʈ ��Ʈ�� ������ �޴´�.

	if( pQString == NULL )	return;																	// ����Ʈ ��Ʈ�� ������ ��ȿ���� ������ ���� ó���� �Ѵ�.

	cPtrList* p = pQString->GetTitle();																// ��Ʈ���� Ÿ��Ʋ�� ����Ʈ �����ͷ� �޴´�.

	QString* string = (QString*)p->GetHead();														// ����Ʈ�� ��带 ��Ʈ�� �����ͷ� �޴´�.

	CHATMGR->AddMsg( CTC_QUEST, CHATMGR->GetChatMsg(299), string->Str,								// �ð� ������ �޽����� �����Ͽ�, ä��â���� ����Ѵ�.
					 time.GetYear()+2000, time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute() );	

	char msg[256] = {0, };																			// �ӽ� ���۸� �����Ѵ�.

	CompositString( msg, CHATMGR->GetChatMsg(299), string->Str,										// �ð� ������ �޽����� �����Ͽ�, ����� �޽����� �ϼ��Ѵ�.
					 time.GetYear()+2000, time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute() );	

	STATUSICONDLG->RemoveQuestTimeIcon( HERO, eStatusIcon_QuestTime );								// ����Ʈ�� �ð����� �������� �����Ѵ�.
}

void CQuestManager::RegistCheckTime( DWORD dwQuestIdx, BYTE bCheckType, DWORD dwCheckTime )			// �ð� üũ ����� �ϴ� �Լ�.
{
	CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );											// ���� ����Ʈ �ε����� ����Ʈ ������ �޴´�.

	if( !pQuest )	return;																			// ����Ʈ ������ ��ȿ���� ������ ���� ó���� �Ѵ�.

	pQuest->RegistCheckTime( bCheckType, dwCheckTime );												// ����Ʈ ������ �ð� üũ ���� ������ �����Ѵ�.

	STATUSICONDLG->AddQuestTimeIcon( HERO, eStatusIcon_QuestTime );									// ����Ʈ �ð� �������� �߰��Ѵ�.
}

void CQuestManager::UnRegistCheckTime( DWORD dwQuestIdx )											// �ð� üũ ����� �����ϴ� �Լ�.
{
	CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );											// ���� ����Ʈ �ε����� ����Ʈ ������ �޴´�.

	if( !pQuest )	return;																			// ����Ʈ ������ ��ȿ���� ������ ���� ó���� �Ѵ�.

	pQuest->UnRegistCheckTime();																	// ����Ʈ �������� UnRegistCheckTime�Լ��� ȣ���Ѵ�.

	STATUSICONDLG->RemoveQuestTimeIcon( HERO, eStatusIcon_QuestTime );								// ����Ʈ �ð� �������� �����Ѵ�.
}

void CQuestManager::QuestEventAck( DWORD dwQuestIdx )												// ����Ʈ �̺�Ʈ ��û �Լ�.
{
	WINDOWMGR->MsgBox( MBI_QUESTMSG, MBT_OK, "�⼮üũ�� �Ǿ����ϴ�." );							// �⼮ üũ�� �Ǿ��ٴ� �޽����� ����Ѵ�.
}

void CQuestManager::QuestEventNack( DWORD dwQuestIdx, DWORD dwError )								// ����Ʈ �̺�Ʈ ���� �Լ�.
{
	switch( dwError )																				// ������ Ȯ���Ѵ�.
	{
	case 1:																							// ������ 1���� ������,
		{
			WINDOWMGR->MsgBox( MBI_QUESTMSG, MBT_OK, "�̹� �⼮üũ�� �Ǿ� ������ ������ �� �����ϴ�." );	// �޽����� ����Ѵ�.
		}
		break;
	case 2:																							// ������ 2���� ������,
		{
			WINDOWMGR->MsgBox( MBI_QUESTMSG, MBT_OK, "Ȩ������ �⼮üũ�� �Ǿ� ���� �ʾ� ������ ������ �� �����ϴ�." );	// �޽����� ����Ѵ�.
		}
		break;
	}
}
void CQuestManager::Quest_Maindata_Load( void* pMsg )												// ���� �����͸� �ε��ϴ� �Լ�.
{
	SEND_MAINQUEST_DATA* pmsg = (SEND_MAINQUEST_DATA*)pMsg;											// ���� ����Ʈ �����͸� �޴� ������ ������ �����ϰ� �޽����� �޴´�.

	if(pmsg->wCount==0)		return;																	// ī��Ʈ�� 0�� ������ ���� ó���� �Ѵ�.
	
	for(int i=0; i<pmsg->wCount; i++)																// ī��Ʈ �� ��ŭ for���� ������.
	{
		SetMainQuestData( pmsg->QuestList[i].QuestIdx, pmsg->QuestList[i].state.value,				// ���� ����Ʈ �����͸� �����Ѵ�.
			pmsg->QuestList[i].EndParam, pmsg->QuestList[i].registTime, pmsg->QuestList[i].CheckType, 
			pmsg->QuestList[i].CheckTime );
	}

	ProcessNpcMark();																				// npc ��ũ ���μ����� �����Ѵ�.
}

void CQuestManager::Quest_Subdata_Load( void* pMsg )												// ���� �����͸� �ε��ϴ� �Լ�.
{
	SEND_SUBQUEST_DATA* pmsg = (SEND_SUBQUEST_DATA*)pMsg;											// ���� ����Ʈ �����͸� �޴� ������ ������ �����ϰ� �޽����� �޴´�.

	if(pmsg->wCount==0)		return;																	// ī��Ʈ�� 0�� ������ ���� ó���� �Ѵ�.
	
	for(int i=0; i<pmsg->wCount; i++)																// ī��Ʈ �� ��ŭ for���� ������.
	{
		SetSubQuestData( pmsg->QuestList[i].QuestIdx, pmsg->QuestList[i].SubQuestIdx,				// ���� ����Ʈ �����͸� �����Ѵ�.
							pmsg->QuestList[i].state, pmsg->QuestList[i].time );
	}

	ProcessNpcMark();																				// npc ��ũ ���μ����� �����Ѵ�.
}

void CQuestManager::Quest_Item_Load( void* pMsg )													// ����Ʈ �������� �ε��ϴ� �Լ�.
{
	SEND_QUESTITEM* pmsg = (SEND_QUESTITEM*)pMsg;													// ����Ʈ ������ ������ �޴� ������ ������ �����ϰ� �޽����� �޴´�.

	if(pmsg->wCount==0)		return;																	// ī��Ʈ�� 0�� ������ ���� ó���� �Ѵ�.

	for(int i=0; i<pmsg->wCount; i++)																// ī��Ʈ ����ŭ for���� ������.
	{			
		m_QuestUpdater->QuestItemAdd(m_QuestItemMGR.GetQuestItem(pmsg->ItemList[i].ItemIdx),		// ����Ʈ �������Ϳ� ����Ʈ �������� �߰��Ѵ�.	
			pmsg->ItemList[i].Count);			
	}
}

void CQuestManager::Quest_Start_Ack( void* pMsg )													// ����Ʈ ���� ���� �Լ�.
{
	SEND_QUEST_IDX* pmsg = (SEND_QUEST_IDX*)pMsg;													// ����Ʈ ������ ���� ������ ������ �����ϰ� �޽����� �޴´�.

	StartQuest( pmsg->MainQuestIdx, pmsg->dwFlag, pmsg->dwData, pmsg->registTime );					// ���� ������ ������ ����Ʈ�� �����Ѵ�.

	// 100317 ONS ����Ʈ �ڵ� �˸��� ��� �߰� 
	if( OPTIONMGR->GetGameOption()->bAutoQuestNotice )
	{
		m_QuickView->RegistQuest( pmsg->MainQuestIdx );
	}
}

void CQuestManager::SubQuest_Start_Ack( void* pMsg )												// ���� ����Ʈ ���� ���� �Լ�.
{
	SEND_QUEST_IDX* pmsg = (SEND_QUEST_IDX*)pMsg;													// ����Ʈ ������ ���� ������ ������ �����ϰ� �޽����� �޴´�.

	StartSubQuest( pmsg->MainQuestIdx, pmsg->SubQuestIdx, pmsg->dwFlag, pmsg->dwData, (DWORD)pmsg->registTime );// ���� ����Ʈ�� �����Ѵ�.

	ProcessNpcMark();																				// npc ��ũ ���μ����� �����Ѵ�.
}

void CQuestManager::Quest_End_Ack( void* pMsg )														// ����Ʈ ���� ���� �Լ�.
{
	SEND_QUEST_IDX* msg = (SEND_QUEST_IDX*)pMsg;													// ����Ʈ ������ ���� �����͸� �����ϰ�, �޽����� �޴´�.
		
	EndQuest( msg->MainQuestIdx, msg->dwFlag, msg->dwData, msg->registTime );						// ����Ʈ�� �����Ѵ�.
	ProcessNpcMark();	
}

void CQuestManager::SubQuest_End_Ack( void* pMsg )													// ���� ����Ʈ ���� ���� �Լ�.
{
	SEND_QUEST_IDX* msg = (SEND_QUEST_IDX*)pMsg;													// ����Ʈ ������ ���� �����͸� �����ϰ�, �޽����� �޴´�.

	EndSubQuest( msg->MainQuestIdx, msg->SubQuestIdx, msg->dwFlag, msg->dwData, (DWORD)msg->registTime );		// ���� ����Ʈ�� �����Ѵ�.
	ProcessNpcMark();																				// npc ��ũ ���μ����� �����Ѵ�.

	GAMENOTIFYMGR->EndSubQuest(msg) ;																// ���� ����Ʈ ���� ������ �߰��Ѵ�.
}

void CQuestManager::SubQuest_Update( void* pMsg )													// ���� ����Ʈ ������Ʈ �Լ�.
{
	SEND_SUBQUEST_UPDATE* msg = (SEND_SUBQUEST_UPDATE*)pMsg;										// ���� ����Ʈ ������Ʈ ������ ���� �����͸� �����ϰ� �޽����� �޴´�.

	UpdateSubQuestData( msg->wQuestIdx, msg->wSubQuestIdx, msg->dwMaxCount, msg->dwData, msg->dwTime );	// ���� ����Ʈ �����͸� ������Ʈ �Ѵ�.

	GAMENOTIFYMGR->UpdateSubQuest(msg) ;															// ����Ʈ ������Ʈ ������ �߰��Ѵ�.
}	

void CQuestManager::QuestItem_Give( void* pMsg )													// ���� ������ ��ȯ �Լ�.
{
	SEND_QUESTITEM_IDX* pmsg = (SEND_QUESTITEM_IDX*)pMsg;											// ����Ʈ ������ ������ ���� �����͸� �����ϰ� �޽����� �޴´�.

	QuestItemDelete( pmsg->dwItemIdx );																// ����Ʈ �������� �����Ѵ�.
}

void CQuestManager::QuestItem_Take( void* pMsg )													// ����Ʈ ������ ���� �Լ�.
{
	SEND_QUESTITEM_IDX* pmsg = (SEND_QUESTITEM_IDX*)pMsg;											// ����Ʈ ������ ������ ���� �����͸� �����ϰ� �޽����� �޴´�.

	QuestItemAdd( pmsg->dwItemIdx, pmsg->dwItemNum );												// ����Ʈ �������� �߰��Ѵ�.

	EFFECTMGR->StartHeroEffectProcess(eEffect_GetItem_Quest);										// �������� �����ϴ� ����Ʈ�� �����Ѵ�.

//			ITEMMGR->ItemDropEffect( pmsg->dwItemIdx );
}

void CQuestManager::QuestItem_Update( void* pMsg )													// ����Ʈ ������ ������Ʈ �Լ�.
{
	SEND_QUESTITEM_IDX* pmsg = (SEND_QUESTITEM_IDX*)pMsg;											// ����Ʈ ������ ������ ���� �����͸� �����ϰ� �޽����� �޴´�.
		
	QuestItemUpdate( eQItem_SetCount, pmsg->dwItemIdx, pmsg->dwItemNum );							// ����Ʈ �������� ������Ʈ �Ѵ�.

	EFFECTMGR->StartHeroEffectProcess(eEffect_GetItem_Quest);										// �������� �����ϴ� ����Ʈ�� �����Ѵ�.
//			ITEMMGR->ItemDropEffect( pmsg->dwItemIdx );
}

void CQuestManager::Quest_Delete_Ack( void* pMsg )													// ����Ʈ ������ �����ϴ� �Լ�.
{
	SEND_QUEST_IDX* pmsg = (SEND_QUEST_IDX*)pMsg;													// ����Ʈ ������ ���� �����͸� �����ϰ� �޽����� �޴´�.

	DeleteQuest( pmsg->MainQuestIdx, (BOOL)pmsg->dwFlag );											// ����Ʈ�� �����Ѵ�.

	ProcessNpcMark();																				// npc ��ũ ���μ����� �����Ѵ�.
}

void CQuestManager::Quest_TakeItem_Ack( void* pMsg )												// ����Ʈ ������ ȹ�� ���� �Լ�.
{
	ITEMOBTAINARRAY * pmsg = (ITEMOBTAINARRAY *)pMsg;												// ������ ȹ�� ������ ���� �����͸� �����ϰ� �޽����� �޴´�.

	CItem* pItem = NULL;																			// ������ ������ ���� �����͸� �����ϰ� nulló���� �Ѵ�.

	for(WORD i = 0; i < pmsg->ItemNum; ++i )
	{
		pItem = GAMEIN->GetInventoryDialog()->GetItemForPos( pmsg->GetItem(i)->Position);

		if(pItem)
		{
			pItem->SetItemBaseInfo( *( pmsg->GetItem(i) ) );												// ������ �⺻ ������ �����Ѵ�.
		}
		else																						// ������ ������ ��ȿ���� ������,
		{
			pItem = ITEMMGR->MakeNewItem(pmsg->GetItem(i),"MP_QUEST_TAKEITEM_ACK");					// ���� �������� ������ �����ͷ� �޴´�.

			GAMEIN->GetInventoryDialog()->AddItem(pItem);
		}
	}
	// 
	cDialog* pDlg = WINDOWMGR->GetWindowForID( DBOX_DIVIDE_INV );									// ����̵� �ڽ� ������ �޴´�.

	if( pDlg )																						// ������ ��ȿ���� üũ�Ѵ�.
	{
		((cDivideBox*)pDlg)->ExcuteDBFunc( 0 );														// �ڽ��� �Լ��� �����Ѵ�.
	}

	QUICKMGR->RefreshQickItem();																	// �� �Ŵ����� �� ������ ���ΰ�ħ�� �����Ѵ�.

	if( !pItem ) return;																			// ������ ������ ��ȿ���� ������ ���� ó���� �Ѵ�.

	{
		CHATMGR->AddMsg( CTC_QUEST, CHATMGR->GetChatMsg(235), pItem->GetItemInfo()->ItemName );		// ����Ʈ �������� �������� ����ٴ� �޽����� ����Ѵ�.
	}

	EFFECTMGR->StartHeroEffectProcess(eEffect_GetItem_Quest);										// ������ ȹ�� ����Ʈ�� �����Ѵ�.

	//GAMENOTIFYMGR->TakeItem(pItem) ;																// ����Ʈ ������ ȹ�� ������ ����.
}

void CQuestManager::Quest_Takemoney_Ack( void* pMsg )												// ����Ʈ �Ӵ� ȹ�� ���� �Լ�.
{
	MSG_DWORDEX* pmsg = (MSG_DWORDEX*)pMsg;																// �޽����� ���� �����͸� �����ϰ� �޽����� �޴´�.

	CHATMGR->AddMsg( CTC_QUEST, CHATMGR->GetChatMsg(236), AddComma(pmsg->dweData) );					// ��带 ����ٴ� �޽����� ����Ѵ�.
	
	EFFECTMGR->StartHeroEffectProcess(eEffect_GetMoney);											// �������� ����ٴ� ����Ʈ�� �����Ѵ�.

	//GAMENOTIFYMGR->TakeMoney(AddComma(pmsg->dwData)) ;												// ����Ʈ ���� ��� ȹ�� ������ ����.
}

void CQuestManager::Quest_TakeExp_Ack( void* pMsg )													// ����Ʈ ����ġ�� ȹ�� ���� �Լ�.
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;																// �޽����� ���� �����͸� �����ϰ� �޽����� �޴´�.

	CHATMGR->AddMsg( CTC_QUEST, CHATMGR->GetChatMsg(237), AddComma(pmsg->dwData) );					// ����ġ�� ����ٴ� �޽����� ����Ѵ�.

	//GAMENOTIFYMGR->TakeExp(pmsg->dwData) ;															// ����Ʈ ���� ����ġ ȹ�� ������ ����.
	
//	HERO->SetExpPoint(HERO->GetExpPoint() + pmsg->dwData) ;											// HERO�� ����ġ�� �����Ѵ�.
}

void CQuestManager::Quest_TakesExp_Ack( void* pMsg )												// ����Ʈ sp ȹ�� ���� �Լ�.
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;																// �޽����� ���� �����͸� �����ϰ� �޽����� �޴´�.

	CHATMGR->AddMsg( CTC_QUEST, CHATMGR->GetChatMsg(238), pmsg->dwData );							// sp�� ����ٴ� �޽����� ����Ѵ�.

	HERO->SetExpPoint(HERO->GetExpPoint() + pmsg->dwData) ;											// HERO�� ����ġ�� �����Ѵ�.
}

void CQuestManager::Quest_GiveItem_Ack( void* pMsg )												// ����Ʈ �������� ��ȯ�ϴ� �Լ�.
{
	MSG_EXCHANGE_REMOVEITEM* pmsg = (MSG_EXCHANGE_REMOVEITEM*)pMsg;
	CItem* pItem = GAMEIN->GetInventoryDialog()->GetItemForPos( pmsg->wAbsPosition );
	const eITEMTABLE wTableIdx = ITEMMGR->GetTableIdxForAbsPos( pmsg->wAbsPosition );

	ITEMMGR->DeleteItemofTable( wTableIdx, pmsg->wAbsPosition, FALSE );								// ������ �Ŵ����� ���̺��� �������� �����Ѵ�.

	QUICKMGR->RefreshQickItem();																	// �� �Ŵ����� �� ������ ���ΰ�ħ�� �����Ѵ�.

	if( !pItem ) return;																			// ������ ������ ��ȿ���� ������ ���� ó���� �Ѵ�.

	CHATMGR->AddMsg( CTC_QUEST, CHATMGR->GetChatMsg(297), pItem->GetItemInfo()->ItemName );			// �������� ������ٴ� �޽����� ����Ѵ�.
}

// 071026 LYW --- QuestManager : Add function to decrease item.
void CQuestManager::Quest_GiveItems_Ack( void* pMsg )
{
	MSG_EXCHANGE_DECREASEITEM* pmsg = (MSG_EXCHANGE_DECREASEITEM*)pMsg;								// ��ȯ ���� ������ ������ �޴� �����͸� �����ϰ� �޽����� �޴´�.

	CItem* pItem = GAMEIN->GetInventoryDialog()->GetItemForPos( pmsg->wAbsPosition );				// ������ ��ġ�� ������ ������ �޴´�.

	pItem->SetDurability(pmsg->nItemCount) ;

	QUICKMGR->RefreshQickItem();																	// �� �Ŵ����� �� ������ ���ΰ�ħ�� �����Ѵ�.

	if( !pItem ) return;																			// ������ ������ ��ȿ���� ������ ���� ó���� �Ѵ�.

	CHATMGR->AddMsg( CTC_QUEST, CHATMGR->GetChatMsg(297), pItem->GetItemInfo()->ItemName );			// �������� ������ٴ� �޽����� ����Ѵ�.
}

// 071028 LYW --- CQuestManager : Add function to process give item error.
void CQuestManager::Quest_GiveItem_Error( void* pMsg )						// ����Ʈ ������ �ݳ��� ������ ���� ó���� �ϴ� �Լ�.
{
	ASSERT(pMsg);

	CHATMGR->AddMsg( CTC_QUEST, CHATMGR->GetChatMsg( 1285 ) );

	MSG_DWORD4* pmsg = (MSG_DWORD4*)pMsg ;									// ���� ó���� �´� ������ ����ȯ�� �Ѵ�.

	sUSERINFO_ERR_GIVEITEM EGI ;											// ���� ������ ���� ����ü�� �����Ѵ�.
	memset(&EGI, 0, sizeof(sUSERINFO_ERR_GIVEITEM)) ;						// ����ü�� �ʱ�ȭ �Ѵ�.

	if( HERO == NULL ) return ;												// ����� ������ ������ ���� ó���� �Ѵ�.

	strcpy(EGI.UserName, HERO->GetObjectName()) ;							// ���� ĳ������ �̸��� �����Ѵ�.
	EGI.bCaption = (BYTE)pmsg->dwData1 ;									// ���� �߻� �Լ��� �����Ѵ�.
	EGI.bErrNum	 = (BYTE)pmsg->dwData2 ;									// ���� ��ȣ�� �����Ѵ�.
	EGI.dwQuestIdx = pmsg->dwData3 ;										// ����Ʈ �ε����� �����Ѵ�.
	EGI.dwItemIdx  = pmsg->dwData4 ;										// ������ �ε����� �����Ѵ�.

	char strFilePath[MAX_PATH] = {0, } ;									// ������ ����� ��θ� ���� ���۸� �����ϰ� �ʱ�ȭ �Ѵ�.

	wsprintf( strFilePath, "%s\\data\\interface\\log\\%d.EGI", 
		DIRECTORYMGR->GetFullDirectoryName(eLM_Root),gHeroID);

	HANDLE hFile = CreateFile( strFilePath, GENERIC_WRITE, 0, NULL, 
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL ) ;						// ������ �����Ѵ�.

	if( hFile == INVALID_HANDLE_VALUE )										// ����� ���� ������,
	{
		return ;															// return.
	}

	DWORD dwSize ;															// ����� ���� ������ �����Ѵ�.

	WriteFile( hFile, &EGI, sizeof(EGI), &dwSize, NULL ) ;					// ���� ������ ����Ѵ�.

	CloseHandle( hFile ) ;
}

void CQuestManager::Quest_GiveMoney_Ack( void* pMsg )												// �Ӵϸ� �ִ� �Լ�.
{
	MSG_DWORDEX* pmsg = (MSG_DWORDEX*)pMsg;																// �޽����� ���� �����͸� �����ϰ� �޽����� �޴´�.

	CHATMGR->AddMsg( CTC_QUEST, CHATMGR->GetChatMsg(236), AddComma(pmsg->dweData) );					// ����Ʈ �������� ��带 ����ٴ� �޽����� ����Ѵ�.
}

void CQuestManager::Quest_Time_Limit( void* pMsg )													// ����Ʈ �ð� ���� �Լ�.
{
	MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;															// �޽����� ���� �����͸� �����ϰ� �޽����� �޴´�.

	QuestTimeLimit( pmsg->dwData1, pmsg->dwData2 );													// ���� ������ ����Ʈ �ð� ������ �Ѵ�.
}

void CQuestManager::Quest_Regist_Checktime( void* pMsg )											// �ð� üũ ����� �ϴ� �Լ�.
{
	MSG_DWORD3* pmsg = (MSG_DWORD3*)pMsg;															// �޽����� ���� �����͸� �����ϰ� �޽����� �޴´�.

	RegistCheckTime( pmsg->dwData1, (BYTE)pmsg->dwData2, pmsg->dwData3 );							// �ð� üũ�� ����Ѵ�.
}

void CQuestManager::Quest_Unregist_Checktime( void* pMsg )											// �ð� üũ ����� ����ϴ� �Լ�.
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;																// �޽����� ���� �����͸� �����ϰ� �޽����� �޴´�.

	UnRegistCheckTime( pmsg->dwData );																// �ð� üũ�� ����Ѵ�.
}

void CQuestManager::QuestEvent_NpcTalk_Ack( void* pMsg )											// npc ��ȭ ��û �̺�Ʈ �Լ�.
{
	MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;															// �޽����� ���� �����͸� �����ϰ� �޽����� �޴´�.

	QuestEventAck( pmsg->dwData1 );																	// ����Ʈ �̺�Ʈ�� �����Ѵ�.
}

void CQuestManager::QuestEvent_NpcTalk_Nack( void* pMsg )											// npc ��ȭ��û ���� �̺�Ʈ �Լ�.
{
	MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;															// �޽����� ���� �����͸� �����ϰ� �޽����� �޴´�.

	QuestEventNack( pmsg->dwData1, pmsg->dwData2 );													// ����Ʈ �̺�Ʈ ���� ó���� �Ѵ�.
}

void CQuestManager::Quest_Execute_Error( void* pMsg )												// ����Ʈ ���� ���� ó�� �Լ�.
{
	MSG_INT* pmsg = (MSG_INT*)pMsg;																	// �޽����� ���� �����͸� �����ϰ� �޽����� �޴´�.

	int nMsgNum = 0 ;																				// ���� �޽��� ��ȣ�� ������ ������ �����ϰ� 0���� �����Ѵ�.

	switch(pmsg->nData)																				// �޽����� �Ѿ�� ���� �ڵ带 Ȯ���Ѵ�.
	{
	case e_EXC_ERROR_NO_ERROR :					nMsgNum = 990 ;		break ;							// ���� �ڵ忡 �´� �޽��� ��ȣ�� �����Ѵ�.
	case e_EXC_ERROR_NO_PLAYERINFO :			nMsgNum = 991 ;		break ;
	case e_EXC_ERROR_NO_QUESTGROUP :			nMsgNum = 992 ;		break ;
	case e_EXC_ERROR_NO_QUEST :					nMsgNum = 993 ;		break ;
	case e_EXC_ERROR_NO_INVENTORYINFO :			nMsgNum = 994 ;		break ;
	case e_EXC_ERROR_NO_EXTRASLOT :				nMsgNum = 995 ;		break ;
	case e_EXC_ERROR_NO_EXTRASLOTQUEST :		nMsgNum = 996 ;		break ;
	case e_EXC_ERROR_NO_QUESTITEM :				nMsgNum = 997 ;		break ;
	case e_EXC_ERROR_NOT_ENOUGH_QUESTITEM :		nMsgNum = 998 ;		break ;
	case e_EXC_ERROR_NO_GIVEITEM :				nMsgNum = 999 ;		break ;
	case e_EXC_ERROR_NOT_ENOUGH_GIVEITEM :		nMsgNum = 1000 ;	break ;
	case e_EXC_ERROR_NOT_ENOUGH_GIVEMONEY :		nMsgNum = 1001 ;	break ;
	//case e_EXC_ERROR_NOT_SAME_WEAPONKIND :		nMsgNum = 1002 ;	break ;
	// 071223 LYW --- QuestManager : ����Ʈ ���� �޽��� ����� �˸�â���� ä��â ������� �ٲ۴�.
	case e_EXC_ERROR_NOT_SAME_WEAPONKIND :
		{
			CHATMGR->AddMsg( CTC_CHEAT_1, CHATMGR->GetChatMsg(1002) ) ;
			return ;
		}
		break ;
	case e_EXC_ERROR_NOT_SAME_WEAPONKIND_COUNT :nMsgNum = 1003 ;	break ;
	case e_EXC_ERROR_CANT_RECEIVE_MONEY :		nMsgNum = 1004 ;	break ;
	// 100414 ONS ������ ���õ��� �ʾ��� ��� �޼��� ���
	case e_EXC_ERROR_NOT_EXIST_REQUITAL:		nMsgNum = 2214 ;	break;
	}

	if( pmsg->nData != e_EXC_ERROR_NOT_EXIST_REQUITAL)
	{
		GAMEIN->GetNpcScriptDialog()->EndDialog();														// npc ��ũ��Ʈ ���̾�α׷� end ���̾�α׸� ȣ���Ѵ�.
		GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);;												// npc ��ũ��Ʈ ���̾�α׸� ��Ȱ��ȭ �Ѵ�.
	}
	WINDOWMGR->MsgBox( MBI_QUESTMSG, MBT_OK, CHATMGR->GetChatMsg(nMsgNum) );						// ����ǰ�� �� ������ ���� ���� �Ұ���� �޽����� ����Ѵ�.
}

void CQuestManager::Quest_Error_Ext( void* pMsg )													// ����Ʈ ��Ÿ ���� ó�� �Լ�.
{
	if( !pMsg ) return ;																			// �޽��� ������ ��ȿ���� ������, ����ó���Ѵ�.

	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg ;															// �޽����� ���� �����͸� �����ϰ� �޽����� �޴´�.

	ITEM_INFO* pItemInfo = ITEMMGR->GetItemInfo(pmsg->dwData);										// �޽����� �Ѿ�� ������ �ε����� ������ ������ �޴´�.

	if( !pItemInfo ) return ;																		// ������ ������ �������� ������, return ó�� �Ѵ�.

	char tempBuf[128] = {0,} ;																		// �޽��� ����� ���� �ӽ� ���۸� �����Ѵ�.
	//sprintf(tempBuf, "[%s] �������� ������ �����Ͽ� ����Ʈ�� �Ϸ��� �� �����ϴ�.", pItemInfo->ItemName) ;	// ���� �޽����� �����Ѵ�.
	sprintf(tempBuf, CHATMGR->GetChatMsg(986), pItemInfo->ItemName) ;	// ���� �޽����� �����Ѵ�.

	GAMEIN->GetNpcScriptDialog()->EndDialog();														// npc ��ũ��Ʈ ���̾�α׷� end ���̾�α׸� ȣ���Ѵ�.
	GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);;												// npc ��ũ��Ʈ ���̾�α׸� ��Ȱ��ȭ �Ѵ�.
	WINDOWMGR->MsgBox( MBI_QUESTMSG, MBT_OK, tempBuf );												// ����ǰ�� �� ������ ���� ���� �Ұ���� �޽����� ����Ѵ�.
}

void CQuestManager::Quest_Error_FullQuest( void* pMsg )
{
	if( !pMsg ) return ;

	char tempBuf[128] = {0,} ;																		// �޽��� ����� ���� �ӽ� ���۸� �����Ѵ�.
	sprintf(tempBuf, CHATMGR->GetChatMsg(1474)) ;

	GAMEIN->GetNpcScriptDialog()->EndDialog();														// npc ��ũ��Ʈ ���̾�α׷� end ���̾�α׸� ȣ���Ѵ�.
	GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);;												// npc ��ũ��Ʈ ���̾�α׸� ��Ȱ��ȭ �Ѵ�.
	WINDOWMGR->MsgBox( MBI_QUESTMSG, MBT_OK, tempBuf );												// ����ǰ�� �� ������ ���� ���� �Ұ���� �޽����� ����Ѵ�.
}

// 070329 LYW --- QuestManager : Add function to make string.
void CQuestManager::CompositString( char* inputStr, char* str, ... )								// ��¿� �޽����� �ϼ��ϴ� �Լ�.
{
	if( inputStr == NULL ) return ;																	// ���� ��Ʈ�� ������ ��ȿ���� ������ ���� ó���� �Ѵ�.
	if( str == NULL ) return;																		// ��Ʈ�� ������ ��ȿ���� ������ ���� ó���� �Ѵ�.

	va_list argList;																				// ���� ���� ������ �����Ѵ�.

	va_start(argList, str);																			// ���� ���� �ؼ��� �����Ѵ�.
	vsprintf(inputStr,str,argList);																	// �޽����� �ϼ��Ѵ�.
	va_end(argList);																				// ���� ���� ��� �Ϸ�.
}

void CQuestManager::LoadQuickViewInfoFromFile(DWORD* pIdx)
{
	if( !pIdx )
	{
		return ;
	}

	CQuestQuickViewDialog* pQuickViewDlg = NULL ;
	pQuickViewDlg = GAMEIN->GetQuestQuickViewDialog() ;

	if( pQuickViewDlg )
	{
		for( int index = 0 ; index < MAX_QUICKVIEW_QUEST ; ++index )
		{
			pQuickViewDlg->RegistQuest(pIdx[index]) ;
		}
		// 100601 ONS ����Ʈ �˸��� ���â�� ������Ʈ�Ѵ�.
		pQuickViewDlg->ResetQuickViewDialog();
	}
}

int CQuestManager::CheckQuestType(DWORD wQuestIdx, DWORD wSubQuestIdx)							
{
	CQuest* pQuest = QUESTMGR->GetQuest( wQuestIdx );							// ����Ʈ ������ �޴´�.
	CSubQuestInfo* pDescSubQuestInfo ;												// ���� ����Ʈ ������ ���� �����͸� �����Ѵ�.

	pDescSubQuestInfo = NULL ;
	pDescSubQuestInfo = pQuest->GetSubQuestInfo(wSubQuestIdx) ;

	if( !pDescSubQuestInfo ) return e_QT_ERROR_TYPE ;

	CQuestNpcScript* pQuestNpcScript = pDescSubQuestInfo->GetQuestNpcScript();
	if( !pQuestNpcScript )
		return e_QT_ERROR_TYPE;

	return pQuestNpcScript->GetDialogType();
}

void CQuestManager::NotifyChangedDateTime( const DWORD dwQuestIdx )
{
	CQuestInfo* pQuestInfo = m_QuestInfoTable.GetData( dwQuestIdx );
	CQuest* pQuest = GetQuest( dwQuestIdx );
	if( !pQuestInfo || !pQuest )
		return;

	// 100621 ShinJS ����Ʈ�� ���డ���� ��츸 �޼����� ����
	if( pQuest->IsQuestComplete() == 1 ||
		pQuest->IsQuestState( pQuestInfo->GetNpcIdx() ) != eQuestState_Possible )
		return;

	const BOOL bIsValid = pQuest->IsValidDateTime();

	int Key = 0;
	COMBINEKEY( dwQuestIdx, 0, Key );
	CQuestString* pQString = m_QuestStringTable.GetData(Key);
	if( pQString == NULL )	return;
	cPtrList* p = pQString->GetTitle();
	ITEM* string = (ITEM*)p->GetHead();

	// ��Ȱ��->Ȱ��, Ȱ��->���� Ȱ�� ���·� ����� ���
	if( bIsValid )
	{
		// ����Ʈ�� �Ϸ�Ǿ��ų� ���� �ٸ� ��� �˸��� �ʴ´�
		int nMapNum = pQuestInfo->GetMapNum();
		if( pQuest->IsQuestComplete() ||
			MAP->GetMapNum() != nMapNum )
			return;

		// ���డ���� ���
		CHATMGR->AddMsg( CTC_QUEST, CHATMGR->GetChatMsg( 2217 ), string->string );			// "[%s] ����Ʈ�� ������ �� �ֽ��ϴ�."

	}
	// Ȱ��->��Ȱ��, ��Ȱ��->���� ��Ȱ��
	else
	{
		// ����Ʈ�� �������̰� �Ϸ���� ���� ���
		if( pQuest->IsPerforming() && 
			pQuest->IsQuestComplete() == 0 )
		{
			CHATMGR->AddMsg( CTC_QUEST, CHATMGR->GetChatMsg( 2218 ), string->string );		// "[%s] ����Ʈ�� ������ �� �����ϴ�."
		}
	}
}

void CQuestManager::Process()
{
	// 100405 ShinJS --- �ð����� ����Ʈ �˻�
	static DWORD dwDateLimitCheckTime;
	if( dwDateLimitCheckTime < gCurTime )
	{
		dwDateLimitCheckTime = gCurTime + 1000;

		BOOL bChanged = FALSE;
		for( std::set< DWORD >::const_iterator iterDateLimitQuest = m_setDateLimitQuest.begin() ; iterDateLimitQuest != m_setDateLimitQuest.end() ; ++iterDateLimitQuest )
		{
			const DWORD dwQuestIdx = *iterDateLimitQuest;

			CQuestInfo* pQuestInfo = m_QuestInfoTable.GetData( dwQuestIdx );
			CQuest* pQuest = GetQuest( dwQuestIdx );
			if( !pQuestInfo || !pQuest )
				continue;

			__time64_t nextDateTime = pQuest->GetNextDateTime();
			pQuest->IsValidDateTime();
			// ���°� ����� ���
			if( pQuest->GetNextDateTime() != nextDateTime )
			{
				// ��ȿ �ð� ���°� ����� ��츦 �˷��ش�.
				NotifyChangedDateTime( dwQuestIdx );
				bChanged = TRUE;
			}
		}

		if( bChanged )
		{
			ProcessNpcMark();
			m_QuestDialog->RefreshQuestList() ;
		}
	}
}

void CQuestManager::InitDateTime()
{
	BOOL bChanged = FALSE;
	for( std::set< DWORD >::const_iterator iterDateLimitQuest = m_setDateLimitQuest.begin() ; iterDateLimitQuest != m_setDateLimitQuest.end() ; ++iterDateLimitQuest )
	{
		const DWORD dwQuestIdx = *iterDateLimitQuest;
		
		CQuest* pQuest = GetQuest( dwQuestIdx );
		if( !pQuest )
			continue;

		// ���°� ����� ���
		__time64_t nextDateTime = pQuest->GetNextDateTime();
		pQuest->InitDateTime();
		if( pQuest->GetNextDateTime() != nextDateTime )
		{
			// ��ȿ �ð� ���°� ����� ��츦 �˷��ش�.
			NotifyChangedDateTime( dwQuestIdx );
			bChanged = TRUE;
		}
	}

	if( bChanged )
	{
		ProcessNpcMark();
		m_QuestDialog->RefreshQuestList() ;
	}
}

BOOL CQuestManager::IsValidDateTime( DWORD dwQuestIdx )
{
	CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );
	if( !pQuest )
		return FALSE;

	return pQuest->IsValidDateTime();
}

BOOL CQuestManager::IsCompletedDemandQuestExecute( DWORD dwQuestIdx )
{
	CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );
	if( !pQuest )
		return FALSE;

	return pQuest->IsCompletedDemandQuestExecute();
}

// 100414 ONS ����Ʈ ������Ʈ���� ������ �����´�.
cPtrList* CQuestManager::GetLastSubQuestTrigger( DWORD dwQuestIdx )
{
	CQuestInfo* pQuestInfo = GetQuestInfo(dwQuestIdx) ;	
	if( !pQuestInfo ) return NULL;

	int nSubQuestCount = pQuestInfo->GetSubQuestCount();
	if( nSubQuestCount < 1  )	return NULL;

	CSubQuestInfo* pSubQuestInfo = pQuestInfo->GetSubQuestInfo(nSubQuestCount-1) ;
	if( !pSubQuestInfo ) return NULL;

	return &(pSubQuestInfo->GetTriggerList());
}

void CQuestManager::AddNpcHasQuest( DWORD dwUniqueNpcIdx, DWORD dwQuestIdx )
{
	NpcHasQuestList& hasQuestList = m_NpcHasQuestList[ dwUniqueNpcIdx ];
	hasQuestList.insert( dwQuestIdx );
}