#pragma once

#include "AddableInfo.h"
#include <hash_map>
#include <hash_set>
#include <list>
#include <map>
#include <set>
#include <string>

#pragma pack(push,1)
typedef unsigned long long DWORDEX;
typedef WORD MAPTYPE;
typedef DWORDEX EXPTYPE;
typedef WORD LEVELTYPE;
typedef DWORD DURTYPE;
typedef WORD POSTYPE;
typedef DWORD ITEMPARAM;
typedef int FAMETYPE;
typedef DWORD MARKNAMETYPE;
typedef DWORDEX MONEYTYPE;
#define MAXMONEY 9999999000000000 // 0xffffffff(4294967295)
#define MAX_JOB_GRADE 6

struct MSGROOT
{
	BYTE CheckSum;
#ifdef _CRYPTCHECK_ 
	MSGROOT():Code(0){}
	char Code;
#endif
	BYTE Category;
	BYTE Protocol;
};
struct MSGBASE :public MSGROOT
{
	DWORD dwObjectID;
};

struct COMPRESSEDPOS
{
	WORD wx;
	WORD wz;
	void Compress(VECTOR3* pPos)
	{
		wx = WORD(pPos->x);
		wz = WORD(pPos->z);
	}
	// 090316 LUJ, ����� �Լ��� ����
	void Decompress(VECTOR3* pRtPos) const
	{
		pRtPos->x = wx;
		pRtPos->z = wz;
		pRtPos->y = 0;
	}
};
struct COMPRESSEDTARGETPOS
{
	void Init()
	{
		PosNum = 0;
		memset( this, 0, sizeof(COMPRESSEDTARGETPOS) );
	}
	void AddTargetPos(VECTOR3 * pPos)
	{
		if( PosNum > MAX_CHARTARGETPOSBUF_SIZE )
		{
			ASSERT(0);
			return;
		}
		pos[PosNum++].Compress(pPos);
	}
	int GetSize()
	{
		return sizeof(COMPRESSEDTARGETPOS) - sizeof(COMPRESSEDPOS)*(MAX_CHARTARGETPOSBUF_SIZE-PosNum);
	}
	int GetSurplusSize()
	{
		return sizeof(COMPRESSEDPOS)*(MAX_CHARTARGETPOSBUF_SIZE-PosNum);
	}
	BYTE PosNum;
	COMPRESSEDPOS pos[MAX_CHARTARGETPOSBUF_SIZE];
};

struct SEND_MOVEINFO
{
	COMPRESSEDPOS CurPos;
	eMoveMode MoveMode;
	WORD KyungGongIdx;
	float AddedMoveSpeed;
};

struct BASEMOVE_INFO
{
	VECTOR3 CurPosition;
private:
	BYTE CurTargetPositionIdx;
	BYTE MaxTargetPositionIdx;
	VECTOR3 TargetPositions[MAX_CHARTARGETPOSBUF_SIZE];
public:
	bool bMoving;
	eMoveMode MoveMode;
	float AddedMoveSpeed;

	WORD KyungGongIdx;			// ���Idx 0: ������� �ƴϴ�  !0: ������� �����ȣ
	
	void SetFrom(SEND_MOVEINFO* pInfo)
	{
		bMoving = FALSE;
		pInfo->CurPos.Decompress(&CurPosition);
		MoveMode = pInfo->MoveMode;
		KyungGongIdx = pInfo->KyungGongIdx;
		AddedMoveSpeed = pInfo->AddedMoveSpeed;
	}
	BYTE GetCurTargetPosIdx() { return CurTargetPositionIdx; }
	BYTE GetMaxTargetPosIdx() { return MaxTargetPositionIdx; }
	BYTE & GetMaxTargetPosIdxRef() { return MaxTargetPositionIdx; }
	void SetCurTargetPosIdx(BYTE idx) { CurTargetPositionIdx=idx; }
	void SetMaxTargetPosIdx(BYTE idx) { MaxTargetPositionIdx=idx; }

	VECTOR3 * GetTargetPosition(BYTE idx) { return &TargetPositions[idx]; }
	VECTOR3 * GetTargetPositionArray() { return TargetPositions; }
	void InitTargetPosition()
	{
		CurTargetPositionIdx = 0;
		MaxTargetPositionIdx = 0;
		memset( TargetPositions, 0, sizeof( VECTOR3 ) * MAX_CHARTARGETPOSBUF_SIZE );
	}
	void SetTargetPosition( VECTOR3* pos )	{ memcpy( TargetPositions, pos, sizeof(VECTOR3)*MAX_CHARTARGETPOSBUF_SIZE );	}
	void SetTargetPosition( BYTE idx, VECTOR3 pos )	{ TargetPositions[idx] = pos;	}
};

struct MOVE_INFO : public BASEMOVE_INFO
{
	BOOL m_bLastMoving; // Ŭ���̾�Ʈ������ ����
	BOOL m_bEffectMoving;

	DWORD m_dwMoveCalculateTime;
	BOOL m_bMoveCalculateTime;

	// ���������� ����� �ð�.
	// ���������� ����� �ð��� �󸶵��� �ʾ����� �Ǵٽ� ������� �ʴ´�
	DWORD Move_LastCalcTime;

	DWORD Move_StartTime;
	VECTOR3 Move_StartPosition;
	VECTOR3 Move_Direction;

	float Move_EstimateMoveTime;

	BOOL bApplyGravity;				// �߷� ���뿩
	DWORD dwValidGravityTime;		// �߷� ���� ��ȿ �ð�
	float fGravityAcceleration;		// �߷� ����
	float fGravitySpeed;			// ���� �߷� ���� �ӵ�
};

struct STATE_INFO
{
	int State_Start_Motion;
	int State_Ing_Motion;
	int State_End_Motion;
	DWORD State_End_MotionTime;
	DWORD State_End_Time;
	DWORD State_Start_Time;
	BOOL bEndState;
	float MotionSpeedRate;
};


struct HERO_TOTALINFO
{
	DWORD	Str;							// Strength
	DWORD	Dex;						// ��ø
	DWORD	Vit;						// ü��
	DWORD	Int;
	DWORD	Wis;						// �ɸ�

	DWORD	Mana;						// ����
	DWORD	MaxMana;					// �ִ� ����
	EXPTYPE	ExpPoint;						// ����ġ
	LEVELTYPE	LevelUpPoint;				// ����������Ʈ		//??????
	MONEYTYPE	Money;						// ��

	DWORD	PartyID;						// ��Ƽ �ε��� 	
	DWORD	Playtime;						// Play Time
	DWORD	LastPKModeEndTime;				// ������ ����� off�ð�
	LEVELTYPE MaxLevel;						// �ö����� �ְ� ����
	char	MunpaCanEntryDate[11];			// can create guild/be member left time 
	char	FamilyCanEntryDate[11];

	DWORD	SkillPoint;
	// 080703 LUJ, �̵��� ��� ���� ����
	BOOL	bUsingTwoBlade;
};

struct MONSTER_TOTALINFO
{
	DWORD Life;
	WORD MonsterKind;
	// 091113 LUJ, �׷� ��Ī�� ������ �� �ֵ��� ����
	DWORD Group;
	MAPTYPE MapNum;
	// 090316 LUJ, ���͸� ������ ������Ʈ ��ȣ
	DWORD OwnedObjectIndex;
	TCHAR mScriptName[MAX_PATH];
};

struct NPC_TOTALINFO
{
	WORD NpcKind;
	WORD Group;
	MAPTYPE MapNum;
	WORD NpcUniqueIdx;
	WORD NpcJob;				// NPC_ROLE�� �ٲ�� ��! : taiyo

	// 080822 LYW --- CommonStruct : ��ȯ�� �ϴ� Npc�� ���, NpcRecallInfo.bin�� �ε����� ����Ѵ�.
	DWORD dwRecallNpcIdx ;

	DWORD dwSummonerUserIndex;	// ��ȯ�� ����� UserIndex
	char szSummonerName[MAX_NAME_LENGTH + 1];

	NPC_TOTALINFO()
	{
		ZeroMemory(this, sizeof(NPC_TOTALINFO));
	}
};

struct STATIC_NPCINFO
{
	MAPTYPE	MapNum;
	WORD	wNpcJob;
	WORD	wNpcUniqueIdx;
	VECTOR3	vPos;
	WORD	wDir;				// 090227 ShinJS --- ĳ������ ���� �߰�
	char NpcName[MAX_NAME_LENGTH+1];
};

struct QUESTNPCINFO
{
	DWORD	dwObjIdx;
	MAPTYPE	wMapNum;
	WORD	wNpcKind;
	char	sName[MAX_NAME_LENGTH+1];
	WORD	wNpcIdx;
	VECTOR3	vPos;
	float	fDir;	
};

struct PET_INFO
{
	DWORD	Index;
	char	Name[MAX_FILE_NAME + 1];
	char	ModelName[MAX_FILE_NAME + 1];
	DWORD	Image;
	DWORD	Sight;
	BYTE	Costume;
	// 090720 ONS ���� ������ų �ε���
	DWORD	StaticItemIndex;
	float	Scale;
};

struct PET_FRIENDLY_PENALTY
{
	BYTE	Friendly;
	float	Penalty;
};

struct PET_STATUS_INFO
{
	BYTE	Level;
	DWORD	STR;
	DWORD	DEX;
	DWORD	VIT;
	DWORD	INT;
	DWORD	WIS;
};

struct PET_HP_MP_INFO
{
	BYTE	Index;
	BYTE	HP;
	BYTE	MP;
};

struct PET_OBJECT_INFO
{
	DWORD	ID;
	DWORD	ItemDBIdx;
	DWORD	MasterIdx;
	char	MasterName[MAX_NAME_LENGTH+1];
	DWORD	Kind;
	BYTE	Level;
	BYTE	Grade;
	BYTE	SkillSlot;
	WORD	Exp;
	BYTE	Friendly;
	DWORD	HP;
	DWORD	MAXHP;
	DWORD	MP;
	DWORD	MAXMP;
	ePetAI	AI;
	ePetState State;
	ePetType Type;
	DWORD	FriendlyCheckTime;
};

struct ICONBASE
{
	DWORD	dwDBIdx;
	DWORD	wIconIdx;		// ������->itemIdx, ����->skillIdx
	POSTYPE Position;		// 
};


struct ITEM_OPTION
{
	// 080130 LUJ, �ɼ� �ε��� ��� ������ DB �ε����� ���
	DWORD	mItemDbIndex;
	// DWORD mIndex;
	DWORD mTransmogIdx;

	// ��ȭ�� ��� ��ġ
	struct Reinforce
	{
		DWORD	mStrength;
		DWORD	mDexterity;
		DWORD	mVitality;
		DWORD	mIntelligence;
		DWORD	mWisdom;
		DWORD	mLife;
		DWORD	mMana;
		DWORD	mManaRecovery;
		DWORD	mLifeRecovery;
		DWORD	mPhysicAttack;
		DWORD	mPhysicDefence;
		DWORD	mMagicAttack;
		DWORD	mMagicDefence;	
		DWORD	mCriticalRate;
		DWORD	mCriticalDamage;
		DWORD	mMoveSpeed;
		DWORD	mEvade;
		DWORD	mAccuracy;
		char	mMadeBy[ MAX_NAME_LENGTH + 1 ];
	}
	mReinforce;

	// ���սÿ� �����ϰ� ȹ��Ǵ� ��ġ
	struct Mix
	{
		DWORD	mStrength;
		DWORD	mIntelligence;
		DWORD	mDexterity;
		DWORD	mWisdom;
		DWORD	mVitality;
		char	mMadeBy[ MAX_NAME_LENGTH + 1 ];
	}
	mMix;

	// ��æƮ�� ��� ��ġ. �� ��ġ�� ��� �����ϴ�.
	struct Enchant
	{
		DWORD	mIndex;
		BYTE	mLevel;
		char	mMadeBy[ MAX_NAME_LENGTH + 1 ];
	}
	mEnchant;
	
	// 080130 LUJ, ��� �ɼ� �߰�
	struct Drop
	{
		enum Key
		{
			KeyNone,
			KeyPlusStrength			= 100,
			KeyPlusIntelligence,
			KeyPlusDexterity,
			KeyPlusWisdom,
			KeyPlusVitality,
			KeyPlusPhysicalAttack,
			KeyPlusPhysicalDefence,
			KeyPlusMagicalAttack,
			KeyPlusMagicalDefence,
			KeyPlusCriticalRate,
			KeyPlusCriticalDamage,
			KeyPlusAccuracy,
			KeyPlusEvade,
			KeyPlusMoveSpeed,
			KeyPlusLife,
			KeyPlusMana,
			KeyPlusLifeRecovery,
			KeyPlusManaRecovery,
			KeyPercentStrength		= 200,
			KeyPercentIntelligence,
			KeyPercentDexterity,
			KeyPercentWisdom,
			KeyPercentVitality,
			KeyPercentPhysicalAttack,
			KeyPercentPhysicalDefence,
			KeyPercentMagicalAttack,
			KeyPercentMagicalDefence,
			KeyPercentCriticalRate,
			KeyPercentCriticalDamage,
			KeyPercentAccuracy,
			KeyPercentEvade,
			KeyPercentMoveSpeed,
			KeyPercentLife,
			KeyPercentMana,
			KeyPercentLifeRecovery,
			KeyPercentManaRecovery,
		};

        struct Value
		{
			Key		mKey;
			float	mValue;
		}
		mValue[ 5 ];
	}
	mDrop;
};

struct PlayerStat
{
	struct Value
	{
		float mPlus;
		float mPercent;
		Value() { mPlus = 0; mPercent = 0; }
	};

	Value mStrength;
	Value mDexterity;
	Value mVitality;
	Value mIntelligence;
	Value mWisdom;
	Value mPhysicAttack;
	Value mPhysicDefense;
	Value mMagicAttack;
	Value mMagicDefense;
	Value mMana;
	Value mLife;
	Value mRecoveryLife;
	Value mRecoveryMana;
	Value mCriticalRate;
	Value mCriticalDamage;
	Value mMagicCriticalRate;
	Value mMagicCriticalDamage;
	Value mMoveSpeed;
	Value mAccuracy;
	Value mEvade;
	Value mAddDamage;
	Value mReduceDamage;
};

struct monster_stats
{
	WORD PhysicalAttackMin1;
	WORD PhysicalAttackMax1;
	WORD PhysicalAttackMin2;
	WORD PhysicalAttackMax2;
	WORD AttributeAttackMin1;
	WORD AttributeAttackMax1;
	WORD AttributeAttackMin2;
	WORD AttributeAttackMax2;
	WORD PhysicalDefense;
};

struct ComposeScript
{
	ComposeScript() :
	mSourceSize( 0 )
	{}

	// 080916 LUJ, 
	DWORD mKeyItemIndex;
	// 080916 LUJ, ��� ����
	DWORD mSourceSize;

	struct Result
	{
		DWORD	mItemIndex;
		BOOL	mIsHidden;
	};

	// 080916 LUJ, Ȯ�� �� ������ �����ϰ� �ִ�. Ű: Ȯ��
	typedef std::map< float, Result >	ResultMap;
	ResultMap							mResultMap;

	// 080916 LUJ, ��� �߿� �������� �ִ��� ��ȸ�ϴµ� ���δ�. ������ �������� ���⿡ ������ ����.
	//				�׷��Ƿ� ������ �������� �÷��̾ ���� ����� ������ �� ����.
	typedef std::set< DWORD >	ResultSet;
	ResultSet					mResultSet;

	// 080916 LUJ, ����� �ο��� �ɼ�
	// 090122 LUJ, ���� ���ʽ��� �Ӽ��� �������� ��
	struct Option
	{
		Option()
		{
			ZeroMemory( this, sizeof( *this ) );
		}

		float						mValue;
		ITEM_OPTION::Drop::Value	mHiddenBonus;
	};

	typedef std::map< ITEM_OPTION::Drop::Key, Option >	OptionMap;
	OptionMap											mOptionMap;
};

// ��Ʈ ������ �⺻ �ڷ� ����
// 080925 LUJ, ItemManager.h���� �̵���Ŵ
struct SetScript
{
	std::string mName;

	// ��Ʈ�� �����Ǵ� ������
	typedef stdext::hash_set< DWORD >	Item;
	Item								mItem;

	struct Element
	{
		PlayerStat	mStat;

		// Ű: ��ų �ε���, ��: ���� ����
		typedef std::map< DWORD, BYTE >	Skill;
		Skill							mSkill;
	};

	// Ű: �ɷ��� �����Ǵ� ��Ʈ ������ ���� ����
	typedef stdext::hash_map< int, Element >	Ability;
	Ability										mAbility;
	// 080916 LUJ, ��Ʈ �������� ������ ���� �̸��� ��ü�� �̸��� ���� �ִ� �ڷᱸ��
	//				���⿡ ���� ������, ��Ʈ�� �����ϴ� ������ �̸� ��� �ڷᱸ����
	//				������ �̸��� ��Ʈ ������ ������ ǥ�õ�
	typedef std::map< EWEARED_ITEM, std::string > SlotNameMap;
	SlotNameMap	mSlotNameMap;
	// 081231 LUJ, �������� �Ӽ��� �߰����ش�
	typedef stdext::hash_map< DWORD, Element >	ItemElementMap;
	ItemElementMap								mItemElementMap;
};

// 080925 LUJ, ItemManager.h���� �̵���Ŵ
struct DissolveScript
{
	typedef struct
	{
		DWORD mItemIndex;
		DWORD mQuantity;
	}
	Result;

	// ���ؽ� ������ �����ϴ� ������
	typedef std::list< Result > StaticResult;
	StaticResult				mStaticResult;

	// ���ؽ� Ȯ���� ���� �����ϴ� ������. lower_bound()�� ���Ƿ� hash_map�� ���� �ȵȴ�.
	typedef std::map< DWORD, Result >	DynamicResult;
	DynamicResult						mDynamicResult;

	// ���ذ����� �ּ� ����
	DWORD mLevel;
};

// 080925 LUJ, ItemManager.h���� �̵���Ŵ
struct ApplyOptionScript
{
	struct
	{
		int	mMin;
		int mMax;
	}
	mLevel;

	// Ű: Ȯ��, ��: �ο��� �ɼ� ����
	typedef std::map< float, DWORD >		OptionSizeMap;
	OptionSizeMap							mOptionSizeMap;
	// ����� ������ ����
	typedef stdext::hash_set< LONGLONG >	TargetTypeSet;
	TargetTypeSet							mTargetTypeSet;

	struct Option
	{
		ITEM_OPTION::Drop::Key	mKey;
		float					mBeginValue;
		float					mEndValue;
	};

	// Ȯ������ ���õ� �ɼ� ������ ��ġ ������ ��� �ִ�. Ű: ���� Ȯ��
	typedef std::map< float, Option >	OptionTypeMap;
	OptionTypeMap						mOptionTypeMap;
};

struct DropOptionScript
{
	// Ű: Ȯ��, ��: ��æƮ ��ġ
	typedef std::map< float, DWORD > EnchantLevelMap;
	EnchantLevelMap	mEnchantLevelMap;
	// Ű: Ȯ��, ��: �ο��� �ɼ� ����
	typedef std::map< float, DWORD > OptionSizeMap;
	OptionSizeMap mOptionSizeMap;

	typedef ApplyOptionScript::Option Option;
	// Ȯ������ ���õ� �ɼ� ������ ��ġ ������ ��� �ִ�
	// Ű: ���� Ȯ��
	typedef std::map< float, Option > OptionMap;
	// Ű: ������ ���� �������� ������ �ɼǿ� ���� Ȯ�� ���� ���� �ִ�
	typedef std::map< DWORD, OptionMap > LevelMap;
	LevelMap mLevelMap;
};

struct ReinforceScript
{
	enum eType
	{
		eTypeNone,
		eTypeStrength,
		eTypeDexterity,
		eTypeVitality,
		eTypeIntelligence,
		eTypeWisdom,
		eTypeLife,
		eTypeMana,
		eTypeLifeRecovery,
		eTypeManaRecovery,
		eTypePhysicAttack,
		eTypePhysicDefence,
		eTypeMagicAttack,
		eTypeMagicDefence,
		eTypeMoveSpeed,
		eTypeEvade,
		eTypeAccuracy,
		eTypeCriticalRate,
		eTypeCriticalDamage,
		eTypeMax
	};

	eType	mType;		// ��ȭ�� ��µǴ� �ɷ� ����
	DWORD	mMax;		// ��ȭ�� �ø� �� �ִ� �ִ� ��ġ
	float	mBias;		// ��ȭ�� ���� ��
	BOOL	mForRare;	// ���� ������ ��ȭ ���� ����

	// �ش� ���� ��ȭ�� �� �ִ� ������ ����(���� �������� ����)
	typedef stdext::hash_set< EWEARED_ITEM > Slot;
	Slot mSlot;
};

// 080929 LUJ, ��ȭ ���� ��ũ��Ʈ
struct ReinforceSupportScript
{
	ReinforceSupportScript() :
	mBias( 0 )
	{
		ZeroMemory( &mValueRange, sizeof( mValueRange ) );
		ZeroMemory( &mBonusRange, sizeof( mBonusRange ) );
	}

	struct Range
	{
		float mMin;
		float mMax;
	};
	// 080929 LUJ, ��ȭ ��ġ ����
	Range	mValueRange;
	// 080929 LUJ, ���ʽ� ��ġ ����
	Range	mBonusRange;
	// 080929 LUJ, ��� 1�� �� ����Ǵ� ��ġ
	float	mBias;
	// 080929 LUJ, ��� ������ ���. Ű: ������ ��ȣ
	typedef std::set< DWORD >	MaterialSet;
	MaterialSet					mMaterialSet;
};
// 080929 LUJ, ��ȭ ���� ��ũ��Ʈ �����̳�. Ű: ���� ������ �ε���
typedef stdext::hash_map< DWORD, ReinforceSupportScript > ReinforceSupportScriptMap;

// 070810 ����, ��æƮ ��ũ��Ʈ
// 080925 LUJ, ItemManager.h���� �̵���Ŵ
struct EnchantScript
{
	// ��æƮ�� �ο��ϴ� ������(��æƮ ��ũ�� ��)�� ������ �ε���
	DWORD mItemIndex;

	enum eType
	{
		eTypeNone,
		eTypeStrength,
		eTypeDexterity,
		eTypeVitality,
		eTypeIntelligence,
		eTypeWisdom,
		eTypeLife,
		eTypeMana,
		eTypeLifeRecovery,
		eTypeManaRecovery,
		eTypePhysicAttack,
		eTypePhysicDefence,
		eTypeMagicAttack,
		eTypeMagicDefence,
		eTypeMoveSpeed,
		eTypeEvade,
		eTypeAccuracy,
		eTypeCriticalRate,
		eTypeCriticalDamage,
		eTypeMax
	};

	// ��æ ������ ������ ���� ���� ���� ����
	struct
	{
		DWORD mMin;
		DWORD mMax;
	}
	mItemLevel;

	DWORD mEnchantMaxLevel;	// �ִ�� ��æƮ�� �� �ִ� �ܰ�

	typedef stdext::hash_set< eType >	Ability;
	Ability								mAbility;
	// �ش� ���� ��æƮ�� �� �ִ� ������ ����(���� �������� ����)
	typedef stdext::hash_set< EWEARED_ITEM > Slot;
	Slot mSlot;
};

struct EnchantProtection
{
	struct Data
	{
		struct Range
		{
			DWORD mMin;
			DWORD mMax;
		};
		Range mEnchantLevelRange;
		Range mItemLimitLevelRange;
		DWORD mMinimumEnchantLevel;
		int mAddedPercent;
		typedef std::map< float, int > BonusMap;
		BonusMap mPlusBonusMap;
		BonusMap mMinusBonusMap;
	};

	typedef stdext::hash_map< DWORD, Data >	ItemMap;
	ItemMap	mItemMap;
};

// 080925 LUJ, ItemManager.h���� �̵���Ŵ
struct MixSetting
{
	enum eAbility
	{
		eAbilityNone,
		eAbilityStrength,
		eAbilityDexterity,
		eAbilityIntelligence,
		eAbilityVitality,
		eAbilityWisdom,
	};

	// �� ���� �� �������� ����ִ�. �� 20%, ��ø�� 80%�� ����. �̶� 100������ �������� ���
	// map�� lower_bound() �޼ҵ带 �̿��Ͽ� ������ �ɷ�ġ�� ����.
	typedef std::map< float, eAbility >	Ability;
	typedef std::map< eStatusKind, Ability > Equippment;
	Equippment mEquippment;

	typedef struct Range
	{
		DWORD mMin;
		DWORD mMax;
	};

	// Ű: ���� ��. ��: ������ ��ġ ����
	typedef std::map< DWORD, Range >	RangeMap;
	RangeMap							mRangeMap;
};

// ���� ������ �⺻ �ڷᱸ��
// 080925 LUJ, ItemManager.h���� �̵�
struct ItemMixResult
{
	DWORD	mItemIndex;
	DWORD	mBaseMaterial;				// 091012 ShinJS --- �⺻ ��� ������ �ε��� �߰�
	WORD	mRequiredLevel;
	DWORDEX	mMoney;
	float	mSuccessRate;

	// Ű: ������ �ε���, ��: �ҿ� ����
	typedef stdext::hash_map< DWORD, DWORD >	Material;
	Material									mMaterial;
};

// 080925 LUJ, ItemManager.h���� �̵�
typedef std::list< ItemMixResult >	MixScript;

// 080925 LUJ, ���� ���� ��ũ��Ʈ
struct MixSupportScript
{
	MixSupportScript() :
	mBonusSucessRate( 0 )
	{}

	// 080925 LUJ, ���� Ȯ�� ���ʽ�
	float				mBonusSucessRate;
	// 080925 LUJ, �ɼ��� �����ϴ� Ȯ�� ����. ���� ���� �������� ����� ���� �� Ȯ�� ������ ����Ѵ�
	typedef	MixSetting::Ability	Ability;
	Ability						mAbiltyMap;
};

