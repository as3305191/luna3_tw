#include "stdafx.h"
#include "InputNameDlg.h"
#include "WindowIDEnum.h"
#include "cEditBox.h"
#include "../input/Mouse.h"
#include "cMsgBox.h"
#include "ChatManager.h"
#include "PartyManager.h"
#include "ObjectManager.h"

CInputNameDlg::CInputNameDlg()
{
}

CInputNameDlg::~CInputNameDlg()
{
}

void CInputNameDlg::Linking()
{
	m_CharName	= (cEditBox*)GetWindowForID( ID_INPUTNAMEDLG_EDIT_NAME );
}

void CInputNameDlg::OnActionEvent(LONG lId, void* p, DWORD we)
{
	if( we & WE_BTNCLICK )
	{
 		switch( lId )
		{
		case ID_INPUTNAMEDLG_BUTTON_OK:
			{
				// �۾����ѽð� Ȯ��
				const DWORD dwLastTime = HERO->GetInvitePartyByNameLastTime();
				const DWORD dwLimitTime = HERO->GetInvitePartyByNameLimitTime();

				// �۾� ���ɽð��� ������� �������
				if( dwLastTime != 0 &&
					gCurTime < dwLastTime + dwLimitTime )
				{
					float fRemainTime = float( dwLimitTime - (gCurTime - dwLastTime) ) / 1000.0f;
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1923 ), fRemainTime );
					return;
				}

				// �۾� ���ѽð� ����
				HERO->SetInvitePartyByNameLastTime( 0 );
				HERO->SetInvitePartyByNameLimitTime( 0 );

				char* const szCharName = m_CharName->GetEditText();

				if( strlen( szCharName ) <= 0 )
					break;

				// �ڱ��ڽ��� �̸��� ���� ���(��ҹ��� ���о��� ��)
				if( stricmp( HERO->GetObjectName(), szCharName ) == 0 )
				{
					CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg( 1269 ) );
					SetActive( FALSE );
					break;
				}

				// ��Ƽ �ʴ� �޼����� ������
				PARTYMGR->InvitePartyMemberByNameSyn( szCharName );

				SetActive( FALSE );
			}
			break;
		}
	}
}

void CInputNameDlg::SetActive(BOOL val)
{
	cDialog::SetActive( val );

	if (m_CharName) {
		m_CharName->SetEditText( "" );
	}
}