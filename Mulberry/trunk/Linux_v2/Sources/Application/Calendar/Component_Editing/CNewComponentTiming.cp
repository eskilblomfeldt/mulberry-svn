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
#include "CDivider.h"
#include "CNewEventTiming.h"
#include "CNewToDoTiming.h"
#include "CNumberEdit.h"
#include "CRecurrenceDialog.h"
#include "CTabController.h"

#include "CICalendarDuration.h"
#include "CICalendar.h"
#include "CICalendarRecurrence.h"
#include "CICalendarRecurrenceSet.h"

#include "TPopupMenu.h"

#include <JXFlatRect.h>
#include <JXRadioGroup.h>
#include <JXStaticText.h>
#include <JXTextCheckbox.h>
#include <JXTextRadioButton.h>
#include <JXTextButton.h>

#include <cassert>

#pragma mark -

void CNewComponentTiming::OnCreate()
{
	// Get UI items
// begin JXLayout1

    mTimingView =
        new JXFlatRect(this,
                    JXWidget::kHElastic, JXWidget::kVElastic, 0,5, 500,130);
    assert( mTimingView != NULL );

    mRepeats =
        new JXTextCheckbox("Repeats", this,
                    JXWidget::kHElastic, JXWidget::kVElastic, 10,140, 80,20);
    assert( mRepeats != NULL );

    CDivider* obj1 =
        new CDivider(this,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 88,150, 402,2);
    assert( obj1 != NULL );

    mRepeatsTabs =
        new CTabController(this,
                    JXWidget::kHElastic, JXWidget::kVElastic, 10,160, 480,140);
    assert( mRepeatsTabs != NULL );

// end JXLayout1

	mRepeatSimpleItems = new CNewComponentRepeatSimple(mRepeatsTabs->GetCardEnclosure(), JXWidget::kFixedLeft, JXWidget::kFixedTop, 0, 0, 500, 120);
	mRepeatsTabs->AppendCard(mRepeatSimpleItems, "Simple");
	mRepeatAdavancedItems = new CNewComponentRepeatAdvanced(mRepeatsTabs->GetCardEnclosure(), JXWidget::kFixedLeft, JXWidget::kFixedTop, 0, 0, 500, 120);
	mRepeatsTabs->AppendCard(mRepeatAdavancedItems, "Advanced");
	mRepeatComplexItems = new CNewComponentRepeatComplex(mRepeatsTabs->GetCardEnclosure(), JXWidget::kFixedLeft, JXWidget::kFixedTop, 0, 0, 500, 120);
	mRepeatsTabs->AppendCard(mRepeatComplexItems, "Complex");

	// Listen to some controls
	ListenTo(mRepeats);
	ListenTo(mRepeatsTabs);
	ListenTo(mRepeatSimpleItems->mOccursGroup);
	ListenTo(mRepeatAdavancedItems->mOccursEdit);
	
	// Init controls
	DoRepeat(false);
	DoRepeatTab(eOccurs_Simple);
	DoOccursGroup(eOccurs_ForEver);
}

// Respond to clicks in the icon buttons
void CNewComponentTiming::Receive(JBroadcaster* sender, const Message& message)
{
	if (message.Is(JXCheckbox::kPushed))
	{
		if (sender == mRepeats)
		{
			DoRepeat(mRepeats->IsChecked());
			return;
		}
	}
	else if (message.Is(JXRadioGroup::kSelectionChanged))
	{
		JIndex index = dynamic_cast<const JXRadioGroup::SelectionChanged*>(&message)->GetID();
		if (sender == mRepeatsTabs)
			DoRepeatTab(index);
		else if (sender == mRepeatSimpleItems->mOccursGroup)
			DoOccursGroup(index);
		return;
	}
	else if (message.Is(JXButton::kPushed))
	{
		if (sender == mRepeatAdavancedItems->mOccursEdit)
		{
			DoOccursEdit();
			return;
		}
	}
}

void CNewComponentTiming::DoRepeat(bool repeat)
{
	mRepeatsTabs->SetActive(repeat);
}

void CNewComponentTiming::DoRepeatTab(JIndex value)
{
	switch(value)
	{
	case eOccurs_Simple:
		break;
	case eOccurs_Advanced:
		// Set description to advanced item
		mRepeatAdavancedItems->mOccursDescription->SetText(mAdvancedRecur.GetUIDescription());
		break;
	case eOccurs_Complex:
		// Set description to complex item
		mRepeatComplexItems->mOccursDescription->SetText(mComplexDescription);
		break;
	}
}

