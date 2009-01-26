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

#include "CNewToDoDialog.h"

#include "CICalendar.h"
#include "CICalendarManager.h"

#include "CCalendarPopup.h"
#include "CCalendarView.h"
#include "CDateTimeZoneSelect.h"
#include "CErrorDialog.h"
#include "CMulberryApp.h"
#include "CNewComponentAlarm.h"
#include "CNewComponentAttendees.h"
#include "CNewComponentDescription.h"
#include "CNewComponentTiming.h"
#include "CPreferences.h"
#include "CTabController.h"
#include "CTextField.h"
#include "CWindowsMenu.h"
#include "CXStringResources.h"

#include "CCalendarStoreManager.h"
#include "CICalendarLocale.h"
#include "CITIPProcessor.h"

#include "TPopupMenu.h"

#include <JXStaticText.h>
#include <JXTextButton.h>
#include <JXTextCheckbox.h>
#include <JXUpRect.h>
#include <JXWindow.h>

#include <jXGlobals.h>

#include <cassert>

uint32_t CNewToDoDialog::sTitleCounter = 0;
set<CNewToDoDialog*> CNewToDoDialog::sDialogs;

// ---------------------------------------------------------------------------
//	CNewToDoDialog														  [public]
/**
	Default constructor */

CNewToDoDialog::CNewToDoDialog(JXDirector* supervisor)
		: CModelessDialog(supervisor)
{
	mCurrentPanel = 0;
	sDialogs.insert(this);

	CWindowsMenu::AddWindow(this, false);
}


// ---------------------------------------------------------------------------
//	~CNewToDoDialog														  [public]
/**
	Destructor */

CNewToDoDialog::~CNewToDoDialog()
{
	sDialogs.erase(this);
	CWindowsMenu::RemoveWindow(this);
}

#pragma mark -

void CNewToDoDialog::OnCreate()
{
	// Get UI items
// begin JXLayout

    JXWindow* window = new JXWindow(this, 540,450, "");
    assert( window != NULL );
    SetWindow(window);

    JXUpRect* obj1 =
        new JXUpRect(window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 540,450);
    assert( obj1 != NULL );

    JXStaticText* obj2 =
        new JXStaticText("Summary:", obj1,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 70,20);
    assert( obj2 != NULL );

    mSummary =
        new CTextInputField(obj1,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 80,10, 200,20);
    assert( mSummary != NULL );

    mCalendar =
        new CCalendarPopup("",obj1,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 300,10, 200,20);
    assert( mCalendar != NULL );

    mCompleted =
        new JXTextCheckbox("Completed", obj1,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 5,40, 90,20);
    assert( mCompleted != NULL );

    mCompletedDateTimeZone =
        new CDateTimeZoneSelect(obj1,
                    JXWidget::kHElastic, JXWidget::kVElastic, 94,35, 400,30);
    assert( mCompletedDateTimeZone != NULL );

    mCompletedNow =
        new JXTextButton("Now", obj1,
                    JXWidget::kFixedRight, JXWidget::kFixedBottom, 495,40, 35,20);
    assert( mCompletedNow != NULL );
    mCompletedNow->SetFontSize(10);

    mTabs =
        new CTabController(obj1,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,70, 520,330);
    assert( mTabs != NULL );

    mOrganiserEdit =
        new JXTextCheckbox("Allow Changes to Organised Event", obj1,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 10,410, 235,20);
    assert( mOrganiserEdit != NULL );

    mCancelBtn =
        new JXTextButton("Cancel", obj1,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 310,410, 85,25);
    assert( mCancelBtn != NULL );
    mCancelBtn->SetShortcuts("]");

    mOKBtn =
        new JXTextButton("OK", obj1,
                    JXWidget::kFixedRight, JXWidget::kFixedBottom, 425,410, 85,25);
    assert( mOKBtn != NULL );
    mOKBtn->SetShortcuts("^M");

// end JXLayout

	window->SetTitle("New Task");
	SetButtons(mOKBtn, mCancelBtn);

	CModelessDialog::OnCreate();
	
	mCalendar->OnCreate();
	mCompletedDateTimeZone->OnCreate();
	mCompletedDateTimeZone->SetActive(false);
	mCompletedDateTimeZone->SetVisible(false);
	mCompletedNow->SetVisible(false);

	ListenTo(mSummary);
	ListenTo(mCalendar);
	ListenTo(mCompleted);
	ListenTo(mCompletedNow);
	ListenTo(mTabs);
	ListenTo(mOrganiserEdit);

	// Init controls
	InitPanels();
	DoTab(1);

	// Focus on summary
}

