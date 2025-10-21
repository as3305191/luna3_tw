#pragma once

#define ITEMDROP_OBJ CItemDrop::GetInstance()

struct MONSTER_ITEM
{
#ifdef _DEBUG
	char Name[MAX_ITEMNAME_LENGTH+1];			// ������ �̸�
#endif
	DWORD wItemIdx;							// �ش� ���Ͱ� ������� �� ����ϴ� ������ - "0"�� �Է� �� ��� ������ ����
	DWORD dwDropPercent;					// �ش���Ͱ� ������� �� ����ϴ� �������� ��� Ȯ�� - �ڸ����� ��ġ�� �Է� ��) 75000 = 7.5%
	BYTE byCount ;							// �ش� ���Ͱ� ������� �� ����ϴ� �������� �ִ� ���� 1~�ִ밳�� �����ϰ� ���.
};

#define MAX_DROP_ITEM_PERCENT 10000

struct MONSTER_DROP_ITEM
{
	WORD			wItemDropIndex;
	WORD			wTotalNum;
	DWORD			dwCurTotalRate;
	DWORD			dwCurMonsterDropItemPercent[MAX_DROPITEM_NUM];
	MONSTER_ITEM	MonsterDropItemPercent[MAX_DROPITEM_NUM];
};

class CItemDrop  
{	
public:
	GETINSTANCE(CItemDrop);
 	
	CItemDrop();
	virtual ~CItemDrop();
	MONEYTYPE DropMoney(const BASE_MONSTER_LIST&, int nPenaltyType);
	BOOL DropItem(WORD MonsterKind, const MONSTER_DROPITEM&, CPlayer*, int nPenaltyType);
};