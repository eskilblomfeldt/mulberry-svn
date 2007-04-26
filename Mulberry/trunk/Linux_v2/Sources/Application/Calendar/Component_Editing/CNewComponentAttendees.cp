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

#include "CNewComponentAttendees.h"

#include "CAddress.h"
#include "CAttendeeTable.h"
#include "CErrorHandler.h"
#include "CICalendarCalAddressValue.h"
#include "CIdentity.h"
#include "CIdentityPopup.h"
#include "CNewAttendeeDialog.h"
#include "CPreferences.h"
#include "CScrollbarSet.h"
#include "CSimpleTitleTable.h"
#include "CURL.h"

#include <JXFlatRect.h>
#include <JXStaticText.h>
#include <JXTextButton.h>

#include <cassert>

// ---------------------------------------------------------------------------
//	CNewComponentAttendees														  [public]
/**
	Default constructor */

CNewComponentAttendees::CNewComponentAttendees(JXContainer* enclosure,
						const HSizingOption hSizing, const VSizingOption vSizing,
						const JCoordinate x, const JCoordinate y,
						const JCoordinate w, const JCoordinate h)
	: CNewComponentPanel(enclosure, hSizing, vSizing, x, y, w, h)
{
	mHasOrganizer = false;
	mOrganizerIsMe = false;
	mOrganizerChanged = false;
	mUseIdentity = false;
	mOldIdentity = NULL;
}

#pragma mark -

const int cTitleHeight = 16;

void CNewComponentAttendees::OnCreate()
{
	// Get UI items
// begin JXLayout1

    mAddAttendeesPane =
        new JXFlatRect(this,
                    JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 500,300);
    assert( mAddAttendeesPane != NULL );

    mAddAttendees =
        new JXTextButton("Add Attendees...", mAddAttendeesPane,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 55,50, 140,26);
    assert( mAddAttendees != NULL );

// end JXLayout1
// begin JXLayout2

    mAttendeesPane =
        new JXFlatRect(this,
                    JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 500,300);
    assert( mAttendeesPane != NULL );

    mOrganiser =
        new JXStaticText("", mAttendeesPane,
                    JXWidget::kHElastic, JXWidget::kVElastic, 95,10, 390,20);
    assert( mOrganiser != NULL );

    JXStaticText* obj1 =
        new JXStaticText("Organiser:", mAttendeesPane,
                    JXWidget::kHElastic, JXWidget::kVElastic, 15,10, 65,20);
    assert( obj1 != NULL );

    mIdentity =
        new CIdentityPopup("",mAttendeesPane,
                    JXWidget::kHElastic, JXWidget::kVElastic, 90,10, 200,20);
    assert( mIdentity != NULL );

    CScrollbarSet* sbs =
        new CScrollbarSet(mAttendeesPane,
                    JXWidget::kHElastic, JXWidget::kVElastic, 10,40, 480,210);
    assert( sbs != NULL );

    mNewBtn =
        new JXTextButton("New...", mAttendeesPane,
                    JXWidget::kHElastic, JXWidget::kFixedBottom, 30,265, 90,25);
    assert( mNewBtn != NULL );

    mChangeBtn =
        new JXTextButton("Change...", mAttendeesPane,
                    JXWidget::kHElastic, JXWidget::kVElastic, 140,265, 90,25);
    assert( mChangeBtn != NULL );

    mDeleteBtn =
        new JXTextButton("Delete", mAttendeesPane,
                    JXWidget::kHElastic, JXWidget::kFixedBottom, 250,265, 90,25);
    assert( mDeleteBtn != NULL );

// end JXLayout2

	mTitles = new CSimpleTitleTable(sbs, sbs->GetScrollEnclosure(),
																	 JXWidget::kHElastic,
																	 JXWidget::kFixedTop,
																	 0, 0, 550, cTitleHeight);

	mTable = new CAttendeeTable(sbs,sbs->GetScrollEnclosure(),
										JXWidget::kHElastic,
										JXWidget::kVElastic,
										0,cTitleHeight, 550, 100);

	mTable->OnCreate();
	mTitles->OnCreate();

	mTitles->LoadTitles("UI::Titles::CalendarAttendees", 3);
	mTitles->SyncTable(mTable, true);
	
	mTable->Add_Listener(this);

	CPreferences* prefs = CPreferences::sPrefs;
	const CIdentityList& list = CPreferences::sPrefs->mIdentities.GetValue();
	if (!list.empty())
	mIdentity->Reset(CPreferences::sPrefs->mIdentities.GetValue());

	mAttendeesPane->SetVisible(false);
	OnSelectionChange();

	// Listen to some controls
	ListenTo(mAddAttendees);
	ListenTo(mIdentity);
	ListenTo(mNewBtn);
	ListenTo(mChangeBtn);
	ListenTo(mDeleteBtn);
}

