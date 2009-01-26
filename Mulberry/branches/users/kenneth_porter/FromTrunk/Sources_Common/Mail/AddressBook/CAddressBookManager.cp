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


// CAddressBookManager.h

// Class to handle all actions related to local & report address books,
// including opening/closing, nick-name resolution, searching etc

#include "CAddressBookManager.h"

#include "CAddressBookWindow.h"
#include "CAdbkManagerTable.h"
#include "CAdbkManagerView.h"
#include "CAdbkProtocol.h"
#include "CAddrLookupProtocol.h"
#include "CAddressDisplay.h"
#include "CAdminLock.h"
#include "CConnectionManager.h"
#include "CEditAddressDialog.h"
#include "CErrorHandler.h"
#include "CLDAPClient.h"
#include "CLocalAddressBook.h"
#include "CMessage.h"
#include "CMessageList.h"
#include "CMulberryApp.h"
#include "CMulberryCommon.h"
#include "CPreferences.h"
#include "CTextListChoice.h"
#include "CURL.h"
#include "C3PaneWindow.h"

#if __dest_os == __mac_os || __dest_os == __mac_os_x
#include "CAddressDisplay.h"
#include "CStringResources.h"
#include "FullPath.h"
#endif

CAddressBookManager* CAddressBookManager::sAddressBookManager = NULL;

CAddressBookManager::CAddressBookManager() : mAdbks(false)
{
	// Add item for local address books (always first)
	cdstring* name = new cdstring;
	name->FromResource("UI::AdbkMgr::Others");
	mAdbks.push_back(name, false);

	mLocalProto = NULL;
	mOSProto = NULL;

	CAddressBookManager::sAddressBookManager = this;
	
	// Must inform any adbk manager windows that currently exist that
	// the adbk manager is now here so that protocol change broadcasts work
	UpdateWindows();
}

CAddressBookManager::~CAddressBookManager()
{
	// Remove all windows
	//CAddressBookWindow::CloseAllWindows();

	// Remove all server views now to prevent illegal updates to 'stale' windows
	{
		cdmutexprotect<CAdbkManagerView::CAdbkManagerViewList>::lock _lock(CAdbkManagerView::sAdbkManagerViews);
		for(CAdbkManagerView::CAdbkManagerViewList::iterator iter = CAdbkManagerView::sAdbkManagerViews->begin(); iter != CAdbkManagerView::sAdbkManagerViews->end(); iter++)
			(*iter)->DoClose();
	}

	// Close all locals
	mAdbks.GetChildren()->at(0)->erase_children();

	// Logoff each protocol (includes the local one)
	for(CAdbkProtocolList::iterator iter = mProtos.begin(); iter != mProtos.end(); iter++)
	{
		StopProtocol(*iter);
		RemoveProtocol(*iter);
	}

	// Remove existing
	for(CAddrLookupProtocolList::iterator iter = mLookups.begin(); iter != mLookups.end(); iter++)
		delete *iter;
	mLookups.clear();

	// Clear out any persistent search results
	ClearSearch();
	CAddressBookManager::sAddressBookManager = NULL;
}

// Update all windows for changes
void CAddressBookManager::UpdateWindows()
{
	// Iterate over all adbk manager windows
	cdmutexprotect<CAdbkManagerView::CAdbkManagerViewList>::lock _lock(CAdbkManagerView::sAdbkManagerViews);
	for(CAdbkManagerView::CAdbkManagerViewList::iterator iter = CAdbkManagerView::sAdbkManagerViews->begin(); iter != CAdbkManagerView::sAdbkManagerViews->end(); iter++)
		// Force visual update
		(*iter)->GetTable()->SetManager(this);
}

// Add a new adbk remote protocol
void CAddressBookManager::AddProtocol(CAdbkProtocol* proto)
{
	// Add item for remote address books
	mAdbks.push_back(proto->GetAdbkList());

	// Add to protos list
	mProtos.push_back(proto);

	// Broadcast change to all after adding
	Broadcast_Message(eBroadcast_NewAdbkAccount, proto);
}

// Remove an adbk remote protocol
void CAddressBookManager::RemoveProtocol(CAdbkProtocol* proto)
{
	// Broadcast change to all before delete
	Broadcast_Message(eBroadcast_RemoveAdbkAccount, proto);

	// Remove from list and delete
	mAdbks.erase(proto->GetAdbkList());
	delete proto;
	proto = NULL;
}

// Get list of protocol names
void CAddressBookManager::GetProtocolNameList(cdstrvect& names)
{
	// Always add local
	cdstring* name = new cdstring;
	name->FromResource("UI::AdbkMgr::Local");
	mAdbks.push_back(name, false);

	// Add remaining protos
	for(CAdbkProtocolList::iterator iter = mProtos.begin(); iter != mProtos.end(); iter++)
		names.push_back((*iter)->GetAccountName());
}

// Add a new adbk remote protocol
CAdbkProtocol* CAddressBookManager::GetProtocol(const cdstring& name)
{
	for(CAdbkProtocolList::iterator iter = mProtos.begin(); iter != mProtos.end(); iter++)
	{
		if ((*iter)->GetAccountName() == name)
			return *iter;
	}

	return NULL;
}

// Clear all search results
void CAddressBookManager::ClearSearch()
{
	// Manually delete all items
	for(CAddressSearchResultList::iterator iter = mSearchResultList.begin();
		iter != mSearchResultList.end(); iter++)
	{
		// Delete address list then item itself
		delete (*iter)->second;
		delete *iter;
	}

	mSearchResultList.clear();
}