// 081203 LUJ, ���� ��ų ����Ʈ�� ����� Ȯ���� �� �ִ� ��ũ��Ʈ
struct SkillScript
{
	enum Type
	{
		TypeNone,
		TypeStrength,
		TypeDexterity,
		TypeVitality,
		TypeIntelligence,
		TypeWisdom,
		TypeLife,
		TypeMana,
		TypeLifeRecovery,
		TypeManaRecovery,
		TypePhysicAttack,
		TypePhysicDefence,
		TypeMagicAttack,
		TypeMagicDefence,
		TypeMoveSpeed,
		TypeEvade,
		TypeAccuracy,
		TypeCriticalRate,
		TypeCriticalDamage,
		TypeMagicCriticalRate,
		TypeMagicCriticalDamage,
	};

	struct Value
	{
		Type	mType;
		float	mPlus;
		float	mPercent;

		Value()
		{
			ZeroMemory( this, sizeof( *this ) );
		}
	};
	typedef std::list< Value >	ValueList;
	// 081203 LUJ, ���ȿ� ����� ���ʽ��� ����ִ�
	ValueList					mStatusBonusList;
	// 081203 LUJ, ���� �ð��� ����� ���ʽ��� ����ִ�
	ValueList					mDurationBonusList;

	struct Buff
	{
		eStatusKind	mKind;
		float		mValue;

		Buff()
		{
			ZeroMemory( this, sizeof( *this ) );
		}
	};
	// 081203 LUJ, ������ �Ӽ� �� ���� ����ִ�
	typedef std::list< Buff >	BuffList;
	BuffList					mBuffList;
	// 090226 LUJ, ��ų �˻� ����
	BOOL mIsNeedCheck;

	// 090630 ONS ��ų �׷� �Ӽ� �߰�
	DWORD mSkillGroupNum;

	SkillScript() :
	mIsNeedCheck( TRUE ),
	mSkillGroupNum( 0 )
	{}
};
// 081203 LUJ, ��ų ��ũ��Ʈ�� �����ϴ� �ڷ� ����. Ű: ��ų ��ȣ
typedef stdext::hash_map< DWORD, const SkillScript* > SkillScriptMap;

// 090316 LUJ, Ż�� ��ũ��Ʈ
struct VehicleScript
{
	// 090316 LUJ, MonsterList.bin���� ������ �о�´�
	WORD mMonsterKind;
	// 090316 LUJ, �¼� ����
	struct Seat
	{
		enum { MaxSize = 20 };
		BOOL mIsEnableControl;
		char mAttachedName[ MAX_NAME_LENGTH ];
	}
	mSeat[ Seat::MaxSize ];
	// 090316 LUJ, ��Ʈ ����
	DWORD mSeatSize;
	// 090316 LUJ, Ż�Ϳ����� �÷��̾� ����
	struct Motion
	{
		DWORD mMount;				// ž�½��� �÷��̾� Motion Index

		// 091109 ShinJS --- Beff ����
		enum
		{
			FixObjectPlay,
			FixObjectStop,
			MoveObjectIdle,
			MoveObjectWalk,
			MoveObjectRun,
		};

		typedef std::pair<DWORD, std::string> MotionInfo;
		std::multimap< DWORD, std::string > mBeffFileList;
	}
	mMotion;
	// 090316 LUJ, ž�� ����
	struct Condition
	{
		// 090316 LUJ, ž�� �� �ҿ�Ǵ� �ݾ�
		MONEYTYPE mTollMoney;
		LEVELTYPE mPlayerMinimumLevel;
		LEVELTYPE mPlayerMaximumLevel;
		// 090316 LUJ, ��ȯ �� �÷��̾�� Ż���� �־��� �� �ִ� �ִ� �Ÿ�
		float mSummonRange;
		enum { BuffMaxSize = 10 };
		DWORD mBuffSkillIndex[ BuffMaxSize ];

		Condition() :
		mTollMoney(0),
		mPlayerMinimumLevel(0),
		mPlayerMaximumLevel(SHRT_MAX),
		mSummonRange(0)
		{
			ZeroMemory(
				mBuffSkillIndex,
				sizeof(mBuffSkillIndex));
		}
	}
	mCondition;
	// 090316 LUJ, �ʱ�ȭ�� ������ ������
	VehicleScript()
	{
		mMonsterKind = 0;
		mSeatSize = 0;
		mMotion.mMount = 0;
		ZeroMemory( mSeat, sizeof( Seat ) * Seat::MaxSize );
	}
};

// 081119 NYJ - �丮
struct stIngredient
{
	DWORD dwItemIdx;
	WORD wQuantity;

	stIngredient()
	{
		dwItemIdx = wQuantity = 0;
	}
};

struct stRecipeInfo
{
	DWORD dwRecipeIdx;
	DWORD dwExpertPointMin;
	DWORD dwExpertPointMax;
	DWORD dwRemainTime;
	DWORD dwFoodItemIdx;
	stIngredient Ingredients[MAX_INGREDIENT_LIST];

	stRecipeInfo()
	{
		dwRecipeIdx = dwExpertPointMin = dwExpertPointMax = dwRemainTime = dwFoodItemIdx = 0;
		memset(Ingredients, 0, sizeof(stIngredient)*MAX_INGREDIENT_LIST); 
	}
};

struct stRecipeLv4Info	// ���η���������
{
	DWORD dwRecipeIdx;
	DWORD dwRemainTime;

	stRecipeLv4Info()
	{
		dwRecipeIdx = dwRemainTime = 0;
	}
};


struct stMonsterEventRewardItemInfo
{
	WORD wMonsterType;
	DWORD dwItemIndex;

	stMonsterEventRewardItemInfo()
	{
		wMonsterType = 0;
		dwItemIndex = 0;
	}
};


// 090316 NYJ - �Ͽ�¡
struct stGeneralFunitureInfo
{
	DWORD		dwItemIndex;
	VECTOR3		vWorldPos;
	float		fAngle;

	stGeneralFunitureInfo()
	{
		memset(this, 0, sizeof(stGeneralFunitureInfo));
	}
};

//����� �ܰ��Ͽ콺 Npc���� 
struct stDynamicHouseNpcInfo
{
	DWORD		dwRankTypeIndex;		//��ŷ(1~)*100 + �ܰ�Ÿ��(1~99)   
	DWORD		dwNpcKind;				//Npc ī�ε�
	float		fWorldPosX;		
	float		fWorldPosZ;		
	float		fAnlge;
	float		fDistance;				//�����û ��ȿ�Ÿ�

	stDynamicHouseNpcInfo()
	{
		memset(this, 0, sizeof(stDynamicHouseNpcInfo) );
	}
};

struct stDynamicHouseNpcMapInfo
{
	DWORD MapIndex;
	CYHHashTable<stDynamicHouseNpcInfo>	pDynamicHouseNpcList;

	stDynamicHouseNpcMapInfo()
	{
		Clear();
		pDynamicHouseNpcList.Initialize(100);
	}

	void Clear()
	{
		MapIndex = 0;
		stDynamicHouseNpcInfo* pNpcInfo = NULL;
		pDynamicHouseNpcList.SetPositionHead();	
		while((pNpcInfo = pDynamicHouseNpcList.GetData() )!= NULL)
		{
			SAFE_DELETE(pNpcInfo);
		}
		pDynamicHouseNpcList.RemoveAll();
	}

	~stDynamicHouseNpcMapInfo()
	{
		Clear();
	}
};

struct stHousingSettingInfo
{
	float fDecoPoint_Weight;	//�ٹ̱� ���� ����ġ
	float fRecommend_Weight;	//��õ�� ���� ����ġ
	float fVisitor_Weight;		//�湮�� ���� ����ġ 

	DWORD dwGeneralFunitureNumArr[MAX_HOUSE_EXTEND_LEVEL];
	stGeneralFunitureInfo* pGeneralFunitureList[MAX_HOUSE_EXTEND_LEVEL];

	DWORD	dwStarPoint_Nomal;
	DWORD	dwStarPoint_Ranker;

	DWORD	dwRankingDay;

	CYHHashTable<stDynamicHouseNpcMapInfo>	m_DynamicHouseNpcMapInfoList;	//�������� �Ͽ콺Npc ������ ����Ʈ 

	stHousingSettingInfo()
	{
		memset(pGeneralFunitureList , 0 , sizeof(stGeneralFunitureInfo*) * MAX_HOUSE_EXTEND_LEVEL );
		m_DynamicHouseNpcMapInfoList.Initialize( 100 );
		Clear();
	}

	void Clear()
	{
		for(int i=0; i<MAX_HOUSE_EXTEND_LEVEL ;i++)
			SAFE_DELETE_ARRAY(pGeneralFunitureList[i]);

		dwStarPoint_Nomal = dwStarPoint_Ranker = dwRankingDay = 0;
		memset(dwGeneralFunitureNumArr , 0 , sizeof(DWORD) * MAX_HOUSE_EXTEND_LEVEL );

		stDynamicHouseNpcMapInfo* pMapInfo;
		m_DynamicHouseNpcMapInfoList.SetPositionHead();	
		while((pMapInfo = m_DynamicHouseNpcMapInfoList.GetData())!= NULL)
		{
			pMapInfo->Clear();
			SAFE_DELETE(pMapInfo);
		}
		m_DynamicHouseNpcMapInfoList.RemoveAll();
	}

	~stHousingSettingInfo()
	{ 
		Clear();
	}
};

struct stFurnitureFileInfo
{
	DWORD dwFurnitureIndex;						//�����ε���
	char szNameCHX[MAX_HOUSING_NAME_LENGTH];	//CHX [�ִϸ��̼� ����]
	char szNameCHR[MAX_HOUSING_NAME_LENGTH];	//CHR [Material ����]
	char szNameMOD[MAX_HOUSING_NAME_LENGTH];	//MOD [�𵨸� ����]

	stFurnitureFileInfo()
	{
		dwFurnitureIndex = 0;
		strcpy(szNameCHX, "");
		strcpy(szNameCHR, "");
		strcpy(szNameMOD, "");
	}
};

struct stFurniture	// DB,����,Ŭ�� �ְ��޴� ��������ü
{
	DWORD dwOwnerUserIndex;		// �������� UserIndex
	DWORD dwFurnitureIndex;		// �����ε���
	DWORD dwLinkItemIndex;		// �������ε���
	DWORD dwObjectIndex;		// �����ε���:����->Ŭ�����۽� ����Index �Ҵ�
	int nMaterialIndex;			// ���������� MaterialIndex
	VECTOR3 vPosition;			// ��ġ
	float fAngle;				// ����
	BYTE wCategory;				// ����â�� - ī�װ���
	BYTE wSlot;					// ����â�� - ����
	BYTE wState;				// ����â�� - ����(eHOUSEFURNITURE_STATE_UNKEEP, eHOUSEFURNITURE_STATE_KEEP, eHOUSEFURNITURE_STATE_INSTALL, eHOUSEFURNITURE_STATE_UNINSTALL)
	BOOL bNotDelete;			// �⺻��ġǰ�� (�����Ұ�)
	DWORD dwRemainTime;			// �����ð�

	DWORD dwRidingPlayer[MAX_HOUSE_ATTATCH_SLOT];		// ž�½��� (ž���� CharIndex����)

	void Clear()
	{
		memset(this, 0, sizeof(stFurniture));
		nMaterialIndex = -1;
	}

	stFurniture()
	{
		Clear();
	}
};

struct stFunitureInfo	// ����,Ŭ�� ��ũ��Ʈ���� �о���� ����
{
	DWORD dwFurnitureIndex;		// �����ε���
	char szName[MAX_HOUSING_NAME_LENGTH];
	DWORD dwItemIndex;			// ������ ItemIndex
	DWORD dwActionIndex;		// �׼�Index
	DWORD dwDecoPoint;			// �ٹ̱�����Ʈ
	DWORD dwRemainTime;			// ���Ⱓ
	BOOL bStackable;			// �����ױ⿩��
	BOOL bRotatable;			// ȸ������
	BOOL bTileDisable;			// �̵��Ұ�
	BOOL bMeshTest;				// Mesh�浹 Test
	BYTE byMaxMaterialNum;		// �ִ� Matrial ����
	BYTE byModelFileFlag;		// ���������� 0: MOD, 1: CHX, 2:CHR
	char szModelFileName[256];	// ������ �̸�
	BYTE byAttachBoneNum;		// ���� AttachBone����
	BOOL bHasAlpha;				// ��ü���� �������� (���ļ��ý� ������ ���� ���ζ����� Ŭ���̾�Ʈ������ ����)

	stFunitureInfo()
	{
		memset(this, 0, sizeof(stFunitureInfo));
	}
};

struct stHouseBonusInfo	// �ٹ̱�ȿ��
{
	DWORD dwBonusIndex;			// ���ʽ��ε��� ( 10000~19999 �⺻���ʽ� , 20000 ~ 29999 ��Ŀ�뺸�ʽ�) 
	DWORD dwDecoPoint;			// �ٹ̱� ����Ʈ 				
	char szName[MAX_HOUSING_NAME_LENGTH];
	DWORD dwTooltipIndex;		//���ʽ� �����ε���
	BYTE byKind;				//0 ���뺸�ʽ� , 1��Ŀ�� ���ʽ� 	
	DWORD dwBonusType;			//0 ����Ÿ�� ,1 ���̵�  
	DWORD dwBonusTypeValue;		//��ų�ε��� , �ʹ�ȣ  
	BYTE byTargetType;			//0 ������	 , ���ȿ��ִ� ���
	DWORD dwUsePoint;			//�������Ʈ 

	stHouseBonusInfo()
	{
		memset(this, 0, sizeof(stHouseBonusInfo));
	}
};

struct stHouseInfo
{
	DWORD dwOwnerUserIndex;
	char szHouseName[MAX_HOUSING_NAME_LENGTH + 1];
	BYTE ExteriorKind;
	BYTE ExtendLevel;
	DWORD dwTotalVisitCount;
	DWORD dwDailyVisitCount;
	DWORD dwHousePoint;			// DecoPoint + VotePoint + VisitPoint
	DWORD dwDecoUsePoint;

	void Clear()
	{
		memset(this, 0, sizeof(stHouseInfo));
	}

	stHouseInfo()
	{
		Clear();
	}
};

// ����,Ŭ�� ��ũ��Ʈ���� �о���� �׼Ǳ׷�����
struct stHouseActionGroupInfo
{
	DWORD dwGroupIndex;		//�׷��ε��� 
	BYTE  byActionNum;		//�����׼Ǽ� 
	DWORD dwActionIndexList[MAX_HOUSE_ACTION_SLOT];

	stHouseActionGroupInfo()
	{
		memset(this, 0, sizeof(stHouseActionGroupInfo));
	}
};

// ����,Ŭ�� ��ũ��Ʈ���� �о���� �׼�����
struct stHouseActionInfo
{
	DWORD	dwActionIndex;						//�׼��ε���
	char	szName[MAX_HOUSING_NAME_LENGTH];	//�׼� �̸�
	DWORD	dwPlayer_MotionIndex;				//�÷��̾� ����ε���
	DWORD	dwPlayer_EffectIndex;				//�÷��̾� ����Ʈ�ε���

	DWORD	dwFurniture_MotionIndex;			//��������ε���
	DWORD	dwFurniture_EffectIndex;			//��������Ʈ�ε���
	BOOL	bUseAttach;							//����ġ���� 							

	DWORD	dwActionType;						//�׼�Ÿ��
	DWORD	dwActionValue;						//�׼�Ÿ�Կ� ���� �߰� ��

	stHouseActionInfo()
	{
		memset(this, 0, sizeof(stHouseActionInfo));
	}
};

struct stHouseRankNPC
{
	DWORD dwHouseUserIndex;
	DWORD dwNpcIndex;

	stHouseRankNPC()
	{
		memset(this, 0, sizeof(stHouseRankNPC));
	}
};

struct stHouseRank
{
	DWORD	dwRank_1_UserIndex;
	char	szRank_1_HouseName[MAX_HOUSING_NAME_LENGTH + 1];
	BYTE	byRank_1_UserExterioKind;
	DWORD	dwRank_1_HousePoint;
	DWORD	dwRank_2_UserIndex;
	char	szRank_2_HouseName[MAX_HOUSING_NAME_LENGTH + 1];
	BYTE	byRank_2_UserExterioKind;
	DWORD	dwRank_2_HousePoint;
	DWORD	dwRank_3_UserIndex;
	char	szRank_3_HouseName[MAX_HOUSING_NAME_LENGTH + 1];
	BYTE	byRank_3_UserExterioKind;
	DWORD	dwRank_3_HousePoint;

	WORD	wCalcYear;
	WORD	wCalcMonth;
	WORD	wCalcDay;

	stHouseRank()
	{
		memset(this, 0, sizeof(stHouseRank));
	}
};

struct stHouse
{
	// DB�����ʿ�
	stHouseInfo	HouseInfo;

	//�������� ���� �� 
	DWORD m_dwCategoryNum[MAX_HOUSING_CATEGORY_NUM];
	DWORD m_dwFurnitureList[MAX_HOUSING_CATEGORY_NUM][MAX_HOUSING_SLOT_NUM];

	// ��Ÿ�ӽ� �ʿ�
	DWORD dwOwnerCharIndex;		// �湮���� ������ ĳ����Index;
	DWORD dwDecoPoint;			// ���缳ġ�� ������Ʈ�� �ٹ̱�����
	DWORD dwCurDoorIndex;		// ���缳ġ�� ���� ObjectIndex
	DWORD dwChannelID;
	DWORD dwJoinPlayerNum;
	CYHHashTable<stFurniture>	pFurnitureList;
	CYHHashTable<stFurniture>	pNotDeleteFurnitureList;	// pFurnitureList�� ��ϵǴ� ��ü�� �⺻��ġ���� ���. �޸��ߺ���������(pFurnitureList�� �����ϸ� ��)
	VECTOR3 vStartPos;

	void Clear()
	{
		HouseInfo.Clear();

		ZeroMemory(m_dwCategoryNum, sizeof(m_dwCategoryNum));
		ZeroMemory(m_dwFurnitureList, sizeof(m_dwFurnitureList));
		dwOwnerCharIndex = dwDecoPoint = dwChannelID = dwJoinPlayerNum = 0;
		dwCurDoorIndex = 0;

		pFurnitureList.RemoveAll();
		pNotDeleteFurnitureList.RemoveAll();
	}

	stHouse()
	{
		Clear();
	}

	~stHouse()
	{
		stFurniture* pFurniture;
		pFurnitureList.SetPositionHead();
		while((pFurniture = pFurnitureList.GetData())!= NULL)
		{
			SAFE_DELETE(pFurniture);
		}
	}
};

struct stDungeonKey
{
	DWORD dwIndex;
	WORD  wMapNum;
	eDIFFICULTY difficulty;
	DWORD dwEntranceNpcJobIndex;
	DWORD dwPosX;
	DWORD dwPosZ;
	DWORD dwTooltipIndex;

	WORD wMinLevel;		// �ּҷ���
	WORD wMaxLevel;		// �ִ뷹��
	WORD wMaxCount;		// �ִ�Ƚ��
	WORD wDelayMin;		// ��������̽ð�(��)

	stDungeonKey()
	{
		ZeroMemory(this, sizeof(stDungeonKey));
	}
};

struct stWarpInfo
{
	DWORD dwIndex;
	WORD  wMapNum;
	WORD  wNpcJob;
	char  name[MAX_MONSTER_NAME_LENGTH + 1];
	WORD  wRadius;
	WORD  wCurPosX;
	WORD  wCurPosZ;
	WORD  wDstPosX;
	WORD  wDstPosZ;
	BOOL  bActive;

	stWarpInfo()
	{
		Clear();
	}

	void Clear()
	{
		ZeroMemory(this, sizeof(stWarpInfo));
	}
};

struct stWarpState
{
	DWORD dwIndex;
	DWORD dwObjectIndex;
	BOOL  bActive;

	stWarpState()
	{
		Clear();
	}

	void Clear()
	{
		ZeroMemory(this, sizeof(stWarpState));
	}
};

struct stSwitchNpcInfo
{
	WORD  wIndex;
	WORD  wMapNum;
	WORD  wNpcJob;
	char  name[MAX_NAME_LENGTH + 1];
	WORD  wRadius;
	WORD  wPosX;
	WORD  wPosZ;
	BOOL  bActive;

	stSwitchNpcInfo()
	{
		Clear();
	}

	void Clear()
	{
		ZeroMemory(this, sizeof(stSwitchNpcInfo));
	}
};

struct stSwitchNpcState
{
	DWORD dwObjectIndex;
	WORD  wIndex;
	WORD  wEffectIndex;
	BOOL  bActive;

	stSwitchNpcState()
	{
		Clear();
	}

	void Clear()
	{
		ZeroMemory(this, sizeof(stSwitchNpcState));
	}
};

struct stBossMonsterState
{
	DWORD	dwObjectIndex;
	WORD	wIndex;
	WORD	wReturnPosX;
	WORD	wReturnPosZ;

	stBossMonsterState()
	{
		Clear();
	}

	void Clear()
	{
		ZeroMemory(this, sizeof(stBossMonsterState));
	}
};

#define MAX_DUNGEON_SWITCH		40
#define MAX_DUNGEON_WARP		40

struct stDungeon
{
	DWORD dwIndex;
	DWORD dwPartyIndex;
	DWORD dwChannelID;
	DWORD dwStartTime;
	DWORD dwPoint;
	DWORD dwJoinPlayerNum;
	eDIFFICULTY difficulty;

	int			mSwitch[MAX_DUNGEON_SWITCH];
	WORD		m_WarpEmptyPos;
	WORD		m_SwitchNpcEmptyPos;

	DWORD		m_TimerAlias;
	stWarpState			m_WarpState[MAX_DUNGEON_WARP];
	stSwitchNpcState	m_SwitchNpcState[MAX_DUNGEON_SWITCH];

	stBossMonsterState	m_CurBossMonsterState;

	stDungeon()
	{
		Clear();
	}

	~stDungeon()
	{
	}

	void Clear()
	{
		ZeroMemory(this, sizeof(stDungeon));
	}

	stWarpState* GetWarp(DWORD dwIndex)
	{
		for(WORD i=0; i<MAX_DUNGEON_WARP; i++)
		{
			if(m_WarpState[i].dwIndex == dwIndex)
				return &m_WarpState[i];
		}

		return NULL;
	}

	stSwitchNpcState* GetSwitchNpc(WORD wIndex)
	{
		for(WORD i=0; i<MAX_DUNGEON_SWITCH; i++)
		{
			if(m_SwitchNpcState[i].wIndex == wIndex)
				return &m_SwitchNpcState[i];
		}
		return NULL;
	}
};

struct SKILL_BASE
{
	DWORD dwDBIdx;
	DWORD wSkillIdx;
	LEVELTYPE Level;
	LEVELTYPE mLearnedLevel;
	DWORDEX wSkillPoint;
	MONEYTYPE wSkillMoney;
};

enum ITEM_TIMEKIND
{
	eKIND_REALTIME = 0,		// ���� �ð� �������� ��� �ð��� ó��.
	eKIND_PLAYTIME ,		// ������ �÷��� ���� �� �ð��� ó��.
} ;

//////////////////////////////////////////////////////////////////////////

// 071123 KTH --- CommonStruct : �������� ���� ���� enum �߰�.
enum ITEM_SEAL_TYPE
{
	eITEM_TYPE_SEAL_NORMAL = 0,
	eITEM_TYPE_SEAL,
	eITEM_TYPE_UNSEAL,
	eITEM_TYPE_GET_UNSEAL,
};

// 100223 pdy ������ ���� �÷��� Ȯ�� �۾� ( ���� â���� �������� �߰� )
enum ITEM_DEPOSIT_TYPE
{
	eITEM_TYPE_DEPOSIT_NONE = 0,				// �����Ұ���
	eITEM_TYPE_DEPOSIT_ALL,						// ��� ��������
	eITEM_TYPE_DEPOSIT_ONLY_STORAGE,			// ����â���� ����
};

struct ITEMBASE : public ICONBASE
{
	DURTYPE Durability;
	POSTYPE	QuickPosition;
	ITEMPARAM ItemParam;
	ITEM_SEAL_TYPE nSealed;
	int nRemainSecond;
	DWORD LastCheckTime;
};

struct SLOTINFO
{
	BOOL	bLock;
	WORD	wState;
};

struct ITEM_TOTALINFO
{
	ITEMBASE Inventory[SLOT_MAX_INVENTORY_NUM];
	ITEMBASE WearedItem[eWearedItem_Max];
};

struct ScriptCheckValue
{
	ScriptCheckValue()
	{
		ZeroMemory( this, sizeof( *this ) );
	}

	LONGLONG mValue;
};

struct PARTY_MEMBER
{
	DWORD dwMemberID;
	BOOL bLogged;
	BYTE LifePercent;
	BYTE ManaPercent;
	char Name[MAX_NAME_LENGTH+1];
	LEVELTYPE Level;
	VECTOR3 mPosition;
	// 091127 LUJ, ������ �� ��ȣ
	MAPTYPE mMapType;
	BYTE mJobGrade;
	BYTE mJob[MAX_JOB_GRADE];
	BYTE mRace;
};

struct PARTY_INFO : public MSGBASE 
{
	DWORD PartyDBIdx;
	BYTE Option;
	BYTE SubOption;
	PARTY_MEMBER Member[MAX_PARTY_LISTNUM];
};

struct SEND_PARTY_MEMBER_INFO : public MSGBASE 
{
	PARTY_MEMBER MemberInfo;
	DWORD PartyID;
	BOOL mIsMaster;
};

struct CHARACTERMAKEINFO : public MSGBASE
{
	char Name[MAX_NAME_LENGTH+1];			// ĳ���� �̸�
	DWORD	UserID;
	BYTE	RaceType;
	BYTE	SexType;
	BYTE	HairType;
	BYTE	FaceType;
	BYTE	JobType;
	BYTE    StandingArrayNum;				// pjs �ɸ����� ��ġ ���� [5/22/2003]
	BOOL	bDuplCheck;
};

struct MSG_INT : public MSGBASE
{
	int nData;
};

struct MSG_INT2 : public MSGBASE
{
	int nData1;
	int nData2;
};