// Respond to clicks in the icon buttons
void CNewComponentAttendees::Receive(JBroadcaster* sender, const Message& message)
{
	if (message.Is(JXMenu::kItemSelected))
	{
		if (sender == mIdentity)
		{
			JIndex item = dynamic_cast<const JXMenu::ItemSelected*>(&message)->GetIndex();
			switch(item)
			{
			// New identity wanted
			case eIdentityPopup_New:
				mIdentity->DoNewIdentity(CPreferences::sPrefs);
				break;
			
			// New identity wanted
			case eIdentityPopup_Edit:
				mIdentity->DoEditIdentity(CPreferences::sPrefs);
				break;
			
			// Delete existing identity
			case eIdentityPopup_Delete:
				mIdentity->DoDeleteIdentity(CPreferences::sPrefs);
				break;
			
			// Select an identity
			default:
				const CIdentity& identity = CPreferences::sPrefs->mIdentities.Value()[item - mIdentity->FirstIndex()];
				SetIdentity(&identity);
				break;
			}

			return;
		}
	}
	else if (message.Is(JXButton::kPushed))
	{
		if (sender == mAddAttendees)
		{
			OnAddAttendees();
			return;
		}
		else if (sender == mNewBtn)
		{
			OnNew();
			return;
		}
		else if (sender == mChangeBtn)
		{
			OnChange();
			return;
		}
		else if (sender == mDeleteBtn)
		{
			OnDelete();
			return;
		}
	}
}

void CNewComponentAttendees::ListenTo_Message(long msg, void* param)
{
	switch (msg)
	{
	case CAttendeeTable::eBroadcast_SelectionChange:
		// Only if current user is the organiser
		if (!mReadOnly && mOrganizerIsMe)
			OnSelectionChange();
		break;
	case CAttendeeTable::eBroadcast_DblClk:
		// Only if current user is the organiser
		if (!mReadOnly && mOrganizerIsMe)
			OnChange();
		break;
	default:;
	}
}

void CNewComponentAttendees::SetEvent(const iCal::CICalendarVEvent& vevent)
{
	// Set the relevant fields
	SetComponent(vevent);
}

void CNewComponentAttendees::SetToDo(const iCal::CICalendarVToDo& vtodo)
{
	// Set the relevant fields
	SetComponent(vtodo);
}

void CNewComponentAttendees::SetComponent(const iCal::CICalendarComponentRecur& vcomp)
{
	// Get organiser info
	cdstring org;
	mHasOrganizer = vcomp.GetProperty(iCal::cICalProperty_ORGANIZER, org);
	mUseIdentity = false;
	
	// Check whether organiser is us
	if (mHasOrganizer)
	{
		mOrganizer = vcomp.GetProperties().find(iCal::cICalProperty_ORGANIZER)->second;
		if (org.compare_start(cMailtoURLScheme, true))
		{
			CAddress addr(org.c_str() + ::strlen(cMailtoURLScheme));
			mOrganizerIsMe = CPreferences::TestSmartAddress(addr);
			
			// Now look for identity
			if (mOrganizer.HasAttribute(iCal::cICalAttribute_ORGANIZER_X_IDENTITY))
			{
				const cdstring& idname = mOrganizer.GetAttributeValue(iCal::cICalAttribute_ORGANIZER_X_IDENTITY);
				const CIdentity* id = CPreferences::sPrefs->mIdentities.GetValue().GetIdentity(idname);
				if (id != NULL)
				{
					mOldIdentity = id;
					mUseIdentity = true;
					mOrganizerIsMe = true;

					StStopListening _stop(mIdentity);
					mIdentity->SetValue(mIdentity->FirstIndex() + CPreferences::sPrefs->mIdentities.GetValue().GetIndex(idname));
				}
			}
		}
		else
		{
			mOrganizerIsMe = false;
		}

		mNewBtn->SetActive(mOrganizerIsMe);
		mChangeBtn->SetActive(mOrganizerIsMe);
		mDeleteBtn->SetActive(mOrganizerIsMe);

		// Switch to attendee pane
		mAddAttendeesPane->SetVisible(false);
		mAttendeesPane->SetVisible(true);
						
		// Hide organiser text and show the identity popup
		mOrganiser->SetVisible(!mUseIdentity);
		mIdentity->SetVisible(mUseIdentity);
		
		// Set organiser text
		mOrganiser->SetText(org);
		
		mOrganizerChanged = false;
	}

	// Copy all attendee properties
	const iCal::CICalendarPropertyMap& props = vcomp.GetProperties();
	pair<iCal::CICalendarPropertyMap::const_iterator, iCal::CICalendarPropertyMap::const_iterator> result = props.equal_range(iCal::cICalProperty_ATTENDEE);
	for(iCal::CICalendarPropertyMap::const_iterator iter = result.first; iter != result.second; iter++)
	{
		// Copy the property
		mAttendees.push_back((*iter).second);
	}
	
	mTable->ResetTable(&mAttendees);
}

