#ifndef __SERVERGAMEDEFINE_H__
#define __SERVERGAMEDEFINE_H__



#define MAX_TOTAL_PLAYER_NUM		50
#define MAX_TOTAL_PET_NUM			20
#define MAX_TOTAL_MONSTER_NUM		200
#define MAX_TOTAL_BOSSMONSTER_NUM	1
#define MAX_TOTAL_NPC_NUM			50
#define MAX_MAPOBJECT_NUM			20



#define GRID_BIT 5

extern DWORD gCurTime;
extern DWORD gTickTime;
extern float gEventRate[];
extern float gEventRateFile[];

#define _SERVER_RESOURCE_FIELD_			// ¼­¹ö¸¸?» ±¸ºÐÇÏ´Â ¿µ¿ª

#define START_LOGIN			0
#define CHANGE_LOGIN		1

enum SERVER_KIND
{
	ERROR_SERVER,
	DISTRIBUTE_SERVER,
	AGENT_SERVER,
	MAP_SERVER,
	CHAT_SERVER,
	MURIM_SERVER,
	MONITOR_AGENT_SERVER,
	MONITOR_SERVER,
	MAX_SERVER_KIND,
};

enum CHEAT_LOG
{
	eCHT_Item,
	eCHT_Money,
	eCHT_Hide,
	eCHT_AddSkill,
	eCHT_SkillLevel,
	eCHT_LevelUp,
	eCHT_Str,
	eCHT_Dex,
	eCHT_Vit,
	eCHT_Wis,
	eCHT_Int,
};

enum ebossstate
{	
	//////////////////////////////////////////////////////////////////////////
	//NORMAL STATE
	eBossState_Stand,
	eBossState_WalkAround,
	eBossState_Pursuit,
	eBossState_RunAway,
	eBossState_Attack,

	//////////////////////////////////////////////////////////////////////////
	//EVENT STATE
	eBossState_Recover,
	eBossState_Summon,
			
	eBossState_Max,
};

enum ebossaction
{
	eBOSSACTION_RECOVER = 1,
	eBOSSACTION_SUMMON,
};

enum ebosscondition
{
	eBOSSCONDITION_LIFE = 1,	
};

//---KES PUNISH  //����: �Ʒ��� �� enum�� ��ȣ���� DB�� ������ �ȴ�. ������ �ٲ��� ���ÿ�.
enum ePunishKind			
{
	ePunish_Login,
	ePunish_Chat,
	ePunish_Trade,
	ePunish_AutoNote,		//---�����Ʈ �������
	ePunish_Max,
};

enum ePunishCountKind
{
	ePunishCount_NoManner,
	ePunishCount_TradeCheat,		//---��ȯ ������ ���
	ePunishCount_AutoUse,		//---������
	ePunishCount_Max,
};

// 100104 LUJ, ���� AI �̺�Ʈ
enum eStateEvent
{
	eSEVENT_DUMMY = -1,
	eSEVENT_NULL,
	eSEVENT_Process,
	eSEVENT_Message,
	eSEVENT_Enter,
	eSEVENT_Leave,
};

// 100104 LUJ, ���Ͱ� ������ ��û�� �� �����ϴ� �޽��� ����
enum enumMESSAGEKINDS
{
	eMK_HelpRequest,
	eMK_HelpShout,
};

#endif //__SERVERGAMEDEFINE_H__