struct MSG_DWORD : public MSGBASE
{
	DWORD dwData;
};

// 080602 LYW --- CommonStruct : DWORDEX Ÿ���� �޽��� �߰�.
struct MSG_DWORDEX : public MSGBASE
{
	DWORDEX dweData ;
} ;

struct MSG_DWORDEX2 : public MSGBASE
{
	DWORDEX dweData1 ;
	DWORDEX dweData2 ;
} ;

struct MSG_DWORDEX3 : public MSGBASE
{
	DWORDEX dweData1 ;
	DWORDEX dweData2 ;
	DWORDEX dweData3 ;
} ;

struct MSG_DWORDEX4 : public MSGBASE
{
	DWORDEX dweData1 ;
	DWORDEX dweData2 ;
	DWORDEX dweData3 ;
	DWORDEX dweData4 ;
} ;

struct MSG_DWORD2 : public MSGBASE
{
	DWORD dwData1;
	DWORD dwData2;
};

struct MSG_DWORDBYTE : public MSGBASE
{
	DWORD dwData;
	BYTE bData;
};

struct MSG_DWORD2BYTE : public MSGBASE
{
	DWORD dwData1;
	DWORD dwData2;
	BYTE bData;
};

struct MSG_DWORD2INT : public MSGBASE
{
	DWORD dwData1;
	DWORD dwData2;
	int nData;
};

struct MSG_DWORDBYTE2 : public MSGBASE
{
	DWORD dwData;
	BYTE bData1;
	BYTE bData2;
};

struct MSG_DWORD3 : public MSGBASE
{
	DWORD dwData1;
	DWORD dwData2;
	DWORD dwData3;
};

struct MSG_DWORD3_GUILD : public MSGBASE
{
	DWORD dwData1;
	DWORD dwData2;
	DWORDEX dwData3;
};

struct MSG_DWORD3_VEHICLE : public MSGBASE
{
	DWORD dwData1;
	DWORDEX dwData2;
	DWORD dwData3;
};

struct MSG_DWORD4 : public MSGBASE
{
	DWORD dwData1;
	DWORD dwData2;
	DWORD dwData3;
	DWORD dwData4;
};

struct MSG_DWORD4_VEHICLE : public MSGBASE
{
	DWORD dwData1;
	DWORD dwData2;
	DWORDEX dwData3;
	DWORD dwData4;
};

struct MSG_DWORD5 : public MSGBASE
{
	DWORD dwData1;
	DWORD dwData2;
	DWORD dwData3;
	DWORD dwData4;
	DWORD dwData5;
};

struct MSG_DWORD6 : public MSGBASE
{
	DWORD dwData1;
	DWORD dwData2;
	DWORD dwData3;
	DWORD dwData4;
	DWORD dwData5;
	DWORD dwData6;
};

struct MSG_OPT : public MSGBASE
{
	DWORD	dwData1;
	DWORD	dwData2;
	DWORDEX dweData3;
	DWORD	dwData4;
	DWORD	dwData5;
	DWORD	dwData6;
	DWORD	dwData7;
	DWORD	dwData8;
};

struct MSG_DWORD3BYTE2 : public MSGBASE
{
	DWORD	dwData1;
	DWORD	dwData2;
	DWORD	dwData3;
	BYTE	bData1;
	BYTE	bData2;
};

struct MSG_WORD : public MSGBASE
{
	WORD wData;
};

struct MSG_WORD2 : public MSGBASE
{
	WORD wData1;
	WORD wData2;
};
struct MSG_WORD3 : public MSGBASE
{
	WORD wData1;
	WORD wData2;
	WORD wData3;
};
struct MSG_WORD4 : public MSGBASE
{
	WORD wData1;
	WORD wData2;
	WORD wData3;
	WORD wData4;
};

struct MSG_DWORD_WORD : public MSGBASE
{
	DWORD dwData;
	WORD wData;
};

struct MSG_DWORD_NAME : public MSGBASE
{
	DWORD dwData;
	char Name[MAX_NAME_LENGTH+1];
};

struct MSG_DWORD2_NAME : public MSGBASE
{
	DWORD dwData1;
	DWORD dwData2;
	char Name[MAX_NAME_LENGTH+1];
};

// 091204 ONS ���� ���ݳ��� ���������� ���� �޼����߰�.
struct MSG_DWORD3_NAME : public MSGBASE
{
	DWORD dwData1;
	DWORD dwData2;
	DWORD dwData3;
	char Name[MAX_NAME_LENGTH+1];
};

struct MSG_BYTE : public MSGBASE
{
	BYTE bData;
};

// 070416 LYW --- CommonStruct : Add struct for two byte.
struct MSG_BYTE2 : public MSGBASE
{
	BYTE bData1 ;
	BYTE bData2 ;
} ;
// 081011 LYW --- CommonStruct : Add struct for three byte.
struct MSG_BYTE3 : public MSG_BYTE2
{
	BYTE bData3 ;
} ;

struct MSG_BYTE4 : public MSG_BYTE3
{
	BYTE bData4 ;
} ;

struct MSG_BYTE5 : public MSG_BYTE4
{
	BYTE bData5 ;
} ;

struct MSG_GUILDNOTICE : public MSGBASE
{
	DWORD dwGuildId;
	char Msg[MAX_GUILD_NOTICE+1];
	int GetMsgLength() { return sizeof(MSG_GUILDNOTICE) - MAX_GUILD_NOTICE + strlen(Msg); }
};

struct TESTMSG : public MSGBASE
{
	char Msg[MAX_CHAT_LENGTH+1];
	int GetMsgLength() { return sizeof(TESTMSG) - MAX_CHAT_LENGTH + strlen(Msg); }
};

struct MSG_USER_ADD_ITEM : public MSGBASE
{
	char	CharacterName[32];
	DWORD	dwIconIdx;
	//int GetMsgLength() { return sizeof(MSG_USER_ADD_ITEM) - MAX_CHAT_LENGTH + strlen(Msg); }
};

struct TESTMSGID : public MSGBASE
{
	DWORD dwSenderID;
	char Msg[MAX_CHAT_LENGTH+1];
	int GetMsgLength() { return sizeof(TESTMSGID) - MAX_CHAT_LENGTH + strlen(Msg); }
};

struct MSG_EVENT_MONSTERREGEN : public MSGBASE
{
	WORD	MonsterKind;
	BYTE	cbMobCount;		//mob count
	WORD	wMap;			//map
	BYTE	cbChannel;		//channel	= 0:all
	VECTOR3 Pos;
	WORD	wRadius;
	DWORD	ItemID;
	DWORD	dwDropRatio;	//item drop ratio
//	BYTE	bBoss;			//�ʿ��Ѱ�?
};

struct MSG_EVENT_NPCSUMMON : public MSGBASE
{
	WORD	NpcKind;
	BYTE	cbNpcCount;		//mob count
	WORD	wMap;			//map
	BYTE	cbChannel;		//channel	= 0:all
	VECTOR3 Pos;
	WORD	wRadius;
};


struct MSG_FAME : public MSGBASE
{
	FAMETYPE Fame;
};


struct MSG_LOGIN_SYN : public MSGBASE
{
	DWORD AuthKey;
#ifdef _TW_LOCAL_
	char id[21];
	char pw[21];
#else
	char id[MAX_NAME_LENGTH+1];
	char pw[MAX_NAME_LENGTH+1];
#endif
	char Version[16];

	// 080109 LYW --- CommonStruct : ���Ⱥ�й�ȣ�� �����Ѵ�.
	// ��) (ABCD) 0895 -- ������ ���� ���ڴ� 895 �̴�.
	// 08/1000 = A, 08/100 = B, 95/10 = C, 95/1 = D �� �����͸� �����ϵ��� �Ѵ�.
	char strSecurityPW[16+1] ;

	ScriptCheckValue Check;
	char mLoginKey[MAX_PATH];
};

struct MSG_USE_DYNAMIC_SYN : public MSGBASE
{
	char id[MAX_NAME_LENGTH+1];
};

struct MSG_USE_DYNAMIC_ACK : public MSGBASE
{
	char mat[32];
};

struct MSG_LOGIN_DYNAMIC_SYN : public MSGBASE
{
	DWORD AuthKey;
	char id[MAX_NAME_LENGTH+1];
	char pw[MAX_NAME_LENGTH+1];
	char Version[16];
};

struct MSG_LOGIN_ACK : public MSGBASE
{
	char agentip[16];
	WORD agentport;
	DWORD userIdx;
	BYTE cbUserLevel;
};

struct MOVE_ONETARGETPOS : public MSGBASE
{
	DWORD			dwMoverID;
	COMPRESSEDPOS	sPos;
	COMPRESSEDPOS	tPos;

	float MoveSpeedChecker;
	float AddedMoveSpeed;

	void SetStartPos( VECTOR3 * pos )
	{
		sPos.Compress(pos);
	}
	void SetTargetPos( VECTOR3* pos )
	{
		tPos.Compress( pos );
	}
	void GetStartPos( VECTOR3 * pos )
	{
		sPos.Decompress( pos );
	}
	void GetTargetInfo(MOVE_INFO * pMoveInfo)
	{
		pMoveInfo->SetCurTargetPosIdx(0);
		pMoveInfo->SetMaxTargetPosIdx(1);
		tPos.Decompress(pMoveInfo->GetTargetPosition(0));
	}
	int GetSize()
	{
		return sizeof(MOVE_ONETARGETPOS);
	}
};

struct MOVE_ONETARGETPOS_FROMSERVER : public MSGBASE
{
	COMPRESSEDPOS	sPos;
	COMPRESSEDPOS	tPos;
	float AddedMoveSpeed;

	void SetStartPos( VECTOR3 * pos )
	{
		sPos.Compress(pos);
	}
	void GetStartPos( VECTOR3 * pos )
	{
		sPos.Decompress( pos );
	}
	void SetTargetPos( VECTOR3* pos )
	{
		tPos.Compress( pos );
	}
	void GetTargetInfo(MOVE_INFO * pMoveInfo)
	{
		pMoveInfo->SetCurTargetPosIdx(0);
		pMoveInfo->SetMaxTargetPosIdx(1);
		tPos.Decompress(pMoveInfo->GetTargetPosition(0));
	}
	int GetSize()
	{
		return sizeof(MOVE_ONETARGETPOS_FROMSERVER);
	}
};

struct MOVE_TARGETPOS : public MSGBASE
{
	DWORD dwMoverID;
	float AddedMoveSpeed;
	COMPRESSEDPOS spos;
	COMPRESSEDTARGETPOS tpos;
	void Init()
	{
		tpos.Init();
	}
	void SetStartPos(VECTOR3 * pos)
	{
		spos.Compress(pos);
	}
	void AddTargetPos(VECTOR3 * pos)
	{
		tpos.AddTargetPos(pos);
	}
	void GetStartPos(VECTOR3 * pos)
	{
		spos.Decompress(pos);
	}
	void GetTargetInfo(MOVE_INFO * pMoveInfo)
	{
		pMoveInfo->SetCurTargetPosIdx(0);//CurTargetPositionIdx = 0;
		pMoveInfo->SetMaxTargetPosIdx(tpos.PosNum);//pMoveInfo->MaxTargetPositionIdx = tpos.PosNum;
		ASSERT( tpos.PosNum <= MAX_CHARTARGETPOSBUF_SIZE );
		for(int i = 0 ; i < tpos.PosNum ; ++i)
			tpos.pos[i].Decompress(pMoveInfo->GetTargetPosition(static_cast<BYTE>(i)));//&pMoveInfo->TargetPositions[i]);
	}
	int GetSize()
	{
		return sizeof(MOVE_TARGETPOS) - tpos.GetSurplusSize();
	}
};


struct MOVE_POS : public MSGBASE
{
	DWORD dwMoverID;
	COMPRESSEDPOS cpos;
};

// 091119 ONS �δ� ���̵幫�� �޼��� �߰�
struct MSG_DUNGEON_FADE_MOVE : public MOVE_POS
{
	DWORD dwWarpIndex;
};


struct MOVE_POS_USER : public MSGBASE
{
	char	Name[MAX_NAME_LENGTH+1];
	COMPRESSEDPOS cpos;
};


struct MSG_WHERE_INFO : public MSGBASE
{
	WORD	wMapNum;		//�ʹ�ȣ
	BYTE	bChannel;		//ä�ι�ȣ
	COMPRESSEDPOS cpos;		//��ġ
};

// 100312 ONS PC����� ���� �޼��� �߰�
struct MSG_PCROOM_BUFF_INFO : public MSGBASE
{
	DWORD dwBuffIndex[5];
	WORD  wStage;
	WORD  wDay;
	BYTE  bIsChanged;
};


struct BASEOBJECT_INFO
{
	BASEOBJECT_INFO()
	{
		BattleID = 0;
		BattleTeam = 0;
	}
	DWORD dwObjectID;
	DWORD dwUserID;
	// 080218 LYW --- CommonStruct : ������Ʈ �̸��� ���̸� �ø��� �۾�.
	//char ObjectName[MAX_NAME_LENGTH+1];
	char ObjectName[32+1];
	DWORD BattleID;
	BYTE BattleTeam;
	EObjectState ObjectState;
	eObjectBattleState ObjectBattleState;
};

#define		MAX_INTRODUCE_LEN	512			// 090317 ShinJS --- ���ҽ��� TextArea ũ��� ���߱� ���� ����(�Ϻ�����226)

// ����Ʈ ��Ī. CPlayer���� ���δ�.
// �ֹ������ε� ���� �� �ִ�. certificate of residence
// desc_hseos_�ֹε��01
// S �ֹε�� �߰� added by hseos 2007.06.09
enum																	// ä�� ��û ����
{
	DATE_MATCHING_CHAT_REQUEST_STATE_NONE = 0,							// �ƹ����� �ƴ�
	DATE_MATCHING_CHAT_REQUEST_STATE_REQUEST,							// ���������� ��ȭ ��û ����
	DATE_MATCHING_CHAT_REQUEST_STATE_RECV_REQUEST,						// ���������� ��ȭ ��û�� ���� ����
	DATE_MATCHING_CHAT_REQUEST_STATE_CHATTING,							// ä�� ��
};

typedef enum															// ä�� ���
{
	DATE_MATCHING_CHAT_RESULT_REQUEST_AND_WAIT = 0,						// ..��û�ڿ��� ��ûó���� ��⸦ �˸�
	DATE_MATCHING_CHAT_RESULT_REQUESTED,								// ..����ڿ��� �������� ä�� ��û�� �˸�
	DATE_MATCHING_CHAT_RESULT_CANCEL,									// ..��û�ڱ� ��û ���
	DATE_MATCHING_CHAT_RESULT_BUSY,										// ..����ڰ� �̹� ä�� �� or ä�� ��û�� or ä�� ��û�޴� �� 
	DATE_MATCHING_CHAT_RESULT_TIMEOUT,									// ..�ð� �ʰ�
	DATE_MATCHING_CHAT_RESULT_REFUSE,									// ..������
	DATE_MATCHING_CHAT_RESULT_START_CHAT,								// ..�¶��Ͽ� ä�� ����
	DATE_MATCHING_CHAT_RESULT_END_CHAT,									// ..ä�� ��
	DATE_MATCHING_CHAT_RESULT_START_CHALLENGEZONE,						// 100107 ONS ����ڰ� ç�������� �־ ä�� �ź�
} DATE_MATCHING_CHAT_RESULT;

// E �ֹε�� �߰� added by hseos 2007.06.09

struct DATE_MATCHING_INFO						// date matching 2007/03/28
{
	BYTE	bIsValid;							// �ֹ��� �߱� �޾Ҵ°�? ����.
	BYTE	bActive;							// Ȱ��ȭ ��°�.? Ŭ���̾�Ʈ���� 'search' �ϴ� ���� Ȱ��ȭ �ȴ�.
	BYTE	byGender;							// ����.	GENDER_MALE = 0, GENDER_FEMALE = 1
	DWORD	dwAge;								// ����.
	// desc_hseos_�ֹε��01
	// S �ֹε�� �߰� added by hseos 2007.06.06	2007.06.09
	// DWORD	dwAgeIndex;						// ������ �з��ڵ�.			// �̰� ����� ���̴��� üũ��.
	DWORD	nRace;								// ����
	// E �ֹε�� �߰� added by hseos 2007.06.06	2007.06.09
	DWORD	dwRegion;							// ��� ����					enum DATE_MATCHING_REGION �� ����.
	DWORD	dwGoodFeelingDegree;				// ȣ�� ����.
	DWORD	pdwGoodFeeling[3];					// �����ϴ� ����				enum DATE_MATCHING_FEELING �� ����.
	DWORD	pdwBadFeeling[2];					// �Ⱦ��ϴ� ����
	DWORD	dwClass;							// 100113 ONS ���� ������ �����Ѵ�.
	char	szIntroduce[MAX_INTRODUCE_LEN+1];	// �Ұ�. �ϴ� char�� 300��(�ѱ� 150��)����.
	char	szName[MAX_NAME_LENGTH+1];			// ĳ���� �̸�. BASEOBJECT_INFO::ObjectName �� ����.
	// desc_hseos_�ֹε��01
	// S �ֹε�� �߰� added by hseos 2007.06.06	2007.06.09	2007.12.18
	DWORD	nSerchTimeTick;						// �˻��� ��û���� ���� �ð� ƽ. �����ð� �������� �˻� ���.
	DWORD	nChatPlayerID;						// ä�� ����� ID
	DWORD	nRequestChatState;					// ä�� ��û ����
	DWORD	nRequestChatTimeTick;				// ä�� ��û ���� �ð� ƽ
	DWORD	nChatPlayerAgentID;					// ä�� ����� ������Ʈ ID
	BYTE	nMatchingPoint;						// ��Ī����Ʈ	
	// E �ֹε�� �߰� added by hseos 2007.06.06	2007.06.09	2007.12.18
};

struct MSG_ITEM_TRANSMOG_SYN : public MSGBASE
{
	POSTYPE ItemPos;
	DWORD	wItemIdx;
	DWORD	wItemDBIdx;

	POSTYPE SacrificeItemPos;
	DWORD	wSacrificeItemIdx;
	DWORD	wSacrificeItemDBIdx;

	POSTYPE MaterialItemPos;
	DWORD	wMaterialItemIdx;
	DWORD	wMaterialItemDBIdx;

	WORD PartTypeItem;
	WORD PartTypeSacrifice;

	eEquipType EquipTypeItem;
	eEquipType EquipTypeSacrifice;

	EWEARED_ITEM EquipSlotItem;
	EWEARED_ITEM EquipSlotSacrifice;
};

struct MSG_ITEM_TRANSMOG_ACK : public MSGBASE
{
	POSTYPE		TargetPos;
	ITEM_OPTION	OptionInfo;
};



struct CHARACTER_TOTALINFO
{
	DWORD	Life;							// ������			//?
	DWORD	MaxLife;						// �ִ� ������		//?
	
	BYTE	Race;
	BYTE	Gender;
	BYTE	FaceType;
	BYTE	HairType;
	DWORD	WearedItemIdx[eWearedItem_Max];
	DWORD	WearedItemTransmog[eWearedItem_Max];
	DWORD	HideFlag;
	BYTE	WeaponEnchant;

	BYTE	JobGrade;
	BYTE	Job[ MAX_JOB_GRADE ];
	LEVELTYPE	Level;						// ����				//?
	MAPTYPE	CurMapNum;						// �����			//?
	MAPTYPE	LoginMapNum;					// �α��θ�			//?
	BOOL	bPeace;
	WORD	MapChangePoint_Index;			// ����ȯ�� ���	//?
	WORD	LoginPoint_Index;				// �α��ν� ���	//?

	DWORD	MunpaID;						// ���� �ε���
	BYTE	PositionInMunpa;				// ���� ���� 

	DWORD   FamilyID;
	DWORD	nFamilyEmblemChangedFreq;
	// E �йи� �߰� added by hseos 2007.11.22

	MARKNAMETYPE MarkName;					// guild mark name	
	MARKNAMETYPE FamilyMarkName;					// guild mark name	
	BOOL bVisible;
	bool bPKMode;
	FAMETYPE BadFame;
	float	Height;							// Ű
	float	Width;							// ü��

	char	NickName[MAX_GUILD_NICKNAME+1]; // ��忡���� ȣĪ
	char	GuildName[MAX_GUILD_NAME+1];	// ���� �̸�

	char    FamilyNickName[MAX_GUILD_NICKNAME+1];
	char	FamilyName[MAX_GUILD_NAME+1];
	
	DWORD	dwGuildUnionIdx;					// ����
	char	sGuildUnionName[MAX_GUILD_NAME+1];	// �����̸�
	DWORD	dwGuildUnionMarkIdx;				// ���͸�ũ
	WORD	HideLevel;
	WORD	DetectLevel;
	WORD wInventoryExpansion;
	BOOL bUsingWaterSeed;
	DATE_MATCHING_INFO DateMatching;
	BOOL	bWingsMove;
};

struct SEND_MONSTER_TOTALINFO	:	public MSGBASE
{
	BASEOBJECT_INFO BaseObjectInfo;
	MONSTER_TOTALINFO TotalInfo;
	SEND_MOVEINFO MoveInfo;

	BYTE bLogin;
	
	CAddableInfoList AddableInfo;

	WORD GetMsgLength()	{	return sizeof(SEND_MONSTER_TOTALINFO) - sizeof(CAddableInfoList) + AddableInfo.GetInfoLength();	}
};

// 090316 LUJ, Ż�� ����
struct SEND_VEHICLE_TOTALINFO :	public MSGBASE
{
	BASEOBJECT_INFO BaseObjectInfo;
	MONSTER_TOTALINFO TotalInfo;
	SEND_MOVEINFO MoveInfo;
	BYTE bLogin;
	// 090316 LUJ, �¼� ��ȣ���� ž���� �÷��̾� ��ȣ�� ����ִ�
	DWORD mSeat[ VehicleScript::Seat::MaxSize ];
	// 091105 ShinJS --- �������� �̸�����
	char MasterName[ MAX_NAME_LENGTH + 1 ];
	DWORD usedItemPosition;

	CAddableInfoList AddableInfo;

	WORD GetMsgLength()	{ return sizeof( *this ) - sizeof( AddableInfo ) + AddableInfo.GetInfoLength();	}
};

struct SEND_NPC_TOTALINFO	:	public MSGBASE
{
	BASEOBJECT_INFO BaseObjectInfo;
	NPC_TOTALINFO TotalInfo;
	SEND_MOVEINFO MoveInfo;
	
	float Angle;
	BYTE bLogin;

	CAddableInfoList AddableInfo;

	WORD GetMsgLength()	{	return sizeof(SEND_NPC_TOTALINFO) - sizeof(CAddableInfoList) + AddableInfo.GetInfoLength();	}
};

struct SEND_CHARACTER_TOTALINFO : public MSGBASE
{
	BASEOBJECT_INFO BaseObjectInfo;
	CHARACTER_TOTALINFO TotalInfo;	
	SEND_MOVEINFO MoveInfo;
	struct Vehicle
	{
		DWORD mVehicleIndex;
		DWORD mSeatIndex;
	}
	mMountedVehicle;
	struct stHouseRideInfo
	{
		DWORD dwFurnitureObjectIndex;
		WORD wSlot;
	} mRiderInfo;

	BYTE bLogin;
	
	CAddableInfoList AddableInfo;

	WORD GetMsgLength()	{	return sizeof(SEND_CHARACTER_TOTALINFO) - sizeof(CAddableInfoList) + AddableInfo.GetInfoLength();	}
};

struct SEND_HERO_TOTALINFO : public MSGBASE
{
	BASEOBJECT_INFO BaseObjectInfo;
	CHARACTER_TOTALINFO ChrTotalInfo;
	HERO_TOTALINFO HeroTotalInfo;
	SEND_MOVEINFO SendMoveInfo;
	DWORD UniqueIDinAgent;
	WORD	SkillNum;
	ITEM_TOTALINFO ItemTotalInfo;
	WORD OptionNum;
	WORD PetNum;
	SYSTEMTIME ServerTime; //ĳ���� ���� �α��� �ð�
	DWORD StorageSize;
	CAddableInfoList AddableInfo;

	WORD GetMsgLength()
	{
		return sizeof( SEND_HERO_TOTALINFO ) - sizeof(CAddableInfoList) + AddableInfo.GetInfoLength();
	}
};

// 071129 LYW --- CommonStruct : ��ų �ʱ�ȭ ���� ���� ����ü �߰�.
struct SEND_SKILL_RESETINFO : public MSGBASE	
{
	CAddableInfoList AddableInfo ;
	WORD SkillNum ;
	int nSkillPoint ;
} ;


struct SEND_CHARSELECT_INFO : public MSGBASE
{
#ifdef _CRYPTCHECK_ 
	HselInit eninit;
	HselInit deinit;
#endif
	int CharNum;
	WORD StandingArrayNum[MAX_CHARACTER_NUM];				// pjs �ɸ����� ��ġ ���� [5/22/2003]
	BASEOBJECT_INFO BaseObjectInfo[MAX_CHARACTER_NUM];
	CHARACTER_TOTALINFO ChrTotalInfo[MAX_CHARACTER_NUM];
};

struct MSG_NAME : public MSGBASE
{
	char Name[MAX_NAME_LENGTH+1];
};

struct MSG_NAME2 : public MSGBASE
{
	char str1[MAX_NAME_LENGTH+1];
	char str2[MAX_NAME_LENGTH+1];
};

struct MSG_NAME_DWORD : public MSGBASE
{
	char Name[MAX_NAME_LENGTH+1];
	DWORD dwData;
};

struct MSG_NAME_WORD : public MSGBASE
{
	char Name[MAX_NAME_LENGTH+1];
	WORD wData;
};

struct MSG_NAME_DWORD2 : public MSGBASE
{
	char Name[MAX_NAME_LENGTH+1];
	DWORD dwData1;
	DWORD dwData2;
};

struct MSG_HOUSENAME : public MSGBASE
{
	char Name[MAX_HOUSING_NAME_LENGTH];
	DWORD dwData;
};

