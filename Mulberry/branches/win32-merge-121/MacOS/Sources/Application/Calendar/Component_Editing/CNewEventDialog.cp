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

#include "CNewEventDialog.h"

#include "CBalloonDialog.h"
#include "CCalendarPopup.h"
#include "CCalendarView.h"
#include "CErrorDialog.h"
#include "CICalendar.h"
#include "CMulberryApp.h"
#include "CNewComponentAlarm.h"
#include "CNewComponentAttendees.h"
#include "CNewComponentDescription.h"
#include "CNewComponentTiming.h"
#include "CPreferences.h"
#include "CTextFieldX.h"
#include "CWindowsMenu.h"
#include "CXStringResources.h"

#include "CCalendarStoreManager.h"
#include "CCalendarStoreNode.h"
#include "CITIPProcessor.h"

#include <LCheckbox.h>
#include <LPopupButton.h>
#include <LTabsControl.h>
#include "MyCFString.h"

uint32_t CNewEventDialog::sTitleCounter = 0;
set<CNewEventDialog*> CNewEventDialog::sDialogs;

// ---------------------------------------------------------------------------
//	CNewEventDialog														  [public]
/**
	Default constructor */

CNewEventDialog::CNewEventDialog(LStream *inStream)
		: CModelessDialog(inStream)
{
	mCurrentPanel = 0;
	mReadOnly = false;
	sDialogs.insert(this);
}


// ---------------------------------------------------------------------------
//	~CNewEventDialog														  [public]
/**
	Destructor */

CNewEventDialog::~CNewEventDialog()
{
	sDialogs.erase(this);
}

#pragma mark -

void CNewEventDialog::FinishCreateSelf()
{
	CModelessDialog::FinishCreateSelf();

	// Get UI items
	mSummary = dynamic_cast<CTextFieldX*>(FindPaneByID(eSummary_ID));
	mSummary->AddListener(this);
	mCalendar = dynamic_cast<CCalendarPopup*>(FindPaneByID(eCalendar_ID));
	mStatus = dynamic_cast<LPopupButton*>(FindPaneByID(eStatus_ID));

	mTabs = dynamic_cast<LTabsControl*>(FindPaneByID(eTabs_ID));
	mTabView = dynamic_cast<LView*>(FindPaneByID(eTabView_ID));

	mOrganiserEdit = dynamic_cast<LCheckBox*>(FindPaneByID(eChangeOrganised_ID));

	// Listen to some of them
	UReanimator::LinkListenerToBroadcasters(this, this, pane_ID);
	
	// Init controls
	InitPanels();
	DoTab(1);
	mPanels[mCurrentPanel]->SetVisible(true);

	// Focus on summary
}

void CNewEventDialog::InitPanels()
{
	// Use tab group as default commander
	LCommander* defCommander;
	GetSubCommanders().FetchItemAt(1, defCommander);

	// Load each panel for the tabs
	mPanels.push_back(static_cast<CNewComponentTiming*>(UReanimator::CreateView(CNewComponentTiming::pane_ID, mTabView, defCommander)));
	mTabView->ExpandSubPane(mPanels.back(), true, true);
	mPanels.back()->SetVisible(false);

	mPanels.push_back(static_cast<CNewComponentDescription*>(UReanimator::CreateView(CNewComponentDescription::pane_ID, mTabView, defCommander)));
	mTabView->ExpandSubPane(mPanels.back(), true, true);
	mPanels.back()->SetVisible(false);

	mPanels.push_back(static_cast<CNewComponentAlarm*>(UReanimator::CreateView(CNewComponentAlarm::pane_ID, mTabView, defCommander)));
	mTabView->ExpandSubPane(mPanels.back(), true, true);
	mPanels.back()->SetVisible(false);

	mPanels.push_back(static_cast<CNewComponentAttendees*>(UReanimator::CreateView(CNewComponentAttendees::pane_ID, mTabView, defCommander)));
	mTabView->ExpandSubPane(mPanels.back(), true, true);
	mPanels.back()->SetVisible(false);
}

void CNewEventDialog::ListenTo_Message(long msg, void* param)
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
void CNewEventDialog::ListenToMessage(
	MessageT	inMessage,
	void		*ioParam)
{
	switch (inMessage)
	{
	case eTabs_ID:
		DoTab(mTabs->GetValue());
		break;

	case eSummary_ID:
		ChangedSummary();
		break;

	case eCalendar_ID:
		ChangedCalendar();
		break;

	case eChangeOrganised_ID:
		OnOrganiserEdit();
		break;

	default:
		CModelessDialog::ListenToMessage(inMessage, ioParam);
		break;
	}
}

