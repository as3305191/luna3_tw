#ifndef __COMMONGAMEDEFINE_H__
#define __COMMONGAMEDEFINE_H__


enum RaceType
{
	RaceType_Human,
	RaceType_Elf,
	// 090504 ONS �ű����� �߰��۾�
	RaceType_Devil,

	RaceType_Max,

	RaceType_GM,
};

//for GMPower
enum eGM_POWER
{
	eGM_POWER_MASTER = 0,
	eGM_POWER_MONITOR,
	eGM_POWER_PATROLLER,
	eGM_POWER_AUDITOR,
	eGM_POWER_EVENTER,
	eGM_POWER_QA,
	eGM_POWER_MAX,
};

#define EXIT_COUNT	5
enum eEXITCODE
{
	eEXITCODE_OK,
	eEXITCODE_PKMODE,
	eEXITCODE_LOOTING,
	eEXITCODE_SPEEDHACK,
	eEXITCODE_DIEONEXIT,
	eEXITCODE_NOT_ALLOW_STATE,
};
//

#define SPEEDHACK_CHECKTIME	60000	//1���ע������ע�E

//for PK
#define PKLOOTING_ITEM_NUM			20
#define PKLOOTING_DLG_DELAY_TIME	2000
#define PKLOOTING_LIMIT_TIME		10000

enum ePKCODE
{
	ePKCODE_OK,
	ePKCODE_ALREADYPKMODEON,
	ePKCODE_SHOWDOWN,
	ePKCODE_STATECONFLICT,
	ePKCODE_NOTALLAW,
};

enum eLOOTINGERROR
{
	eLOOTINGERROR_OK,
	eLOOTINGERROR_NOMORE_CHANCE,
	eLOOTINGERROR_NOMORE_ITEMLOOTCOUNT,
	eLOOTINGERROR_INVALID_POSITION,
	eLOOTINGERROR_ALREADY_SELECTED,
	eLOOTINGERROR_OVER_DISTANCE,
	eLOOTINGERROR_NO_LOOTINGROOM,
};

#define NPC_TALKING_DISTANCE 700
#define PK_LOOTING_DISTANCE	1000
#define eDROPITEMKIND_MAX 16

enum eUSERLEVEL
{
	eUSERLEVEL_GOD = 1,
	eUSERLEVEL_PROGRAMMER,
	eUSERLEVEL_DEVELOPER,
	eUSERLEVEL_GM,
	eUSERLEVEL_SUPERUSER,
	eUSERLEVEL_USER,
	eUSERLEVEL_SUBUSER,	
};

// 091125 ONS ��������� �ִ밹�� 100 -> 1000���� ����
#define MAX_ITEMBUY_NUM		1000
#define MAX_INVENTORY_MONEY	9999999000000000

enum eSTATUSICON
{
	eStatusIcon_None,
	eStatusIcon_QuestTime,
	eStatusIcon_Max,
};

enum eBOSSMONSTERKIND
{
	eBOSSKIND_TARINTUS = 261,
	eBOSSKIND_KIERRA = 267,
	eBOSSKIND_ARACH = 280,
	eBOSSKIND_LEOSTEIN = 721,
	eBOSSKIND_DRAGONIAN = 762,
};

#define RECALL_GROUP_ID			-1
#define EVENTRECALL_GROUP_ID	-2
#define QUESTRECALL_GROUP_ID	-3
// desc_hseos_����Ʈ ��_01
// S ����Ʈ �� �߰� added by hseos 2007.11.29
#define CHALLENGERECALL_GROUP_ID	-4
// E ����Ʈ �� �߰� added by hseos 2007.11.29

#define ITEMRECALL_GROUP_ID	1001	// ���ͼ�ȯ ������ ���
#define DUNGEON_MONSTER_GROUP_ID	2001	// �ν��Ͻ����� ����

// 100603 ONS Ż�� �׷���̵� �߰�
#define VEHICLE_RECALL_GROUP_ID	3001

#define eOPT_HIDEPARTS_NONE				0x00000000
#define eOPT_HIDEPARTS_EQUIP_BAND		0x00000001
#define eOPT_HIDEPARTS_EQUIP_GLASSES	0x00000002
#define eOPT_HIDEPARTS_EQUIP_WING		0x00000004
#define eOPT_HIDEPARTS_COSTUME_HEAD		0x00000008
#define eOPT_HIDEPARTS_COSTUME_DRESS	0x00000010
#define eOPT_HIDEPARTS_COSTUME_GLOVE	0x00000020
#define eOPT_HIDEPARTS_COSTUME_SHOES	0x00000040
#define eOPT_HIDEPARTS_COSTUME_WING		0x00000080
#define eOPT_HIDEPARTS_ALL				( eOPT_HIDEPARTS_EQUIP_BAND			|\
											eOPT_HIDEPARTS_EQUIP_GLASSES	|\
											eOPT_HIDEPARTS_EQUIP_WING		|\
											eOPT_HIDEPARTS_COSTUME_HEAD		|\
											eOPT_HIDEPARTS_COSTUME_DRESS	|\
											eOPT_HIDEPARTS_COSTUME_GLOVE	|\
											eOPT_HIDEPARTS_COSTUME_SHOES	|\
											eOPT_HIDEPARTS_COSTUME_WING )
#define MAXPLAYER_IN_CHANNEL	300

enum eBATTLE_KIND
{
	eBATTLE_KIND_NONE = 0,		// Player vs Monster ���סˢ�o����uOAC �ˢ�ic��I�Ϩ�ECO��EO�ˢ碮��AC Battle
	eBATTLE_KIND_SHOWDOWN,
	eBATTLE_KIND_GTOURNAMENT,
	eBATTLE_KIND_MAX,
};

enum GRID_TYPE
{
	eGridType_General,
	eGridType_Murim
};

enum Friend_ERROR_CODE
{
	//add friend
	eFriend_AddSuccess=0,
	eFriend_AddInvalidUser,
	eFriend_AddFullList,
	eFriend_AddDeny,
	eFriend_NotLogin,
	eFriend_AlreadyRegist,
//	eFriend_RegistSelf,
	//delete friend
	eFriend_DelInvaliedUser,
//----
	eFriend_OptionNoFriend,
};

enum {
LOGIN_SUCCESS = 0,
LOGIN_ERROR_INVALID_VERSION,
LOGIN_ERROR_OVERLAPPEDLOGIN,
LOGIN_ERROR_NOAGENTSERVER,
LOGIN_ERROR_NODISTRIBUTESERVER,
LOGIN_ERROR_INVALIDUSERLEVEL,
LOGIN_ERROR_WRONGIDPW,
LOGIN_ERROR_INVALIDDISTAUTHKEY,
LOGIN_ERROR_DISTSERVERISBUSY,
LOGIN_ERROR_AGENTSERVERISBUSY,
LOGIN_ERROR_AGENTMAXCONNECTION,
LOGIN_ERROR_BLOCKUSERLEVEL,
LOGIN_ERROR_INVALID_IP,	//ipcheck
LOGIN_ERROR_DISTCONNET_ERROR,
LOGIN_ERROR_MAXUSER,
LOGIN_ERROR_OVERLAPPEDLOGININOTHERSERVER,
LOGIN_ERROR_MINOR_INADULTSERVER,
LOGIN_ERROR_NOT_CLOSEBETAWINNER,
LOGIN_ERROR_SECEDEDACCOUNT,
LOGIN_ERROR_NOREMAINTIME,
LOGIN_ERROR_NOIPREGEN,
LOGIN_ERROR_NONACCADMINIP,
// 080111 LYW --- CommonGameDefine : �α��� ���� ��й�ȣ ���� ���� ó�� �߰�.
LOGIN_ERROR_INVALID_SEC_PASS,				// ���� ��й�ȣ ������ �����ߴٴ� �����޽���.
LOGIN_ERROR_CHANGE_SEC_PASS,				// �߸��� ���� �����ȣ, Ȩ���������� ���� ��й�ȣ�� �� ��� �Ͻÿ�.
LOGIN_ERROR_FILE_CRC,
LOGIN_ERROR_LOGINSESSION_INVALID,
};

#define MHSHADOW_OFF	0
#define MHSHADOW_CIRCLE	1
#define MHSHADOW_DETAIL	2
// E��E�������ע�����Io ����oA�ˢ硧���ˢ�I
#define RECOVERTIMEUNIT					3000
// E��E�������ע�����Io �ˢ硧���ˢ�I�ˢ硧��Y
#define RECOVERINTERVALUNIT				10

// A��E����AI �ˢ�icAIAi(CN A��E����AIAC A����I�ˢ硧ua)
#define fTILE_SIZE 50.f

#define SKILLAREA_ATTR_DAMAGE			0x00000001
#define SKILLAREA_ATTR_BLOCK			0x00000002
#define SKILLAREA_ATTR_SAFETYZONE		0x00000003
#define SKILLAREA_ATTR_OTHEREFFECT		0x00000004

typedef BYTE AREATILE;
#define GetAreaTile	GetByte

#define MAX_ITEM_NUM 3000	//confirm
#define MAX_ITEM_OPTION_NUM	90

// INDEXGENERATER ID START INDEX
#define MAX_ITEMICON_NUM	999

// 080203 LYW --- �ൿ ���� ������ �ִ� ���� ����.
#define MAX_CONDUCTICON_NUM	999

#define IG_ITEM_STARTINDEX				10000
#define IG_MSGBOX_STRARTINDEX			12000
#define IG_STREETSTALLITEM_STARTINDEX	13000	//LBS 03.09.19
#define IG_EXCHANGEITEM_STARTINDEX		14000	//KES EXCHANGE 030920
#define IG_MENUSLOTITEM_STARTINDEX		15000	//KES 040510	//Ao�ˢ硧uY ����uE����u��E��I��E��IU. Ao��E?i�ˢ硧ui.
#define IG_PKLOOTINGDLG_START			16000	//KES PKLOOTING 040720
#define IG_DEBUG_START					20000	//KES DEBUG 040720
#define IG_QUESTITEM_STARTINDEX			25000	//RaMa
#define IG_SHOPITEM_STARTINDEX			28000	//RaMa 04.11.17
#define IG_DEALITEM_START				30000
#define IG_MAINBARICON_START			40000
//#define IG_ABILITYICON_START			31000
//#define IG_ABILITYPOINT_START			40000	//RaMa Ư��ġ ��й�
#define IG_QUICKITEM_STARTINDEX			60000
// 070216 LYW --- CommonGameDefine : Define start index of conduct part.
#define IG_CONDUCT_STARTINDEX			70000

// 070608 LYW --- CommonGameDefine : Define start index of matching favor icon part.
#define IG_FAVOR_STARTINDEX				150000
#define IG_IDENTIFICATION_STARTINDEX	151000
#define IG_MATCH_MYINFO_STARTINDEX		152000
#define IG_MATCH_PARTNERINFO_STARTINDEX	153000
#define IG_MATCH_ENDINDEX				154000

//090423 pdy �Ͽ�¡ ������ �ε��� 
#define IG_HS_STOREDICON_START 155000
#define HOUSING_FURNITURE_STARTINDEX	(UINT_MAX - 2000000)
#define MONSTER_INDEX_SIZE				200000
#define MONSTER_STARTINDEX				HOUSING_FURNITURE_STARTINDEX - MONSTER_INDEX_SIZE
#define VEHICLE_INSTALL_ID				MONSTER_STARTINDEX - 1

enum NPC_ROLE
{
	OBJECT_ROLE = 0,
	DEALER_ROLE = 1,
	MUNPA_ROLE = 3,
	CHANGGO_ROLE = 4,
	FYOKUK_ROLE = 5,
	TALKER_ROLE = 6,
	MAP_MOVE_ROLE = 8,
	FAMILY_ROLE = 15,
 	// 080901 LYW --- CommonGameDefine :
	CANDLESTAND_ROLE = 20,
	// 080821 LYW --- CommonGameDefine : �� �̵��� �ϴµ�, Ȯ�� �޽��� �ڽ��� ���� Ÿ���̴�.(���������� ���)
	PORTAL_ROLE = 21,
	// 080901 LYW --- CommonGameDefine : 
	SIEGEWELL_ROLE = 22,
	BOMUL_ROLE	= 23,
	// 080901 LYW --- CommonGameDeinfe : ���� NPC ���� �߰�.
	BUFFNPC_ROLE = 24,
	// 080903 LYW --- CommonGameDefine : ���� ��� �߰�.
	SIEGEFLAGROLE_RUSHEN = 25,
	SIEGEFLAGROLE_ZEVYN = 26,
	MAPCHANGE_ROLE = 27,	
	FISHINGPLACE_ROLE = 28,
	CAMPFIRE_ROLE = 29,
	// 070605 LYW --- CommonGameDefine : Add identification role.
	IDENTIFICATION_ROLE = 38,

	// 081125 LYW --- CommonGameDefine : ����Ʈ ���� ���� ���� ���� �߰�.
	LIMITDUNGEON_ROLE	= 40,

	//090522 pdy  --- CommonGameDefine : �Ͽ�¡ NPC ���� �߰�.
	HOUSING_ROLE		= 50,
	HOUSING_RANK1_ROLE	= 51,
	HOUSING_RANK2_ROLE	= 52,
	HOUSING_RANK3_ROLE	= 53,
	HOUSING_PORTAL_ROLE	= 54,
	DUNGEON_WARP_ROLE	= 55,
	DUNGEON_SWITCH_ROLE = 56,
	
	NOTE_ROLE			= 60,	// ������
	CONSIGNMENT_ROLE	= 61,	// �ǸŴ���
};


enum eMoveMode
{
	eMoveMode_Run = 0,
	eMoveMode_Walk = 1,
};

#define MAX_PACKEDDATA_SIZE 60000 // for SEND_PACKED_DATA

enum eMONSTER_ACTION
{
	eMA_STAND,
	eMA_WALKAROUND,
	eMA_PERSUIT,
	eMA_RUNAWAY,
	eMA_WALKAWAY,
	eMA_ATTACK,
	eMA_PAUSE,
	eMA_SCRIPT_RUN,
	eMA_SCRIPT_WALK,
};

enum LIMIT_RACE
{
	eLimitRace_Common = 0,
	eLimitRace_HumanElf,
	eLimitRace_Devil,
};

enum ITEM_KIND
{
	ITEM_KIND_HP_RECOVER		= 1,	// ������ ȸ����
	ITEM_KIND_MP_RECOVER		= 2,	// ��� ȸ����
//---KES 
	ITEM_KIND_HP_RECOVER_INSTANT	= 3,	// ������ ȸ����
	ITEM_KIND_MP_RECOVER_INSTANT	= 4,	// ��� ȸ����
	ITEM_KIND_HPMP_RECOVER			= 5,	// ������� ���� ȸ����
	ITEM_KIND_HPMP_RECOVER_INSTANT	= 6,	// ������� ���� ȸ����
//-----
	ITEM_KIND_FARM_PLANT		= 7,	// ������ ���۹� �ɱ�
	ITEM_KIND_FARM_MANURE		= 8,	// ������ ���۹� ��� �ֱ�
	ITEM_KIND_BUFF_ITEMS		= 11,	// ������ ��ɵ��� ����� ���� �����۵�.
	ITEM_KIND_FARM_HARVEST		= 12,	// ������ ���۹� ��Ȯ
	ITEM_KIND_COOLTIME			= 13,	// 080820 LUJ, ȹ�� �ÿ� ��Ÿ���� ���� �ִ� ������

	ITEM_KIND_CHANGESIZE_UPWEIGHT	= 20,	// ĳ���� Ű���� - Ŀ��Ȯ�� ����ġ
	ITEM_KIND_CHANGESIZE_DNWEIGHT	= 21,	// ĳ���� Ű���� - �۾���Ȯ�� ����ġ
	ITEM_KIND_CHANGESIZE_DEFALUT	= 22,	// ĳ���� Ű���� - �������

	ITEM_KIND_ADDRECIPE			= 23,	// �丮 - �����ǹ���
	//ITEM_KIND_DELRECIPE			= 24,	// �丮 - �����������
	ITEM_KIND_FOODSKILLBOOK		= 25,	// �丮 - ��ų��
	ITEM_KIND_FOOD				= 26,	// �丮 - �丮

	//090527 pdy �Ͽ�¡ 
	ITEM_KIND_FURNITURE						= 30,
	ITEM_KIND_FURNITURE_WALLPAPER			= 31,
	ITEM_KIND_FURNITURE_FLOORPAPER			= 32,
	ITEM_KIND_FURNITURE_CEILINGPAPER		= 33,
	ITEM_KIND_VISIT_HOUSE					= 34,	// �Ͽ콺�湮
	ITEM_KIND_VISIT_HOME					= 35,	// my�Ͽ콺����
	ITEM_KIND_EXTEND_HOUSE					= 36,	// �Ͽ콺 Ȯ��

	ITEM_KIND_CHANGE_CLASS		= 49,	// 100511 ONS �����ʱ�ȭ ������	
	ITEM_KIND_RESET_STATS		= 50,	// ���� �ʱ�ȭ ������
	ITEM_KIND_RESET_SKILL		= 51,	// ��ų �ʱ�ȭ ������

	// 080611 LYW --- CommonGameDefine : ������ Ÿ�� �߰�.
	ITEM_KIND_GET_SKILLPOINT	= 52,	// ��ų ����Ʈ ȹ��.

	ITEM_KIND_PROTECT_EXP		= 53,	// ����ġ ��ȣ ������
	ITEM_KIND_RECALL_MOVE		= 54,	// ����̵� �ֹ���
	ITEM_KIND_EXTEND_INVEN		= 55,	// �κ��丮 Ȯ�� ������
	ITEM_KIND_EXTEND_STORAGE	= 56,	// â�� Ȯ�� ������
	ITEM_KIND_MEMORY_STONE		= 57,	// ��Ｎ

	// 080607 LYW --- CommonGameDefine : ������ Ÿ�� �߰�.
	ITEM_KIND_OPENSTORAGE_BYITEM = 58,	// â�� ��ȯ ������.