void CNewToDoDialog::InitPanels()
{
	// Load each panel for the tabs
	mPanels.push_back(new CNewComponentTiming(mTabs->GetCardEnclosure(), JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 500,300));
	mTabs->AppendCard(mPanels.back(), "Timing");

	mPanels.push_back(new CNewComponentDescription(mTabs->GetCardEnclosure(), JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 500,300));
	mTabs->AppendCard(mPanels.back(), "Details");

	mPanels.push_back(new CNewComponentAlarm(mTabs->GetCardEnclosure(), JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 500,300));
	mTabs->AppendCard(mPanels.back(), "Alarms");

	mPanels.push_back(new CNewComponentAttendees(mTabs->GetCardEnclosure(), JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 500,300));
	mTabs->AppendCard(mPanels.back(), "Attendees");
}

void CNewToDoDialog::ListenTo_Message(long msg, void* param)
{
	switch(msg)
	{
	case iCal::CICalendar::eBroadcast_Closed:
		// Force dialog to close immediately as event is about to be deleted.
		// Any changes so far will be lost.
		OnCancel();
		break;
	default:;
	}
}

// Handle controls
void CNewToDoDialog::Receive(JBroadcaster* sender, const Message& message)
{
	if (message.Is(JTextEditor16::kTextChanged))
	{
		if (sender == mSummary)
		{
			ChangedSummary();
			return;
		}
	}
	else if(message.Is(JXMenu::kItemSelected))
	{
		const JXMenu::ItemSelected* is = dynamic_cast<const JXMenu::ItemSelected*>(&message);
		if (sender == mCalendar)
		{
			ChangedCalendar();
			return;
		}
	}
	else if (message.Is(JXTextButton::kPushed))
	{
		if (sender == mCompletedNow)
		{
			DoNow();
			return;
		}
	}
	else if (message.Is(JXRadioGroup::kSelectionChanged))
	{
		JIndex index = dynamic_cast<const JXRadioGroup::SelectionChanged*>(&message)->GetID();
		if (sender == mTabs)
		{
			DoTab(mTabs->GetSelectedItem());
			return;
		}
	}
	else if (message.Is(JXCheckbox::kPushed))
	{
		if (sender == mCompleted)
		{
			DoCompleted(mCompleted->IsChecked());
			return;
		}
		else if (sender == mOrganiserEdit)
		{
			OnOrganiserEdit();
			return;
		}
	}

	CModelessDialog::Receive(sender, message);
}

void CNewToDoDialog::DoTab(JIndex value)
{
	// Only if different
	if (mCurrentPanel != value - 1)
	{
		mCurrentPanel = value - 1;
	}
}

void CNewToDoDialog::DoCompleted(bool set)
{
	if (set && !mCompletedExists)
	{
		mActualCompleted.SetTimezone(iCal::CICalendarManager::sICalendarManager->GetDefaultTimezone());
		mActualCompleted.SetNow();
		mCompletedExists = true;
		mCompletedDateTimeZone->SetDateTimeZone(mActualCompleted, false);
	}

	mCompletedDateTimeZone->SetVisible(set);
	mCompletedNow->SetVisible(set);
}

void CNewToDoDialog::DoNow()
{
	mActualCompleted.SetTimezone(iCal::CICalendarManager::sICalendarManager->GetDefaultTimezone());
	mActualCompleted.SetNow();
	mCompletedDateTimeZone->SetDateTimeZone(mActualCompleted, false);
}

bool CNewToDoDialog::ContainsToDo(const iCal::CICalendarVToDo& vtodo) const
{
	// Does this dialog contain a copy of this event
	return vtodo.GetMapKey() == mVToDo->GetMapKey();
}

void CNewToDoDialog::SetToDo(iCal::CICalendarVToDo& vtodo)
{
	// Listen to original calendar as we need to cancel if calendar goes away
	iCal::CICalendarRef calref = vtodo.GetCalendar();
	iCal::CICalendar* cal = iCal::CICalendar::GetICalendar(calref);
	if (cal)
		cal->Add_Listener(this);
	
	mVToDo = &vtodo;

	// Set the relevant fields
	
	mSummary->SetText(vtodo.GetSummary());

	mCalendar->SetCalendar(vtodo.GetCalendar());
	ChangedCalendar();

	mCompleted->SetState(vtodo.GetStatus() == iCal::eStatus_VToDo_Completed);

	mCompletedExists = vtodo.HasCompleted();
	if (mCompletedExists)
	{
		// COMPLETED is in UTC but we adjust to local timezone
		mActualCompleted = vtodo.GetCompleted();
		mActualCompleted.AdjustTimezone(iCal::CICalendarManager::sICalendarManager->GetDefaultTimezone());
	
		mCompletedDateTimeZone->SetDateTimeZone(mActualCompleted, false);
	}
	DoCompleted(vtodo.GetStatus() == iCal::eStatus_VToDo_Completed);

	// Set in each panel
	for(CNewComponentPanelList::iterator iter = mPanels.begin(); iter != mPanels.end(); iter++)
	{
		(*iter)->SetToDo(vtodo);
	}
	
	// Set title to summary if not empty
	if (!vtodo.GetSummary().empty())
	{
		GetWindow()->SetTitle(vtodo.GetSummary());
	}

	// Determine read-only status based on organiser
	SetReadOnly((vtodo.CountProperty(iCal::cICalProperty_ORGANIZER) != 0) && !iCal::CITIPProcessor::OrganiserIsMe(vtodo));
	mOrganiserEdit->SetVisible(mReadOnly);
}