// Clear single search result
void CAddressBookManager::ClearSearchItem(CAddressSearchResult* item)
{
	CAddressSearchResultList::iterator found = ::find(mSearchResultList.begin(), mSearchResultList.end(), item);

	if (found != mSearchResultList.end())
	{
		delete item->second;
		delete item;
		mSearchResultList.erase(found);
	}
}

// Clear search result address
void CAddressBookManager::ClearSearchItemAddress(CAddressSearchResult* item, CAddress* addr)
{
	// Find result first
	CAddressSearchResultList::iterator found = ::find(mSearchResultList.begin(), mSearchResultList.end(), item);

	if (found != mSearchResultList.end())
	{
		// Now find address
		CAddressList::iterator found_addr = ::find(item->second->begin(), item->second->end(), addr);
		if (found_addr != item->second->end())
			item->second->erase(found_addr);
	}
}

// Sync with changed accounts
void CAddressBookManager::SyncAccounts(const CAddressAccountList& accts)
{
	// Sync accounts for adbk and directory protocols
	SyncProtos(accts);
	SyncLookups(accts);

	// Make sure 3-pane is told to re-instate previously open address books
	if (C3PaneWindow::s3PaneWindow)
		C3PaneWindow::s3PaneWindow->DoneInitAdbkAccounts();
}

// Sync with changed accounts
void CAddressBookManager::SyncProtos(const CAddressAccountList& accts)
{
	// Only add local protocol if allowed by admin
	if (!CAdminLock::sAdminLock.mNoLocalAdbks)
	{
#if __dest_os == __mac_os || __dest_os == __mac_os_x
#if PP_Target_Carbon
		// Addressbook.Framework only available on OS X 10.2 and later
	    if (UEnvironment::GetOSVersion() >= 0x01020)
		{
			// First check if local exists
			if (mOSProto)
			{
				// Sync its account with prefs etc
				UpdateProtocol(mOSProto);

				// Resync account object
				mOSProto->SetAccount(&CPreferences::sPrefs->mOSAdbkAccount.Value());
			}
			else
			{
				// Create new local protocol
				mOSProto = new CAdbkProtocol(&CPreferences::sPrefs->mOSAdbkAccount.Value());
				mOSProto->SetDescriptor(CPreferences::sPrefs->mOSAdbkAccount.GetValue().GetName());
				AddProtocol(mOSProto);
			}
		}
#endif
#endif

		// First check if local exists
		if (mLocalProto)
		{
			// Sync its account with prefs etc
			UpdateProtocol(mLocalProto);

			// Resync account object
			mLocalProto->SetAccount(&CPreferences::sPrefs->mLocalAdbkAccount.Value());
		}
		else
		{
			// Create new local protocol
			mLocalProto = new CAdbkProtocol(&CPreferences::sPrefs->mLocalAdbkAccount.Value());
			mLocalProto->SetDescriptor(CPreferences::sPrefs->mLocalAdbkAccount.GetValue().GetName());
			AddProtocol(mLocalProto);
		}
	}

	// New smart alogorithm: compare protocol and protocol embedded in account

	// This maintains a list of protocols that need to be started
	CAdbkProtocolList started;

	// First delete protocols not in accounts
	unsigned long proto_offset = (mOSProto ? 1 : 0) + (mLocalProto ? 1 : 0);
	for(CAdbkProtocolList::iterator iter1 = mProtos.begin() + proto_offset; iter1 != mProtos.end(); iter1++)
	{
		bool found = false;

		// Look for match between existing protocol and new accounts
		for(CAddressAccountList::const_iterator iter2 = accts.begin(); !found && (iter2 != accts.end()); iter2++)
		{
			// Only if IMSP/ACAP
			if (((*iter2)->GetServerType() != CINETAccount::eIMSP) &&
				((*iter2)->GetServerType() != CINETAccount::eACAP))
				continue;

			if ((*iter2)->GetProtocol() == *iter1)
				found = true;
		}

		// Remove protocol if no matching account
		if (!found)
		{
			StopProtocol(*iter1);
			RemoveProtocol(*iter1);

			// Remove from list
			iter1 = mProtos.erase(iter1);
			iter1--;
		}
	}

	// Now resync accounts with existing protocols and add new protocols
	for(CAddressAccountList::const_iterator iter2 = accts.begin(); iter2 != accts.end(); iter2++)
	{
		// Only if IMSP/ACAP
		if (((*iter2)->GetServerType() != CINETAccount::eIMSP) &&
			((*iter2)->GetServerType() != CINETAccount::eACAP))
			continue;

		bool found = false;

		// Look for protocol matching account
		unsigned long proto_offset = (mOSProto ? 1 : 0) + (mLocalProto ? 1 : 0);
		for(CAdbkProtocolList::iterator iter1 = mProtos.begin() + proto_offset; !found && (iter1 != mProtos.end()); iter1++)
		{
			if ((*iter2)->GetProtocol() == *iter1)
			{
				found = true;

				// May need to redo server and/or authenticator
				if (((*iter1)->GetAccountUniqueness() != (*iter2)->GetUniqueness()) ||
					((*iter1)->GetAuthenticatorUniqueness() != (*iter2)->GetAuthenticator().GetUniqueness()))
					UpdateProtocol(*iter1);

				// Resync account object
				(*iter1)->SetAccount(*iter2);

				// Do login at start if required
				if ((*iter2)->GetLogonAtStart() && !(*iter1)->IsLoggedOn())
					started.push_back(*iter1);;
			}
		}

		// If not found add new protocol
		if (!found)
		{
			// Create a protocol for each account and add to list
			CAdbkProtocol* aAdbkProtocol = new CAdbkProtocol(*iter2);
			try
			{
				aAdbkProtocol->SetDescriptor((*iter2)->GetServerIP());
				AddProtocol(aAdbkProtocol);

				// Do login at start if required
				if ((*iter2)->GetLogonAtStart())
					started.push_back(aAdbkProtocol);
			}
			catch (...)
			{
				CLOG_LOGCATCH(...);

				// Do not allow to fail
			}
		}
	}

	// Do all protocol starts here once the entire list of accounts is consistent
	

	// Only start local protocols if allowed by admin
	if (mOSProto)
		StartProtocol(mOSProto);
	if (mLocalProto)
		StartProtocol(mLocalProto);

	for(CAdbkProtocolList::const_iterator iter = started.begin(); iter != started.end(); iter++)
		StartProtocol(*iter);
	
	// No need to do this - broadcasting of changes now takes care of it
	// Force visual update
	//UpdateWindows();
}