//ÆÑ µ¥?ÌÅÝ °ú·Ã ////////////////////////////////////////////////
struct MSG_NAME_DWORD3 : public MSGBASE
{
	char	Name[MAX_NAME_LENGTH+1];
	DWORD	dwData1;
	DWORD	dwData2;
	DWORD	dwData3;
};

struct MSG_NAME_DWORD4 : public MSGBASE
{
	char	Name[MAX_NAME_LENGTH+1];
	DWORD	dwData1;
	DWORD	dwData2;
	DWORD	dwData3;
	DWORD	dwData4;
};

struct MSG_NAME_DWORD5 : public MSGBASE
{
	char	Name[MAX_NAME_LENGTH+1];
	DWORD	dwData1;
	DWORD	dwData2;
	DWORD	dwData3;
	DWORD	dwData4;
	DWORD	dwData5;
};

struct MSG_NAME2_DWORD : public MSGBASE
{
	char	Name1[MAX_NAME_LENGTH+1];
	char	Name2[MAX_NAME_LENGTH+1];
	DWORD	dwData;
};

// desc_hseos_����Ʈ ��_01
// S ����Ʈ �� �߰� added by hseos 2008.01.23
struct MSG_NAME2_DWORD2 : public MSGBASE
{
	char	Name1[MAX_NAME_LENGTH+1];
	char	Name2[MAX_NAME_LENGTH+1];
	DWORD	dwData1;
	DWORD	dwData2;
};

// 091124 ONS ç������ ���� �˸� �޼��� �߰�
struct MSG_NAME2_DWORD3 : public MSGBASE
{
	char	Name1[MAX_NAME_LENGTH+1];
	char	Name2[MAX_NAME_LENGTH+1];
	DWORD	dwData1;
	DWORD	dwData2;
	DWORD	dwData3;
};
// E ����Ʈ �� �߰� added by hseos 2008.01.23

//�� ������ ���� ////////////////////////////////////////////////
struct SEND_PACKED_DATA : public MSGBASE
{
	WORD wRealDataSize;
	WORD wReceiverNum;
	char Data[MAX_PACKEDDATA_SIZE];
};
struct SEND_PACKED_TOMAPSERVER_DATA : public MSGBASE
{
	MAPTYPE ToMapNum;
	WORD wRealDataSize;
	char Data[MAX_PACKEDDATA_SIZE];
	WORD GetMsgLength()
	{
		return sizeof(SEND_PACKED_TOMAPSERVER_DATA) - MAX_PACKEDDATA_SIZE + wRealDataSize;
	}
};
// ������ ���� ��Ʈ�p �޽��� ////////////////////////////////////

struct MSG_ITEM : public MSGBASE
{
	ITEMBASE ItemInfo;
};

struct MSG_ITEM_WITH_OPTION : public MSGBASE
{
	ITEMBASE ItemInfo;
	ITEM_OPTION OptionInfo;
};

struct MSG_CONSIGNMENT_REGIST : public MSG_ITEM
{
	DWORD dwCategory1Value;
	DWORD dwCategory2Value;

	char szItemName[MAX_ITEMNAME_LENGTH + 1];
	DWORD dwDeposit;
	DWORD dwCommission;
	DWORD dwPrice;
	POSTYPE FromPos;
};

struct MSG_CONSIGNMENT_SEARCH_REQUEST : public MSGBASE
{
	WORD wSortType;
	WORD wStartPage;
	int nCategory1;
	int nCategory2;
	int nMinLevel;
	int nMaxLevel;
	int nRareLevel;
};

struct MSG_CONSIGNMENT_SEARCHINDEX_REQUEST : public MSGBASE
{
	WORD wSortType;
	WORD wStartPage;
	DWORD dwItemList[CONSIGNMENT_SEARCHINDEX_NUM];
};

struct MSG_CONSIGNMENT_UPDATE : public MSGBASE
{
	DWORD dwConsignmentIndex;
	WORD wUpdateKind;

	DWORD dwBuyItemIndex;
	DWORD dwBuyTotalPrice;
	WORD wBuyDurability;
};

struct MSG_CONSIGNMENT_UPDATERESULT : public MSGBASE
{
	int nResult;
	DWORD dwTotalPrice;
	char szItemName[MAX_ITEMNAME_LENGTH + 1];
};

#define MAX_CONSIGNMENT_REGISTNUM 10
struct stConsignmentItemInfo
{
	DWORD dwConsignmentIdx;
	DWORD dwItemDBIdx;
	DWORD dwItemIndex;
	DWORD dwPlayerIndex;
	char szPlayerName[MAX_NAME_LENGTH + 1];
	DWORD dwCategory1;
	DWORD dwCategory2;
	WORD wRareLevel;
	WORD wUseLevel;
	WORD wEnchant;
	DWORD dwRemainMinute;
	WORD wInitDurability;
	DWORD dwDeposit;
	DWORD dwCommission;
	DWORD dwUnitPrice;
	DWORD dwAverageSellPrice;
};

struct MSG_CONSIGNMENT_SEARCH_RESULT : public MSGBASE
{
	DWORD dwCount;
	DWORD dwStartPage;
	DWORD dwEndPage;

	stConsignmentItemInfo ItemList[MAX_CONSIGNMENT_REGISTNUM];
	ITEMBASE ItemInfo[MAX_CONSIGNMENT_REGISTNUM];
	ITEM_OPTION OptionInfo[MAX_CONSIGNMENT_REGISTNUM];
};

struct MSG_CONSIGNMENT_LIST : public MSGBASE
{
	DWORD dwCount;
	DWORD dwStartPage;
	DWORD dwTotalPage;
	stConsignmentItemInfo ItemList[MAX_CONSIGNMENT_REGISTNUM];
};


// 080130 LUJ, ������ �ɼ��� �����ϴ� �޽��� ����ü
struct MSG_ITEM_OPTION : public MSGBASE
{
	DWORD		mSize;
	//ITEM_OPTION mOption[ SLOT_MAX_INVENTORY_NUM ];
	ITEM_OPTION mOption[ SLOT_MAX_ITEM_OPTION_NUM  ];

	DWORD GetSize() const
	{
		return sizeof( MSG_ITEM_OPTION ) - ( sizeof( mOption ) / sizeof( *mOption ) - mSize ) * sizeof( *mOption );
	}
};

//���� �۾�
struct ITEMOBTAINARRAY : public MSGBASE
{
	ITEMOBTAINARRAY()
	{
		ItemNum = 0;
	}
	void Init(MP_CATEGORY cg, BYTE pt, DWORD dwID)
	{
		Category = BYTE(cg);
		Protocol = pt;
		dwObjectID = dwID;
	}
	void AddItem(DWORD DBIdx, DWORD ItemIdx, DURTYPE Durability, POSTYPE bPosition, POSTYPE QuickPosition, ITEMPARAM Param, DWORD RareIdx, ITEM_SEAL_TYPE Sealed, int RemainSecond = 0)
	{
		ItemInfo[ItemNum].dwDBIdx		= DBIdx;
		ItemInfo[ItemNum].wIconIdx		= ItemIdx;
		ItemInfo[ItemNum].Durability	= Durability;
		ItemInfo[ItemNum].Position		= bPosition;
		ItemInfo[ItemNum].QuickPosition	= QuickPosition;
		ItemInfo[ItemNum].ItemParam		= Param;
		//SW050920 RareItem
		// 071127 KTH -- ���ΰ� ��ȿ�ð��� �־��ش�.
		ItemInfo[ItemNum].nSealed		= Sealed;
		ItemInfo[ItemNum].nRemainSecond = RemainSecond;
		ItemNum++;
	}
	void AddItem( WORD idx, ITEMBASE * item)
	{
		memcpy( &ItemInfo[idx], item, sizeof( ITEMBASE ) );
		ItemNum++;
	}
	ITEMBASE * GetItem( WORD idx ) { return &ItemInfo[idx]; }
	int GetSize()
	{
		//return ( sizeof( ITEMOBTAINARRAY ) - ( sizeof( ITEMBASE ) * ( SLOT_INVENTORY_NUM - ItemNum ) ) );
		// 071213 KTH --
		return ( sizeof( ITEMOBTAINARRAY ) - ( sizeof( ITEMBASE ) * ( SLOT_MAX_INVENTORY_NUM - ItemNum ) ) );
	}
	void Clear()
	{
		memset( this, 0, sizeof( ITEMOBTAINARRAY ) );
		ItemNum = 0;
	}
	BYTE		ItemNum;
	WORD		wObtainCount;
	//ITEMBASE	ItemInfo[SLOT_INVENTORY_NUM];
	// 071213 KTH --
	ITEMBASE	ItemInfo[SLOT_MAX_INVENTORY_NUM];
};

struct MSG_ITEM_ERROR : public MSGBASE
{
	int ECode;
};

struct MSG_ITEM_MOVE_SYN : public MSGBASE
{
	POSTYPE FromPos;
	DWORD wFromItemIdx;
	POSTYPE ToPos;
	DWORD wToItemIdx;

	WORD PartTypeA;
	WORD PartTypeB;

	eEquipType EquipTypeA;
	eEquipType EquipTypeB;

	EWEARED_ITEM EquipSlotA;
	EWEARED_ITEM EquipSlotB;
};

struct MSG_PET_ITEM_MOVE_SYN : public MSGBASE
{
	POSTYPE PetPos;
	DWORD PetItemIdx;
	POSTYPE PlayerPos;
	DWORD PlayerItemIdx;
};

struct MSG_ITEM_MOVE_ACK : public MSG_ITEM_MOVE_SYN
{
};
struct MSG_ITEM_COMBINE_SYN : public MSGBASE
{
	DWORD wItemIdx;
	POSTYPE FromPos;
	POSTYPE ToPos;
	DURTYPE FromDur;
	DURTYPE ToDur;
};
struct MSG_ITEM_COMBINE_ACK : public MSG_ITEM_COMBINE_SYN
{
};
struct MSG_ITEM_DIVIDE_SYN : public MSGBASE
{
	DWORD wItemIdx;
	POSTYPE FromPos;
	POSTYPE ToPos;
	DURTYPE FromDur;
	DURTYPE ToDur;
};

struct MSG_ITEM_DISCARD_SYN : public MSGBASE
{
	POSTYPE TargetPos;
	DWORD wItemIdx;
	DURTYPE ItemNum;
};

struct MSG_ITEM_BUY_SYN : public MSGBASE
{
	DWORD wBuyItemIdx;
	WORD BuyItemNum;
	WORD wDealerIdx;
};

struct MSG_ITEM_SELL_SYN : public MSGBASE
{
	POSTYPE TargetPos;
	DWORD	wSellItemIdx;
	WORD	SellItemNum;
	WORD	wDealerIdx;
};
struct MSG_ITEM_SELL_ACK : public MSG_ITEM_SELL_SYN
{
};


struct MSG_ITEM_ENCHANT_SYN : public MSGBASE
{
	POSTYPE ItemPos;
	DWORD	wItemIdx;

	POSTYPE MaterialItemPos;
	DWORD	wMaterialItemIdx;

	// 080228 LUJ, ��ȣ ������
	ICONBASE mProtectionItem;
};


struct MSG_ITEM_ENCHANT_ACK : public MSGBASE
{
	POSTYPE		TargetPos;
	ITEM_OPTION	OptionInfo;
};


// 071217 LUJ, Ŭ���̾�Ʈ �˻�����θ� ���
struct MSG_ITEM_MIX_SYN : public MSGBASE
{
	DWORD	mResultIndex;
	DWORD	mSourceIndex;	// �迭���� �⺻ �������� ���° �迭�� ��ġ���ִ��� �˷��ش�
	DWORD	mMixSize;		// ���� ������ ����� ��� ����
	DWORD	mSourceSize;	// ��ᰡ �����ϴ� ���� ����
	DWORD	mResultSize;	// ����� �����ϴ� ���� ����

	// 080228 LUJ, ��ȣ ������
	ICONBASE mProtectionItem;

	struct Material
	{
		DWORD	mItemIndex;
		POSTYPE	mPosition;
		DWORD	mQuantity;
	}
	mMaterial[ SLOT_MAX_INVENTORY_NUM * 2 ];	// ��� ���԰� ��� ������ ��� �ʿ��ϹǷ� �κ��丮 ������ �ι踦 ��´�

	int GetSize()
	{
		return sizeof( MSG_ITEM_MIX_SYN ) - ( sizeof( mMaterial ) / sizeof( Material ) - ( mSourceSize + mResultSize ) ) * sizeof( Material );
	}
};



// 071217 LUJ, ����� ���������� ������ �����ϹǷ� �ʿ� ������ �ѱ��
struct MSG_ITEM_MIX_SYN2 : public MSGBASE
{
	POSTYPE	mSourceItemPosition;
	DWORD	mSourceItemDbIndex;
	DWORD	mMixSize;
	DWORD	mResultIndex;

	// 080228 LUJ, ��ȣ ������
	ICONBASE mProtectionItem;
	// 080925 LUJ, ���� ������
	ICONBASE mSupportItem;
};

struct ITEM_UPDATE_RESULT
{
	DWORD mSize;
	ITEMBASE mItem[SLOT_MAX_INVENTORY_NUM];

	size_t GetSize() const
	{
		return sizeof(*this) - (sizeof(mItem) / sizeof(*mItem) - mSize) * sizeof(*mItem);
	}
};

// 071217 LUJ
// ���⿡�� �ð� ������ ó���� ������ ������ ���簡 ����. ó������ �о ������ 0�̸� �����ϰ� 2�̸� 2�� ������Ʈ�ϸ� �ȴ�.
// ����: ���ߺ� �������� ��� Durability = UINT_MAX�� ��� ������ ��������
// ����: ���̴� ������ ���� ���´�. ���� ���� ������ �������� mMaterlal + mRemoveSize�� �����͸� ��ȸ�Ѵ�
struct MSG_ITEM_MIX_ACK	: public MSGBASE
{
	POSTYPE		mSourceItemPosition;
	DWORD		mSourceItemDbIndex;
	DWORD		mMixSize;
	DWORD		mResultIndex;
	ITEM_UPDATE_RESULT mUpdateResult;

	size_t GetSize() const
	{
		return sizeof(*this) - sizeof(mUpdateResult) + mUpdateResult.GetSize();
	}
};

struct MSG_ITEM_USE_SYN : public MSGBASE
{
	POSTYPE TargetPos;
	DWORD	wItemIdx;
};

// 100517 ONS �����ʱ�ȭ ������ ó�� �߰�
struct MSG_ITEM_CHANGE_CLASS : public MSGBASE
{
	POSTYPE TargetPos;
	DWORD	wItemIdx;
	BYTE	ClassStage[MAX_CLASS_LEVEL - 1];
};

struct MSG_ITEM_USE_ACK : public MSGBASE
{
	POSTYPE			TargetPos;
	DWORD			dwItemIdx;
	eItemUse_Err	eResult;
};

struct MSG_ITEM_DISSOLVE_SYN : public MSGBASE
{
	size_t mSourceSize;
	size_t mSize;

	struct Item
	{
		enum Type
		{
			TypeNone,
			TypeSource,
			TypeResult,
		}
		mType;
		DWORD mIndex;
		DWORD mDbIndex;
		POSTYPE	mPosition;
		DURTYPE	mQuantity;
	}
	mItem[SLOT_MAX_INVENTORY_NUM];

	BOOL Add(const Item& item)
	{
		if(mSize >= SLOT_MAX_INVENTORY_NUM)
		{
			return FALSE;
		}

		mItem[mSize++] = item;
		mSourceSize += (Item::TypeSource == item.mType ? 1 : 0);
		return TRUE;
	}

	size_t GetSize() const
	{
		return sizeof(MSG_ITEM_DISSOLVE_SYN) - (sizeof(mItem) / sizeof(*mItem) - mSize) * sizeof(Item);
	}
};

struct MSG_ITEM_DISSOLVE_ACK : public MSGBASE
{
	ITEM_UPDATE_RESULT mUpdateResult;

	size_t GetSize() const
	{
		return sizeof(*this) - sizeof(mUpdateResult) + mUpdateResult.GetSize();
	}
};

struct MSG_ITEM_REINFORCE_SYN : public MSGBASE
{
	DWORD	mSourceItemIndex;		// ��ȭ�� ��� ������ ��ȣ
	POSTYPE mSourceItemPosition;	// ��ȭ�� ��� ������ ��ġ
	DWORD	mSourceItemDbIndex;
	WORD	mSize;					// ��� ����

	// 080228 LUJ, ��ȣ ������
	ICONBASE mProtectionItem;
	// 080929 LUJ, ���� ������
	ICONBASE mSupportItem;
	ITEMBASE mItem[ SLOT_MAX_INVENTORY_NUM ];	// 071213 KTH --

	DWORD GetSize() const
	{
		return sizeof(MSG_ITEM_REINFORCE_SYN) - ( sizeof( mItem ) / sizeof( *mItem ) - mSize ) * sizeof( *mItem );
	}	
};


struct MSG_ITEM_REINFORCE_OPTION_ACK : public MSGBASE
{
	DWORD		mItemDbIndex;
	ITEM_OPTION mOption;
};


struct MSG_ITEM_REINFORCE_ACK : public MSGBASE
{
	DWORD		mSourceItemDbIndex;
	
	DWORD		mSize;
	ITEMBASE	mItem[ SLOT_MAX_INVENTORY_NUM ];

	DWORD GetSize() const
	{
		return sizeof( MSG_ITEM_REINFORCE_ACK ) - ( sizeof( mItem ) / sizeof( *mItem ) - mSize ) * sizeof( *mItem );
	}
};

// 081119 NYJ - �丮
struct MSG_COOK_SYN : public MSGBASE
{
	DWORD	dwRecipeIdx;
	DWORD	dwFireNpcIdx;
	WORD	wMakeNum;
};

struct MSG_COOK_ACK : public MSGBASE
{
	DWORD dwResultIdx;
	ITEM_UPDATE_RESULT mUpdateResult;

	size_t GetSize() const
	{
		return sizeof(*this) - sizeof(mUpdateResult) + mUpdateResult.GetSize();
	}
};

// ����â
struct SLOT_INFO
{
	BYTE	kind;
	DWORD	dbIdx;
	DWORD	idx;
	WORD	data;
};

struct TAB_INFO
{
	BYTE		tabNum;
	SLOT_INFO	slot[10];
};

struct MSG_QUICK_INFO : public MSGBASE
{
	TAB_INFO	tab[8];
};

struct MSG_QUICK_ADD_SYN : public MSGBASE
{
	BYTE		tabNum;
	BYTE		pos;
	SLOT_INFO	slot;
};

struct MSG_QUICK_REMOVE_SYN : public MSGBASE
{
	BYTE		tabNum;
	BYTE		pos;
};

struct MSG_QUICK_CHANGE_SYN : public MSGBASE
{
	BYTE	tabNum1;
	BYTE	pos1;
	BYTE	tabNum2;
	BYTE	pos2;
};

struct MSG_SKILL_UPDATE : public MSGBASE
{
	SKILL_BASE SkillBase;
};

// 080417 LUJ, ��ų ���� ���� ����ü
struct MSG_SKILL_LIST : public MSGBASE
{
	DWORD		mSize;
	SKILL_BASE	mData[ 100 ];

	DWORD GetSize() const
	{
		return sizeof( MSG_SKILL_LIST ) - ( sizeof( mData ) / sizeof( *mData ) - mSize ) * sizeof( *mData );
	}
};

struct MSG_LINKITEM : public MSG_ITEM
{
	POSTYPE		wAbsPosition;
};

//SW050920 Rare
struct MSG_LINKITEM_ADD : public MSG_LINKITEM	//��ȯ�� �ɼ������� �Ѱ��ٶ� ����.
{
	BYTE				bPetInfo;
	ITEM_OPTION			sOptionInfo;
};

struct MSG_LINKITEMEX : public MSG_LINKITEM
{
	MONEYTYPE	dwMoney;
};

struct MSG_LINKBUYITEMEX : public MSG_LINKITEMEX
{
	WORD wVolume;
};

struct MSG_ITEMEX : public MSG_ITEM
{
	BYTE PetInfoCount;
	ITEM_OPTION	sOptionInfo;
	MSG_ITEMEX() {memset(this, 0, sizeof(MSG_ITEMEX));}
};

struct MSG_BUYITEM : public MSG_ITEMEX
{
	DWORD dwData;
};

struct MSG_LOOTINGIEM : public MSG_LINKITEM
{
	DWORD				dwDiePlayerIdx;
	BYTE				bPetSummonItem;
	ITEM_OPTION			sOptionInfo;
};

struct SEND_LINKITEM_TOTALINFO : public MSG_LINKITEMEX
{
	BYTE PetInfoCount;
	ITEM_OPTION sOptionInfo;
};

struct SEND_LINKBUYITEM_TOTALINFO : public SEND_LINKITEM_TOTALINFO
{
	WORD wVolume;
};

struct MSG_EXCHANGE_REMOVEITEM : public MSGBASE
{
	POSTYPE		wAbsPosition;
};

// 071026 LYW --- CommonStruct : Add struct to change item count.
struct MSG_EXCHANGE_DECREASEITEM : public MSGBASE
{
	POSTYPE		wAbsPosition ;
	int			nItemCount ;
} ;

struct MSG_ITEM_DESTROY : public MSGBASE
{
	POSTYPE		wAbsPosition;
	BYTE		cbReason;
};

// LBS ������ ����..
struct MSG_STREETSTALL_TITLE : public MSGBASE
{
	char Title[MAX_STREETSTALL_TITLELEN+1];
	WORD StallKind;
};

struct MSG_STREETSTALL_ITEMSTATUS : public MSG_ITEM
{
	POSTYPE		wAbsPosition;
	MONEYTYPE		dwMoney;
	WORD	wVolume;
};


struct STREETSTALLITEM
{
	WORD wVolume; // ���ų����� ����
	DWORD wIconIdx;
	DWORD dwDBIdx;
	DURTYPE Durability;
	MONEYTYPE money;
	char Locked; // ����
	char Fill;
	ITEM_SEAL_TYPE nSeal;
	ITEMPARAM ItemParam;
};

struct STREETSTALL_INFO : public MSGBASE
{
	DWORD StallOwnerID;
	char Title[MAX_STREETSTALL_TITLELEN+1];
	STREETSTALLITEM Item[SLOT_STREETSTALL_NUM];
	WORD StallKind;

	WORD count;
	WORD PetItemCount;
	
	CAddableInfoList AddableInfo;

	WORD GetSize()
	{
		return sizeof( STREETSTALL_INFO ) - sizeof(CAddableInfoList) + AddableInfo.GetInfoLength();
	}
};

struct STREETSTALLTITLE_INFO : public MSGBASE
{
	char Title[MAX_STREETSTALL_TITLELEN+1];
};

struct STREETSTALL_BUYINFO : public MSG_ITEM
{
	DWORD	StallOwnerID;
	POSTYPE StallPos;
	DWORDEX	dwBuyPrice;
};

// LBS StreetStall 03.11.27
struct MSG_REGISTITEM : public MSG_ITEM
{
	DWORD dwData;
};

struct MSG_REGISTITEMEX : public MSG_REGISTITEM
{
	MONEYTYPE dwMoney;
};

struct MSG_REGISTBUYITEMEX : public MSG_REGISTITEMEX
{
	WORD wVolume;
	WORD wAbsPosition;
};

struct MSG_SELLITEM : public MSG_REGISTITEM
{
	DWORD count;
};

struct MSG_STALLMSG	: public MSGBASE
{
	DWORD	SellerID;
	DWORD	BuyerID;
	DWORD	ItemIdx;
	WORD	count;
	DWORDEX	money;
};

struct ITEM_SELLACK_INFO : public MSGBASE
{
	//DWORD ItemDBIdx;			// check�� ����  �ӽ� ����
	MONEYTYPE Money;
	POSTYPE Pos;
};

//struct APPEARANCE_INFO : public MSGBASE
struct MSG_APPERANCE_ADD : public MSGBASE
{
	//DWORD	PlayerID;
	DWORD	mSlotIndex;
	DWORD	mItemIndex;
	 DWORD	mItemTransmog;
	//DWORD WearedItem[eWearedItem_Max];
};


struct MSG_APPERANCE_REMOVE : public MSGBASE
{
	DWORD	mSlotIndex;
};

struct MSG_LEVEL : public MSGBASE
{
	enum EXPPOINTKIND
	{
		EXPPOINTKIND_ACQUIRE,
		EXPPOINTKIND_DIE,
	};
	LEVELTYPE Level;
	EXPTYPE CurExpPoint;
	EXPTYPE MaxExpPoint;
};
struct MSG_EXPPOINT : public MSGBASE
{
	enum EXPPOINTKIND
	{
		EXPPOINTKIND_ACQUIRE,
		EXPPOINTKIND_DIE,
	};
	EXPTYPE ExpPoint;
	BYTE ExpKind;
};
struct MSG_SKILLEXP : public MSGBASE
{
	EXPTYPE ExpPoint;
	POSTYPE absPos;
};
struct M2C_DAMAGE_MSG : public MSGBASE
{
	DWORD dwDamage;
};


//////////////////////////////////////////////////////////////////////////
// Skill ����
struct MAINTARGET
{
	enum
	{
		MAINTARGETKIND_OBJECTID,
		MAINTARGETKIND_POS,
	};
	BYTE MainTargetKind;
	union {
		DWORD dwMainTargetID;		
		COMPRESSEDPOS cpTargetPos;
	};
	void SetMainTarget(DWORD id)
	{	MainTargetKind = MAINTARGETKIND_OBJECTID;	dwMainTargetID = id;	}
	void SetMainTarget(VECTOR3* pp)
	{	MainTargetKind = MAINTARGETKIND_POS;	cpTargetPos.Compress(pp);	}
	MAINTARGET()	{}
	MAINTARGET(DWORD id)	{	SetMainTarget(id);	}
	MAINTARGET(VECTOR3* pp){	SetMainTarget(pp);	}
};

struct SKILLOBJECT_INFO
{
	DWORD SkillObjectIdx;
	DWORD SkillIdx;

	VECTOR3 Pos;
	DWORD StartTime;
	VECTOR3 Direction;
	BYTE SkillLevel;
	DWORD Operator;

	MAINTARGET MainTarget;