	ITEM_KIND_CHANGE_NAME		= 66,	// �̸� ���� ������
	ITEM_KIND_MAP_MOVE_SCROLL	= 67,	// 071201 ���̵� �ֹ���
	ITEM_KIND_GTBUFF_ITEMS		= 76,		// �����ʸ�Ʈ ���� ���������� (Ÿ�ٴ�󿡰Ը� �� �� �ִ�)
	ITEM_KIND_JOIN_EVENT		= 77,	// 080204 KTH -- �̺�Ʈ ���� ����
	ITEM_KIND_GTHP_RECOVER_INSTANT	= 78,	// �����ʸ�Ʈ ���� HP����
	ITEM_KIND_GTMP_RECOVER_INSTANT	= 79,	// �����ʸ�Ʈ ���� MP����
	ITEM_KIND_WORLDSHOUT		= 80,	// 080312 NYJ -- ��ü��ġ��(����Ȯ����) ������
	ITEM_KIND_FISHING_BAIT		= 81,	// 080410 NYJ -- ���� �̳�������
	ITEM_KIND_FISHING_FISH		= 82,	// 080410 NYJ -- ���� ������
	ITEM_KIND_RESET_REGIDENT_REGIST	= 88,// 080215 KTH -- �ֹε�� �ʱ�ȭ ������
	ITEM_KIND_RETURN_SCROLL		= 99,	// ��ȯ �ֹ���

 	// 080826 LYW --- CommonGameDefine : ���ͽõ� ������ Ÿ�� �߰�.
	ITEM_KIND_SIEGEWARFARE_WATERSEED = 100,

	ITEM_KIND_CHALLENGEZONE_ENTER_FREQ	= 200,			// ç���� �� ���� Ƚ�� ����
	ITEM_KIND_ANIMAL			= 500,	// 080415 KTH -- ����
	ITEM_KIND_ANIMAL_FOOD		= 501,	// 080507 KTH -- ����
	ITEM_KIND_ANIMAL_CLEANER	= 502,	// 080514 KTH -- ��� û�� ����
	ITEM_KIND_PET							= 600,
	ITEM_KIND_PET_SKILL						= 601,
	ITEM_KIND_PET_COSTUME					= 602,
	ITEM_KIND_PET_EQUIP						= 603,

	ITEM_KIND_PET_EGG						= 700,
	ITEM_KIND_PET_FOOD						= 701,
	ITEM_KIND_PET_NORMAL_RESURRECTION		= 702,
	ITEM_KIND_PET_SPECIAL_RESURRECTION		= 703,
	ITEM_KIND_PET_HP_RECOVER				= 704,
	ITEM_KIND_PET_MP_RECOVER				= 705,
	ITEM_KIND_PET_EVOLUTION_PYHSIC_TYPE		= 706,
	ITEM_KIND_PET_EVOLUTION_MAGIC_TYPE		= 707,
	ITEM_KIND_PET_EVOLUTION_SUPPORT_TYPE	= 708,
	ITEM_KIND_SUMMON_NPC					= 800,
	ITEM_KIND_SUMMON_MONSTER				= 801,
	ITEM_KIND_SUMMON_EFFECT					= 802,
	ITEM_KIND_SUMMON_VEHICLE				= 803,
	ITEM_KIND_DUNGEON_KEY					= 900,
	ITEM_KIND_LEVEL_UP						= 910,
	ITEM_KIND_GUILDPOINT_UP					= 911,
	ITEM_KIND_FAMILYPOINT_UP				= 912,
	ITEM_KIND_EXP_UP						= 913,
	ITEM_KIND_PCROOMPOINT_UP				= 914,
	ITEM_KIND_FISHINGEXP_UP					= 915,
	ITEM_KIND_FISHPOINT_UP					= 916,
	ITEM_KIND_COOKLEVEL_UP					= 917,
	ITEM_KIND_DECOUSEPOINT_UP				= 918,
	ITEM_KIND_PKPOINT_UP					= 919,
	ITEM_KIND_PKPOINT_DN					= 920,
};
// E ����ý��� �߰� added by hseos 2007.05.14

/// ������ ���� ������
enum eItemCategory
{
	eItemCategory_Etc,
	eItemCategory_Equip,
	eItemCategory_Expend,
	eItemCategory_Material,
	eItemCategory_Quest,
	eItemCategory_Max,
};

enum eEquipType
{
	eEquipType_None,
	eEquipType_Weapon,
	eEquipType_Armor,
	eEquipType_Accessary,
	eEquipType_Pet,
	eEquipType_Max,
};

enum ePetEquipSlot
{
	ePetEquipSlot_Helmet,
	ePetEquipSlot_Armor,
	ePetEquipSlot_Glove,
	ePetEquipSlot_Accessaries,
	ePetEquipSlot_Weapon,
	
	ePetEquipSlot_Face,
	ePetEquipSlot_Head,
	ePetEquipSlot_Back,
	ePetEquipSlot_Tail,
	
	ePetEquipSlot_Skill,

	ePetEquipSlot_Skill1 = ePetEquipSlot_Skill,
	ePetEquipSlot_Skill2,
	ePetEquipSlot_Skill3,
	ePetEquipSlot_Skill4,
	ePetEquipSlot_Skill5,
	ePetEquipSlot_Skill6,


	ePetEquipSlot_Max,
};

enum eWeaponAnimationType
{
	eWeaponAnimationType_None,
	eWeaponAnimationType_OneHand,
	eWeaponAnimationType_TwoHand,
	eWeaponAnimationType_Dagger,
	eWeaponAnimationType_Staff,
	eWeaponAnimationType_Bow,	
	eWeaponAnimationType_Gun,
	eWeaponAnimationType_TwoBlade,
	// 090528 ONS ���� ��չ��� �Ӽ� �߰�
	eWeaponAnimationType_BothDagger,
	// 090204 LUJ, �Ӽ� �߰�
	eWeaponAnimationType_Spear,
	eWeaponAnimationType_Max,
};

enum eWeaponType
{
	eWeaponType_None,
	eWeaponType_Sword,
	eWeaponType_Mace,
	eWeaponType_Axe,
	eWeaponType_Dagger,
	eWeaponType_Staff,
	eWeaponType_Bow,
	eWeaponType_Gun,
	eWeaponType_FishingPole,
	eWeaponType_CookUtil,
	// 090528 ONS �ű����� ���� Ÿ�� �Ӽ� �߰�
	eWeaponType_BothDagger,
	eWeaponType_Max,
};

enum eArmorType
{
	eArmorType_None,
	eArmorType_Robe,
	eArmorType_Leather,
	eArmorType_Metal,
	// 090819 ShinJS --- ���� �������̽� ����, ����/��� ������ ���� Armor Type �߰�
	eArmorType_Helmet,
	eArmorType_Shield,
	eArmorType_Max,
};

enum eAccessaryType
{
	eAccessaryType_None,
	eAccessaryType_Ring,
	// 090820 ShinJS --- ���� �������̽� ����, ����̿� �Ͱ��̰� �����۸���Ʈ�� ���� ���� �ٲ�� �־� ����
	eAccessaryType_Necklace,
	eAccessaryType_Earring,
	eAccessaryType_Belt,
	eAccessaryType_Band,
	eAccessaryType_Max,
};

// 100104 ShinJS --- ������ Point Type
enum eItemPointType
{
	eItemPointType_None,
	eItemPointType_Item,
	eItemPointType_PCRoomPoint,
	eItemPointType_GuildPoint,
	eItemPointType_FamilyPoint,
	eItemPointType_Exp,
	eItemPointType_PKPoint,
	eItemPointType_FishPoint,
};

#define PET_EQUIP_SLOT_START 70
#define MAX_JOB	3
#define MAX_NPC_NUM 300
#define	MAX_MOVENPC_NUM	10						// 090227 ShinJS --- �̵�NPC �ִ� ���� 
#define MAX_DROPITEM_NUM 20
#define MAX_CHARACTER_LEVEL_NUM		120			// A��I�Ϩ���E����?AI �ˢ�ˢ���Ii��o��Ei��u�ˢ碮�� ����uo

#define MAX_PLAYERLEVEL_NUM	121
#define MAX_MONSTERLEVELPOINTRESTRICT_NUM	9
#define MONSTERLEVELRESTRICT_LOWSTARTNUM	6

#define MAX_COMBO_NUM		3
#define SKILL_COMBO_NUM		100
#define SKILLRESULTKIND_NONE 0

enum ReviveFlag
{
	ReviveFlagNone,
	ReviveFlagHere,
	ReviveFlagTown,
};

enum eItemUse_Err
{
	eItemUseSuccess,
	eItemUseSuccess_ResetRegidentCard,
	eItemUseSuccess_ChangeItem,
	// E ü���������� �߰� added by hseos 2007.08.29

	eItemUseErr_PreInsert,
	eItemUseErr_Move,
	eItemUseErr_Combine,
	eItemUseErr_Divide,
	eItemUseErr_Discard,
	eItemUseErr_Quest,
	eItemUseErr_Err,
	eItemUseErr_Upgrade,
	eItemUseErr_Lock,
	eItemUseErr_AlreadyUse,
	eItemUseErr_Unabletime,
	eItemUseErr_AlreadyPoint,
	eItemUseErr_AlredyChange,
	eItemuseErr_DontUseToday,
	eItemUseErr_DontDupUse,
	eItemUseErr_UseFull,
	// desc_hseos_����ý���_01
	// S ����ý��� �߰� added by hseos 2007.08.29
	eItemUseErr_FullInven,
	eItemUseErr_WithNoneMsg,
	// E ����ý��� �߰� added by hseos 2007.08.29

	eItemUseUnsealed,	//080317 NYJ ��������
	eItemUseErr_NoSummonInfo,

 	// 080607 LYW --- CommonGameDefine : â�� ��ȯ ������ ��� ���� ���� �߰�.
	eItemUse_OpenStorage_byItem_Failed_Storage_IsZero,
	// 080609 LYW --- CommonGameDefine : â�� ��ȯ ������ ��� ���� Ÿ�� �߰�.
	eItemUse_OpenStorage_Success,
	// 080902 LYW --- CommonGameDefine : 
	eItemUseErr_WaterseedErr,

	// 080904 LYW --- CommonGameDefine : ���� �� ����� �� ���� ������ �޽��� ���.
	eItemUseErr_SiegeWarfareForbiddenItem,

	eItemUseErr_ForbiddenSummon,	// 081027 NYJ ��ȯ����

	eItemUseErr_InvalidCookGrade,
	eItemUseErr_LowerLevelForEat,
	eItemUseErr_AlreadyLearnRecipe,
	eItemUseErr_FullLearnRecipe,

	eItemUseErr_HaveNoHouse,

	// 100218 ONS �ֹε�� �ʱ�ȭ ������ ��� ���� ���� �߰�
	eItemUseErr_NotRegist,
	eItemUseErr_PlayerIsGod,
	eItemUseErr_SkillCanNotStart,
	eltemUseErr_Stun,
};

/// GameResource��E?��E��c �ˢ�ic��E?e��IiC��E��IA define ///////////////////////////////////////////////
// 091125 ONS ��������� �ִ밹�� 100 -> 1000���� ����
#define MAX_YOUNGYAKITEM_DUPNUM			1000
// 061211 LYW --- Adjust max character count.
//#define MAX_CHARACTER_NUM	4
#define MAX_CHARACTER_NUM	5
#define MAX_STREETSTALL_TITLELEN 66

#define MAX_FILE_NAME		32
enum { STR_POINT, WIS_POINT, DEX_POINT, VIT_POINT, INT_POINT, MAX_BTN_POINT};

#define GENDER_MALE 0
#define GENDER_FEMALE 1
#define GENDER_MAX 2

#define MAX_PARTY_LISTNUM 7
#define MAX_PARTY_LOAD_NUM 100

// 070105 LYW --- Define max count for family member.
#define MAX_FAMILY_LISTNUM 5
#define MAX_BASESTORAGELIST_NUM 2
#define MAX_STORAGELIST_NUM 5
#define MAX_FRIEND_NUM 30
#define MAX_FRIEND_LIST 10
#define MAX_FRIEND_PAGE MAX_FRIEND_NUM/MAX_FRIEND_LIST // 30/10 = 10

// 100118 ONS ģ������ ���̾�α׿� PC���������� ����߰�
#define MAX_PCROOM_MEMBER_NUM 30
#define MAX_PCROOM_MEMBER_LIST 10

#define MAX_NOTENUM 88 //40
#define MAX_NOTE_PAGE 8 //4
#define NOTENUM_PER_PAGE MAX_NOTENUM/MAX_NOTE_PAGE // 30/3 = 10
#define MAX_NOTE_LENGTH 600//255

#define MAX_CHANNEL_NUM 10
#define MAX_CHANNEL_NAME 16

enum EWEARED_ITEM
{
	eWearedItem_Weapon,
	eWearedItem_Shield,
	eWearedItem_Dress,
	eWearedItem_Hat,
	eWearedItem_Glove,
	eWearedItem_Shoes,

	eWearedItem_Ring1,
	eWearedItem_Ring2,
	eWearedItem_Necklace,
	eWearedItem_Earring1,
	eWearedItem_Earring2,
	eWearedItem_Belt,
	eWearedItem_Band,

	eWearedItem_Glasses,
	eWearedItem_Mask,
	eWearedItem_Wing,

	eWearedItem_Costume_Head,
	eWearedItem_Costume_Dress,
	eWearedItem_Costume_Glove,
	eWearedItem_Costume_Shoes,

	eWearedItem_Costume_Wing,

	eWearedItem_Max,
	eWearedItem_None = 999,
};

enum ERROR_GUILD
{
	eGuildErr_None	= 0,
	eGuildErr_Err	= 1,
			
	eGuildErr_Create_Invalid_Name,
	eGuildErr_Create_Already,
	eGuildErr_Create_Low_Level,
	eGuildErr_Create_Insufficient_Money,
	eGuildErr_Create_Unable_Entry_Data,
	
	eGuildErr_BreakUp,
	
	eGuildErr_DeleteMember,
	eGuildErr_DeleteMember_NothaveAuthority,
	eGuildErr_DeleteMember_NotMember,
	
	eGuildErr_AddMember_NotPlayer,
	eGuildErr_AddMember_OtherGuild,
	eGuildErr_AddMember_AlreadyMember,
	eGuildErr_AddMember_FullMember,	
	eGuildErr_AddMember_TargetNotDay,
	eGuildErr_AddMember_NothaveAuthority,
	
	eGuildErr_InviteApt_NoMaster,

	eGuildErr_Mark_NoGuild,
	eGuildErr_Mark_NoMark,

	eGuildErr_LevelUp_NotAbil,
	eGuildErr_LevelUp_NotMoney,
	eGuildErr_LevelUp_Complete,

	eGuildErr_ChangeRank_NoGuild,
	eGuildErr_ChangeRank_NotMember,
	eGuildErr_ChangeRank_LowerLevel,
	eGuildErr_ChangeRank_Master,
	eGuildErr_ChangeRank_AlreadyHave,
	eGuildErr_ChangeRank_NoAuthority,
	eGuildErr_ChangeRank_NoGraduatedStudent,
	eGuildErr_ChangeRank_Student,

	eGuildErr_NoGuild,

	eGuildErr_Nick_Filter,
	eGuildErr_Nick_NotMember,
	eGuildErr_Nick_NotAvailableName,
	eGuildErr_Nick_NotLogIn,

	eGuildErr_AddStudent_TooHighLevelAsStudent,
	eGuildErr_AddStudent_Full,

	// 080417 LUJ, ��� ��ų �߰� ����
	eGuildErr_InsufficentMoney	=	100,
	eGuildErr_InsufficentScore,
	eGuildErr_NoMaster,
	eGuildErr_NoScript,
	eGuildErr_Skill_Max,
	eGuildErr_Skill_InvalidLevel,
	eGuildErr_Skill_NotExist,

	// 090317 NYJ - �������� ���͸����� ����
	eGuildErr_Notice_FilterCharacter,
};
enum
{
	eGuild_Delete,
	eGuild_Secede,		
};

enum ERROR_FAMILY
{
	eFamilyErr_Err = 1,
			
	eFamilyErr_Create_Name,
	eFamilyErr_Create_Intro,
	
	eFamilyErr_BreakUp,
	
	eFamilyErr_DeleteMember,
	eFamilyErr_DeleteMember_NothaveAuthority,
	eFamilyErr_DeleteMember_NotMember,
	
	eFamilyErr_AddMember_NotPlayer,
	eFamilyErr_AddMember_OtherFamily,
	eFamilyErr_AddMember_AlreadyMember,
	eFamilyErr_AddMember_FullMember,	
	eFamilyErr_AddMember_TargetNotDay,
	eFamilyErr_AddMember_NothaveAuthority,

	eFamilyErr_InviteApt_NoMaster,

	eFamilyErr_Mark_NoFamily,
	eFamilyErr_Mark_NoMark,

	eFamilyErr_LevelUp_NotAbil,
	eFamilyErr_LevelUp_NotMoney,
	eFamilyErr_LevelUp_Complete,

	eFamilyErr_ChangeRank_NoFamily,
	eFamilyErr_ChangeRank_NotMember,

	eFamilyErr_NoFamily,

	eFamilyErr_Nick_Filter,
	eFamilyErr_Nick_NotMember,
	eFamilyErr_Nick_NotAvailableName,
	eFamilyErr_Nick_NotLogIn,

	eFamilyErr_AddStudent_TooHighLevelAsStudent,
};
enum
{
	eFamily_Delete,
	eFamily_Secede,		
};

//--------��E?��E��c�ˢ�ˢ�? ��E�������ע��Ϣ���oAAo----------------------------------------------------------
// 090227 ShinJS --- Coin���� ���� �߰�(NOT_COIN)
enum DEAL_BUY_ERROR { NOT_ERROR, NOT_MONEY = 101, NOT_SPACE, NOT_EXIST, NOT_PLAYER, NOT_OWNERMONEY, NOT_SAMEPRICE, NOT_SAMEINFO, NOT_FISHINGPOINT, OVER_INVENMONEY, NOT_COIN, NOT_PCROOMPOINT, NOT_PCROOM };

// 090107 LYW --- CommonGameDefine : ������ ���� �Ǹ� ���� �з� �߰�.
enum DEAL_SELL_ERROR
{
	ERR_NONE = 0,			// �����ƴ�.
	ERR_NOTSELLITEM,		// ���ǰ ������.
	ERR_NOTINVENITEM,		// �κ��丮�� �������� �ƴ�.
	ERR_INVALSLOT,			// ������ SLOT.
	ERR_INVALITEMINFO,		// ������ ITEM.
	ERR_INVALPLAYER,		// ������ PLAYER.
	ERR_LOOTPLAYER,			// �������� PLAYER.
	ERR_INVALNPC,			// ������ NPC.
	ERR_INVALIDENTITY,		// ������ ���ϼ� üũ ����.
	ERR_OVERSTACKCOUNT,		// ���� ī��Ʈ ����.
	ERR_MAXMONEY,			// �������ɱݾ� ����.
	ERR_FAILDISCARD,		// ������ ����ó�� ����.
} ;

