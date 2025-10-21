#pragma once


#include "./Interface/cDialog.h"
//#include "GameResourceStruct.h"

class cListDialog;
class cEditBox;
class cButton;
class CMouse;
class CExchangeItem;
class cStatic;

#define MAX_VOLUME_TEXT	"2000"
/*
enum ITEM_TYPE
{
	WEAPON,
	CLOTHES,
	ACCESSORY,
	POTION,
	MATERIAL,
	ETC,
	ITEM_MALL,
	ITEM_TYPE_COUNT,
};*/

enum ITEM_TYPE
{
	// 091127 ONS ���Ż��� �����۸���Ʈ ����/�߰�
	WEAPON,		//1	�����
	CLOTHES,	//2	��1
	ACCESSORY,	//3	�׼�����
	POTION,		//4	�Ҹ�ǰ
	MATERIAL,	//5	���
	PET,		//6	���ǰ
	COSTUME,	//7	�ڽ�Ƭ
	PRODUCTION,	//8	����
	HOUSING,	//9	�Ͽ�¡
	ETC,		//10	��Ÿ

	ITEM_TYPE_COUNT,
};

typedef struct _BUYREGINFO{
	WORD Type;
	WORD Item;
	WORD Class;
	WORD Volume;
	DWORDEX Money;
} BUY_REG_INFO;


struct ITEM_INFO;


class CBuyRegDialog : public cDialog  
{
	cListDialog* m_pTypeList;
	cListDialog* m_pItemList[ITEM_TYPE_COUNT];
	cListDialog* m_pClassList;
	cEditBox* m_pVolumeEdit;
	cEditBox* m_pMoneyEdit;
	cButton* m_pRegBtn;
	cButton* m_pDltBtn;

	cStatic* m_pClassText;
	cStatic* m_pClassRect1;
	cStatic* m_pClassRect2;
	cStatic* m_pItemBigRect;
	cStatic* m_pItemSmallRect1;
	cStatic* m_pItemSmallRect2;

	WORD m_nType;
	WORD m_nItem;
	WORD m_nClass;
	
	WORD m_nVolume;
	DWORDEX m_nMoney;

	void LoadItemList();

	DWORDEX	mMoney;
	DWORD	mColor;

public:
	CBuyRegDialog();
	virtual ~CBuyRegDialog();

	void Linking();
	void Show();
	void Close();
	void InitRegInfo();
	void SetRegInfo(BUY_REG_INFO& RegInfo);

	void UpdateType();
	void UpdateItem();
	void UpdateClass();
	ITEM_INFO* GetSelectItemInfo();
	BOOL GetBuyRegInfo(BUY_REG_INFO& RegInfo);

	void OnActionEvnet(LONG lId, void * p, DWORD we);
	virtual void Render();
};