	DWORD BattleID;
	WORD Option;
	float Rate;
	DWORD RemainTime;
	// 090204 LUJ, ��ġ ������ Ȯ����
	int Count;
	bool IsNew;
};

enum SKILLKIND
{
	SKILLKIND_PHYSIC,
	SKILLKIND_MAGIC,
	SKILLKIND_PASSIVE,
	SKILLKIND_ONOFF,
	SKILLKIND_BUFF,
	SKILLKIND_MAX
};

enum UNITKIND
{
	UNITKIND_NONE,
	UNITKIND_PHYSIC_ATTCK,
	UNITKIND_MAGIC_ATTCK,
	UNITKIND_LIFE_RECOVER,
	UNITKIND_RESURRECTION,
	UNITKIND_DEBUFF,
	UNITKIND_FAKE_DAMAGE,
	UNITKIND_DECOY,
	UNITKIND_MANA_RECOVER,
	UNITKIND_RETURN,
	UNITKIND_SPELLBREAK,
	UNITKIND_SUMMON,
	// 091127 LUJ, �÷��̾� ��ȯ
	UNITKIND_RECALL,
	UNITKIND_TAME,
	UNITKIND_PHYSIC_FAKE_DAMAGE,
	UNITKIND_MAGIC_FAKE_DAMAGE,
	UNITKIND_MAX,
};

#define MAX_BUFF_COUNT		5
#define MAX_STATUS_COUNT	3

// 070215 LYW --- CommonStruct : Add struct for conduct part.
struct CONDUCT_INFO
{
	char	ConductName[MAX_NAME_LENGTH+1] ;

	WORD	ConductIdx ;
	WORD	ConductTootipIdx ;
	WORD	ConductKind ;
	WORD	ConductPos ;

	int		HighImage ;

	int		MotionIdx ;

	// 091022 ShinJS --- Ż�� ž�½��� ���� ����
	BOOL	bInvalidOnVehicle;
} ;

struct SKILL_INFO
{
	DWORD Index;
	TCHAR Name[MAX_SKILL_NAME_LENGTH+1];
	/// ���� �ε���
	DWORD Tooltip;
	int	Image;
	SKILLKIND Kind;
	/// ��ų ����
	LEVELTYPE Level;
	// 071224 KTH -- ��Ƽ�߿� ��� �Ұ����� ���� �ΰ�?
	BOOL Party;
	DWORD Money;
	// 080616 LUJ, ����/HP �ҿ䰡 ������ �ƴ϶� �ۼ�Ʈ�ε� �����ϰ� �Ѵ�
	struct Value
	{
		float mPercent;
		float mPlus;
	};
	Value mLife;
	Value mMana;
	// 091211 LUJ, ��� ����
	eWeaponType mWeaponType;
	eWeaponAnimationType mWeaponAnimationType;
	eArmorType mArmorType;
};

struct BUFF_SKILL_INFO : public SKILL_INFO
{
	DWORD DelayTime;
	int SkillEffect;
	// 080618 LUJ, � ������ ���� ��ų���� ��Ÿ��. enum������ ����
	eStatusKind	Status;
	// 080616 LUJ, ���� � ���·� ó���ϴ��� ��Ÿ��. enum������ ����
	enum DataType
	{
		StatusTypeNone,
		StatusTypeAdd,
		StatusTypePercent,
	}
	StatusDataType;
	float StatusData;
	// 090204 LUJ, ���ǿ� ���� ���� ���� ���θ� �Ѱų� ���� ����
	struct Condition
	{
		// 090204 LUJ, �� ���
		enum Type
		{
			TypeNone,
			TypeLifePercent,
			TypeLife,
			TypeLifeMax,
			TypeLifeRecovery,
			TypeManaPercent,
			TypeMana,
			TypeManaMax,
			TypeManaRecovery,
			TypeStrength,
			TypeIntelligence,
			TypeDexterity,
			TypeWisdom,
			TypeVitality,
			TypePhysicalAttack,
			TypePhysicalDefense,
			TypeMagicalAttack,
			TypeMagicalDefense,
			TypeCriticalRate,
			TypeCriticalDamage,
			TypeAccuracy,
			TypeEvade,
			TypeMoveSpeed,
			TypeMagicalCriticalRate,
			TypeMagicalCriticalDamage,
		}
		mType;
		// 090204 LUJ, �� ���
		enum Operation
		{
			OperationNone,
			OperationMoreEqual,	// >=
			OperationLessEqual,	// <=
			OperationMore,		// >
			OperationLess,		// <
			OperationEqual,		// =
		}
		mOperation;
		// 090204 LUJ, �� ��
		float mValue;
	}
	mCondition;
	// 090204 LUJ, ��ġ ������ Ȯ����
	int Count;
	eBuffSkillActiveRule mRuleBattle;
	eBuffSkillActiveRule mRulePeace;
	eBuffSkillActiveRule mRuleMove;
	eBuffSkillActiveRule mRuleStop;
	eBuffSkillActiveRule mRuleRest;
	BOOL Die;
	BOOL NoUpdate;	//DB�� ������Ʈ ���� �ʴ´�. ���̵��� ���ŵǴ� ����� ���.

	// 090204 LUJ, Ÿ���� ��Ȯ�� ��
	eBuffSkillCountType CountType;
	// 080219 LUJ, �Ⱓ�� ��ų ����
	BOOL IsEndTime;
	// 080616 LUJ, �̺�Ʈ�� �ߵ��� ��ų
	DWORD mEventSkillIndex;
	// 080819 LUJ, ĳ�� ��ų ����
	BOOL IsCash;
	/// �Ҹ� ǰ��
	DWORD Item[2];
	WORD ItemCount[2];
};

struct ACTIVE_SKILL_INFO : public SKILL_INFO
{
	int	OperatorEffect;
	int	TargetEffect;
	int	SkillEffect;
	DWORD AnimationTime;
	DWORD CastingTime;
	WORD DelayType;
	DWORD DelayTime;
	DWORD CoolTime;
	WORD Target;
	WORD Range;
	TARGET_KIND	Area;
	eSkillAreaTarget AreaTarget;
	WORD AreaData;
	UNITKIND Unit;
	WORD UnitDataType;
	DWORD UnitData;
	/// ��ų Ȯ��
	WORD Success;
	WORD Cancel;
	WORD TrainPoint;
	MONEYTYPE TrainMoney;
	DWORD Buff[ MAX_BUFF_COUNT ];
	WORD BuffRate[ MAX_BUFF_COUNT ];
	DWORD RequiredBuff;
	BOOL IsMove;
	LEVELTYPE RequiredPlayerLevel;
	// 080602 LUJ, ��ų ��� �� �Ҹ�Ǵ� ������
	ITEMBASE mConsumeItem;
	// 080602 LUJ, ��ų ��� �� �Ҹ�Ǵ� ����Ʈ
	struct ConsumePoint
	{
		// 080602 LUJ, �Ҹ�Ǵ� ����Ʈ ����
		enum Type
		{
			TypeNone,
			TypeGuild,
			TypeFamily,
			TypePlayerKill,
			TypeExperience,
		}
		mType;
		DWORD mValue;
	}
	mConsumePoint;
	BOOL mIsBattle;
	BOOL mIsPeace;
	BOOL mIsMove;
	BOOL mIsStop;
};

#define MAX_SKILL_TREE 255

struct SKILL_TREE_INFO
{
	WORD	ClassIndex;
	WORD	Count;
	DWORD	SkillIndex[ MAX_SKILL_TREE ];
};

#include ".\TargetList\TargetList.h"
#include ".\TargetList\TargetListIterator.h"
struct MSG_SKILL_START_SYN : public MSGBASE
{
	DWORD SkillIdx;
	DWORD Operator;
	// 080602 LUJ, ����� ������ ����. �������� ��ȸ�Ϸ��� �ð��� ���� �ɸ��Ƿ� Ŭ���̾�Ʈ���� �Ѵ�
	ITEMBASE	mConsumeItem;
	VECTOR3 SkillDir;
	DWORD CoolTime;
	DWORD AreaData;
	DWORD Range;
	MAINTARGET MainTarget;
	CTargetList TargetList;

	void InitMsg(DWORD wSkillIdx,MAINTARGET* pMainTarget,const VECTOR3& dir,DWORD pOperator)
	{
		Category = MP_SKILL;
		Protocol = MP_SKILL_START_SYN;
		SkillIdx = wSkillIdx;
		TargetList.Clear();
		SkillDir = dir;
        Operator = pOperator;
		memcpy(&MainTarget,pMainTarget,sizeof(MAINTARGET));
	}
	WORD GetMsgLength()
	{
		return sizeof(MSG_SKILL_START_SYN) - sizeof(CTargetList) + TargetList.GetTotalMsgLen();
	}
};

// 071203 KTH --- ��ų�� ����ϴ� �������� ����Ҷ� �޴´�.
struct MSG_ITEM_SKILL_START_SYN : public MSGBASE
{
	MSG_SKILL_START_SYN	SkillStartSyn;
	POSTYPE	Pos;
	DWORD	Idx;
};

// 080602 LUJ, ������ ��� ��ų �ߵ�
struct MSG_ITEM_SKILL_START_TO_MAP : public MSGBASE
{
	SKILL_BASE	mSkill;
	DWORD		mGuildIndex;
	DWORD		mFamilyIndex;
};

// 090109 LUJ, ��ũ��Ʈ�� �˻��Ͽ� �����ϴ� ����ü
struct MSG_SCRIPT_CHECK : public MSGBASE
{
	ScriptCheckValue mCheckValue;
};

struct MSG_SKILLOBJECT_ADD2 : public MSGBASE
{
	SKILLOBJECT_INFO SkillObjectInfo;
	bool bCreate;

	void InitMsg(SKILLOBJECT_INFO* pSkillObjectInfo,bool Create)
	{
		Category = MP_SKILL;
		Protocol = MP_SKILL_SKILLOBJECT_ADD;
		memcpy(&SkillObjectInfo,pSkillObjectInfo,sizeof(SkillObjectInfo));
		SkillObjectInfo.StartTime = gCurTime - SkillObjectInfo.StartTime;
		bCreate = Create;
	}
};

struct MSG_SKILL_RESULT : public MSGBASE
{
	DWORD SkillObjectID;
	bool SkillDamageKind;
	CTargetList TargetList;

	void InitMsg(DWORD SOID)
	{
		Category = MP_SKILL;
		Protocol = MP_SKILL_SKILL_RESULT;
		SkillObjectID = SOID;
		TargetList.Clear();
	}
	WORD GetMsgLength()
	{
		// 080804 LUJ, ����ü ũ�⸦ �ùٷ� ��ȯ�� �� �ֵ��� ����
		return sizeof(MSG_SKILL_RESULT) - sizeof(CTargetList) + TargetList.GetTotalMsgLen();
	}

};

struct MSG_SKILL_UPDATE_TARGET : public MSGBASE
{
	DWORD SkillObjectID;
	CTargetList TargetList;

	void InitMsg(BYTE bProtocol, DWORD SOID)
	{
		Category = MP_SKILL;
		Protocol = bProtocol;
		SkillObjectID = SOID;
		TargetList.Clear();
	}
	WORD GetMsgLength()
	{
		// 080804 LUJ, ����ü ũ�⸦ �ùٷ� ��ȯ�� �� �ֵ��� ����
		return sizeof(MSG_SKILL_UPDATE_TARGET) - sizeof(CTargetList) + TargetList.GetTotalMsgLen();
	}
};

struct MUNPA_WAREHOUSE_ITEM_INFO
{
//	ITEMBASE WarehouseItem[MAX_MUNPA_WAREITEM_PERTAB_NUM];
	ITEMBASE WarehouseItem[TABCELL_GUILDWAREHOUSE_NUM];
	MONEYTYPE money;
};

struct SEND_MUNPA_WAREHOUSE_ITEM_INFO : public MSGBASE
{
	MUNPA_WAREHOUSE_ITEM_INFO info;
	WORD wOptionCount;
	WORD wRareOptionCount;
	WORD wPetInfoCount;
	BYTE bTabNum;

	CAddableInfoList AddableInfo;

	WORD GetSize()
	{
		return static_cast<WORD>(sizeof( SEND_MUNPA_WAREHOUSE_ITEM_INFO ) - sizeof(CAddableInfoList) + AddableInfo.GetInfoLength());
	}
};

struct STORAGELIST
{
	DWORD MaxCellNum;
	MONEYTYPE MaxMoney;
	MONEYTYPE BuyPrice;
};

struct SEND_STORAGE_ITEM_INFO : public MSGBASE
{
	MONEYTYPE money;
	ITEMBASE StorageItem[SLOT_STORAGE_NUM];

	WORD wOptionCount;
	WORD wRareOptionCount;
	WORD wPetInfoCount;

	CAddableInfoList AddableInfo;

	WORD GetSize()
	{
		return static_cast<WORD>(sizeof( SEND_STORAGE_ITEM_INFO ) - sizeof(CAddableInfoList) + AddableInfo.GetInfoLength());
	}
	
};

struct MSG_FRIEND_MEMBER_ADDDELETEID : public MSGBASE
{
	DWORD PlayerID;
	char Name[MAX_NAME_LENGTH+1];
};

struct FRIEND
{
	char Name[MAX_NAME_LENGTH+1];
	DWORD Id;
	BOOL IsLoggIn;
};

struct FRIEND_LIST
{
	FRIEND Friend[MAX_FRIEND_LIST];
	int totalnum;
};

struct MSG_FRIEND_LIST_DLG : public MSGBASE
{
	BYTE count;
	FRIEND FriendList[MAX_FRIEND_NUM];
	WORD GetMsgLength()
	{
		return static_cast<WORD>(sizeof(MSG_FRIEND_LIST_DLG) - (sizeof(FRIEND) * (MAX_FRIEND_NUM - count)));
	}
};

struct MSG_FRIEND_SEND_NOTE : public MSGBASE
{
	DWORD FromId;
	char FromName[MAX_NAME_LENGTH+1];
	char ToName[MAX_NAME_LENGTH+1];
	char Title[MAX_NOTE_TITLE+1];
	char Note[MAX_NOTE_LENGTH+1];
	WORD GetMsgLength()
	{
		return static_cast<WORD>(sizeof(MSG_FRIEND_SEND_NOTE) - (MAX_NOTE_LENGTH+1) + strlen(Note)+1);
	}
};

//100118 ONS ģ������ ���̾�α׿� PC���������� ����߰�
struct PCROOM_MEMBER
{
	DWORD	CharIndex;
	DWORD	CharClass;
	char	Name[MAX_NAME_LENGTH+1];
};

struct PCROOM_MEMBER_LIST
{
	int totalnum;
	PCROOM_MEMBER Member[MAX_PCROOM_MEMBER_LIST];
};

struct MSG_PCROOM_MEMBER_LIST : public MSGBASE
{	
	BYTE				count;
	WORD				PartyStage;
	WORD				StageMemberMin;
	PCROOM_MEMBER		PCRoomMemList[MAX_PCROOM_MEMBER_NUM];
	WORD GetMsgLength()
	{
		return static_cast<WORD>(sizeof(MSG_PCROOM_MEMBER_LIST) - (sizeof(PCROOM_MEMBER) * (MAX_PCROOM_MEMBER_NUM - count)));
	}
};

struct MSG_GUILD_SEND_NOTE : public MSGBASE
{
	DWORD FromId;
	char FromName[MAX_NAME_LENGTH+1];
	char Note[MAX_NOTE_LENGTH+1];
	WORD GetMsgLength()
	{
		return static_cast<WORD>(sizeof(MSG_GUILD_SEND_NOTE) - (MAX_NOTE_LENGTH+1) + strlen(Note)+1);
	}
};

struct MSG_FRIEND_SEND_NOTE_ID : public MSGBASE
{
	DWORD TargetID;
	char FromName[MAX_NAME_LENGTH+1];
	char Note[MAX_NOTE_LENGTH+1];
	WORD GetMsgLength()
	{
		return static_cast<WORD>(sizeof(MSG_FRIEND_SEND_NOTE_ID) - (MAX_NOTE_LENGTH+1) + strlen(Note)+1);
	}
};

struct FRIEND_NOTE
{
	//note�� ���� �ҷ��´�. 
	char FromName[MAX_NAME_LENGTH+1];
	DWORD NoteID;
	char SendDate[MAX_SENDDATE_LENGTH+1];
	BYTE bIsRead;

	char SendTitle[MAX_NOTE_TITLE+1];
	DWORD PackageItemIdx;
	DWORD PackageMoney;
};

struct MSG_FRIEND_NOTE_LIST : public MSGBASE
{
	FRIEND_NOTE NoteList[NOTENUM_PER_PAGE];
	BYTE TotalPage;
	WORD TotalMsgNum;
};

struct MSG_FRIEND_DEL_NOTE : public MSGBASE
{
	DWORD NoteID;
	BOOL bLast;
};

struct MSG_FRIEND_READ_NOTE : public MSGBASE
{
	char FromName[MAX_NAME_LENGTH+1];
	
	DWORD NoteID;
	WORD ItemIdx;
	DWORD dwPackageMoney;
	char Note[MAX_NOTE_LENGTH+1];
	WORD GetMsgLength()
	{
		return static_cast<WORD>(sizeof(MSG_FRIEND_READ_NOTE) - (MAX_NOTE_LENGTH+1) + strlen(Note)+1);
	}
};

struct MSG_FRIEND_READ_NOTE_WITH_PACKAGE : public MSG_ITEM_WITH_OPTION
{
	char FromName[MAX_NAME_LENGTH+1];
	
	DWORD NoteID;
	WORD ItemIdx;
	DWORD PackageItem;
	DWORD PackageMoney;
	char Note[MAX_NOTE_LENGTH+1];
	WORD GetMsgLength()
	{
		return static_cast<WORD>(sizeof(MSG_FRIEND_READ_NOTE_WITH_PACKAGE) - (MAX_NOTE_LENGTH+1) + strlen(Note)+1);
	}
};

//////////////////////////////////////////////////////////////////////////
//ä��
struct MSG_CHANNEL_INFO : public MSGBASE
{
	WORD PlayerNum[MAX_CHANNEL_NUM];
	char ChannelName[MAX_CHANNEL_NAME+1];
	DWORD dwUniqueIDinAgent;
	BYTE Count;
};

struct MSG_CHANNEL_INFO_MORNITOR : public MSGBASE
{
	MSG_CHANNEL_INFO	ChannelInfo;
	char				sServerIP[MAX_IPADDRESS_SIZE];
	WORD				wServerPort;
};
//////////////////////////////////////////////////////////////////////////


//------------ AUTOPATCH ���� �޽��� ----------------------
typedef struct tagServerTraffic : public MSGROOT
{
	LONG UserNum;
	int Returnkey;
	
}ServerTraffic;


//////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// ä�ð��ø޼��� //////////////////////////////////////

struct MSG_CHAT : public MSGBASE
{
	char	Name[MAX_NAME_LENGTH+1];
	char	Msg[MAX_CHAT_LENGTH+1];
	int		GetMsgLength() { return sizeof(MSG_CHAT) - MAX_CHAT_LENGTH + strlen(Msg); }
};

struct MSG_CHAT_WITH_SENDERID : public MSGBASE
{
	DWORD	dwSenderID;
	char	Name[MAX_NAME_LENGTH+1];
	char	Msg[MAX_CHAT_LENGTH+1];
	int		GetMsgLength() { return sizeof(MSG_CHAT_WITH_SENDERID) - MAX_CHAT_LENGTH + strlen(Msg); }
};

struct MSG_WHISPER : public MSGBASE
{
	DWORD	dwReceiverID;
	char	SenderName[MAX_NAME_LENGTH+1];
	char	ReceiverName[MAX_NAME_LENGTH+1];
	char	Msg[MAX_CHAT_LENGTH+1];
	int		GetMsgLength() { return sizeof(MSG_WHISPER) - MAX_CHAT_LENGTH + strlen(Msg); }
};

struct SEND_PARTY_CHAT : public MSGBASE
{
	DWORD	MemberID[MAX_PARTY_LISTNUM];
	BYTE	MemberNum;

	char	Name[MAX_NAME_LENGTH+1];
	char	Msg[MAX_CHAT_LENGTH+1];
	int		GetMsgLength() { return sizeof(SEND_PARTY_CHAT) - MAX_CHAT_LENGTH + strlen(Msg); }
};

// 070105 LYW --- Add struct for chatting of family system.
struct SEND_FAMILY_CHAT : public MSGBASE
{
	DWORD	MemberID[ MAX_FAMILY_LISTNUM ] ;
	BYTE	MemberNum ;

	char	Name[ MAX_NAME_LENGTH + 1 ] ;
	char	Msg[ MAX_CHAT_LENGTH + 1 ] ;
	int		GetMsgLength() { return sizeof( SEND_FAMILY_CHAT ) - MAX_CHAT_LENGTH + strlen( Msg ) ; }
} ;

struct MSG_CHAT_WORD : public MSGBASE
{
	WORD	wData;
	char	Msg[MAX_CHAT_LENGTH+1];
	int		GetMsgLength() { return sizeof(MSG_CHAT_WORD) - MAX_CHAT_LENGTH + strlen(Msg); }
};

enum eChatError
{
	CHATERR_ERROR,
	CHATERR_NO_NAME,
	CHATERR_NOT_CONNECTED,
	CHATERR_OPTION_NOWHISPER,
};

struct BATTLE_TEAMMEMBER_INFO
{
	DWORD MemberID;
	char MemberName[MAX_NAME_LENGTH+1];
	void set(DWORD id,char* name)
	{
		MemberID = id;
//		strcpy(MemberName,name);
		strncpy( MemberName, name, MAX_NAME_LENGTH );
		MemberName[MAX_NAME_LENGTH] = 0;

		MemberKillNum = 0;
		MemberDieNum = 0;
	}
	BYTE MemberKillNum;
	BYTE MemberDieNum;
};

struct BATTLE_INFO_BASE
{
	DWORD BattleID;
	BYTE BattleKind;
	BYTE BattleState;
	DWORD BattleTime;
};

#define MAX_BATTLE_INFO_LENGTH	2048
struct MSG_BATTLE_INFO : public MSGBASE
{
	BYTE BattleKind;
	char info[MAX_BATTLE_INFO_LENGTH];
	WORD msglen;

	int GetMsgLength()
	{
		return sizeof(MSGBASE) + msglen + sizeof(BYTE);
	}
};

struct MSG_BATTLE_TEAMMEMBER_ADDDELETE : public MSGBASE

{
	BATTLE_TEAMMEMBER_INFO Member;
	BYTE Team;
};


//==============
//For Showdown
struct MSG_BATTLE_SHOWDOWN_CREATESTAGE : public MSGBASE
{
	DWORD	dwBattleID;
	VECTOR3 vPosStage;		//�̰� ������. �Ʒ��� �ٲ���.
//	COMPRESSEDPOS vPosStage
};
//===============


struct MSG_MONEY : public MSGBASE
{
	MONEYTYPE dwTotalMoney;		// ��ü �ݾ�
	BYTE bFlag;					// MsgFlag
};

struct SEND_GUILD_CREATE_SYN : public MSGBASE
{
	char GuildName[MAX_GUILD_NAME+1];
	char Intro[MAX_GUILD_INTRO+1];
	int GetMsgLength()
	{
		return sizeof(SEND_GUILD_CREATE_SYN) - (MAX_GUILD_INTRO+1) + strlen(Intro)+1;
	}
};
struct GUILDINFO
{
	DWORD GuildIdx;
	DWORD MasterIdx;
	char MasterName[MAX_NAME_LENGTH+1];
	char GuildName[MAX_GUILD_NAME+1];
	DWORD UnionIdx;
	BOOL mIsUnionMaster;
	LEVELTYPE GuildLevel;
	MARKNAMETYPE MarkName;
};


struct GUILDMEMBERINFO
{
	DWORD MemberIdx;
	char MemberName[MAX_NAME_LENGTH+1];
	LEVELTYPE Memberlvl;
	BYTE Rank;
	BOOL bLogged;
    // 081031 LUJ, �� ��ȣ
	MAPTYPE	mMapType;
	// 080225 LUJ, ���� ���� ����
	BYTE	mJobGrade;
	BYTE	mJob[ MAX_JOB_GRADE ];
	BYTE	mRace;
};


struct SEND_GUILD_CREATE_ACK : public MSGBASE
{
	DWORD GuildIdx;
	char GuildName[MAX_GUILD_NAME+1];
};


struct SEND_GUILD_CREATE_NOTIFY : public MSGBASE
{
	GUILDINFO		GuildInfo;
	LEVELTYPE		MasterLvl;

	// 080225 LUJ, ������ ȸ�� ����
	GUILDMEMBERINFO	mMaster;
};


struct SEND_GUILD_MEMBER_INFO : public MSGBASE
{
	DWORD GuildIdx;
	char GuildName[MAX_GUILD_NAME+1];	
	GUILDMEMBERINFO MemberInfo;
};
struct SEND_GUILD_TOTALINFO : public MSGBASE
{
	GUILDINFO GuildInfo;
	//SW060719 ��������Ʈ
	//GUILDPOINT_INFO PointInfo;
	int membernum;
	GUILDMEMBERINFO MemberInfo[MAX_GUILD_MEMBER];
	void InitGuildInfo( const GUILDINFO& pInfo)
	{
		GuildInfo.GuildIdx = pInfo.GuildIdx;
		GuildInfo.GuildLevel = pInfo.GuildLevel;
		strcpy(GuildInfo.GuildName, pInfo.GuildName);
		GuildInfo.MasterIdx = pInfo.MasterIdx;
		// 06. 03. ���İ��� - �̿���
		//strcpy(GuildInfo.GuildNotice, pInfo.GuildNotice);
		GuildInfo.UnionIdx = pInfo.UnionIdx;
		strcpy(GuildInfo.MasterName, pInfo.MasterName);
		GuildInfo.MarkName = pInfo.MarkName;
	}
	//void InitPointInfo(GUILDPOINT_INFO* pInfo)
	//{
	//	memcpy(&PointInfo, pInfo, sizeof(GUILDPOINT_INFO));
	//}
	int GetMsgLength() { return sizeof(SEND_GUILD_TOTALINFO) - (MAX_GUILD_MEMBER-membernum)*sizeof(GUILDMEMBERINFO);	}
};
struct SEND_GUILD_INVITE : public MSGBASE
{
	DWORD MasterIdx;
	char MasterName[MAX_NAME_LENGTH+1];
	char GuildName[MAX_GUILD_NAME+1];
};