void CNewComponentAttendees::GetEvent(iCal::CICalendarVEvent& vevent)
{
	// Do descriptive items
	GetComponent(vevent);
}

void CNewComponentAttendees::GetToDo(iCal::CICalendarVToDo& vtodo)
{
	// Do descriptive items
	GetComponent(vtodo);
}

void CNewComponentAttendees::GetComponent(iCal::CICalendarComponentRecur& vcomp)
{
	// Deal with organizer if it has changed
	if (mOrganizerChanged && mHasOrganizer)
	{
		vcomp.RemoveProperties(iCal::cICalProperty_ORGANIZER);
		vcomp.AddProperty(mOrganizer);
	}

	// Remove existing properties
	vcomp.RemoveProperties(iCal::cICalProperty_ATTENDEE);
	
	// Add new ones that exist
	for(iCal::CICalendarPropertyList::const_iterator iter = mAttendees.begin(); iter != mAttendees.end(); iter++)
	{
		// Copy the property
		vcomp.AddProperty(*iter);
	}
}

void CNewComponentAttendees::SetReadOnly(bool read_only)
{
	mReadOnly = read_only;

	mAddAttendees->SetActive(!read_only);

	mIdentity->SetActive(!read_only);

	mNewBtn->SetActive(!read_only);
	mChangeBtn->SetActive(!read_only);
	mDeleteBtn->SetActive(!read_only);
}

void CNewComponentAttendees::OnSelectionChange()
{
	bool has_selection = mTable->IsSelectionValid();
	mChangeBtn->SetActive(has_selection);
	mDeleteBtn->SetActive(has_selection);
}

void CNewComponentAttendees::SetIdentity(const CIdentity* id)
{
	// We get passed in an email address, so parse into an address item
	CAddress addr(id->GetFrom(true));
	
	// Create the CalAddress URI part
	cdstring uri(cMailtoURLScheme);
	uri += addr.GetMailAddress();
	
	// Create the property
	iCal::CICalendarProperty prop(iCal::cICalProperty_ORGANIZER, uri, iCal::CICalendarValue::eValueType_CalAddress);
	
	// If a full name is present, create the CN= attribute
	if (!addr.GetName().empty())
	{
		prop.AddAttribute(iCal::CICalendarAttribute(iCal::cICalAttribute_CN, addr.GetName()));
	}
	
	if (id->UseSender())
	{
		CAddress sender(id->GetSender());
		cdstring uri(cMailtoURLScheme);
		uri += addr.GetMailAddress();
		prop.AddAttribute(iCal::CICalendarAttribute(iCal::cICalAttribute_SENT_BY, uri));
	}
	
	prop.AddAttribute(iCal::CICalendarAttribute(iCal::cICalAttribute_ORGANIZER_X_IDENTITY, id->GetIdentity()));

	// Set the organiser
	mOrganizer = prop;
	
	// Look for CHAIR in attendees and see if it matches the old identity
	if (mOldIdentity != NULL)
	{
		for(iCal::CICalendarPropertyList::const_iterator iter = mAttendees.begin(); iter != mAttendees.end(); iter++)
		{
			// Check for ROLE=CHAIR
			const iCal::CICalendarProperty& prop = *iter;
			if (prop.HasAttribute(iCal::cICalAttribute_ROLE))
			{
				const cdstring& role = prop.GetAttributeValue(iCal::cICalAttribute_ROLE);
				if (role.compare(iCal::cICalAttribute_ROLE_CHAIR, true) == 0)
				{
					// See if value matches this identity
					const cdstring& caladdr = prop.GetCalAddressValue()->GetValue();
					if (caladdr.compare_start(cMailtoURLScheme))
					{
						CAddress old_addr(mOldIdentity->GetFrom(true));
						cdstring temp(caladdr, ::strlen(cMailtoURLScheme));
						if (old_addr == temp)
						{
							// Change value to new identity
							iCal::CICalendarCalAddressValue* value = const_cast<iCal::CICalendarCalAddressValue*>(prop.GetCalAddressValue());
							value->SetValue(uri);
							
							// Remove any CN=
							const_cast<iCal::CICalendarProperty&>(prop).RemoveAttributes(iCal::cICalAttribute_CN);
							
							// If a full name is present, create the CN= attribute
							if (!addr.GetName().empty())
							{
								const_cast<iCal::CICalendarProperty&>(prop).AddAttribute(iCal::CICalendarAttribute(iCal::cICalAttribute_CN, addr.GetName()));
							}

							mTable->ResetTable(&mAttendees);
							break;
						}
					}
				}
			}
		}
	}
	
	// Cache old identity
	mOldIdentity = id;
}

