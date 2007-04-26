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


// CAdbkProtocol

#include "CAdbkProtocol.h"

#include "CACAPClient.h"
#include "CActionManager.h"
#include "CAdbkClient.h"
#include "CAdbkList.h"
#include "CAddressBookManager.h"
#include "CConnectionManager.h"
#include "CIMSPClient.h"
#include "CLocalAdbkClient.h"
#if __dest_os == __mac_os || __dest_os == __mac_os_x
#if PP_Target_Carbon
#include "CMacOSXAdbkClient.h"
#endif
#endif
#include "CPreferences.h"
#include "CRemoteAddressBook.h"

#include "cdfstream.h"

// CAdbkProtocol: Handles quotas for all resources

// Constructor
CAdbkProtocol::CAdbkProtocol(CINETAccount* account)
	: CINETProtocol(account)
{
	mClient = NULL;
	mAdbkList = new CAdbkList(false);

	// Only IMSP/ACAP servers can disconnect
	switch(GetAccountType())
	{
	case CINETAccount::eIMSP:
	case CINETAccount::eACAP:
		SetFlags(eCanDisconnect, IsOfflineAllowed() &&
									GetAddressAccount()->GetDisconnected());
		break;
	default:
		SetFlags(eCanDisconnect, false);
		SetACLDisabled(true);
		break;
	}

	mRecorder = NULL;

	CreateClient();
}

// Copy constructor
CAdbkProtocol::CAdbkProtocol(const CAdbkProtocol& copy, bool force_local, bool force_remote)
	: CINETProtocol(copy)
{
	// Init instance variables
	mClient = NULL;
	mAdbkList = new CAdbkList(false);

	mRecorder = NULL; 				// Only original proto uses this - clones do not

	// Copy client
	if (((GetAccountType() == CINETAccount::eIMSP) ||
		 (GetAccountType() == CINETAccount::eACAP))
		&& (force_local || force_remote))
	{
		if (force_remote)
		{
			SetFlags(eIsOffline, false);
			SetFlags(eDisconnected, false);
			switch(GetAccountType())
			{
			case CINETAccount::eIMSP:
				mClient = new CIMSPClient(NULL, this);
				break;
			case CINETAccount::eACAP:
				mClient = new CACAPClient(NULL, this);
				break;
			default:;
			}
		}
		else
		{
			SetFlags(eIsOffline, true);
			SetFlags(eDisconnected, true);
			InitDisconnect();
			mClient = new CLocalAdbkClient(this);
			static_cast<CLocalAdbkClient*>(mClient)->SetRecorder(mRecorder);
		}
		CINETProtocol::mClient = mClient;
	}
	else
		CopyClient(copy);

}

// Default destructor
CAdbkProtocol::~CAdbkProtocol()
{
	// Delete client
	RemoveClient();

	delete mAdbkList;
	mAdbkList = NULL;

	// Delete recorder
	delete mRecorder;
	mRecorder = NULL;

}

void CAdbkProtocol::SetAccount(CINETAccount* account)
{
	// Do inherited
	CINETProtocol::SetAccount(account);
	
	// Check disconnected state
	switch(GetAccountType())
	{
	case CINETAccount::eIMSP:
	case CINETAccount::eACAP:
		SetFlags(eCanDisconnect, IsOfflineAllowed() &&
									GetAddressAccount()->GetDisconnected());
		break;
	default:
		SetFlags(eCanDisconnect, false);
		break;
	}

}

