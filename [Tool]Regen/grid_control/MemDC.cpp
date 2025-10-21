#include "stdafx.h"
#include "MemDC.h"

CMyMemDC::CMyMemDC(CDC* pDC) : CDC()
{
    ASSERT(pDC != NULL);

    m_pDC = pDC;
    m_pOldBitmap = NULL;
#ifndef _WIN32_WCE_NO_PRINTING
    m_bMemDC = !pDC->IsPrinting();
#else
    m_bMemDC = FALSE;
#endif

    if (m_bMemDC)
    {
        pDC->GetClipBox(&m_rect);
        CreateCompatibleDC(pDC);
        m_bitmap.CreateCompatibleBitmap(pDC, m_rect.Width(), m_rect.Height());
        m_pOldBitmap = SelectObject(&m_bitmap);
#ifndef _WIN32_WCE
        SetWindowOrg(m_rect.left, m_rect.top);
#endif
        FillSolidRect(m_rect, pDC->GetBkColor());
    }
    else
    {
#if !defined(_WIN32_WCE) || ((_WIN32_WCE > 201) && !defined(_WIN32_WCE_NO_PRINTING))
        m_bPrinting = pDC->m_bPrinting;
#endif
        m_hDC       = pDC->m_hDC;
        m_hAttribDC = pDC->m_hAttribDC;
    }
}

CMyMemDC::~CMyMemDC()
{
    if (m_bMemDC)
    {
        m_pDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
                      this, m_rect.left, m_rect.top, SRCCOPY);
        SelectObject(m_pOldBitmap);
    }
    else
    {
        m_hDC = m_hAttribDC = NULL;
    }
}

CMyMemDC* CMyMemDC::operator->()
{
    return this;
}

CMyMemDC::operator CMyMemDC*()
{
    return this;
}