struct SEND_GUILD_NICKNAME : public MSGBASE
{
	DWORD TargetIdx;
	char NickName[MAX_GUILD_NICKNAME+1];
};

struct SEND_GUILD_NAME : public MSGBASE
{
	char GuildName[MAX_GUILD_NAME+1];
};

struct SEND_GUILDMEMBER_LOGININFO : public MSGBASE
{
	DWORD MemberIdx;
	BOOL bLogIn;
	// 081031 LUJ, �� ��ȣ
	MAPTYPE	mMapType;
};

struct SEND_GUILD_HUNTEDMOB_INFO : public MSG_DWORD2
{};

#define GUILDMARKBPP	2
#define GUILDMARKWIDTH	16
#define GUILDMARKHEIGHT	16
#define GUILDMARK_BUFSIZE	(GUILDMARKBPP*GUILDMARKWIDTH*GUILDMARKHEIGHT)
#define GUILDUNIONMARKHEIGHT	16
#define GUILDUNIONMARK_BUFSIZE	(GUILDMARKBPP*GUILDMARKWIDTH*GUILDUNIONMARKHEIGHT)

struct MSG_GUILDMARK_IMG : public MSGBASE
{
	DWORD GuildIdx;
	MARKNAMETYPE MarkName;
	char imgData[GUILDMARK_BUFSIZE];
};

struct MSG_GUILDUNIONMARK_IMG : public MSGBASE
{
	DWORD	dwMarkIdx;
	DWORD	dwGuildUnionIdx;
	char	imgData[GUILDUNIONMARK_BUFSIZE];
};

// 071015 ����, ���Ͱ��� ������ ���������� ���� ���� �̸��� �˷��ֵ��� MSG_GUILDWAR_INFO�� Ȯ���
struct MSG_GUILD_LIST : public MSGBASE
{
	DWORD mSize;

	struct Data : public GUILDINFO
	{
		char mUnionName[ MAX_NAME_LENGTH + 1 ];
	}
	mData[ 100 ];

	DWORD GetSize() const
	{
		return sizeof( MSG_GUILD_LIST ) - ( sizeof( mData ) / sizeof( Data ) - mSize ) * sizeof( Data );
	};
};


// 071017 ����, ������� ����� �ٸ� �ʼ����� �����Ѵ�.
struct MSG_GUILD_WAR_RESULT : public MSGBASE
{
	struct Data
	{
		DWORD	mGuildIndex;
		DWORD	mScoreValue;		// �ֽ� ����
		float	mScoreVariation;	// ��ȭ�� ��
	};
	
	Data	mWinner;
	Data	mLoser;
};

// 070801 ����. �÷��̾� ���� ��, ����� �� ������ �����Ͽ� ��´�
struct MSG_GUILD_DATA : public MSGBASE
{
	DWORD	mIndex;	
	DWORD	mLevel;
	MAPTYPE	mLocation;
	DWORD	mUnionIndex;
	DWORD	mScore;
	DWORD	mWarehouseSize;
	DWORD	mWarehouseRank;
	char	mName	[ MAX_GUILD_NAME	+ 1 ];
	char	mIntro	[ MAX_GUILD_INTRO	+ 1 ];
	char	mMark	[ MAX_NAME_LENGTH	+ 1 ];	// TODO: ���� ��ũ�� ���ؼ� ���� ���� �ʿ�
	char	mNotice	[ MAX_GUILD_NOTICE  + 1 ];

	DWORD			mMemberSize;
	GUILDMEMBERINFO	mMember[ MAX_GUILD_MEMBER ];

	DWORD GetSize() const
	{
		return sizeof( MSG_GUILD_DATA ) - ( sizeof( mMember ) / sizeof( GUILDMEMBERINFO ) - mMemberSize ) * sizeof( GUILDMEMBERINFO );
	}
};

struct MSG_GUILD_OPEN_WAREHOUSE : public MSGBASE
{
	DWORD	mPlayerIndex;
	char	mName[ MAX_NAME_LENGTH ];
	DWORDEX	mMoney;
	USHORT	mWarehouseSize;	// ��� â�� ũ��
	BYTE	mWarehouseRank;	// ��� â�� ���� ����
};


struct MSG_GUILD_SET_WAREHOUSE_RANK : public MSGBASE
{
	DWORD	mGuildIndex;
	BYTE	mRank;			// â�� ���� ���� ����
};


// 070808 ����, ��� ������ ����
struct MSG_GUILD_ITEM : public MSGBASE
{
	DWORD		mSize;
	ITEMBASE	mItem[ SLOT_GUILDWAREHOUSE_NUM ];

	DWORD GetSize() const
	{
		return sizeof( MSG_GUILD_ITEM ) - ( sizeof( mItem ) / sizeof( ITEMBASE ) - mSize ) * sizeof( ITEMBASE );
	}
};


// 070808 ����, ��� �������� �ɼ� ����
struct MSG_GUILD_ITEM_OPTION : public MSGBASE
{
	DWORD		mSize;
	ITEM_OPTION	mOption[ SLOT_GUILDWAREHOUSE_NUM ];

	DWORD GetSize() const
	{
		return sizeof( MSG_GUILD_ITEM_OPTION ) - ( sizeof( mOption ) / sizeof( ITEM_OPTION ) - mSize ) * sizeof( ITEM_OPTION );
	}
};

// 081031 LUJ, ��ȯ ��û
struct MSG_RECALL_SYN : public MSGBASE
{
	struct Player
	{
		DWORD	mIndex;
		char	mName[ MAX_NAME_LENGTH + 1 ];
		MAPTYPE	mMapType;
	};
	Player	mRecallPlayer;
	Player	mTargetPlayer;

	DWORD	mChannelIndex;
	DWORD	mRandomPositionRange;
	DWORD	mLimitMiliSecond;
	DWORD	mKey;
	DWORD	mSkillIndex;
	
	MSG_RECALL_SYN( MP_CATEGORY category = MP_MAX, BYTE protocol = 0 )
	{
		ZeroMemory( this, sizeof( *this ) );
		Category	= static_cast<BYTE>(category);
		Protocol	= protocol;
	}
};

// 081031 LUJ, ��ȯ �� ���� �޽���
struct MSG_RECALL_NACK : public MSGBASE
{
	enum Type
	{
		TypeNone,
		TypePlayerIsNotExist,
		TypePlayerIsNoMember,
		TypePlayerLocateOnBlockedMap,
		TypePlayerHasNoSkill,
		TypePlayerIsInCoolTimeForSkill,
		TypePlayerFailedUsingSkill,
		TypePlayerCannotRecallOneself,
		TypePlayerDied,
		TypePlayerIsDealing,
		TypePlayerIsExchanging,
		TypePlayerIsGuestInStall,
		TypePlayerIsOwnerInStall,
		TypePlayerIsInPlayerKillMode,
		TypePlayerIsOnShowdown,
		TypeTargetIsNotExist,
		TypeTargetIsNoMember,
		TypeTargetLocateOnBlockedMap,
		TypeTargetIsInPlayerKillMode,
		TypeTargetIsLooting,
		TypeTargetDoAutoNote,
		TypeTargetIsDealing,
		TypeTargetIsExchanging,
		TypeTargetDied,
		TypeTargetIsGuestInStall,
		TypeTargetIsOwnerInStall,
		TypeTargetIsOnShowdown,
		TypeTargetIsOnBattle,
		TypeSkillIsNotExist,
		TypeSkillNeedsMoreGuildScore,
		TypeSkillNeedsMoreFamilyPoint,
		TypeTimeOver,
	}
	mType;

	DWORD mKey;
	TCHAR mRecallPlayerName[ MAX_NAME_LENGTH + 1 ];
	TCHAR mTargetPlayerName[ MAX_NAME_LENGTH + 1 ];

	// 081203 LUJ, ����� ���ϱ� ���� Ÿ���� ������
	MSG_RECALL_NACK( MP_CATEGORY category, BYTE protocol, Type type, DWORD objectIndex, DWORD key )
	{
		ZeroMemory( this, sizeof( *this ) );
		Category	= static_cast<BYTE>(category);
		Protocol	= protocol;
		mType		= type;
		dwObjectID	= objectIndex;
		mKey		= key;
	}
};

#define FAMILYMARKBPP	2
#define FAMILYMARKWIDTH	16
#define FAMILYMARKHEIGHT	12
#define FAMILYMARK_BUFSIZE	(FAMILYMARKBPP*FAMILYMARKWIDTH*FAMILYMARKHEIGHT)

struct MSG_FAMILYMARK_IMG : public MSGBASE
{
	DWORD FamilyIdx;
	MARKNAMETYPE MarkName;
	char imgData[FAMILYMARK_BUFSIZE];
};

#pragma pack(1)

typedef DWORD	QSTATETYPE;
#define MAX_BIT ((sizeof(QSTATETYPE))*(8))

#define YEARTOSECOND	31536000
#define MONTHTOSECOND	2592000
#define DAYTOSECOND		86400
#define HOURTOSECOND	3600
#define MINUTETOSECOND	60

extern DWORD DayOfMonth[];
extern DWORD DayOfMonth_Yundal[];

//������������������������������������������������������������������
//��stTIME														  ��
//������������������������������������������������������������������
struct stTIME{
	QSTATETYPE		value;

	stTIME():value(0) {}

	void SetTime(DWORD time)	{	value = time;	}
	void SetTime(DWORD year, DWORD month, DWORD day, DWORD hour, DWORD minute, DWORD second)
	{
		value=0;
		QSTATETYPE ch=0;

		ch = year;
		value = (value | (ch<<28));
		ch = month;
		value = (value | (ch<<24));
		ch = day;
		value = (value | (ch<<18));
		ch = hour;
		value = (value | (ch<<12));
		ch = minute;
		value = (value | (ch<<6));
		ch = second;
		value = (value | ch);
	}

	//
	DWORD GetYear()		{	return value>>28;	}
	DWORD GetMonth()	{	DWORD msk = value<<4;		return msk>>28;		}
	DWORD GetDay()		{	DWORD msk = value<<8;		return msk>>26;		}
	DWORD GetHour()		{	DWORD msk = value<<14;		return msk>>26;		}
	DWORD GetMinute()	{	DWORD msk = value<<20;		return msk>>26;		}
	DWORD GetSecond()	{	DWORD msk = value<<26;		return msk>>26;		}
	inline BOOL	operator >(const stTIME& time);
	inline void operator +=(const stTIME& time);
	inline void operator -=(const stTIME& time);
	inline void operator =(const stTIME& time);
};

inline BOOL	 stTIME::operator >(const stTIME& time)
{
	BOOL bResult = FALSE;
	stTIME ctime = time;

	if( this->GetYear() == ctime.GetYear() )
	{
		if( this->GetMonth() > ctime.GetMonth() )
			bResult = TRUE;
		else if( this->GetMonth() == ctime.GetMonth() )
		{
			if( this->GetDay() > ctime.GetDay() )
				bResult = TRUE;
			else if( this->GetDay() == ctime.GetDay() )
			{
				if( this->GetHour() > ctime.GetHour() )
					bResult = TRUE;
				else if( this->GetHour() == ctime.GetHour() )
				{
					if( this->GetMinute() > ctime.GetMinute() )
						bResult = TRUE;
					else if( this->GetMinute() == ctime.GetMinute() )
						if( this->GetSecond() > ctime.GetSecond() )
							bResult = TRUE;
				}						
			}
		}
	}
	else if( this->GetYear() > ctime.GetYear() )
		bResult = TRUE;

	return bResult;
}

//---KES ETC 701020
inline void	 stTIME::operator +=(const stTIME& time)
{
	stTIME atime = time;
	int year, month, day, hour, minute, second, calcmonth;
	SYSTEMTIME systime;
	GetLocalTime(&systime);

	year = this->GetYear() + atime.GetYear();
	month = this->GetMonth() + atime.GetMonth();
	day = this->GetDay() + atime.GetDay();
	hour = this->GetHour() + atime.GetHour();
	minute = this->GetMinute() + atime.GetMinute();
	second = this->GetSecond() + atime.GetSecond();

	if( this->GetMonth() == 1 )			calcmonth = 11;
	else								calcmonth = this->GetMonth()-1;

	if(second >= 60)
	{
		++minute;
		second -= 60;
	}
	if(minute >= 60)
	{
		++hour;
		minute -= 60;
	}
	if(hour >= 24)
	{
		++day;
		hour -= 24;
	}
	if( (systime.wYear%4) == 0 )
	{
		if(day > (int)(DayOfMonth_Yundal[calcmonth]))
		{
			++month;
			day -= DayOfMonth_Yundal[calcmonth];
		}
	}
	else
	{
		if(day > (int)(DayOfMonth[calcmonth]))
		{
			++month;
			day -= DayOfMonth[calcmonth];
		}
	}
	if(month > 12)
	{
		++year;
		month -= 12;
	}
	
	this->SetTime(year, month, day, hour, minute, second);
}
inline void	 stTIME::operator -=(const stTIME& time)
{
	stTIME atime = time;
	int year, month, day, hour, minute, second, calcmonth;
	SYSTEMTIME systime;
	GetLocalTime(&systime);

	year = this->GetYear() - atime.GetYear();
	month = this->GetMonth() - atime.GetMonth();
	day = this->GetDay() - atime.GetDay();
	hour = this->GetHour() - atime.GetHour();
	minute = this->GetMinute() - atime.GetMinute();
	second = this->GetSecond() - atime.GetSecond();

	if( this->GetMonth() == 1 )			calcmonth = 11;
	else								calcmonth = this->GetMonth()-2;

	if(second < 0)
	{
		--minute;
		second += 60;
	}
	if(minute < 0)
	{
		--hour;
		minute += 60;
	}
	if(hour < 0)
	{
		--day;
		hour += 24;
	}
	if( (systime.wYear%4) == 0 )
	{
		if(day < 0)
		{
			--month;
			day += DayOfMonth_Yundal[calcmonth]; 
		}		
	}
	else
	{
		if(day < 0)
		{
			--month;
			day += DayOfMonth[calcmonth]; 
		}
	}
	if(month <= 0)
	{
		--year;
		month += 12;
	}
	
	this->SetTime(year, month, day, hour, minute, second);
}
//-------------------
inline void	 stTIME::operator =(const stTIME& time)
{
	stTIME atime = time;
	this->SetTime(atime.GetYear(), atime.GetMonth(), atime.GetDay(), atime.GetHour(), atime.GetMinute(), atime.GetSecond());
}
//



struct stPlayTime {
	DWORD value;

	stPlayTime::stPlayTime()
	{
		value = 0;
	}
	void GetTime(int& Year, int& Day, int& Hour, int& Minute, int& Second)
	{
		int mv = 0;

		Year = value/YEARTOSECOND;
		mv = value%YEARTOSECOND;

		Day = mv/DAYTOSECOND;
		mv = mv%DAYTOSECOND;

		Hour = mv/HOURTOSECOND;
		mv = mv%HOURTOSECOND;

		Minute = mv/MINUTETOSECOND;
		Second = mv%MINUTETOSECOND;		
	}
};


// LBS 04.01.06----------------------------------
typedef struct tagField{
	DWORD b0:1; 
	DWORD b1:1;
	DWORD b2:1;
	DWORD b3:1;
	DWORD b4:1;
	DWORD b5:1;
	DWORD b6:1;
	DWORD b7:1;

	DWORD b8:1;
	DWORD b9:1;
	DWORD b10:1;
	DWORD b11:1;
	DWORD b12:1;
	DWORD b13:1;
	DWORD b14:1;
	DWORD b15:1;

	DWORD b16:1;
	DWORD b17:1;
	DWORD b18:1;
	DWORD b19:1;
	DWORD b20:1;
	DWORD b21:1;
	DWORD b22:1;
	DWORD b23:1;

	DWORD b24:1;
	DWORD b25:1;
	DWORD b26:1;
	DWORD b27:1;
	DWORD b28:1;
	DWORD b29:1;
	DWORD b30:1;
	DWORD b31:1;

} stFIELD, *pFIELD;

typedef union tagQuestFlag{
	QSTATETYPE	value;
	stFIELD		flag;

	tagQuestFlag():value(0) {}

	void SetField(BYTE bit, BOOL bSetZero=FALSE) // nBit�� ��ȿ ���ڴ� DWORD�ΰ��1 ~ 32 �̴�
	{
		if( bit  < 1 || bit > MAX_BIT ) return;

		QSTATETYPE ch;
		bSetZero ? ch=1 : ch=0;
		value = (value | (ch<<(MAX_BIT- bit)));
	}

	BOOL IsSet(BYTE bit)
	{
		// ������ �������� ��� ���� ����Ǿ��ٰ� �˸���.
		if( bit  < 1 || bit > MAX_BIT ) return TRUE;

		QSTATETYPE ch = 1;
		return (value & (ch<<(MAX_BIT-bit)))? TRUE:FALSE;
	}

} QFLAG, *pQFLAG;


struct QBASE
{
	WORD		QuestIdx;
	QSTATETYPE	state;
};

struct QMBASE
{
	WORD		QuestIdx;	
	QFLAG		state;
	DWORD		EndParam;
	__time64_t	registTime;
	BYTE		CheckType;
	DWORD		CheckTime;
};

// 071023 LYW --- CommonStruct : Add struct for tutorial info.
struct TIBASE
{
	int			nTutorialIdx ;
	BOOL		IsComplete ;
} ;

struct QSUBASE
{
	WORD		QuestIdx;
	WORD		SubQuestIdx;
	QSTATETYPE	state;
	QSTATETYPE	time;
};

struct QITEMBASE
{
	DWORD		ItemIdx;
	WORD		Count;
	WORD		QuestIdx;
};

struct SEND_QUEST_IDX : public MSGBASE
{
	WORD		MainQuestIdx;
	WORD		SubQuestIdx;
	DWORD		dwFlag;
	DWORD		dwData;
	__time64_t	registTime;
};

// 100414 ONS  ����Ʈ �̺�Ʈ �޼����� ���ú�������� ������ �߰��Ѵ�.
struct SEND_QUEST_REQUITAL_IDX : public SEND_QUEST_IDX
{
	DWORD		dwRequitalIdx;
	DWORD		dwRequitalCount;
};

struct SEND_QUESTITEM_IDX : public MSGBASE
{
	DWORD	dwItemIdx;
	DWORD	dwItemNum;
};

struct SEND_QUEST_TOTALINFO : public MSGBASE
{
	int GetSize() {
		return (sizeof(SEND_QUEST_TOTALINFO) - sizeof(QBASE)*(50 - wCount));
	}

	WORD	wCount;
	QBASE	QuestList[50];
};

struct SEND_MAINQUEST_DATA : public MSGBASE
{
	int GetSize() {
		return (sizeof(SEND_MAINQUEST_DATA) - sizeof(QMBASE)*(100 - wCount));
	}

	WORD	wCount;
	QMBASE	QuestList[100];
};

// 071023 LYW --- CommonStruct : Add struct for tutorial data.
struct SEND_TUTORIAL_DATA : public MSGBASE
{
	int GetSize() {
		return (sizeof(SEND_TUTORIAL_DATA) - sizeof(TIBASE)*(100 - wCount));
	}

	WORD	wCount;
	TIBASE	TutorialList[100];
};

// 071106 LYW --- CommonStruct : Add structure for send message about update to default stats.
struct SEND_DEFAULT_STATS : public MSGBASE
{
	int nStr ;
	int nDex ;
	int nVit ;
	int nInt ;
	int nWis ;
	int nPoint ;

	int nErrorCode ;
};

struct SEND_SUBQUEST_DATA : public MSGBASE
{
	int GetSize() {
		return (sizeof(SEND_SUBQUEST_DATA) -sizeof(QSUBASE)*(100-wCount));
	}

	WORD	wCount;
	QSUBASE	QuestList[100];
};

struct SEND_SUBQUEST_UPDATE : public MSGBASE
{
	WORD	wQuestIdx;
	WORD	wSubQuestIdx;
	DWORD	dwMaxCount;
	DWORD	dwData;
	DWORD	dwTime;
};

struct SEND_QUESTITEM : public MSGBASE
{
	int GetSize() {
		return (sizeof(SEND_QUESTITEM)-sizeof(QITEMBASE)*(100-wCount));
	}

	WORD		wCount;
	QITEMBASE	ItemList[100];
};

struct QUESTINFO {
	DWORD	m_dwIdx;		// Quest idx
	QFLAG	m_flag;			// Quest State Value
};

// PartyIcon
struct MoveCoord
{
	WORD posX;
	WORD posZ;

	MoveCoord::MoveCoord()
	{
		posX = posZ = 0;
	}
};

struct PARTYICON_MOVEINFO
{
	DWORD	dwMoverID;
	BYTE	tgCount;
	WORD	KyungGongIdx;
	float	fSpeed;
	MoveCoord	cPos;
	MoveCoord	tgPos[MAX_CHARTARGETPOSBUF_SIZE];

	PARTYICON_MOVEINFO::PARTYICON_MOVEINFO()
	{
		dwMoverID	= 0;
		tgCount		= 0;
		KyungGongIdx = 0;
		fSpeed = 0.0f;
		memset(&cPos, 0, sizeof(MoveCoord));
		memset(tgPos, 0, sizeof(MoveCoord)*MAX_CHARTARGETPOSBUF_SIZE);
	}

	int GetSize()
	{
		return sizeof(PARTYICON_MOVEINFO)-sizeof(MoveCoord)*(MAX_CHARTARGETPOSBUF_SIZE-tgCount);
	}
};

struct SEND_PARTYICON_MOVEINFO : public MSGBASE
{
	PARTYICON_MOVEINFO MoveInfo;

	int GetSize()
	{
		return sizeof(MSGBASE)+MoveInfo.GetSize();
	}
};

struct SEND_PARTYICON_REVIVE : public MSGBASE
{
	DWORD		dwMoverID;
	MoveCoord	Pos;
};

struct SEND_SHOPITEM_INFO : public MSGBASE
{
	WORD		ItemCount;
	ITEMBASE	Item[SLOT_SHOPITEM_NUM];

	SEND_SHOPITEM_INFO::SEND_SHOPITEM_INFO() {
		ItemCount = 0;
		memset(Item, 0, sizeof(ITEMBASE)*SLOT_SHOPITEM_NUM);
	}
	int GetSize() {
		return (sizeof(SEND_SHOPITEM_INFO) - sizeof(ITEMBASE)*(SLOT_SHOPITEM_NUM-ItemCount));
	}
};

struct CHARACTERCHANGE_INFO
{
	BYTE	Gender;
	BYTE	FaceType;
	BYTE	HairType;
	float	Height;
	float	Width;

	CHARACTERCHANGE_INFO::CHARACTERCHANGE_INFO()
	{
		Gender = 0;
		FaceType = 0;
		HairType = 0;
		Height = 0;
		Width = 0;
	}
};

// 080414 LUJ, �ܾ� ���� �޽���
struct MSG_CHARACTER_CHANGE : public MSGBASE
{
	CHARACTERCHANGE_INFO	mInfo;
	ITEMBASE				mItem;
};

struct SEND_SHOUTBASE_ITEMINFO : public MSGBASE
{	
	DWORD dwItemIdx;
	DWORD dwItemPos;
	WORD wItemSupplyValue;
	char Name[MAX_NAME_LENGTH+1];
	char Msg[MAX_NAME_LENGTH+MAX_SHOUT_LENGTH+1];

	SEND_SHOUTBASE_ITEMINFO::SEND_SHOUTBASE_ITEMINFO()
	{
		ZeroMemory(
			this,
			sizeof(*this));
	}

	int	GetMsgLength() { return sizeof(SEND_SHOUTBASE_ITEMINFO) - MAX_SHOUT_LENGTH + strlen(Msg); }
};

enum eCheatEvent
{
	eEvent_None=0,
	eEvent_ExpRate,
	eEvent_ItemRate,
	eEvent_MoneyRate,
	eEvent_DamageReciveRate,
	eEvent_DamageRate,
	eEvent_ManaRate,
	eEvent_UngiSpeed,
	eEvent_PartyExpRate,
	eEvent_AbilRate,
	eEvent_GetMoney,
	eEvent_SkillExp,

	eEvent_Max,
};


struct MSG_GM_MOVE : public MSGBASE
{
	char Name[MAX_NAME_LENGTH+1];
	DWORD dwData1;
	DWORD dwData2;
	DWORD dwGMIndex;
	char strGMID[MAX_NAME_LENGTH+1];
};

struct MSG_EVENTNOTIFY_ON : public MSGBASE
{
	char strTitle[32];
	char strContext[128];
	BYTE EventList[eEvent_Max];
};

struct GAMEEVENT 
{
	void Init(BYTE kind, DWORD data)
	{
		bEventKind = kind;
		dwData = data;
	}

	BYTE bEventKind;			// �̺�Ʈ�� ����
	DWORD dwData;				// ������1
};

struct REGISTEDGUILDINFO
{
	DWORD			GuildIdx;
	char			ImageData[GUILDMARK_BUFSIZE];
	char			GuildName[MAX_GUILD_NAME+1];
	DWORD			MarkExist;
	BYTE			Position;
	BYTE			Ranking;
	BYTE			ProcessTournament;
	DWORD			BattleID;
};

struct GUILDRANKING
{
	DWORD			GuildIdx;
	BYTE			Ranking;
};


struct GTTEAM_MEMBERINFO
{
	DWORD		TeamGuildIdx[2];
	DWORD		TeamLevel[2];
	WORD		TeamMember[2];
	WORD		TeamDieMember[2];
};

struct SEND_GTWINLOSE : public MSGBASE
{
	WORD		TournamentCount;
	DWORD		GuildIdx;
	char		GuildName[MAX_GUILD_NAME+1];
	BYTE		bWin;
	DWORD		Param;			// ��� �̱�� ���°�, client-��� ����ΰ�
	BYTE		bUnearnedWin;	// �������ΰ�
};