void CNewComponentAttendees::OnAddAttendees()
{
	// Start with default identity - ideally should pick tied identity
	const CIdentity* id = &CPreferences::sPrefs->mIdentities.GetValue().front();
	SetIdentity(id);
	{
		StStopListening _stop(mIdentity);
		mIdentity->SetValue(mIdentity->FirstIndex());
	}

	// Set the organiser
	mHasOrganizer = true;
	mOrganizerIsMe = true;
	mOrganizerChanged = true;
	mUseIdentity = true;

	// Switch to attendee pane
	mAddAttendeesPane->SetVisible(false);
	mAttendeesPane->SetVisible(true);
	
	// Hide organiser text and show the identity popup
	mOrganiser->SetVisible(false);
	mIdentity->SetVisible(true);
	
	// We get passed in an email address, so parse into an address item
	CAddress addr(id->GetFrom(true));
	
	// Create the CalAddress URI part
	cdstring uri(cMailtoURLScheme);
	uri += addr.GetMailAddress();
	
	// Add chair
	iCal::CICalendarProperty prop(iCal::cICalProperty_ATTENDEE, uri, iCal::CICalendarValue::eValueType_CalAddress);
	prop.AddAttribute(iCal::CICalendarAttribute(iCal::cICalAttribute_ROLE, iCal::cICalAttribute_ROLE_CHAIR));
	prop.AddAttribute(iCal::CICalendarAttribute(iCal::cICalAttribute_PARTSTAT, iCal::cICalAttribute_PARTSTAT_ACCEPTED));
	if (!addr.GetName().empty())
	{
		prop.AddAttribute(iCal::CICalendarAttribute(iCal::cICalAttribute_CN, addr.GetName()));
	}
	mAttendees.push_back(prop);

	mTable->ResetTable(&mAttendees);
	OnSelectionChange();
}

void CNewComponentAttendees::OnNew()
{
	// List for possibly multiple attendees
	iCal::CICalendarPropertyList props;

	// Do dialog
	if (CNewAttendeeDialog::PoseDialog(props))
	{
		// Add to list and reset table
		for(iCal::CICalendarPropertyList::const_iterator iter = props.begin(); iter != props.end(); iter++)
			mAttendees.push_back(*iter);
		
		mTable->ResetTable(&mAttendees);
	}
}

void CNewComponentAttendees::OnChange()
{
	// Get the selected item from the table
	TableIndexT selected = mTable->GetFirstSelectedRow();
	
	// Do dialog
	iCal::CICalendarProperty prop = mAttendees.at(selected - 1);
	if (CNewAttendeeDialog::PoseDialog(prop))
	{
		// Add to list and reset table
		mAttendees.at(selected - 1) = prop;
		mTable->ResetTable(&mAttendees);
	}
}

void CNewComponentAttendees::OnDelete()
{
	// Do delete prompt
	if (CErrorHandler::PutCautionAlertRsrc(true, "CNewComponentAttendees::ReallyDelete") == CErrorHandler::Cancel)
		return;

	// Get the selected item from the table
	ulvector selected;
	mTable->GetSelectedRows(selected);
	
	// Do in reverse
	for(ulvector::reverse_iterator riter = selected.rbegin(); riter != selected.rend(); riter++)
	{
		mAttendees.erase(mAttendees.begin() + *riter - 1);
	}
	mTable->ResetTable(&mAttendees);
}
