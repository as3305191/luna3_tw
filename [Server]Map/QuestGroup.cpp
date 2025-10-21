// QuestGroup.cpp: implementation of the CQuestGroup class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "QuestGroup.h"

#include "QuestManager.h"
#include "Quest.h"

#include "Player.h"
#include "ItemManager.h"
#include "MapDBMsgParser.h"
#include "QuestRegenMgr.h"
#include "..\[CC]Quest\QuestExecute_RandomItem.h"													// ����Ʈ ���� ���� ������ ����� �ҷ��´�.

#include "PartyManager.h"
#include "Party.h"
#include "UserTable.h"
#include "..\[CC]Quest\QuestInfo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL CheckQuestProbability(DWORD dwProbability)
{
	if( dwProbability == 0 )
		return FALSE;
	if( dwProbability != MAX_QUEST_PROBABILITY )
	{
		DWORD nFlag = rand()%MAX_QUEST_PROBABILITY;
		if( nFlag >= dwProbability )
			return FALSE;
	}

	return TRUE;
}

CQuestGroup::CQuestGroup()
{
	m_pPlayer = NULL;
	m_QuestTable.Initialize( MAX_QUEST );
	m_QuestItemTable.Initialize( MAX_QUESTITEM );
	memset( m_QuestEvent, 0, sizeof(GQUESTEVENT)*MAX_QUESTEVENT_PLYER );
	m_dwQuestEventNum = 0;
}

CQuestGroup::CQuestGroup( PLAYERTYPE* pPlayer )
{
	m_pPlayer = pPlayer;
	m_QuestTable.Initialize( MAX_QUEST );
	m_QuestItemTable.Initialize( MAX_QUESTITEM );
	memset( m_QuestEvent, 0, sizeof(GQUESTEVENT)*MAX_QUESTEVENT_PLYER );
	m_dwQuestEventNum = 0;
}

CQuestGroup::~CQuestGroup()
{
	Release();
}

void CQuestGroup::Initialize( PLAYERTYPE* pPlayer )
{
	m_pPlayer = pPlayer;
//	m_QuestTable.Initialize( MAX_QUEST );
//	m_QuestItemTable.Initialize( MAX_QUESTITEM );
	memset( m_QuestEvent, 0, sizeof(GQUESTEVENT)*MAX_QUESTEVENT_PLYER );
	m_dwQuestEventNum = 0;
	m_RequitalIndexMap.clear();
}

void CQuestGroup::Release()
{
	CQuest* pQuest = NULL;
	m_QuestTable.SetPositionHead();
	while( (pQuest = m_QuestTable.GetData()) != NULL)
	{
		delete pQuest;
		pQuest = NULL;
	}
	m_QuestTable.RemoveAll();

	QUESTITEM* pQuestItem = NULL;
	m_QuestItemTable.SetPositionHead();
	while( (pQuestItem = m_QuestItemTable.GetData()) != NULL)
	{
		delete pQuestItem;
		pQuestItem = NULL;
	}
	m_QuestItemTable.RemoveAll();

	m_pPlayer = NULL;
	memset( m_QuestEvent, 0, sizeof(GQUESTEVENT)*MAX_QUESTEVENT_PLYER );
	m_dwQuestEventNum = 0;
}

void CQuestGroup::ReleaseRandomItemList()
{
	RANDOMITEM* pItem = NULL;	

	PTRLISTPOS pos = NULL ;
	pos = m_RandomItemList.GetHeadPosition() ;

	while(pos)
	{
		pItem = NULL ;
		pItem = (RANDOMITEM*)m_RandomItemList.GetNext(pos) ;

		if( !pItem ) continue ;

		m_RandomItemList.Remove(pItem) ;

		delete pItem ;
	}

	m_RandomItemList.RemoveAll() ;
}

void CQuestGroup::CreateQuest( DWORD dwQuestIdx, CQuestInfo* pQuestInfo )
{
	CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );
	if( pQuest == NULL )
	{
		pQuest = new CQuest( m_pPlayer, pQuestInfo );
		m_QuestTable.Add( pQuest, dwQuestIdx );

		// 100405 ShinJS --- �ð����� ����Ʈ ���
		if( pQuest->HasDateLimit() )
		{
			m_setDateLimitQuest.insert( dwQuestIdx );
		}
	}
}

void CQuestGroup::SetMainQuestData( DWORD dwQuestIdx, DWORD dwSubQuestFlag, DWORD dwData, __time64_t registTime, BYTE bCheckType, DWORD dwCheckTime )
{
	CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );
	if( pQuest )
		pQuest->SetMainQuestData( dwSubQuestFlag, dwData, registTime, bCheckType, dwCheckTime );
}

void CQuestGroup::SetSubQuestData( DWORD dwQuestIdx, DWORD dwSubQuestIdx, DWORD dwData, DWORD dwTime )
{
	CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );
	if( pQuest )
		pQuest->SetSubQuestData( dwSubQuestIdx, dwData, dwTime );
}

void CQuestGroup::SetQuestItem( DWORD dwQuestIdx, DWORD dwItemIdx, DWORD dwItemNum )
{
	QUESTITEM* pQuestItem = NULL;

	// AO��AAo E��AICI��i AO8��e Ao��i��U.
	pQuestItem = m_QuestItemTable.GetData( dwItemIdx );
	if(pQuestItem)
	{
		m_QuestItemTable.Remove( pQuestItem->dwItemIdx );
		delete pQuestItem;
		pQuestItem = NULL;
	}

	pQuestItem = new QUESTITEM;
	pQuestItem->dwQuestIdx = dwQuestIdx;
	pQuestItem->dwItemIdx = dwItemIdx;
	pQuestItem->dwItemNum = dwItemNum;
	
	m_QuestItemTable.Add( pQuestItem, pQuestItem->dwItemIdx );
}

void CQuestGroup::AddQuestEvent( CQuestEvent* pQuestEvent, DWORD dwQuestIdx )
{
	if( m_dwQuestEventNum < MAX_QUESTEVENT_PLYER )
	{
		m_QuestEvent[m_dwQuestEventNum].dwQuestIdx = dwQuestIdx;
		m_QuestEvent[m_dwQuestEventNum].QuestEvent.SetValue( pQuestEvent );
		++m_dwQuestEventNum;
	}
}

BOOL CQuestGroup::IsQuestComplete( DWORD dwQuestIdx )
{
	CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );

	if( pQuest )
	if( pQuest->IsQuestComplete() )
		return TRUE;

	return FALSE;
}