void CAdbkProtocol::CreateClient()
{
	RemoveClient();

	// Check disconnected state
	switch(GetAccountType())
	{
	case CINETAccount::eIMSP:
	case CINETAccount::eACAP:
		if (CConnectionManager::sConnectionManager.IsConnected() || !CanDisconnect())
		{
			SetFlags(eIsOffline, false);
			SetFlags(eDisconnected, false);
			switch(GetAccountType())
			{
			case CINETAccount::eIMSP:
				mClient = new CIMSPClient(NULL, this);
				break;
			case CINETAccount::eACAP:
				mClient = new CACAPClient(NULL, this);
				break;
			default:;
			}
		}
		else
		{
			SetFlags(eIsOffline, true);
			SetFlags(eDisconnected, true);
			InitDisconnect();
			mClient = new CLocalAdbkClient(this);
			static_cast<CLocalAdbkClient*>(mClient)->SetRecorder(mRecorder);
		}
		break;
	case CINETAccount::eLocalAdbk:
		SetFlags(eIsOffline, true);
		SetFlags(eDisconnected, false);
		InitOffline();
		mClient = new CLocalAdbkClient(this);
		break;
#if __dest_os == __mac_os || __dest_os == __mac_os_x
#if PP_Target_Carbon
	case CINETAccount::eOSAdbk:
		SetFlags(eIsOffline, true);
		SetFlags(eDisconnected, false);
		InitOffline();
		mClient = new CMacOSXAdbkClient(this);
		break;
#endif
#endif
	default:;
	}

	CINETProtocol::mClient = mClient;
}

void CAdbkProtocol::CopyClient(const CINETProtocol& copy)
{
	// NB This is called in the constructor and cannot rely on virtual functions
	// being called for this object!
	CAdbkClient* copy_it = static_cast<const CAdbkProtocol&>(copy).mClient;
	if (dynamic_cast<CIMSPClient*>(copy_it))
		mClient = new CIMSPClient(static_cast<const CIMSPClient&>(*copy_it), NULL, this);
	else if (dynamic_cast<CACAPClient*>(copy_it))
		mClient = new CACAPClient(static_cast<const CACAPClient&>(*copy_it), NULL, this);
	else if (dynamic_cast<CLocalAdbkClient*>(copy_it))
		mClient = new CLocalAdbkClient(static_cast<const CLocalAdbkClient&>(*copy_it), this);
#if __dest_os == __mac_os || __dest_os == __mac_os_x
#if PP_Target_Carbon
	else if (dynamic_cast<CMacOSXAdbkClient*>(copy_it))
		mClient = new CMacOSXAdbkClient(static_cast<const CMacOSXAdbkClient&>(*copy_it), this);
#endif
#endif
	else
		mClient = NULL;

	CINETProtocol::mClient = mClient;
}

// Set into synchronising mode
void CAdbkProtocol::SetSynchronising()
{
	// Just remove recorder
	if (mRecorder)
	{
		// Remove from local client
		CLocalAdbkClient* client = dynamic_cast<CLocalAdbkClient*>(mClient);
		if (client)
			client->SetRecorder(NULL);

		delete mRecorder;
		mRecorder = NULL;
	}
}

void CAdbkProtocol::RemoveClient()
{
	delete mClient;
	mClient = NULL;
	CINETProtocol::mClient = NULL;
}

// Logoff from protocol server
void CAdbkProtocol::Logoff()
{
	if (IsLoggedOn())
	{
		try
		{
			// Tell listeners we are about to remove all address books
			// Do this BEFORE logging out so that any pending address book changes can be committed
			// whilst connection is still in place
			Broadcast_Message(eBroadcast_ClearList, this);

			// Do inherited
			CINETProtocol::Logoff();

			// Remove address books
			mAdbkList->erase_children();
		}
		catch (...)
		{
			CLOG_LOGCATCH(...);

			// Tell listeners we are about to remove all address books
			Broadcast_Message(eBroadcast_ClearList, this);

			// Always remove address books
			mAdbkList->erase_children();

			CLOG_LOGRETHROW;
			throw;
		}
	}
}

// Force off from protocol server
void CAdbkProtocol::Forceoff()
{
	// Do inherited
	CINETProtocol::Forceoff();

	// Tell listeners we are about to remove all address books
	Broadcast_Message(eBroadcast_ClearList, this);

	// Remove address books
	mAdbkList->erase_children();
}

