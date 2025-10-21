#include "StdAfx.h"
#include "client.h"
#include "ConsignLogCommand.h"

CConsignLogCommand::CConsignLogCommand(CclientApp& application, const TCHAR* title, const CConsignLogCommand::Configuration& configuration) :
CCommand(application, title),
mConfiguration(configuration)
{}

void CConsignLogCommand::SaveToExcel(DWORD serverIndex, const CListCtrl& listCtrl) const
{
	CString textHead;
	textHead.LoadString(IDS_STRING165);

	CString textPoint;
	textPoint.LoadString(IDS_STRING34);

	CString textLog;
	textLog.LoadString(IDS_STRING10);

	CclientApp::SaveToExcel(
		textHead + _T(" ") + textPoint + _T(" ") + textLog + _T("-") + mApplication.GetServerName(serverIndex),
		textLog,
		listCtrl);
}

void CConsignLogCommand::Initialize(CListCtrl& listCtrl) const
{
	struct Field
	{
		int mText;
		int mWidth;
		int mFormat;
	};
	const Field fieldArray[] = {
		{IDS_STRING22, 0, LVCFMT_RIGHT}, // �α� ��ȣ
		{IDS_STRING3, 110, LVCFMT_LEFT}, // ����
		{IDS_STRING522, 100, LVCFMT_RIGHT}, // �ǸŴ��� ��ȣ
		{IDS_STRING54, 80, LVCFMT_RIGHT}, // �÷��̾� ��ȣ
		{IDS_STRING4, 70, LVCFMT_LEFT}, // �α� ����
		{IDS_STRING482, 100, LVCFMT_RIGHT}, // ������ DB��ȣ
		{IDS_STRING189, 0, LVCFMT_RIGHT}, // ������ ��ȣ
		{IDS_STRING189, 100, LVCFMT_LEFT}, // ������ �̸�
		{IDS_STRING42, 40, LVCFMT_RIGHT},
		{IDS_STRING58, 100, LVCFMT_RIGHT},
	};

	CRect rect;
	listCtrl.GetClientRect(
		rect);
	
	for(int i = 0; i < _countof(fieldArray); ++i)
	{
		const Field& field = fieldArray[i];

		CString text;
		text.LoadString(
			field.mText);
		listCtrl.InsertColumn(
			i,
			text,
			field.mFormat,
			field.mWidth);
	}
}

void CConsignLogCommand::Find(DWORD serverIndex, const TCHAR* beginTime, const TCHAR* endTime)
{
	MSG_RM_CONSIGN_LOG_REQUEST message;
	ZeroMemory(
		&message,
		sizeof(message));
	message.Category = MP_RM_CONSIGN_LOG;
	message.Protocol = MP_RM_CONSIGN_LOG_SIZE_SYN;
	message.mRequestTick = mTickCount = GetTickCount();
	message.dwObjectID	= mConfiguration.mPlayerIndex;
	message.mItemDBIndex = mConfiguration.mItemDBIndex;
	message.mConsignIndex = mConfiguration.mConsignIndex;
	strncpy(
		message.mBeginDate,
		CW2AEX< MAX_PATH >(beginTime),
		sizeof(message.mBeginDate));
	strncpy(
		message.mEndDate,
		CW2AEX< MAX_PATH >(endTime),
		sizeof(message.mEndDate));
	mApplication.Send(
		serverIndex,
		message,
		sizeof(message));
}


void CConsignLogCommand::Stop(DWORD serverIndex) const
{
	MSGROOT message;
	ZeroMemory(
		&message,
		sizeof(message));
	message.Category = MP_RM_CONSIGN_LOG;
	message.Protocol = MP_RM_CONSIGN_LOG_STOP_SYN;
	mApplication.Send(
		serverIndex,
		message,
		sizeof(message));
}

