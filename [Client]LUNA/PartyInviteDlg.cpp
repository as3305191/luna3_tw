// PartyInviteDlg.cpp: implementation of the CPartyInviteDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PartyInviteDlg.h"
#include "WindowIDEnum.h"
#include "./Interface/cTextArea.h"
#include "./Interface/cStatic.h"
#include "ChatManager.h"
#include "cResourceManager.h"

CPartyInviteDlg::CPartyInviteDlg() :
mInviterPlayerIndex(0)
{
	m_type = WT_PARTYINVITEDLG;
}

CPartyInviteDlg::~CPartyInviteDlg()
{

}

void CPartyInviteDlg::Linking()
{
	m_pDistribute = (cStatic*)GetWindowForID(PA_INVITEDISTRIBUTE);
	m_pOption = (cStatic*)GetWindowForID(PA_INVITEOPTION);
	m_pInviter = (cTextArea*)GetWindowForID(PA_INVITER);
}

void CPartyInviteDlg::Update(const MSG_NAME_DWORD4& message)
{
	mInviterPlayerIndex = message.dwData3;
	
	switch(PARTY_OPTION(message.dwData2))
	{
	case ePartyOpt_Damage :
		{
			m_pDistribute->SetStaticText(
				RESRCMGR->GetMsg(163));
		}
		break ;

	case ePartyOpt_Sequence :
		{
			m_pDistribute->SetStaticText(
				RESRCMGR->GetMsg(161));
		}
		break ;
	}

	DWORD m_DiceGrade = message.dwData4;

	switch(m_DiceGrade)
	{
	case 0:
		m_pOption->SetStaticText(RESRCMGR->GetMsg(1194));
		break;
	case 1:
		m_pOption->SetStaticText(RESRCMGR->GetMsg(1182));
		break;
	case 2:
		m_pOption->SetStaticText(RESRCMGR->GetMsg(1183));
		break;
	case 3:
		m_pOption->SetStaticText(RESRCMGR->GetMsg(1184));
		break;
	case 4:
		m_pOption->SetStaticText(RESRCMGR->GetMsg(1185));
		break;
	case 5:
		m_pOption->SetStaticText(RESRCMGR->GetMsg(1193));
		break;
	}

	TCHAR text[MAX_PATH] = {0};
	_sntprintf(
		text,
		_countof(text),
		CHATMGR->GetChatMsg(765),
		message.Name);
	m_pInviter->SetScriptText(
		text);
}