void CQuestGroup::DeleteQuest( DWORD dwQuestIdx )
{
	CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );
	if( pQuest )
	{
		pQuest->EndQuest( dwQuestIdx, 1 );

		// quest item delete
		QUESTITEM* pQuestItem = NULL;
		m_QuestItemTable.SetPositionHead();
		cPtrList list;
		while( (pQuestItem = m_QuestItemTable.GetData()) != NULL)
		{
			if( pQuestItem->dwQuestIdx == dwQuestIdx )
			{
				list.AddTail( pQuestItem );
				//m_QuestItemTable.Remove( pQuestItem->dwItemIdx );
				//delete pQuestItem;
				//pQuestItem = NULL;
			}
		}

		PTRLISTPOS pos = list.GetHeadPosition();
		while( pos )
		{
			QUESTITEM* p = (QUESTITEM*)list.GetNext(pos);
			m_QuestItemTable.Remove( p->dwItemIdx );
			delete p;
		}
		list.RemoveAll();
	}
}

void CQuestGroup::EndQuest( DWORD dwQuestIdx, DWORD dwSubQuestIdx )
{
	CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );
	if( pQuest )
	if( pQuest->EndQuest( dwQuestIdx, dwSubQuestIdx ) )
	{
		QUESTMGR->EndQuest( m_pPlayer, dwQuestIdx, dwSubQuestIdx );
	}
}

void CQuestGroup::StartSubQuest( DWORD dwQuestIdx, DWORD dwSubQuestIdx )
{
	CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );
	if( pQuest )	
	if( pQuest->StartSubQuest( dwQuestIdx, dwSubQuestIdx ) )
	{
		// questmgr
		QUESTMGR->StartSubQuest( m_pPlayer, dwQuestIdx, dwSubQuestIdx );
	}
}

void CQuestGroup::EndSubQuest( DWORD dwQuestIdx, DWORD dwSubQuestIdx )
{
	CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );
	if( pQuest )	
	if( pQuest->EndSubQuest( dwQuestIdx, dwSubQuestIdx ) )
	{
		// questmgr
		if( dwSubQuestIdx == 0 )
			QUESTMGR->StartQuest( m_pPlayer, dwQuestIdx, dwSubQuestIdx );
		else
			QUESTMGR->EndSubQuest( m_pPlayer, dwQuestIdx, dwSubQuestIdx );
	}
}

void CQuestGroup::ChangeSubQuestValue( DWORD dwQuestIdx, DWORD dwSubQuestIdx, DWORD dwKind )
{
	CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );
	if( pQuest )
	{
		pQuest->ChangeSubQuestValue( dwSubQuestIdx, dwKind );	

		// questmgr
		QUESTMGR->UpdateSubQuestData( m_pPlayer, dwQuestIdx, dwSubQuestIdx, 0 );

		// event
		CQuestEvent qe;
		qe.SetValue( eQuestEvent_Count, dwSubQuestIdx, pQuest->GetSubQuestData(dwSubQuestIdx) );
		QUESTMGR->AddQuestEvent( m_pPlayer, &qe, dwQuestIdx );
		//
	}
}

void CQuestGroup::Process()
{
	if( m_pPlayer == NULL )			return;									// �÷��̾� ������ ��ȿ���� ������ return ó���� �Ѵ�.

	// 100405 ShinJS --- �ð����� ����Ʈ �˻�
	static DWORD dwDateLimitCheckTime;
	if( dwDateLimitCheckTime < gCurTime )
	{
		dwDateLimitCheckTime = gCurTime + 1000;

		__time64_t curTime=0;
		_time64( & curTime );

		for( std::set<DWORD>::const_iterator iterDateLimitQuest = m_setDateLimitQuest.begin() ; iterDateLimitQuest != m_setDateLimitQuest.end() ; ++iterDateLimitQuest )
		{
			const DWORD dwQuestIdx = *iterDateLimitQuest;

			CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );
			CQuestInfo* pQuestInfo = QUESTMGR->GetQuestInfo( dwQuestIdx );
			if( !pQuest || !pQuestInfo )
				continue;

			__time64_t nextDateTime = pQuest->GetNextDateTime();
			const BOOL bIsValid = pQuest->IsValidDateTime();
			BOOL bChange = ( pQuest->GetNextDateTime() != nextDateTime );

			// ���°� ����� ���, ��Ȱ��->Ȱ��, Ȱ��->���� Ȱ�� ���·� ����� ���
			// �ݺ�����Ʈ�� �ƴϰ� �Ϸ�� ��� ������ �����Ѵ�.
			if( bChange && 
				bIsValid &&
				pQuest->IsQuestComplete() && 
				pQuestInfo->GetEndParam() == 0 )
			{
				
				QUESTMGR->DeleteQuestForcedly( *m_pPlayer, dwQuestIdx );
			}
		}
	}


	if( m_dwQuestEventNum <= 0 )	return;									// ����Ʈ �̺�Ʈ�� ������ return ó���� �Ѵ�.

	DWORD dwPartyIdx = m_pPlayer->GetPartyIdx() ;							// �÷��̾��� ��Ƽ �ε����� �޴´�.
	CParty* pParty = PARTYMGR->GetParty(dwPartyIdx);						// ���� ��Ƽ �ε����� ��Ƽ ������ �޴´�.

	if( pParty )															// �÷��̾ ��Ƽ�� ���� �ִٸ�,
	{
		DWORD dwMemberId ;													// ��Ƽ ����� ���̵� ���� ������ �����Ѵ�.
		CPlayer* pMember ;													// ��Ƽ ����� ������ ���� �����͸� �����Ѵ�.

		VECTOR3 playerPos ;													// ���� �÷��̾��� ��ġ�� ���� ���͸� �����Ѵ�.
		VECTOR3 memberPos ;													// �� ��� ����� ��ġ�� ���� ���͸� �����Ѵ�.

		CQuest* pQuest = NULL;												// ����Ʈ ������ ���� �����͸� �����ϰ� null ó���� �Ѵ�.
		CQuest* pMemberQuest ;												// ����� ����Ʈ ������ ���� �����͸� �����Ѵ�.

		DWORD dwQuestIdx ;													// ����Ʈ �ε����� ���� ������ �����Ѵ�.

		DWORD dwEventKind ;

		memset(&playerPos, 0, sizeof(VECTOR3)) ;							// ���� �÷��̾��� ��ġ�� ���� ���͸� �ʱ�ȭ �Ѵ�.
		m_pPlayer->GetPosition(&playerPos) ;								// ���� �÷��̾��� ��ġ�� �޴´�.

		for( int count = 0 ; count < MAX_PARTY_LISTNUM ; ++count )			// �ִ� ��Ƽ ��� �� ��ŭ for���� ������.
		{
			dwMemberId = pParty->GetMemberID(count) ;						// ī��Ʈ�� �ش��ϴ� ����� ���̵� �޴´�.

			pMember = NULL ;												// ��� ������ ���� �����͸� nulló���� �Ѵ�.
			pMember = (CPlayer*)g_pUserTable->FindUser( dwMemberId );		// ���� ��� ���̵�� ��� ������ �޴´�.

			if( !pMember ) continue ;										// ��� ������ ��ȿ���� ������, continue ó���� �Ѵ�.

			// 071215 LYW --- QuestQroup : ��Ƽ ����� ���� ���¿�����, �������� �߰����� �ʴ´�.
			if( pMember->GetState() == eObjectState_Die ) continue ;

			if( !pParty->IsMemberLogIn(count) ) continue ;					// ����� �α��� ���°� �ƴϸ�, continue ó���� �Ѵ�.

			if( m_pPlayer->GetGridID() != pMember->GetGridID() ) continue ;	// ����� ä���� �ٸ���, continue ó���� �Ѵ�.

			float fDistance = 0;

			memset(&memberPos, 0, sizeof(VECTOR3)) ;						// ����� ��ġ�� ���� ���͸� �ʱ�ȭ �Ѵ�.			
			pMember->GetPosition(&memberPos) ;								// ����� ��ġ�� �޴´�.

			fDistance = CalcDistanceXZ( &playerPos, &memberPos ) ;			// ���� �÷��̾�� ����� �Ÿ��� ���Ѵ�.

			if( fDistance > POINT_VALID_DISTANCE ) continue ;				// �Ÿ��� ��Ƽ ��ȿ �Ÿ����� ũ��, continue ó���� �Ѵ�.

			m_QuestTable.SetPositionHead();									// ����Ʈ ���̺��� ��� ���������� �����Ѵ�.
			while( (pQuest = m_QuestTable.GetData()) != NULL)						// ����Ʈ ���̺����� ���� ����Ʈ ������ ��ȿ�� ���� while���� ������.
			{
				dwQuestIdx = pQuest->GetQuestIdx();
				pMemberQuest = pMember->GetQuestGroup().GetQuest(dwQuestIdx);

				if( !pMemberQuest ) continue ;								// ����� ����Ʈ ������ ��ȿ���� ������ continue ó���� �Ѵ�.

				if( pMemberQuest->IsQuestComplete() ) continue ;			// ����Ʈ�� �Ϸ� �Ͽ��ٸ�, continue ó���� �Ѵ�.

				dwEventKind = 0 ;
				dwEventKind = m_QuestEvent[0].QuestEvent.m_dwQuestEventKind ;

				if( pMember->GetID() == m_pPlayer->GetID() )
				{
					for( DWORD i = 0; i < m_dwQuestEventNum; ++i )				// ����Ʈ �̺�Ʈ �� ��ŭ for���� ������.
					{	
						pMemberQuest->OnQuestEvent(
							pMember,
							&pMember->GetQuestGroup(),
							&m_QuestEvent[i].QuestEvent,
							m_QuestEvent[i].dwQuestIdx);
					}
				}
				else
				{
					if( dwEventKind != eQuestEvent_Hunt &&  dwEventKind != eQuestEvent_Count) continue ;

					if( dwEventKind != eQuestEvent_Count )
					{
						for( DWORD i = 0; i < m_dwQuestEventNum; ++i )				// ����Ʈ �̺�Ʈ �� ��ŭ for���� ������.
						{	
							pMemberQuest->OnQuestEvent(
								pMember,
								&pMember->GetQuestGroup(),
								&m_QuestEvent[i].QuestEvent,
								m_QuestEvent[i].dwQuestIdx);
						}
					}
				}
			}
		}
	}
	else																	// �÷��̾ ��Ƽ�� ���� ���� ������,
	{
		CQuest* pQuest = NULL;												// ����Ʈ ������ ���� �����͸� �����ϰ� null ó���� �Ѵ�.

		m_QuestTable.SetPositionHead();										// ����Ʈ ���̺��� ��� ���������� �����Ѵ�.
		while( (pQuest = m_QuestTable.GetData()) != NULL)							// ����Ʈ ���̺����� ���� ����Ʈ ������ ��ȿ�� ���� while���� ������.
		{
			if( pQuest->IsQuestComplete() ) continue ;					// ����Ʈ�� �Ϸ� �Ͽ��ٸ�, continue ó���� �Ѵ�.

			for( DWORD i = 0; i < m_dwQuestEventNum; ++i )					// ����Ʈ �̺�Ʈ �� ��ŭ for���� ������.
			{				
				pQuest->OnQuestEvent( m_pPlayer, this, &m_QuestEvent[i].QuestEvent, m_QuestEvent[i].dwQuestIdx );
			}
		}
	}

	memset( m_QuestEvent, 0, sizeof(GQUESTEVENT)*MAX_QUESTEVENT_PLYER );
	m_dwQuestEventNum = 0;
}

