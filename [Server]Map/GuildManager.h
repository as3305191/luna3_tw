#pragma once


#define GUILDMGR CGuildManager::GetInstance()
//#define OnErr(pPlayer, Protocol, errstate) \
//if(errstate) { SendNackMsg(pPlayer, Protocol, errstate); return;}

class CGuild;
class CGuildMark;
class CItemSlot;
class CGuildScore;

class CGuildManager  
{
	CYHHashTable<CGuild> m_GuildRoomHashTable;
	CYHHashTable<CGuildMark> m_GuildMarkTable;
	
	//MONEYTYPE m_LevelUpInfo[MAX_GUILD_STEP];
	CGuildManager();
	BOOL IsVaildGuildName( const char* );

	BYTE DoBreakUp(DWORD GuildIdx);
	void RegistMarkSyn(DWORD GuildIdx, char* ImgData, DWORD PlayerIdx);
	void RequestMark(CPlayer* pPlayer, MARKNAMETYPE MarkName);
	int convertCharToInt(char c);
	BYTE HexToByte(char* pStr);
	//void LoadLevelUpInfo();
	//MONEYTYPE GetLevelUpMoney(BYTE GuildLevel);
	BOOL CanEntryGuild(char* date);
public:
	//GETINSTANCE(CGuildManager);
	static CGuildManager* GetInstance();
	virtual ~CGuildManager();

	void NetworkMsgParse( BYTE Protocol, void* pMsg );

	CGuild* RegistGuild( const GUILDINFO& pGuildInfo, MONEYTYPE GuildMoney );
	//void RegistGuildItem(DWORD GuildIdx, ITEMBASE* Guilditem);
	void LoadMembers(GUILDMEMBERINFO_ID* pInfo);
	void LoadMark(MARKNAMETYPE MarkName, char* pImgData);

	void UserLogIn(CPlayer* pPlayer);
	void UserLogOut(CPlayer* pPlayer);

	void CreateGuildSyn( CPlayer*,  const char* GuildName, const char* Intro);
	void CreateGuildResult(LEVELTYPE MasterLvl, const GUILDINFO& pGuildInfo);

	void BreakUpGuildSyn(CPlayer* pMaster);
	void BreakUpGuildResult(CPlayer* pMaster, DWORD GuildIdx, char* EntryDate);

	//void DeleteMemberSyn(CPlayer* pMaster, DWORD MemberIDX);
	BYTE DoDeleteMember(DWORD GuildIDX, DWORD MemberIDX, BYTE bType, char* EntryDay);
	void DeleteMemberResult(DWORD GuildIDX, DWORD MemberIDX, char* EntryDay);

	void AddMemberSyn(CPlayer* pMaster, DWORD TargetIdx);
	void AddMemberResult(CPlayer* pTarget, DWORD MasterIdx);

	CGuild* GetGuild(DWORD GuildIdx);
	CGuild* GetGuildFromMasterIdx( DWORD dwMasterIdx );
	CGuildMark* GetMark(MARKNAMETYPE MarkName);

	void SendNackMsg(CPlayer* pPlayer, BYTE Protocol, BYTE errstate);

	void SecedeSyn(CPlayer* pPlayer);
	void SecedeResult(DWORD GuildIdx, DWORD MemberIDX, char* EntryDay);

	void SendChatMsg(DWORD GuildIdx, DWORD PlayerIdx, char* pMsg);

	// 091111 ONS ��幮�� ���� ó�� �߰�
	void DeleteMarkResult(DWORD dwPlayerIdx, DWORD dwMarkIdx, DWORD dwGuildIdx);
	void RegistMarkResult(DWORD GuildIdx, MARKNAMETYPE MarkName, char * pImgData);
	void DoRegistMark(CGuild* pGuild, MARKNAMETYPE MarkName, char* imgData);

	void DeleteMark(MARKNAMETYPE MarkName);
	void MarkChange(CPlayer* pPlayer, DWORD GuildIdx, MARKNAMETYPE MarkName);

	//void LevelUpSyn(CPlayer* pPlayer);

	void SendGuildEntryDate(CPlayer* pPlayer, char* day);

	void ChangeMemberRank(CPlayer* pMaster, DWORD MemberIdx, BYTE Rank);

	void GiveNickNameSyn(CPlayer* pMaster, DWORD TargetId, char* NickName);

	CItemSlot* GetSlot(DWORD GuildIdx);
	//void GuildWarehouseInfo(CPlayer* pPlayer, BYTE TabNum);
	//void LeaveWareHouse(CPlayer* pPlayer, BYTE TabNum);