void CNewToDoDialog::GetToDo(iCal::CICalendarVToDo& vtodo)
{
	// Do descriptive items
	vtodo.EditSummary(mSummary->GetText());

	vtodo.EditStatus(mCompleted->IsChecked() ? iCal::eStatus_VToDo_Completed : iCal::eStatus_VToDo_NeedsAction);
	
	// Changed completed date if needed
	mCompletedDateTimeZone->GetDateTimeZone(mActualCompleted, false);
	if (mCompleted->IsChecked() && (vtodo.GetCompleted() != mActualCompleted))
	{
		// Adjust to UTC and then change
		mActualCompleted.AdjustToUTC();
		vtodo.EditCompleted(mActualCompleted);
	}

	// Get from each panel
	for(CNewComponentPanelList::iterator iter = mPanels.begin(); iter != mPanels.end(); iter++)
	{
		(*iter)->GetToDo(vtodo);
	}
}

void CNewToDoDialog::ChangedSummary()
{
	GetWindow()->SetTitle(mSummary->GetText());
	CWindowsMenu::RenamedWindow();
}

void CNewToDoDialog::ChangedCalendar()
{
	iCal::CICalendarRef newcal;
	mCalendar->GetCalendar(newcal);
	iCal::CICalendar* cal = iCal::CICalendar::GetICalendar(newcal);
	mOKBtn->SetActive(!mReadOnly && (cal != NULL) && (!cal->IsReadOnly()));
}

void CNewToDoDialog::SetReadOnly(bool read_only)
{
	mReadOnly = read_only;

	// This will reset state of OK button
	ChangedCalendar();

	mSummary->SetReadOnly(mReadOnly);
	mCalendar->SetActive(!mReadOnly);
	mCompleted->SetActive(!mReadOnly);
	mCompletedDateTimeZone->SetActive(!mReadOnly);
	mCompletedNow->SetActive(!mReadOnly);

	// Set in each panel
	for(CNewComponentPanelList::iterator iter = mPanels.begin(); iter != mPanels.end(); iter++)
	{
		(*iter)->SetReadOnly(mReadOnly);
	}
}

cdstring CNewToDoDialog::GetCurrentSummary() const
{
	return mSummary->GetText();
}

void CNewToDoDialog::OnOK()
{
	bool result = true;
	switch(mAction)
	{
	case eNew:
	case eDuplicate:
		result = DoNewOK();
		break;
	case eEdit:
		result = DoEditOK();
		break;
	default:;
	}
	
	// Now do inherited if result was OK
	if (result)
		CModelessDialog::OnOK();
}

void CNewToDoDialog::OnCancel()
{
	DoCancel();
	
	// Now do inherited
	CModelessDialog::OnCancel();
}

void CNewToDoDialog::OnOrganiserEdit()
{
	SetReadOnly(!mOrganiserEdit->IsChecked());
}

bool CNewToDoDialog::DoNewOK()
{
	// Check and get new calendar if different
	iCal::CICalendarRef newcal;
	iCal::CICalendar* new_cal = NULL;
	if (!GetCalendar(0, newcal, new_cal))
		// Return to dialog
		return false;

	// Get updated info
	GetToDo(*mVToDo);
	
	// Look for change to calendar
	if (newcal != mVToDo->GetCalendar())
	{
		// Use new calendar
		mVToDo->SetCalendar(newcal);
		
		// Set the default calendar for next time
		const calstore::CCalendarStoreNode* node = calstore::CCalendarStoreManager::sCalendarStoreManager->GetNode(new_cal);
		if (node != NULL)
			CPreferences::sPrefs->mDefaultCalendar.SetValue(node->GetAccountName());
	}

	// Add to calendar (this will do the display update)
	iCal::CICalendar* cal = iCal::CICalendar::GetICalendar(mVToDo->GetCalendar());
	new_cal->AddNewVToDo(mVToDo);
	CCalendarView::ToDosChangedAll();
	
	return true;
}