struct SEND_BATTLEJOIN_INFO : public MSGBASE
{
	DWORD		AgentIdx;
	DWORD		UserLevel;
	DWORD		GuildIdx;
	DWORD		BattleIdx;
	DWORD		ReturnMapNum;
};


struct SEND_BATTLESCORE_INFO : public MSGBASE
{
	char		GuildName[2][MAX_GUILD_NAME+1];
	DWORD		Players[2][MAX_GTOURNAMENT_PLAYER];
	DWORD		PlayerScore[2][MAX_GTOURNAMENT_PLAYER];
	DWORD		Score[2];
	DWORD		FightTime;
	DWORD		EntranceTime;
	DWORD		State;
	BYTE		Team;
	WORD		GoalScore;

	SEND_BATTLESCORE_INFO::SEND_BATTLESCORE_INFO()
	{
		ZeroMemory(
			this,
			sizeof(*this));
	}
};


struct SEND_REGISTEDGUILDINFO : public MSGBASE
{
	BYTE				MaxTeam;
	BYTE				Count;
	WORD				CurTournament;
	WORD				CurRound;
	WORD				CurState;
	DWORD				PlayerID;
	DWORD				RemainTime;
	REGISTEDGUILDINFO	GInfo[MAXGUILD_INTOURNAMENT];

	SEND_REGISTEDGUILDINFO::SEND_REGISTEDGUILDINFO()
	{
		Count = 0;
		CurTournament = 0;
		PlayerID = 0;
		memset( GInfo, 0, sizeof(REGISTEDGUILDINFO)*MAXGUILD_INTOURNAMENT );
	}
	DWORD SEND_REGISTEDGUILDINFO::GetSize()
	{
		return (sizeof(SEND_REGISTEDGUILDINFO) - sizeof(REGISTEDGUILDINFO)*(MAXGUILD_INTOURNAMENT-Count));
	}
};

struct SEND_GUILDRANKING : public MSGBASE
{
	BYTE			Count;
	GUILDRANKING	Ranking[MAXGUILD_INTOURNAMENT];

	SEND_GUILDRANKING::SEND_GUILDRANKING()
	{
		Count = 0;
		memset( Ranking, 0, sizeof(GUILDRANKING)*MAXGUILD_INTOURNAMENT );
	}
	DWORD SEND_GUILDRANKING::GetSize()
	{
		return (sizeof(SEND_GUILDRANKING) - sizeof(GUILDRANKING)*(MAXGUILD_INTOURNAMENT-Count));
	}
};


struct GTBATTLE_INFO
{
	BYTE	Group;		// A, B, C, D
	DWORD	BattleID;
	char	GuildName1[MAX_GUILD_NAME+1];
	char	GuildName2[MAX_GUILD_NAME+1];
};


struct SEND_GTBATTLE_INFO : public MSGBASE
{
	BYTE			Count;
	BYTE			PlayOff;		// ����-1, ����-2
	GTBATTLE_INFO	BattleInfo[MAXGUILD_INTOURNAMENT/2];
	
	SEND_GTBATTLE_INFO::SEND_GTBATTLE_INFO()
	{
		Count = 0;
		memset( BattleInfo, 0, sizeof(GTBATTLE_INFO)*(MAXGUILD_INTOURNAMENT/2) );
	}
	DWORD SEND_GTBATTLE_INFO::GetSize()
	{
		return (sizeof(SEND_GTBATTLE_INFO) - sizeof(GTBATTLE_INFO)*(MAXGUILD_INTOURNAMENT/2-Count));
	}
};


struct GTBATTLE_INFOBROAD
{
	DWORD	BattleID;
	DWORD	Group;			// �����
	DWORD	GuildIdx[2];
};


struct SEND_GTBATTLE_INFOBROAD : public MSGBASE
{
	BYTE					Count;
	DWORD					CurTournament;
	GTBATTLE_INFOBROAD		BattleInfo[MAXGUILD_INTOURNAMENT/2];
	
	SEND_GTBATTLE_INFOBROAD::SEND_GTBATTLE_INFOBROAD()
	{
		Count = 0;
		memset( BattleInfo, 0, sizeof(GTBATTLE_INFOBROAD)*MAXGUILD_INTOURNAMENT/2 );
	}
	DWORD SEND_GTBATTLE_INFOBROAD::GetSize()
	{
		return (sizeof(SEND_GTBATTLE_INFOBROAD) - sizeof(GTBATTLE_INFOBROAD)*(MAXGUILD_INTOURNAMENT/2-Count));
	}
};


struct MSG_GTEDIT_PLAYERS : public MSGBASE
{
	DWORD dwRemainTime;
	DWORD dwPlayers[MAX_GTOURNAMENT_PLAYER];

	MSG_GTEDIT_PLAYERS(){dwRemainTime=0; memset(dwPlayers, 0, sizeof(dwPlayers));}
};

struct MSG_GTEDIT_PLAYERS_DWORD3 : public MSGBASE
{
	DWORD dwRemainTime;
	DWORD dwPlayers[MAX_GTOURNAMENT_PLAYER];
	DWORD dwData1;
	DWORD dwData2;
	DWORD dwData3;

	MSG_GTEDIT_PLAYERS_DWORD3(){dwRemainTime=0; memset(dwPlayers, 0, sizeof(dwPlayers));}
};


#define		MAX_GUILD_UNION_NUM		7

struct sGUILDIDXNAME
{
	DWORD	dwGuildIdx;
	char	sGuildName[MAX_GUILD_NAME+1];
};

struct SEND_GUILD_UNION_INFO : public MSGBASE
{
	DWORD			dwGuildUnionIdx;
	char			sGuildUnionName[MAX_GUILD_NAME+1];
	DWORD			dwGuildUnionMarkIdx;
	int				nMaxGuildNum;
	sGUILDIDXNAME	GuildInfo[MAX_GUILD_UNION_NUM];
	
	SEND_GUILD_UNION_INFO()
	{
		dwGuildUnionIdx = 0;
		memset( sGuildUnionName, 0, MAX_GUILD_NAME+1 );
		dwGuildUnionMarkIdx = 0;
		memset( GuildInfo, 0, sizeof(sGUILDIDXNAME)*MAX_GUILD_UNION_NUM );
		nMaxGuildNum = 0;
	}
	void AddGuildInfo( DWORD dwGuildIdx, char* pGuildName )
	{
		GuildInfo[nMaxGuildNum].dwGuildIdx = dwGuildIdx;
		strncpy( GuildInfo[nMaxGuildNum].sGuildName, pGuildName, MAX_GUILD_NAME+1 );
		++nMaxGuildNum;
	}
	int GetSize()
	{
		return ( sizeof(SEND_GUILD_UNION_INFO) - sizeof(sGUILDIDXNAME)*(MAX_GUILD_UNION_NUM-nMaxGuildNum) );
	}
};

struct SEND_CHEAT_ITEM_OPTION : public MSGBASE
{
	SEND_CHEAT_ITEM_OPTION(){memset(this, 0, sizeof(SEND_CHEAT_ITEM_OPTION));}
	POSTYPE bPosition;
	DWORD wItemIdx;
	WORD wOptionKind;

	ITEM_OPTION		OptionInfo;
};

struct MAPOBJECT_INFO
{
	DWORD	Life;
	DWORD	MaxLife;
	DWORD	PhyDefence;
	float	Radius;
};

struct AbnormalStatus
{
	// 080807 LUJ, �̻� ���� ��ø�� ��, �����Ǹ� �ϰ��� �����Ǵ� ������ �����ϱ� ���� ī�����ϵ��� ��
	int	IsParalysis;
	int	IsStun;
	int	IsSlip;
	int	IsFreezing;
	int	IsStone;
	int	IsSilence;
	int	IsGod;
	int	IsBlockAttack;
	int	IsPoison;
	int	IsShield;
	int	IsBleeding;
	int	IsBurning;
	int	IsHolyDamage;
	int	IsMoveStop;
	int IsUnableUseItem;
	int IsUnableBuff;
	float Attract;
	float Poison;
	float Shield;
	float Bleeding;
	float Burning;
	float HolyDamage;
};

struct Status
{
	float	Str;
	float	Dex;
	float	Vit;
	float	Int;
	float	Wis;

	float	PhysicAttack;
	float	PhysicDefense;
	float	MagicAttack;
	float	MagicDefense;
	
	float	Accuracy;
	float	Avoid;
	float	CriticalRate;
	float	Range;
	float	MagicCriticalRate;
	float	CriticalDamage;
	float	MagicCriticalDamage;
	float	MoveSpeed;
	float	Block;
	float	CoolTime;
	float	CastingProtect;

	float	AddDamage;
	float	ReduceDamage;

	float	MaxLife;
	float	MaxMana;
	float	LifeRecoverRate;
	float	ManaRecoverRate;
	float	LifeRecover;
	float	ManaRecover;

	float	Reflect;
	float	Absorb;
	float	DamageToLife;
	float	DamageToMana;
	float	GetLife;
	float	GetMana;
	float	GetExp;
	float	GetGold;
	
	float	Attrib_None;
	float	Attrib_Earth;
	float	Attrib_Water;
	float	Attrib_Divine;
	float	Attrib_Wind;
	float	Attrib_Fire;
	float	Attrib_Dark;
	
	float	Sword;
	float	Mace;
	float	Axe;
	float	Staff;
	float	Bow;
	float	Gun;
	float	Dagger;
	float	Spear;
	float	OneHanded;
	float	TwoHanded;
	float	TwoBlade;
	float	Robe;
	float	LightArmor;
	float	HeavyArmor;
	float	Shield;

	float	NormalSpeedRate;
	float	PhysicSkillSpeedRate;
	float	MagicSkillSpeedRate;

	// 071203 KTH -- ��ų ���� ���� ���� %
	float	DecreaseManaRate;
	// 071204 KTH -- ����� ��� %
	float	IncreaseDropRate;
	BOOL	IsProtectExp;

	Status() {memset( this, 0, sizeof( Status ) );}
};

struct	DATE_MATCHING_SEARCH_OUTPUT
{
	char		szName[MAX_NAME_LENGTH+1];									// �˻��� ����.
	DWORD		dwGoodFeelingScore;						// ȣ������
	DWORD		dwScore;								// �˻� ��� ����.
	DWORD		dwAge;									// ����.
	DWORD		dwUserID;								// ���� ID
};

#define	DATE_MATCHING_LIST_MAX	20

struct	MSG_DATEMATCHING_SEARCH_RESULT: public MSGBASE				// s2c ����Ʈ ��� ����Ʈ�� ������.
{
	DWORD	dwResultCount;
	DATE_MATCHING_SEARCH_OUTPUT		pList[DATE_MATCHING_LIST_MAX];
	int	GetMsgLength(){ return	sizeof(MSG_DATEMATCHING_SEARCH_RESULT) - (sizeof(DATE_MATCHING_SEARCH_OUTPUT)*DATE_MATCHING_LIST_MAX) + sizeof(DATE_MATCHING_SEARCH_OUTPUT)*dwResultCount;}
};

struct	MSG_DATEMATCHING_REQUEST_TARGET_INFO: public MSGBASE	// c2s
{
//	char	szTargetName[MAX_CHAT_LENGTH+1];
	DWORD	dwTargetUserID;							// ã������ UserID;
};

struct	MSG_DATEMATCHING_TARGET_INFO: public MSGBASE			// s2c
{
	DATE_MATCHING_INFO	TargetInfo;
};

struct	MSG_DATEMATCHING_MODIFY_INTRODUCE:	public MSGBASE		// c2s
{
	// �� �Ұ� ������ �������� ����.
	char	szIntroduce[MAX_INTRODUCE_LEN+1];
};

// desc_hseos_���͹���01
// S ���͹��� �߰� added by hseos 2007.05.23
typedef struct
{
	DWORD			nPlayTimeTick;
	DWORD			nPlayTime;
	DWORD			nKillMonsterNum;
	DWORD			nPlayTimeTotal;
	DWORD			nKillMonsterNumTotal;
} stMONSTERMETER;
// E ���͹��� �߰� added by hseos 2007.05.23

// desc_hseos_�󱼱�ü01
// S �󱼱�ü �߰� added by hseos 2007.06.07	2007.06.08
enum
{
	FACE_ACTION_KIND_EYE_BLINK = 0,
	FACE_ACTION_KIND_EYE_CHATTING_CMD, 
};

enum
{
	PLAYER_FACE_SHAPE_EYE_OPEN = 0,
	PLAYER_FACE_SHAPE_EYE_CLOSE,
};

typedef struct
{
	DWORD	nFaceShapeTimeTick;					// �� ��� �ð� ƽ
	int		nFaceShapeState;					// �� ��� ���� ( �ִϸ��̼ǽ� �ʿ�. - ��������)
	int		nFaceActionKind;					// �� ���� ���� ( 0: �⺻ ��������, 1: ä��â ���ɾ� �� ����)
} stFACEACTION;
// E �󱼱�ü �߰� added by hseos 2007.06.07	2007.06.08

// desc_hseos_�йи�01
// S �йи� �߰� added by hseos 2007.07.09
// ..�������� ����� ����� ����
enum RESULT
{
	RESULT_OK = 0,								// ����
	RESULT_OK_02,
	RESULT_OK_03,
	RESULT_OK_04,
	RESULT_OK_05,
	RESULT_OK_06,

	RESULT_FAIL_INVALID,						// ����(�ý��� ������ ����� ����)

	RESULT_FAIL_01,								// ����01
	RESULT_FAIL_02,								// ����02
	RESULT_FAIL_03,								// ����03
	RESULT_FAIL_04,								// ����04
	RESULT_FAIL_05,								// ����05
	RESULT_FAIL_06,								// ����06
	RESULT_FAIL_07,								// ����07
	RESULT_FAIL_08,								// ����08
	RESULT_FAIL_09,								// ����09
	RESULT_FAIL_10,								// ����10
	RESULT_FAIL_11,								// ����11
	RESULT_FAIL_12,								// ����12
	RESULT_FAIL_13,								// ����13
	RESULT_FAIL_14,								// ����14
};
// E �йи� �߰� added by hseos 2007.07.09


//---KES Distribute Encrypt 071003
struct MSG_DIST_CONNECTSUCCESS : public MSG_DWORD
{
	HselInit eninit;
	HselInit deinit;
};

// 071226 KTH -- �̸� ���� ������ ���
struct MSG_CHANGE_CHARACTER_NAME_SYN : public MSG_ITEM_USE_SYN
{
	char	changeName[17];
};

struct MSG_CHANGE_CHARACTER_NAME_ACK : public MSG_CHANGE_CHARACTER_NAME_SYN
{
};

struct MSG_CHANGE_CHARACTER_NAME_AGENT : public MSGBASE
{
	char changeName[17];
	int CharacterIdx;
	int UserIdx;
};

struct MSG_AUTOLIST_ADD : public MSGBASE
{
	char Name[MAX_NAME_LENGTH+1];
	char Date[20];		//2008.01.25 00:00:00  (19����)
};

struct MSG_AUTOLIST_ALL : public MSGBASE
{
	struct AUTOLIST_ROW
	{
		char Name[MAX_NAME_LENGTH+1];
		char Date[20];
	};

	int nCount;
	AUTOLIST_ROW	row[100];

	int	GetMsgLength()
	{
		return sizeof(MSG_AUTOLIST_ALL) - ( sizeof(AUTOLIST_ROW) * ( 100-nCount ) );
	}
};

struct MSG_AUTONOTE_IMAGE : public MSGBASE
{
	BYTE Image[128*32*3];
};





// 080404 LYW --- CommonStruct : ä�ù� �ý��ۿ� �ʿ��� ������ ����ü��.

#define MAX_USER_BUCKETCNT			100								// ���� ���� �ؽ� ���̺��� �ִ� ��Ŷ ��.
#define MAX_ROOM_COUNT				250								// �ִ� ���� ä�ù� ��.
#define MAX_USER_PER_ROOM			20								// �� ä�ù� �� �ִ� ������ ��.

#define TITLE_SIZE					64								// ���� ������.

#define ROOM_COUNT_PER_PAGE			19								// ������ �� ä�ù� ��.

#define CHATROOM_DELAY_TIME			1500							// ������ ���� �� �� �ִ� ������ �ð�.

#define SECRET_CODE_SIZE			20								// ��ȣ ������.

struct ST_CR_USER													// ���� ����.
{
	BYTE byIdx ;													// �������� ä�ù� �ε���.

	DWORD dwUserID ;												// ���� �ε���.
	DWORD dwPlayerID ;												// ĳ���� �ε���.

	//WORD  wClassIdx ;												// Ŭ���� �ε���.

	BYTE  byLevel ;													// ĳ���� ����.
	BYTE  byMapNum ;												// ���� ĳ���� ��.

	char  name[ MAX_NAME_LENGTH+1 ] ;								// ĳ���� �̸�.

	DWORD dwConnectionIdx ;
} ;

struct MSG_CR_USER : public MSGBASE									// ���� ���� ���ۿ� �޽���.
{
	ST_CR_USER user ;												// ���� ����.
} ;

struct ST_CR_ROOM_SRV												// ���� ������ ä�ù� ����.
{
	BYTE byIdx ;													// ä�ù� �ε���.

	char code[SECRET_CODE_SIZE+1] ;									// ä�ù� ��ȣ.

	DWORD dwOwnerIdx ;												// ���� �ε���.

	BYTE bySecretMode ;												// ����/����� ���.
	BYTE byRoomType ;												// ä�ù� �з�.

	BYTE byCurGuestCount ;											// ���� ���� ���� ��.
	BYTE byTotalGuestCount ;										// �ִ� ���� ���� ��.

	char title[ TITLE_SIZE+1 ] ;									// ä�ù� ����.

	DWORD dwUser[MAX_USER_PER_ROOM] ;								// ������ ���̵�.
} ;

struct ST_CR_ROOM_CLT												// Ŭ���̾�Ʈ ���� ä�ù� ����.
{
	BYTE byIdx ;													// ä�ù� �ε���.

	BYTE bySecretMode ;												// ����/����� ���.
	BYTE byRoomType ;												// ä�ù� �з�.

	BYTE byCurGuestCount ;											// ���� ���� ���� ��.
	BYTE byTotalGuestCount ;										// �ִ� ���� ���� ��.

	char title[ TITLE_SIZE+1 ] ;									// ä�ù� ����.
} ;

struct MSG_CR_ROOMINFO : public MSGBASE								// �ϳ��� ä�ù� ������ ��� ����ü.
{
	BYTE byCount ;													// ���� ī��Ʈ.

	ST_CR_ROOM_SRV room ;											// ������.
	ST_CR_USER user[MAX_USER_PER_ROOM] ;							// ������ ����.

	int GetMsgSize()
	{
		return sizeof(MSG_CR_ROOMINFO) - (MAX_USER_PER_ROOM-byCount)*sizeof(ST_CR_USER) ;
	}
} ;

struct MSG_CR_ROOMLIST : public MSGBASE								// ä�ù��� ���� ����Ʈ ����.
{
	BYTE byCurPage ;												// ���� ������.
	BYTE byTotalPage ;												// �� ������.

	BYTE byRoomType ;												// ä�ù� Ÿ��.

	BYTE byCount ;													// ä�ù� ����.

	ST_CR_ROOM_CLT room[ROOM_COUNT_PER_PAGE] ;						// Ŭ���̾�Ʈ ���� ä�ù�.

	int GetMsgSize()												// ������ ��ȯ �Լ�.
	{
		return sizeof(MSG_CR_ROOMLIST) - (ROOM_COUNT_PER_PAGE-byCount)*sizeof(ST_CR_ROOM_CLT) ;
	}
} ;

struct MSG_CR_ELEMENT : public MSGBASE								// �� ���� ��� ����.
{
	BYTE byIdx ;													// �� �ε���.

	char code[SECRET_CODE_SIZE+1] ;									// ä�ù� ��ȣ.

	BYTE bySecretMode ;												// ����/�����.
	BYTE byRoomType ;												// ä�ù� �з�.

	BYTE byTotalGuestCount ;										// �ִ� ���� �ο�.

	char title[ TITLE_SIZE+1 ] ;									// ä�ù� ����.
} ;

struct MSG_CR_CHANGE_OWNER : public MSGBASE							// ���� ���� ó���� �ϴ� �޽���.
{
	DWORD dwOwnerID ;												// ���� ���̵�.
	char  name[ MAX_NAME_LENGTH+1 ] ;								// ���� �̸�.
} ;	

struct MSG_CR_MSG : public MSGBASE									// �� ������� �޽����� ������ �� ����ϴ� ����ü.
{
	char  name[ MAX_NAME_LENGTH+1 ] ;								// �̸�.
	char Msg[MAX_CHAT_LENGTH+1] ;									// �޽���.
} ;

struct MSG_CR_MSG_BROADCAST : public MSGBASE						// ä�ù濡 ������ ����鿡�� �޽����� ������ �� ����ϴ� ����ü.
{
	char  name[ MAX_NAME_LENGTH+1 ] ;								// ������� �̸�.

	BYTE byCount ;													// ������ ��.
	DWORD dwUser[MAX_USER_PER_ROOM] ;								// ������ ���̵�.

	char Msg[MAX_CHAT_LENGTH+1] ;									// �޽���.
} ;

struct MSG_CR_ROOM_USER : public MSGBASE							// ������ ������ �ʿ��� �� ����ϴ� �޽���.
{
	BYTE byCount ;													// ������ ī��Ʈ.
	DWORD dwUser[MAX_USER_PER_ROOM] ;								// ������ ���̵�.
} ;

struct MSG_CR_IDNAME : public MSG_CR_ROOM_USER						// �⺻ ������ ������, Ư���� ���̵�, �̸��� �߰� �� �޽���.
{
	DWORD dwID ;													// Ư���� ���̵�.
	char  name[ MAX_NAME_LENGTH+1 ] ;								// �̸�.
} ;

struct MSG_CR_MSG_RESULT : public MSGBASE							// �⺻ ������ ������, ä�ù� ��, ������ �޽���.
{
	char  name[ MAX_NAME_LENGTH+1 ] ;								// �̸�.
	char Msg[MAX_CHAT_LENGTH+1] ;									// �޽���.
} ;

struct MSG_CR_ROOM_NOTICE : public MSG_CR_ROOM_USER					// �⺻ ������ ������, ä�ù� �� ������ ���� �޽���.
{
	ST_CR_ROOM_SRV room ;											// ä�ù� ����.
} ;

struct MSG_CR_JOIN_NOTICE	: public MSG_CR_ROOM_USER				// �⺻ ������ ������, ä�ù� ������ ���� �޽���.
{
	ST_CR_USER user ;												// ������ ����� ����.
} ;

struct MSG_CR_KICK_ACK : public MSG_CR_ROOMLIST
{
	DWORD dwKickPlayer ;
} ;

struct MSG_CR_SEARCH_NAME : public MSGBASE							// �̸� �˻��� ����ü.
{
	BYTE byRoomIdx ;												// ���ε���.
	char name[MAX_NAME_LENGTH+1] ;									// �̸�.
} ;

struct MSG_CR_SEARCH_TITLE : public MSGBASE							// ���� �˻��� ����ü.
{
	BYTE byRoomIdx ;												// ���ε���.
	char title[ TITLE_SIZE+1 ] ;									// ����.
} ;

struct MSG_CR_JOIN_SYN : public MSGBASE								// ä�ù濡 ������ �� ����� �޽���.
{
	BYTE byRoomIdx ;
	char code[SECRET_CODE_SIZE+1] ;									// ä�ù� ��ȣ.
} ;

struct MSG_CR_ROOMINFO_TO_NOTICE : public MSGBASE					// ������ Ŭ���̾�Ʈ�� ������ ������ �� ���� ����ü.
{
	ST_CR_ROOM_SRV room ;
} ;

struct MSG_FISHING_READY_SYN : public MSGBASE
{
	DWORD dwPlaceIdx;
	DWORD dwPoleItemIdx;
	DWORD dwBaitItemIdx;
	WORD wBaitItemPos;
};

struct MSG_FISHING_GETFISH_SYN : public MSGBASE
{
	DWORD dwPlaceIdx;
	DWORD dwPoleItemIdx;
	WORD  wPerpectCnt;
	WORD  wGreatCnt;
	WORD  wGoodCnt;
	WORD  wMissCnt;
};

struct MSG_FISHING_GETFISH_ACK : public MSGBASE
{
	WORD wResCode;
	DWORD dwItemIdx;
};

struct MSG_FISHING_MISSION_CODE : public MSGBASE
{
	DWORD dwItem1;
	DWORD dwItem2;
	DWORD dwItem3;
	DWORD dwProcessTime;
};

struct FISHITEMINFO
{
	DWORD dwItemIdx;
	DURTYPE dur;
	POSTYPE pos;
};

struct MSG_FISHING_FPCHANGE_SYN : public MSGBASE
{
	DWORD dwAddFishPoint;
	FISHITEMINFO FishList[12];
};

struct MSG_PET_ADD : public MSGBASE
{
	DWORD	MasterIdx;
	BASEOBJECT_INFO BaseObjectInfo;
	SEND_MOVEINFO MoveInfo;
	PET_OBJECT_INFO PetObjectInfo;
	ITEMBASE ItemInfo[ ePetEquipSlot_Max ];
	BOOL	IsLogin;
	BOOL	IsSummoned;							// 091214 ONS �����ۼ�ȯ/���̵��� �Ǵ��ϴ� �÷���.
	CAddableInfoList AddableInfo;

	WORD GetMsgLength()	{	return sizeof(SEND_CHARACTER_TOTALINFO) - sizeof(CAddableInfoList) + AddableInfo.GetInfoLength();	}
};

struct MSG_PET_INFO : public MSGBASE
{
	PET_OBJECT_INFO PetObjectInfo;
};

struct MSG_PET_ITEM_INFO : public MSGBASE
{
	DWORD	ID;
	ITEMBASE	Item[ePetEquipSlot_Max];
};