void CQuestGroup::AddCount( DWORD dwQuestIdx, DWORD dwSubQuestIdx, DWORD dwMaxCount )
{
	CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );
	if( pQuest )
	{
		pQuest->ChangeSubQuestValue( dwSubQuestIdx, eQuestValue_Add );	

		// questmgr
		QUESTMGR->UpdateSubQuestData( m_pPlayer, dwQuestIdx, dwSubQuestIdx, dwMaxCount );

		// event
		CQuestEvent qe;
		qe.SetValue( eQuestEvent_Count, dwSubQuestIdx, pQuest->GetSubQuestData(dwSubQuestIdx) );
		QUESTMGR->AddQuestEvent( m_pPlayer, &qe, dwQuestIdx );
		//
	}
}

void CQuestGroup::AddCountFromWeapon( DWORD dwQuestIdx, DWORD dwSubQuestIdx, DWORD dwMaxCount, DWORD dwWeaponKind )
{
	if(ITEMMGR->GetWeaponKind(m_pPlayer->GetWearedWeapon()) != eWeaponType(dwWeaponKind))
	{
		return;
	}
	else if(CQuest* pQuest = m_QuestTable.GetData(dwQuestIdx))
	{
		pQuest->ChangeSubQuestValue( dwSubQuestIdx, eQuestValue_Add );	

		// questmgr
		QUESTMGR->UpdateSubQuestData( m_pPlayer, dwQuestIdx, dwSubQuestIdx, dwMaxCount );

		// event
		CQuestEvent qe;
		qe.SetValue( eQuestEvent_Count, dwSubQuestIdx, pQuest->GetSubQuestData(dwSubQuestIdx) );
		QUESTMGR->AddQuestEvent( m_pPlayer, &qe, dwQuestIdx );
		//
	}
}

void CQuestGroup::AddCountFromQWeapon( DWORD dwQuestIdx, DWORD dwSubQuestIdx, DWORD dwMaxCount, DWORD dwWeaponKind )
{
	CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );
	if( pQuest )
	{
		pQuest->ChangeSubQuestValue( dwSubQuestIdx, eQuestValue_Add );	

		// questmgr
		QUESTMGR->UpdateSubQuestData( m_pPlayer, dwQuestIdx, dwSubQuestIdx, dwMaxCount );

		// event
		CQuestEvent qe;
		qe.SetValue( eQuestEvent_Count, dwSubQuestIdx, pQuest->GetSubQuestData(dwSubQuestIdx) );
		QUESTMGR->AddQuestEvent( m_pPlayer, &qe, dwQuestIdx );
		//
	}
}