enum ERROR_MUNPABOARDCONTENTS {
	ERROR_MUNPABOARDCONTENTS_NOCONTENTS = 1,
	ERROR_MUNPABOARDCONTENTS_NOAUTHORITY,
};

enum ERROR_PARTY 
{
	eErr_Create_DB,
	eErr_Add_NoPlayer,
	eErr_Add_AlreadyinParty,
	eErr_Add_OptionNoParty,
	eErr_Add_DiedPlayer,
	eErr_ChangingMaster,
	eErr_Add_Full, //can not join cause party is full!
	eErr_BrokenParty, // Party has already broken.
	eErr_Create_MapHasParty, //map has party but client doesn't have party
	eErr_Add_NotOurGuild,		// During Guild Tournament
	// 070110 LYW --- Add error message for party.
	eErr_Fail_Change_PickupItem,
	eErr_Success_Change_PickupItem,
	eErr_Fail_Change_DiceGrade,
	eErr_Success_Change_DiceGrade,
	// 090528 ShinJS --- �̸��˻����� ��Ƽ�ʴ�� ���� �߰�
	eErr_AlreadySearchingByName,	// �˻����� ���
	eErr_Add_NotConnectedPlayer,	// �ʴ����� ���������� ���� ���
	eErr_SettingLimitTime,			// �ִ� �ʴ��û �ʰ��� ���ѽð� ����
};

enum PARTY_OPTION
{
	ePartyOpt_Damage,
	ePartyOpt_Sequence,
};

#define PARTY_DICEGRADE_DEFAULT 3

// 090528 ShinJS --- �̸����� ��Ƽ�ʴ��û�� ���� ���Ѽ���
enum PARTY_INVITE_BYNAME_SETTING
{
	ePartyInvite_LimitCnt = 2,									// �ִ� �ʴ��û ����ȸ�� (�ʰ��� eOverCntLimitTime ��ŭ �������)
	ePartyInvite_CheckSearchTimeNoPlayer = 5000,				// �̸��˻��� �˻�ð� - �÷��̾ �������� �ʴ°��
	ePartyInvite_CheckSearchTimeNotConnectPlayer = 5000,		// �̸��˻��� �˻�ð� - �÷��̾ �������� �������
	ePartyInvite_CheckSearchTimeAlreadyInParty = 5000,			// �̸��˻��� �˻�ð� - �÷��̾ �̹� �ٸ���Ƽ�� ���� ���
	ePartyInvite_CheckSearchTimeSuccess = 5000,					// �̸��˻��� �˻�ð� - �˻��� ������ ���
	ePartyInvite_OverCntLimitTimeNoPlayer = 5000,				// ���� �ʰ��� ���ѽð� ���� - �÷��̾ �������� �ʴ°��
	ePartyInvite_OverCntLimitTimeAlreadyInParty = 5000,			// ���� �ʰ��� ���ѽð� ���� - �÷��̾ �̹� �ٸ���Ƽ�� ���� ���
	ePartyInvite_OverCntLimitTimeNotConnectPlayer = 5000,		// ���� �ʰ��� ���ѽð� ���� - �÷��̾ �������� �������
	ePartyInvite_OverCntLimitTimeSuccess = 5000,				// ���� �ʰ��� ���ѽð� ���� - �˻��� ������ ���
};

//--------��E?��E��c�ˢ�ˢ�? ��E�������ע��Ϣ���oAAo----------------------------------------------------------

enum TABNUMBER
{
	TAB_INVENTORY_NUM		= 2,
	TAB_MUNPAWAREHOUSE_NUM	= 7,
	TAB_BASE_STORAGE_NUM	= 2,
	TAB_STORAGE_NUM			= TAB_BASE_STORAGE_NUM + 3,
	TAB_GUILDWAREHOUSE_NUM	= 2,
};

enum CELLPERTAB
{
	TABCELL_INVENTORY_NUM		= 20,
	TABCELL_MUNPAWAREHOUSE_NUM	= 45,
	TABCELL_STORAGE_NUM			= 36,
	TABCELL_GUILDWAREHOUSE_NUM	= 36,
};

enum
{
	SLOT_INVENTORY_NUM			= TAB_INVENTORY_NUM * TABCELL_INVENTORY_NUM,
	SLOT_EXTENDED_INVENTORY1_NUM = TABCELL_INVENTORY_NUM,
	SLOT_EXTENDED_INVENTORY2_NUM = TABCELL_INVENTORY_NUM,
	SLOT_MAX_INVENTORY_NUM		= SLOT_INVENTORY_NUM + SLOT_EXTENDED_INVENTORY1_NUM + SLOT_EXTENDED_INVENTORY2_NUM,	// 80
	SLOT_WEAR_NUM				= 21,
	SLOT_MUNPAWAREHOUSE_NUM		= TAB_MUNPAWAREHOUSE_NUM * TABCELL_MUNPAWAREHOUSE_NUM,	//315
	SLOT_STORAGE_NUM			= TAB_STORAGE_NUM * TABCELL_STORAGE_NUM,				//180
	SLOT_GUILDWAREHOUSE_NUM		= TAB_GUILDWAREHOUSE_NUM * TABCELL_GUILDWAREHOUSE_NUM,	//72
	SLOT_SHOPITEM_NUM			= 36,
	SLOT_STREETSTALL_NUM		= 24,
	SLOT_STREETBUYSTALL_NUM		= 5,	
};

enum
{
	TP_INVENTORY_START				= 0,
	TP_INVENTORY_END				= TP_INVENTORY_START		+ SLOT_INVENTORY_NUM,		// 40
	TP_EXTENDED_INVENTORY1_START	= TP_INVENTORY_END,
	TP_EXTENDED_INVENTORY1_END		= TP_EXTENDED_INVENTORY1_START	+ SLOT_EXTENDED_INVENTORY1_NUM,	// 60
	TP_EXTENDED_INVENTORY2_START	= TP_EXTENDED_INVENTORY1_END,
	TP_EXTENDED_INVENTORY2_END		= TP_EXTENDED_INVENTORY2_START	+ SLOT_EXTENDED_INVENTORY2_NUM,	// 80
	TP_WEAR_START					= TP_EXTENDED_INVENTORY2_END,
	TP_WEAR_END						= TP_WEAR_START				+ SLOT_WEAR_NUM,			// 100
	TP_STORAGE_START				= TP_WEAR_END,											// 100
	TP_STORAGE_END					= TP_STORAGE_START			+ SLOT_STORAGE_NUM,			// 280
	TP_SHOPITEM_START				= TP_STORAGE_END,										// 280
	TP_SHOPITEM_END					= TP_SHOPITEM_START			+ SLOT_SHOPITEM_NUM,		// 316
	TP_MAX							= TP_SHOPITEM_END,										// 316
	TP_GUILDWAREHOUSE_START			= 500,
	TP_GUILDWAREHOUSE_END			= TP_GUILDWAREHOUSE_START + SLOT_GUILDWAREHOUSE_NUM,	// 572
};

enum eITEMTABLE
{
	eItemTable_Inventory,
	eItemTable_Weared,
	eItemTable_Storage,
	eItemTable_Shop,
	eItemTable_TableMax,
	eItemTable_MunpaWarehouse,
	eItemTable_Deal,
	eItemTable_Max,
};

enum MONEY_FLAG {
	MF_NOMAL = 0, MF_OBTAIN, MF_LOST, MF_NONE, MF_PUTINGUILD = 20, MF_PUTOUTGUILD, MF_PUTINFAMILY, MF_PUTOUTFAMILY, MF_FEE,
};

enum EAPPEAR_PART
{
	eAppearPart_Body,
	eAppearPart_Hair,
	eAppearPart_Face,
	eAppearPart_Hand,
	eAppearPart_Foot,
	eAppearPart_Weapon,
	
	eAppearPart_Max
};

#define WALKSPEED		225
#define RUNSPEED		450


#define MOVE_LIMIT_ERRORDISTANCE	1000
#define MAX_NAME_LENGTH				16
#define MAX_SKILL_NAME_LENGTH		32
#define MAX_NPC_NAME_LENGTH		32
#define MAX_MAP_NAME_LENGTH		32
#define MAX_MONSTER_NAME_LENGTH		60
#define MAX_CHAT_LENGTH				127
#define	MAX_CHANNELTITLE_LENGTH		64
#define	MAX_PLAYROOMTITLE_LENGTH	64
#define MAX_ITEMNAME_LENGTH			32
#define MAX_TARGET_NUM		300

#define MAX_SENDDATE_LENGTH		16
#define MAX_NOTE_TITLE			36


#define MIN_CHAR_CHANGESIZE	0.3f
#define MAX_CHAR_CHANGESIZE	3.0f
#define POINT_VALID_DISTANCE		6500


#define REMOVEFADE_START_TIME		2500
#define MONSTERREMOVE_TIME			5000
#define PLAYERREVIVE_TIME			5000
// ���ð��� Define
#define MAX_FISHING_LEVEL			4			// ���ü��õ� �ܰ�
#define MAX_FISHING_MISSION			3			// ���ù̼ǰ���
#define MAX_FISHLIST_FORPOINT		12			// �����⸮��Ʈ : ����Ʈ��ȯ�� ���
#define MAX_FISHINGMISSIONCODE		99			// 
#define MAX_FISHING_DISTANCE		2300.0f		// ���� ������ �Ÿ�
#define MAX_FISHPOINT				9999999		// �ִ� ����������Ʈ 9,999,999
#define FISHINGTIME_STARTPULLING	2000		// ������ ���۽ð�
#define FISHINGMISSION_INVALID		1000000		// �̼��ڵ�� 000000 ~ 999999 �����̹Ƿ� �̸� �ʰ��ϸ� ��ȿ��.

enum eCOOKLEVEL{eCOOKLEVEL1=1, eCOOKLEVEL2=2, eCOOKLEVEL3=3, eCOOKLEVEL4=4};
enum eCOOKRECIPE{eCOOKRECIPE_ADD=1, eCOOKRECIPE_DEL=2, eCOOKRECIPE_UPDATE=3};
#define ERR_FULLLEANRN_RECIPE		-2
#define ERR_ALREADYLEARN_RECIPE		-1
#define MAX_COOKING_LEVEL			4					// �ִ� �丮���ôܰ� 4��
#define MAX_INGREDIENT_LIST			5					// �ִ� ����� 5��
#define MAX_RECIPE_LV4_LIST			10					// DB�� ��ϵǴ� �ִ� ���η����Ǹ�� 10��
#define MAX_RECIPE_DISPLIST			99					// UI�� ��µǴ� �ִ� �����Ǹ�� 99��
#define MAX_COOKING_DISTANCE		300.0f				// �丮 ������ �Ÿ�
#define LV4FOOD_EAT_LEVEL			75					// ���� �丮�� ���� �� �ִ� ����

// �Ͽ�¡���� Define
enum {eHOUSEFURNITURE_ADD=1, eHOUSEFURNITURE_DEL=2, eHOUSEFURNITURE_UPDATE=3};
enum eHOUSEFURNITURE_STATE
{
	eHOUSEFURNITURE_STATE_UNKEEP	= 0,		// �κ��丮�� ������ (RemainTime ��������)
	eHOUSEFURNITURE_STATE_KEEP		= 1,		// ����â���� ������ (RemainTime ��������)
	eHOUSEFURNITURE_STATE_INSTALL	= 2,		// �Ͽ콺�� ��ġ���� (RemainTime ������)
	eHOUSEFURNITURE_STATE_UNINSTALL	= 3,		// �Ͽ콺�� ��ġ�ߴٰ� ����â���� ������ (RemainTime ������)
};
enum {
	eHOUSEERR_NONE			= 0,	// ����(��������)
	eHOUSEERR_NOOWNER		= 1,	// �����ھ���
	eHOUSEERR_NOHOUSE		= 2,	// �Ͽ콺����
	eHOUSEERR_HAVEHOUSE		= 3,	// �Ͽ콺����
	eHOUSEERR_ENTRANCEFAIL	= 4,	// �������
	eHOUSEERR_LOADING		= 5,	// DB���� �����ͷε���
	eHOUSEERR_FULL			= 6,	// �Ͽ콺�����ʰ�
	eHOUSEERR_DONOT_HOUSE	= 7,	// �Ͽ콺���� ó���Ұ���
	eHOUSEERR_DO_HOUSE		= 8,	// �Ͽ콺���� ����
	eHOUSEERR_NOTOWNER		= 9,	// �����ڰ� �ƴ�
	eHOUSEERR_MAX_EXTEND	= 10,	// �Ͽ콺Ȯ�� �ִ�
	eHOUSEERR_DECOMODE		= 11,	// �ٹ̱����� �Ұ�
	eHOUSEERR_NOTENOUGHPOINT = 12,	// DecoUse����Ʈ ����
	eHOUSEERR_INVAILDSTATE	= 13,	// ��ȿ���� ���� ����
	eHOUSEERR_NOFURNITURE	= 14,	// ����������
	eHOUSEERR_HASRIDER		= 15,	// ž���� ����
	eHOUSEERR_NOTRIDING		= 16,	// ž�»��� �ƴ�
	eHOUSEERR_RIDING		= 17,	// ž����
	eHOUSEERR_RESERVATING	= 18,	// �湮������
	eHOUSEERR_DISTANCE_FAR	= 19,	// �Ÿ��� �ִ�
	eHOUSEERR_NOTREGIST		= 20,	// �ֹε�� ������.
	eHOUSEERR_HOME2HOME_FAIL = 21,	// Ȩ->Ȩ �̵�����
	eHOUSEERR_ONVEHICLE		= 22,	// Ż�ͼ�ȯ or ž����
	eHOUSEERR_NORANKER		=23,	// ��Ŀ�� �ƴ� 		

	eHOUSEERR_ERROR			   ,
};

enum eHouseLog
{
	// ������(�κ�)<->����(����â��)
	eHouseLog_Keep	=	1,
	eHouseLog_UnKepp,

	// ����â��
	eHouseLog_Install,
	eHouseLog_UnInstall,
	eHouseLog_EndTime,	//�ð��Ҹ�
	eHouseLog_Destroy,

	// �Ͽ�¡���� �����ۻ��(�κ�)
	eHouseLog_WallPaper,
	eHouseLog_FloorPaper,
	eHouseLog_CeillingPaper,
	eHouseLog_Extend,
	eHouseLog_ChangeHouseName,
};

enum {eHouseVisitByCheat=0, eHouseVisitByLink=1, eHouseVisitByItemRandom=2, eHouseVisitByItem=3, eHouseVisitByName=4};
enum {eHOUSEEXTEND_NONE=0, eHOUSEEXTEND_LV1=1, eHOUSEEXTEND_LV2=2, eHOUSEEXTEND_LV3=3, eHOUSEEXTEND_LV4=4};
#define MAX_HOUSING_CATEGORY_NUM	8		// ī�װ��� : [0]�ӽ� [1]���� [2]���� [3]�� [4]�� [5]���� [6]��ǰ [7]���
#define MAX_HOUSING_SLOT_NUM		60		// ����â�� ���԰���
#define MAX_HOUSING_NAME_LENGTH		32		// �Ͽ�¡���� �̸�
#define MAX_FURNITURE_STORAGE_NUM	(MAX_HOUSING_CATEGORY_NUM * MAX_HOUSING_SLOT_NUM)	// ����â�� �ִ뺸�����
//#define MAX_BONUS_LIST				20		// �ִ� �ٹ̱�ȿ�� ����
#define MAX_HOUSE_DECOUSEPOINT		100		// �ٹ̱� �Ҹ�����Ʈ

#define MAX_HOUSE_NUM				1000	// ���� �� �ִ� �ִ� �Ͽ콺���� (�����δ� MAX_HOUSE_NUM - 1 ��ŭ ������)

#define HOUSE_INSTALL_MARGIN		1
#define	CHECK_PECENT				50.0f
#define	MAX_HOUSE_EXTEND_LEVEL		eHOUSEEXTEND_LV4
#define MAX_HOUSE_ACTION_SLOT		5
#define MAX_HOUSE_ATTATCH_SLOT		5
#define MAX_HOUSE_ACTION_DISTANCE	500.0f
#define MAX_START_FURNITURE_HEIGHT	6.0f

#define HOUSE_ACTIONINDEX_GETOFF	7		// ������ �׼��ε��� 

enum HOUSE_ACTION_TYPE
{
	eHOUSE_ACTION_TYPE_INVALID	= 0,		// ���������
	eHOUSE_ACTION_TYPE_RIDE		= 1,		// ž��
	eHOUSE_ACTION_TYPE_GETOFF	= 2,		// ������
	eHOUSE_ACTION_TYPE_BUFF		= 3,		// ����
	eHOUSE_ACTION_TYPE_NORMAL	= 4,		// �ִ�,����Ʈ
	eHOUSE_ACTION_TYPE_STORAGE	= 5,		// â��

	eHOUSE_ACTION_TYPE_DOOR_EXITHOUSE	= 1000,
	eHOUSE_ACTION_TYPE_DOOR_VOTEHOUSE	= 1001,

	// 091105 pdy �Ͽ�¡ ���� �׼��߰� ( �̴�Ȩ�� �� ������ ��ũ ) 
	eHOUSE_ACTION_TYPE_OPEN_HOMEPAGE = 2000,	
};

// �Ͽ�¡���� ���̹� ��
enum HOUSE_HIGHCATEGORY
{
	eHOUSE_HighCategory_Furniture		= 1,
	eHOUSE_HighCategory_Electric		= 2,
	eHOUSE_HighCategory_Door			= 3,
	eHOUSE_HighCategory_Wall			= 4,
	eHOUSE_HighCategory_Carpet			= 5,
	eHOUSE_HighCategory_Properties		= 6,
	eHOUSE_HighCategory_Decoration		= 7,
	
	eFN_HighCatefory_Max,
};

enum HOUSE_LOWCATEGORY_FURNITURE
{
	eHOUSE_LowCategory_Furniture_Etc		= 1,
	eHOUSE_LowCategory_Furniture_Table		= 2,
	eHOUSE_LowCategory_Furniture_Chair		= 3,
	eHOUSE_LowCategory_Furniture_Sofa		= 4,
	eHOUSE_LowCategory_Furniture_Closet		= 5,
	eHOUSE_LowCategory_Furniture_Chest		= 6,
	eHOUSE_LowCategory_Furniture_Drawer		= 7,
	eHOUSE_LowCategory_Furniture_Bed		= 8,
	