	void MemberLevelUp(DWORD GuildIdx, DWORD PlayerIdx, LEVELTYPE PlayerLvl);
	void SendGuildName(CPlayer*, DWORD dwGuildIdx, const char* GuildName);
	void SetLogInfo(CGuild*, DWORD PlayerIdx, BOOL vals);
	void MsgGuildNoteSyn(LPVOID);
	void AddStudentResult(CPlayer*, DWORD MasterID);
	void AddHuntedMonster(CPlayer* pPlayer, CObject* pHuntedMonster);
	void Process();
	void GuildHuntedMonsterCountDBUpdate();
	void CheckDateForGuildHuntedMonsterCountConvertToGuildPoint();
	DWORD GetMasterIndex(LPCTSTR);

	void AddPlayer( CPlayer* );
	void RemovePlayer( const CPlayer* );

	CGuildScore* GetGuildScore( DWORD guildIndex );

private:
	// ��� ������ ����
	typedef stdext::hash_map< DWORD, CGuildScore* >	GuildMap;
	GuildMap										mGuildScoreMap;

	// �ð� ������ ó���� ��� ��ȣ�� ����
	typedef std::list< DWORD >						TimeList;
	TimeList										mTimeList;


	// 070801 ����, ��� ������ �о���δ�.
private:
	// 081031 LUJ, ��ȯ ��ũ��Ʈ �Ľ� �߰�
	void LoadScript();
	void ParseScoreScript( const std::list< std::string >& );
	void ParseLevelScript( const std::list< std::string >& );
	// 080417 LUJ, ��� ��ų ��ũ��Ʈ �ε�
	void ParseSkillScript( const std::list< std::string >& );

public:
	struct ScoreSetting
	{
		DWORD	mUnitTime;		// ���� ���� �ð�
		float	mUnitTimeScore;	// ���� ���� �ð� �� ��� ����
		float	mHuntScore;		// ���� ��ɽø��� ��� ����
		DWORD	mUpdateGap;		// DB�� �����ϰ� �������� ��ε�ĳ�����ϴ� �ֱ�. �и��� ����. �ʹ� ������ ���Ϲ߻�...
		float	mKillerScore;	// �ٸ� ������ ���ؾ��� �� ��� ����
		float	mCorpseScore;	// �ٸ� �������� ���صǾ��� �� �Ҵ� ����

		// ����� �¸��� ��� ����
		// ����: mPercent�� �й� ��尡 �Ҵ� ������ ���տ��� �¸� ��尡 ȹ���ϴ� ������ ���Ѵ�.
		struct Unit
		{
			float mValue;
			float mPercent;
		}
		mWinScore;;

		float	mWinMoneyRate;	// �����ݿ��� ȹ���ϴ� ����

		// ����� �й�� �Ҵ� ����
		typedef std::map< DWORD, Unit >		LostScoreMap;
		LostScoreMap						mLostScoreMap;
	};

private:
	ScoreSetting mScoreSetting;


public:
	const ScoreSetting& GetScoreSetting() const;

	// 071012 ����, ���� ������ �� ���� ������ ��ȯ�Ѵ�.
	float GetLosingScore( float score ) const;


public:
	struct LevelSetting
	{
		DWORD	mRequiredMasterLevel;
		DWORD	mRequiredScore;
		DWORD	mRequiredMoney;
		DWORD	mRequiredQuest;
		DWORD	mMemberSize;
		DWORD	mUnionSize;
		DWORD	mApprenticeSize;
		BOOL	mGuildMark;
		DWORD	mWarehouseSize;
		// 080417 LUJ, ȹ�� ������ ��� ��ų ����
		DWORD	mSkillSize;
	};

	const LevelSetting* GetLevelSetting( DWORD level ) const;


private:
	// Ű: ��� ����
	typedef stdext::hash_map< DWORD, LevelSetting >	LevelSettingMap;
	LevelSettingMap									mLevelSettingMap;


	// 080417 LUJ, ��� ��ų ������ �д´�
public:
	struct SkillSetting
	{
		// 080417 LUJ, ��� ������ �ִ� ��ų ����
		BYTE				mLevel;

		// 080417 LUJ, ��밡���� ����
		std::set< DWORD >	mRank;
	};

	// 080417 LUJ, ��ų ���� ������ ��� ��, Ű: ��ų �ε���
	typedef stdext::hash_map< DWORD, SkillSetting > SkillSettingMap;

	// 080417 LUJ, ���� �� ��ų ���� ������ ��� ��. Ű: ��� ����
	typedef stdext::hash_map< DWORD, SkillSettingMap >	SkillSettingLevelMap;

	// 080417 LUJ, ������ ��� ��ų ���� ��ȯ
	const SkillSettingMap* GetSkillSetting( DWORD level ) const;

	// 080417 LUJ, �ִ� ��� ������ ��ų ������ ��´�
	const SkillSettingMap* GetMaxLevelSkillSetting() const;

private:
	SkillSettingLevelMap mSkillSettingLevelMap;
};
