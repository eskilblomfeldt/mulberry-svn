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

#include "CNewComponentTiming.h"

#include "CDateTimeZoneSelect.h"
#include "CNewEventTiming.h"
#include "CNewToDoTiming.h"
#include "CNumberEdit.h"
#include "CRecurrenceDialog.h"
#include "CStaticText.h"

#include "CICalendarDuration.h"
#include "CICalendar.h"
#include "CICalendarRecurrence.h"
#include "CICalendarRecurrenceSet.h"

#include <LCheckBox.h>
#include <LLittleArrows.h>
#include <LPopupButton.h>
#include <LPushButton.h>
#include <LRadioButton.h>
#include <LTabsControl.h>
#include "MyCFString.h"

// ---------------------------------------------------------------------------
//	CNewComponentTiming														  [public]
/**
	Default constructor */

CNewComponentTiming::CNewComponentTiming(LStream *inStream) :
	CNewComponentPanel(inStream)
{
	mTimingPanel = NULL;
}


// ---------------------------------------------------------------------------
//	~CNewComponentTiming														  [public]
/**
	Destructor */

CNewComponentTiming::~CNewComponentTiming()
{
}

#pragma mark -

void CNewComponentTiming::FinishCreateSelf()
{
	// Get UI items
	mTimingView = dynamic_cast<LView*>(FindPaneByID(eTimingView_ID));

	mRepeats = dynamic_cast<LCheckBox*>(FindPaneByID(eRepeats_ID));
	mRepeatsTabs = dynamic_cast<LTabsControl*>(FindPaneByID(eRepeatsTabs_ID));

	mOccursSimpleItems = dynamic_cast<LView*>(FindPaneByID(eOccursSimpleItems_ID));

	mOccursInterval = dynamic_cast<CNumberEdit*>(FindPaneByID(eOccursInterval_ID));
	mOccursIntervalSpin = dynamic_cast<LLittleArrows*>(FindPaneByID(eOccursIntervalSpin_ID));
	mOccursInterval->SetArrows(mOccursIntervalSpin, 1, 1000, 0);
	mOccursFreq = dynamic_cast<LPopupButton*>(FindPaneByID(eOccursFreq_ID));

	mOccursForEver = dynamic_cast<LRadioButton*>(FindPaneByID(eOccursForEver_ID));
	mOccursCount = dynamic_cast<LRadioButton*>(FindPaneByID(eOccursCount_ID));
	mOccursUntil = dynamic_cast<LRadioButton*>(FindPaneByID(eOccursUntil_ID));

	mOccursCounter = dynamic_cast<CNumberEdit*>(FindPaneByID(eOccursCounter_ID));
	mOccursCounterSpin = dynamic_cast<LLittleArrows*>(FindPaneByID(eOccursCounterSpin_ID));
	mOccursCounter->SetArrows(mOccursCounterSpin, 1, 1000, 0);

	mOccursDateTimeZone = CDateTimeZoneSelect::CreateInside(dynamic_cast<LView*>(FindPaneByID(eOccursDateTimeZone_ID)));

	mOccursAdvancedItems = dynamic_cast<LView*>(FindPaneByID(eOccursAdvancedItems_ID));

	mOccursDescription = dynamic_cast<CStaticText*>(FindPaneByID(eOccursDescription_ID));
	mOccursEdit = dynamic_cast<LPushButton*>(FindPaneByID(eOccursEdit_ID));
	
	// Listen to some controls
	UReanimator::LinkListenerToBroadcasters(this, this, pane_ID);
	
	// Init controls
	DoRepeat(false);
	DoRepeatTab(eOccurs_Simple);
	DoOccursGroup(eOccurs_ForEver);
}

// Respond to clicks in the icon buttons
void CNewComponentTiming::ListenToMessage(MessageT inMessage,void *ioParam)
{
	switch (inMessage)
	{
	case eOccursEdit_ID:
		DoOccursEdit();
		break;
	case eRepeats_ID:
		DoRepeat(mRepeats->GetValue() == 1);
		break;
	case eRepeatsTabs_ID:
		DoRepeatTab(mRepeatsTabs->GetValue());
		break;
	case eOccursForEver_ID:
		DoOccursGroup(eOccurs_ForEver);
		break;
	case eOccursCount_ID:
		DoOccursGroup(eOccurs_Count);
		break;
	case eOccursUntil_ID:
		DoOccursGroup(eOccurs_Until);
		break;
	}
}

void CNewComponentTiming::DoRepeat(bool repeat)
{
	mRepeatsTabs->SetEnabled(repeat);
}

void CNewComponentTiming::DoRepeatTab(UInt32 value)
{
	switch(value)
	{
	case eOccurs_Simple:
		mOccursSimpleItems->SetVisible(true);
		mOccursAdvancedItems->SetVisible(false);
		break;
	case eOccurs_Advanced:
		mOccursSimpleItems->SetVisible(false);
		mOccursAdvancedItems->SetVisible(true);
		mOccursEdit->SetVisible(true);
		
		// Set description to advanced item
		{
			MyCFString txt(mAdvancedRecur.GetUIDescription(), kCFStringEncodingUTF8);
			mOccursDescription->SetCFDescriptor(txt);
		}
		break;
	case eOccurs_Complex:
		mOccursSimpleItems->SetVisible(false);
		mOccursAdvancedItems->SetVisible(true);
		mOccursEdit->SetVisible(false);
		
		// Set description to complex item
		{
			MyCFString txt(mComplexDescription, kCFStringEncodingUTF8);
			mOccursDescription->SetCFDescriptor(txt);
		}
		break;
	}
}

