#pragma once


#include "./Interface/cDialog.h"
#include "./Interface/cImage.h"

class cIconGridDialog;
class cEditBox;
class cPushupButton;
class cButton;
class cStatic;

class CExchangeDialog  : public cDialog
{
protected:

//for linking
	BOOL				m_bInit;
	cIconGridDialog*	m_pMyIconGridDlg;
	cIconGridDialog*	m_pOpIconGridDlg;
	cEditBox*			m_pMyMoneyEditBox;
	cEditBox*			m_pOpMoneyEditBox;
	cPushupButton*		m_pLockBtn;
	cPushupButton*		m_pExchangeBtn;
	cButton*			m_pInputMoneyBtn;

	cStatic*			m_pMyNameStc;
	cStatic*			m_pOpNameStc;

	cImage				m_LockImage;
	
	BOOL				m_bMyLock;
	BOOL				m_bOpLock;
	
	DWORDEX				m_dwMoney;
	DWORDEX				m_dwOpMoney;
	
	BOOL				m_bExchangable;

	int					m_nMyItemNum;		//��ȯ�� �� ������ ��
	int					m_nOpItemNum;		//��ȯ�� ��� ������ ��

public:
	CExchangeDialog();
	virtual ~CExchangeDialog();
	
	void Linking();
	virtual void SetActive( BOOL val );
//	virtual DWORD ActionEvent( CMouse* mouseInfo, CKeyboard* keyInfo );
	virtual void Render();
	void OnActionEvent(LONG lId, void * p, DWORD we);

	virtual BOOL FakeMoveIcon( LONG x, LONG y, cIcon* icon );
	void AddItem( int nType, POSTYPE pos, cIcon* pItem );
	void DelItem( int nType, POSTYPE pos );
	void DelItemOptionInfo( POSTYPE pos );
	void DelItemOptionInfoAll();
	//void DelItemRareOptionInfo( POSTYPE pos );
	//void DelItemRareOptionInfoAll();

	DWORDEX InputMoney( int nType, DWORDEX dwMoney );

	void OpLock( BOOL bLock );	//��� ���ɱ�
	void MyLock( BOOL bLock );

	DWORDEX GetMoney() { return m_dwMoney; }
	BOOL IsMyLock()	{ return m_bMyLock; }
	BOOL IsOpLock() { return m_bOpLock; }
	
	void SetPlayerName( char* pMyName, char* pOpName );

	void MoneyInputButtonEnable( BOOL bEnable );

	static void OnInputMoney( LONG iId, void* p, DWORDEX param1, void * vData1, void * vData2 );
	static void OnInputCancel( LONG iId, void* p, DWORDEX param1, void * vData1, void * vData2 );
};