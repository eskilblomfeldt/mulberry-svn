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
#include "CNewAttendeeDialog.h"
#include "CPreferences.h"
#include "CUnicodeUtils.h"
#include "CURL.h"

// ---------------------------------------------------------------------------
//	CNewComponentAttendees														  [public]
/**
	Default constructor */

CNewComponentAttendees::CNewComponentAttendees() :
	CNewComponentPanel(IDD_CALENDAR_NEW_ATTENDEES)
{
	mHasOrganizer = false;
	mOrganizerIsMe = false;
	mOrganizerChanged = false;
	mUseIdentity = false;
	mOldIdentity = NULL;
}


// ---------------------------------------------------------------------------
//	~CNewComponentAttendees														  [public]
/**
	Destructor */

CNewComponentAttendees::~CNewComponentAttendees()
{
}

#pragma mark -

BEGIN_MESSAGE_MAP(CNewComponentAttendees, CNewComponentPanel)
	//{{AFX_MSG_MAP(CNewComponentAttendees)
	ON_COMMAND(IDC_CALENDAR_ATTENDEES_ADD, OnAddAttendees)
	ON_COMMAND_RANGE(IDM_IDENTITY_NEW, IDM_IDENTITYEnd, OnIdentityPopup)
	ON_COMMAND(IDC_CALENDAR_ATTENDEES_LIST, OnSelectionChange)
	ON_COMMAND(IDC_CALENDAR_ATTENDEES_LISTDBLCLK, OnChange)
	ON_COMMAND(IDC_CALENDAR_ATTENDEES_NEW, OnNew)
	ON_COMMAND(IDC_CALENDAR_ATTENDEES_CHANGE, OnChange)
	ON_COMMAND(IDC_CALENDAR_ATTENDEES_DELETE, OnDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CNewComponentAttendees::OnInitDialog()
{
	CNewComponentPanel::OnInitDialog();

	// Get UI items
	mOrganiser.SubclassDlgItem(IDC_CALENDAR_ATTENDEES_ORGANISER, this);
	mIdentity.SubclassDlgItem(IDC_CALENDAR_ATTENDEES_IDENTITY, this, IDI_POPUPBTN, 0, 0, 0, true, false);
	mIdentity.SetMenu(IDR_POPUP_IDENTITY);
	mIdentity.Reset(CPreferences::sPrefs->mIdentities.GetValue());
	mTitles.SubclassDlgItem(IDC_CALENDAR_ATTENDEES_TITLES, this);
	mTitles.LoadTitles("UI::Titles::CalendarAttendees", 3);
	mTable.SetTitles(&mTitles);
	mTable.SubclassDlgItem(IDC_CALENDAR_ATTENDEES_LIST, this);
	mNewBtn.SubclassDlgItem(IDC_CALENDAR_ATTENDEES_NEW, this);
	mChangeBtn.SubclassDlgItem(IDC_CALENDAR_ATTENDEES_CHANGE, this);
	mDeleteBtn.SubclassDlgItem(IDC_CALENDAR_ATTENDEES_DELETE, this);

	// Start with attendee list hidden
	ShowAttendees(false);
	OnSelectionChange();

	return true;
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

					mIdentity.SetValue(IDM_IDENTITYStart + CPreferences::sPrefs->mIdentities.GetValue().GetIndex(idname));
				}
			}
		}
		else
		{
			mOrganizerIsMe = false;
		}

		mNewBtn.EnableWindow(mOrganizerIsMe);
		mChangeBtn.EnableWindow(mOrganizerIsMe);
		mDeleteBtn.EnableWindow(mOrganizerIsMe);

		// Switch to attendee pane
		ShowAttendees(true);
						
		// Hide organiser text and show the identity popup
		mOrganiser.ShowWindow(!mUseIdentity);
		mIdentity.ShowWindow(mUseIdentity);
		
		// Set organiser text
		CUnicodeUtils::SetWindowTextUTF8(&mOrganiser, org);
		
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
	
	mTable.ResetTable(&mAttendees);
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

	GetDlgItem(IDC_CALENDAR_ATTENDEES_ADD)->EnableWindow(!read_only);

	mIdentity.EnableWindow(!read_only);

	mNewBtn.EnableWindow(!read_only);
	mChangeBtn.EnableWindow(!read_only);
	mDeleteBtn.EnableWindow(!read_only);
}

