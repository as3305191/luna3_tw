#include "stdafx.h"
#include "weatherdialog.h"
#include "WindowIDEnum.h"
#include ".\Interface\cWindowManager.h"

// 081020 LYW --- WeatherDialog : SetActive �Լ� �߰�(������������ �˾Ƽ� Ȱ��ȭ ���� �ʵ��� �����Ѵ�. - �۰���).
#include "./SiegeWarfareMgr.h"

//090525 pdy �Ͽ�¡ �߰�(�Ͽ�¡������ �˾Ƽ� Ȱ��ȭ ���� �ʵ��� �����Ѵ� ).
#include "cHousingMgr.h"

CWeatherDialog::CWeatherDialog(void)
{
}

CWeatherDialog::~CWeatherDialog(void)
{
}

void CWeatherDialog::Linking()
{
/*	int i;
	for(i=0; i<eWS_Max; i++)
	{
		m_pWeatherBtn[i] = (cButton*)GetWindowForID(WEATHER_BTN_CLEAR + i);
		m_pWeatherBtn[i]->SetActive(FALSE);
		m_pWeatherBtn[i]->SetDisable(TRUE);
	}*/
}

void CWeatherDialog::SetWeather(WORD wState)
{
	int i;
	for(i=0; i<eWS_Max; i++)
	{
		m_pWeatherBtn[i]->SetDisable(FALSE);

		if(i == (int)wState)
			m_pWeatherBtn[i]->SetActive(TRUE);
		else
			m_pWeatherBtn[i]->SetActive(FALSE);

		m_pWeatherBtn[i]->SetDisable(TRUE);
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: SetActive
//	DESC		: SetActive �Լ� �߰�(������������ �˾Ƽ� Ȱ��ȭ ���� �ʵ��� �����Ѵ�. - �۰���).
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 20, 2008
//-------------------------------------------------------------------------------------------------
void CWeatherDialog::SetActive(BOOL val)
{
	// 090525 pdy --- MiniMapDlg : �Ͽ�¡ ���̸�, CWeatherDialog�� ��Ȱ��ȭ �Ѵ�.
  	if( SIEGEWARFAREMGR->IsSiegeWarfareZone() || 
		HOUSINGMGR->IsHousingMap() )
  	{
  		cDialog::SetActive( FALSE ) ;
  		return ;
  	}
	else
	{
		cDialog::SetActive( val ) ;
	}
}