	eHOUSE_LowCategory_Furniture_Max,
};

enum HOUSE_LOWCATEGORY_ELECTRIC
{
	eHOUSE_LowCategory_Electric_Etc		= 1,
	eHOUSE_LowCategory_Electric_Ref		= 2,
	eHOUSE_LowCategory_Electric_Tv		= 3,
	
	eHOUSE_LowCategory_Electric_Max,
};

enum HOUSE_LOWCATEGORY_DOOR
{
	eHOUSE_LowCategory_Door_Etc			= 1,
	eHOUSE_LowCategory_Door_Frame		= 2,
	eHOUSE_LowCategory_Door_Window		= 3,
	eHOUSE_LowCategory_Door_Watch		= 4,
	eHOUSE_LowCategory_Door_Door		= 5,

	eHOUSE_LowCategory_Door_Max,
};

enum HOUSE_LOWCATEGORY_WALL
{
	eHOUSE_LowCategory_Wall_Etc			= 1,
	eHOUSE_LowCategory_Wall_Wall		= 2,

	eHOUSE_LowCategory_Wall_Max,
};

enum HOUSE_LOWCATEGORY_CARPET
{
	eHOUSE_LowCategory_Carpet_Etc		= 1,
	eHOUSE_LowCategory_Carpet_Tile		= 2,

	eHOUSE_LowCategory_Carpet_Max,
};

enum HOUSE_LOWCATEGORY_PROPERTIES
{
	eHOUSE_LowCategory_Properties_Etc		= 1,
	eHOUSE_LowCategory_Properties_Frame		= 2,
	eHOUSE_LowCategory_Properties_FlowerPot	= 3,
	eHOUSE_LowCategory_Properties_Cushion	= 4,
	eHOUSE_LowCategory_Properties_Clock		= 5,
	eHOUSE_LowCategory_Properties_Doll		= 6,
	eHOUSE_LowCategory_Properties_Basket	= 7,
	
	eHOUSE_LowCategory_Properties_Max,
};

enum HOUSE_LOWCATEGORY_DECORATION
{
	eHOUSE_LowCategory_Decoration_Etc		= 1,
	eHOUSE_LowCategory_Decoration_FirePlace	= 2,
	eHOUSE_LowCategory_Decoration_Mark		= 3,
	eHOUSE_LowCategory_Decoration_Lamp		= 4,
	eHOUSE_LowCategory_Decoration_Bronze	= 5,
	eHOUSE_LowCategory_Decoration_CCover	= 6,
	
	eHOUSE_LowCategory_Decoration_Max,
};

enum HOUSE_WORKINDEX
{
	eHOUSE_WorkIndex_A0		= 1,
	eHOUSE_WorkIndex_B0		= 2,
	eHOUSE_WorkIndex_C0		= 3,
	eHOUSE_WorkIndex_D0		= 4,
	eHOUSE_WorkIndex_E0		= 5,
	eHOUSE_WorkIndex_F0		= 6,

	eHOUSE_WorkIndex_Max,
};

enum HOUSE_FUNCTION
{
	eHOUSE_Function_None	= 0,
	eHOUSE_Function_Wall	= 1,
	eHOUSE_Function_Floor	= 2,
	eHOUSE_Function_Door	= 3,
	eHOUSE_Function_Start	= 4,
	eHOUSE_Function_Action	= 5,

	eHOUSE_Function_Max,
};

enum HOUSE_FURNITUREINDEX_PARTS
{
	eHOUSE_FurnitureIndex_Part_HighCategory,
	eHOUSE_FurnitureIndex_Part_LowCategory,
	eHOUSE_FurnitureIndex_Part_Work,
	eHOUSE_FurnitureIndex_Part_Index,
	eHOUSE_FurnitureIndex_Part_Action,
	
	eHOUSE_FurnitureIndex_Part_Max,
};

#define HOUSE_HIGHCATEGORY_CHAR_FURINITURE			"FU"
#define HOUSE_HIGHCATEGORY_CHAR_ELECTRIC			"EL"
#define HOUSE_HIGHCATEGORY_CHAR_DOOR				"DO"
#define HOUSE_HIGHCATEGORY_CHAR_WALL				"WA"
#define HOUSE_HIGHCATEGORY_CHAR_CARPET				"CA"
#define HOUSE_HIGHCATEGORY_CHAR_PROPERTIES			"PR"
#define HOUSE_HIGHCATEGORY_CHAR_DECORATION			"DE"

#define HOUSE_LOWCATEGORY_CHAR_FURNITURE_ETC		"ETC"
#define HOUSE_LOWCATEGORY_CHAR_FURNITURE_TABLE		"TABLE"
#define HOUSE_LOWCATEGORY_CHAR_FURNITURE_CHAIR		"CHAIR"
#define HOUSE_LOWCATEGORY_CHAR_FURNITURE_SOFA		"SOFA"
#define HOUSE_LOWCATEGORY_CHAR_FURNITURE_CLOSET		"CLOSET"
#define HOUSE_LOWCATEGORY_CHAR_FURNITURE_CHEST		"CHEST"
#define HOUSE_LOWCATEGORY_CHAR_FURNITURE_DRAWER		"DRAWER"
#define HOUSE_LOWCATEGORY_CHAR_FURNITURE_BED		"BED"

#define HOUSE_LOWCATEGORY_CHAR_ELECTRIC_ETC			"ETC"
#define HOUSE_LOWCATEGORY_CHAR_ELECTRIC_REF			"REF"
#define HOUSE_LOWCATEGORY_CHAR_ELECTRIC_TV			"TV"

#define HOUSE_LOWCATEGORY_CHAR_DOOR_ETC				"ETC"
#define HOUSE_LOWCATEGORY_CHAR_DOOR_FRAME			"FRAME"
#define HOUSE_LOWCATEGORY_CHAR_DOOR_WINDOWS			"WINDOW"//"WINDOWS"
#define HOUSE_LOWCATEGORY_CHAR_DOOR_WATCH			"WATCH"
#define HOUSE_LOWCATEGORY_CHAR_DOOR_DOOR			"DOOR"

#define HOUSE_LOWCATEGORY_CHAR_WALL_ETC				"ETC"
#define HOUSE_LOWCATEGORY_CHAR_WALL_WALL			"WALL"

#define HOUSE_LOWCATEGORY_CHAR_CARPET_ETC			"ETC"
#define HOUSE_LOWCATEGORY_CHAR_CARPET_TILE			"TILE"

#define HOUSE_LOWCATEGORY_CHAR_PROPERTIES_ETC		"ETC"
#define HOUSE_LOWCATEGORY_CHAR_PROPERTIES_FRAME		"FRAME"
#define HOUSE_LOWCATEGORY_CHAR_PROPERTIES_FLOWERPOT	"FLOWERPOT"
#define HOUSE_LOWCATEGORY_CHAR_PROPERTIES_CUSHION	"CUSHION"
#define HOUSE_LOWCATEGORY_CHAR_PROPERTIES_CLOCK		"CLOCK"
#define HOUSE_LOWCATEGORY_CHAR_PROPERTIES_DOLL		"DOLL"
#define HOUSE_LOWCATEGORY_CHAR_PROPERTIES_BASKET	"BASKET"

#define HOUSE_LOWCATEGORY_CHAR_DECORATION_ETC		"ETC"
#define HOUSE_LOWCATEGORY_CHAR_DECORATION_FIREPLACE	"FIREPLACE"
#define HOUSE_LOWCATEGORY_CHAR_DECORATION_MARK		"MARK"
#define HOUSE_LOWCATEGORY_CHAR_DECORATION_LAMP		"LAMP"
#define HOUSE_LOWCATEGORY_CHAR_DECORATION_BRONZE	"BRONZE"
#define HOUSE_LOWCATEGORY_CHAR_DECORATION_CCOVER	"CCOVER"

#define HOUSE_WORK_CHAR_A0							"A0"
#define HOUSE_WORK_CHAR_B0							"B0"
#define HOUSE_WORK_CHAR_C0							"C0"
#define HOUSE_WORK_CHAR_D0							"D0"
#define HOUSE_WORK_CHAR_E0							"E0"
#define HOUSE_WORK_CHAR_F0							"F0"

#define HOUSE_FUNCTION_CHAR_NONE					"N"
#define HOUSE_FUNCTION_CHAR_WALL					"W"
#define HOUSE_FUNCTION_CHAR_FLOOR					"F"
#define HOUSE_FUNCTION_CHAR_DOOR					"D"
#define HOUSE_FUNCTION_CHAR_START					"S"
#define HOUSE_FUNCTION_CHAR_ACTION					"A"

enum eDIFFICULTY
{
	eDIFFICULTY_NONE	= 0,
	eDIFFICULTY_EASY	= 1,
	eDIFFICULTY_NORMAL	= 2,
	eDIFFICULTY_HARD	= 3,
	eDIFFICULTY_MAX,
};


#define MAX_DUNGEON_NUM	100

enum {
	eDungeonERR_None			= 0,	// ����(��������)
	eDungeonERR_NoKeyInfo		= 1,	// Ű��������
	eDungeonERR_NoParty			= 2,	// ��Ƽ����
	eDungeonERR_NotEnoughLevel	= 3,	// �����̴�
	eDungeonERR_OverLevel		= 4,	// �����ʰ�
	eDungeonERR_EntranceDelay	= 5,	// ��������̽ð�
	eDungeonERR_OverCount		= 6,	// ����Ƚ���ʰ�
	eDungeonERR_DungeonMapOff	= 7,	// �����ʼ��� Off
	eDungeonERR_DungeonFull		= 8,	// ������ Full
	eDungeonERR_NoNPCInfo		= 9,	// NPC ���� ����

	eDungeonERR_WrongParam		= 10,	// �Ķ���� �߸�
	eDungeonERR_NoChannel		= 11,	// ä�� ����

	eDungeonERR_WrongDiffculty	= 12,	// �ٸ� ���̵��� �δ�����
	eDungeonERR_CreateFailed	= 13,	// ���� ���� ����

	eDungeonERR_ERROR			= 99,	// ������ ����
};


enum eTimerType
{
	eTimerType_CountDown	= 0,
	eTimerType_CountUp		= 1,
};

enum eTimerState
{
	// ���۰� �����ִ� ���´� Ready, Run, Pause, Stop�� �װ����̴�.
	eTimerState_Ready		= 0,
	eTimerState_Run			= 1,

	// ������ �����ϱ� ���� �ǷڵǴ� �����̴�. ����� Run, Pause, Stop���� ��Ÿ����.
	eTimerState_Pause		= 2,	// ��� : Pause
	eTimerState_Resume		= 3,	// ��� : Run
	eTimerState_Stop		= 4,	// ��� : Stop
};

#define CONSIGNMENT_DEPOSIT_RATE		0.001
#define CONSIGNMENT_COMMISSION_RATE		0.03
#define CONSIGNMENT_SEARCHINDEX_NUM		10

enum {
	eConsignmentERR_None			= 0,	// ����(��������)
	eConsignmentERR_WrongState		= 1,	// �߸��� ����
	eConsignmentERR_WrongItem		= 2,	// �߸��� ������
	eConsignmentERR_WrongParam		= 3,	// �߸��� ����
	eConsignmentERR_PoorMoney		= 4,	// ������
	eConsignmentERR_PoorSlot		= 5,	// �κ�����
	eConsignmentERR_FastRequest		= 6,	// ������û
	eConsignmentERR_GoldOver		= 7,	// ����ʰ�

	eConsignmentERR_ERROR			= 99,	// ������ ����
};

enum {
	eConsignmentSORT_NameAsc		= 0,
	eConsignmentSORT_NameDesc		= 1,
	eConsignmentSORT_LevelAsc		= 2,
	eConsignmentSORT_LevelDesc		= 3,
	eConsignmentSORT_PriceAsc		= 4,
	eConsignmentSORT_PriceDesc		= 5,
};

enum {
	eConsignmentUPDATEKIND_CancelByUser	= 0,
	eConsignmentUPDATEKIND_CancelByTime	= 1,
	eConsignmentUPDATEKIND_Buy			= 2,
};

enum {
	eConsignmentDBResult_RegistedOver		= -4, // ����ʰ�
	eConsignmentDBResult_DurabilityErr		= -3, // ��������ġ
	eConsignmentDBResult_PoorMoney			= -2, // ������
	eConsignmentDBResult_Err				= -1, // ����
	eConsignmentDBResult_None				= 0,
	eConsignmentDBResult_SoldOut			= 1, // ����Ǹŵ�
	eConsignmentDBResult_UserCancel_Ack		= 2, // ������Ҽ���
	eConsignmentDBResult_UserCancel_Nack	= 3, // ������ҽ���
	eConsignmentDBResult_TimeCancel_Ack		= 4, // �Ⱓ��Ҽ���
	eConsignmentDBResult_TimeCancel_Nack	= 5, // �Ⱓ��ҽ���
	eConsignmentDBResult_Buy_Ack			= 6, // ���ż���
	eConsignmentDBResult_Buy_Nack			= 7, // ���Ž���
};

enum eConsignmentLog
{
	eConsignmentLog_Regist			= 0, // ���
	eConsignmentLog_Buy				= 1, // ����
	eConsignmentLog_SellUpdate		= 2, // ���� (�κ��ǸŽ� ������������ ��������)
	eConsignmentLog_SoldOut			= 3, // �ǸſϷ�
	eConsignmentLog_UserCancel		= 4, // �������
	eConsignmentLog_TimeCancel		= 5, // �Ⱓ���
	eConsignmentLog_GetPackage		= 6, // ����ȹ��
	eConsignmentLog_GetPackageFail	= 7, // ����ȹ�����
	eConsignmentLog_DelByDate		= 8, // �����Ⱓ����
};

enum BASICEFFECT
{
	eEffect_NewCharacter = 1,	//A��I�Ϩ���E����?AI ��IiiAa
	eEffect_Standard = 2,		//A��I�Ϩ���E����?AI �ˢ硧ua���ע����ˢ�i(��Io�ˢ�i�ˢ硧ua����u��I����AI)
	eEffect_Walk,	//�ˢ硧��E�ˢ硧ua (��Io�ˢ�i�ˢ硧ua ����u��I����AI)
	eEffect_Run,	//��EOU�ˢ硧ua (��Io�ˢ�i�ˢ硧ua����u��I����AI)
	eEffect_BattleWalk_Gum,		//AuAo �ˢ硧��E�ˢ硧ua(�ˢ硧��E_��Io�ˢ�i�ˢ硧uaAaA��I����)
	eEffect_BattleWalk_Gwun,		//AuAo �ˢ硧��E�ˢ硧ua(�ˢ硧uC��Io�ˢ�i�ˢ硧uaAaA��I����)
	eEffect_BattleWalk_Do,		//AuAo �ˢ硧��E�ˢ硧ua(��Ii��Ii_��Io�ˢ�i�ˢ硧uaAaA��I����)
	eEffect_BattleWalk_Chang,		//AuAo �ˢ硧��E�ˢ硧ua(A�ˢ�E_��Io�ˢ�i�ˢ硧uaAaA��I����)
	eEffect_BattleWalk_Gung,		//AuAo �ˢ硧��E�ˢ硧ua(�ˢ硧uA_��Io�ˢ�i�ˢ硧uaAaA��I����)
	eEffect_BattleWalk_Amgi,		//AuAo �ˢ硧��E�ˢ硧ua(����uI�ˢ硧ua_��Io�ˢ�i�ˢ硧uaAaA��I����)
	eEffect_BattleRun_Gum,		//AuAo ��EOU�ˢ硧ua(�ˢ硧��E_��Io�ˢ�i�ˢ硧uaAaA��I����)
	eEffect_BattleRun_Gwon,		//AuAo ��EOU�ˢ硧ua(�ˢ硧uC_��Io�ˢ�i�ˢ硧uaAaA��I����)
	eEffect_BattleRun_Do,		//AuAo ��EOU�ˢ硧ua(��Ii��Ii_��Io�ˢ�i�ˢ硧uaAaA��I����)
	eEffect_BattleRun_Chang,		//AuAo ��EOU�ˢ硧ua(A�ˢ�E��Io�ˢ�i�ˢ硧uaAaA��I����)
	eEffect_BattleRun_Gung,		//AuAo ��EOU�ˢ硧ua(�ˢ硧uA_��Io�ˢ�i�ˢ硧uaAaA��I����)
	eEffect_BattleRun_Amgi,		//AuAo ��EOU�ˢ硧ua(����uI�ˢ硧ua_��Io�ˢ�i�ˢ硧uaAaA��I����)
	eEffect_UngijosikStart,	//��E?i�ˢ硧uav����oA(����uE��E��IA��Ii��E?AU)
	eEffect_Ungijosik,	//��E?i�ˢ硧uav����oA
	eEffect_Ungijosik_End,	//��E?i�ˢ硧uav����oA (��I�Ϩ���E��c��I�Ϩ��ˢ硧IEA)
	eEffect_UngijosikDamage,	//��E?i�ˢ硧uav����oA A��I������E?��E��c ��Ii�ˢ�I��IoIAo
	eEffect_StallStart,	//��I�Ϩ�ea�ˢ�io ��Ii��E?AU1 (����uE�ˢ硧ua)
	eEffect_Stall,	//��I�Ϩ�ea�ˢ�io ��Ii��E?AU1 (����uE����u���סˢ� AO�ˢ硧ua)
	eEffect_StallEnd,	//��I�Ϩ�ea�ˢ�io ��Ii��E?AU1 (����u�ˢ硧I�ˢ硧ua)
	eEffect_NormalDamage = 13,		//��Ii�ˢ�I��IoIAo1 (AA)
	eEffect_CriticalDamage = 14,		//��Ii�ˢ�I��IoIAo2 (��E?i)
	eEffect_Die1 = 16,		//A�ˢ��?= 1
	eEffect_Died1 = 16,		//A�ˢ��?����ui����u�ˢ硧I ��E��I����I��E?o AO��E��IA ��E������I��I����oA 1
	eEffect_Die2 = 15,		//A�ˢ��?= 2
	eEffect_Died2 = 15,		//A�ˢ��?����ui����u�ˢ硧I ��E��I����I��E?o AO��E��IA ��E������I��I����oA 2
	eEffect_LevelUpSentence = 3,	//�ˢ�ˢ���Ii��o��Ei��u�ˢ碮������u�ˢ�A �ˢ硧uUAU
	eEffect_MouseMovePoint,	//��E������EO��E?i����o���ע��� AI��Ii��E?���סˢ�ˢ�AAI���סˢ碮Ec
	