bool CNewToDoDialog::DoEditOK()
{
	// Find the original calendar if it still exists
	iCal::CICalendarRef oldcal = mVToDo->GetCalendar();
	iCal::CICalendar* old_cal = iCal::CICalendar::GetICalendar(oldcal);
	if (old_cal == NULL)
	{
		// Inform user of missing calendar
		CErrorDialog::StopAlert(rsrc::GetString("CNewToDoDialog::MissingOriginal"));
		
		// Disable OK button
		mOKBtn->SetActive(false);
		
		return false;
	}
	
	// Find the original to do if it still exists
	iCal::CICalendarVToDo*	original = static_cast<iCal::CICalendarVToDo*>(old_cal->FindComponent(mVToDo));
	if (original == NULL)
	{
		// Inform user of missing calendar
		CErrorDialog::StopAlert(rsrc::GetString("CNewToDoDialog::MissingOriginal"));
		
		// Disable OK button and return to dialog
		mOKBtn->SetActive(false);
		return false;
	}

	// Check and get new calendar if different
	iCal::CICalendarRef newcal;
	iCal::CICalendar* new_cal = NULL;
	if (!GetCalendar(oldcal, newcal, new_cal))
		// Return to dialog
		return false;

	// Get updated info
	GetToDo(*original);
	
	// Look for change to calendar
	if (new_cal != NULL)
	{
		// Remove from old calendar (without deleting)
		old_cal->RemoveVToDo(original, false);
		
		// Add to new calendar (without initialising)
		original->SetCalendar(newcal);
		new_cal->AddNewVToDo(original, true);
	}
	
	// Tell it it has changed (i.e. bump sequence, dirty calendar)
	original->Changed();

	CCalendarView::ToDosChangedAll();
	
	return true;
}

void CNewToDoDialog::DoCancel()
{
	// Delete the to do which we own and is not going to be used
	delete mVToDo;
	mVToDo = NULL;
}

bool CNewToDoDialog::GetCalendar(iCal::CICalendarRef oldcal, iCal::CICalendarRef& newcal, iCal::CICalendar*& new_cal)
{
	mCalendar->GetCalendar(newcal);
	if ((oldcal == 0) || (newcal != oldcal))
	{
		new_cal = iCal::CICalendar::GetICalendar(newcal);
		if (new_cal == NULL)
		{
			// Inform user of missing calendar
			CErrorDialog::StopAlert(rsrc::GetString("CNewToDoDialog::MissingNewCalendar"));
			
			// Force calendar popup reset and return to dialog
			mCalendar->Reset();
			mCalendar->SetCalendar(oldcal);
			return false;
		}
	}
	
	return true;
}

void CNewToDoDialog::StartNew(const iCal::CICalendar* calin)
{
	const iCal::CICalendarList& cals = calstore::CCalendarStoreManager::sCalendarStoreManager->GetActiveCalendars();
	if (cals.size() == 0)
		return;

	const iCal::CICalendar* cal = calin;
	if (cal == NULL)
	{
		// Default calendar is the first one
		cal = cals.front();

		// Check prefs for default calendar
		const calstore::CCalendarStoreNode* node = calstore::CCalendarStoreManager::sCalendarStoreManager->GetNode(CPreferences::sPrefs->mDefaultCalendar.GetValue());
		if ((node != NULL) && (node->GetCalendar() != NULL))
			cal = node->GetCalendar();
	}

	// Start with an empty to do
	iCal::CICalendarVToDo* vtodo = static_cast<iCal::CICalendarVToDo*>(iCal::CICalendarVToDo::Create(cal->GetRef()));
	
	// Set event with initial timing
	vtodo->EditTiming();

	StartModeless(*vtodo, CNewToDoDialog::eNew);
}

void CNewToDoDialog::StartEdit(const iCal::CICalendarVToDo& original)
{
	// Look for an existinf dialog for this event
	for(set<CNewToDoDialog*>::const_iterator iter = sDialogs.begin(); iter != sDialogs.end(); iter++)
	{
		if ((*iter)->ContainsToDo(original))
		{
			(*iter)->GetWindow()->Raise();
			return;
		}
	}

	// Use a copy of the event
	iCal::CICalendarVToDo* vtodo = new iCal::CICalendarVToDo(original);
	
	StartModeless(*vtodo, CNewToDoDialog::eEdit);
}

void CNewToDoDialog::StartDuplicate(const iCal::CICalendarVToDo& original)
{
	// Start with an empty new event
	iCal::CICalendarVToDo* vtodo = new iCal::CICalendarVToDo(original);
	vtodo->Duplicated();
	
	StartModeless(*vtodo, CNewToDoDialog::eDuplicate);
}

void CNewToDoDialog::StartModeless(iCal::CICalendarVToDo& vtodo, EModelessAction action)
{
	CNewToDoDialog* dlog = new CNewToDoDialog(JXGetApplication());
	dlog->OnCreate();
	dlog->SetAction(action);
	dlog->SetToDo(vtodo);
	dlog->Activate();
}