void CQuestGroup::AddCountFromLevelGap( DWORD dwQuestIdx, DWORD dwSubQuestIdx, DWORD dwMaxCount, DWORD dwMinLevel, DWORD dwMaxLevel )
{
	int nMin = m_pPlayer->GetLevel() - m_pPlayer->GetKillMonsterLevel();
	int nMax = m_pPlayer->GetKillMonsterLevel() - m_pPlayer->GetLevel();
	if( nMin > 0 && nMin > (int)dwMinLevel )	return;
	if( nMax > 0 && nMax > (int)dwMaxLevel )	return;

	CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );
	if( pQuest )
	{
		pQuest->ChangeSubQuestValue( dwSubQuestIdx, eQuestValue_Add );	

		// questmgr
		QUESTMGR->UpdateSubQuestData( m_pPlayer, dwQuestIdx, dwSubQuestIdx, dwMaxCount );

		// event
		CQuestEvent qe;
		qe.SetValue( eQuestEvent_Count, dwSubQuestIdx, pQuest->GetSubQuestData(dwSubQuestIdx) );
		QUESTMGR->AddQuestEvent( m_pPlayer, &qe, dwQuestIdx );
		//
	}
}

void CQuestGroup::AddCountFromMonLevel( DWORD dwQuestIdx, DWORD dwSubQuestIdx, DWORD dwMaxCount, DWORD dwMinLevel, DWORD dwMaxLevel )
{
	WORD wMonLevel = m_pPlayer->GetKillMonsterLevel();
	if( wMonLevel < dwMinLevel )	return;
	if( wMonLevel > dwMaxLevel )	return;

	CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );
	if( pQuest )
	{
		pQuest->ChangeSubQuestValue( dwSubQuestIdx, eQuestValue_Add );	

		// questmgr
		QUESTMGR->UpdateSubQuestData( m_pPlayer, dwQuestIdx, dwSubQuestIdx, dwMaxCount );

		// event
		CQuestEvent qe;
		qe.SetValue( eQuestEvent_Count, dwSubQuestIdx, pQuest->GetSubQuestData(dwSubQuestIdx) );
		QUESTMGR->AddQuestEvent( m_pPlayer, &qe, dwQuestIdx );
		//
	}
}

BOOL CQuestGroup::GiveQuestItem( PLAYERTYPE* pPlayer, DWORD dwQuestIdx, DWORD dwItemIdx, DWORD dwItemNum )
{
	if( !pPlayer ) return FALSE ;

	BOOL bResult = FALSE ;

	QUESTITEM* pQuestItem = m_QuestItemTable.GetData( dwItemIdx );

	if( pQuestItem )
	{
		if(QUESTMGR->GiveQuestItem( pPlayer, dwQuestIdx, pQuestItem->dwItemIdx, 0 ))
		{
			m_QuestItemTable.Remove( pQuestItem->dwItemIdx );
			delete pQuestItem;

			bResult = TRUE ;
		}
		else
		{
			bResult = FALSE ;
		}
	}
	else
	{
		bResult = FALSE ;
	}

	return bResult ;
}

void CQuestGroup::TakeQuestItem( PLAYERTYPE* pPlayer, DWORD dwQuestIdx, DWORD dwSubQuestIdx, DWORD dwItemIdx, DWORD dwItemNum, DWORD dwProbability )
{
	if( CheckQuestProbability(dwProbability) == FALSE )
		return;

	ChangeSubQuestValue( dwQuestIdx, dwSubQuestIdx, eQuestValue_Add );

	// ��ax���� Au�������� ����AIAUAI AO��AAo...
	QUESTITEM* pQuestItem = m_QuestItemTable.GetData( dwItemIdx );
	if( pQuestItem )
	{
		pQuestItem->dwItemNum += dwItemNum;
        DWORD dwValue = GetSubQuestValue( dwQuestIdx, dwSubQuestIdx );
		if( pQuestItem->dwItemNum != dwValue )
		{
			if( pQuestItem->dwItemNum > dwValue )
				pQuestItem->dwItemNum = dwValue;
		}
//		if( pQuestItem->dwItemNum > 100 )
//			pQuestItem->dwItemNum = 100;
		QUESTMGR->UpdateQuestItem( pPlayer, dwQuestIdx, pQuestItem->dwItemIdx, pQuestItem->dwItemNum );
	}
	else	// ������U��e
	{
		pQuestItem = new QUESTITEM;
		pQuestItem->dwQuestIdx = dwQuestIdx;
		pQuestItem->dwItemIdx = dwItemIdx;
		pQuestItem->dwItemNum = dwItemNum;
		m_QuestItemTable.Add( pQuestItem, pQuestItem->dwItemIdx );

		QUESTMGR->TakeQuestItem( pPlayer, pQuestItem->dwQuestIdx, pQuestItem->dwItemIdx, pQuestItem->dwItemNum );
	}
}

void CQuestGroup::TakeQuestItemFromQWeapon( PLAYERTYPE* pPlayer, DWORD dwQuestIdx, DWORD dwSubQuestIdx, DWORD dwItemIdx, DWORD dwItemNum, DWORD dwProbability, DWORD dwWeaponKind )
{
	if( CheckQuestProbability(dwProbability) == FALSE )
		return;

	ChangeSubQuestValue( dwQuestIdx, dwSubQuestIdx, eQuestValue_Add );

	// ��ax���� Au�������� ����AIAUAI AO��AAo...
	QUESTITEM* pQuestItem = m_QuestItemTable.GetData( dwItemIdx );
	if( pQuestItem )
	{
		int nValue = GetSubQuestValue( dwQuestIdx, dwSubQuestIdx );

		pQuestItem->dwItemNum = nValue;
		QUESTMGR->UpdateQuestItem( pPlayer, dwQuestIdx, pQuestItem->dwItemIdx, pQuestItem->dwItemNum );
	}
	else	// ������U��e
	{
		pQuestItem = new QUESTITEM;
		pQuestItem->dwQuestIdx = dwQuestIdx;
		pQuestItem->dwItemIdx = dwItemIdx;
		pQuestItem->dwItemNum = dwItemNum;
		m_QuestItemTable.Add( pQuestItem, pQuestItem->dwItemIdx );

		QUESTMGR->TakeQuestItem( pPlayer, pQuestItem->dwQuestIdx, pQuestItem->dwItemIdx, pQuestItem->dwItemNum );
	}
}