	eEffect_GetMoney = 5,	// ��E�����ˢ�AA�ˢ��?��E?����I����u�ˢ硧I ��Ii�ˢ�ˢ� ����uo�ˢ硧ua
	eEffect_GetItem_Accessory = 25,	// ��E�����ˢ�AA�ˢ��?��E?����I����u�ˢ硧I ����u���סˢ�AIA���סˢ� ����uo�ˢ硧ua

	eEffect_Emoticon_Bow,	//AI�ˢ�ic
	eEffect_Emoticon_Thanks,	//�ˢ硧���ˢ碮���ˢ�ic
	eEffect_Emoticon_Yes,	//�ˢ硧uad 
	eEffect_Emoticon_No,	//���ע���Id
	eEffect_Emoticon_Pleasure,	//�ˢ硧ua�ˢ�iY
	eEffect_Emoticon_Sadness,	//����o����oCA
	eEffect_Emoticon_Love,	//�ˢ�ic��EOu
	eEffect_Emoticon_Hostility,	//Au��E��Ie
	eEffect_Emoticon_Disappoint,	//����oC��E����A
	eEffect_Emoticon_Angry,	//���ע������ע�E��I�Ϩ�e
	eEffect_Emoticon_Suprised,	//��I�Ϩ�i��EO�ˢ�A
	eEffect_Emoticon_Evasion,	//E��E����CC
	eEffect_Emoticon_Declare,	//����u�ˢ硧u����u��I��I
	eEffect_Defence_Gum,		//��Ii��o|i��������ui��Ii��E?AU(�ˢ硧��E)
	eEffect_Defence_Gwun,		//��Ii��o|i��������ui��Ii��E?AU(�ˢ硧uC)
	eEffect_Defence_Do,		//��Ii��o|i��������ui��Ii��E?AU(��Ii��Ii)
	eEffect_Defence_Chang,		//��Ii��o|i��������ui��Ii��E?AU(A�ˢ�E)
	eEffect_Defence_Gung,		//��Ii��o|i��������ui��Ii��E?AU(�ˢ硧uA)
	eEffect_Defence_Amgi,		//��Ii��o|i��������ui��Ii��E?AU(����uI�ˢ硧ua)

	eEffect_GetItem_Weapon = 7,		//��Ii��o|i��������ui��Ii��E?AU(����uI�ˢ硧ua)
	eEffect_GetItem_Dress = 8,
	eEffect_GetItem_Quest = 10,
	eEffect_GetItem_Youngyak = 6,

	eEffect_MonRegen = 80,				// ������ ���� ������ eEffect_NewCharacter ����Ʈ�� �����
	eEffect_ChangeStage_Hwa = 98,
	eEffect_ChangeStage_Geuk = 99,
	eEffect_PetSummon = 3151,			// ���ȯ
	eEffect_PetSeal		= 3152,			// �����
	eEffect_FeedUp		= 3153,			// �����
	eEffect_PetWarp		= 3154,			// �����

	eEffect_ShopItem_Life = 3200,		// ��â��	
	eEffect_ShopItem_NaeRuyk,			// ����	
	eEffect_ShopItem_Revive,			// ��Ȱ�ֹ���
	eEffect_ShopItem_Boom1,				// ����(����)
	eEffect_ShopItem_Boom2,				// ����(��)
	eEffect_ShopItem_Hair1,				// ����1	
	eEffect_ShopItem_Hair2,				// ����2	
	eEffect_ShopItem_Hair3,				// ����3	
	eEffect_ShopItem_Teleport1,			// �ڷ���Ʈ1
	eEffect_ShopItem_Teleport2,			// �ڷ���Ʈ2
	eEffect_ShopItem_Teleport3,			// �ڷ���Ʈ3
	eEffect_ShopItem_MaxLife,			// ������ ����
	eEffect_ShopItem_MaxNaeRuyk,		// ������ ����

	// 070528 LYW CommonGameDefine : Add effect of emoticon part.
	eEffect_Emoticon1 = 51, 
	eEffect_Emoticon2 = 52, 
	eEffect_Emoticon3 = 53, 
	eEffect_Emoticon4 = 54, 
	eEffect_Emoticon5 = 55, 
	eEffect_Emoticon6 = 56, 

	eEffect_GetItem_Unique	= 40,
	eEffect_GetItem_Rare	= 41,

	// 071206 LUJ ��� ���� ȿ��
	eEffect_Guild_Graduate	= 49,
};

enum MapName
{
	MapNameNone,
	Alker_Farm = 2,
	// 090921 LUJ, �׶� ���� �߰�
	Nera_Farm = 3,
	Moon_Blind_Swamp = 13,
	Red_Orc_Outpost,	
	Moon_Blind_Forest,	
	Haunted_Mine_1F = 16,
	Haunted_Mine_2F = 17,
	The_Gate_Of_Alker = 19,
	Alker_Harbor,	

	Ruins_Of_Draconian,	
	Zakandia,	
	Tarintus,	
	Mont_Blanc_Port = 25,	
	Dryed_Gazell_Fall,		
	Zakandia_Outpost,		
	The_Dark_Portal,		
	Distorted_crevice = 29,
	The_Way_To_The_Howling_Ravine = 31,
	Howling_Ravine = 32,
	Howling_Cave_1F = 33,
	Howling_Cave_2F = 34,
	Ghost_Tree_Swamp = 41, 
	Lair_of_Kierra = 42,
	The_Valley_Of_Fairy = 51,
	The_Town_of_Nera_Castle = 52,
	The_Great_Garden = 53,
	// 080714 LUJ, "������ ����" �� �߰�
	TheKnightsGrave	= 54,
	A_Harbor_of_Nera = 55,
	Temple_of_Greed = 56,
	Broken_Shipwrecked = 57,
	The_tower_of_wizardly_Low = 60,
	The_tower_of_wizardly_Middle = 61,
	The_tower_of_wizardly_High = 62,
	The_Death_tower = 63,
	The_Spirit_Forest = 64,
	The_Cristal_Empire = 65,
	TreeOfKenai = 67,
	TheCryingTavern = 68,
	TheUndergroundPassage = 69,
	SahelZone = 71,
	MirageInTheParasus = 72,
	RuinsOfAncientElpis = 73,
	DragoniansTomb1 = 74,
	DragoniansTomb2 = 75,
 	// 080905 LYW --- CommonGameDefine : ���� ���� �� �̸� �߰�.
	The_Castle_of_Rushen = 81,
	The_Culvert = 82,
	The_Castle_of_Zevyn = 83,
 	Advanced_Dungeon = 84,

	DateDungeon_Cemetery = 91,
	DateDungeon_BrokenHouse = 92,
	DateDungeon_DarkCave = 93,

	GuildTournamentStadium = 94,
	// 090609 LUJ, �ű� ���� �߰�
	DesolatedGorge = 96,

	DateDungeon_CookieGarden = 97,

	AlkerResidence = 99,
	
	// 090626 NYJ �ִ� �� ���� 200���� Ȯ��
	MAX_MAP_NUM = 200,
	//MAX_MAP_NUM,
};

enum EObjectState
{
	eObjectState_None,						// ����u���סˢ硧Io�ˢ�i�ˢ�ioAA ����u���סˢ碮E��IO..
	eObjectState_Enter,						// A��I�Ϩ���E����?AI��I�Ϩ����ע�I ��E����o����o���ע���AI�ˢ硧����E��c A��I�Ϩ�= ��IiiAaCO��EO�ˢ碮�� AC �ˢ�ioAA
	eObjectState_Move,						// AAC�ˢ�I AI��Ii��E? �ˢ�ioAA
	eObjectState_Rest,						// ��E����o����o���ע���AIAC E���ע��Ϣ���oA �ˢ�ioAA
	
//	eObjectState_Speech,					// Npc��E?I ��E��IeE�ˢ硧I �ˢ�ioAA
	eObjectState_Deal,						// �ˢ�ioa Open �ˢ�ioAA

	eObjectState_Exchange,					// �ˢ硧u��I�Ϩ�E? �ˢ�ioAA
	eObjectState_StreetStall_Owner,			// ��I�Ϩ�ea�ˢ�io Open �ˢ�ioAA
	eObjectState_StreetStall_Guest,			// ��I�Ϩ�ea�ˢ�io ����uO��E��IO �ˢ�ioAA
	eObjectState_PrivateWarehouse,			// �ˢ硧����I�Ϩ�AIA�ˢ�E�ˢ硧��i 10
	eObjectState_Munpa,						//��Io��Ec���סˢ�A ��E������Ec����i��o��Ei��u���סˢ碮Ec��I�Ϩ����ע�I ��Io��Ec���סˢ�AA�ˢ�E

	eObjectState_SkillStart,				// ����o���ע���A��I�Ϩ� ����oAAu �ˢ�ioAA
	eObjectState_SkillSyn,					//
	eObjectState_SkillBinding,				//
	eObjectState_SkillUsing,				//
	eObjectState_SkillDelay,				// A���ע��Ϣ��ע�����E�����ˢ硧����E��c ��I��A�ˢ�!����a; ��EO�ˢ碮��, ��Io�ˢ�i�ˢ硧����I��̢??��I��A�ˢ硧��i��I�Ϩ��ˢ硧IEAAC ��Iio�ˢ�ˢ���IoAI
	
	eObjectState_TiedUp_CanMove,			// �ˢ硧��a��IoU �ˢ�ioAA
	eObjectState_TiedUp_CanSkill,
	eObjectState_TiedUp,
	
	eObjectState_Die,						// A�ˢ��?: �ˢ�ioAA 20	
	
	eObjectState_BattleReady,
	eObjectState_Exit,						//KES 040707 A��I�Ϩ���E����?AI�ˢ硧����E��c ��I�Ϩ����ע�I�ˢ硧���ˢ�I��EO�ˢ碮��
	eObjectState_Immortal,					//����I��������AI, �ˡ�EAuE?, AuAa'A�ˢ� ��ii��u�����ϩ����̢���a��oA ��o��iAu��ioAA
	eObjectState_ImmortalDefense,			//

	eObjectState_Society,					//KES A����A��I���� �좯AU
	
	eObjectState_ItemUse,

	eObjectState_TournamentReady,
	eObjectState_TournamentProcess,
	eObjectState_TournamentEnd,
	eObjectState_TournamentDead,

	eObjectState_Engrave,

	// 070503 LYW --- CommonGameDefine : Add enum to setting rest damage.
	eObjectState_RestDamage,
	eObjectState_Connect,					// ���� 070823

	// 080428 NYJ --- ���û���
	eObjectState_Fishing,
	eObjectState_FishingResult,

	eObjectState_Cooking,
	eObjectState_Housing,
	eObjectState_HouseRiding,
	
	eObjectState_MAX,	
};

enum eObjectBattleState
{
	eObjectBattleState_Peace,
	eObjectBattleState_Battle,
	eObjectBattleState_Max,
};

#define FARM_RENDER_OBJ_ID_START	0x8fffffff
// E ����ý��� �߰� added by hseos 2007.05.07
#define STATICNPC_ID_START		0xa0000000
#define STATICNPC_ID_MAX		0x10000000
#define SKILLOBJECT_ID_START	0xc0000000
#define SKILLOBJECT_ID_MAX		0x0000ffff
#define BATTLE_ID_START			0xd0000000
#define BATTLE_ID_MAX			0x0000ffff
#define TEMP_SKILLOBJECT_ID		0xffffffff
#define BATTLE_TEAMEFFECT_ID	0xfffffffe
#define BATTLE_STAGEEFFECT_ID	0xfffffffd
#define STATE_EFFECT_ID			0xfffffffc
#define PK_EFFECT_ID			0xfffffffb
#define NPCMARK_1_ID			0xfffffffa		// !
#define NPCMARK_2_ID			0xfffffff9		// ?
#define BATTLE_GTTEAM1_EFFECTID	0xfffffff7
#define BATTLE_GTTEAM2_EFFECTID	0xfffffff6
#define ENGRAVE_EFFECTID		0xfffffff4
#define ENGRAVE_ACK_EFFECTID	0xfffffff3
#define ENGRAVE_NACK_EFFECTID	0xfffffff2
#define LIFE_RECOVER_EFFECTID	0xfffffff1
#define MANA_RECOVER_EFFECTID	0xfffffff0
#define BATTLE_GTTEAM3_EFFECTID	0xffffffef
#define ALL_RECOVER_EFFECTID	0xffffffee
#define DETECT_EFFECT_ID		0xffffffea

#define HELMET_EFFECT			0xffffffe9
#define SHIELD_EFFECT			0xffffffe8
#define HEART_EFFECT			0xffffffe7
#define ITEM_WORKING_SUCCESS_EFFECT	0xffffffe6
#define ITEM_WORKING_FAIL_EFFECT	0xffffffe5
#define ITEM_WORKING_EFFECT			0xffffffe4

#define FISHING_START_EFFECT		0xffffffe3
#define FISHING_SUCCESS_EFFECT		0xffffffe2
#define FISHING_FAIL_EFFECT			0xffffffe1

#define BATTLE_GTIMMORTAL_ID		0xffffffe0

// 080827 LYW --- CommonGameDefine : ���ͽõ� ��� ����Ʈ ��ȣ�� �����Ѵ�.
#define SIEGEWARFARE_USING_WATERSEED	0xffffffdf

#define ITEM_SUMMON_EFFECTACTIONID	0xffffffde
#define ITEM_SUMMON_EFFECTID		0xffffffdd

#define COOKING_EFFECT				0xffffffdc
#define HOUSE_DECOMODE_EFFECT		0xffffffdb
// 100406 ShinJS --- �ð����� ����Ʈ !, ? Effect ID
#define NPCMARK_3_ID			0xffffffda		// !
#define NPCMARK_4_ID			0xffffffd9		// ?
typedef BYTE DIRINDEX;
#define MAX_DIRECTION_INDEX		32
#define DEGTODIR(angleDeg)	(BYTE)((angleDeg)/360*MAX_DIRECTION_INDEX)
#define DIRTODEG(DirIndex)	(((float)DirIndex)*360/MAX_DIRECTION_INDEX)
#define DIRTORAD(DirIndex)	(DEGTORAD(DIRTODEG(DirIndex)))
#define RADTODIR(angleRad)	(DEGTODIR(RADTODEG(angleRad)))

// A�ˢ�i��E�������ע��Ϣ�EOo ����u��I�Ϩ�d
#define CAMERA_NEAR		100
#define MAX_CHARTARGETPOSBUF_SIZE	15

//system
#define MAX_IPADDRESS_SIZE	16

// type : 0->����oA�ˢ硧���ˢ�I��EO�ˢ碮�����ע����ˢ硧�� ��I�Ϩ���I��A�ˢ硧ue. 1~100->AId�ˢ�ˢ���Ec AI�ˢ�io���ע���?E�ˢ硧I����oA ��I�Ϩ���I��A�ˢ硧ue(�ˢ硧����E��c�ˢ�ie), 101~200->(�ˢ硧���ˢ碮���ˢ�ie)
// 1->�ˢ�ioa��E?��E��c ���סˢ�C��E����A, 2->��I�Ϩ�ea��E?��E��c����u�ˢ硧I ���סˢ�C��E����A, 3->��E����o����o���ע���AI��E?��E��c�ˢ硧��O E��Io��Ii��I�ˢ�, 4->�ˢ硧u��I�Ϩ�E?, 5->A�ˢ�E�ˢ硧��i��E?��E��c����u�ˢ硧I �ˢ�i�ˢ�ϡ���EO�ˢ碮��, 6->��Io��Ec���סˢ�AA�ˢ�E�ˢ硧��i��E?��E��c����u�ˢ硧I �ˢ�i�ˢ�ϡ���EO�ˢ碮��
// 101->�ˢ�ioa��E?��E��c����u�ˢ硧I �ˢ�ii��EO�ˢ碮��, 102->��I�Ϩ�ea��E?��E��c����u�ˢ硧I �ˢ�ii��EO�ˢ碮��, 103->A�ˢ��?����uu;��EO�ˢ碮��, 104->�ˢ硧u��I�Ϩ�E?, 105->A�ˢ�E�ˢ硧��i��E?��E��c ��I�Ϩ�O=, 106->��Io��Ec���סˢ�AA�ˢ�E�ˢ硧��i��E?��E��c ��I�Ϩ�O=, 107->C�ˢ�I�ˢ硧u��Io �ˢ�ii��EO�ˢ碮��
enum eMoneyLogtype
{
	eMoneyLog_Time				= 0,
// �ˢ硧����E��c�ˢ�ie
	eMoneyLog_GetStall			= 1,
	eMoneyLog_GetStreetStall	= 2,
	eMoneyLog_GetMonster		= 3,
	eMoneyLog_GetExchange		= 4,
	eMoneyLog_GetStorage			= 5,
	eMoneyLog_GetGuild			= 6,
	eMoneyLog_GetWantedPrize	= 7,
	eMoneyLog_GetPKLooting		= 8,
	eMoneyLog_GetSuryunFail		= 9,
	eMoneyLog_GetPrize			= 10,
	eMoneyLog_GetSWProfit		= 11,
	eMoneyLog_GetFromQuest		= 12,
	eMoneyLog_GetGMTool			= 13,
	eMoneyLog_GetFamily,
	// desc_hseos_���͹���01
	// S ���͹��� �߰� added by hseos 2007.07.30
	eMoneyLog_GetMonstermeter,
	// E ���͹��� �߰� added by hseos 2007.07.30

	
// �ˢ硧���ˢ碮���ˢ�ie
	eMoneyLog_LoseStall			= 101,
	eMoneyLog_LoseStreetStall	= 102,
	eMoneyLog_LoseDie			= 103,
	eMoneyLog_LoseExchange		= 104,
	eMoneyLog_LoseStorage		= 105,
	eMoneyLog_LoseGuild			= 106,
	eMoneyLog_LoseStorageBuy		= 107,
	eMoneyLog_LoseWantedRegist	= 108,
	eMoneyLog_LoseWantedBuyRight = 109,
	eMoneyLog_LosePKLooted		= 110,
	eMoneyLog_LosePKModeExitPanelty = 111,
	eMoneyLog_LoseBadFrameBail	= 112,
	eMoneyLog_LoseSuryunFee		= 113,
	eMoneyLog_LoseGuildLevelUp	= 114,
	eMoneyLog_LoseChangeLocation	= 115,
	eMoneyLog_LoseGMTool		= 116,
	eMoneyLog_LoseFamily,
	eMoneyLog_LoseFamilyLevelUp,
	// desc_hseos_����ý���_01
	// S ����ý��� �߰� added by hseos 2007.08.23	2007.09.10	2008.01.15
	eMoneyLog_LoseFarmBuy,
	eMoneyLog_LoseFarmUpgrade,
	eMoneyLog_LoseFarmTax,
	eMoneyLog_TollForVehicle,
	eMoneyLog_Enchant,
	eMoneyLog_TriggerAction,
	eMoneyLog_Skill,
	eMoneyLog_GetCheatMoney = 201,