void CNewComponentTiming::DoOccursGroup(UInt32 value)
{
	mOccursCounter->SetEnabled(value == eOccurs_Count);
	mOccursCounterSpin->SetEnabled(value == eOccurs_Count);

	mOccursDateTimeZone->SetEnabled(value == eOccurs_Until);
}

void CNewComponentTiming::DoOccursEdit()
{
	// Get tzid set in the start
	iCal::CICalendarTimezone tzid;
	mTimingPanel->GetTimezone(tzid);

	bool all_day = mTimingPanel->GetAllDay();

	// Edit the stored recurrence item
	iCal::CICalendarRecurrence	temp(mAdvancedRecur);
	
	if (CRecurrenceDialog::PoseDialog(temp, tzid, all_day))
	{
		mAdvancedRecur = temp;

		// Update description
		MyCFString txt(mAdvancedRecur.GetUIDescription(), kCFStringEncodingUTF8);
		mOccursDescription->SetCFDescriptor(txt);
	}
}

void CNewComponentTiming::SetEvent(const iCal::CICalendarVEvent& vevent)
{
	if (mTimingPanel == NULL)
	{
		// Find super commander in view chain
		LView* parent = GetSuperView();
		while((parent != NULL) && (dynamic_cast<LDialogBox*>(parent) == NULL))
			parent = parent->GetSuperView();
		LDialogBox* dlog = dynamic_cast<LDialogBox*>(parent);

		// Get the tab group
		LCommander* defCommander = NULL;
		if (dlog)
			dlog->GetSubCommanders().FetchItemAt(1, defCommander);

		// Load each panel for the tabs
		mTimingPanel = static_cast<CNewEventTiming*>(UReanimator::CreateView(CNewEventTiming::pane_ID, mTimingView, defCommander));
		mTimingView->ExpandSubPane(mTimingPanel, true, true);
		mTimingPanel->SetVisible(true);
	}

	// Set the relevant fields
	
	// Do timing panel
	mTimingPanel->SetEvent(vevent);
	
	// Set recurrence
	SetRecurrence(vevent.GetRecurrenceSet());
}

void CNewComponentTiming::SetToDo(const iCal::CICalendarVToDo& vtodo)
{
	if (mTimingPanel == NULL)
	{
		// Find super commander in view chain
		LView* parent = GetSuperView();
		while((parent != NULL) && (dynamic_cast<LDialogBox*>(parent) == NULL))
			parent = parent->GetSuperView();
		LDialogBox* dlog = dynamic_cast<LDialogBox*>(parent);

		// Get the tab group
		LCommander* defCommander = NULL;
		if (dlog)
			dlog->GetSubCommanders().FetchItemAt(1, defCommander);

		// Load each panel for the tabs
		mTimingPanel = static_cast<CNewToDoTiming*>(UReanimator::CreateView(CNewToDoTiming::pane_ID, mTimingView, defCommander));
		mTimingView->ExpandSubPane(mTimingPanel, true, true);
		mTimingPanel->SetVisible(true);
	}

	// Set the relevant fields
	
	// Do timing panel
	mTimingPanel->SetToDo(vtodo);
	
	// Set recurrence
	//SetRecurrence(vtodo.GetRecurrenceSet());
}