BOOL CQuestGroup::GiveItem( PLAYERTYPE* pPlayer, DWORD wItemIdx, DURTYPE dwItemNum, DWORD dwQuestIdx )			// �������� �ݳ��ϴ� �Լ�.
{	
	if( !pPlayer ) return FALSE ;

	ITEM_TOTALINFO ItemInfo;
	ZeroMemory(
		&ItemInfo,
		sizeof(ItemInfo));
	pPlayer->GetItemtotalInfo(
		ItemInfo,
		GETITEM_FLAG_INVENTORY | GETITEM_FLAG_WEAR);

	BOOL bStackItem = FALSE ;												// ���� ���������� ���θ� ���� ������ �����ϰ� false �����Ѵ�.

	if( ITEMMGR->IsDupItem( wItemIdx ) )									// ���ڷ� �Ѿ�� ��������, ���� �������̸�,
	{
		bStackItem = TRUE ;													// true �����Ѵ�.
	}

	int nGiveNum = (int)dwItemNum ;											// ������ ������ �޴´�.

	if( bStackItem )														// ���� ������ �̶��,
	{
		Give_StackItem_ItemInven(pPlayer, &ItemInfo, wItemIdx, &nGiveNum, dwQuestIdx) ; // ������ �κ��丮���� ���� �������� �ݳ��Ѵ�.

		if( nGiveNum <= 0 ) return TRUE ;									// �ʿ��� ������ ���� �ݳ������� return ó���Ѵ�.

		Give_StackItem_WearInven(pPlayer, &ItemInfo, wItemIdx, &nGiveNum, dwQuestIdx) ;	// ��� �κ��丮���� ���� �������� �ݳ��Ѵ�.
	}
	else																	// �Ϲ� �������̶��,
	{
		Give_NormalItem_ItemInven(pPlayer, &ItemInfo, wItemIdx, &nGiveNum, dwQuestIdx) ;// ������ �κ��丮���� �Ϲ� �������� �ݳ��Ѵ�.

		if( nGiveNum <= 0 ) return TRUE ;									// �ʿ��� ������ ���� �ݳ������� return ó���Ѵ�.

		Give_NormalItem_WearInven(pPlayer, &ItemInfo, wItemIdx, &nGiveNum, dwQuestIdx) ;// ��� �κ��丮���� �Ϲ� �������� �ݳ��Ѵ�.
	}

	if( nGiveNum <= 0 ) return TRUE ;										// �ʿ��� ������ ���� �ݳ������� true return ó���Ѵ�.
	else return FALSE ;														// false return ó���Ѵ�.
}





// 071027 LYW --- QuestGroup : Add function to give stack item of item inventory.
void CQuestGroup::Give_StackItem_ItemInven(PLAYERTYPE* pPlayer, ITEM_TOTALINFO* pInfo, DWORD wItemIdx, int* pGiveNum, DWORD dwQuestIdx)
{
	ASSERT(pPlayer) ;														// ASSERT üũ.
	ASSERT(pInfo) ;															// ASSERT üũ.

	ITEMBASE* pInven ;														// �κ��丮�� ���� �����͸� �����Ѵ�.
	POSTYPE	pos ;															// ������ ���� ���� ������ �����Ѵ�.

	// 071223 LYW --- QuestExecute_Item : ������ �ݳ� ó�� ����.
	int nTotalInvenCount = 0 ;
	nTotalInvenCount = (int)(SLOT_INVENTORY_NUM + pPlayer->GetInventoryExpansionSize()) ;

	for( int count = 0 ; count < nTotalInvenCount ; ++count )				// �κ��丮 ���� �� ��ŭ for���� ������
	{
		if( *pGiveNum <= 0 ) return ;										// �ݳ��� ������ ���� 0���ϸ�, return ó���Ѵ�.

		pInven = NULL ;														// �κ��丮 �����͸� null ó���Ѵ�.
		pInven = &pInfo->Inventory[count] ;									// ��ġ�� �ش��ϴ� �κ��丮 ������ �޴´�.

		ASSERT( pInven ) ;													// ASSERT üũ.

		if( pInven->wIconIdx != wItemIdx ) continue ;						// �ε����� ��ġ���� ������, continue ó���Ѵ�.

		pos = NULL ;														// ��ġ ���� ���� �����͸� null.
		pos = pInven->Position ;											// �κ��丮�� ������ ���� �޴´�.

		int nResult = err_No_Error ;										// ������ ��ȯ ����� ���� ������ �����ϰ�, -1�� ����.
		nResult = ITEMMGR->QG_Discard_StackItem(pPlayer, pos, wItemIdx, pGiveNum) ;	// ��� ���� �޴´�.

		if( nResult == err_No_Error )										// ������ �߻����� �ʾ�����, 
		{
			Log_GiveItem(pPlayer, pInven) ;									// �α׸� �����.
		}
		else																// ������ �߻��ߴٸ�,
		{
			MSG_DWORD4 Msg ;												// �޽��� ����ü�� �����Ѵ�.

			Msg.Category = MP_QUEST ;										// ī�װ����� ����Ʈ�� �����Ѵ�.
			Msg.Protocol = MP_QUEST_GIVEITEM_ERR ;							// ���������� ������ �ݳ� ������ �����Ѵ�.

			Msg.dwData1 = QG_GSI ;											// ���� ��ġ�� �����Ѵ�.
			Msg.dwData2 = nResult ;											// ���� ���� �����Ѵ�.
			Msg.dwData3 = dwQuestIdx ;										// ���� ����Ʈ �ε����� �����Ѵ�.
			Msg.dwData4 = wItemIdx ;										// ������ �ε����� �����Ѵ�.

			pPlayer->SendMsg( &Msg, sizeof(Msg) ) ;							// �÷��̾�� �޽����� �����Ѵ�.

//			*pGiveNum = 0 ;													// ������ ���� 0���� �����Ѵ�.

			return ;														// return.
		}
	}
}





// 071027 LYW --- QuestGroup : Add function to give stack item of wear inventory.
void CQuestGroup::Give_StackItem_WearInven(PLAYERTYPE* pPlayer, ITEM_TOTALINFO* pInfo, DWORD wItemIdx, int* pGiveNum, DWORD dwQuestIdx)
{
	ASSERT(pPlayer) ;														// ASSERT üũ.
	ASSERT(pInfo) ;															// ASSERT üũ.

	ITEMBASE* pInven ;														// �κ��丮�� ���� �����͸� �����Ѵ�.
	POSTYPE	pos ;															// ������ ���� ���� ������ �����Ѵ�.

	//for( int count = 0 ; count < SLOT_INVENTORY_NUM ; ++count )				// �κ��丮 ���� �� ��ŭ for���� ������.
	for( int count = 0 ; count < SLOT_WEAR_NUM ; ++count )				// �κ��丮 ���� �� ��ŭ for���� ������.
	{
		if( *pGiveNum <= 0 ) return ;										// �ݳ��� ������ ���� 0���ϸ�, return ó���Ѵ�.

		pInven = NULL ;														// �κ��丮 �����͸� null ó���Ѵ�.
		pInven = &pInfo->WearedItem[count] ;								// ��ġ�� �ش��ϴ� �κ��丮 ������ �޴´�.

		ASSERT( pInven ) ;													// ASSERT üũ.

		if( pInven->wIconIdx != wItemIdx ) continue ;						// �ε����� ��ġ���� ������, continue ó���Ѵ�.

		pos = NULL ;														// ��ġ ���� ���� �����͸� null.
		pos = pInven->Position ;											// �κ��丮�� ������ ���� �޴´�.

		int nResult = err_No_Error ;										// ������ ��ȯ ����� ���� ������ �����ϰ�, -1�� ����.
		nResult = ITEMMGR->QG_Discard_StackItem(pPlayer, pos, wItemIdx, pGiveNum) ;	// ��� ���� �޴´�.

		if( nResult == err_No_Error )										// ������ �߻����� �ʾ�����, 
		{
			Log_GiveItem(pPlayer, pInven) ;									// �α׸� �����.
		}
		else																// ������ �߻��ߴٸ�,
		{
			MSG_DWORD4 Msg ;												// �޽��� ����ü�� �����Ѵ�.

			Msg.Category = MP_QUEST ;										// ī�װ����� ����Ʈ�� �����Ѵ�.
			Msg.Protocol = MP_QUEST_GIVEITEM_ERR ;							// ���������� ������ �ݳ� ������ �����Ѵ�.

			Msg.dwData1 = QG_GSW ;											// ���� ��ġ�� �����Ѵ�.
			Msg.dwData2 = nResult ;											// ���� ���� �����Ѵ�.
			Msg.dwData3 = dwQuestIdx ;										// ���� ����Ʈ �ε����� �����Ѵ�.
			Msg.dwData4 = wItemIdx ;										// ������ �ε����� �����Ѵ�.

			pPlayer->SendMsg( &Msg, sizeof(Msg) ) ;							// �÷��̾�� �޽����� �����Ѵ�.

//			*pGiveNum = 0 ;													// ������ ���� 0���� �����Ѵ�.

			return ;														// return.
		}
	}
}





