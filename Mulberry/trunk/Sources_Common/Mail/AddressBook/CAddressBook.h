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


// CAddressBook.h

// Class that encapsulates address book functionality (local and remote).
// This is an abstract base class and should be derived for each type
// of address book (eg local, remote IMSP-style, remote-LDAP etc)

#ifndef __CADDRESSBOOK__MULBERRY__
#define __CADDRESSBOOK__MULBERRY__

#include "CAdbkACL.h"
#include "CAdbkList.h"
#include "CAdbkAddress.h"
#include "CAddressList.h"
#include "CGroupList.h"

#include "SBitFlags.h"

#include "cdstring.h"
#include "cdmutex.h"

// Classes

class CAdbkProtocol;

class CAddressBook
{
public:
	enum EFlags
	{
		// Adbk status
		eNone = 					0,
		eReadOnly =					1L << 1,
		eOpen =						1L << 2,
		eLoaded =					1L << 3,
		eOpenOnStart =				1L << 4,
		eLookup =					1L << 5,
		eSearch =					1L << 6,
		eAdd =						1L << 7,
		eAutoSync =					1L << 8,
		eDeleteOnClose =			1L << 9,
		eLocalAdbk =				1L << 10,
		eCachedAdbk =				1L << 11,
		eSynchronising =			1L << 12		// Addressbook is being synchronised
		
	};

public:

	static cdmutex _mutex;										// Used for locks

		CAddressBook()
			{ InitAddressBook(); }
		explicit CAddressBook(const char* name)
			{ InitAddressBook(); mAdbkName = name; }
		CAddressBook(const CAddressBook& copy);

	virtual ~CAddressBook();

	// Getters
	const cdstring& GetName() const
		{ return mAdbkName; }
	void SetName(const char* name);

	virtual cdstring GetAccountName() const = 0;
	virtual cdstring GetURL(bool full = false) const = 0;

	CAddressList* GetAddressList()
		{ return &mAddresses; }
	CGroupList* GetGroupList()
		{ return &mGroups; }

	void	SetFlags(EFlags new_flags, bool add = true)			// Set flags
		{ mFlags.Set(new_flags, add); }
	EFlags	GetFlags() const								// Get flags
		{ return (EFlags) mFlags.Get(); }

	bool IsOpen()
		{ return mFlags.IsSet(eOpen); }
	bool IsLoaded()
		{ return mFlags.IsSet(eLoaded); }
	bool	IsOpenOnStart() const
		{ return mFlags.IsSet(eOpenOnStart); }
	bool	IsLookup() const
		{ return mFlags.IsSet(eLookup); }
	bool	IsSearch() const
		{ return mFlags.IsSet(eSearch); }
	bool	IsAdd() const
		{ return mFlags.IsSet(eAdd); }
	bool	IsAutoSync() const
		{ return mFlags.IsSet(eAutoSync);}
	bool	IsLocalAdbk() const
		{ return mFlags.IsSet(eLocalAdbk);}
	bool	IsCachedAdbk() const
		{ return mFlags.IsSet(eCachedAdbk);}
	bool	IsSynchronising() const
		{ return mFlags.IsSet(eSynchronising);}

	// Opening/closing
	virtual void	New();									// New visual address book on source
	virtual void	Open();									// Open visual address book from source
	virtual void	Read();									// Read in addresses
	virtual void	Save();									// Save addresses
	virtual void	Close();								// Close visual address book
	virtual void	Clear();								// Clear addresses

	// Manipulation
	virtual void	Rename(cdstring& new_name);				// Rename
	virtual void	Delete();								// Delete
	virtual void	Empty();								// Empty

	virtual void	CopyAll(CAddressBook* adbk);			// Copy addresses and groups to another address book

	virtual void	Synchronise(bool fast);					// Synchronise to local
	virtual void	ClearDisconnect();						// Clear disconnected cache
	virtual void	SwitchDisconnect(CAdbkProtocol* local);	// Switch into disconnected mode

