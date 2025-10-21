// DlgBarSheet.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "RegenTool.h"
#include "DlgBarSheet.h"
#include ".\dlgbarsheet.h"


// CDlgBarSheet

IMPLEMENT_DYNAMIC(CDlgBarSheet, CPropertySheet)
CDlgBarSheet::CDlgBarSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	AddPage( &m_PageMonster );
}

CDlgBarSheet::CDlgBarSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPage( &m_PageMonster );
}

CDlgBarSheet::~CDlgBarSheet()
{
}


BEGIN_MESSAGE_MAP(CDlgBarSheet, CPropertySheet)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CDlgBarSheet �޽��� ó�����Դϴ�.

void CDlgBarSheet::OnSize(UINT nType, int cx, int cy)
{
	CPropertySheet::OnSize(nType, cx, cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	int nBlankX, nBlankY;

	nBlankX = 15;
	nBlankY = 15;

	CRect sz( 0, 0, cx, cy );
	sz.DeflateRect( nBlankX, nBlankX);

	CWnd* pTab = GetTabControl();
	if( pTab )
	{
		pTab->MoveWindow( &sz );
	}

	sz.SetRect( nBlankX + 4, nBlankY + 21, cx - nBlankX - 4, cy - nBlankY - 4 );

	CPropertyPage* pPage = GetActivePage();
	if( pPage )
	{
		pPage->MoveWindow( &sz );
	}

}
