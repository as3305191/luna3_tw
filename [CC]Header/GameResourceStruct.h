#ifndef __GAMERESOURCESTRUCT_H__
#define __GAMERESOURCESTRUCT_H__

class cActiveSkillInfo;
enum EObjectKind;

class RECALLINFO
{
public:
	DWORD	RecallType;
	DWORD	RecallID;
	DWORD	RecallMonsterNum;
	DWORD	RecallRange;
};

class CRecallInfo
{
public:
	RECALLINFO * pRecallInfo;

	CRecallInfo(){ memset(this, 0, sizeof(CRecallInfo)); }
	virtual ~CRecallInfo()
	{
		if(pRecallInfo)
		delete [] pRecallInfo;
	}
	void Init(DWORD nRecall)
	{
		MaxRecallNum = nRecall;
		pRecallInfo = new RECALLINFO[MaxRecallNum];
		memset(pRecallInfo, 0, sizeof(RECALLINFO)*MaxRecallNum);
	}
	
	RECALLINFO * GetRecallInfo(DWORD dwIdx)
	{
		ASSERT(dwIdx < MaxRecallNum);
		return &pRecallInfo[dwIdx];
	}
	void SetRecallType(DWORD dwIdx, DWORD dwType)
	{
		pRecallInfo[dwIdx].RecallType = dwType;
	}
	void SetRecallID(DWORD dwIdx, DWORD dwID)
	{
		pRecallInfo[dwIdx].RecallID = dwID;
	}
	void SetRecallMonsterNum(DWORD dwIdx, DWORD dwNum)
	{
		pRecallInfo[dwIdx].RecallMonsterNum = dwNum;
	}
	void SetRecallRange(DWORD dwIdx, DWORD dwRange)
	{
		pRecallInfo[dwIdx].RecallRange = dwRange;
	}
protected:
	DWORD	MaxRecallNum;
	
};

struct MONSTER_DROPITEM
{
	DWORD wItemIdx;
	DWORD dwDropPercent;
	BYTE byCount;
};

struct BASE_MONSTER_LIST
{
	WORD		MonsterKind;			// ��ȣ
	EObjectKind	ObjectKind;				// Object��ȣ 32 �Ϲ� 33 ���� 35 �ʵ庸�� 36 �ʵ庸������
	char		Name[MAX_MONSTER_NAME_LENGTH +1];				// �����̸�	
	LEVELTYPE	Level;					// ���ͷ���	
	char		ChxName[32];			// ���� CHX_Index	
	float		Scale;					// ���� ũ��	
	DWORD		Gravity;				// ���� ����
	float		Weight;					// ���� ����	
	DWORD		MonsterRadius;
	DWORD		Life;					// �����	
	DWORD		ExpPoint ;
	WORD		Tall;					// Ű
	WORD		Accuracy;
	WORD		Avoid;
	WORD		AttackPhysicalMin;// ���� �ּҹ��� ���ݷ�	
	WORD		AttackPhysicalMax;// ���� �ִ빰�� ���ݷ�
	WORD		AttackMagicalMin;// ���� �ּҸ��� ���ݷ�	
	WORD		AttackMagicalMax;// ���� �ִ븶�� ���ݷ�
	WORD		CriticalPercent;// ũ��Ƽ�� �ۼ�Ʈ
	WORD		PhysicalDefense;// ���� ����	
	WORD		MagicalDefense;// ���� ����	
	WORD		WalkMove;// ���Ͱȱ��̵���	
	WORD		RunMove;// ���Ͷٱ��̵���	
	WORD		RunawayMove;// ���͵����̵���

	int			Damage1DramaNum;// ���� 1 ���� ��ȣ	
	int			Die1DramaNum;// ���� 1 ���� ��ȣ	
	int			Die2DramaNum;// ���� 2 ���� ��ȣ	

	DWORD		StandTime;
	BYTE		StandRate;
	BYTE		WalkRate;
	BYTE		RunRate;
	DWORD		DomainRange;					// ���� ���� : ������
	DWORD		PursuitForgiveTime;
	DWORD		PursuitForgiveDistance;	