// Sync with changed accounts
void CAddressBookManager::SyncLookups(const CAddressAccountList& accts)
{
	// Remove existing
	for(CAddrLookupProtocolList::iterator iter = mLookups.begin(); iter != mLookups.end(); iter++)
		delete *iter;
	mLookups.clear();

	// Compare existing protocols with accounts
	for(CAddressAccountList::const_iterator iter1 = accts.begin(); iter1 != accts.end(); iter1++)
	{
		bool found = false;

		switch((*iter1)->GetServerType())
		{
		case CAddressAccount::eIMSP:
		case CAddressAccount::eACAP:
			// Never - these are ADBKProtocols
			break;

		case CAddressAccount::eLDAP:
			{
				// Create new client
				CLDAPClient* ldap_client = new CLDAPClient;
				
				// Give it account details
				ldap_client->SetAccount(*iter1);
				
				// Add to list
				mLookups.push_back(ldap_client);
				break;
			}
		case CAddressAccount::eWHOISPP:
		case CAddressAccount::eFinger:
		default:
			// Not supported at the moment
			break;
		}
	}	
}

// Sync cache with prefs
void CAddressBookManager::SyncCache()
{
}

// Start local
void CAddressBookManager::StartLocal()
{
	// Force window reset first
	UpdateWindows();

	cdstring local_url = cFileURLScheme;
	local_url += cURLLocalhost;

	// Try every open at start adbk
	for(cdstrvect::iterator iter = CPreferences::sPrefs->mAdbkOpenAtStartup.Value().begin();
			iter != CPreferences::sPrefs->mAdbkOpenAtStartup.Value().end(); )
	{
		if (::strncmp(*iter, local_url, local_url.length()) ==0)
		{
			const char* adbk_name = ((const char*) *iter) + local_url.length();

			// Convert URL to path name
			cdstring fname = adbk_name;
			char* p = fname;
			while(*p)
			{
				if (*p == '/') *p = os_dir_delim;
				p++;
			}
			fname.DecodeURL();
#if __dest_os == __mac_os || __dest_os == __mac_os_x
			// Resolve to file spec
			MyCFString cfstr(fname, kCFStringEncodingUTF8);
			PPx::FSObject spec(cfstr);

			// Open (erase from list if it fails)
			if (spec.IsValid())
				CMulberryApp::sApp->OpenDocument(&spec);
			else
			{
				if (CErrorHandler::PutCautionAlertRsrcStr(true, "Alerts::Adbk::NoLocalFound", fname.c_str()) == CErrorHandler::Ok)
				{
					iter = CPreferences::sPrefs->mAdbkOpenAtStartup.Value().erase(iter);
					CPreferences::sPrefs->mAdbkOpenAtStartup.SetDirty();
					continue;
				}
			}
#elif __dest_os == __win32_os
			// Open (erase from list if it fails)
			if (!CMulberryApp::sApp->OpenDocumentFile(fname.win_str()))
			{
				if (CErrorHandler::PutCautionAlertRsrcStr(true, "Alerts::Adbk::NoLocalFound", fname) == CErrorHandler::Ok)
				{
					iter = CPreferences::sPrefs->mAdbkOpenAtStartup.Value().erase((cdstrvect::iterator) iter);
					CPreferences::sPrefs->mAdbkOpenAtStartup.SetDirty();
					continue;
				}
			}
#elif __dest_os == __linux_os
			// Open (erase from list if it fails)
			if (!CMulberryApp::sApp->OpenLocalAddressBook(fname))
			{
				if (CErrorHandler::PutCautionAlertRsrcStr(true, "Alerts::Adbk::NoLocalFound", fname) == CErrorHandler::Ok)
				{
					iter = CPreferences::sPrefs->mAdbkOpenAtStartup.Value().erase((cdstrvect::iterator) iter);
					CPreferences::sPrefs->mAdbkOpenAtStartup.SetDirty();
					continue;
				}
			}
#else
#error __dest_os
#endif
		}
		
		iter++;
	}
}

