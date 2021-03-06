/*
    Copyright (c) 2007 Cyrus Daboo. All rights reserved.
    
    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at
    
        http://www.apache.org/licenses/LICENSE-2.0
    
    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "CYearView.h"

#include "CCommands.h"
#include "CGUtils.h"
#include "CLog.h"
#include "CStaticText.h"
#include "CStatusWindow.h"
#include "CYearPrintout.h"

#include "CCalendarStoreManager.h"

#include <LBevelButton.h>
#include <LPopupButton.h>
#include "MyCFString.h"

// ---------------------------------------------------------------------------
//	CYearView														  [public]
/**
	Default constructor */

CYearView::CYearView(LStream *inStream) :
	CCalendarViewBase(inStream)
{
	mLayoutValue = NCalendarView::e3x4;
}


// ---------------------------------------------------------------------------
//	~CYearView														  [public]
/**
	Destructor */

CYearView::~CYearView()
{
}

#pragma mark -

void CYearView::FinishCreateSelf()
{
	// Get the UI objects
	mYearMinus = dynamic_cast<LBevelButton*>(FindPaneByID(eYearMinus_ID));
	mYearPlus = dynamic_cast<LBevelButton*>(FindPaneByID(eYearPlus_ID));

	mYear = dynamic_cast<CStaticText*>(FindPaneByID(eYear_ID));
	
	mLayout = dynamic_cast<LPopupButton*>(FindPaneByID(eLayout_ID));
	mLayoutValue = NCalendarView::e3x4;
	mLayout->SetValue(mLayoutValue + 1);
	
	mTable = dynamic_cast<CYearTable*>(FindPaneByID(eTable_ID));
	mTable->SetCalendarView(this);
	mTable->Add_Listener(this);
	
	// Listen to some controls
	UReanimator::LinkListenerToBroadcasters(this, this, pane_ID);
}

CCalendarTableBase* CYearView::GetTable() const
{
	return mTable;
}

// Respond to clicks in the icon buttons
void CYearView::ListenToMessage(MessageT inMessage,void *ioParam)
{
	switch (inMessage)
	{
	case eYearMinus_ID:
		OnPreviousYear();
		break;

	case eYearPlus_ID:
		OnNextYear();
		break;

	case eLayout_ID:
		OnLayout();
		break;
	}
}

//	Respond to commands
Boolean CYearView::ObeyCommand(CommandT inCommand,void *ioParam)
{
	bool cmdHandled = true;

	switch (inCommand)
	{
	case cmd_PageSetup:
		{
			StPrintSession			session(*GetPrintSpec());
			StDesktopDeactivator	deactivator;
			UPrinting::AskPageSetup(*GetPrintSpec());
		}
		break;

	case cmd_Print:
	case cmd_ToolbarMessagePrintBtn:
		//  Hide status window as Print Manager gets name of top window and does not know about floats
		{
			StPrintSession	session(*GetPrintSpec());
			StStatusWindowHide hide;
			bool do_print = false;
			{
				StDesktopDeactivator	deactivator;
				do_print = UPrinting::AskPrintJob(*GetPrintSpec());
			}
			if (do_print)
				DoPrint();
		}

		// Print job status window might mess up window order
		UDesktop::NormalizeWindowOrder();
		break;

	case cmd_NewEvent:
	case cmd_ToolbarNewEventBtn:
		OnNewEvent();
		break;

	case cmd_GotoToday:
	case cmd_ToolbarShowTodayBtn:
		OnThisYear();
		break;

	case cmd_GotoDate:
	case cmd_ToolbarGoToDateBtn:
		OnPickDate();
		break;

	default:
		cmdHandled = CCalendarViewBase::ObeyCommand(inCommand, ioParam);
		break;
	};

	return cmdHandled;
}

//	Pass back status of a (menu) command
void CYearView::FindCommandStatus(
	CommandT	inCommand,
	Boolean		&outEnabled,
	Boolean		&outUsesMark,
	UInt16		&outMark,
	Str255		outName)
{
	outUsesMark = false;

	switch (inCommand)
	{
	case cmd_NewEvent:
	case cmd_ToolbarNewEventBtn:
	{
		// Only if subscribed nodes
		if (calstore::CCalendarStoreManager::sCalendarStoreManager != NULL)
		{
			const iCal::CICalendarList& cals = calstore::CCalendarStoreManager::sCalendarStoreManager->GetSubscribedCalendars();
			outEnabled = (GetCalendar() != NULL) || (cals.size() > 0);
		}
		else
			outEnabled = false;
		break;
	}

	case cmd_PageSetup:
	case cmd_Print:
	case cmd_ToolbarMessagePrintBtn:
	case cmd_GotoToday:
	case cmd_ToolbarShowTodayBtn:
	case cmd_GotoDate:
	case cmd_ToolbarGoToDateBtn:
		// Always enabled
		outEnabled = true;
		break;

	default:
		CCalendarViewBase::FindCommandStatus(inCommand, outEnabled, outUsesMark, outMark, outName);
		break;
	}
}

void CYearView::SetCaptions()
{
	// Set static text
	char buf[256];
	std::snprintf(buf, 256, "%ld", mDate.GetYear());
	MyCFString year(buf, kCFStringEncodingUTF8);
	mYear->SetCFDescriptor(year);
}

void CYearView::SetLayout(NCalendarView::EYearLayout layout)
{
	mLayoutValue = layout;
	mLayout->SetValue(mLayoutValue + 1);
}

void CYearView::OnLayout()
{
	mLayoutValue = static_cast<NCalendarView::EYearLayout>(mLayout->GetValue() - 1);
	ResetDate();
}

void CYearView::DoPrint()
{
	auto_ptr<CYearPrintout> pout;

	try
	{
		pout.reset(static_cast<CYearPrintout*>(LPrintout::CreatePrintout(CYearPrintout::pane_ID)));
		pout->SetPrintSpec(*GetPrintSpec());
		pout->SetDetails(mDate.GetYear(), mLayoutValue);

		pout->DoPrintJob();
	}
	catch (...)
	{
		CLOG_LOGCATCH(...);
	}
}
