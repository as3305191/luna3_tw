#pragma once

#define ITEMMGR CItemManager::GetInstance()

class CItemSlot;
class CBackupSlot;

struct DealerItem
{
	BYTE Tab;
	BYTE Pos;
	DWORD ItemIdx;
};

typedef void (*DBResult)(void * pPlayer, WORD ArrayID);

struct ITEMOBTAINARRAYINFO
{
	DWORD wObtainArrayID;
	DBResult CallBack;
	DWORD dwFurnisherIdx;
	eLogitemmoney wType;
	ITEMOBTAINARRAY	ItemArray;
};

struct DealerData
{
	BOOL FindItem(DWORD ItemIdx)
	{
		POS pos = m_DealItemList.GetFirstPos();
		DealerItem* pItem = NULL;
		while((pItem = m_DealItemList.GetNextPos(pos) ) != NULL)
		{
			if(pItem->ItemIdx == ItemIdx)
				return TRUE;
		}

		return FALSE;
	}

	cLinkedList<DealerItem> m_DealItemList;
};

// 071028 LYW --- ItemManager : Add enum code for error of discard item from quest group.
enum QG_DISCARD_ERR															// ����Ʈ �׷쿡�� ������ �����⸦ �� �� �߻��ϴ� ���� ����.
{
	err_No_Error = 10,														// ���� ����.
	err_Invalid_CItemSlot,													// ������ ���� ������ ����.
	err_Invalid_ITEMBASE,													// ������ �⺻ ������ ����.
	err_Not_Same_ItemIdx,													// ������ �ε��� ��ġ ���� ����.
	err_Not_StackItem,														// ���� �������� �ƴҰ�� ����.
	err_Not_NormalItem,														// �Ϲ� �������� �ƴҰ�� ����.
	err_Over_ItemCount,														// �ִ� ���� ī��Ʈ ���� ����.
	err_Invalid_RemainCount,												// ���� ������ �� ����.
	err_Failed_UpdateItem,													// ������ ������Ʈ ����.
	err_Failed_DeleteItem,													// ������ ���� ����.
};

class CMHFile;

class CItemManager  
{
	// desc_hseos_ü����������01
	// S ü���������� �߰� added by hseos 2007.07.29
	enum CHANGEITEM_RESULT_KIND										// ü���� ������ ��� ����
	{
		TRK_ITEM = 0,												// ..������
		TRK_MONEY,													// ..��
	};

	struct stCHANGEITEM_RESULT_BASE
	{
		CHANGEITEM_RESULT_KIND	eResultKind;						// ..��� ����
		DWORD					nValue01;							// ..��ġ1
		DWORD					nValue02;							// ..��ġ2
		DWORD					nValue03;							// ..��ġ3
		DWORD					nProbaValue;						// ..Ȯ��
	};

	struct stCHANGEITEM_RESULT
	{
		int						  nResultBaseNum;
		stCHANGEITEM_RESULT_BASE* stResultBase;
	};

	struct stCHANGEITEM												// ü���� ������ ����
	{
		int						nChangeItemID;						// ..ü���� ������ ID
		int						nResultNum;							// ..��� ����
		stCHANGEITEM_RESULT*	stResult;							// ..���
	};
	// E ü���������� �߰� added by hseos 2007.07.29

	CYHHashTable<DealerData>	m_DealerTable;
	stCHANGEITEM*			m_stChangeItemInfo;
	int						m_nChangeItemInfoNum;
	// E ü���������� �߰� added by hseos 2007.07.29

public:
	CItemManager();
	virtual ~CItemManager();
	//GETINSTANCE(CItemManager);

	static CItemManager* GetInstance();