void CNewComponentTiming::DoOccursGroup(JIndex value)
{
	mRepeatSimpleItems->mOccursCounter->SetActive(value == eOccurs_Count);
	mRepeatSimpleItems->mOccursDateTimeZone->SetActive(value == eOccurs_Until);
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
		mRepeatAdavancedItems->mOccursDescription->SetText(mAdvancedRecur.GetUIDescription());
	}
}

void CNewComponentTiming::SetEvent(const iCal::CICalendarVEvent& vevent)
{
	if (mTimingPanel == NULL)
	{
		// Create the event timing panel
		mTimingPanel = new CNewEventTiming(mTimingView, JXWidget::kFixedLeft, JXWidget::kFixedTop, 0, 0, 500, 130);
		mTimingPanel->OnCreate();
		mTimingPanel->FitToEnclosure();
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
		// Create the event timing panel
		mTimingPanel = new CNewToDoTiming(mTimingView, JXWidget::kFixedLeft, JXWidget::kFixedTop, 0, 0, 500, 130);
		mTimingPanel->OnCreate();
		mTimingPanel->FitToEnclosure();
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
			mRepeats->SetState(true);
			mRepeatsTabs->ShowCard(eOccurs_Simple);

			// Set frequency
			mRepeatSimpleItems->mOccursFreq->SetValue(cFreqValueToPopup[recur->GetFreq()]);
			
			// Set interval
			mRepeatSimpleItems->mOccursInterval->SetNumberValue(recur->GetInterval());
			
			// Set count
			if (recur->GetUseCount())
			{
				mRepeatSimpleItems->mOccursGroup->SelectItem(eOccurs_Count);
				mRepeatSimpleItems->mOccursCounter->SetNumberValue(recur->GetCount());
			}
			else if (recur->GetUseUntil())
			{
				mRepeatSimpleItems->mOccursGroup->SelectItem(eOccurs_Until);
				
				// NB the UNTIL value is always UTC, but we want to display it to the user relative to their start time
				
				// Get tzid set in the start
				iCal::CICalendarTimezone tzid;
				mTimingPanel->GetTimezone(tzid);

				// Adjust UNTIL to new timezone
				iCal::CICalendarDateTime until(recur->GetUntil());
				until.AdjustTimezone(tzid);

				mRepeatSimpleItems->mOccursDateTimeZone->SetDateTimeZone(until, mTimingPanel->GetAllDay());
			}
			else
				mRepeatSimpleItems->mOccursGroup->SelectItem(eOccurs_ForEver);
			
			// Always remove the complex tab as user cannot create a complex item
			mRepeatsTabs->DeactivateCard(eOccurs_Complex);
			return;
		}
		else if (recurs->IsAdvancedUI())
		{
			const iCal::CICalendarRecurrence* recur = recurs->GetUIRecurrence();

			// Cache the value we will be editing
			mAdvancedRecur = *recur;
			
			// Is repeating
			mRepeats->SetState(true);
			mRepeatsTabs->ShowCard(eOccurs_Advanced);
			
			// Always remove the complex tab as user cannot create a complex item
			mRepeatsTabs->DeactivateCard(eOccurs_Complex);
			
			return;
		}
		
		// Fall through to here => complex rule
		mComplexDescription = recurs->GetUIDescription();

		// Is repeating
		mRepeats->SetState(true);
		mRepeatsTabs->ShowCard(eOccurs_Complex);
				
	}
	else
	{
		// Is not repeating
		mRepeats->SetState(false);
		mRepeatsTabs->ShowCard(eOccurs_Simple);
		
		// Always remove the complex tab as user cannot create a complex item
		mRepeatsTabs->DeactivateCard(eOccurs_Complex);
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
	if (!mRepeats->IsChecked())
		return true;
	
	// Do not do anything to existing recurrence if complex mode
	if (mRepeatsTabs->GetSelectedItem() == eOccurs_Complex)
		return false;
	
	// Get simple/advanced data
	if (mRepeatsTabs->GetSelectedItem() == eOccurs_Simple)
	{
		// Simple mode means we only do a single RRULE
		iCal::CICalendarRecurrence recur;
		
		// Set frequency
		recur.SetFreq(cFreqPopupToValue[mRepeatSimpleItems->mOccursGroup->GetSelectedItem() - 1]);
		
		// Set interval
		recur.SetInterval(mRepeatSimpleItems->mOccursInterval->GetNumberValue());
		
		// Determine end
		if (mRepeatSimpleItems->mOccursGroup->GetSelectedItem() == eOccurs_ForEver)
		{
			// Nothing to add
		}
		else if (mRepeatSimpleItems->mOccursGroup->GetSelectedItem() == eOccurs_Count)
		{
			recur.SetUseCount(true);
			recur.SetCount(mRepeatSimpleItems->mOccursCounter->GetNumberValue());
		}
		else if (mRepeatSimpleItems->mOccursGroup->GetSelectedItem() == eOccurs_Until)
		{
			// NB the UNTIL value is always UTC, but we want to display it to the user relative to their start time,
			// so we must convert from dialog tzid to UTC
			
			// Get value from dialog
			iCal::CICalendarDateTime until;
			mRepeatSimpleItems->mOccursDateTimeZone->GetDateTimeZone(until, mTimingPanel->GetAllDay());
			
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

	mRepeats->SetActive(!read_only);
	mRepeatsTabs->SetActive(!read_only && mRepeats->IsChecked());
}

#pragma mark ______________________________CNewComponentRepeatSimple

void CNewComponentRepeatSimple::OnCreate()
{
// begin JXLayout2

    JXStaticText* obj1 =
        new JXStaticText("Every:", this,
                    JXWidget::kHElastic, JXWidget::kVElastic, 5,5, 47,20);
    assert( obj1 != NULL );

    mOccursInterval =
        new CNumberEdit(this,
                    JXWidget::kHElastic, JXWidget::kVElastic, 55,5, 70,20);
    assert( mOccursInterval != NULL );

    mOccursFreq =
        new HPopupMenu("",this,
                    JXWidget::kHElastic, JXWidget::kVElastic, 135,5, 110,20);
    assert( mOccursFreq != NULL );

    mOccursGroup =
        new JXRadioGroup(this,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 5,30, 80,85);
    assert( mOccursGroup != NULL );
    mOccursGroup->SetBorderWidth(0);

    mOccursForEver =
        new JXTextRadioButton(1, "For Ever", mOccursGroup,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 80,20);
    assert( mOccursForEver != NULL );

    mOccursCount =
        new JXTextRadioButton(2, "For:", mOccursGroup,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,30, 70,20);
    assert( mOccursCount != NULL );

    mOccursUntil =
        new JXTextRadioButton(3, "Until:", mOccursGroup,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,60, 70,20);
    assert( mOccursUntil != NULL );

    mOccursCounter =
        new CNumberEdit(this,
                    JXWidget::kHElastic, JXWidget::kVElastic, 75,60, 70,20);
    assert( mOccursCounter != NULL );

    JXStaticText* obj2 =
        new JXStaticText("time(s)", this,
                    JXWidget::kHElastic, JXWidget::kVElastic, 150,65, 47,20);
    assert( obj2 != NULL );

    mOccursDateTimeZone =
        new CDateTimeZoneSelect(this,
                    JXWidget::kHElastic, JXWidget::kVElastic, 75,85, 400,30);
    assert( mOccursDateTimeZone != NULL );

// end JXLayout2

	mOccursInterval->OnCreate(1, 0x7FFFFFFF);
	mOccursFreq->SetMenuItems("Year(s) %r | Month(s) %r | Week(s) %r | Day(s) %r | Hour(s) %r | Minute(s) %r | Second(s) %r");
	mOccursFreq->SetValue(1);
	mOccursCounter->OnCreate(1, 0x7FFFFFFF);
	mOccursDateTimeZone->OnCreate();
}

#pragma mark ______________________________CNewComponentRepeatAdvanced

void CNewComponentRepeatAdvanced::OnCreate()
{
// begin JXLayout3

    JXStaticText* obj1 =
        new JXStaticText("Description:", this,
                    JXWidget::kHElastic, JXWidget::kVElastic, 10,5, 80,20);
    assert( obj1 != NULL );

    mOccursDescription =
        new JXStaticText("", this,
                    JXWidget::kHElastic, JXWidget::kVElastic, 90,5, 380,40);
    assert( mOccursDescription != NULL );

    mOccursEdit =
        new JXTextButton("Edit...", this,
                    JXWidget::kHElastic, JXWidget::kVElastic, 368,56, 80,25);
    assert( mOccursEdit != NULL );

// end JXLayout3
}

#pragma mark ______________________________CNewComponentRepeatComplex

void CNewComponentRepeatComplex::OnCreate()
{
// begin JXLayout4

    JXStaticText* obj1 =
        new JXStaticText("Description:", this,
                    JXWidget::kHElastic, JXWidget::kVElastic, 10,5, 80,20);
    assert( obj1 != NULL );

    mOccursDescription =
        new JXStaticText("", this,
                    JXWidget::kHElastic, JXWidget::kVElastic, 90,5, 380,40);
    assert( mOccursDescription != NULL );

// end JXLayout4
}
