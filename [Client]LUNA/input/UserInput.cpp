// UserInput.cpp: implementation of the CUserInput class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UserInput.h"
#include "../Gamein.h"
#include "../MHCamera.h"
#include "../Object.h"
#include "../MoveManager.h"
#include "../Monster.h"
#include "../Player.h"
#include "../Npc.h"
#include "../ObjectManager.h"
#include "../Interface\cWindowManager.h"
#include "../Interface\cEditBox.h"
#include "../WindowIDEnum.h"
#include <WinUser.h>
#include "CameraRotate.inl"
#include "CameraWheelZoom.inl"

#include "MacroManager.h"
#include "MainGame.h"



CUserInput	g_UserInput;
extern HWND g_hWnd;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUserInput::CUserInput()
{
	m_MouseEventThrower.SetDefaultReceiver(&m_HeroMove);
	m_MouseEventThrower.SetReceiverToDefault();
	
	m_bFocus = TRUE;
	m_bAllowMouseClick = TRUE;
}

CUserInput::~CUserInput()
{

}


void CUserInput::Init()
{
	HRESULT hr;

	hr = DirectInput8Create( GetModuleHandle(NULL), DIRECTINPUT_VERSION, 
								IID_IDirectInput8, (VOID**)&m_pdi, NULL );

	ASSERT(hr==S_OK);

	m_Keyboard.Init( m_pdi );
	m_Mouse.Init();
}


void CUserInput::Release()
{
	SAFE_RELEASE( m_pdi );

	m_Keyboard.Release();
	m_Mouse.Release();
}


void CUserInput::Process()
{
//////////////
//Update Input
	m_Keyboard.UpdateKeyboardState();
	//���콺�� winproc���� ���� ������Ʈ

///////////////////
//Key Input Process
	StartGetKeyState();
//	do	//Key Input event�� ������ �ѹ��� ����Ǿ���.(press��������?)
//	{
//		GetNextKeyState();
//		MACROMGR->KeyboardInput( &m_Keyboard );
//		WINDOWMGR->KeyboardInput( &m_Keyboard );
//	}
//	while( !m_Keyboard.IsEmptyKeyboardEvent() );

	if( m_bFocus )
	{
		while( GetNextKeyState() )
		{
			MACROMGR->KeyboardInput( &m_Keyboard );
			WINDOWMGR->KeyboardInput( &m_Keyboard );
		}

		MACROMGR->KeyboardPressInput( &m_Keyboard );
	}

/////////////////////
//Mouse Input Process
	do	//mouse event�� ������ �ѹ��� ����Ǿ���.
	{
		m_Mouse.GetNextMouseEvent();	//��Ŀ���� ��� que���� pop�� �Ǿ��ϴ�..

		// 100125 ShinJS --- ���콺 ó�� ��� ���� �߰�
		if( m_bFocus && m_bAllowMouseClick )
		{
			WINDOWMGR->MouseInput( &m_Mouse );
			//if( !WINDOWMGR->IsMouseInputProcessed() )	//MouseEventThrower.Process()������ �ű�
			if( MAINGAME->GetCurStateNum() == eGAMESTATE_GAMEIN )
			{
				//---KES ���󰡱�
				m_MouseEventThrower.Follow();
				//---------------
				m_MouseEventThrower.Process( &m_Mouse );
			}
		}
	}
	while( !m_Mouse.IsEmptyMouseEvent() );
	
////////
//Camera
	CameraRotate( &m_Keyboard, &m_Mouse );
	CameraWheelZoom( &m_Keyboard, &m_Mouse );
}


void CUserInput::StartGetKeyState()
{
	m_Keyboard.StartGetKeyState();
}


BOOL CUserInput::GetNextKeyState()
{
	return m_Keyboard.GetNextKeyState();
}

void CUserInput::SetInputFocus( BOOL bFocus )
{
	m_bFocus = bFocus;
	MACROMGR->SetKeyMoveDir(KEY_MOVE_STOP);
}