void CNewEventDialog::DoTab(UInt32 value)
{
	// Only if different
	if (mCurrentPanel != value - 1)
	{
		mPanels[mCurrentPanel]->SetVisible(false);
		mCurrentPanel = value - 1;
		mPanels[mCurrentPanel]->SetVisible(true);
	}
}

bool CNewEventDialog::ContainsEvent(const iCal::CICalendarVEvent& vevent) const
{
	// Does this dialog contain a copy of this event
	return vevent.GetMapKey() == mVEvent->GetMapKey();
}

void CNewEventDialog::SetEvent(iCal::CICalendarVEvent& vevent)
{
	// Listen to original calendar as we need to cancel if calendar goes away
	iCal::CICalendarRef calref = vevent.GetCalendar();
	iCal::CICalendar* cal = iCal::CICalendar::GetICalendar(calref);
	if (cal)
		cal->Add_Listener(this);
	
	mVEvent = &vevent;

	// Set the relevant fields
	
	mSummary->SetText(vevent.GetSummary());

	mCalendar->SetCalendar(vevent.GetCalendar());
	ChangedCalendar();

	mStatus->SetValue(vevent.GetStatus() + 1);

	// Set in each panel
	for(CNewComponentPanelList::iterator iter = mPanels.begin(); iter != mPanels.end(); iter++)
	{
		(*iter)->SetEvent(vevent);
	}
	
	// Set title to summary if not empty
	if (!vevent.GetSummary().empty())
	{
		MyCFString cftitle(vevent.GetSummary(), kCFStringEncodingUTF8);
		SetCFDescriptor(cftitle);
	}

	// Determine read-only status based on organiser
	SetReadOnly((vevent.CountProperty(iCal::cICalProperty_ORGANIZER) != 0) && !iCal::CITIPProcessor::OrganiserIsMe(vevent));
	mOrganiserEdit->SetVisible(mReadOnly);
}

void CNewEventDialog::GetEvent(iCal::CICalendarVEvent& vevent)
{
	// Do descriptive items
	vevent.EditSummary(mSummary->GetText());
	
	vevent.EditStatus(static_cast<iCal::EStatus_VEvent>(mStatus->GetValue() - 1));

	// Get from each panel
	for(CNewComponentPanelList::iterator iter = mPanels.begin(); iter != mPanels.end(); iter++)
	{
		(*iter)->GetEvent(vevent);
	}
}

void CNewEventDialog::ChangedSummary()
{
	MyCFString cftitle(mSummary->GetText(), kCFStringEncodingUTF8);
	SetCFDescriptor(cftitle);
	CWindowsMenu::RenamedWindow();
}

void CNewEventDialog::ChangedCalendar()
{
	iCal::CICalendarRef newcal;
	mCalendar->GetCalendar(newcal);
	iCal::CICalendar* cal = iCal::CICalendar::GetICalendar(newcal);
	FindPaneByID(eOK_ID)->SetEnabled(!mReadOnly && (cal != NULL) && (!cal->IsReadOnly()));
}

void CNewEventDialog::SetReadOnly(bool read_only)
{
	mReadOnly = read_only;

	// This will reset state of OK button
	ChangedCalendar();

	mSummary->SetReadOnly(mReadOnly);
	mCalendar->SetEnabled(!mReadOnly);
	mStatus->SetEnabled(!mReadOnly);

	// Set in each panel
	for(CNewComponentPanelList::iterator iter = mPanels.begin(); iter != mPanels.end(); iter++)
	{
		(*iter)->SetReadOnly(mReadOnly);
	}
}

cdstring CNewEventDialog::GetCurrentSummary() const
{
	return mSummary->GetText();
}

void CNewEventDialog::OnOK()
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

void CNewEventDialog::OnCancel()
{
	DoCancel();
	
	// Now do inherited
	CModelessDialog::OnCancel();
}

void CNewEventDialog::OnOrganiserEdit()
{
	SetReadOnly(mOrganiserEdit->GetValue() == 0);
}

bool CNewEventDialog::DoNewOK()
{
	// Check and get new calendar if different
	iCal::CICalendarRef newcal;
	iCal::CICalendar* new_cal = NULL;
	if (!GetCalendar(0, newcal, new_cal))
		// Return to dialog
		return false;

	// Get updated info
	GetEvent(*mVEvent);
	
	// Look for change to calendar
	if (newcal != mVEvent->GetCalendar())
	{
		// Use new calendar
		mVEvent->SetCalendar(newcal);
		
		// Set the default calendar for next time
		const calstore::CCalendarStoreNode* node = calstore::CCalendarStoreManager::sCalendarStoreManager->GetNode(new_cal);
		if (node != NULL)
			CPreferences::sPrefs->mDefaultCalendar.SetValue(node->GetAccountName());
	}

	// Add to calendar (this will do the display update)
	new_cal->AddNewVEvent(mVEvent);
	CCalendarView::EventChangedAll(mVEvent);
	
	return true;
}

