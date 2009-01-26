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

#ifndef __CADBKPROTOCOL__MULBERRY__
#define __CADBKPROTOCOL__MULBERRY__

// CAdbkProtocol: Handles quotas for all resources

#include "CINETProtocol.h"
#include "CAdbkList.h"
#include "CAdbkRecord.h"
#include "CAddressAccount.h"

// Classes
class CACL;
class CAdbkClient;
class CAddressBook;

class CAdbkProtocol : public CINETProtocol
{
	friend class CAddressBook;
	friend class CRemoteAddressBook;
	friend class CAdbkRecord;

public:
	// Flags
	enum EAdbkFlags
	{
		eACLDisabled = 		1L << 16
	};

	// Messages for broadcast
	enum
	{
		eBroadcast_ClearList = 'apcl',
		eBroadcast_RefreshList = 'aprl'
	};

			CAdbkProtocol(CINETAccount* account);
			CAdbkProtocol(const CAdbkProtocol& copy,
							bool force_local = false,
							bool force_remote = false);
	virtual ~CAdbkProtocol();

	virtual void	CreateClient();
	virtual void	CopyClient(const CINETProtocol& copy);
	virtual void	RemoveClient();

	virtual void	Logoff();							// Logoff from protocol server
	virtual void	Forceoff();							// Forced close

	// Setters/Getters
	virtual CAdbkList*	GetAdbkList()					// Return the adbk list
		{ return mAdbkList; }

	virtual void SetDescriptor(const char* desc);			// Set list title

	virtual const CAddressAccount* GetAddressAccount() const
		{ return static_cast<const CAddressAccount*>(GetAccount()); }
	virtual CAddressAccount* GetAddressAccount()
		{ return static_cast<CAddressAccount*>(GetAccount()); }

	virtual char GetSeparator() const;
	virtual const char* GetUserPrefix() const;

	// Account
	virtual void SetAccount(CINETAccount* account);

	// Offline
	virtual void	InitDisconnect();
	virtual const cdstring& GetOfflineDefaultDirectory();
	virtual void	GoOffline();
	virtual void	GoOnline();
			void	SynchroniseRemoteAll(bool fast);
			bool	DoPlayback();
	virtual void	SetSynchronising();						// Set into synchronising mode

	// Handle adbks
	
	// A D B K L I S T
			void	LoadList();							// Load adbk list from server
			void	SyncList();							// Sync adbks with prefs options
			void	RefreshList();						// Refresh adbk list from server

			CAddressBook*	FindAddressBook(const char* name);	// FInd address book with matching name

	// A D B K S
			void	AddAdbk(CAddressBook* adbk);			// Add adbk to protocol list
			void	CreateAdbk(CAddressBook* adbk);			// Create a new adbk
			void	TouchAdbk(CAddressBook* adbk);			// Touch adbk
			bool	TestAdbk(CAddressBook* adbk);			// Test adbk
			void	DeleteAdbk(CAddressBook* adbk);			// Delete adbk
			void	RenameAdbk(CAddressBook* adbk,
								const char* adbk_new);		// Rename adbk

			void	OpenAdbk(CAddressBook* adbk);			// Open adbk on server
			void	AdbkChanged(CAddressBook* adbk);		// State of adbk changed
			void	CloseAdbk(CAddressBook* adbk);			// Close existing mailbox

			void	AddAddress(CAddressBook* adbk,
								CAddressList* addrs);		// Add addresses
			void	AddGroup(CAddressBook* adbk,
								CGroupList* grps);			// Add groups
			void	ChangeAddress(CAddressBook* adbk,
								CAddressList* addrs);		// Change addresses
			void	ChangeGroup(CAddressBook* adbk,
								CGroupList* grps);			// Change groups
			void	RemoveAddress(CAddressBook* adbk,
								CAddressList* addrs);		// Remove addresses
			void	RemoveGroup(CAddressBook* adbk,
								CGroupList* grps);			// Remove groups

			void	ResolveAddress(CAddressBook* adbk,
									const char* nick_name,
									CAdbkAddress*& addr);	// Resolve address nick-name
			void	ResolveGroup(CAddressBook* adbk,
									const char* nick_name,
									CGroup*& grp);			// Resolve group nick-name

			void	SearchAddress(CAddressBook* adbk,
									const cdstring& name,
									CAdbkAddress::EAddressMatch match,
									CAdbkAddress::EAddressField field,
									CAddressList& addr_list);	// Do search

	// ACLs
			void SetACLDisabled(bool disabled)
		{ mFlags.Set((EFlags) eACLDisabled, disabled); }
			bool GetACLDisabled()
		{ return mFlags.IsSet(eACLDisabled); }

			bool UsingACLs()							// Indicates that client can use ACLs
		{ return !GetACLDisabled(); }

			void SetACL(CAddressBook* adbk, CACL* acl);			// Set acl on server
			void DeleteACL(CAddressBook* adbk, CACL* acl);		// Delete acl on server
			void GetACL(CAddressBook* adbk);					// Get all acls for mailbox from server
			void MyRights(CAddressBook* adbk);					// Get current user's rights to mailbox

protected:
	CAdbkClient*		mClient;							// Its client
	CAdbkList*			mAdbkList;							// Hierarchic address book list

	CAdbkRecord*		mRecorder;							// Recorder for mail ops

	// Offline
			void DumpAddressBooks();
			void ReadAddressBooks();
			void RecoverAddressBooks();
	
			void SynchroniseRemote(CAddressBook* adbk, bool fast);
			void SyncRemote(CAddressBook* remote, CAddressBook* local, bool fast);

			void ClearDisconnect(CAddressBook* adbk);

};

#endif