// 071027 LYW --- QuestGroup : Add function to give normal item of item inventory.
void CQuestGroup::Give_NormalItem_ItemInven(PLAYERTYPE* pPlayer, ITEM_TOTALINFO* pInfo, DWORD wItemIdx, int* pGiveNum, DWORD dwQuestIdx)
{
	ASSERT(pPlayer) ;														// ASSERT üũ.
	ASSERT(pInfo) ;															// ASSERT üũ.

	ITEMBASE* pInven ;														// �κ��丮�� ���� �����͸� �����Ѵ�.
	POSTYPE	pos ;															// ������ ���� ���� ������ �����Ѵ�.

	// 071223 LYW --- QuestExecute_Item : ������ �ݳ� ó�� ����.
	int nTotalInvenCount = 0 ;
	nTotalInvenCount = (int)(SLOT_INVENTORY_NUM + pPlayer->GetInventoryExpansionSize()) ;

	for( int count = 0 ; count < nTotalInvenCount ; ++count )				// �κ��丮 ���� �� ��ŭ for���� ������.
	{
		if( *pGiveNum <= 0 ) return ;										// �ݳ��� ������ ���� 0���ϸ�, return ó���Ѵ�.

		pInven = NULL ;														// �κ��丮 �����͸� null ó���Ѵ�.
		pInven = &pInfo->Inventory[count] ;									// ��ġ�� �ش��ϴ� �κ��丮 ������ �޴´�.

		ASSERT( pInven ) ;													// ASSERT üũ.

		if( pInven->wIconIdx != wItemIdx ) continue ;						// �ε����� ��ġ���� ������, continue ó���Ѵ�.

		pos = NULL ;														// ��ġ ���� ���� �����͸� null.
		pos = pInven->Position ;											// �κ��丮�� ������ ���� �޴´�.

		int nResult = err_No_Error ;										// ������ ��ȯ ����� ���� ������ �����ϰ�, -1�� ����.
		nResult = ITEMMGR->QG_Discard_NormalItem(pPlayer, pos, wItemIdx, pGiveNum) ;	// ��� ���� �޴´�.

		if( nResult == err_No_Error )										// ������ �߻����� �ʾ�����, 
		{
			Log_GiveItem(pPlayer, pInven) ;									// �α׸� �����.
		}
		else																// ������ �߻��ߴٸ�,
		{
			MSG_DWORD4 Msg ;												// �޽��� ����ü�� �����Ѵ�.

			Msg.Category = MP_QUEST ;										// ī�װ����� ����Ʈ�� �����Ѵ�.
			Msg.Protocol = MP_QUEST_GIVEITEM_ERR ;							// ���������� ������ �ݳ� ������ �����Ѵ�.

			Msg.dwData1 = QG_GNI ;											// ���� ��ġ�� �����Ѵ�.
			Msg.dwData2 = nResult ;											// ���� ���� �����Ѵ�.
			Msg.dwData3 = dwQuestIdx ;										// ���� ����Ʈ �ε����� �����Ѵ�.
			Msg.dwData4 = wItemIdx ;										// ������ �ε����� �����Ѵ�.

			pPlayer->SendMsg( &Msg, sizeof(Msg) ) ;							// �÷��̾�� �޽����� �����Ѵ�.

//			*pGiveNum = 0 ;													// ������ ���� 0���� �����Ѵ�.

			return ;														// return.
		}
	}
}





// 071027 LYW --- QuestGroup : Add function to give normal item of wear inventory.
void CQuestGroup::Give_NormalItem_WearInven(PLAYERTYPE* pPlayer, ITEM_TOTALINFO* pInfo, DWORD wItemIdx, int* pGiveNum, DWORD dwQuestIdx)
{
	ASSERT(pPlayer) ;														// ASSERT üũ.
	ASSERT(pInfo) ;															// ASSERT üũ.

	ITEMBASE* pInven ;														// �κ��丮�� ���� �����͸� �����Ѵ�.
	POSTYPE	pos ;															// ������ ���� ���� ������ �����Ѵ�.

	for( int count = 0 ; count < SLOT_WEAR_NUM ; ++count )				// �κ��丮 ���� �� ��ŭ for���� ������.
	{
		if( *pGiveNum <= 0 ) return ;										// �ݳ��� ������ ���� 0���ϸ�, return ó���Ѵ�.

		pInven = NULL ;														// �κ��丮 �����͸� null ó���Ѵ�.
		pInven = &pInfo->WearedItem[count] ;								// ��ġ�� �ش��ϴ� �κ��丮 ������ �޴´�.

		ASSERT( pInven ) ;													// ASSERT üũ.

		if( pInven->wIconIdx != wItemIdx ) continue ;						// �ε����� ��ġ���� ������, continue ó���Ѵ�.

		pos = NULL ;														// ��ġ ���� ���� �����͸� null.
		pos = pInven->Position ;											// �κ��丮�� ������ ���� �޴´�.

		int nResult = err_No_Error ;										// ������ ��ȯ ����� ���� ������ �����ϰ�, -1�� ����.
		nResult = ITEMMGR->QG_Discard_NormalItem(pPlayer, pos, wItemIdx, pGiveNum) ;	// ��� ���� �޴´�.

		if( nResult == err_No_Error )										// ������ �߻����� �ʾ�����, 
		{
			Log_GiveItem(pPlayer, pInven) ;									// �α׸� �����.
		}
		else																// ������ �߻��ߴٸ�,
		{
			MSG_DWORD4 Msg ;												// �޽��� ����ü�� �����Ѵ�.

			Msg.Category = MP_QUEST ;										// ī�װ����� ����Ʈ�� �����Ѵ�.
			Msg.Protocol = MP_QUEST_GIVEITEM_ERR ;							// ���������� ������ �ݳ� ������ �����Ѵ�.

			Msg.dwData1 = QG_GNW ;											// ���� ��ġ�� �����Ѵ�.
			Msg.dwData2 = nResult ;											// ���� ���� �����Ѵ�.
			Msg.dwData3 = dwQuestIdx ;										// ���� ����Ʈ �ε����� �����Ѵ�.
			Msg.dwData4 = wItemIdx ;										// ������ �ε����� �����Ѵ�.

			pPlayer->SendMsg( &Msg, sizeof(Msg) ) ;							// �÷��̾�� �޽����� �����Ѵ�.

//			*pGiveNum = 0 ;													// ������ ���� 0���� �����Ѵ�.

			return ;														// return.
		}
	}
}