	BOOL		bForeAttack;					// ������
	DWORD		SearchPeriodicTime;				// Ž�� �ֱ�
	WORD		TargetSelect;					// Ÿ�� ���� : FIND_CLOSE, FIND_FIRST
	DWORD		SearchRange;
	DWORD		SearchAngle;
	WORD		TargetChange;
	DWORD		AttackNum;
	DWORD		AttackIndex[5];
	DWORD		AttackRate[5];
	cActiveSkillInfo* SkillInfo[5];
	// 100104 LUJ, ���Ͱ� �ֺ��� ������ ��û�Ѵ�
	enum HelpType
	{
		HelpNone,
		HelpLifeIsUnder50Percent,
		HelpLifeIsUnder30Percent,
		HelpDie,
		HelpAlways,
	}
	mHelpType;
	BOOL		bDetectSameRace;
	BOOL		bHearing;
	DWORD		HearingDistance;

	WORD		SpecialType;
	WORD		MonsterType;
	WORD		MonsterRace;
	WORD		MonsterAttribute;

	BYTE		BaseReinforce;
	BYTE		PhysicAttackReinforce;
	BYTE		MagicAttackReinforce;
	BYTE		PhysicDefenseReinforce;
	BYTE		MagicDefenseReinforce;

	MONEYTYPE dropMoney;
	DWORD dwMoneyDropRate;
	MONSTER_DROPITEM dropItem[eDROPITEMKIND_MAX - 1];
	DWORD		dwMonsterTargetType;	//���� GuageTargetDlg Type
};

typedef struct _CHXLIST
{
	StaticString FileName;
}CHXLIST;

//-----------------------------------------------------------------------
// ����ȯ ��ǥ��� ����ü
//-----------------------------------------------------------------------
typedef struct _MAPCHANGE_INFO
{
	WORD Kind;

	// 071214 LUJ, �� �̸��� MAX_NAME_LENGTH ���̸� �ʰ��Ͽ� ���̸� ���� ���� 
	char CurMapName[ MAX_MONSTER_NAME_LENGTH + 1 ];
	char ObjectName[ MAX_MONSTER_NAME_LENGTH + 1 ];
	//char CurMapName[MAX_NAME_LENGTH+1];
	//char ObjectName[MAX_NAME_LENGTH+1];

	WORD CurMapNum;
	WORD MoveMapNum;
	VECTOR3 CurPoint;
	VECTOR3 MovePoint;
	WORD chx_num;
	
	// ���⼳��
}MAPCHANGE_INFO;

//-----------------------------------------------------------------------
// LogIn ��ǥ��� ����ü
//-----------------------------------------------------------------------
typedef struct _LOGINPOINT_INFO
{
	WORD Kind;
	char MapName[MAX_NAME_LENGTH+1];
	WORD MapNum;
	BYTE MapLoginPointNum;
	VECTOR3 CurPoint[10];
	WORD chx_num;

	// ���⼳��
}LOGINPOINT_INFO;

//-----------------------------------------------------------------------
// NPC ����
//-----------------------------------------------------------------------
struct NPC_LIST
{
	WORD NpcKind;			// Npc ����
	char Name[MAX_NPC_NAME_LENGTH+1];			// Npc �̸�
	WORD ModelNum;			// MonsterChxList������ ��ȣ
	WORD JobKind;			// ��������������
	float Scale;			// Npc ũ��
	WORD Tall;				// Ű
	BOOL ShowJob;
};

//-----------------------------------------------------------------------
// NPC ����
//-----------------------------------------------------------------------
typedef struct _NPC_REGEN
{

	_NPC_REGEN():dwObjectID(0),NpcKind(0)
	{
		Pos.x = Pos.y = Pos.z = 0;
	}
	DWORD	dwObjectID;		// �ű� �߰� taiyo
	MAPTYPE	MapNum;			// Map��ȣ
	WORD	NpcKind;		// NpcKind
	char	Name[MAX_NAME_LENGTH+1];		// Npc �̸�
	WORD	NpcIndex;		// NpcIndex(������ȣ�� ���)
	VECTOR3 Pos;			// ���� ��ġ
	float	Angle;			// ���� ����
}NPC_REGEN;


//-----------------------------------------------------------------------
// MOD ����Ʈ 
//-----------------------------------------------------------------------
struct MOD_LIST
{
	MOD_LIST()
	{
		MaxModFile = 0;
		ModFile = NULL;
	}
	~MOD_LIST()
	{
		if(MaxModFile == 0)
			return;
		SAFE_DELETE_ARRAY(ModFile);
	}
	DWORD MaxModFile;
	StaticString* ModFile;
	StaticString BaseObjectFile;	
};