	eMoneyLog_Consignment_Charge,
	eMoneyLog_MoneyFromNote,
    eMoneyLog_Transmog,
};

enum eLogSkill
{
	eLog_SkillLearn = 0,
	eLog_SkillDiscard,
	eLog_SkillDestroyByWanted,
	eLog_SkillDestroyByBadFame,
	eLog_SkillLevelup,
	eLog_SkillCheatLevelup,
	eLog_SkillLevelDownByWanted,
	eLog_SkillLevelDownByBadFame,
	eLog_SkillDestroyByGetJulCho,
	eLog_SkillDestroyByBadFameJulChoDown,
	eLog_SkillDestroyByGetNextLevel,		// for japan

	////////////////////////////////////////////////////////////////////////////////
	// 06. 06. 2�� ���� - �̿���
	// ���� ��ȯ
	eLog_SkillOption,
	eLog_SkillOptionClear,
	eLog_SkillDestroyByOptionClear,
	////////////////////////////////////////////////////////////////////////////////

	eLog_Skill_RM	= 100,
	eLog_SkillLearn_RM,
	eLog_SkillLevel_RM,
	eLog_SkillRemove_RM,

	eLog_SkillGetPoint,
	// 080819 LUJ, ĳ�� ��ų �α�
	eLog_SkillCashAdd,
	eLog_SkillCashRemove,
};


enum eFamilyLog
{
	// ����Ʈ
	eLog_FamilyPoint	= 0,
	eLog_FamilyPointAdd,
	// 080610 LUJ, ����Ʈ ����
	eLog_FamilyPointUpdate,

	// ����
	eLog_FamilyFarm		= 100,
	eLog_FamilyFarmPutCrop,		// ����
	eLog_FamilyFarmDieCrop,		// ����
	eLog_FamilyFarmGrowCrop,	// ����
	eLog_FamilyFarmCareCrop,	// ���
	eLog_FamilyFarmHarvestCrop,	// ��Ȯ
	eLog_FamilyFarmLostRight,	// �Ǹ� ���(�йи� Ż�� ������ ���� �Ǹ� ���)
	eLog_FamilyFarmDisband,		// ���� ��ü(�йи� �ػ����� ����)
	// desc_hseos_����ý���_01
	// S ����ý��� �߰� added by hseos 2008.01.16
	eLog_FamilyFarmTax,			// ���� ��ü(������ �̳�)
	eLog_FamilyFarmMystery,		// ���� ��ü(���� ��ü �� �α׸� �߰����� �ʾ��� ���)
	// E ����ý��� �߰� added by hseos 2008.01.16
	// 080425 KTH -- ��� ���� �α� �߰� --------------
	eLog_FamilyFarmInstallAnimal,
	eLog_FamilyFarmDieAnimal,
	eLog_FamilyFarmFeedAnimal,
	eLog_FamilyFarmCleanAnimal,
	eLog_FamilyFarmAnimalReward,
	// 081205 LUJ, �йи� �ൿ
	eLog_FamilyCreate = 200,
	eLog_FamilyAddMember,
	eLog_FamilyChangeNick,
	eLog_FamilyRegistEmblem,
	eLog_FamilyLeaveMember,
	eLog_FamilyExpelMember,
	eLog_FamilyBreakup,
	// 091111 ONS �йи� ���� ����
	eLog_FamilyDelEmelem,
	// 091126 ONS �йи��� �̾� ��� �߰�
	eLog_FamilyChangeMaster,
};

// desc_hseos_����ý���_01
// S ����ý��� �߰� added by hseos 2008.01.23
enum FARM_LOG_KIND																			// ���� �α� ����
{
	FARM_LOG_KIND_BUY = 0,																	// ..����
	FARM_LOG_KIND_UPGRADE,																	// ..���׷��̵�
	FARM_LOG_KIND_PAY_TAX,																	// ..������ ����
	FARM_LOG_KIND_DEL_FAMILY_BREAKUP,														// ..���� - �йи� ��ü
	FARM_LOG_KIND_DEL_TAX_ARREARAGE,														// ..���� - ������ ü��
	FARM_LOG_KIND_MYSTERY = 9999,
};

enum FARM_UPGRADE_LOG_KIND																	// ���� ���׷��̵� �α� ����
{
	FARM_UPGRADE_LOG_KIND_GARDEN = 0,														// ..�Թ�
	FARM_UPGRADE_LOG_KIND_FENCE,															// ..��Ÿ��
	FARM_UPGRADE_LOG_KIND_HOUSE,															// ..��
	FARM_UPGRADE_LOG_KIND_WAREHOUSE,														// ..â��
	FARM_UPGRADE_LOG_KIND_ANIMALCAGE,														// ..���
	FARM_UPGRADE_LOG_KIND_MYSTERY = 9999,
};

enum eLogitemmoney
{
	eLog_ItemNone		= 0,
	eLog_ItemDiscard	= 1,
	eLog_PetItemDiscard,
	
	//item move
	eLog_ItemMoveInvenToStorage = 100,
	eLog_ItemMoveStorageToInven,
	eLog_ItemMoveStorageToStorage,
	eLog_ItemMoveInvenToGuild,
	eLog_ItemMoveInvenToInven,
	eLog_ItemMoveGuildToInven,
	eLog_ItemMoveGuild,
	eLog_MoneyDepositToGuild,
	eLog_MoneyWithdrwaGuild,
	// 080716 LUJ, ��<->�÷��̾� �̵�
	eLog_ItemMoveFromPlayerToPet,
	eLog_ItemMoveFromPetToPlayer,
	
	//item obtain/destory
	eLog_ItemObtainBuy = 200,
	eLog_ItemObtainDivide,
	eLog_ItemObtainCheat,
	eLog_ItemObtainMonster,
	eLog_ItemObtainQuest,
	eLog_ItemObtainPK,
	eLog_ItemObtainDissolve,
	eLog_ItemObtainMix,
	eLog_ItemObtainFromChangeItem,
	eLog_ItemRareObtainCheat,
	eLog_ItemOptionObtainCheat,
	eLog_ItemObtainMonstermeter,
	eLog_ItemObtainMarriage,
	eLog_ItemObtainFishing,
	eLog_ItemObtainGift,
	eLog_ItemObtainMerge,
	eLog_ItemObtainGTReward,	// �����ʸ�Ʈ ��»�ǰ

	eLog_ItemObtainHouseRestore, // �Ͽ콺 �����ۺ�������
	eLog_ItemObtainHouseBonus,	// �Ͽ콺 ���ʽ�(������ȹ��)

	eLog_ItemSell = 300,
	eLog_ItemUse,
	
	eLog_StreetStallBuyAll = 400,
	eLog_StreetStallBuyDivide,
	eLog_StreetStallSellDivide,

	eLog_Exchange = 500,

	eLog_ItemReinforceSuccess = 600,
	eLog_ItemReinforceFail,
	eLog_ItemReinforceDataHaveTooManyMaterials,
	eLog_ItemReinforceSourceInvalid,
	eLog_ItemReinforceMaterialIsInvalid,
	eLog_ItemReinforceWhichMaterialHasZero,
	eLog_ItemReinfoceYouCannotOver100ToReinforce,
	eLog_ItemReinforceSourceItemNotExist,
	eLog_ItemRerinfoceServerHasNoReinforceScript,
	eLog_ItemReinforceMaterialSourceItemCannnotReinforce,
	eLog_ItemReinforceSourceItemCanDuplicate,
	eLog_ItemReinforceRemovingMaterialIsFailed,
	eLog_ItemReinforceUpdatingItemIsFailed,
	// 080929 LUJ, ��ȭ ���� ������ ���� �߰�
	eLog_ItemReinforceInvalidSupportItem,
	eLog_ItemReinforceNoSupportScript,
	eLog_ItemReinforceInvalidMaterialForSupportItem,

	eLog_ItemMixSuccess = 700,
	eLog_ItemMixFail,
	eLog_ItemMixBigFail,
	eLog_ItemMixInsufficentSpace,
	eLog_ItemMixServerError,
	eLog_ItemMixUse,
	
	eLog_ItemDissolveSuccess,
	eLog_ItemDissolvePlayerNotExist,
	eLog_ItemDissolveSourceItemNotExist,
	eLog_ItemDissolveServerHaveNoDissolvingData,
	eLog_ItemDissolveItemDataNotMatched,
	eLog_ItemDissolveItNeedMoreLevel,
	eLog_ItemDissolveRemovingItemIsFailed,
	eLog_ItemDissolveAddingItemIsFailed,
	eLog_ItemDissolvePlayserHasNoInventory,
	eLog_ItemDissolveInsufficentSpace,
	eLog_ItemDissolveNoResult,
	eLog_ItemDissolveServerError,
	eLog_ItemDissolveScriptInvalid,
	eLog_ItemDissolveInsert,		// �� ���Կ� �������� ����
	eLog_ItemDissolveAddDuplicated,	// ���ؽ� �������� �� ���Կ� �������� �������� �߰���

	eLog_ItemEnchantSuccess,
	eLog_ItemEnchantItemInvalid,
	eLog_ItemEnchantServerError,
	eLog_ItemEnchantHacking,
	eLog_ItemEnchantItemUpdateFailed,
	eLog_ItemEnchantUse,
	eLog_ItemEnchantDestroy,
	eLog_ItemEnchantNotEnoughMoney,	// 090713 ShinJS --- ��æƮ�� �������� ������ ���
	eLog_ItemEnchantLevelChanged,
	eLog_ItemEnchantProtected,

	eLog_ItemMixSystemFault,	// ���� �� �ý��� ����
	eLog_ItemMixNotFound,		// ������ ����� ã�� ����
	eLog_itemMixBadRequest,		// �߸��� ��û
	eLog_ItemMixBadCondition,	// ������ ������ ���� ����
	eLog_ItemMixBadQuantity,	// ������ �߸���
		
	eLog_ItemDestroyMix = 800,
	eLog_ItemDestroyReinforce,
	eLog_ItemDestroyUpgrade,
	eLog_ItemDestroyByWanted, // eLog_ItemDestroyByBadFame��| AIAEA����I ������A����Ao
	eLog_ItemDestroyByBadFame, //die panelty with bad fame.
	eLog_ItemDestroyDissolution, // dissolve base item
	eLog_ItemDestroyGiveQuest, // by complete quest 
	eLog_ItemDestroyDeleteQuest,
	eLog_ItemDestroybyChangeItem,
	eLog_ItemDestroyReinforceWithShopItem,

	// 090207 LUJ, �ռ� �α�
	eLog_ItemComposeSuccess = 900,
	eLog_ItemComposeSuccessWithConsume,
	eLog_ItemComposeFailureByMemoryMismatch,
	eLog_ItemComposeFailureByWrongScript,
	eLog_ItemComposeFailureByWrongUseItemSize,
	eLog_ItemComposeFailureByInvalidResult,
	eLog_ItemComposeFailureByInvalidKeyItem,
	eLog_ItemComposeFailureByNoExistedOption,
	eLog_ItemComposeFailureByNoResultItemInfo,

	eLog_MoneyObtainPK = 1000,
	//exp obtain
	eLog_ExpObtainPK,

	eLog_ShopItemUse = 1500,	
	eLog_ShopItemUseEnd,
	eLog_ShopItemMoveToInven,
	eLog_ShopItemProtectAll,
	eLog_ShopItemSeal,
	eLog_ShopItemUnseal,
	eLog_ShopItemUnseal_Failed,
	eLog_ShopItem_ReinforceReset,
	eLog_ShopItemGetCheat,
	// 090625 pdy �� �Ⱓ�� ������ ����ó�� �߰� 
	eLog_ShopItemUseEndFromPetInven,

	eLog_ShopItemUse_MixSuccess=1600,

	// 080602 LUJ, ��ų ��� �� �Ҹ�
	eLog_SkillItemUse	= 1700,
	eLog_SkillMoneyUse,
	
	eLog_Item = 2000,
	eLog_ItemOptionAdd_RM,
	eLog_ItemOptionRemove_RM,
	eLog_ItemAdd,
	eLog_ItemRemove,
	eLog_ItemRemoveMarriageDivorce,
	eLog_ItemDropOptionAdd,
		
	eLog_ItemMoveInvenToPetInven = 2100,
	eLog_ItemMovePetInvenToInven,

	eLog_ItemSkillLearn	= 2200,

	eLog_GTournamentReg = 2300,		// �����ʸ�Ʈ ��Ϻ�
	// 081205 LUJ, ����
	eLog_FarmTaxPaid = 2400,

	eLog_ItemCookUse,
	eLog_ItemCookSuccess,
	eLog_ItemVehicleToll,
	eLog_ItemHouseKeep,
	eLog_ItemHouseUnKepp,
	eLog_ItemUseHouseItem,

	// 090227 ShinJS --- Coin Item�� ���� ����/����
	eLog_CoinDecreaseByItemBuy,					// Coin �������(Update��)
	eLog_CoinDestroyByItemBuy,					// Coin �������(Delete��)
	// Ʈ���ŷ� ���� ������ ����/�߰�
	eLog_AddItemByTrigger,
	eLog_RemoveItemByTrigger,
	eLog_ItemChangeClass,


	// �ǸŴ���
	eLog_Consignment_Regist = 2412,			// �ǸŴ�����
	// ������
	eLog_Note_Consignmnet_Buy,			// �ǸŴ��౸��
	eLog_Note_Consignmnet_SellUpdate,	// �ǸŴ��఻�� (�κ��ǸŽ� ������������ ��������)
	eLog_Note_Consignmnet_Receipt,		// �ǸŴ��࿵����
	eLog_Note_Consignmnet_UserCancel,	// �ǸŴ������(����)
	eLog_Note_Consignmnet_TimeCancel,	// �ǸŴ������(�Ⱓ)
	eLog_Note_CombineDelete,			// �����ޱ����(������ ���������ۿ� �������� ������)
	eLog_Note_Obtain_Success,			// �����ޱ�����(������)
	eLog_Note_Obtain_Money,				// �����ޱ�����(���)
	eLog_Note_UserDelete,				// ������������
	eLog_Note_TimeDelete,				// �����Ⱓ����
	eLog_ItemVehicleSummon,
	eLog_ItemVehicleUnsummon,
	eLog_DungeonBegin,
	eLog_DungeonEnd,
	eLog_LimitDungeonBegin,
	eLog_LimitDungeonEnd,
	eLog_DateMatchBegin,
	eLog_DateMatchEnd,
	eLog_Note_Obtain_Fail,				// �����ޱ����
	eLog_Note_SendItemFromNPC,			// NPC�� ���������۹߼�

	eLog_ItemUse_GTBUFFITEM,		
	    eLog_ItemTransmogSuccess,
	eLog_ItemTransmogHacking,
	eLog_ItemTransmogItemInvalid,
	eLog_ItemTransmogNotEnoughMoney,
	eLog_ItemTransmogDummyRemoved,
	eLog_ItemTransmogMaterialRemoved,	
    eLog_ItemTransmogReversed,	
	eLog_Max,
};


enum eGuildLog
{
	// member
	eGuildLog_MemberSecede = 1,
	eGuildLog_MemberBan,
	eGuildLog_MemberAdd,
	eGuildLog_StudentAdd,

	// master 
	eGuildLog_MasterChangeRank = 100,

	// guild
	eLogGuild_GuildCreate = 200,
	eGuildLog_GuildBreakUp,
	eGuildLog_GuildLevelUp,
	eGuildLog_GuildLevelDown,
	eGuildLog_Retire,
	eGuildLog_SkillAdd,

	// guildunion
	eGuildLog_CreateUnion = 400,
	eGuildLog_DestroyUnion,
	eGuildLog_AddUnion,
	eGuildLog_RemoveUnion,	
	eGuildLog_SecedeUnion,

	// ����
	eGuildLog_Score	= 500,
	eGuildLog_TimeScoreAdd,
	eGuildLog_ScoreAddByKill,		// ����� ��� ����
	eGuildLog_ScoreRemoveByMurder,	// ��������� ���
	eGuildLog_ScoreAddByWin,		// ����� �¸�
	eGuildLog_ScoreRemoveByDefeat,	// ����� �й�
	eGuildLog_ScoreCheat,			// ġƮ
	eGuildLog_ScoreLevelUp,			// �������� ���
	eGuildLog_ScoreSkillUse,		// 080602 LUJ, ��ų �ߵ��� �Ҹ�
	eGuildLog_ScoreRemoveByGTFee,	// �����ʸ�Ʈ ��ϼҸ�
	eGuildLog_ScoreAddByGTWin,		// �����ʸ�Ʈ �¸�����
	eGuildLog_ScoreAddByItem,		// �������Ʈȹ�� ������

	// gm ��
	eGuildLog_GM	= 600,
	eGuildLog_GM_ChangeRank,
	eGuildLog_GM_KIck_Member,
	// 081027 LUJ, ��� ��ʸ�Ʈ �α�
	eGuildLog_TournamentAdd,
};


enum eQuestLog
{
	eQuestLog_None	= 0,

	eQuestLog_GM	= 1,

	eQuestLog_User	= 100,
	eQuestLog_Main_Begin,
	eQuestLog_Sub_Begin,
	eQuestLog_Main_End,
	eQuestLog_Sub_End,
	eQuestLog_Main_Delete,
	eQuestLog_Sub_Delete,
	eQuestLOg_ItemTake,
	eQuestLOg_ItemGive,
	eQuestLOg_ItemUpdate,
};


enum eLogExppoint
{
	eExpLog_Time,
		
	eExpLog_LevelUp = 10,
	// 081027 LUJ, ����ġ ȹ��
	eExpLog_Get,