	void LoadDealerItem();
	DealerData * GetDealer(WORD wDealerIdx);
	int DivideItem( CPlayer*, DWORD ItemIdx,  POSTYPE FromPos, POSTYPE ToPos, DURTYPE FromDur, DURTYPE ToDur);
	int CombineItem( CPlayer*, DWORD ItemIdx, POSTYPE FromPos, POSTYPE ToPos, DURTYPE & FromDur, DURTYPE & ToDur);
	int MoveItem( CPlayer*, DWORD FromItemIdx, POSTYPE FromPos, DWORD ToItemIdx, POSTYPE& ToPos);
	int MovePetItem( CPlayer*, DWORD FromItemIdx, POSTYPE FromPos, DWORD ToItemIdx, POSTYPE& ToPos);
	int DiscardItem(CPlayer*, POSTYPE whatPos, DWORD whatItemIdx, DURTYPE whatItemNum);

	   MP_PROTOCOL_ITEM Transmog(CPlayer*, const MSG_ITEM_TRANSMOG_SYN*, eLogitemmoney&);
	   
	WORD GetItemStackNum(DWORD dwItemIndex);
	int QG_Discard_StackItem(CPlayer*, POSTYPE, DWORD whatItemIdx, int* nGiveNum);
	int QG_Discard_NormalItem(CPlayer*, POSTYPE, DWORD whatItemIdx, int* nGiveNum);
	void SendErrorMessage(CObject*, BYTE protocol, int errorCode);
	int ObtainItemEx(CPlayer*, ITEMOBTAINARRAYINFO*, DWORD whatItemIdx, WORD whatItemNum, WORD* EmptyCellPos, WORD EmptyCellNum, WORD ArrayInfoUnitNum, ITEM_SEAL_TYPE);	
	int SellItem(CPlayer*, POSTYPE whatPos, DWORD wSellItemIdx, DURTYPE sellItemDur, DWORD DealerIdx );
	int BuyItem(CPlayer*, DWORD buyItemIdx, WORD buyItemNum, WORD DealerIdx );
	void MonsterObtainItem(CPlayer*, DWORD obtainItemIdx, DWORD dwFurnisherIdx, WORD ItemNum = 1);
	int CheatObtainItem(CPlayer*, DWORD obtainItemIdx, WORD ItemNum);
	int ObtainMonstermeterItem(CPlayer*, DWORD obtainItemIdx, WORD ItemNum);
	int ObtainGeneralItem(CPlayer*, DWORD obtainItemIdx, WORD ItemNum, eLogitemmoney, MP_PROTOCOL_ITEM);
    static void ObtainItemDBResult(CPlayer*, WORD ArrayID);
	static void BuyItemDBResult(CPlayer*, WORD ArrayID );	
	static void DivideItemDBResult(CPlayer*, WORD ArrayID);
	//SW050920 Rare
	//static void RareItemDBResult( CPlayer*, WORD ArrayID, ITEM_RARE_OPTION_INFO* );

	void NetworkMsgParse( DWORD dwConnectionIndex, BYTE Protocol, void* );

	void LoadItemList();
	void ReloadItemList();
	void SetItemInfo(DWORD ItemIdx,ITEM_INFO*, CMHFile*);
	ITEM_INFO* GetItemInfo(DWORD wItemIdx);
	eItemUse_Err UseItem(CPlayer*, POSTYPE, DWORD wItemIdx);
	ITEM_INFO* GetUseItemInfo(CPlayer*, POSTYPE, DWORD wItemIdx);
	ITEMOBTAINARRAYINFO* Alloc(CPlayer*, MP_CATEGORY, BYTE protocol, DWORD dwObjectID, DWORD dwFurnisherIdx, eLogitemmoney, WORD ObtainNum, DBResult);
	void Free(CPlayer*, ITEMOBTAINARRAYINFO*);
	BOOL IsDupItem(DWORD wItemIdx);
	BOOL IsTwoHand(DWORD wItemIdx);
	BOOL IsRare(const ITEM_INFO&) const;
	void UpdateResult(CPlayer&, CItemSlot&, const CBackupSlot&, ITEM_UPDATE_RESULT&, MP_PROTOCOL_ITEM, eLogitemmoney logForRemove, eLogitemmoney logForInsert, const ICONBASE& itemForMixSupport);
	BOOL ItemMoveUpdateToDBbyTable(CPlayer*, DWORD dwfromDBIdx, DWORD dwfromIconIdx,  POSTYPE frompos, DWORD dwtoDBIdx, POSTYPE topos);
	void ItemUpdatebyTable(CPlayer*, DWORD dwDBIdx, DWORD wIconIdx, DURTYPE FromDur, POSTYPE Position, POSTYPE QuickPosition);
	const ITEMBASE * GetItemInfoAbsIn(CPlayer*, POSTYPE Pos);