// Set list title
void CAdbkProtocol::SetDescriptor(const char* desc)
{
	cdstring* name = new cdstring(GetAccountName());
	mAdbkList->SetData(name);

	CINETProtocol::SetDescriptor(desc);
}

char CAdbkProtocol::GetSeparator() const
{
	switch(GetAccountType())
	{
	case CINETAccount::eLocalAdbk:
	case CINETAccount::eOSAdbk:
	default:
		return os_dir_delim;
	case CINETAccount::eIMSP:
		return '.';
	case CINETAccount::eACAP:
		return '/';
	}
}

const char* CAdbkProtocol::GetUserPrefix() const
{
	switch(GetAccountType())
	{
	case CINETAccount::eIMSP:
	case CINETAccount::eLocalAdbk:
	case CINETAccount::eOSAdbk:
	default:
		return cdstring::null_str;
	case CINETAccount::eACAP:
		return "user/";
	}
}

#pragma mark ____________________________Adbks

// Load adbk list from server
void CAdbkProtocol::LoadList()
{
	// Check whether connected or not
	if (IsDisconnected())
		ReadAddressBooks();
	else
	{
		mClient->_FindAllAdbks("");

		// Look for default address book if remote
		if (!IsOffline())
		{
			bool has_default = false;

			// Get default name
			cdstring default_name;
			if (GetAccountType() == CINETAccount::eACAP)
				default_name += "user/";
			default_name += GetAccount()->GetAuthenticator().GetAuthenticator()->GetActualUID();

			// Scan abooks for default
			if (mAdbkList->HasChildren())
				for(CAdbkList::node_list::const_iterator niter = mAdbkList->GetChildren()->begin();
					!has_default && (niter != mAdbkList->GetChildren()->end()); niter++)
				{
					if ((*niter)->IsSelectable() && ((*niter)->GetSelectData()->GetName() == default_name))
						has_default = true;
				}

			// Create default entry
			if (!has_default)
			{
				// Add adress book to list
				CRemoteAddressBook* adbk = new CRemoteAddressBook(this, default_name);
				
				// Policy:
				//   IMSP: create a dummy entry in default address book then delete it
				//   ACAP: just create the default address book
				switch(GetAccountType())
				{
				case CINETAccount::eIMSP:
				{
					GetAdbkList()->push_back(adbk, false);

					// Now force real creation of address book by storing fake address
					CAddressList list;
					list.push_back(new CAdbkAddress("a0001", default_name, default_name));
					AddAddress(adbk, &list);
					RemoveAddress(adbk, &list);
					break;
				}
				case CINETAccount::eACAP:
					CreateAdbk(adbk);
					GetAdbkList()->push_back(adbk, true);
					break;
				default:;
				}

				// Make sure address book is added to open on startup, nick-names and searching
				adbk->SetFlags(CAddressBook::eOpenOnStart, true);
				adbk->SetFlags(CAddressBook::eLookup, true);
				adbk->SetFlags(CAddressBook::eSearch, true);

				// Change prefs list
				CAddressBookManager::sAddressBookManager->SyncAddressBook(adbk, true);
				CPreferences::sPrefs->ChangeAddressBookOpenOnStart(adbk, true);
				CPreferences::sPrefs->ChangeAddressBookLookup(adbk, true);
				CPreferences::sPrefs->ChangeAddressBookSearch(adbk, true);
			}
		}
	}
}