// Start remote protocol
void CAddressBookManager::StartProtocol(CAdbkProtocol* proto, bool silent)
{
	// Only do if not already logged on
	if (proto->IsLoggedOn())
		return;

	// Check for a non-disconnectable proto when disconnected
	if (!CConnectionManager::sConnectionManager.IsConnected() &&
		!proto->IsOffline() && !proto->CanDisconnect())
		return;

	try
	{
		// Open protocol and get adbks
		if (CMulberryApp::sApp->BeginINET(proto))
		{
			proto->Logon();
			proto->LoadList();
			proto->SyncList();
		}
	}
	catch (...)
	{
		CLOG_LOGCATCH(...);

		if (!silent)
		{
			CLOG_LOGRETHROW;
			throw;
		}
	}
}

// Update remote protocol
void CAddressBookManager::UpdateProtocol(CAdbkProtocol* proto)
{
	try
	{
		// Logoff protocol (clears adbks)
		proto->Logoff();
	}
	catch (...)
	{
		CLOG_LOGCATCH(...);
	}
}

// Stop remote protocol
void CAddressBookManager::StopProtocol(CAdbkProtocol* proto)
{
	try
	{
		// Logoff protocol (clears adbks)
		proto->Logoff();
	}
	catch (...)
	{
		CLOG_LOGCATCH(...);
	}

	// Always force table to logged off state
	int pos = 0;

	if (FindProtocol(proto, pos))
	{
		TableIndexT temp = pos;
		// Iterate over all adbk manager windows
		cdmutexprotect<CAdbkManagerView::CAdbkManagerViewList>::lock _lock(CAdbkManagerView::sAdbkManagerViews);
		for(CAdbkManagerView::CAdbkManagerViewList::iterator iter = CAdbkManagerView::sAdbkManagerViews->begin(); iter != CAdbkManagerView::sAdbkManagerViews->end(); iter++)
			(*iter)->GetTable()->RemoveChildren(temp, true);
	}
}

// Sync address book
void CAddressBookManager::SyncAddressBook(CAddressBook* adbk, bool add)
{
	if (add)
	{
		if (adbk->IsLookup())
		{
			// Only add if not already present
			CFlatAdbkList::iterator found = ::find(mAdbkNickName.begin(), mAdbkNickName.end(), adbk);
			if (found == mAdbkNickName.end())
				mAdbkNickName.push_back(adbk);
		}
		if (adbk->IsSearch())
		{
			// Only add if not already present
			CFlatAdbkList::iterator found = ::find(mAdbkSearch.begin(), mAdbkSearch.end(), adbk);
			if (found == mAdbkSearch.end())
				mAdbkSearch.push_back(adbk);
		}
	}
	else
	{
		// Remove from caches
		CFlatAdbkList::iterator found = ::find(mAdbkNickName.begin(), mAdbkNickName.end(), adbk);
		if (found != mAdbkNickName.end())
			mAdbkNickName.erase(found);
		found = ::find(mAdbkSearch.begin(), mAdbkSearch.end(), adbk);
		if (found != mAdbkSearch.end())
			mAdbkSearch.erase(found);
	}
}

// Add a new local adbk
void CAddressBookManager::AddLocal(CAddressBook* adbk)
{
	// Add to list
	CAdbkList::node_type* node = mAdbks.GetChildren()->at(0)->push_back(adbk, true);

	// Get adbk URL
	cdstring adbk_url = adbk->GetURL();

	// Match in open at start list
	cdstrvect::const_iterator found = ::find(CPreferences::sPrefs->mAdbkOpenAtStartup.GetValue().begin(),
											 CPreferences::sPrefs->mAdbkOpenAtStartup.GetValue().end(),
											 adbk_url);

	if (found != CPreferences::sPrefs->mAdbkOpenAtStartup.GetValue().end())
		adbk->SetFlags(CAddressBook::eOpenOnStart, true);

	// Match in nick name list
	found = ::find(CPreferences::sPrefs->mAdbkNickName.GetValue().begin(),
					CPreferences::sPrefs->mAdbkNickName.GetValue().end(), adbk_url);

	if (found != CPreferences::sPrefs->mAdbkNickName.GetValue().end())
		adbk->SetFlags(CAddressBook::eLookup, true);

	// Match in search list
	found = ::find(CPreferences::sPrefs->mAdbkSearch.GetValue().begin(),
					CPreferences::sPrefs->mAdbkSearch.GetValue().end(),  adbk_url);

	if (found != CPreferences::sPrefs->mAdbkSearch.GetValue().end())
		adbk->SetFlags(CAddressBook::eSearch, true);

	// Sync with caches
	SyncAddressBook(adbk, true);

	// Iterate over all adbk manager windows
	cdmutexprotect<CAdbkManagerView::CAdbkManagerViewList>::lock _lock(CAdbkManagerView::sAdbkManagerViews);
	for(CAdbkManagerView::CAdbkManagerViewList::iterator iter = CAdbkManagerView::sAdbkManagerViews->begin(); iter != CAdbkManagerView::sAdbkManagerViews->end(); iter++)
	{
		// Update window
		if (mAdbks.GetChildren()->at(0)->size() == 1)
			(*iter)->GetTable()->ResetTable();
		else
		{
			TableIndexT pos = 1;
			(*iter)->GetTable()->AddNode(node, pos, true, true);
		}
	}
}