	void ItemMoveLog(POSTYPE FromPos, POSTYPE ToPos, CPlayer*, ITEMBASE* pItem);

	// 071211 KTH -- CPlayer �߰�
	unsigned int GetTotalEmptySlotNum(CItemSlot*, CPlayer*); //060612 Add by wonju

	// desc_hseos_ü����������01
	// S ü���������� �߰� added by hseos 2007.07.29	2007.08.03
	BOOL LoadScriptFileDataChangeItem();
	BOOL ProcChangeItem(CPlayer* pPlayer, DWORD nItemID);
	BOOL IsChangeItem(DWORD nItemID);
	// E ü���������� �߰� added by hseos 2007.07.29	2007.08.03

	WORD CheckExtraSlot(CPlayer*, CItemSlot*, DWORD whatItemIdx, DURTYPE whatItemNum, WORD * EmptyCellPos, WORD & EmptyCellNum);
	WORD GetCanBuyNumInSpace(CPlayer*, CItemSlot*, DWORD whatItemIdx, DURTYPE whatItemNum, WORD * EmptyCellPos, WORD & EmptyCellNum);
private:
	
	WORD GetCanBuyNumInMoney(CPlayer*, WORD butNum, MONEYTYPE Price);
	WORD GetCanBuyNumInFishingPoint(CPlayer*, WORD buyNum, MONEYTYPE Point);
	BOOL CanMovetoGuildWare(POSTYPE FromPos, POSTYPE ToPos, CPlayer*);
	BOOL CanbeMoved(DWORD wIconIdx,POSTYPE pos, CPlayer*);
	BOOL CanEquip(ITEM_INFO*, CPlayer*);
	void ClearQuickPos(ITEMBASE*, POSTYPE ToPos);

	CYHHashTable<ITEM_INFO> m_ItemInfoList;
	// 090923 ONS �޸�Ǯ ��ü
	CPool<ITEMOBTAINARRAYINFO> m_ItemArrayPool;
	CIndexGenerator		m_ArrayIndexCreator;

public:
	int	ObtainItemFromQuest( CPlayer*, DWORD wItemKind, DWORD dwItemNum );
	eWeaponType	GetWeaponKind(DWORD wWeapon);
	int	ObtainItemFromChangeItem( CPlayer*, DWORD wItemKind, WORD wItemNum );
	BOOL CheckHackNpc( CPlayer*, WORD wNpcIdx, WORD wParam=0 );	
	BOOL CheckHackItemMove( CPlayer*, CItemSlot* pFromSlot, CItemSlot* pToSlot );
	BOOL IsValidItem(DWORD nItemID)	{ if (m_ItemInfoList.GetData(nItemID)) return TRUE ; return FALSE; }
	int GetSetItemSize(CPlayer*, const SetScript*);
	eLogitemmoney Reinforce(CPlayer*, const MSG_ITEM_REINFORCE_SYN&, const CBackupSlot*);
	MP_PROTOCOL_ITEM Enchant(CPlayer*, const MSG_ITEM_ENCHANT_SYN*, eLogitemmoney&);
	void ApplyOption(const ITEMBASE& sourceItem, const ITEMBASE& targetItem, ITEM_OPTION&);

private:
	eLogitemmoney Mix(CPlayer*, const MSG_ITEM_MIX_SYN2&, const CBackupSlot*);
	const MixScript* GetMixScript(DWORD itemIndex ) const;
	BOOL AddMixOption(CPlayer&, const ITEMBASE& mixedItem, const ICONBASE& supportItem);
	void Compose(const MSG_ITEM_COMPOSE_SYN&);
	void LoadMixScript();
	void LoadMixSetting();
	// 080925 LUJ, ���� ���� ������ �����´�
	const MixSupportScript* GetMixSupportScript( DWORD itemIndex ) const;

