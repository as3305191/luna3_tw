#pragma once

class CDungeonMgr;

/*
	�� : �δ� �ʺ��� ����ó���� Ŭ������ �����ؼ� ����ؾ� �մϴ�.
	����ó���� Ŭ������ CDungeonProcessorBase�� ��ӹ޾Ƽ� ����Ͻø� �˴ϴ�.

	DungeonMgr::Init()���� ����ó���ڸ� �����ϸ�,
	DungeonMgr::Process()�� ��� ������ ���� ���μ����� ó���մϴ�.
				�������� �ٸ� ó���� ����ó������ Process()���� ó���Ͽ��� �մϴ�.
				
				��� ������ �������� ó���� DungeonMgr���� ó���ϸ�,
				�������� �ٸ��κ��� �������̽��� �߰��ؼ� ������ ó���Ͽ��� �մϴ�.
*/

class CDungeonProcessorBase
{
public:
	CDungeonProcessorBase(void);
	virtual ~CDungeonProcessorBase(void);

	virtual void	Init(CDungeonMgr* pParent);
	virtual void	Create(stDungeon* pDungeon) = 0;
	virtual void	Process() = 0;

	virtual void	SetSwitch(DWORD dwChannelID, WORD num, BOOL val) = 0;	// ����ġ�� ���°� ����Ǿ��� �� ó��
	virtual void	Info_Syn(CObject* pObject) = 0;							// Ŭ���̾�Ʈ���� MP_DUNGEON_INFO_SYN �� ���� �� ó��
	virtual void	SetBossMonster(stDungeon* pDungeon) = 0;

private:
	CDungeonMgr*		m_pParent;
};

class CDungeonProcessorEmpty : public CDungeonProcessorBase
{
public:
	CDungeonProcessorEmpty(void) {}
	virtual ~CDungeonProcessorEmpty(void) {}

	static CDungeonProcessorBase* Clone()
	{
		return new CDungeonProcessorEmpty;
	}

	virtual void	Create(stDungeon* pDungeon) {}
	virtual void	Process() {}
	virtual void	SetSwitch(DWORD dwChannelID, WORD num, BOOL val) {}
	virtual void	Info_Syn(CObject*) {}
	virtual void	SetBossMonster(stDungeon* pDungeon) {}
};