	eExpLog_LosebyBadFame = 50, //reduce 
	eExpLog_LosebyReviveLogIn,
	eExpLog_LosebyRevivePresent,
	eExpLog_LosebyLooting,
	eExpLog_LosebyBreakupGuild,
	eExpLog_LosebyReviveExpProtect,
	eExpLog_LosebyReviveReviveOther,
	eExpLog_LosebyGuildFieldWar,
	eExpLog_LosebyBreakupFamily,
	// desc_hseos_�йи�01
	// S �йи� �߰� added by hseos 2007.10.11
	eExpLog_LosebyLeaveFamily,
	eExpLog_LosebyExpleFamilyMember,
	// E �йи� �߰� added by hseos 2007.10.11
	eExpLog_LoseByPKMode,
	// 080602 LUJ, ��ų ������� ���� ����ġ �Ҹ�
	eExpLog_LoseBySkillUse,

	eExpLog_GetbyQuest = 100,
	eExpLog_SExpGetbyQuest = 101,
	eExpLog_GetbyItem = 102,
	
	eExpLog_ProtectExp = 150,
	eExpLog_ReviveExp,
};

enum eLogToolType	// Tool A����u
{
	eToolLog_GM = 1,
	eToolLog_RM,
	eToolLog_Server,
};

enum eGMLogKind		// GMTool ��I���� A����u
{
	eGMLog_LogIn = 1,
	eGMLog_Move,	
	eGMLog_Item,
	eGMLog_Money,
	eGMLog_Summon,
	eGMLog_SummonDelete,
	eGMLog_MonsterKill,
	eGMLog_PKAllow,
	eGMLog_Disconnect_Map,
	eGMLog_Disconnect_User,
	eGMLog_Block,
	eGMLog_Event,
	eGMLog_PlusTime,

	eGMLog_Skill,
	eGMLog_Ability,
	eGMLog_Chat_Map,
	eGMLog_Chat_User,
	eGMLog_PK,
	eGMLog_Regen,
};

enum eFishingLog
{
	eFishingLog_SetPointFromCheat = 1,	// ġƮ�κ��� ����������Ʈ ����
	eFishingLog_GetPointFromFish,		// ������κ��� ����������Ʈ ȹ��
	eFishingLog_UsePointForBuyItem,		// �������� ������� ����������Ʈ ���
	eFishingLog_SetLevel,				// 080808 LUJ, ���� ���� ����
	eFishingLog_Regular,				// 080808 LUJ, ���� �α� ����
	eFishingLog_GetExpFromItem,
	eFishingLog_GetPointFromItem,
};

enum eCookingLog
{
	eCookingLog_CookCount = 1,			// �丮�ϱ�
	eCookingLog_EatCount,				// �丮�Ա�
	eCookingLog_FireCount,				// ��ں� �ǿ��

	eCookingLog_RecipeReg,				// ������ ���
	eCookingLog_RecipeDel_by_Time,		// ������ ���� (�ð����� �ڵ�����)
	eCookingLog_RecipeDel_by_User,		// ������ ���� (���� ��������)
};

enum eHousingLog
{
	eHousingLog_Keep = 1,			// ����
	eHougingLog_UnKeep,				// ��������
	eHougingLog_Install,			// ��ġ
	eHougingLog_UnInstall,			// ��ġ����
	eHougingLog_Destroy,			// �ı�
	eHougingLog_EndTime,			// ���ð�����

	eHougingLog_WallPaper,			// �������
	eHougingLog_FloorPaper,			// �ٴ������
	eHougingLog_CeilingPaper,		// õ�������
	eHougingLog_Extend,				// Ȯ��
	eHougingLog_Visit,				// �湮
	eHougingLog_Exterior,			// �������
};

// 081028 LUJ, ���� �α�. ���� 
enum eSiegeLog
{
	eSiegeLogNone,
	eSiegeLogChangeState,
	eSiegeLogDestoryCatapult,
	eSiegeLogGetCastle,
	eSiegeLogUseSeed,
	eSiegeLogSetSeedSucceed,
	eSiegeLogSetSeedFailed,
	eSiegeLogComplete,
};


// 080716 LUJ, �� �α�
enum ePetLog
{
	ePetLogSummon,
	ePetLogDie,
	ePetLogRevive,
	ePetLogChangeGrade,
	ePetLogChangeLevel,
	ePetLogRegular,		// 080716 LUJ, ���� �������� ���� ����
};


// �����ʸ�Ʈ �α�
enum eGTLogKind
{
	eGTLogKind_None=0,
	eGTLogKind_Regist,				// ���
	eGTLogKind_Win,					// ��
	eGTLogKind_Lose,				// ��
};


// ������ �α�
enum eSWLogKind
{
	eSWLogKind_None=0,
	eSWLogKind_DefenceProposal,			// �������� ��û
	eSWLogKind_Attack,					// ��������
	eSWLogKind_Defence,					// ������ ����
	eSWLogKind_DefenceAccept,			// �������� ������� �������� ����
	eSWLogKind_DefenceUnion,			// ���ͰἺ���� �������� ����
	eSWLogKind_CancelByCastleGuild,		// �����İ� ����Ż��� �ٸ��������� �������� ���
	eSWLogKind_CancelByUnionSecession,	// �����Ŀ��� ���� Ż��� �������� ���
	eSWLogKind_StartSiegeWar,			// ��������
	eSWLogKind_SucceedCastleGuild,		// ������������ ���� ������ ����
	eSWLogKind_SucceedMoveToDefence,	// ������������ ���� ������ ������ ������ �������� �̵�
	eSWLogKind_SucceedAttack,			// �������� ������ ���� �״���� ����
	eSWLogKind_SucceedMoveToAttack,		// �������� ������������ ����->�������� �Űܰ� ����
	eSWLogKind_EndCastleGuild,			// ���������� ���� ������ ����
	eSWLogKind_EndDefenceUnionGuild,	// ���������� �����̾ �������� ��ϵǴ� ����
	eSWLogKind_EngraveSyn,				// ���νõ�
	eSWLogKind_EngraveNack,				// ���ν��� �¾� ����
	eSWLogKind_EngraveNackCancel,		// ���ν��� ���
	eSWLogKind_EngraveNackLogOut,		// ���ν��� �α׾ƿ�
	eSWLogKind_SiegeWarSucceed,			// ���μ���
	eSWLogKind_EndSiegeWar,				// ��������
};

// 090609 ShinJS --- AutoNote Log �߰�
enum eAutoNoteLogKind
{
	eAutoNoteLogKind_None,
	eAutoNoteLogKind_Report,			// �Ű�
	eAutoNoteLogKind_Regist,			// Auto ���
	eAutoNoteLogKind_Ban,				// Ban ���
};

#define MAX_KIND_PERMAP		150

enum
{
	ePreLoad_Monster,
	ePreLoad_Item,

	ePreLoad_Max,
};


struct sPRELOAD_INFO
{
	int		MapNum;
	int		Count[ePreLoad_Max];
	int		Kind[ePreLoad_Max][MAX_KIND_PERMAP];

	sPRELOAD_INFO()
	{
		MapNum		= 0;
		memset(Count, 0, sizeof(int)*ePreLoad_Max);
		memset(Kind, 0, sizeof(int)*ePreLoad_Max*MAX_KIND_PERMAP);
	}
};

#define MAX_GUILD_NAME 16
#define MAX_GUILD_INTRO		60
// 06. 03. ���İ��� - �̿���
#define MAX_GUILD_NOTICE	150
#define MAX_GUILD_UNION		7
#define MAX_GUILD_NICKNAME	16
#define GUILD_MASTER		60
#define GUILD_VICEMASTER	50
#define GUILD_SENIOR		40
#define GUILD_JUNIOR		30
#define GUILD_MEMBER		20
#define GUILD_STUDENT		10
#define GUILD_NOTMEMBER		0

#define GUILD_STUDENT_MAX_LEVEL	40
#define MAX_GUILD_MEMBER	80 //60 + 20(���û�)
#define MAX_SHOUT_LENGTH 60

enum
{
	eMode_NormalNote = 0,
	eMode_StoredNote,
};

#define MAXGUILD_INTOURNAMENT			32		//�����ʸ�Ʈ �ִ�������
#define MAX_GTOURNAMENT_PLAYER			14		//�����ʸ�Ʈ ��������
#define GT_INVALID_BATTLEID				99999	//�����ʸ�Ʈ ��ƲID�� �������

enum
{
	eGTError_NOERROR=0,					// No Error
	eGTError_NOTREGISTDAY,				// Not a Regist-day
	eGTError_NOTREGISTCANCELDAY,		// Not a Registcancel-day
	eGTError_DONTFINDGUILDINFO,			// Don't find a GuildInfo
	eGTError_MAXGUILDOVER,				// Over MAXGUILD_INTOURNAMENT
	eGTError_ALREADYREGISTE,			// Already Registed GuildTournament
	eGTError_ALREADYCANCELREGISTE,		// Already Cancel Registed GuildTournament
	eGTError_UNDERLIMITEMEMBER,			// Under LIMITMEMBER_GUILDTOURNAMENT
	eGTError_UNDERLEVEL,				// Under LIMITLEVEL_GUILDTOURNAMENT
	eGTError_DELETEAPPLIED,				// CancelError Regited GuildTournament
	eGTError_NOGUILDMASTER,				// Not a GuildMaster
	eGTError_DONTFINDPOSITION,			// GuildTournament-Position Find Error
	eGTError_DONTFINDBATTLE,			// Don't find a Battle	
	eGTError_BATTLEJOINFAIL,			// Failed a join in Battle
	eGTError_NOTENTERTIME,				// Not a time of enter the battle;
	eGTError_STUDENTCANTENTER,			// a guild student can't enter the battle
	eGTError_FAILTOREGIST,				// Fail to Regist
	eGTError_INVALIDROUND,
	eGTError_ALREADYSETMATCH,
	eGTError_NOTALLOWOBSERVER,
	eGTError_INVALIDDAY,
	eGTError_READYTONEXTMATCH,
	eGTError_NOTENOUGHFEE,
	eGTError_ERROR,
};

enum
{
	eGTState_BeforeRegist=0,
	eGTState_Regist,
	eGTState_BeforeEntrance,
	eGTState_Entrance,
	eGTState_Process,
	eGTState_Leave,

	eGTState_AutoSchedule,
	
	eGTState_Max,
};

enum
{
	eGTStanding_1ST = 1,
	eGTStanding_2ND = 2,
	eGTStanding_3RD = 3,
	eGTStanding_4TH = 4,
	eGTStanding_8TH = 8,
	eGTStanding_16TH = 16,
	eGTStanding_32TH = 32,
};

enum
{
	eGTFight_32		= 0,
	eGTFight_16		= 1,
	eGTFight_8		= 2,
	eGTFight_4		= 3,
	eGTFight_2		= 4,
	eGTFight_1		= 5,
	eGTFight_End,

	eGTT_MAX,
};

enum
{
	eGTNotify_RegistStart = 0,
	eGTNotify_RegistEnd,
	eGTNotify_TournamentStart,
	eGTNotify_TournamentEnd,
	eGTNotify_RoundStartforPlayer,
	eGTNotify_RoundStart,
	eGTNotify_RoundEnd,
	
	eGTNotify_EnteranceRemain,
	eGTNotify_RegistRemain,
	eGTNotify_BattleTableOpen,

	eGTNotify_WinLose,

	eGTNotify_Max,
};

enum
{
	eGTDay_SUNDAY=0,
	eGTDay_MONDAY,
	eGTDay_TUESDAY,
	eGTDay_WEDNESDAY,
	eGTDay_THURSDAY,
	eGTDay_FRIDAY,
	eGTDay_SATURDAY,
};

enum
{
	eGTWin_None = 0,
	eGTWin_Unearned,		// ������
	eGTWin_RemainPlayer,	// ���������
	eGTWin_LowLevel,		// �������� ����
	eGTWin_LowExp,			// ����ġ�� ����.
};

enum
{
	eNpcParam_ShowpStorage=1,
};

#define ITEM_PARAM_PRESENT	0x00000100

enum eITEM_PARAM
{
	eITEM_PARAM_NORMAL,
	eITEM_PARAM_SEAL,
	eITEM_PARAM_UNSEAL,
};

// 070414 LYW --- CommonGameDefine : Modified eCharacterStage.
enum eCharacterStage
{
	eStage_H_Fighter			= 1111,	// ������
	eStage_H_Guard				= 1121,	// ����
	eStage_H_Warrior			= 1122,	// ����
	eStage_H_InfantryMan		= 1131,	// ����
	eStage_H_SwordMan			= 1132,	// �ҵ��
	eStage_H_Mercenary			= 1133,	// �뺴
	eStage_H_Phalanx			= 1141, // �ȶ�ũ��
	eStage_H_Knight				= 1142, // ���
	eStage_H_Gladiator			= 1143, // ������
	eStage_H_RunicKnight		= 1144, // �� ����Ʈ
	eStage_H_Paladin			= 1151, // �ӷ���
	eStage_H_Panzer				= 1152, // �Ⱙ�δ��
	eStage_H_Crusader			= 1153, // ���ڱ�����
	eStage_H_Destroyer			= 1154, // �ı���
	eStage_H_SwordMaster		= 1155, // �ҵ帶����
	eStage_H_Magners			= 1156, // �ű׳ʽ�
	eStage_H_Lord				= 1161, // �ε�
	eStage_H_DeathKNight		= 1162, // ��������Ʈ
	eStage_H_ArcTemplar			= 1163, // ��ũ���÷�

	eStage_E_Fighter			= 1211,	// ������
	eStage_E_Guard				= 1221,	// ����
	eStage_E_Warrior			= 1222,	// ����
	eStage_E_InfantryMan		= 1231,	// ����
	eStage_E_SwordMan			= 1232,	// �ҵ��
	eStage_E_Mercenary			= 1233,	// �뺴
	eStage_E_Phalanx			= 1241, // �ȶ�ũ��
	eStage_E_Knight				= 1242, // ���
	eStage_E_Gladiator			= 1243, // ������
	eStage_E_RunicKnight		= 1244, // �� ����Ʈ
	eStage_E_Paladin			= 1251, // �ӷ���
	eStage_E_Panzer				= 1252, // �Ⱙ�δ��
	eStage_E_Crusader			= 1253, // ���ڱ�����
	eStage_E_Destroyer			= 1254, // �ı���
	eStage_E_SwordMaster		= 1255, // �ҵ帶����
	eStage_E_Magners			= 1256, // �ű׳ʽ�
	eStage_E_Lord				= 1261, // �ε�
	eStage_E_DeathKNight		= 1262, // ��������Ʈ
	eStage_E_ArcTemplar			= 1263, // ��ũ���÷�
	

	eStage_H_Rogue				= 2111, // �����
	eStage_H_Voyager			= 2121, // ������
	eStage_H_Ruffian			= 2122, // ������
	eStage_H_Archer				= 2131, // �ü�
	eStage_H_Chief				= 2132, // ����
	eStage_H_Scout				= 2133, // ������
	eStage_H_Ranger				= 2141, // �δ��
	eStage_H_TreasureHunter 	= 2142, // ������ɲ�
	eStage_H_Assassin			= 2143, // �ϻ���
	eStage_H_RunicWorker		= 2144, // ���Ŀ
	eStage_H_Sniper				= 2151, // ���ݺ�
	eStage_H_Intraper			= 2152, // 
	eStage_H_BladeTaker			= 2153, //
	eStage_H_TemperMaster		= 2154, //

	// 080709 LYW --- CommonGameDefine : ��ũ������ ������ �߰��Ѵ�.
	eState_H_ArcRanger			= 2155,	// ��ũ������

	eStage_H_SoulEye			= 2161, //
	eStage_H_BloodStalker		= 2162, //
	eStage_H_ArcBridger			= 2163, //

	eStage_E_Rogue				= 2211, // �����
	eStage_E_Voyager			= 2221, // ������
	eStage_E_Ruffian			= 2222, // ������
	eStage_E_Archer				= 2231, // �ü�
	eStage_E_Chief				= 2232, // ����
	eStage_E_Scout				= 2233, // ������
	eStage_E_Ranger				= 2241, // �δ��
	eStage_E_TreasureHunter 	= 2242, // ������ɲ�
	eStage_E_Assassin			= 2243, // �ϻ���
	eStage_E_RunicWorker		= 2244, // ���Ŀ
	eStage_E_Sniper				= 2251, // ���ݺ�		
	eStage_E_Intraper			= 2252, // 
	eStage_E_BladeTaker			= 2253, //
	eStage_E_TemperMaster		= 2254, //

	// 080709 LYW --- CommonGameDefine : ��ũ������ ������ �߰��Ѵ�.
	eState_E_ArcRanger			= 2255,	// ��ũ������

	eStage_E_SoulEye			= 2261, //
	eStage_E_BloodStalker		= 2262, //
	eStage_E_ArcBridger			= 2263, //

	eStage_H_Mage				= 3111, //
	eStage_H_Cleric				= 3121, //
	eStage_H_Wiard				= 3122, //
	eStage_H_Priest				= 3131, //
	eStage_H_Sorcerer			= 3132, //
	eStage_H_Monk				= 3133, //
	eStage_H_Bishop				= 3141, //
	eStage_H_Warlock			= 3142, //
	eStage_H_Inquirer			= 3143, //
	eStage_H_ElementalMaster	= 3144, //
	eStage_H_Cardinal			= 3151, //
	eStage_H_SoulArbiter		= 3152, //
	eStage_H_GrandMaster		= 3153, //
	eStage_H_Necromancer		= 3154, //
	eStage_H_RunicMaster		= 3155, //
	eStage_H_Saint				= 3161, //
	eStage_H_DarkArchon			= 3162, //
	eStage_H_ArcCelebrant		= 3163, //

	eStage_E_Mage				= 3211, //
	eStage_E_Cleric				= 3221, //
	eStage_E_Wiard				= 3222, //
	eStage_E_Priest				= 3231, //
	eStage_E_Sorcerer			= 3232, //
	eStage_E_Monk				= 3233, //
	eStage_E_Bishop				= 3241, //
	eStage_E_Warlock			= 3242, //
	eStage_E_Inquirer			= 3243, //
	eStage_E_ElementalMaster	= 3244, //
	eStage_E_Cardinal			= 3251, //
	eStage_E_SoulArbiter		= 3252, //
	eStage_E_GrandMaster		= 3253, //
	eStage_E_Necromancer		= 3254, //
	eStage_E_RunicMaster		= 3255, //
	eStage_E_Saint				= 3261, //
	eStage_E_DarkArchon			= 3262, //
	eStage_E_ArcCelebrant		= 3263, //

};