	// Ű: ������ �ε���
	typedef stdext::hash_map< DWORD, MixScript >	MixScriptMap;
	MixScriptMap									mMixScriptMap;
	// 080925 LUJ, ������ CommonStruct.h�� �̵���Ŵ
	MixSetting mMixSetting;

	// 080228 LUJ, ���� ��ȣ ������ ����
	BOOL IsMixProtectionItem( const ITEMBASE& ) const;

	// 080228 LUJ, ���� ��ȣ ������ ����
	typedef stdext::hash_set< DWORD >	MixProtectionItemSet;
	MixProtectionItemSet				mMixProtectionItemSet;
	// 080925 LUJ, ���� ���� ���� ����. Ű: ������ �ε���
	typedef stdext::hash_map< DWORD, MixSupportScript >	MixSupportScriptMap;
	MixSupportScriptMap									mMixSupportScriptMap;

	eLogitemmoney Dissolve(CPlayer&, const MSG_ITEM_DISSOLVE_SYN&, const CBackupSlot&);
	const DissolveScript* GetDissolveScript(DWORD itemIndex) const;
	void LoadDissolveScript();

	// Ű: ������ �ε���
	typedef std::map< DWORD, DissolveScript >	DissolveScriptMap;
	DissolveScriptMap							mDissolveScriptMap;


	class CRandom
	{
	public:
		CRandom() :
		mValue( rand() )
		{}

		DWORD GetDword()
		{
			return mValue = mValue * 1103515245 + rand();
		}

		float GetFloat()
		{
			return float( GetDword() ) / UINT_MAX;
		}

	private:
		DWORD mValue;
	}
	mRandom;

	// 080124 ����, ��� �ɼ� ����
public:
	void AddDropOption( const ITEMBASE&, ITEM_OPTION& );

private:
	void LoadCoolTimeScript();

	struct CoolTimeScript
	{
		struct Unit
		{
			DWORD mIndex;	// ��Ÿ�� �׷� �ε���
			DWORD mTime;	// ��Ÿ�� ���� �ð�: �и�������
		};

		// Ű: ������ �ε���
		typedef stdext::hash_map< DWORD, Unit >	ItemData;
		ItemData								mItemData;

		// ��: ������ �ε���
		typedef stdext::hash_set< DWORD >				ItemIndex;

		// Ű: �׷� �ε���
		typedef stdext::hash_map< DWORD, ItemIndex >	GroupData;
		GroupData										mGroupData;
	}
	mCoolTimeScript;

	// ��: ��Ÿ�� �׷�
	typedef stdext::hash_set< DWORD >					CoolTimeGroup;

	// Ű: �÷��̾� �ε���, ��: ��� ���� ��Ÿ�� ������ �׷� �ε���
	typedef stdext::hash_map< DWORD, CoolTimeGroup >	UsingCoolTimeGroup;
	UsingCoolTimeGroup									mUsingCoolTimeGroup;

public:
	DWORD GetCoolTimeGroupIndex( DWORD itemIndex ) const;
	void RemoveCoolTime(DWORD playerIndex);
	BOOL Move_ChangeMap(CPlayer*, MAPTYPE);
	BOOL UseItem_ResetStats(CPlayer*);
	BOOL UseItem_ResetSkill(CPlayer*);
	const ITEM_OPTION& GetOption( const ITEMBASE& ) const;
	void AddOption( const ITEM_OPTION& );
	void RemoveOption( const ITEMBASE& );
	void RemoveOption( DWORD itemDbIndex );

private:
	typedef stdext::hash_map< DWORD, ITEM_OPTION* >	OptionMap;
	OptionMap										mOptionMap;
	// 090923 ONS �޸�Ǯ ��ü
	CPool< ITEM_OPTION >	mOptionPool;