// Check if local address book already open
#if __dest_os == __mac_os || __dest_os == __mac_os_x
const CAddressBook* CAddressBookManager::CheckLocalOpen(const PPx::FSObject* fspec) const
#else
const CAddressBook* CAddressBookManager::CheckLocalOpen(const char* fname) const
#endif
{
	if (mAdbks.GetChildren()->at(0)->GetChildren())
	{
		for(CAdbkList::node_list::const_iterator iter = mAdbks.GetChildren()->at(0)->GetChildren()->begin();
			iter != mAdbks.GetChildren()->at(0)->GetChildren()->end(); iter++)
		{
#if __dest_os == __mac_os || __dest_os == __mac_os_x
			const PPx::FSObject& spec = ((CLocalAddressBook*) (*iter)->GetSelectData())->GetSpec();
			if (spec.IsEqualTo(*fspec))
				return (*iter)->GetSelectData();
#else
			if (((CLocalAddressBook*) (*iter)->GetSelectData())->GetFileName() == fname)
				return (*iter)->GetSelectData();
#endif
		}
	}

	return NULL;
}

// Remove local adbk
void CAddressBookManager::RemoveLocal(CAddressBook* adbk)
{
	size_t pos = mAdbks.GetChildren()->at(0)->erase(adbk);
	if (pos >= 0)
	{
		// Iterate over all adbk manager windows
		cdmutexprotect<CAdbkManagerView::CAdbkManagerViewList>::lock _lock(CAdbkManagerView::sAdbkManagerViews);
		for(CAdbkManagerView::CAdbkManagerViewList::iterator iter = CAdbkManagerView::sAdbkManagerViews->begin(); iter != CAdbkManagerView::sAdbkManagerViews->end(); iter++)
		{
			if (mAdbks.GetChildren()->at(0)->size())
				(*iter)->GetTable()->RemoveRows(1, pos + 2, true);
			else
				(*iter)->GetTable()->ResetTable();
		}
	}

	SyncAddressBook(adbk, false);
}

// Add adbk
void CAddressBookManager::AddAddressBook(CAddressBook* adbk)
{
	SyncAddressBook(adbk, true);
}

// Remove adbk
void CAddressBookManager::RemoveAddressBook(CAddressBook* adbk)
{
	int pos = 0;

	if (FindAddressBook(adbk, pos))
	{
		// Iterate over all adbk manager windows
		{
			cdmutexprotect<CAdbkManagerView::CAdbkManagerViewList>::lock _lock(CAdbkManagerView::sAdbkManagerViews);
			for(CAdbkManagerView::CAdbkManagerViewList::iterator iter = CAdbkManagerView::sAdbkManagerViews->begin(); iter != CAdbkManagerView::sAdbkManagerViews->end(); iter++)
				(*iter)->GetTable()->RemoveRows(1, pos, true);
		}
		
		// Readjust for missing local proto
		if (!mAdbks.GetChildren()->at(0)->size())
			pos++;
		mAdbks.erase(++pos);
	}

	SyncAddressBook(adbk, false);
}

// Remove adbk
void CAddressBookManager::RefreshAddressBook(CAddressBook* adbk)
{
	int pos = 0;

	if (FindAddressBook(adbk, pos))
	{
		// Iterate over all adbk manager windows
		cdmutexprotect<CAdbkManagerView::CAdbkManagerViewList>::lock _lock(CAdbkManagerView::sAdbkManagerViews);
		for(CAdbkManagerView::CAdbkManagerViewList::iterator iter = CAdbkManagerView::sAdbkManagerViews->begin(); iter != CAdbkManagerView::sAdbkManagerViews->end(); iter++)
			(*iter)->GetTable()->RefreshRow(pos);
	}
}

bool CAddressBookManager::FindAddressBook(const CAddressBook* adbk, int& pos) const
{
	pos = -1;
	bool result = mAdbks.find(adbk, pos);
	
	// Adjust for missing local
	if (result && !mAdbks.GetChildren()->at(0)->size())
		pos--;
	
	return result;
}

CAddressBook* CAddressBookManager::FindAddressBook(const cdstring& name, bool url) const
{
	// Look at each item trying to find a match
	for(CAdbkList::node_list::const_iterator iter1 = mAdbks.GetChildren()->begin();
		iter1 != mAdbks.GetChildren()->end(); iter1++)
	{
		// Look at each child
		if ((*iter1)->HasChildren())
		{
			for(CAdbkList::node_list::const_iterator iter2 = (*iter1)->GetChildren()->begin();
				iter2 != (*iter1)->GetChildren()->end(); iter2++)
			{
				if ((*iter2)->IsSelectable())
				{
					if (url && ((*iter2)->GetSelectData()->GetURL() == name) ||
						!url && ((*iter2)->GetSelectData()->GetAccountName() == name))
						return (*iter2)->GetSelectData();
				}
			}
		}
	}
	
	return NULL;
}

bool CAddressBookManager::FindProtocol(const CAdbkProtocol* proto, int& pos) const
{
	cdstring desc = proto->GetAccountName();

	pos = -1;
	bool result = mAdbks.findval(&desc, pos);

	// Adjust for missing local
	if (result && !mAdbks.GetChildren()->at(0)->size())
		pos--;
	
	return result;
}