// Sync adbks with prefs options
void CAdbkProtocol::SyncList()
{
	// Only if its not empty
	if (!mAdbkList->GetChildren())
		return;

	// Look for matching address book
	for(CAdbkList::node_list::const_iterator niter = mAdbkList->GetChildren()->begin();
		niter != mAdbkList->GetChildren()->end(); niter++)
	{
		if ((*niter)->IsSelectable())
		{
			CAddressBook* adbk = (*niter)->GetSelectData();

			// Get adbk URL
			cdstring adbk_url = adbk->GetURL();

			// Match in open at start list
			cdstrvect::const_iterator found = ::find(CPreferences::sPrefs->mAdbkOpenAtStartup.GetValue().begin(),
													 CPreferences::sPrefs->mAdbkOpenAtStartup.GetValue().end(),
													 adbk_url);

			if (found != CPreferences::sPrefs->mAdbkOpenAtStartup.GetValue().end())
			{
				adbk->SetFlags(CAddressBook::eOpenOnStart, true);

				// Open it if its not already open
				if (!adbk->IsOpen())
					CActionManager::OpenAddressBook(adbk, true);
			}

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

			// Match in auto sync list
			found = ::find(CPreferences::sPrefs->mAdbkAutoSync.GetValue().begin(),
							CPreferences::sPrefs->mAdbkAutoSync.GetValue().end(),  adbk_url);

			if (found != CPreferences::sPrefs->mAdbkAutoSync.GetValue().end())
				adbk->SetFlags(CAddressBook::eAutoSync, true);

			// Force update of manager
			CAddressBookManager::sAddressBookManager->SyncAddressBook(adbk, true);
		}
	}

	Broadcast_Message(eBroadcast_RefreshList, this);
}

// Refresh adbk list from server
void CAdbkProtocol::RefreshList()
{
	// Tell listeners we are about to remove all address books
	Broadcast_Message(eBroadcast_ClearList, this);

	// Remove address books
	mAdbkList->erase_children();
	
	// Reload and sync
	LoadList();
	SyncList();
}

// FInd address book with matching name
CAddressBook* CAdbkProtocol::FindAddressBook(const char* name)
{
	// Write out each address book
	if (mAdbkList && mAdbkList->HasChildren())
	{
		for(CAdbkList::node_list::const_iterator niter = mAdbkList->GetChildren()->begin();
			niter != mAdbkList->GetChildren()->end(); niter++)
		{
			if ((*niter)->IsSelectable())
			{
				const CAddressBook* adbk = (*niter)->GetSelectData();
				if (adbk->GetName() == name)
					return const_cast<CAddressBook*>(adbk);
			}
		}
	}

	return NULL;
}

// Create a new adbk
void CAdbkProtocol::CreateAdbk(CAddressBook* adbk)
{
	// Do create action
	mClient->_CreateAdbk(adbk);
}

// Touch mbox on server
void CAdbkProtocol::TouchAdbk(CAddressBook* adbk)
{
	// Do touch action
	mClient->_TouchAdbk(adbk);

	// If disconnected set flag
	if (IsDisconnected())
		// Now convert to local adbk
		adbk->SetFlags(CAddressBook::eCachedAdbk);
}

// Test mbox on server
bool CAdbkProtocol::TestAdbk(CAddressBook* adbk)
{
	// Do test action
	return mClient->_TestAdbk(adbk);

}

// Delete adbk
void CAdbkProtocol::DeleteAdbk(CAddressBook* adbk)
{
	// Do delete action
	mClient->_DeleteAdbk(adbk);
}

// Rename adbk
void CAdbkProtocol::RenameAdbk(CAddressBook* adbk, const char* adbk_new)
{
	// Do rename action
	mClient->_RenameAdbk(adbk, adbk_new);
}

// Open adbk on server
void CAdbkProtocol::OpenAdbk(CAddressBook* adbk)
{
	// Disconnected touch
	if (IsDisconnected())
		TouchAdbk(adbk);

	mClient->_FindAllAddresses(adbk);
}

// State of adbk changed
void CAdbkProtocol::AdbkChanged(CAddressBook* adbk)
{
}

// Close existing address book
void CAdbkProtocol::CloseAdbk(CAddressBook* adbk)
{
}

// Add address
void CAdbkProtocol::AddAddress(CAddressBook* adbk, CAddressList* addrs)
{
	mClient->_StoreAddress(adbk, addrs);
}

