#ifndef __CLIENTGAMEDEFINE_H__
#define __CLIENTGAMEDEFINE_H__

//------------------------------------------------------------------------------------
//
//	ClientGameDefine.h: Ŭ���̾�Ʈ ���ӽý��ۿ����� ����ϴ� define, typedef, enum
//						global variable extern �� �����Ѵ�.
//	��� ��Ģ:
//	1. comment�� �Ѵ�
//	2. �� ���Ͽ� ����ϱ⿡ �������� �ٽ� �ѹ� �����Ѵ�
//
//------------------------------------------------------------------------------------


enum eGameInInitKind
{
	eGameInInitKind_Login,
	eGameInInitKind_MapChange,
	eGameInInitKind_MovePos,
	eGameInInitKind_GTEnter,
	eGameInInitKind_GTLeave,
	eGameInInitKind_GTObserverEnter,
	eGameInInitKind_DungeonObserverEnter,
};

typedef WORD	SNDIDX;

#define HEROID gHeroID
extern DWORD gHeroID;
extern DWORD gUserID;

extern DWORD gCurTime;
extern DWORD gTickTime;
extern float gTickPerFrame;
extern float gAntiGravity;
extern int gChannelNum;
extern int gCheatMove;

#define MAX_PLAYEROBJECT_NUM	300
#define MAX_MONSTEROBJECT_NUM	200
#define MAX_NPCOBJECT_NUM		50
#define MAX_PETOBJECT_NUM		100
#define MAX_MAPOBJECT_NUM		20
// desc_hseos_����ý���_01
// S ����ý��� �߰� added by hseos 2007.05.07
#define MAX_FARM_RENDER_OBJ_NUM	500
// E ����ý��� �߰� added by hseos 2007.05.07
// 090422 ShinJS --- Ż�� �߰�
#define MAX_VEHICLEOBJECT_NUM	100
#define WEAPON_ATTACH_BONENAME_R	"Bone_R"
#define WEAPON_ATTACH_BONENAME_L	"Bone_L"
#define WEAPON_ATTACH_BONENAME_BACK	"Bone_Back"

#define LEFT_HAND			"Lhand"
#define RIGHT_HAND			"Rhand"
#define LEFT_HAND_BONE		"Lhand_bone"
#define RIGHT_HAND_BONE		"Rhand_bone"
#define HEAD_BONE			"head_bone"
#define FACE_BONE			"face_bone"
#define MAX_QUEST_PAGEBTN		6

// 100611 ������ ��ư Ȱ��ȭ �ּ� ��ų & ���� ����Ʈ
#define MINIMUM_LEVELUP_POINT 1
#define MINIMUM_SKILL_POINT 20

enum
{
	eDamageKind_Front = 1,
	eDamageKind_Left,
	eDamageKind_Right,
	
	eDamageKind_Counter,
	eDamageKind_ContinueDamage,
};

enum
{
	eRangeDamageKind_SameTime = 1,
	eRangeDamageKind_FrontToBack,
	eRangeDamageKind_LeftToRight,
	eRangeDamageKind_RightToLeft,
	eRangeDamageKind_CenterToEdge,
};

// LBS ������ ���� 03.09.30
#define STREETSTALL_ENTER_DISTANCE		200

#define _CLIENT_RESOURCE_FIELD_			// ���ҽ��� ������ ����� ����

//#define SIGNAL_COMMON_TEXT	"����"		//KES : msg�� �ϴ°��� �ϰ���?
//#define SIGNAL_ONEUSER_TEXT "�˸�"
//#define SIGNAL_SYSTEM_TEXT	"�ý���"

//DirectInputVersion
#define DIRECTINPUT_VERSION		0x0800

enum GTournament_NameType
{
	eGTNameType_OurGuild = 0,
	eGTNameType_Enemy,
	eGTNameType_Max,
};

struct DIALOGUE
{
	WORD wType;
	DWORD dwColor;
	TCHAR str[MAX_PATH];
	WORD wLine;
};

#endif //__CLIENTGAMEDEFINE_H__
