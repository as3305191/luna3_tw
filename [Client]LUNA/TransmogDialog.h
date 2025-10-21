#pragma once


#include "interface/cDialog.h"


class CItem;
class CVirtualItem;
class cIconDialog;
class cStatic;

struct EnchantScript;


class CTransmogDialog : public  cDialog
{
public:
	CTransmogDialog();
	virtual ~CTransmogDialog();
	virtual BOOL FakeMoveIcon( LONG x, LONG y, cIcon* );
	virtual void OnActionEvent(LONG lId, void* p, DWORD we);
	void Linking();
	void Restore();
	virtual DWORD ActionEvent(CMouse*);
	void Send();
	virtual void SetActive( BOOL );
	void Succeed( const MSG_ITEM_TRANSMOG_ACK&);
	void Fail();


private:
	BOOL comparePart(const ITEM_INFO* firstInfo , const ITEM_INFO* secondInfo);
	BOOL isTransmogable(CItem* item);
	DWORD transmogItemIdx;

private:
	cStatic* m_pTransmogCost;
	cIconDialog*	mIconDialog;
	CVirtualItem*	mSourceItem;
	CVirtualItem*	mDummyItem;
	CItem*			mMaterialItem;
	
	MSG_ITEM_TRANSMOG_SYN	mMessage;
	ITEM_OPTION		m_PrevOption;

};