// Add group
void CAdbkProtocol::AddGroup(CAddressBook* adbk, CGroupList* grps)
{
	mClient->_StoreGroup(adbk, grps);
}

// Change address
void CAdbkProtocol::ChangeAddress(CAddressBook* adbk, CAddressList* addrs)
{
	mClient->_ChangeAddress(adbk, addrs);
}

// Change group
void CAdbkProtocol::ChangeGroup(CAddressBook* adbk, CGroupList* grps)
{
	mClient->_ChangeGroup(adbk, grps);
}

// Remove address
void CAdbkProtocol::RemoveAddress(CAddressBook* adbk, CAddressList* addrs)
{
	mClient->_DeleteAddress(adbk, addrs);
}

// Remove group
void CAdbkProtocol::RemoveGroup(CAddressBook* adbk, CGroupList* grps)
{
	mClient->_DeleteGroup(adbk, grps);
}

#pragma mark ____________________________Lookup

// Resolve address nick-name
void CAdbkProtocol::ResolveAddress(CAddressBook* adbk, const char* nick_name, CAdbkAddress*& addr)
{
	mClient->_ResolveAddress(adbk, nick_name, addr);
}

// Resolve group nick-name
void CAdbkProtocol::ResolveGroup(CAddressBook* adbk, const char* nick_name, CGroup*& grp)
{
	mClient->_ResolveGroup(adbk, nick_name, grp);
}

// Do search
void CAdbkProtocol::SearchAddress(CAddressBook* adbk,
									const cdstring& name,
									CAdbkAddress::EAddressMatch match,
									CAdbkAddress::EAddressField field,
									CAddressList& addr_list)
{
	mClient->_SearchAddress(adbk, name, match, field, addr_list);
}

#pragma mark ____________________________ACLs

// Set new value on server
void CAdbkProtocol::SetACL(CAddressBook* adbk, CACL* acl)
{
	// Do nothing if not available
	if (!UsingACLs())
		return;

	// Send command to server
	// Adbk will take care of adding/replacing in list if successful
	mClient->_SetACL(adbk, acl);
}

// Delete value on server
void CAdbkProtocol::DeleteACL(CAddressBook* adbk, CACL* acl)
{
	// Do nothing if not available
	if (!UsingACLs())
		return;

	// Send command to server
	// Adbk will take care of adding/replacing in list if successful
	mClient->_DeleteACL(adbk, acl);
}

// Get values on server
void CAdbkProtocol::GetACL(CAddressBook* adbk)
{
	// Do nothing if not available
	if (!UsingACLs())
		return;

	// Send command to server
	// Adbk will take care of recovery
	mClient->_GetACL(adbk);
}

// List user's rights on server
void CAdbkProtocol::MyRights(CAddressBook* adbk)
{
	// Do nothing if not available
	if (!UsingACLs())
		return;

	// Send command to server
	// Adbk will take care of recovery
	mClient->_MyRights(adbk);
}

#pragma mark ____________________________Disconnected

void CAdbkProtocol::InitDisconnect()
{
	// Do inherited to setup directory
	CINETProtocol::InitDisconnect();

	// Now create recorder
	if (!mRecorder)
	{
		mRecorder = new CAdbkRecord;
		mRecorder->SetDescriptor(mOfflineCWD + "Record.mbr");
		mRecorder->SetFullRecord();
	}
	if (mRecorder)
		mRecorder->Open();
}

const cdstring& CAdbkProtocol::GetOfflineDefaultDirectory()
{
	static cdstring name("Address Books");
	return name;
}

void CAdbkProtocol::GoOffline()
{
	// Check that offline directory exists
	if (mOfflineCWD.empty())
		InitOffline();

	// Dump current addressbooks list to disk
	DumpAddressBooks();

	// Now do inherited connection switch
	CINETProtocol::GoOffline();
	
	// Must reload/sync list
	if (IsLoggedOn())
	{
		LoadList();
		SyncList();
	}
}