// 071027 LYW --- QuestGroup : Add functino to write log.
void CQuestGroup::Log_GiveItem(PLAYERTYPE* pPlayer, ITEMBASE* pInven)
{
	ASSERT(pPlayer) ;														// ASSERT üũ.
	ASSERT(pInven) ;														// ASSERT üũ.

	ITEMBASE logitem ;														// �α׿� ������ �⺻ ���� ����ü�� �����Ѵ�.

	DWORD dwPlayerID	= pPlayer->GetID() ;								// �÷��̾� ���̵� �޴´�.
	char* pPlayerName	= pPlayer->GetObjectName() ;						// �÷��̾� �̸��� �޴´�.
	MONEYTYPE Money		= pPlayer->GetMoney(eItemTable_Inventory) ;			// �÷��̾� �Ӵϸ� �޴´�.
	EXPTYPE	playerExp	= pPlayer->GetPlayerExpPoint() ;					// �÷��̾� ����ġ�� �޴´�.

	logitem.Durability	= pInven->Durability ;								// �������� �����Ѵ�.
	logitem.dwDBIdx		= pInven->dwDBIdx ;									// DB �ε����� �����Ѵ�.
	logitem.Position	= pInven->Position ;								// �������� �����Ѵ�.
	logitem.wIconIdx	= pInven->wIconIdx ;								// ������ �ε����� �����Ѵ�.

	LogItemMoney(
		dwPlayerID, pPlayerName, 0, "", eLog_ItemDestroyGiveQuest,			
		Money, 0, 0, logitem.wIconIdx , logitem.dwDBIdx, logitem.Position, 
		0, logitem.Durability, playerExp) ;									// �α׸� �����.
}












void CQuestGroup::GiveMoney( PLAYERTYPE* pPlayer, DWORDEX dwMoney )
{
	MONEYTYPE money = pPlayer->SetMoney( dwMoney, MONEY_SUBTRACTION );

	// network send
	MSG_DWORDEX Msg;
	Msg.Category = MP_QUEST;
	Msg.Protocol = MP_QUEST_GIVEMONEY_ACK;
	Msg.dwObjectID = pPlayer->GetID();
	Msg.dweData = money;
	pPlayer->SendMsg(&Msg, sizeof(Msg));
}

BOOL CQuestGroup::TakeItem( PLAYERTYPE* pPlayer, DWORD dwQuestIdx, DWORD dwSubQuestIdx, DWORD wItemIdx, DWORD dwItemNum, DWORD dwProbability )
{
	if( CheckQuestProbability(dwProbability) == FALSE )
		return TRUE;

	if( !ITEMMGR->GetItemInfo( wItemIdx ) )	return TRUE;

	if( ITEMMGR->ObtainItemFromQuest( pPlayer, wItemIdx, dwItemNum ) == 1 )	return FALSE;

	return TRUE;
}

void CQuestGroup::TakeMoney( PLAYERTYPE* pPlayer, DWORDEX dwMoney )
{
	MONEYTYPE money = pPlayer->SetMoney( dwMoney, MONEY_ADDITION );

	// log
	InsertLogMoney( eMoneyLog_GetFromQuest, pPlayer->GetID(), pPlayer->GetMoney(), dwMoney, 0, 0 );

	// network send
	MSG_DWORDEX Msg;
	Msg.Category = MP_QUEST;
	Msg.Protocol = MP_QUEST_TAKEMONEY_ACK;
	Msg.dwObjectID = pPlayer->GetID();
	Msg.dweData = money;
	pPlayer->SendMsg(&Msg, sizeof(Msg));
}

void CQuestGroup::TakeExp( PLAYERTYPE* pPlayer, DWORD dwExp )
{
	pPlayer->SetPlayerExpPoint( pPlayer->GetPlayerExpPoint() + dwExp );
	//log
	InsertLogExp( eExpLog_GetbyQuest, pPlayer->GetID(), pPlayer->GetLevel(), dwExp, pPlayer->GetPlayerExpPoint(),
		0, 0, /*�����Ƽ ���� - pPlayer->GetPlayerAbilityExpPoint()*/0 );

	// network send
	MSG_DWORD Msg;
	Msg.Category = MP_QUEST;
	Msg.Protocol = MP_QUEST_TAKEEXP_ACK;
	Msg.dwObjectID = pPlayer->GetID();
	Msg.dwData = dwExp;
	pPlayer->SendMsg(&Msg, sizeof(Msg));
}

void CQuestGroup::TakeSExp( PLAYERTYPE* pPlayer, DWORD dwSExp )
{
	/*�����Ƽ ���� - pPlayer->AddAbilityExp( dwSExp );*/
	//log
	InsertLogExp( eExpLog_SExpGetbyQuest, pPlayer->GetID(), pPlayer->GetLevel(), dwSExp, pPlayer->GetPlayerExpPoint(),
		0, 0, /*�����Ƽ ���� - pPlayer->GetPlayerAbilityExpPoint()*/0 );

	// network send
	MSG_DWORD Msg;
	Msg.Category = MP_QUEST;
	Msg.Protocol = MP_QUEST_TAKESEXP_ACK;
	Msg.dwObjectID = pPlayer->GetID();
	Msg.dwData = dwSExp;
	pPlayer->SendMsg(&Msg, sizeof(Msg));
}

