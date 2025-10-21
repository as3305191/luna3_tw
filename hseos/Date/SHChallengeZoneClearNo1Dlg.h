#ifndef _SHCHALLENGEZONECLEARNO1DLG
#define _SHCHALLENGEZONECLEARNO1DLG

#include "./Interface/cDialog.h"

class cStatic;
class CImageNumber;

class CSHChallengeZoneClearNo1Dlg : public cDialog
{
private:
	CImageNumber*	m_csChallengeZoneLimitTime1;
	CImageNumber*	m_csChallengeZoneLimitTime2;
	DWORD			m_nChallengeZoneStartTime;
	DWORD			m_nChallengeZoneStartTimeTick;

	cStatic*		m_csSection;
	cStatic*		m_csTime;

	BOOL			m_bOnlyRenderLimitTime;			// 090317 ShinJS --- ���ѽð��� Render�Ҽ� �ֵ��� üũ

public:
	CSHChallengeZoneClearNo1Dlg();
	virtual ~CSHChallengeZoneClearNo1Dlg();
	void		 	Linking();
	virtual void 	Render();

	void			SetChallengeZoneStartTime(DWORD nTime) { m_nChallengeZoneStartTime = nTime; }
	void			SetChallengeZoneStartTimeTick(DWORD nTick) { m_nChallengeZoneStartTimeTick = nTick; }
	void			SetChallengeZoneLeastClearSection( LPCTSTR );
	void			SetOnlyRenderLimitTime( BOOL bRender ) { m_bOnlyRenderLimitTime = bRender; }
	BOOL			GetOnlyRenderLimitTime() const { return m_bOnlyRenderLimitTime; }

// 091124 ONS ����ġ���� ǥ��
private:
	cStatic*		m_csExpRatePlayer1;
	cStatic*		m_csExpRatePlayer2;

public:
	void			SetChallengeZoneExpRatePlayer1( LPCTSTR );
	void			SetChallengeZoneExpRatePlayer2( LPCTSTR );

};



#endif