void CConsignLogCommand::Parse(const MSGROOT* message, CListCtrl& listCtrl, CProgressCtrl& progressCtrl, CStatic& resultStatic, CButton& findButton, CButton& stopButton) const
{
	switch(message->Protocol)
	{
	case MP_RM_CONSIGN_LOG_ACK:
		{
			const DWORD previousLogIndex = _ttoi(listCtrl.GetItemText(
				listCtrl.GetItemCount() - 1,
				0));

			const MSG_RM_CONSIGN_LOG* m = (MSG_RM_CONSIGN_LOG*)message;

			for(size_t i = 0; i < m->mSize; ++i)
			{
				const MSG_RM_CONSIGN_LOG::Log& log = m->mLog[i];

				if(previousLogIndex > log.mLogIndex)
				{
					continue;
				}

				int field = -1;
				const int row = listCtrl.GetItemCount();
				TCHAR buffer[MAX_PATH] = {0};

				listCtrl.InsertItem(
					row,
					_itot(log.mLogIndex, buffer, 10),
					++field);
				listCtrl.SetItemText(
					row,
					++field,
					CString(log.mDate));
				listCtrl.SetItemText(
					row,
					++field,
					_itot(log.mConsignIndex, buffer, 10));
				listCtrl.SetItemText(
					row,
					++field,
					_itot(log.mPlayerIndex, buffer, 10));
				listCtrl.SetItemText(
					row,
					++field,
					mApplication.GetText(log.mType));
				listCtrl.SetItemText(
					row,
					++field,
					_itot(log.mItemDbIndex, buffer, 10));
				listCtrl.SetItemText(
					row,
					++field,
					_itot(log.mItemIndex, buffer, 10));
				listCtrl.SetItemText(
					row,
					++field,
					mApplication.GetItemName(log.mItemIndex));
				listCtrl.SetItemText(
					row,
					++field,
					_itot(log.mQuantity, buffer, 10));
				listCtrl.SetItemText(
					row,
					++field,
					mApplication.ConvertToText(log.mPrice));
			}

			{
				int minRange = 0;
				int maxRange = 0;
				progressCtrl.GetRange(minRange, maxRange);
				progressCtrl.SetPos(progressCtrl.GetPos() + int(m->mSize));

				CString text;
				text.Format(_T("%d/%d"), progressCtrl.GetPos(), maxRange);
				resultStatic.SetWindowText(text);

				// 080523 LUJ, ��ư Ȱ��ȭ üũ�� ���������� �ǵ��� ������
				if(progressCtrl.GetPos() == maxRange)
				{
					findButton.EnableWindow(TRUE);
					stopButton.EnableWindow(FALSE);
				}
			}

			break;
		}
	case MP_RM_CONSIGN_LOG_SIZE_ACK:
		{
			const MSG_DWORD* m = (MSG_DWORD*)message;
			const DWORD size = m->dwData;
			CString text;
			text.Format(_T("0/%d"), size);

			resultStatic.SetWindowText(text);
			progressCtrl.SetRange32(0, size);
			progressCtrl.SetPos(0);
			findButton.EnableWindow(FALSE);
			stopButton.EnableWindow(TRUE);
			listCtrl.DeleteAllItems();
			break;
		}
	case MP_RM_CONSIGN_LOG_SIZE_NACK:
		{
			CString textThereIsNoResult;
			textThereIsNoResult.LoadString(IDS_STRING1);
			MessageBox(0, textThereIsNoResult, _T(""), MB_ICONERROR | MB_OK);
			break;
		}
	case MP_RM_CONSIGN_LOG_STOP_ACK:
		{
			findButton.EnableWindow(TRUE);
			stopButton.EnableWindow(FALSE);

			CString textSearchWasStopped;
			textSearchWasStopped.LoadString(IDS_STRING2);

			MessageBox(0, textSearchWasStopped, _T(""), MB_ICONERROR | MB_OK);
			break;
		}
	case MP_RM_CONSIGN_LOG_NACK_BY_AUTH:
		{
			CString textYouHaveNoAuthority;
			textYouHaveNoAuthority.LoadString(IDS_STRING18);

			MessageBox(0, textYouHaveNoAuthority, _T(""), MB_OK | MB_ICONERROR);
			break;
		}
	default:
		{
			ASSERT(0);
			break;
		}
	}
}