void CAdbkProtocol::GoOnline()
{
	// Reset state of address books
	RecoverAddressBooks();

	// Now do inherited connection switch
	CINETProtocol::GoOnline();
	
	// Must reload/sync list
	if (IsLoggedOn())
	{
		LoadList();
		SyncList();
	}
}

void CAdbkProtocol::DumpAddressBooks()
{
	cdstring list_name = mOfflineCWD + "adbklist";
	cdofstream dump(list_name);

	// Write out each address book
	if (mAdbkList && mAdbkList->HasChildren())
	{
		// Create local clone
		auto_ptr<CAdbkProtocol> clone(new CAdbkProtocol(*this, true));
		clone->SetSynchronising();

		try
		{
			clone->Open();

			for(CAdbkList::node_list::iterator niter = mAdbkList->GetChildren()->begin();
				niter != mAdbkList->GetChildren()->end(); niter++)
			{
				if ((*niter)->IsSelectable())
				{
					CAddressBook* adbk = (*niter)->GetSelectData();
					dump << adbk->GetName() << endl;
					dump << 1 << endl;
					
					adbk->SwitchDisconnect(clone.get());
				}
				else
				{
					const cdstring* name = (*niter)->GetNoSelectData();
					dump << name << endl;
					dump << 0 << endl;
				}
			}
		}
		catch (...)
		{
			CLOG_LOGCATCH(...);

			clone->Close();
			CLOG_LOGRETHROW;
			throw;
		}

		clone->Close();
	}

	// Always end with blank line
	dump << endl;
}

void CAdbkProtocol::ReadAddressBooks()
{
	cdstring list_name = mOfflineCWD + "adbklist";
	cdifstream in(list_name);
	if (in.fail())
		return;

	// Read in each hierarchy
	while(true)
	{
		// Get addressbook name
		cdstring adbk_name;
		getline(in, adbk_name);

		// Check for end of list
		if (adbk_name.empty())
			break;

		// Get flags
		unsigned long flags;
		in >> flags;
		in.ignore();

		if (flags)
		{
			// Add adress book to list
			CRemoteAddressBook* adbk = new CRemoteAddressBook(this, adbk_name);
			mAdbkList->push_back(adbk, false);
			adbk->SwitchDisconnect(this);
		}
		else
		{
			// Add name only to list
			cdstring* hier_name = new cdstring(adbk_name);
			mAdbkList->push_back(hier_name, false);
		}
	}
}

void CAdbkProtocol::RecoverAddressBooks()
{
	if (mAdbkList && mAdbkList->HasChildren())
	{
		for(CAdbkList::node_list::iterator niter = mAdbkList->GetChildren()->begin();
			niter != mAdbkList->GetChildren()->end(); niter++)
		{
			if ((*niter)->IsSelectable())
			{
				CAddressBook* adbk = (*niter)->GetSelectData();
				adbk->SwitchDisconnect(NULL);
			}
		}
	}
}

void CAdbkProtocol::SynchroniseRemoteAll(bool fast)
{
	// Sync each address book set for nick-name resolution
	if (mAdbkList && mAdbkList->HasChildren())
	{
		for(CAdbkList::node_list::iterator niter = mAdbkList->GetChildren()->begin();
			niter != mAdbkList->GetChildren()->end(); niter++)
		{
			if ((*niter)->IsSelectable())
			{
				CAddressBook* adbk = (*niter)->GetSelectData();
				if (adbk->IsAutoSync())
					SynchroniseRemote(adbk, fast);
			}
		}
	}
}