void CNewComponentTiming::SetRecurrence(const iCal::CICalendarRecurrenceSet* recurs)
{
	static const int cFreqValueToPopup[] =
	{
		CNewComponentTiming::eOccurs_Secondly, CNewComponentTiming::eOccurs_Minutely, CNewComponentTiming::eOccurs_Hourly,
		CNewComponentTiming::eOccurs_Daily, CNewComponentTiming::eOccurs_Weekly, CNewComponentTiming::eOccurs_Monthly, CNewComponentTiming::eOccurs_Yearly
	};

	// See whether it is simple enough that we can handle it
	if ((recurs != NULL) && recurs->HasRecurrence())
	{
		if (recurs->IsSimpleUI())
		{
			const iCal::CICalendarRecurrence* recur = recurs->GetUIRecurrence();

			// Is repeating
			mRepeats->SetValue(1);
			mRepeatsTabs->SetValue(eOccurs_Simple);

			// Set frequency
			mOccursFreq->SetValue(cFreqValueToPopup[recur->GetFreq()]);
			
			// Set interval
			mOccursInterval->SetNumberValue(recur->GetInterval());
			
			// Set count
			if (recur->GetUseCount())
			{
				mOccursCount->SetValue(1);
				mOccursCounter->SetNumberValue(recur->GetCount());
			}
			else if (recur->GetUseUntil())
			{
				mOccursUntil->SetValue(1);
				
				// NB the UNTIL value is always UTC, but we want to display it to the user relative to their start time
				
				// Get tzid set in the start
				iCal::CICalendarTimezone tzid;
				mTimingPanel->GetTimezone(tzid);

				// Adjust UNTIL to new timezone
				iCal::CICalendarDateTime until(recur->GetUntil());
				until.AdjustTimezone(tzid);

				mOccursDateTimeZone->SetDateTimeZone(until, mTimingPanel->GetAllDay());
			}
			else
				mOccursForEver->SetValue(1);
			
			// Always remove the complex tab as user cannot create a complex item
			mRepeatsTabs->SetMaxValue(2);
			return;
		}
		else if (recurs->IsAdvancedUI())
		{
			const iCal::CICalendarRecurrence* recur = recurs->GetUIRecurrence();

			// Cache the value we will be editing
			mAdvancedRecur = *recur;
			
			// Is repeating
			mRepeats->SetValue(1);
			mRepeatsTabs->SetValue(eOccurs_Advanced);
			
			// Always remove the complex tab as user cannot create a complex item
			mRepeatsTabs->SetMaxValue(2);
			
			return;
		}
		
		// Fall through to here => complex rule
		mComplexDescription = recurs->GetUIDescription();

		// Is repeating
		mRepeats->SetValue(1);
		mRepeatsTabs->SetValue(eOccurs_Complex);
				
	}
	else
	{
		// Is not repeating
		mRepeats->SetValue(0);
		mRepeatsTabs->SetValue(eOccurs_Simple);
		
		// Always remove the complex tab as user cannot create a complex item
		mRepeatsTabs->SetMaxValue(2);
	}
}

void CNewComponentTiming::GetEvent(iCal::CICalendarVEvent& vevent)
{
	// Do timing panel
	mTimingPanel->GetEvent(vevent);
	
	// Do recurrence items
	// NB in complex mode we do not change the existing set
	iCal::CICalendarRecurrenceSet recurs;
	if (GetRecurrence(recurs))
		vevent.EditRecurrenceSet(recurs);
}

void CNewComponentTiming::GetPeriod(iCal::CICalendarPeriod& period)
{
	// Do timing panel
	static_cast<CNewEventTiming*>(mTimingPanel)->GetPeriod(period);
}

void CNewComponentTiming::GetToDo(iCal::CICalendarVToDo& vtodo)
{
	// Do timing panel
	mTimingPanel->GetToDo(vtodo);
	
	// Do recurrence items
	// NB in complex mode we do not change the existing set
	//iCal::CICalendarRecurrenceSet recurs;
	//if (GetRecurrence(recurs))
	//	vtodo.EditRecurrenceSet(recurs);
}

static const iCal::ERecurrence_FREQ cFreqPopupToValue[] = 
{
	iCal::eRecurrence_YEARLY, iCal::eRecurrence_MONTHLY, iCal::eRecurrence_WEEKLY, iCal::eRecurrence_DAILY,
	iCal::eRecurrence_HOURLY, iCal::eRecurrence_MINUTELY, iCal::eRecurrence_SECONDLY
};

bool CNewComponentTiming::GetRecurrence(iCal::CICalendarRecurrenceSet& recurs)
{
	// Only if repeating enabled
	if (mRepeats->GetValue() == 0)
		return true;
	
	// Do not do anything to existing recurrence if complex mode
	if (mRepeatsTabs->GetValue() == eOccurs_Complex)
		return false;
	
	// Get simple/advanced data
	if (mRepeatsTabs->GetValue() == eOccurs_Simple)
	{
		// Simple mode means we only do a single RRULE
		iCal::CICalendarRecurrence recur;
		
		// Set frequency
		recur.SetFreq(cFreqPopupToValue[mOccursFreq->GetValue() - 1]);
		
		// Set interval
		recur.SetInterval(mOccursInterval->GetNumberValue());
		
		// Determine end
		if (mOccursForEver->GetValue() == 1)
		{
			// Nothing to add
		}
		else if (mOccursCount->GetValue() == 1)
		{
			recur.SetUseCount(true);
			recur.SetCount(mOccursCounter->GetNumberValue());
		}
		else if (mOccursUntil->GetValue() == 1)
		{
			// NB the UNTIL value is always UTC, but we want to display it to the user relative to their start time,
			// so we must convert from dialog tzid to UTC
			
			// Get value from dialog
			iCal::CICalendarDateTime until;
			mOccursDateTimeZone->GetDateTimeZone(until, mTimingPanel->GetAllDay());
			
			// Adjust to UTC
			until.AdjustToUTC();

			recur.SetUseUntil(true);
			recur.SetUntil(until);
		}
		
		// Now add recurrence
		recurs.Add(recur);
	}
	else
		// Just add advanced item
		recurs.Add(mAdvancedRecur);
	
	return true;
}

void CNewComponentTiming::SetReadOnly(bool read_only)
{
	mReadOnly = read_only;

	mTimingPanel->SetReadOnly(read_only);

	mRepeats->SetEnabled(!read_only);
	mRepeatsTabs->SetEnabled(!read_only && (mRepeats->GetValue() == 1));
}