	virtual CAdbkAddress* FindAddress(const char* name);
	virtual CAdbkAddress* FindAddress(const CAddress* addr);
	virtual CAdbkAddress* FindAddressEntry(const char* name);
	virtual CGroup* FindGroup(const char* name);
	virtual CGroup* FindGroupEntry(const char* name);

	// Adding/removing items
	virtual void	AddAddress(CAddress* addr, bool sorted = false);
	virtual void	AddAddress(CAddressList* addrs, bool sorted = false);
	virtual void	AddUniqueAddresses(CAddressList& add);							// Add unique addresses from list
	virtual void	UpdateAddress(CAddress* addr, bool sorted = false);				// Address changed
	virtual void	UpdateAddress(CAddressList* addrs, bool sorted = false) {}		// Address changed
	virtual void	UpdateAddress(CAddressList* old_addrs, CAddressList* new_addrs, bool sorted = false);		// Address changed
	virtual void	RemoveAddress(CAddress* addr);
	virtual void	RemoveAddress(CAddressList* addrs)
		{ if (IsLoaded()) mAddresses.RemoveAddress(addrs); }

	virtual void	AddGroup(CGroup* grp, bool sorted = false);
	virtual void	AddGroup(CGroupList* grps, bool sorted = false);
	virtual void	AddUniqueGroups(CGroupList& add);								// Add unique groups from list
	virtual void	UpdateGroup(CGroup* grp, bool sorted = false);					// Group changed
	virtual void	UpdateGroup(CGroupList* grps, bool sorted = false) {}			// Group changed
	virtual void	UpdateGroup(CGroupList* old_grps, CGroupList* new_grps, bool sorted = false);		// Address changed
	virtual void	RemoveGroup(CGroup* grp);
	virtual void	RemoveGroup(CGroupList* grps)
		{ if (IsLoaded()) mGroups.RemoveGroup(grps); }

	virtual void	MakeUniqueEntry(CAdbkAddress* addr) const;	// Give this address a unique entry

	// Reading/writing
	virtual void	ImportAddresses(char* txt);
	virtual void	ImportAddress(char* txt, bool add,
									CAdbkAddress** addr,
									CGroup** grp, bool add_entry = false);
	virtual char*	ExportAddress(const CAdbkAddress* addr) const;
	virtual char*	ExportGroup(const CGroup* grp) const;

	// Lookup
	virtual	bool	FindNickName(const char* nick_name,
										CAdbkAddress*& addr);	// Find address from nick-name

	virtual	bool	FindGroupName(const char* grp_name,
										CGroup*& grp);			// Find group from group-name

	virtual void	SearchAddress(const cdstring& name,
									CAdbkAddress::EAddressMatch match,
									CAdbkAddress::EAddressField field,
									CAddressList& addr_list);	// Do search

	// ACLs
	virtual void	SetMyRights(SACLRight rights)				// Set user's rights to this mailbox
		{ mMyRights = rights; }
	virtual SACLRight	GetMyRights() const
		{ return mMyRights; }
	virtual void	CheckMyRights() = 0;					// Get user's rights from server

	virtual void	AddACL(const CAdbkACL* acl) = 0;			// Add ACL to list
	virtual void	SetACL(CAdbkACL* acl) = 0;					// Set ACL on server
	virtual void	DeleteACL(CAdbkACL* acl) = 0;				// Delete ACL on server
	virtual CAdbkACLList*	GetACLs() const
		{ return mACLs; }
	virtual void	CheckACLs() = 0;						// Get ACLs from server

protected:
	cdstring			mAdbkName;		// Name of adbk
	SBitFlags			mFlags;			// Flags state
	CAddressList		mAddresses;
	CGroupList			mGroups;
	SACLRight			mMyRights;					// User's rights on this address book
	CAdbkACLList*		mACLs;						// List of ACLs on this address book
	unsigned long		mRefCount;					// Reference count of opens

			char SkipTerm(char** txt, cdstring& copy);

			bool OpenCount();						// Determine whether to open based on ref count
			bool CloseCount();						// Determine whether to close based on ref cunt

private:
	void	InitAddressBook();
};

#endif