void CAddressBookManager::GetCurrentAddressBookList(cdstrvect& adbks, bool url) const
{
	adbks.clear();

	// Look at each item trying to find a match
	for(CAdbkList::node_list::const_iterator iter1 = mAdbks.GetChildren()->begin();
		iter1 != mAdbks.GetChildren()->end(); iter1++)
	{
		// Add empty string as protocol separator if more than one
		if (adbks.size() != 0)
			adbks.push_back(cdstring::null_str);

		// Look at each child
		if ((*iter1)->HasChildren())
		{
			for(CAdbkList::node_list::const_iterator iter2 = (*iter1)->GetChildren()->begin();
				iter2 != (*iter1)->GetChildren()->end(); iter2++)
			{
				// If its a real address book add it to the list
				if ((*iter2)->IsSelectable())
					adbks.push_back(url ? (*iter2)->GetSelectData()->GetURL() : (*iter2)->GetSelectData()->GetAccountName());
			}
		}
	}
}

#pragma mark ____________________________Searching

// Resolve nick-name
bool CAddressBookManager::ResolveNickName(const char* nick_name, CAddressList* list)
{
	bool found = false;

	// Look through all address books
	for(CFlatAdbkList::const_iterator iter = mAdbkNickName.begin(); iter != mAdbkNickName.end(); iter++)
	{
		CAddressBook* adbk = *iter;
		CAdbkAddress* addr;

		// Not if disconnected without cache
		if (adbk->IsLocalAdbk() && !adbk->IsCachedAdbk())
			continue;

		// Try to get nick-name from address book
		if (adbk->FindNickName(nick_name, addr))
		{
			// Add to list if found (must copy to prevent double-deleting of address)
			list->push_back(new CAdbkAddress(*addr));
			found = true;
			break;
		}
	}

	return found;
}

// Resolve nick-name
bool CAddressBookManager::ResolveGroupName(const char* grp_name, CAddressList* list, short level)
{
	bool found = false;

	// Look through all address books
	for(CFlatAdbkList::const_iterator iter = mAdbkNickName.begin(); iter != mAdbkNickName.end(); iter++)
	{
		CAddressBook* adbk = *iter;
		CGroup* grp;

		// Not if disconnected without cache
		if (adbk->IsLocalAdbk() && !adbk->IsCachedAdbk())
			continue;

		// Try to get group nick-name from address book
		if (adbk->FindGroupName(grp_name, grp))
		{
			for(unsigned long i = 0; i < grp->GetAddressList().size(); i++)
			{
				// Get the string and insert into list, resolving nick-names/groups
				const cdstring& aStr = grp->GetAddressList().at(i);
				if (!list->InsertTextAddress(aStr, level))
				{
					// Prevent recursion and stack explosion by turning of resolve
					CAddressDisplay::AllowResolve(false);
					CErrorHandler::PutStopAlertRsrc("Alerts::Adbk::RecursionLimit");
					CAddressDisplay::AllowResolve(true);

					// Prevent further processing of nick-names to avoid alert more than once
					break;
				}
			}

			found = true;
			break;
		}
	}

	return found;
}

// Do search
void CAddressBookManager::SearchAddress(const cdstring& name, CAdbkAddress::EAddressMatch match, CAdbkAddress::EAddressField field, const cdstring& field_name)
{
	auto_ptr<CAddressList> addr_list(new CAddressList);

	// Get search results
	SearchAddress(name, match, field, *addr_list);

	// Add result to list
	cdstring title = field_name;
	title += " = ";
	title += name;
	CAddressSearchResult* item = new CAddressSearchResult(title, addr_list.release());
	mSearchResultList.push_back(item);
}

// Do search
void CAddressBookManager::SearchAddress(const cdstring& name, CAdbkAddress::EAddressMatch match, CAdbkAddress::EAddressField field, CAddressList& results)
{
	// Look through all address books
	for(CFlatAdbkList::const_iterator iter = mAdbkSearch.begin(); iter != mAdbkSearch.end(); iter++)
	{
		// Not if disconnected without cache
		if ((*iter)->IsLocalAdbk() && !(*iter)->IsCachedAdbk())
			continue;

		(*iter)->SearchAddress(name, match, field, results);
	}
}

// Do search (not IMSP/ACAP)
void CAddressBookManager::SearchAddress(EAddrLookup method, bool expanding, const cdstring& item, CAdbkAddress::EAddressMatch match, CAdbkAddress::EAddressField field)
{
	auto_ptr<CAddressList> addr_list(new CAddressList);

	// Get search results
	SearchAddress(method, expanding, item, match, field, *addr_list);

	// Add result to list
	cdstring title = item;
	CAddressSearchResult* result = new CAddressSearchResult(title, addr_list.release());
	mSearchResultList.push_back(result);
}

// Do search (not IMSP/ACAP)
void CAddressBookManager::SearchAddress(EAddrLookup method, bool expanding, const cdstring& item, CAdbkAddress::EAddressMatch match, CAdbkAddress::EAddressField field, CAddressList& results)
{
	// Look through all search servers
	for(CAddrLookupProtocolList::iterator iter = mLookups.begin(); iter != mLookups.end(); iter++)
	{
		bool do_it = false;

		// Match types
		switch(method)
		{
		case eMulberryLookup:
			// Never - done as adbk protocol
			break;
		case eLDAPLookup:
			// Must be connected and LDAP and marked for expansion/search
			do_it = CConnectionManager::sConnectionManager.IsConnected() &&
						((*iter)->GetAccount()->GetServerType() == CAddressAccount::eLDAP) &&
						(expanding ? (*iter)->GetAccount()->GetLDAPUseExpansion() : (*iter)->GetAccount()->GetLDAPSearch());
			
			// Also must check that LDAP attribute is specified
			if (do_it)
				do_it = !(*iter)->GetAccount()->GetLDAPMatch(field).empty();
			break;
		case eWhoisppLookup:
		case eFingerLookup:
		default:
			// Currently not supported
			break;
		}

		if (do_it)
			(*iter)->Lookup(item, match, field, results);
	}
}