struct ITEM_INFO
{
	DWORD ItemIdx;
	char ItemName[MAX_ITEMNAME_LENGTH + 1];
	DWORD ItemTooltipIdx;
	WORD Image2DNum;
	WORD Part3DType;
	WORD Part3DModelNum;
	LIMIT_RACE LimitRace;
	WORD LimitGender;
	LEVELTYPE LimitLevel;
	WORD Shop;
	WORD Stack;
	WORD Trade;
	WORD Deposit;
	WORD Sell;
	WORD Decompose;
	WORD Repair;
	BOOL IsEnchant;		// ���׷��̵� ���� ����
	WORD Improvement;

	DWORD EnchantValue;	// ���� ���� Ȯ�� ����
	DWORD EnchantDeterm;	// ���� �ܰ踶�� �ο��Ǵ� ��ġ

	DWORD DecomposeIdx;
	DWORD Time;
	MONEYTYPE BuyPrice;
	MONEYTYPE SellPrice;
	LEVELTYPE Grade;
	DURTYPE Durability;
	int PhysicAttack;
	int MagicAttack;
	int PhysicDefense;
	int MagicDefense;
	eItemCategory Category;
	eEquipType EquipType;
	EWEARED_ITEM EquipSlot;
	eWeaponAnimationType WeaponAnimation;
	eWeaponType WeaponType;
	eArmorType ArmorType;
	eAccessaryType AccessaryType;
	int ImprovementStr;			
	int ImprovementDex;			
	int ImprovementVit;			
	int ImprovementInt;
	int ImprovementWis;			
	int ImprovementLife;			
	int ImprovementMana;		
	int ImprovementSkill;

	DWORD Skill;
	ITEM_KIND SupplyType;
	DWORD SupplyValue;
	// ��Ȳ�� ����
	BOOL Battle;
	BOOL Peace;
	BOOL Move;
	BOOL Stop;
	BOOL Rest;
	ITEM_SEAL_TYPE wSeal;
	ITEM_TIMEKIND nTimeKind;
	DWORD dwUseTime;
	DWORD RequiredBuff;

	enum eKind
	{
		eKindNormal,
		eKindHighClass,
		eKindRare,
		eKindUnique,
		eKindLegend,
	}
	kind;
	DWORD dwBuyFishPoint;
	eItemPointType wPointType;
	DWORD dwPointTypeValue1;
	DWORD dwPointTypeValue2;
	DWORD dwType; // Item ����
	DWORD dwTypeDetail; // Item ��������
};

struct stEnchantFailSetting
{
	ITEM_INFO::eKind dwItemKind;
	LEVELTYPE dwManusMinLv;	// ��æƮ���� ���̳ʽ� ���� �ּ� Lv
	LEVELTYPE dwManusMaxLv;	// ��æƮ���� ���̳ʽ� ���� �ִ� Lv
	LEVELTYPE dwZeroMinLv; // ��æƮ���� 0 ���� �ּ� Lv
	LEVELTYPE dwZeroMaxLv; // ��æƮ���� 0 ���� �ִ� Lv
	LEVELTYPE dwDeleteItemMinLv; // ��æƮ �õ� ������ ���� �ּ� Lv
	LEVELTYPE dwDeleteItemMaxLv; // ��æƮ �õ� ������ ���� �ִ� Lv
};

#ifdef _CLIENT_RESOURCE_FIELD_
//-----------------------------------------------------------------------------------------------------------//

typedef struct _PRELOAD
{
	StaticString FileName;
}PRELOAD;

#ifdef _TESTCLIENT_

//trustpak
struct TESTCHARACTER_INFO
{
	BYTE byGender;
	float fX;
	float fY;
	float fZ;
	float fAngle;
};

#define MAX_TEST_CHARACTER_NO 8
//


struct TESTCLIENTINFO {
	TESTCLIENTINFO()
	{
		//trustpak
		memset(this, 0, sizeof(TESTCLIENTINFO));
		//

		LightEffect = 1;
		Effect = 0;
		SkillIdx = 0;
		WeaponIdx = 0;
		Gender = GENDER_MALE;
		x = 30000.f;
		z = 30000.f;
		MonsterEffect = 1;
		MonsterNum = 1;
		MonsterKind = 13;
		Map = 17;

		DressIdx = 0;
		HatIdx = 0;
		ShoesIdx = 0;
		HairType = 0;
		FaceType = 0;
		

		CharacterNum = 0;
		BossMonsterNum = 0;
		BossMonsterKind = 1000;
		
	}
	float x,z;

	WORD DressIdx;
	WORD HatIdx;
	WORD ShoesIdx;
	WORD ShieldIdx;

	BYTE HairType;
	BYTE FaceType;

	WORD MonsterKind;