void CNewComponentAttendees::OnSelectionChange()
{
	bool has_selection = mTable.IsSelectionValid() && !mReadOnly && mOrganizerIsMe;
	mChangeBtn.EnableWindow(has_selection);
	mDeleteBtn.EnableWindow(has_selection);
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
	
	// Look for CHAIR in attendees and see if it matches the old identity\
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

							mTable.ResetTable(&mAttendees);
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
	mIdentity.SetValue(IDM_IDENTITYStart);

	// Set the organiser
	mHasOrganizer = true;
	mOrganizerIsMe = true;
	mOrganizerChanged = true;
	mUseIdentity = true;
	
	// Switch to attendee pane
	ShowAttendees(true);
	
	// Hide organiser text and show the identity popup
	mOrganiser.ShowWindow(false);
	mIdentity.ShowWindow(true);
	
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

	mTable.ResetTable(&mAttendees);
}

void CNewComponentAttendees::OnIdentityPopup(UINT nID)
{
	switch(nID)
	{
	// New identity wanted
	case IDM_IDENTITY_NEW:
		mIdentity.DoNewIdentity(CPreferences::sPrefs);
		break;
	
	// New identity wanted
	case IDM_IDENTITY_EDIT:
		mIdentity.DoEditIdentity(CPreferences::sPrefs);
		break;
	
	// Delete existing identity
	case IDM_IDENTITY_DELETE:
		mIdentity.DoDeleteIdentity(CPreferences::sPrefs);
		break;
	
	// Select an identity
	default:
		{
			const CIdentity& identity = CPreferences::sPrefs->mIdentities.GetValue()[nID - IDM_IDENTITYStart];
			SetIdentity(&identity);
			mIdentity.SetValue(nID);
		}
		break;
	}
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
		
		mTable.ResetTable(&mAttendees);
	}
}

void CNewComponentAttendees::OnChange()
{
	if (mReadOnly || !mOrganizerIsMe)
		return;

	// Get the selected item from the table
	TableIndexT selected = mTable.GetFirstSelectedRow();
	
	// Do dialog
	iCal::CICalendarProperty prop = mAttendees.at(selected - 1);
	if (CNewAttendeeDialog::PoseDialog(prop))
	{
		// Add to list and reset table
		mAttendees.at(selected - 1) = prop;
		mTable.ResetTable(&mAttendees);
	}
}

void CNewComponentAttendees::OnDelete()
{
	// Do delete prompt
	if (CErrorHandler::PutCautionAlertRsrc(true, "CNewComponentAttendees::ReallyDelete") == CErrorHandler::Cancel)
		return;

	// Get the selected item from the table
	ulvector selected;
	mTable.GetSelectedRows(selected);
	
	// Do in reverse
	for(ulvector::reverse_iterator riter = selected.rbegin(); riter != selected.rend(); riter++)
	{
		mAttendees.erase(mAttendees.begin() + *riter - 1);
	}
	mTable.ResetTable(&mAttendees);
}

void CNewComponentAttendees::ShowAttendees(bool show)
{
	if (show)
	{
		GetDlgItem(IDC_CALENDAR_ATTENDEES_ADD)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CALENDAR_ATTENDEES_ORGANISER_TITLE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CALENDAR_ATTENDEES_ORGANISER)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CALENDAR_ATTENDEES_IDENTITY)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CALENDAR_ATTENDEES_TITLES)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CALENDAR_ATTENDEES_LIST)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CALENDAR_ATTENDEES_NEW)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CALENDAR_ATTENDEES_CHANGE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CALENDAR_ATTENDEES_DELETE)->ShowWindow(SW_SHOW);
	}
	else
	{
		GetDlgItem(IDC_CALENDAR_ATTENDEES_ADD)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CALENDAR_ATTENDEES_ORGANISER_TITLE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CALENDAR_ATTENDEES_ORGANISER)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CALENDAR_ATTENDEES_IDENTITY)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CALENDAR_ATTENDEES_TITLES)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CALENDAR_ATTENDEES_LIST)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CALENDAR_ATTENDEES_NEW)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CALENDAR_ATTENDEES_CHANGE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CALENDAR_ATTENDEES_DELETE)->ShowWindow(SW_HIDE);
	}
}