// Do address expansion
void CAddressBookManager::ExpandAddresses(const cdstring& expand, cdstrvect& results)
{
	// Search for addresses in Mulberry address books
	CAddressList addr_list;
	if (CPreferences::sPrefs->mExpandFullName.GetValue())
	{
		SearchAddress(expand, CAdbkAddress::eMatchAnywhere, CAdbkAddress::eName, addr_list);
	}
	if (CPreferences::sPrefs->mExpandNickName.GetValue())
	{
		SearchAddress(expand, CAdbkAddress::eMatchAnywhere, CAdbkAddress::eNickName, addr_list);
	}
	if (CPreferences::sPrefs->mExpandEmail.GetValue())
	{
		SearchAddress(expand, CAdbkAddress::eMatchAnywhere, CAdbkAddress::eEmail, addr_list);

	}

	// Search for addresses in LDAP (only if required)
	if (!CPreferences::sPrefs->mSkipLDAP.GetValue() || (addr_list.size() == 0))
	{
		if (CPreferences::sPrefs->mExpandFullName.GetValue())
		{
			SearchAddress(eLDAPLookup, true, expand, CAdbkAddress::eMatchAnywhere, CAdbkAddress::eName, addr_list);
		}
		if (CPreferences::sPrefs->mExpandNickName.GetValue())
		{
			SearchAddress(eLDAPLookup, true, expand, CAdbkAddress::eMatchAnywhere, CAdbkAddress::eNickName, addr_list);
		}
		if (CPreferences::sPrefs->mExpandEmail.GetValue())
		{
			SearchAddress(eLDAPLookup, true, expand, CAdbkAddress::eMatchAnywhere, CAdbkAddress::eEmail, addr_list);
		}
	}
	
	// Now make results unique
	addr_list.MakeUnique();
	
	// Add them to the cdstrvect
	addr_list.AddMailAddressToList(results, true);
}

// Do address capture
void CAddressBookManager::CaptureAddress(const cdstring& capture)
{
	// Parse addresses without resolution
	CAddressList parsed_addrs(capture.c_str(), capture.length(), 0, false);

	// Make unique via merge to a new list
	CAddressList addrs;
	addrs.MergeList(&parsed_addrs);
	
	// Now capture them all
	CaptureAddress(addrs);
}

// Do address capture
void CAddressBookManager::CaptureAddress(const CMessage& msg)
{
	// Get addresses to capture
	CAddressList addrs;
	ExtractCaptureAddress(msg, addrs);
	
	// Now capture them all
	CaptureAddress(addrs);
}

// Do address capture
void CAddressBookManager::CaptureAddress(const CMessageList& msgs)
{
	// Get addresses to capture
	CAddressList addrs;
	for(CMessageList::const_iterator iter = msgs.begin(); iter != msgs.end(); iter++)
	{
		ExtractCaptureAddress(**iter, addrs);
	}
	
	// Now capture them all
	CaptureAddress(addrs);
}

