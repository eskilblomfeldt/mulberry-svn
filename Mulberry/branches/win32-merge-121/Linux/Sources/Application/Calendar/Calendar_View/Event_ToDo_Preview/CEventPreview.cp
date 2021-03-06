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


// Source for CEventPreview class

#include "CEventPreview.h"

#include "CNewEventDialog.h"
#include "CNewToDoDialog.h"
#include "CTextDisplay.h"
#include "CXStringResources.h"

#include "cdstring.h"

#include "CCalendarStoreManager.h"
#include "CICalendarComponentRecur.h"
#include "CICalendarLocale.h"
#include "CICalendarVEvent.h"
#include "CICalendarVToDo.h"

#include <strstream.h>

#include <JXTextButton.h>

#include <cassert>

// Static members

// C O N S T R U C T I O N / D E S T R U C T I O N  M E T H O D S

// Default constructor
CEventPreview::CEventPreview(JXContainer* enclosure,
								const HSizingOption hSizing, const VSizingOption vSizing,
								const JCoordinate x, const JCoordinate y,
								const JCoordinate w, const JCoordinate h)
		: CBaseView(enclosure, hSizing, vSizing, x, y, w, h)
{
}

// Default destructor
CEventPreview::~CEventPreview()
{
}

// O T H E R  M E T H O D S ____________________________________________________________________________

// Get details of sub-panes
void CEventPreview::OnCreate(void)
{
	// Do inherited
	CBaseView::OnCreate();

// begin JXLayout1

    mDetails =
        new CTextDisplay(this,
                    JXWidget::kHElastic, JXWidget::kVElastic, 5,5, 490,260);
    assert( mDetails != NULL );

    mEditBtn =
        new JXTextButton("Edit", this,
                    JXWidget::kFixedRight, JXWidget::kFixedBottom, 435,270, 50,20);
    assert( mEditBtn != NULL );

// end JXLayout1

	mEditBtn->SetActive(kFalse);
	
	// Listen to buttons
	ListenTo(mEditBtn);
}

// Respond to clicks in the icon buttons
void CEventPreview::Receive(JBroadcaster* sender, const Message& message)
{
	if (message.Is(JXButton::kPushed))
	{
		if (sender == mEditBtn)
		{
			// Edit item
			Edit();
			return;
		}
	}
	
	CBaseView::Receive(sender, message);
}

bool CEventPreview::HasFocus() const
{
	// Switch target to the text (will focus)
	return mDetails->IsTarget();;
}

void CEventPreview::Focus()
{
	mDetails->Focus();
}

void CEventPreview::SetComponent(const iCal::CICalendarComponentExpandedShared& comp)
{
	if (mComp.get() != comp.get())
	{
		if (comp.get() != NULL)
		{
			mEditBtn->SetActive(kTrue);
			mDetails->SetActive(kTrue);
			
			mSummary = comp->GetOwner()->GetSummary();
			
			// Determine type
			switch(comp->GetOwner()->GetType())
			{
			case iCal::CICalendarComponent::eVEVENT:
				SetEvent(comp);
				break;
			case iCal::CICalendarComponent::eVTODO:
				SetToDo(comp);
				break;
			default:;
			}
		}
		else
		{
			mEditBtn->SetActive(kFalse);
			mDetails->SetActive(kFalse);
			
			mSummary = cdstring::null_str;
			
			mDetails->SetText(cdstring::null_str);
		}

		mComp = comp;

		// Broadcast change to listeners
		Broadcast_Message(eBroadcast_ViewChanged, this);
	}
}

void CEventPreview::SetEvent(const iCal::CICalendarComponentExpandedShared& comp)
{
	// Setup a help tag;
	ostrstream ostr;
	ostr << rsrc::GetString("EventTip::Summary") << comp->GetMaster<iCal::CICalendarVEvent>()->GetSummary() << endl;
	if (comp->GetInstanceStart().IsDateOnly())
	{
		ostr << rsrc::GetString("EventTip::All Day Event") << endl;
	}
	else
	{
		ostr << rsrc::GetString("EventTip::Starts on") << comp->GetInstanceStart().GetAdjustedTime().GetTime(false, !iCal::CICalendarLocale::Use24HourTime()) << endl;
		ostr << rsrc::GetString("EventTip::Ends on") << comp->GetInstanceEnd().GetAdjustedTime().GetTime(false, !iCal::CICalendarLocale::Use24HourTime()) << endl;
	}
	if (!comp->GetMaster<iCal::CICalendarVEvent>()->GetLocation().empty())
		ostr << rsrc::GetString("EventTip::Location") << comp->GetMaster<iCal::CICalendarVEvent>()->GetLocation() << endl;
	if (!comp->GetMaster<iCal::CICalendarVEvent>()->GetDescription().empty())
		ostr << rsrc::GetString("EventTip::Description") << comp->GetMaster<iCal::CICalendarVEvent>()->GetDescription() << endl;
	
	// Add calendar name if more than one calendar in use
	const iCal::CICalendarList& cals = calstore::CCalendarStoreManager::sCalendarStoreManager->GetActiveCalendars();
	if (cals.size() > 1)
	{
		iCal::CICalendar* cal = iCal::CICalendar::GetICalendar(comp->GetMaster<iCal::CICalendarVEvent>()->GetCalendar());
		if (cal != NULL)
			ostr << endl << rsrc::GetString("EventTip::Calendar") << cal->GetName() << endl;
	}

	ostr << ends;
	cdstring temp;
	temp.steal(ostr.str());
	
	mDetails->SetText(temp);
}

void CEventPreview::SetToDo(const iCal::CICalendarComponentExpandedShared& comp)
{
	// Setup a help tag;
	ostrstream ostr;
	ostr << rsrc::GetString("ToDoTip::Summary") << comp->GetMaster<iCal::CICalendarVToDo>()->GetSummary() << endl;

	if (!comp->GetMaster<iCal::CICalendarVToDo>()->HasEnd())
	{
		ostr << rsrc::GetString("ToDoTip::No due date") << endl;
	}
	else
	{
		ostr << rsrc::GetString("ToDoTip::Due on") << comp->GetInstanceEnd().GetAdjustedTime().GetLocaleDate(iCal::CICalendarDateTime::eAbbrevDate) << endl;
	}

	// Add calendar name if more than one calendar in use
	const iCal::CICalendarList& cals = calstore::CCalendarStoreManager::sCalendarStoreManager->GetActiveCalendars();
	if (cals.size() > 1)
	{
		iCal::CICalendar* cal = iCal::CICalendar::GetICalendar( comp->GetMaster<iCal::CICalendarVToDo>()->GetCalendar());
		if (cal != NULL)
			ostr << endl << rsrc::GetString("ToDoTip::Calendar") << cal->GetName() << endl;
	}

	ostr << ends;
	cdstring temp;
	temp.steal(ostr.str());
	
	mDetails->SetText(temp);
}

bool CEventPreview::HasData() const
{
	return (mComp.get() != NULL);
}

void CEventPreview::Edit()
{
	// Determine type
	switch(mComp->GetOwner()->GetType())
	{
	case iCal::CICalendarComponent::eVEVENT:
		CNewEventDialog::StartEdit(*mComp->GetMaster<iCal::CICalendarVEvent>());
		break;
	case iCal::CICalendarComponent::eVTODO:
		CNewToDoDialog::StartEdit(*mComp->GetMaster<iCal::CICalendarVToDo>());
		break;
	default:;
	}
}