void CAdbkProtocol::SynchroniseRemote(CAddressBook* adbk, bool fast)
{
	// Only if possible
	if (!CanDisconnect())
		return;

	// For each addressbook being synchronised do this:
	//
	// Clone this but force it to local
	// Create a temp address book object and give it the local proto
	// Sync address book from this proto to temp one

	// Create local clone
	auto_ptr<CAdbkProtocol> clone(new CAdbkProtocol(*this, true));
	clone->SetSynchronising();

	try
	{
		clone->Open();

		// Create copy of address book
		auto_ptr<CRemoteAddressBook> temp(new CRemoteAddressBook(static_cast<CRemoteAddressBook&>(*adbk)));

		// Local address book is given local protocol
		temp->SetProtocol(clone.get());

		// Make sure local item exists
		clone->TouchAdbk(temp.get());

		// Now sync them both
		SyncRemote(adbk, temp.get(), fast);
	}
	catch (...)
	{
		CLOG_LOGCATCH(...);

		clone->Close();
		CLOG_LOGRETHROW;
		throw;
	}

	clone->Close();
}

void CAdbkProtocol::SyncRemote(CAddressBook* remote, CAddressBook* local, bool fast)
{
	// If fast then only sync new messages in remote store
	// If uids then only sync specific messages in remote store

	bool remote_loaded = remote->IsLoaded();
	remote->SetFlags(CAddressBook::eSynchronising);

	try
	{
		// Make sure remote has all addresses available
		if (!remote_loaded)
			remote->Read();
		
		// Empty out addresses from local
		local->SetFlags(CAddressBook::eSynchronising);
		local->Empty();
		
		// Copy all addresses
		remote->CopyAll(local);
	}
	catch (...)
	{
		CLOG_LOGCATCH(...);

	}

	// Make sure remote is cleared
	if (!remote_loaded)
		remote->Clear();

	remote->SetFlags(CAddressBook::eSynchronising, false);
	local->SetFlags(CAddressBook::eSynchronising, false);
}

void CAdbkProtocol::ClearDisconnect(CAddressBook* adbk1)
{
	CRemoteAddressBook* adbk = static_cast<CRemoteAddressBook*>(adbk1);

	// The addressbook is either remote or local
	if (adbk->GetProtocol()->IsDisconnected())
	{
		// Only bother if cache actually exists
		if (!TestAdbk(adbk))
			return;

		// Don't record the delete/expunge operation!
		static_cast<CLocalAdbkClient*>(mClient)->SetRecorder(NULL);
		try
		{
			DeleteAdbk(adbk);
			
			// Remove cached flag
			adbk->SetFlags(CAddressBook::eCachedAdbk, false);
		}
		catch (...)
		{
			CLOG_LOGCATCH(...);

			static_cast<CLocalAdbkClient*>(mClient)->SetRecorder(mRecorder);
			CLOG_LOGRETHROW;
			throw;
		}
		static_cast<CLocalAdbkClient*>(mClient)->SetRecorder(mRecorder);
	}
	else
	{
		// Create local clone
		auto_ptr<CAdbkProtocol> clone(new CAdbkProtocol(*this, true));
		clone->SetSynchronising();

		try
		{
			clone->Open();

			// Create copy of address book
			auto_ptr<CRemoteAddressBook> temp(new CRemoteAddressBook(*adbk));

			// Local address book is given local protocol
			temp->SetProtocol(clone.get());

			// Only bother if local item exists
			if (clone->TestAdbk(temp.get()))
				clone->DeleteAdbk(temp.get());
		}
		catch (...)
		{
			CLOG_LOGCATCH(...);

			clone->Close();
			CLOG_LOGRETHROW;
			throw;
		}

		clone->Close();
	}
}

bool CAdbkProtocol::DoPlayback()
{
	// Ignore if no defined recorder
	if (!mRecorder)
		return true;

	// Create remote clone
	auto_ptr<CAdbkProtocol> clone(new CAdbkProtocol(*this, false, true));
	clone->SetSynchronising();

	// Prevent further recording
	mRecorder->SetNoRecord();

	bool result = mRecorder->Playback(clone.get(), this);

	// Enable recording
	mRecorder->SetFullRecord();
	
	return result;
}