// 080916 LUJ, ������ �ռ� ��û
struct MSG_ITEM_COMPOSE_SYN : public MSGBASE
{
	// 080916 LUJ, �ռ� �� �ο��� �ɼ� ����
	ITEM_OPTION::Drop::Key	mOptionKey;
	// 080916 LUJ, �ռ� ��� �߿��� '����'�� �ִ�. �� �� �� �÷��װ� �� ���̴�
	BOOL					mIsRandomResult;
	// 080916 LUJ, �ռ� ���ۿ� �ʿ��� ������
	ICONBASE				mKeyItem;
	// 080916 LUJ, �ռ� ����� ������ ������
	ICONBASE				mResultItem;
	// 080916 LUJ, �ҿ�Ǵ� ������ ����
	DWORD					mUsedItemSize;
	ICONBASE				mUsedItem[ MAX_ITEMBUY_NUM ];
	
	DWORD GetSize() const
	{
		return sizeof( MSG_ITEM_COMPOSE_SYN ) - ( sizeof( mUsedItem ) / sizeof( *mUsedItem ) - mUsedItemSize ) * sizeof( *mUsedItem );
	}
};

// 080916 LUJ, ������ �ռ� ����
struct MSG_ITEM_COMPOSE_NACK : public MSGBASE
{
	enum Type
	{
		TypeNone,
		TypeInvalidUsedItem,
		TypeNotMatchedScript,
		TypeNotExistedScript,
		TypeNotExistedOption,
		TypeWrongUsedItemSize,
		TypeInvalidResult,
		TypeInvalidKeyItem,
		TypeEmptyResult,
		TypeFailedUpdateMemory,
	}
	mType;
};

// 080916 LUJ, ������ �ռ� ���
struct MSG_ITEM_COMPOSE_ACK : public MSGBASE
{
	// 090122 LUJ, Ŭ���̾�Ʈ �� ������ ������ ITEMBASE Ÿ������ ����
	ITEMBASE	mResultItem;
	ICONBASE	mUsedKeyItem;
	DWORD		mUsedItemSize;
	ICONBASE	mUsedItem[ MAX_ITEMBUY_NUM ];

	DWORD GetSize() const
	{
		return sizeof( MSG_ITEM_COMPOSE_ACK ) - ( sizeof( mUsedItem ) / sizeof( *mUsedItem ) - mUsedItemSize ) * sizeof( *mUsedItem );
	}
};

enum SIEGERECALLMGR_COMMAND_KIND
{
	e_CK_ADD_THEME = 0,
	e_CK_ADD_THEME_IDX,
	e_CK_ADD_MAP,
	e_CK_ADD_STEP,
	e_CK_ADD_OBJECT,
	e_CK_START_TERM,
	e_CK_END_TERM,
	e_CK_COMMAND,
	e_CK_DIE_RECALL_OBJ,
	e_CK_CHECK_DIE_OBJ,
	e_CK_USE_ITEM,

	e_CK_MAX,
} ;

// Enumerate a terms to need recall and destroy or object.
enum SIEGERECALLMGR_TERM_KINDS
{
	e_TERM_NONE = 0,

	e_TERM_DIE_OBJ,
	e_TERM_USE_ITEM,

	e_TERM_MAX,									// �ִ� ��ġ üũ��.
} ;

// 
enum SIEGERECALLMGR_COMMAND_KINDS
{
	e_COMMAND_NONE = 0,

	e_COMMAND_RECALL_DIE_OBJ,

	e_COMMAND_MAX,
} ;

// ���� ������ ���� �Ѵ�.
enum SIEGERECALLMGR_STEP_KINDS
{
	e_STEP_ADD_OBJ = 0,

	e_STEP_MAX,									// �ִ� ��ġ üũ��.
} ;

// 
enum SIEGEBASEKIND
{
	e_SIEGE_BASEKIND_EMPTY = 0,
	e_SIEGE_BASEKIND_START_TERM,
	e_SIEGE_BASEKIND_END_TERM,
	e_SIEGE_BASEKIND_COMMAND,

	e_SIEGE_BASEKIND_MAX,
} ;

// 
enum SIEGERECALL_REMOVETYPE
{
	e_SIEGERECALL_REMOVE_ALL_MAP = 0,
	e_SIEGERECALL_REMOVE_SPECIFY_MAP,

	e_SIEGERECALL_REMOVE_ALL_OBJ,
	e_SIEGERECALL_REMOVE_SPECIFY_OBJ,
} ;

enum SIEGERECALL_OBJTYPE
{
	e_ObjParent = 0 ,
	e_ObjChild,
} ;

struct st_SIEGEOBJ
{
	WORD wThemeIdx ;

	MAPTYPE	mapNum ;

	BYTE	byStepIdx ;

	DWORD	dwObjectIdx ;

	float	fXpos ;
	float	fZpos ;

	BYTE	byUseRandomPos ;

	BYTE	byRadius ;

	BYTE	byAddObjIdx ;

	BYTE	byComKind ;

	BYTE	byComIndex ;

	WORD	wParentMap ;
} ;

// 
//struct st_SIEGEOBJ
//{
//	MAPTYPE	mapNum ;
//	WORD	wObjectKind ;
//	DWORD	dwObjectIdx ;
//
//	float	fXpos ;
//	float	fZpos ;
//} ;

// 
struct st_DIE_RECALL_OBJ
{
	BYTE byIdx ;

	st_SIEGEOBJ objInfo ;
} ;

//
struct st_CHECK_OBJ_DIE
{
	BYTE byIdx ;

	MAPTYPE	mapNum ;

	BYTE byObjectIdx ;
} ;

struct st_CHECK_USE_ITEM
{
	BYTE byIdx ;

	DWORD dwItemIdx ;

	WORD wUseCount ;
} ;

struct MSG_SIEGERECALL_OBJINFO : public MSGBASE
{
	WORD wExValue ;

	st_SIEGEOBJ siegeObj ;
} ;

struct MSG_SIEGERECALL_PARENTINFO : public MSGBASE
{
	MAPTYPE p_mapNum ;

	BYTE p_byStepIdx ;

	//090812 pdy ������ Agent�� Map�������� ��ȯObj �ε��������� �߸����ִ� ���� ���� 
	//BYTE p_byChildIdx ;
	DWORD p_dwObjectIdx ;
} ;

struct MSG_SIEGERECALL_CHECK_OBJ_DIE : public MSG_SIEGERECALL_PARENTINFO
{
	BYTE byCheckKind ;

	MAPTYPE check_mapNum ;
	BYTE check_ObjIdx ;
} ;

struct MSG_SIEGERECALL_CHECK_USE_ITEM : public MSG_SIEGERECALL_PARENTINFO
{
	BYTE byCheckKind ;

	DWORD dwItemIdx ;
	WORD wItemCount ;
} ;

struct MSG_SIEGERECALL_COMMAND_DIE_RECALL_OBJ : public MSG_SIEGERECALL_PARENTINFO
{
	BYTE byCommandKind ;

	st_DIE_RECALL_OBJ dieRecallObj ;
} ;

struct MSG_SIEGERECALL_LOADOBJINFO : public MSGBASE
{
	BYTE	ThemeIdx ;
	WORD	MapIdx ;
	WORD	RecallMapIdx ;
	BYTE	StepIdx ;
	BYTE	AddObjIdx ;
	BYTE	ComKind ;
	BYTE	ComIndex ;
} ;

// 090706 pdy ���ͽõ� ��� ��û 
struct MSG_SIEGEWARFARE_USE_WATERSEED_SYN : public MSG_ITEM_USE_SYN 
{
	DWORD dwWaterSeedUsingSecond;
};

// 080805 KTH -- ���ͽõ� ��� �Ϸ�
struct MSG_WATERSEED_COMPLETE : public MSGBASE
{
	DWORD	dwMapType;
	DWORD	dwGuildIdx;
	char	szGuildName[MAX_NAME_LENGTH+1];
};

// 080819 LYW --- CommonStruct : NPC ��ȯ ���� ������ �����Ѵ�.
enum NPCRECALL_TYPE													// NPC ��ȯ ���� �з�.
{
	eNR_None = 0,													// ��ȯ ������ ���� �⺻ ����.
	eNR_ItSelf,														// ������ ��ȯ�Ǵ� Ÿ��.
	eNR_RemainTime,													// �ѹ� ��ȯ ��, ���� �ð��� ������ ������� Ÿ��.
	eNR_Moveable,													// ������ �̵��� ������ Ÿ��.

	eNR_NpcRecall_Max,												// ��ȯ���� ���� üũ��.
} ;

enum RECALL_NPC_ERROR_TYPE											// NPC ��ȯ���� ���� �з� ����.
{
	e_RNET_INVALID_NPC_TYPE =0,										// �߸� �� NPC Ÿ������ ���� ����.
	e_RNET_FAILED_RECEIVE_REMAINNPC,								// ���� �ð����� ��ȯ/�Ҹ�Ǵ� NPC ������ �޴µ� ����.

	e_RNET_FAILED_RECEIVE_STATICNPCINFO,							// STATICNPC ����Ʈ���� NPC ������ �޴µ� ����.
	e_RNET_FAILED_CREATE_NPC,										// NPC ���� ����.
	e_RNET_FAILED_RECEIVE_NPCLIST,									// NPC ����Ʈ �ޱ� ����.
	e_RNET_FAILED_RECEIVE_NPC_FROM_USERTABLE,						// �� ������ �������̺�����, NPC ���� �޴µ� ����.
	e_RNET_FAILED_RECEIVE_SERVERPORT,								// ���� ��Ʈ ���� �޴µ� ����.
	e_RNET_FAILED_RECEIVE_NPCINFO,									// NPC ������ �޴µ� ����.
	e_RNET_FAILED_RECEIVE_USERINFO,									// ���� ������ �޴µ� ����.
	e_RNET_FAILED_CHANGEMAP_INVALID_STATE,							// ���̵��� �� �� ���� �����̱� ������, �� �̵� ����.
	e_RNET_FAILED_ALREADY_RECALLED,									// �̹� ��ȯ �� NPC�� ��ȯ ����.
	e_RNET_FAILED_NOT_SIEGEWARTIME,									// ������ ���� �ƴ϶� �̵��� �� ����.
	e_RNET_FAILED_CHECK_HACK_RECALLNPC_CHECK,						// ���� NPC �������� ���� 

	e_RNET_MAX,														// ���� ���� üũ��.
} ;

struct MSG_NPCRECALL_INFO : public MSGBASE
{
	DWORD		dwCreatedIdx ;										// �� �������� ��ȯ(����)�� ���̵� ����� ����.

	BYTE		byRecallCodition ;									// ��ȯ ����.

	DWORD		dwNpcIdx ;											// Npc �ε���. StaticNpcList.bin�� Unique index�� ����Ѵ�.
	WORD		wNpcKind ;											// Npc ����. NpcList.bin�� Kind�� �����Ѵ�.

	MAPTYPE		RecallMapNum ;										// ��ȯ �� ��ȣ.

	float		fXpos ;												// ��ǥ X.
	float		fZpos ;												// ��ǥ Z.

	BYTE		byActive ;											// Ȱ��ȭ ���θ� ��� ����.
} ;

struct MSG_NPCRECALL_CHANGEMAP : public MSGBASE
{
	DWORD	dwNpcIdx ;												// �̵��� �ϰ� �ϴ� npc �ε���.

	MAPTYPE CurMapNum ;												// ���� �� ��ȣ.
	MAPTYPE ChangeMapNum ;											// �̵��� �� ��ȣ.

	BYTE    ChannelNum ;											// ä�� ��ȣ.

	float	fChangeMapXPos ;										// �̵��� ���� X��ǥ.
	float	fChangeMapZPos ;										// �̵��� ���� Z��ǥ.
} ;

// 090227 ShinJS --- Coin Item ���Խ� Coin ���� ����
struct MSG_ITEM_BUY_BY_COIN : public MSGBASE
{
	DWORD		mSize;												// ����Ǵ� Coin Item�� ����
	ITEMBASE	mItem[ SLOT_MAX_INVENTORY_NUM ];					// ����Ǵ� Coin Item ����

	DWORD GetSize() const
	{
		return sizeof( MSG_ITEM_BUY_BY_COIN ) - ( sizeof( mItem ) / sizeof( *mItem ) - mSize ) * sizeof( *mItem );
	}
};

// 090317 ONS ��� ä�� �޼����߰�
struct MSG_GUILD_CHAT : public MSGBASE
{
	DWORD dwGuildIdx;
	DWORD dwSenderIdx;
	char Msg[MAX_CHAT_LENGTH+1];
	int GetMsgLength() { return sizeof(MSG_GUILD_CHAT) - MAX_CHAT_LENGTH + strlen(Msg); }
};

// 090317 ONS ��忬�� ä�� �޼����߰�
struct MSG_GUILDUNION_CHAT : public MSGBASE
{
	DWORD	dwGuildUnionIdx;
	char	Name[MAX_NAME_LENGTH+1];
	char	Msg[MAX_CHAT_LENGTH+1];
	int		GetMsgLength() { return sizeof(MSG_GUILDUNION_CHAT) - MAX_CHAT_LENGTH + strlen(Msg); }
};

// 090317 ONS ���, ���� ��Ʈ �޼����߰�
struct MSG_GUILD_UNION_SEND_NOTE : public MSGBASE
{
	DWORD dwGuildUnionIdx;
	char FromName[MAX_NAME_LENGTH+1];
	char Note[MAX_NOTE_LENGTH+1];
	WORD GetMsgLength()
	{
		return static_cast<WORD>(sizeof(MSG_GUILD_UNION_SEND_NOTE) - (MAX_NOTE_LENGTH+1) + strlen(Note)+1);
	}
};

// 090422 ShinJS --- Ż�� ��ȯ ����
struct MSG_VEHICLE_SUMMON : public MSGBASE
{
	ICONBASE IconBase;				// ��ȯ Item ����
	COMPRESSEDPOS SummonPos;		// ��ȯ ��ġ
};

struct MSG_VEHICLE_ERROR : public MSG_DWORD
{
	enum Error
	{
		ErrorNone,
		ErrorSummonByItem,
		ErrorSummonByDistance,
		ErrorSummonByUsingItem,
		ErrorSummonByCreateVehicle,
		ErrorSummonAlready,
		ErrorSummonByMount,
		ErrorSummonByState,
		ErrorUnsummonByDistance,
		ErrorUnsummonByNoMine,
		ErrorUnsummonByPassenger,
		ErrorUnsummonByState,
		ErrorMountByDistance,
		ErrorMountByFullSeat,
		ErrorMountBySitAlready,
		ErrorMountByNoVehicle,
		ErrorMountByOverSeatIndex,
		ErrorMountByNoOwner,
		ErrorMountByPlayerLevel,
		ErrorMountBySummonedAlready,
		ErrorMountByState,
		ErrorMountByCollision,
		ErrorDismountbyDistance,
		ErrorDismountByNotSit,
		ErrorDismountByState,
		ErrorMoveByNoOwner,
		ErrorTollByFullSeat,
		ErrorTollByNoOwner,
		ErrorTollByNoPassenger,
		ErrorTollByOwnerHasFullMoney,
		ErrorTollByPassengerHasInsufficientMoney,
		ErrorTollByInvalidKey,
		ErrorInHouse,
	}
	mError;
};

// 090316 LUJ, �¼� ��ȣ���� ž���� �÷��̾� ��ȣ�� ����ִ�
struct MSG_VEHICLE_PASSENGER : public MSGBASE
{
	DWORD mSeat[ VehicleScript::Seat::MaxSize ];
};

// 090316 LUJ, ž�� ���
struct MSG_VEHICLE_ALLOW : public MSGBASE
{
	DWORD mOwnerObjectIndex;
	DWORD mMountedObjectIndex;
	DWORD mMountedSeatIndex;
	char mMountedObjectName[ MAX_NAME_LENGTH ];
	DWORD mVehicleObjectIndex;
	char mVehicleObjectName[ MAX_NAME_LENGTH ];
	MONEYTYPE mTollMoney;
};

struct MSG_LIMITDUNGEON_ERROR : public MSGBASE
{
	enum Error
	{
		ErrorNone,
		ErrorMapMove,
		ErrorRoom,
		ErrorLevel,
		ErrorItem,
	}
	mError;
};

struct MSG_HOUSE_CREATE : public MSGBASE
{
	DWORD dwPlayerID;
	char szHouseName[MAX_HOUSING_NAME_LENGTH+1];
	WORD GetMsgLength()
	{
		return static_cast<WORD>(sizeof(MSG_HOUSE_CREATE) - (MAX_HOUSING_NAME_LENGTH+1) + strlen(szHouseName)+1);
	}
};

struct MSG_HOUSE_CREATESRV : public MSGBASE
{
	DWORD dwPlayerID;
	DWORD dwPlayerMapNum;
	DWORD dwConnectionIndex;
	char szHouseName[MAX_HOUSING_NAME_LENGTH+1];
	WORD GetMsgLength()
	{
		return static_cast<WORD>(sizeof(MSG_HOUSE_CREATE) - (MAX_HOUSING_NAME_LENGTH+1) + strlen(szHouseName)+1);
	}
};

struct MSG_HOUSE_HOUSEINFO : public MSGBASE
{
	BOOL				bInit;			// ���ʿ���
	CHAR				cRank;			// 1,2,3���� ��ϵǰ� �������� 0
	DWORD				dwChannel;		// �Ͽ콺 ä��
	DWORD				dwDecoPoint;	// DecoPoint != (HousePoint�ʹ� �ٸ�)
	stHouseInfo			HouseInfo;		// �Ͽ�¡ ����
};

struct MSG_HOUSE_FURNITURELIST : public MSGBASE
{
	WORD wCategory;
	WORD wNum;
	stFurniture Furniture[MAX_HOUSING_SLOT_NUM];

	WORD GetMsgLength()
	{
		return sizeof(MSG_HOUSE_FURNITURELIST) - sizeof(stFurniture)*(MAX_HOUSING_SLOT_NUM-wNum);
	}
};

struct MSG_HOUSE_FURNITURE : public MSGBASE
{
	WORD wState;
	DWORD dwDecoPoint;
	stFurniture Furniture;
};

struct MSG_HOUSE_FURNITURE_INSTALL : public MSGBASE
{
	DWORD dwChannel;
	DWORD dwFurnitureObjectIndex;
	WORD wSlot;

	float fAngle;
	VECTOR3 vPos;
};

struct stHouse_CheatInfo
{
	DWORD dwChannelID;
	DWORD dwOwnerUserIndex;
	DWORD dwVisiterNum;
	char szHouseName[MAX_HOUSING_NAME_LENGTH + 1];
};

struct stHouse_CheatInfoDetail
{
	DWORD dwChannelID;
	DWORD dwOwnerUserIndex;
	DWORD dwVisiterNum;
	char szHouseName[MAX_HOUSING_NAME_LENGTH + 1];
	char szOwnerName[MAX_HOUSING_NAME_LENGTH + 1];

	DWORD m_dwFurnitureNum[MAX_HOUSING_CATEGORY_NUM];
	DWORD m_dwInstalledNum[MAX_HOUSING_CATEGORY_NUM];

	stHouse_CheatInfoDetail()
	{
		strcpy(szHouseName, "");
		strcpy(szOwnerName, "");
		ZeroMemory(m_dwFurnitureNum, sizeof(m_dwFurnitureNum));
		ZeroMemory(m_dwInstalledNum, sizeof(m_dwInstalledNum));
	};
};

struct MSG_HOUSE_CHEATINFO_ONE : public MSGBASE
{
	stHouse_CheatInfoDetail HouseInfo;

	WORD GetMsgLength()
	{
		return sizeof(MSG_HOUSE_CHEATINFO_ONE);
	}
};

struct MSG_HOUSE_CHEATINFO_ALL : public MSGBASE
{
	DWORD dwHouseNum;
	DWORD dwUserNum;
	DWORD dwReservationNum;
	stHouse_CheatInfo HouseInfo[MAX_HOUSE_NUM];

	WORD GetMsgLength()
	{
		return (WORD)(sizeof(MSG_HOUSE_CHEATINFO_ALL) - sizeof(stHouse_CheatInfo)*(MAX_HOUSE_NUM-dwHouseNum));
	}
};

struct MSG_HOUSE_RANK_UPDATE : public MSGBASE
{
	stHouseRank HouseRank;
};

struct MSG_HOUSE_EXTEND : public MSGBASE
{
	WORD wExtendLevel;
	WORD wUnInstallNum;
	WORD wInstallNum;
	DWORD dwDecoPoint;

	stFurniture UnInstall[MAX_HOUSING_SLOT_NUM];
	stFurniture Install[MAX_HOUSING_SLOT_NUM];

	WORD GetMsgLength()
	{
		return sizeof(MSG_HOUSE_EXTEND) - sizeof(UnInstall)*(MAX_HOUSING_SLOT_NUM-wUnInstallNum) - sizeof(Install)*(MAX_HOUSING_SLOT_NUM-wInstallNum);
	}
};

struct MSG_HOUSE_VISIT : public MSGBASE
{
	CHAR cKind;			//eHouseVisitByCheat=0, eHouseVisitByLink=1, eHouseVisitByItem=3
	DWORD dwValue1;		//eHouseVisitByLink(Npc�ε���), eHouseVisitByItem(������Index)
	DWORD dwValue2;		//eHouseVisitByItem(������Slot)

	char Name[MAX_NAME_LENGTH+1];
};

struct MSG_HOUSE_VISITSRV : public MSGBASE
{
	CHAR cKind;
	DWORD dwValue1;
	DWORD dwValue2;
	DWORD dwConnectionIndex;
	DWORD dwMapNum;

	char Name[MAX_NAME_LENGTH+1];
};

struct MSG_DUNGEON_INFO : public MSGBASE
{
	DWORD dwIndex;
	DWORD dwPartyIndex;
	DWORD dwChannelID;
	DWORD dwPoint;
	DWORD dwJoinPlayerNum;
	eDIFFICULTY difficulty;
	stWarpState warpState[MAX_DUNGEON_WARP];
	stSwitchNpcState SwitchNpcState[MAX_DUNGEON_SWITCH];
};

struct stDungeonSummary
{
	DWORD dwChannel;
	DWORD dwPartyIndex;
	DWORD dwJoinPlayerNum;
	DWORD dwPoint;
	eDIFFICULTY	difficulty;
};

struct MSG_DUNGEON_INFO_ALL : public MSGBASE
{
	DWORD dwDungeonNum;
	DWORD dwUserNum;
	stDungeonSummary DungeonInfo[MAX_DUNGEON_NUM];

	WORD GetMsgLength()
	{
		return (WORD)(sizeof(MSG_DUNGEON_INFO_ALL) - sizeof(stDungeonSummary)*(MAX_DUNGEON_NUM-dwDungeonNum));
	}
};

struct stDungeonDetail
{
	stDungeonSummary DungeonSummary;
	
	PARTY_MEMBER	PartyMember[MAX_PARTY_LISTNUM];
	stWarpState warpState[MAX_DUNGEON_WARP];
	int			switchState[MAX_DUNGEON_SWITCH];
};

struct MSG_DUNGEON_INFO_ONE : public MSGBASE
{
	stDungeonDetail DungeonInfo;
};

// 100601 ONS ä�ñ��� ���� ���� ����ü
struct MSG_FORBID_CHAT : public MSGBASE
{
	DWORD	dwData;
	char	Name[MAX_NAME_LENGTH + 1];
	char	Reason[MAX_CHAT_LENGTH + 1];
};

// 100722 ONS ��Ƽ������ Master�� Tip���� ���ó�� �߰�.
struct MSG_PARTY_CREATE : public MSGBASE
{
	BYTE PartyOpt;
	BYTE JobGrade;
	BYTE Race;
	BYTE DiceGrade;
	MAPTYPE MapType;
	DWORD PartyIDX;
	DWORD MasterID;
	DWORD MasterLv;

	BYTE Job[MAX_JOB_GRADE];
	char Name[MAX_NAME_LENGTH + 1];
};


struct stEnchantGoldRate
{
	float	fLevelRate;		// �����۷��� ���
	float	fGradeRate;		// �����۵�� ���
	float	fBasicRate;		// �⺻ ���
};

// 090713 ShinJS --- ��æƮ ������ ���� ���ʽ����� ��ġ ����
struct stEnchantLvBonusRate
{
	LEVELTYPE minLevel;			// �ּ� ����
	LEVELTYPE maxLevel;			// �ִ� ����	
	float fEnchantLvRate1;		// ��æƮ ���� ���1
	float fEnchantLvRate2;		// ��æƮ ���� ���2
};

// 090909 ONS ��ųƮ�� ����
struct SkillData
{
	DWORD	index;
	BYTE	level;
};

struct LimitDungeonScript
{
	DWORD mNameIndex;
	MAPTYPE mMapType;
	// 091106 LUJ, ������ ����
	DWORD mChannel;
	TCHAR mMonsterScript[MAX_PATH];
	struct Date
	{
		// 091106 LUJ, 0~6. �Ͽ���~�����
		int mDayOfWeek;
		int mStartHour;
		int mStartmMinute;
		int mPlayTime;
	};
	typedef std::list< Date > DateList;
	DateList mDateList;
	struct Level
	{
		LEVELTYPE mMin;
		LEVELTYPE mMax;
	}
	mLevel;
	struct Item
	{
		DWORD mItemIndex;
		DURTYPE mQuantity;
	}
	mItem;
	struct Home
	{
		MAPTYPE mMapType;
		VECTOR3 mPosition;
	}
	mHome;
	struct Alarm
	{
		__time64_t mTime;
		DWORD mMessage;
	};
	typedef std::list< Alarm > AlarmList;
	AlarmList mAlarmList;
	typedef std::list< DWORD > DescriptionList;
	DescriptionList mDescriptionList;

	LimitDungeonScript() :
	mNameIndex(0),
	mMapType(0),
	mChannel(0)
	{
		const Level emptyLevel = {0, USHRT_MAX};
		mLevel = emptyLevel;
		ZeroMemory(mMonsterScript, sizeof(mMonsterScript));
		ZeroMemory(&mItem, sizeof(mItem));
		ZeroMemory(&mHome, sizeof(mHome));
	}
};

struct stTime64t : public MSGBASE
{
	__time64_t time64t;
};

#pragma pack(pop)