void CQuestGroup::TakeMoneyPerCount( PLAYERTYPE* pPlayer, DWORD dwQuestIdx, DWORD dwSubQuestIdx, DWORD dwItemIdx, DWORDEX dwMoney )
{
	QUESTITEM* pQuestItem = m_QuestItemTable.GetData( dwItemIdx );
	if( pQuestItem )
	{
		if( pQuestItem->dwItemNum > 0 )		// Item�� �ϳ��� �ִٸ�...
		{
			MONEYTYPE total = pQuestItem->dwItemNum * dwMoney;

			// quest item ����
			m_QuestItemTable.Remove( pQuestItem->dwItemIdx );
			QUESTMGR->GiveQuestItem( pPlayer, dwQuestIdx, pQuestItem->dwItemIdx, 0 );
			delete pQuestItem;
			ChangeSubQuestValue( dwQuestIdx, dwSubQuestIdx, eQuestValue_Reset );

			// money ���
			MONEYTYPE money = pPlayer->SetMoney( total, MONEY_ADDITION );
			
			MSG_DWORDEX Msg;
			Msg.Category = MP_QUEST;
			Msg.Protocol = MP_QUEST_TAKEMONEY_ACK;
			Msg.dwObjectID = pPlayer->GetID();
			Msg.dweData = money;
			pPlayer->SendMsg(&Msg, sizeof(Msg));
		}
		else
		{
		}
	}
	else	// Item�� ������...
	{
	}
}

void CQuestGroup::RegenMonster( DWORD dwRegenType )
{
	QUESTREGENMGR->MonsterRegen( m_pPlayer, dwRegenType );
}





//=========================================================================
//	NAME : MapChange
//	DESC : The function to move player to specified destination. 080104 LYW
//=========================================================================
void CQuestGroup::MapChange( MAPTYPE destinationMap, float xPos, float zPos )
{
	m_pPlayer->SendPlayerToMap(destinationMap, xPos, zPos) ;
}

void CQuestGroup::ChangeStage( DWORD dwGrade, DWORD dwIndex ) 
{
	BYTE byStage = (BYTE)dwGrade ;
	BYTE byIndex = (BYTE)dwIndex ;

	if( byIndex > 0 && byIndex < 7 )
	{
		switch( byStage )
		{
		case 2 : if( byIndex > 0 && byIndex < 3 ) m_pPlayer->SetJob( byStage, byIndex ) ; break ;
		case 3 : if( byIndex > 0 && byIndex < 4 ) m_pPlayer->SetJob( byStage, byIndex ) ; break ;
		case 4 : if( byIndex > 0 && byIndex < 5 ) m_pPlayer->SetJob( byStage, byIndex ) ; break ;
		case 5 : if( byIndex > 0 && byIndex < 7 ) m_pPlayer->SetJob( byStage, byIndex ) ; break ;
		case 6 : if( byIndex > 0 && byIndex < 4 ) m_pPlayer->SetJob( byStage, byIndex ) ; break ;
		}
	}
}

void CQuestGroup::RegistCheckTime( DWORD dwQuestIdx, DWORD dwSubQuestIdx, BYTE bType, DWORD dwDay, DWORD dwHour, DWORD dwMin )
{
	CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );
	if( !pQuest )	return;

	BOOL bStart = TRUE;
	stTIME curTime, regTime;
	curTime.value = GetCurTime();
	if( bType == 1 )		// ���� �ð�
	{		
		if( curTime.GetHour() > dwHour )
		{
			regTime.SetTime( 0, 0, dwDay, 0, 0, 0 );
			regTime += curTime;
			regTime.SetTime( regTime.GetYear(), regTime.GetMonth(), regTime.GetDay(), dwHour, dwMin, 0 );
		}
		else if( curTime.GetHour() == dwHour )
		{
			if( curTime.GetMinute() < dwMin )
			{
				regTime.SetTime( curTime.GetYear(), curTime.GetMonth(), curTime.GetDay(), dwHour, dwMin, 0 );
			}
			else
			{
				regTime.SetTime( 0, 0, dwDay, 0, 0, 0 );
				regTime += curTime;
				regTime.SetTime( regTime.GetYear(), regTime.GetMonth(), regTime.GetDay(), dwHour, dwMin, 0 );
			}
		}
		else
		{
			regTime.SetTime( curTime.GetYear(), curTime.GetMonth(), curTime.GetDay(), dwHour, dwMin, 0 );
		}
	}
	else if( bType == 2 )	// ���� �ð���
	{
		regTime.SetTime( 0, 0, dwDay, dwHour, dwMin, 0 );
		regTime += curTime;
	}
	else if( bType == 3 )	// ���� �ð��� ����
	{
		regTime.SetTime( 0, 0, dwDay, dwHour, dwMin, 0 );
		regTime += curTime;
	}
	else if( bType == 4 )	// ���� �ð��� ����
	{
		regTime.SetTime( curTime.GetYear(), curTime.GetMonth(), curTime.GetDay(), dwHour, dwMin, 0 );
	}
	else
	{
		regTime.value = 0;
		bStart = FALSE;
	}

	pQuest->RegistCheckTime( bStart, bType, regTime );	
}


DWORD CQuestGroup::GetSubQuestValue( DWORD dwQuestIdx, DWORD dwSubQuestIdx )
{
	CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );
	if( !pQuest )	return DWORD(-1);

	return pQuest->GetSubQuestData( dwSubQuestIdx );
}

BOOL CQuestGroup::IsValidDateTime( DWORD dwQuestIdx )
{
	CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );
	if( !pQuest )	return FALSE;

	return pQuest->IsValidDateTime();
}

// 100414 ONS ���ú����� �����Ѵ�.
BOOL CQuestGroup::TakeSelectItem( PLAYERTYPE* pPlayer, CQuestGroup* pQuestGroup, DWORD dwQuestIdx, DWORD dwSubQuestIdx )
{
	if( !pQuestGroup ) 
		return FALSE;
    
	const REQUITALINFO* pInfo = pQuestGroup->FindRequitalIndex( dwQuestIdx );
	if( !pInfo ) 
		return FALSE;
	
	if( !TakeItem( pPlayer, pInfo->dwRequitalIndex, dwQuestIdx, dwSubQuestIdx, pInfo->dwRequitalCount, MAX_QUEST_PROBABILITY ) )
		return FALSE;

	DeleteQuestRequital( dwQuestIdx );
	
	return TRUE;
}

// 100414 ONS ���ú��� ������ �����Ѵ�.
void CQuestGroup::SetQuestRequital( DWORD dwQuestIdx, DWORD dwRequitalIndex, DWORD dwRequitalCount )
{
	REQUITALINFO& Info = m_RequitalIndexMap[dwQuestIdx];
	Info.dwRequitalIndex = dwRequitalIndex;
	Info.dwRequitalCount = dwRequitalCount;
}

// 100414 ONS �ش�����Ʈ�� ���ú��������� �����Ѵ�.
void CQuestGroup::DeleteQuestRequital( DWORD dwQuestIdx )
{
	RequitalIndexMap::iterator iter = m_RequitalIndexMap.find( dwQuestIdx );
	if( m_RequitalIndexMap.end() == iter )
	{
		return;
	}
	m_RequitalIndexMap.erase( iter );
}

// 100414 ONS �ش� ����Ʈ�� ���ú��������� �˻��Ѵ�.
REQUITALINFO* CQuestGroup::FindRequitalIndex( DWORD dwQuestIdx )
{
	RequitalIndexMap::iterator iter = m_RequitalIndexMap.find( dwQuestIdx );
	return m_RequitalIndexMap.end() == iter ? 0 : &(iter->second);
}