// Do address capture
void CAddressBookManager::CaptureAddress(CAddressList& addrs)
{
	try
	{
		// Look at each address and remove smarts or invalid
		for(CAddressList::iterator iter = addrs.begin(); iter != addrs.end() ; )
		{
			if (CPreferences::TestSmartAddress(**iter) || !(*iter)->IsValid())
			{
				iter = addrs.erase(iter);
				continue;
			}
			
			iter++;
		}

		// Must have some addresses to add
		if (addrs.empty())
			return;

		// Get default address book to capture to
		CAddressBook* adbk = NULL;
		
		// Get default address book chosen via prefs (uses accunt name not url)
		adbk = FindAddressBook(CPreferences::sPrefs->mCaptureAddressBook.GetValue(), false);
		
		// If none set, use the first one in the first account
		if (adbk == NULL)
		{
			// Try the first account
			CAdbkProtocol* proto = NULL;
			if (mProtos.size())
				proto = mProtos.front();
			
			// Get the first address book from the proto
			if (proto)
			{
				// Check for valid address book list in protocol
				CAdbkList* alist = proto->GetAdbkList();
				if ((alist != NULL) && alist->HasChildren())
				{
					// Get the first valid address book
					for(CAdbkList::node_list::iterator iter = alist->GetChildren()->begin(); iter != alist->GetChildren()->end(); iter++)
					{
						if ((*iter)->IsSelectable())
						{
							adbk = (*iter)->GetSelectData();
							break;
						}
					}
				}
			}
		}

		// If no default address book - warn the user
		if (adbk == NULL)
		{
			CErrorHandler::PutStopAlertRsrc("Alerts::Adbk::MissingCaptureAdbk");
			return;
		}

		// Do user choice if more than one
		CAddressList add_addrs;
		if ((addrs.size() > 1) && CPreferences::sPrefs->mCaptureAllowChoice.GetValue())
		{
			cdstrvect txtaddrs;
			for(CAddressList::iterator iter = addrs.begin(); iter != addrs.end() ; iter++)
				txtaddrs.push_back((*iter)->GetFullAddress());

			// Display list to user
			ulvector selection;
			if (CTextListChoice::PoseDialog("Alerts::Letter::AddressDisplay_Title", "Alerts::Letter::AddressDisplay_Desc", NULL, false, false, false, true,
												txtaddrs, cdstring::null_str, selection, NULL))
			{
				for(ulvector::const_iterator iter = selection.begin(); iter != selection.end(); iter++)
					add_addrs.push_back(new CAdbkAddress(*addrs.at(*iter)));
			}
		}
		else
		{
			for(CAddressList::iterator iter = addrs.begin(); iter != addrs.end() ; iter++)
				add_addrs.push_back(new CAdbkAddress(**iter));
		}

		// Look at each address
		for(CAddressList::iterator iter = add_addrs.begin(); iter != add_addrs.end() ; iter++)
		{
			auto_ptr<CAdbkAddress> test(new CAdbkAddress(**iter));

			// Check to see whether it already exists
			cdstring email = test->GetMailAddress();
			
			// See if it exists
			CAddressList found;
			adbk->SearchAddress(email, CAdbkAddress::eMatchExactly, CAdbkAddress::eEmail, found);
			
			// If its empty then do capture
			if (found.empty())
			{
				// See if user wanted to edit it first
				if (CPreferences::sPrefs->mCaptureAllowEdit.GetValue())
				{
					// If edit is cancelled do not capture it
					if (!CEditAddressDialog::PoseDialog(test.get()))
						continue;
				}

				// If there is no name, use the email address as the name
				if (test->GetName().empty())
					test->SetName(email);
					
				// Now add to address book (which takes control of the address object) in sorted order
				adbk->AddAddress(test.release(), true);
			}
		}
		
	}
	catch(...)
	{
		CLOG_LOGCATCH(...);
		
		// Flag error and ignore exception as capture should not cause the caller to fail
		CErrorHandler::PutStopAlertRsrc("Alerts::Adbk::FailedCaptureAdbk");
	}
}

// Extract address captures
void CAddressBookManager::ExtractCaptureAddress(const CMessage& msg, CAddressList& addrs)
{
	// Make sure envelope exists
	if (!msg.GetEnvelope())
		return;

	// Get the From addresses
	if (CPreferences::sPrefs->mCaptureFrom.GetValue())
		addrs.MergeList(msg.GetEnvelope()->GetFrom());
	
	// Get the Cc addresses
	if (CPreferences::sPrefs->mCaptureCc.GetValue())
		addrs.MergeList(msg.GetEnvelope()->GetCC());
	
	// Get the Reply-to addresses
	if (CPreferences::sPrefs->mCaptureReplyTo.GetValue() && !msg.GetEnvelope()->GetReplyTo()->empty())
		addrs.MergeList(msg.GetEnvelope()->GetReplyTo());
	else
		addrs.MergeList(msg.GetEnvelope()->GetFrom());
	
	// Get the To addresses
	if (CPreferences::sPrefs->mCaptureTo.GetValue())
		addrs.MergeList(msg.GetEnvelope()->GetTo());
}

#pragma mark ____________________________Disconnected

void CAddressBookManager::GoOffline(bool force, bool sync, bool fast)
{
	// Do sync of auto sync address books if not being forced off
	if (!force && sync)
		DoOfflineSync(fast);

	// Go offline on each protocol
	for(CAdbkProtocolList::iterator iter = mProtos.begin(); iter != mProtos.end(); iter++)
	{
		// Convert allowed protos to disconnected state
		if ((*iter)->CanDisconnect())
			(*iter)->GoOffline();
		else if (!(*iter)->IsOffline())
		{
			// This must fail silently
			try
			{
				// Other network protos must be logged out
				(*iter)->Close();
			}
			catch (...)
			{
				CLOG_LOGCATCH(...);
			}
		}
	}

	// Force visual update
	UpdateWindows();
}

void CAddressBookManager::DoOfflineSync(bool fast)
{
	// NB Must fail silently - no throws out of here
	
	try
	{
		// Go offline on each protocol
		for(CAdbkProtocolList::iterator iter = mProtos.begin(); iter != mProtos.end(); iter++)
		{
			// Convert allowed protos to disconnected state
			if ((*iter)->CanDisconnect())
				(*iter)->SynchroniseRemoteAll(fast);
		}
	}
	catch (...)
	{
		CLOG_LOGCATCH(...);
	}
}

void CAddressBookManager::GoOnline(bool sync)
{
	// Go online on each protocol
	for(CAdbkProtocolList::iterator iter = mProtos.begin(); iter != mProtos.end(); iter++)
	{
		// Only do allowed accounts
		if ((*iter)->CanDisconnect())
		{
			if (sync && (*iter)->IsLoggedOn())
				(*iter)->DoPlayback();
			(*iter)->GoOnline();
		}
		else if ((*iter)->GetAccount()->GetLogonAtStart())
			// Allow start to throw
			StartProtocol(*iter, false);
	}

	// Force visual update
	UpdateWindows();
}

#pragma mark ____________________________Sleep

void CAddressBookManager::Suspend()
{
	// Nothing to do for now
}

void CAddressBookManager::Resume()
{
	// Nothing to do for now
}