	WORD BossMonsterKind;
	WORD BossMonsterNum;
	char BossInfofile[17];

	int Effect;
	WORD SkillIdx;
	WORD WeaponIdx;
	BYTE Race;
	BYTE Gender;
	BYTE Job;
	int MonsterEffect;
	WORD MonsterNum;
	WORD CharacterNum;
	MAPTYPE Map;

	BOOL LightEffect;
	
	float Width;
	float Height;

	//trustpak
	DWORD				dwTestCharacterNo;
	TESTCHARACTER_INFO	aTestCharacterInfo[MAX_TEST_CHARACTER_NO];
	
	//

};
#endif

//-----------------------------------------------------------------------
// game config ���� 
//-----------------------------------------------------------------------
struct GAMEDESC_INI
{
	GAMEDESC_INI()
	{
		strcpy(DistributeServerIP,"211.233.35.36");
		DistributeServerPort = 400;

		AgentServerPort = 100;
		
		bShadow = MHSHADOW_CIRCLE;
		
		dispInfo.dwWidth = 1024;		//WINDOW SIZE
		dispInfo.dwHeight = 768;
		dispInfo.dwBPS = 4;
		dispInfo.dispType = WINDOW_WITH_BLT;
		//dispInfo.bWindowed = TRUE;
		dispInfo.dwRefreshRate = 70;

		MaxShadowTexDetail = 256;
		MaxShadowNum = 4;

		FramePerSec = 30;
		TickPerFrame = 1000/(float)FramePerSec;

		MasterVolume = 1;
		SoundVolume = 1;
		BGMVolume = 1;
		
		CameraMinAngleX = 0;
		CameraMaxAngleX = 89.f;
		CameraMinDistance = 200.f;
		CameraMaxDistance = 1000.f;

		LimitDay = 0;
		LimitID[0] = 0;
		LimitPWD[0] = 0;

		strWindowTitle[0] = 0;
	}

	float MasterVolume;
	float SoundVolume;
	float BGMVolume;

	char DistributeServerIP[32];
	WORD DistributeServerPort;
	
	WORD AgentServerPort;
	
	DISPLAY_INFO dispInfo;

	BYTE bShadow;
	DWORD MaxShadowNum;
	DWORD MaxShadowTexDetail;

	DWORD FramePerSec;
	float TickPerFrame;

	char MovePoint[64];

	float CameraMinAngleX;
	float CameraMaxAngleX;
	float CameraMinDistance;
	float CameraMaxDistance;

	DWORD LimitDay;
	char LimitID[MAX_NAME_LENGTH+1];
	char LimitPWD[MAX_NAME_LENGTH+1];

	char strWindowTitle[128];
};

struct SEVERLIST
{
	char	DistributeIP[16];
	WORD	DistributePort;
	char	ServerName[64];
	WORD	ServerNo;
	BOOL	bEnter;

	SEVERLIST()
	{
		ServerNo = 1;
		strcpy( DistributeIP, "211.233.35.36" );
		DistributePort = 400;
		strcpy( ServerName, "Test" );
		bEnter = TRUE;
	}
};

struct PET_FRIENDLY_STATE
{
	BYTE	Friendly;
	DWORD	SpeechIndex;
	BYTE	SpeechRate;
	DWORD	EmoticonIndex;
	BYTE	EmoticonRate;
};
//-----------------------------------------------------------------------------------------------------------//
#endif //_CLIENT_RESOURCE_FIELD_
//-----------------------------------------------------------------------------------------------------------//


//-----------------------------------------------------------------------------------------------------------//
//		������ ����ϴ� �κ�
#ifdef _SERVER_RESOURCE_FIELD_

//-----------------------------------------------------------------------------------------------------------//
#include "ServerTable.h"
//-----------------------------------------------------------------------
// ĳ���� IN/OUT POINT ����
//-----------------------------------------------------------------------
typedef struct _CHARACTERINOUTPOINT
{
	WORD MapNum;
	VECTOR3 MapInPoint[MAX_MAP_NUM];
	VECTOR3 MapOutPoint[MAX_MAP_NUM];
}CHARACTERINOUTPOINT;


//---KES ��ų�ʱ�ȭ��	071207
struct SKILL_MONEY
{
	DWORD SkillIdx;
	DWORD SP;
	DWORD Money;
};

//----------------------------


//-----------------------------------------------------------------------------------------------------------//
#endif //_SERVER_RESOURCE_FIELD_
//-----------------------------------------------------------------------------------------------------------//
#endif //__GAMERESOURCESTRUCT_H__