	// 080414 LUJ, �ܾ� ��ȯ ��ũ��Ʈ ó��
public:
	struct BodyChangeScript
	{
		DWORD		mGender;
		RaceType	mRace;
		PART_TYPE	mPart;

		// 080414 LUJ, �ؽ�Ʈ ��ȣ(InterfaceMsg.bin ����)
		DWORD		mText;
		DWORD		mValue;
	};

	typedef std::list< BodyChangeScript >					BodyChangeScriptList;
	typedef stdext::hash_map< DWORD, BodyChangeScriptList >	BodyChangeScriptListMap;

private:	
	BodyChangeScriptListMap	mBodyChangeScriptListMap;

	void LoadBodyChangeScript();


	//081027 NYJ - ���ͼ�ȯ������
public:
	struct stMonSummonInfo
	{
		DWORD MonsterIdx;		// ���� �ε���
		DWORD minRate;
		DWORD maxRate;

		stMonSummonInfo() {	MonsterIdx = minRate = maxRate = 0;	}
	};

	struct stMonSummonGroupInfo
	{
		WORD	GroupIdx;		// �׷��ε���
		DWORD	dwDieTime;
		bool	bNotForbidden;
		
		std::vector<stMonSummonInfo*>	vecBossMosSummonGroup;
		std::vector<stMonSummonInfo*>	vecMonSummonGroup;


		stMonSummonGroupInfo() { GroupIdx = 0; dwDieTime =0; bNotForbidden=false;}

		~stMonSummonGroupInfo()
		{
			std::vector<stMonSummonInfo*>::iterator iter;
			for(iter=vecBossMosSummonGroup.begin(); iter!=vecBossMosSummonGroup.end(); iter++)
			{
				SAFE_DELETE(*iter);
			}

			for(iter=vecMonSummonGroup.begin(); iter!=vecMonSummonGroup.end(); iter++)
			{
				SAFE_DELETE(*iter);
			}
		}

		DWORD MonSummon(DWORD rate)
		{
			if(false == vecBossMosSummonGroup.empty())
			{
				int nBossCnt = vecBossMosSummonGroup.size() - 1;

				if(0<=nBossCnt && rate <= vecBossMosSummonGroup[nBossCnt]->maxRate)
				{
					// Ȯ�����̺����� ��ġ�� �������� ��ȯ��.
					std::vector<stMonSummonInfo*>::iterator iter;
					for(iter=vecBossMosSummonGroup.begin(); iter!=vecBossMosSummonGroup.end(); iter++)
					{
						if((*iter)->minRate<=rate && rate<=(*iter)->maxRate)
							return (*iter)->MonsterIdx;
					}

					return 0;
				}
			}

			if(vecMonSummonGroup.empty())
			{
				return 0;
			}

			const int nRand = rand() % vecMonSummonGroup.size();

			return vecMonSummonGroup[nRand]->MonsterIdx;
		}
	};

	void LoadMonSummonItemInfo();

private:
	std::vector< stMonSummonGroupInfo* > m_vecMonSummonGroupInfo;
	std::set< MAPTYPE > m_vecMonSummonForbiddenMap;

	//081027 NYJ - NPC��ȯ������
public:
	struct stNpcSummonInfo
	{
		DWORD SummonIdx;	// ��ȯ�ε���
		DWORD NpcIdx;		// ���� �ε���
		DWORD LifeTime;

		stNpcSummonInfo() {	SummonIdx = NpcIdx = LifeTime = 0; }
	};

	void LoadNpcSummonItemInfo();

private:
	std::map< DWORD, stNpcSummonInfo* > m_mapNpcSummonInfo;
	std::set< MAPTYPE > m_vecNpcSummonForbiddenMap;

public:
	ITEM_OPTION LimiterItemOption(ITEM_OPTION option, const ITEM_INFO* info, CPlayer* player, BOOL isRare);
	ITEM_OPTION LoadLimiterItemOption(ITEM_OPTION option, const ITEM_INFO* info, CPlayer* player, BOOL isRare);
};