bool CNewEventDialog::DoEditOK()
{
	// Find the original calendar if it still exists
	iCal::CICalendarRef oldcal = mVEvent->GetCalendar();
	iCal::CICalendar* old_cal = iCal::CICalendar::GetICalendar(oldcal);
	if (old_cal == NULL)
	{
		// Inform user of missing calendar
		CErrorDialog::StopAlert(rsrc::GetString("CNewEventDialog::MissingOriginal"));
		
		// Disable OK button
		FindPaneByID(eOK_ID)->Disable();
		
		return false;
	}
	
	// Find the original event if it still exists
	iCal::CICalendarVEvent*	original = static_cast<iCal::CICalendarVEvent*>(old_cal->FindComponent(mVEvent));
	if (original == NULL)
	{
		// Inform user of missing calendar
		CErrorDialog::StopAlert(rsrc::GetString("CNewEventDialog::MissingOriginal"));
		
		// Disable OK button and return to dialog
		FindPaneByID(eOK_ID)->Disable();
		return false;
	}

	// Check and get new calendar if different
	iCal::CICalendarRef newcal;
	iCal::CICalendar* new_cal = NULL;
	if (!GetCalendar(oldcal, newcal, new_cal))
		// Return to dialog
		return false;

	// Get updated info into original event
	GetEvent(*original);

	// Do calendar change
	if (new_cal != NULL)
	{
		// Remove from old calendar (without deleting)
		old_cal->RemoveVEvent(original, false);
		
		// Add to new calendar (without initialising)
		original->SetCalendar(newcal);
		new_cal->AddNewVEvent(original, true);
	}
	
	// Tell it it has changed (i.e. bump sequence, dirty calendar)
	original->Changed();

	CCalendarView::EventChangedAll(mVEvent);
	
	return true;
}

void CNewEventDialog::DoCancel()
{
	// Delete the event which we own and is not going to be used
	delete mVEvent;
	mVEvent = NULL;
}

bool CNewEventDialog::GetCalendar(iCal::CICalendarRef oldcal, iCal::CICalendarRef& newcal, iCal::CICalendar*& new_cal)
{
	mCalendar->GetCalendar(newcal);
	if ((oldcal == 0) || (newcal != oldcal))
	{
		new_cal = iCal::CICalendar::GetICalendar(newcal);
		if (new_cal == NULL)
		{
			// Inform user of missing calendar
			CErrorDialog::StopAlert(rsrc::GetString("CNewEventDialog::MissingNewCalendar"));
			
			// Force calendar popup reset and return to dialog
			mCalendar->Reset();
			mCalendar->SetCalendar(oldcal);
			return false;
		}
	}
	
	return true;
}

void CNewEventDialog::StartNew(const iCal::CICalendarDateTime& dtstart, const iCal::CICalendar* calin)
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

	// Start with an empty new event
	auto_ptr<iCal::CICalendarVEvent> vevent(static_cast<iCal::CICalendarVEvent*>(iCal::CICalendarVEvent::Create(cal->GetRef())));
	
	// Duration is one hour
	iCal::CICalendarDuration duration(60 * 60);
	
	// Set event with initial timing
	vevent->EditTiming(dtstart, duration);

	StartModeless(*vevent.release(), CNewEventDialog::eNew);
}

void CNewEventDialog::StartEdit(const iCal::CICalendarVEvent& original)
{
	// Look for an existinf dialog for this event
	for(set<CNewEventDialog*>::const_iterator iter = sDialogs.begin(); iter != sDialogs.end(); iter++)
	{
		if ((*iter)->ContainsEvent(original))
		{
			(*iter)->Select();
			return;
		}
	}

	// Use a copy of the event
	iCal::CICalendarVEvent* vevent = new iCal::CICalendarVEvent(original);
	
	StartModeless(*vevent, CNewEventDialog::eEdit);
}

void CNewEventDialog::StartDuplicate(const iCal::CICalendarVEvent& original)
{
	// Start with an empty new event
	iCal::CICalendarVEvent* vevent = new iCal::CICalendarVEvent(original);
	vevent->Duplicated();
	
	StartModeless(*vevent, CNewEventDialog::eDuplicate);
}

void CNewEventDialog::StartModeless(iCal::CICalendarVEvent& vevent, EModelessAction action)
{
	CNewEventDialog* dlog = static_cast<CNewEventDialog*>(LWindow::CreateWindow(pane_ID, CMulberryApp::sApp));
	dlog->SetAction(action);
	dlog->SetEvent(vevent);
	dlog->Show();
}
