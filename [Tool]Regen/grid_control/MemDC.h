// MemDC.h

#if !defined(AFX_MEMDC_H__CA1D3541_7235_11D1_ABBA_00A0243D1382__INCLUDED_)
#define AFX_MEMDC_H__CA1D3541_7235_11D1_ABBA_00A0243D1382__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif

#include <afxwin.h>  // Untuk CDC, CRect, dll

class CMyMemDC : public CDC
{
public:
    CMyMemDC(CDC* pDC);
    virtual ~CMyMemDC();

    CMyMemDC* operator->();
    operator CMyMemDC*();

private:
    CBitmap  m_bitmap;
    CBitmap* m_pOldBitmap;
    CDC*     m_pDC;
    CRect    m_rect;
    BOOL     m_bMemDC;
};

#endif // !defined(AFX_MEMDC_H__...)