enum eGuildUnion_Err
{
	eGU_Not_ValidName,
	eGU_Not_InGuild,
	eGU_Not_GuildMaster,
	eGU_Not_Level,
	eGU_Not_GuildUnionMaster,
	eGU_Not_InGuildUnion,
	eGU_Aleady_InGuildUnion,
	eGU_Time,
	eGU_Other_Not_InGuild,
	eGU_Other_Not_InGuildUnion,
	eGU_No_Lone_Union,
	eGU_Other_Not_GuildMaster,
	eGU_Other_Aleady_InGuildUnion,
	eGU_Other_Time,
	eGU_Is_Full,	
	eGU_Is_GuildFieldWar,
	eGU_Not_ValidMap,
};

// ������
enum eWeatherState
{
	//eWS_None,	// 0
	//eWS_Snow,	// 1
	eWS_Clear,	// ����
	eWS_Cloudy,	// �帲
	eWS_Rainy,	// ��
	eWS_Windy,	// �ٶ�
	eWS_Foggy,	// �Ȱ�
	eWS_Typoon,	// ��ǳ

	eWS_Max,	
};

// 080410 NYJ --- ���ð��� �����ڵ� 
enum eFishingErrorCode
{
	eFishingError_None = 0,		// NotError, Success
	eFishingError_InvaildHero,	// ��ȿ���� ���� �÷��̾�
	eFishingError_InvaildPlace,	// ��ȿ���� ���� ������
	eFishingError_NoPole,		// ���ô� ������
	eFishingError_NoBait,		// �̳� �̻��
	eFishingError_OverTime,		// �ð��ʰ�
	eFishingError_UserCancel,	// ������ ��ҿ�û
	eFishingError_InvenFull,	// �κ��� ����������
	eFishingError_InvalidGrade,	// ���õ�� ����ġ
	eFishingError_EarlyTime,	// �ð�������
	eFishingError_InvaildState,	// ��ȿ���� ���� ����
	eFishingError_OverMaxFishPoint,	// �ִ� ����������Ʈ �ʰ�
	eFishingError_FailFishPoint,// ����������Ʈ��ȯ ����
	eFishingError_OverDistance,	// �Ÿ��ʰ�
};

// 080414 NYJ --- ���ð��� �����������Ÿ��
enum eFishItemType
{
	eFishItem_Poor = 0,		// [0]�Ƕ��
	eFishItem_Common,		// [1]�ؾ�
	eFishItem_UnCommon,		// [2]�׾�
	eFishItem_Rare,			// [3]Ȳ���׾�, Ȳ���ڶ�
	eFishItem_Unique,		// [4]������ ������
	eFishItem_Event,		// [5]ģȯ��̳�
	eFishItem_Reserve1,		// [6]����1
	eFishItem_Reserve2,		// [7]����1
	eFishItem_Reserve3,		// [8]����1
	eFishItem_Reserve4,		// [9]����1
	
	eFishItem_Max,
};

enum eCookErrorCode
{
	eCookError_None = 0,		// NotError, Success
	eCookError_InvaildRecipe,	// ��ȿ���� ���� ������
	eCookError_InvaildFire,		// ��ȿ���� ���� ��ں�
	eCookError_InvaildMakeNum,	// ��ȿ���� ���� ���ۼ���
	eCookError_InvaildUtil,		// ��ȿ���� ���� �丮����
	eCookError_LowerUtil,		// �����Ǻ��� ����� ���� �丮����
	eCookError_LowerExpertPoint,// �����Ǻ��� ������ ���� ���õ�
	eCookError_InvalidGrade,	// ���õ�� ����ġ
	eCookError_InvaildState,	// ��ȿ���� ���� ����
	eCookError_OverDistance,	// �Ÿ��ʰ�
	eCookError_InvenFull,		// �κ��� ����������
	eCookError_NeedIngredient,	// ������
};

enum PART_TYPE
{
	ePartType_None,
	ePartType_OneHand,
	ePartType_TwoHand,
	ePartType_TwoBlade,
	ePartType_Shield,

	ePartType_Body,
	ePartType_Hand = 7,
	ePartType_Foot,

	ePartType_Hair,
	ePartType_Face,
	ePartType_Head,

	ePartType_Helmet = 15,
	ePartType_Glasses,
	ePartType_Mask,

	ePartType_Head_Chicken = 20,
	ePartType_Head_Panda,
	ePartType_Head_Tights,
	ePartType_Head_Bag,
	ePartType_Head_Mask,
	//090506 pdy ���� �߰� 
	ePartType_Head_CatEar,		

	ePartType_Costume_Body = 30,
	ePartType_Costume_Foot,
	ePartType_Costume_Hand,

	ePartType_Wing = 40,
	ePartType_Wing2,

	//100406 pdy �����߰� : Bip01 Pelvis�� Attach
	ePartType_Pelvis,

	//090506 pdy �����߰� : Bip01�� Attach
	ePartType_Tail	= 50, 


	ePartType_Pet_Face = 70,
	ePartType_Pet_Head,
	ePartType_Pet_Wing,
	ePartType_Pet_Tail,
	
	ePartType_Job_Effect = 100,

	ePartType_Max,
};

enum	DATE_MATCHING_REGION				// ����Ʈ ��Ī �ý��ۿ��� ��� ���� ����. ���ö���¡ �Ϸ��� ������������ ������ ������ ��.?
{
	eDATE_MATCHING_REGION_KYUNGKI,			// ���,
	eDATE_MATCHING_REGION_SEOUL,			// ����
	eDATE_MATCHING_REGION_INCHON,			// ��õ
	eDATE_MATCHING_REGION_KANGWON,			// ����
	eDATE_MATCHING_REGION_CHUNGNAM,			// �泲
	eDATE_MATCHING_REGION_CHUNGBUK,			// ���
	eDATE_MATCHING_REGION_DAEJEON,			// ����
	eDATE_MATCHING_REGION_KYUNGBUK,			// ���
	eDATE_MATCHING_REGION_KYUNGNAM,			// �泲
	eDATE_MATCHING_REGION_DAEGU,			// �뱸
	eDATE_MATCHING_REGION_BUSAN,			// �λ�
	eDATE_MATCHING_REGION_ULSAN,			// ���
	eDATE_MATCHING_REGION_JEONBUK,			// ����
	eDATE_MATCHING_REGION_JEONNAM,			// ����
	eDATE_MATCHING_REGION_KWANGJU,			// ����
	eDATE_MATCHING_REGION_JEJU,				// ����
	eDATE_MATCHING_REGION_OVERSEAS,			// �ؿ�
};

enum	DATE_MATCHING_FEELING				// ����Ʈ ��Ī �ý����� ����.
{
	eDATE_MATCHING_FEELING_BLACKHAIR,		// ���� �Ӹ�
	eDATE_MATCHING_FEELING_DYEDHAIR,			// ������ �Ӹ�
	eDATE_MATCHING_FEELING_FORMALDRESS,		// ����
	eDATE_MATCHING_FEELING_CASUALWEAR,		// ĳ�־� ����
	eDATE_MATCHING_FEELING_MAKEUP,			// ȭ��
	eDATE_MATCHING_FEELING_PERFUME,			// ���
	eDATE_MATCHING_FEELING_THIN,				// ����
	eDATE_MATCHING_FEELING_FAT,				// �׶���
	eDATE_MATCHING_FEELING_BEARD,			// ����
	eDATE_MATCHING_FEELING_EYEGLASS,			// �Ȱ�
	eDATE_MATCHING_FEELING_HAT,				// ����
	eDATE_MATCHING_FEELING_ACCESSORIES,		// �׼�����
};

enum eStatusKind
{
	eStatusKind_None,
	eStatusKind_Str,
	eStatusKind_Dex,
	eStatusKind_Vit,
	eStatusKind_Int,
	eStatusKind_Wis,
	eStatusKind_All,
	eStatusKind_PhysicAttack = 11,
	eStatusKind_PhysicDefense,
	eStatusKind_MagicAttack,
	eStatusKind_MagicDefense,
	eStatusKind_Accuracy,
	eStatusKind_Avoid,
	eStatusKind_CriticalRate,
	eStatusKind_Range,
	eStatusKind_MagicCriticalRate,
	eStatusKind_CriticalDamage = 22,
	eStatusKind_MoveSpeed,
	eStatusKind_Block,
	eStatusKind_CoolTime,
	eStatusKind_CastingProtect,

	eStatusKind_Hide,
	eStatusKind_Detect,

	eStatusKind_IncreaseDropRate = 29,
	eStatusKind_DecreaseManaRate = 30,
	eStatusKind_MaxLife = 31,
	eStatusKind_MaxMana,
	eStatusKind_LifeRecoverRate,
	eStatusKind_ManaRecoverRate,
	eStatusKind_LifeRecover,
	eStatusKind_ManaRecover,
	eStatusKind_Poison = 40,
	eStatusKind_Paralysis,
	eStatusKind_Stun,
	eStatusKind_Slip,
	eStatusKind_Freezing,
	eStatusKind_Stone,
	eStatusKind_Silence,
	eStatusKind_BlockAttack,
	eStatusKind_God,
	eStatusKind_Shield,
	eStatusKind_Bleeding,
	eStatusKind_Burning,
	eStatusKind_HolyDamage,
	eStatusKind_MoveStop,
	eStatusKind_Attract,
	// 090204 LUJ, �ǰ� �� MP�� ���ҽ�Ų��
	eStatusKind_ManaBurn,

	eStatusKind_Reflect = 61,
	eStatusKind_Absorb,
	eStatusKind_DamageToLife,
	eStatusKind_DamageToMana,
	eStatusKind_GetLife,
	eStatusKind_GetMana,
	eStatusKind_GetExp,
	eStatusKind_GetGold,
	// 071217 KTH --- ����ġ ��ȣ
	eStatusKind_ProtectExp = 70,
	eStatusKind_Attrib_None = 71,
	eStatusKind_Attrib_Earth,
	eStatusKind_Attrib_Water,
	eStatusKind_Attrib_Divine,
	eStatusKind_Attrib_Wind,
	eStatusKind_Attrib_Fire,
	eStatusKind_Attrib_Dark,
	eStatusKind_Sword = 81,
	eStatusKind_Mace,
	eStatusKind_Axe,
	eStatusKind_Staff,
	eStatusKind_Bow,
	eStatusKind_Gun,
	eStatusKind_Dagger,
	eStatusKind_Spear,
	eStatusKind_TwoHanded,
	eStatusKind_TwoBlade,
	eStatusKind_OneHanded,

	eStatusKind_RobeArmor,
	eStatusKind_LightArmor,
	eStatusKind_HeavyArmor,
	eStatusKind_ShieldArmor,
	eStatusKind_BothDagger,
	eStatusKind_FishingPole,
	eStatusKind_CookUtil,
	
	eStatusKind_NormalSpeedRate = 100,
	eStatusKind_PhysicSkillSpeedRate,
	eStatusKind_MagicSkillSpeedRate,	
	eStatusKind_NormalPhysicSkillSpeedRate = 105,
	// 080616 LUJ, 5�� ��ų �߰�
	eStatusKind_ConvertHealthWithGivedDamage	= 106,
	eStatusKind_ConvertManaWithGivedDamage		= 107,
	eStatusKind_EventSkillToVictim				= 108,
	eStatusKind_EventSkillToKiller				= 109,
	// 080703 LUJ, �̵��� ��� ����
	eStatusKind_EnableTwoBlade					= 110,
	// 081203 LUJ, ������ ��� ���� ����
	eStatusKind_UnableUseItem					= 111,
	// 081203 LUJ, ���� ��� ���� ����
	eStatusKind_UnableBuff						= 112,
	// 071204 KTH -- ����ġ ���� %
	eStatusKind_IncreaseExpRate = 120,
	// 100221 ShinJS --- �������� ���� �������� ��ȯ��Ų��
	eStatusKind_DamageToManaDamage = 121,

	// Push ����߰�
	eStatusKind_Push_Target,
	eStatusKind_Push_Operator,
	eStatusKind_PushPull_Target,
	eStatusKind_PushPull_Operator,

	eStatusKind_Max,
};

enum eSkillTargetKind
{
	eSkillTargetKind_None,
	eSkillTargetKind_OnlyOthers,
	eSkillTargetKind_Ground,
	eSkillTargetKind_OnlySelf,
	eSkillTargetKind_AllObject,
};

// 080602 LUJ, ��ų Ÿ�� ���. Target.h���� �Űܿ�
enum TARGET_KIND
{
	TARGET_KIND_SINGLE,
	TARGET_KIND_MULTI,
	TARGET_KIND_SINGLE_CIRCLE,
	TARGET_KIND_MULTI_CIRCLE,
	TARGET_KIND_MULTI_RECT,
	TARGET_KIND_MULTI_ARC,
	// 080602 LUJ, ���� ���� ��ų
	TARGET_KIND_WORLD,
	// 080708 LUJ, ���� ��� ���� ��ų
	TARGET_KIND_MULTI_MONSTER,
	// 090204 LUJ, ��ü(����/�÷��̾�) ��� ���� ��ų
	TARGET_KIND_MULTI_ALL,
	TARGET_KIND_MAX,
};

// 080602 LUJ, ��ų ���� ���� Ÿ��. Target.h���� �Űܿ�
enum eSkillAreaTarget
{
	eSkillAreaTargetNone,
	eSkillAreaTargetMonster,
	eSkillAreaTargetParty,
	eSkillAreaTargetFamily,
	eSkillAreaTargetGuild,
	eSkillAreaTargetAll,
	eSkillAreaTargetAllExceptMe,
};

// 080602 LUJ, ��ų ���. Ŭ���̾�Ʈ�� ���� ������ ��Ÿ���� ���� [cc]skill/server/manager/skillManager.h���� �Űܿ�
enum SKILL_RESULT
{
	SKILL_SUCCESS = 0,
	SKILL_OPERATOR_INVALUED,
	SKILL_INFO_INVALUED,
	SKILL_LEVEL_INVALUED,
	SKILL_STATE_INVALUED,
	SKILL_TARGET_INVALUED,
	SKILL_TARGET_IMMORTALED,
	SKILL_RANGE_NOTENOUGH,
	SKILL_COOLING,
	SKILL_FAIL_ETC,
	SKILL_DESTROY,

	SKILL_CDHACK,
	SKILL_AREAHACK,
	SKILL_CHEAT_REPLACE,
	SKILL_RANGEHACK,
};

enum eBuffSkillActiveRule
{
	eBuffSkillActiveRule_Active,
	eBuffSkillActiveRule_Delete,
	eBuffSkillActiveRule_Stop,
};

// 090204 LUJ, �Ӽ� ����
enum eBuffSkillCountType
{
	eBuffSkillCountType_None,
	eBuffSkillCountType_Dead,
	// 090204 LUJ, ��� ������ ����
	eBuffSkillCountType_AnyDamageVolumn = 10,
	eBuffSkillCountType_AnyDamage,
	eBuffSkillCountType_AnyHitVolumn,
	eBuffSkillCountType_AnyHit,
	// 090204 LUJ, ���� ����
	eBuffSkillCountType_PhysicalDamageVolumn = 20,
	eBuffSkillCountType_PhysicalDamage,
	eBuffSkillCountType_PhysicalHitVolumn,
	eBuffSkillCountType_PhysicalHit,
	// 090204 LUJ, ���� ����
	eBuffSkillCountType_MagicalDamageVolumn = 30,
	eBuffSkillCountType_MagicalDamage,
	eBuffSkillCountType_MagicalHitVolumn,
	eBuffSkillCountType_MagicalHit,
	// 090204 LUJ, ��
	eBuffSkillCountType_HealVolumn = 40,
	eBuffSkillCountType_Heal,
	eBuffSkillCountType_BeHealedVolumn,
	eBuffSkillCountType_BeHealed,
};

enum eAutoNoteError
{
	eAutoNoteError_None,
	eAutoNoteError_CantFind,
	eAutoNoteError_NotProperState,
	eAutoNoteError_AlreadyAsked,
	eAutoNoteError_AlreadyAsking,
	eAutoNoteError_CantUseMap,
};

enum ePetAI
{
	ePetAI_Aggressive,
	ePetAI_Support,
	ePetAI_Stand,
};

enum ePetType
{
	ePetType_Basic,
	ePetType_Physic,
	ePetType_Magic,
	ePetType_Support,
	ePetType_Max,
};

enum ePetState
{
	ePetState_None,
	ePetState_Summon,
	ePetState_Die,
};

// 090701 ShinJS --- ���� ���� ����
enum eLIMITDUNGEON_TYPE
{
	eLIMITDUNGEON_TYPE_LOW,			// �ʱ� ����
	eLIMITDUNGEON_TYPE_MIDDLE,		// �߱� ����
	eLIMITDUNGEON_TYPE_HIGH,		// ���� ����
};


// 090909 ShinJS --- ��ã�� �߰��� ���� �ʿ����� Common Header �� �̵�
#define MOUSE_MOVE_TICK		400
#define MOUSE_PRESS_TICK	400
#define MOUSE_ON_TICK		200
#define FOLLOW_TICK			500		//---KES ���󰡱�
 

//  �ű����� ���� ������ ����ĳ������ ������ġ
//#define MAKE_DEVIL_LIMIT_LEVEL	50

// 100322 ONS ����Ʈ�˸��� �ڵ���� �������� ����
#define MAX_QUICKVIEW_QUEST 10
#define MAX_QUEST_COUNT 20

#define MAX_CLASS_LEVEL		6
enum ENUM_CLASS
{
	eClass_FIGHTER = 0,
	eClass_ROGUE,
	eClass_MAGE, 
	eClass_DEVIL,
	eClass_MAXCLASS
} ;

enum eNoteParsingKind
{
	eNoteParsing_None = 0,
	eNoteParsing_FishingLevelUp,
	eNoteParsing_CookLevelUp,
};

enum eNoteError
{
	eNoteErr_None = 0,
	eNoteErr_WrongState,
	eNoteErr_InsertFail,
};

#define DEFAULT_MONEY_TEXT	"0"
#define DEFAULT_VOLUME_TEXT	"1"

enum eBUFF_START_RESULT
{
	eBUFF_START_RESULT_SUCCESS,
	eBUFF_START_RESULT_BATTLE_RULE_DELETE,
	eBUFF_START_RESULT_PEACE_RULE_DELETE,
	eBUFF_START_RESULT_MOVE_RULE_DELETE,
	eBUFF_START_RESULT_STOP_RULE_DELETE,
	eBUFF_START_RESULT_REST_RULE_DELETE,
};

#define MIN_MAGIC_CASTING_TIME	700.f

#endif //__COMMONGAMEDEFINE_H__
