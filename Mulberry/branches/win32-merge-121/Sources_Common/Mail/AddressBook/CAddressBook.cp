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


// CAddressBook.cp

// Class that encapsulates address book functionality (local and remote).
// This is an abstract base class and should be derived for each type
// of address book (eg local, remote IMSP-style, remote-LDAP etc)

#include "CAddressBook.h"

#include "CAddressBookManager.h"
#include "CCharSpecials.h"
#include "CMailControl.h"
#include "CPreferences.h"
#include "CStringUtils.h"

#include <strstream>

cdmutex CAddressBook::_mutex;										// Used for locks

// Copy constructor
CAddressBook::CAddressBook(const CAddressBook& copy)
{
	InitAddressBook();
	
	mAdbkName = copy.mAdbkName;
	mFlags = copy.mFlags;
}

// Destructor
CAddressBook::~CAddressBook()
{
	CAddressBookManager::sAddressBookManager->SyncAddressBook(this, false);

	mACLs = NULL;
}

// Initialization
void CAddressBook::InitAddressBook()
{
	mACLs = NULL;
	mRefCount = 0;
}

#pragma mark ____________________________Opening/Closing

void CAddressBook::SetName(const char* name)
{
	// Remove from manager's cache
	if (mAdbkName.length())
		CAddressBookManager::sAddressBookManager->SyncAddressBook(this, false);

	mAdbkName = name;
	CAddressBookManager::sAddressBookManager->RefreshAddressBook(this);

	// Add to manager's cache
	CAddressBookManager::sAddressBookManager->SyncAddressBook(this, true);
}

// Determine whether to open based on ref count
bool CAddressBook::OpenCount()
{
	// Lock global mutex
	cdmutex::lock_cdmutex _lock(_mutex);
	
	// If ref count is 0, then open required - also bump ref count
	return mRefCount++ == 0;
}

// Determine whether to close based on ref cunt
bool CAddressBook::CloseCount()
{
	// Lock global mutex
	cdmutex::lock_cdmutex _lock(_mutex);
	
	// Bump down ref count, if ref count is now 0, then close required
	return --mRefCount == 0;
}

// New visual address book on source
void CAddressBook::New()
{
	SetFlags(eOpen);
}

// Open visual address book from source
void CAddressBook::Open()
{
	SetFlags(eOpen);

	// Remove any cached addresses
	//mAddresses.erase_all();
	//mGroups.erase_all();
}

// Read in addresses
void CAddressBook::Read()
{
	SetFlags(eLoaded);
}

// Save addresses
void CAddressBook::Save()
{
}

// Close visual address book
void CAddressBook::Close()
{
	// Only if already open
	if (!IsOpen())
		return;

	// Clean up the UI
	CMailControl::AddressBookClosed(this);

	SetFlags(eOpen, false);
	SetFlags(eLoaded, false);
	
	Clear();
}

// Rename address book
void CAddressBook::Rename(cdstring& new_name)
{
	// Remove from manager's cache
	CAddressBookManager::sAddressBookManager->SyncAddressBook(this, false);
	CPreferences::sPrefs->ChangeAddressBookOpenOnStart(this, false);
	CPreferences::sPrefs->ChangeAddressBookLookup(this, false);
	CPreferences::sPrefs->ChangeAddressBookSearch(this, false);

	mAdbkName = new_name;

	// Add to manager's cache
	CAddressBookManager::sAddressBookManager->SyncAddressBook(this, true);
	CPreferences::sPrefs->ChangeAddressBookOpenOnStart(this, IsSearch());
	CPreferences::sPrefs->ChangeAddressBookLookup(this, IsLookup());
	CPreferences::sPrefs->ChangeAddressBookSearch(this, true);
}

// Clear addresses
void CAddressBook::Clear()
{
	mAddresses.clear();
	mGroups.clear();
}

// Delete address book
void CAddressBook::Delete()
{
	// Close first
	Close();

	// Remove from manager's cache
	CAddressBookManager::sAddressBookManager->SyncAddressBook(this, false);
	CPreferences::sPrefs->ChangeAddressBookOpenOnStart(this, false);
	CPreferences::sPrefs->ChangeAddressBookLookup(this, false);
	CPreferences::sPrefs->ChangeAddressBookSearch(this, false);
}

// Empty address book
void CAddressBook::Empty()
{
	Clear();
}

// Copy addresses and groups to another address book
void CAddressBook::CopyAll(CAddressBook* adbk)
{
	// Copy all addresses
	CAddressList addrs;
	addrs.set_delete_data(false);
	for(CAddressList::iterator iter = mAddresses.begin(); iter != mAddresses.end(); iter++)
		addrs.push_back(new CAdbkAddress(*static_cast<CAdbkAddress*>(*iter)));
	adbk->AddAddress(&addrs);

	// Copy all groups
	CGroupList grps;
	grps.set_delete_data(false);
	for(CGroupList::iterator iter = mGroups.begin(); iter != mGroups.end(); iter++)
		grps.push_back(new CGroup(*static_cast<CGroup*>(*iter)));
	adbk->AddGroup(&grps);
}

// Synchronise to local
void CAddressBook::Synchronise(bool fast)
{
}

// Clear disconnected cache
void CAddressBook::ClearDisconnect()
{
}

// Switch into disconnected mode
void CAddressBook::SwitchDisconnect(CAdbkProtocol* local)
{
}

CAdbkAddress* CAddressBook::FindAddress(const char* name)
{
	// Look through address list for match
	for(CAddressList::iterator iter = mAddresses.begin(); iter != mAddresses.end(); iter++)
	{
		if ((*iter)->GetName() == name)
			return static_cast<CAdbkAddress*>(*iter);
	}

	return NULL;
}

CAdbkAddress* CAddressBook::FindAddress(const CAddress* addr)
{
	// Look through address list for match
	for(CAddressList::iterator iter = mAddresses.begin(); iter != mAddresses.end(); iter++)
	{
		if (**iter == *addr)
			return static_cast<CAdbkAddress*>(*iter);
	}

	return NULL;
}

CAdbkAddress* CAddressBook::FindAddressEntry(const char* entry)
{
	CAddressList::iterator found = ::find_if(mAddresses.begin(), mAddresses.end(), CAdbkAddress::same_entry_str(entry));
	if (found != mAddresses.end())
		return static_cast<CAdbkAddress*>(*found);

	return NULL;
}

CGroup* CAddressBook::FindGroup(const char* name)
{
	// Look through group list for match
	for(CGroupList::iterator iter = mGroups.begin(); iter != mGroups.end(); iter++)
	{
		if ((*iter)->GetName() == name)
			return *iter;
	}
	
	return NULL;
}

CGroup* CAddressBook::FindGroupEntry(const char* entry)
{
	CGroupList::iterator found = ::find_if(mGroups.begin(), mGroups.end(), CGroup::same_entry_str(entry));
	if (found != mGroups.end())
		return static_cast<CGroup*>(*found);

	return NULL;
}

void CAddressBook::AddAddress(CAddress* addr, bool sorted)
{
	CAddressList addrs;
	addrs.set_delete_data(false);
	addrs.push_back(addr);
	AddAddress(&addrs, sorted);
}

// Add addresses to list or delete them
void CAddressBook::AddAddress(CAddressList* addrs, bool sorted)
{
	for(CAddressList::const_iterator iter = addrs->begin(); iter != addrs->end(); iter++)
	{
		if (IsLoaded())
		{
			if (sorted)
				mAddresses.push_back_sorted(*iter);
			else
				mAddresses.push_back(*iter);
		}
		else
			delete *iter;
	}
}

// Add unique addresses from list
void CAddressBook::AddUniqueAddresses(CAddressList& add)
{
	unsigned long count = 0;
	CAddressList unique;

	// Count unique items first
	for(CAddressList::iterator iter = add.begin(); iter != add.end(); iter++)
	{
		if (!mAddresses.IsDuplicate(*iter))
		{
			unique.push_back(*iter);
			count++;
		}
	}

	// Add unique
	if (count)
	{
		// Get current size of list
		size_t old_size = mAddresses.size();

		// Insert total number of blanks addresses
		mAddresses.insert(mAddresses.begin() + old_size, count, NULL);

		// Set to fill in from start of new items
		CAddressList::iterator pos = mAddresses.begin() + old_size;

		// Add unique items
		for(CAddressList::iterator iter = unique.begin(); iter != unique.end(); iter++)

			// Copy from unique list
			*pos++ = new CAdbkAddress(**iter);
	}

	// Do not delete addresses twice
	unique.clear_without_delete();
}

void CAddressBook::UpdateAddress(CAddress* addr, bool sorted)
{
	CAddressList addrs;
	addrs.set_delete_data(false);
	addrs.push_back(addr);
	UpdateAddress(&addrs, sorted);
}

// Address changed
void CAddressBook::UpdateAddress(CAddressList* old_addrs, CAddressList* new_addrs, bool sorted)
{
	// Switch pointer to old items to pointers to new items
	for(CAddressList::iterator iter = old_addrs->begin(); iter != old_addrs->end(); iter++)
	{
		// Find new one
		CAddressList::iterator found = ::find_if(new_addrs->begin(), new_addrs->end(), CAdbkAddress::same_entry(static_cast<CAdbkAddress*>(*iter)));
		if (found != new_addrs->end())
		{
			// Get index of old one and replace with new one
			unsigned long index = mAddresses.FetchIndexOf(*iter);
			if (index != 0)
				mAddresses.at(index - 1) = *found;
		}
	}
	
	// Now do actual change
	UpdateAddress(new_addrs, sorted);
	
	// Update the entries in the old ones to match those of the new ones
	// This is needed with IMSP/LocalAdbk which use the full name as the key
	int ctr = 0;
	for(CAddressList::iterator iter = old_addrs->begin(); iter != old_addrs->end(); iter++, ctr++)
		static_cast<CAdbkAddress*>(*iter) ->SetEntry(static_cast<CAdbkAddress*>(new_addrs->at(ctr))->GetEntry());
}

void CAddressBook::RemoveAddress(CAddress* addr)
{
	CAddressList addrs;
	addrs.set_delete_data(false);
	addrs.push_back(addr);
	RemoveAddress(&addrs);
}

void CAddressBook::AddGroup(CGroup* grp, bool sorted)
{
	CGroupList grps;
	grps.set_delete_data(false);
	grps.push_back(grp);
	AddGroup(&grps, sorted);
}

// Add group to group list.
void CAddressBook::AddGroup(CGroupList* grps, bool sorted)
{
	for(CGroupList::const_iterator iter = grps->begin(); iter != grps->end(); iter++)
	{
		if (IsLoaded())
		{
			if (sorted)
				mGroups.push_back_sorted(*iter);
			else
				mGroups.push_back(*iter);
		}
		else
			delete *iter;
	}
}

// Add unique groups from list
void CAddressBook::AddUniqueGroups(CGroupList& add)
{
	unsigned long count = 0;
	CGroupList unique;
	CGroupList duplicate;

	// Count unique items first
	for(CGroupList::const_iterator iter1 = add.begin(); iter1 != add.end(); iter1++)
	{
		if (!mGroups.IsDuplicate(*iter1))
		{
			unique.push_back(*iter1);
			count++;
		}
		else
			duplicate.push_back(*iter1);
	}

	// Get current size of list
	size_t old_size = mGroups.size();

	// Insert total number of blank groups
	mGroups.insert(mGroups.begin() + old_size, count, nil);

	// Set to fill in from start of new items
	CGroupList::iterator pos = mGroups.begin() + old_size;

	// Add unique items
	for(CGroupList::iterator iter2 = unique.begin(); iter2 != unique.end(); iter2++)

		// Copy from unique list
		*pos++ = new CGroup(**iter2);

	// Merge duplicate items
	for(CGroupList::iterator iter3 = duplicate.begin(); iter3 != duplicate.end(); iter3++)
	{
		for(CGroupList::iterator iter4 = mGroups.begin(); iter4 != mGroups.end(); iter4++)
		{
			if (**iter4 == **iter3)
			{
				// Merge with existing
				(*iter4)->Merge(*iter3);
				break;
			}
		}
	}

	// Do not delete itesms twice
	unique.clear_without_delete();
	duplicate.clear_without_delete();
}

void CAddressBook::UpdateGroup(CGroup* grp, bool sorted)
{
	CGroupList grps;
	grps.set_delete_data(false);
	grps.push_back(grp);
	UpdateGroup(&grps, sorted);
}

// Address changed
void CAddressBook::UpdateGroup(CGroupList* old_grps, CGroupList* new_grps, bool sorted)
{
	// Switch pointer to old items to pointers to new items
	for(CGroupList::iterator iter = old_grps->begin(); iter != old_grps->end(); iter++)
	{
		// Find new one
		CGroupList::iterator found = ::find_if(new_grps->begin(), new_grps->end(), CGroup::same_entry(*iter));
		if (found != new_grps->end())
		{
			// Get index of old one and replace with new one
			unsigned long index = mGroups.FetchIndexOf(*iter);
			if (index != 0)
				mGroups.at(index - 1) = *found;
		}
	}
	
	// Now do actual change
	UpdateGroup(new_grps, sorted);
}

void CAddressBook::RemoveGroup(CGroup* grp)
{
	CGroupList grps;
	grps.set_delete_data(false);
	grps.push_back(grp);
	RemoveGroup(&grps);
}

// Give this address a unique entry
void CAddressBook::MakeUniqueEntry(CAdbkAddress* addr) const
{
	// If it has an entry we're done
	bool done = false;
	while(!done)
	{
		// Generate time stamp
		char time_str[256];
		::snprintf(time_str, 256, "Mulberry_%04x", ::time(NULL) + (time_t) ::clock());

		// Look for match
		done = true;
		for(CAddressList::const_iterator iter = mAddresses.begin(); iter != mAddresses.end(); iter++)
		{
			if (static_cast<CAdbkAddress*>(*iter)->GetEntry() == time_str)
			{
				done = false;
				break;
			}
		}

		// Add if its unique
		if (done)
			addr->SetEntry(time_str);
	}
}

#pragma mark ____________________________Read/Write

#define ANON				"Anonymous"
#define	GROUP_HEADER		"Group:"
#define	NEW_GROUP_HEADER	"Grp:"

void CAddressBook::ImportAddresses(char* txt)
{
	// Do to each line
	char* p = txt;
	char* line = p;
	while(*p)
	{
		// Punt to end of line/string
		while(*p && (*p != '\r') && (*p != '\n'))
			p++;
		
		// Terminate if not at end
		if (*p)
			*p++ = 0;

		// Import whole line
		ImportAddress(line, true, NULL, NULL);
		
		// Punt over line ends
		while(*p && ((*p == '\r') || (*p == '\n')))
			p++;
		
		// Reset line start
		line = p;
	}
}

void CAddressBook::ImportAddress(char* txt, bool add, CAdbkAddress** raddr, CGroup** rgrp, bool add_entry)
{
	char* p = txt;

	cdstring adl;
	cdstring name;
	cdstring whole_name;
	cdstring eaddr;
	cdstring company;
	cdstring address;
	cdstring phone_work;
	cdstring phone_home;
	cdstring fax;
	cdstring url;
	cdstring notes;

	// ADL will be at start
	char q = SkipTerm(&p, adl);
	adl.ConvertToOS();

	// Check for group
	if (((adl == NEW_GROUP_HEADER) || (adl == GROUP_HEADER)) && (q != '\r') && (q != '\n') && q)
	{
		if (adl == NEW_GROUP_HEADER)
		{
			// Nick-name is next
			q = SkipTerm(&p, adl);
			adl.ConvertToOS();

			// Do not allow anonymous
			if (adl == ANON)
				adl = cdstring::null_str;
		}
		else
			adl = cdstring::null_str;

		// Name is next
		q = SkipTerm(&p, name);
		name.ConvertToOS();

		// Do not allow anonymous
		if (name == ANON)
			name = cdstring::null_str;

		// Create new group
		CGroup* grp  = new CGroup(name, adl);
		if (add_entry)
			grp->SetEntry(name);

		// Add all addresses to group
		while(q && (q != '\r') && (q != '\n'))
		{
			q = SkipTerm(&p, eaddr);
			eaddr.ConvertToOS();

			if (!eaddr.empty())
				grp->GetAddressList().push_back(eaddr);
		}
		
		// Add it or return it
		if (add)
			GetGroupList()->push_back(grp);
		else if (rgrp)
			*rgrp = grp;
	}

	// Got address
	else if ((q != '\r') && (q != '\n') && q)
	{
		// Check for anonymous
		if (adl == ANON)
			adl = cdstring::null_str;

		// Name is next
		q = SkipTerm(&p, name);
		name.ConvertToOS();

		// Check for anonymous
		if (name != ANON)
		{
			// Process into first & last names

			// Look for comma
			const char* comma = ::strchr(name.c_str(), ',');

			if (comma)
			{
				const char* r = comma + 1;

				// Strip leading space
				r += ::strspn(r, SPACE);

				// Copy first names first with trailing space
				whole_name = r;
				whole_name += SPACE;

				// Copy last name up to comma
				whole_name += cdstring(name, 0, comma - name.c_str());
			}

			// Name in correct order - just copy
			else
				whole_name = name;
		}

		short ctr = 2;
		while ((q != '\r') && (q != '\n') && q)
		{
			switch(ctr++)
			{
			case 2:
				q = SkipTerm(&p, eaddr);
				eaddr.ConvertToOS();
				break;
			case 3:
				q = SkipTerm(&p, company);
				company.ConvertToOS();
				break;
			case 4:
				q = SkipTerm(&p, address);

				// Must unescape address
				address.FilterOutEscapeChars();
				address.ConvertToOS();
				break;
			case 5:
				q = SkipTerm(&p, phone_work);
				phone_work.ConvertToOS();
				break;
			case 6:
				q = SkipTerm(&p, phone_home);
				phone_home.ConvertToOS();
				break;
			case 7:
				q = SkipTerm(&p, fax);
				fax.ConvertToOS();
				break;
			case 8:
				q = SkipTerm(&p, url);

				// Must unescape URL
				url.FilterOutEscapeChars();
				url.ConvertToOS();

				// If no more then last item was notes
				if ((q == '\r') || (q == '\n') || !q)
				{
					notes = url;
					url = cdstring::null_str;
				}
				break;
			case 9:
				q = SkipTerm(&p, notes);

				// Must unescape notes
				notes.FilterOutEscapeChars();
				notes.ConvertToOS();
				break;
			default:
				// Step till line end or string end
				while ((*p) && (*p != '\r') && (*p != '\n')) p++;
				q = *p;
				if (*p == '\r') p++;
				if (*p == '\n') p++;
			}
		}


		CAdbkAddress* addr = new CAdbkAddress(NULL, eaddr, whole_name, adl, company, address,
													phone_work, phone_home, fax, url, notes);
		if (add_entry)
			addr->SetEntry(whole_name);
		
		// Add it or return it
		if (add)
			GetAddressList()->push_back(addr);
		else if (raddr)
			*raddr = addr;
	}
}

// Read and set bounds from file
char CAddressBook::SkipTerm(char** txt, cdstring& copy)
{
	const char* p = *txt;

	// Step up to terminator
	while ((**txt) && (**txt != '\t') && (**txt != '\r') && (**txt != '\n')) (*txt)++;

	char q = **txt;

	// null terminate and advance
	if ((**txt == '\r') || (**txt == '\n'))
	{
		**txt = '\0';
		(*txt)++;
		while((**txt) && ((**txt == '\r') || (**txt == '\n')))
			(*txt)++;
	}
	else if (**txt == '\t')
	{
		**txt = '\0';
		(*txt)++;
	}

	// Copy tokenised item
	copy = p;

	return q;
}

char* CAddressBook::ExportAddress(const CAdbkAddress* addr) const
{
	ostrstream out;

	// Write nick-name (adl)
	cdstring str;
	if (addr->GetADL().empty())
		str = ANON;
	else
		str = addr->GetADL();
	out << str;
	out.put('\t');

	// Write user name
	if (addr->GetName().empty())
		str = ANON;
	else
		str = addr->GetName();
	char whole_name[256];
	::strcpy(whole_name, str);

	// Find last name
	char* last_name = ::strrchr(whole_name, ' ');
	if (last_name)
	{
		// Tie it off and advance to last name
		*last_name = '\0';
		last_name++;
		if (::strlen(last_name) > 0)
		{
			str = last_name;
			out << str << ", ";
		}
	}

	// Do first name
	str = whole_name;
	out << str;
	out.put('\t');

	// Write email address mailbox@host
	str = addr->GetMailAddress();
	out << str;
	out.put('\t');

	// Write company
	str = addr->GetCompany();
	out << str;
	out.put('\t');

	// Write address
	str = addr->GetAddress(CAdbkAddress::eDefaultAddressType);
	if (!str.empty())
	{
		str.FilterInEscapeChars(cCEscapeChar);
		out << str;
	}
	out.put('\t');

	// Write phone work
	str = addr->GetPhone(CAdbkAddress::eWorkPhoneType);
	out << str;
	out.put('\t');

	// Write phone home
	str = addr->GetPhone(CAdbkAddress::eHomePhoneType);
	out << str;
	out.put('\t');

	// Write fax
	str = addr->GetPhone(CAdbkAddress::eFaxType);
	out << str;
	out.put('\t');

	// Write URL
	str = addr->GetURL();
	if (!str.empty())
	{
		str.FilterInEscapeChars(cCEscapeChar);
		out << str;
	}
	out.put('\t');

	// Write notes
	str = addr->GetNotes();
	if (!str.empty())
	{
		str.FilterInEscapeChars(cCEscapeChar);
		out << str;
	}

	out << os_endl << ends;
	return out.str();
}

char* CAddressBook::ExportGroup(const CGroup* grp) const
{
	ostrstream out;

	// Write group header
	cdstring str = NEW_GROUP_HEADER;
	out << str;
	out.put('\t');

	// Write group nick-name
	if (grp->GetNickName().empty())
		str = ANON;
	else
		str = grp->GetNickName();
	out << str;
	out.put('\t');

	// Write group name
	if (grp->GetName().empty())
		str = ANON;
	else
		str = grp->GetName();
	out << str;

	// Write each address out
	for(cdstrvect::const_iterator iter = grp->GetAddressList().begin(); iter != grp->GetAddressList().end(); iter++)
	{
		// Write TAB before address
		out.put('\t');

		// Write address
		str = *iter;
		out << str;
	}

	out << os_endl << ends;
	return out.str();
}

#pragma mark ____________________________Lookup

// Find nick-name
bool CAddressBook::FindNickName(const char* nick_name, CAdbkAddress*& addr)
{
	// Try to get nick-name from list
	for(CAddressList::const_iterator iter = mAddresses.begin(); iter != mAddresses.end(); iter++)
	{
		if (::strcmpnocase(nick_name, (*iter)->GetADL())==0)
		{
			addr = dynamic_cast<CAdbkAddress*>(*iter);
			return true;
		}
	}

	return false;
}

// Find group nick-name
bool CAddressBook::FindGroupName(const char* grp_name, CGroup*& grp)
{
	// Try to get nick-name from list
	for(CGroupList::const_iterator iter = mGroups.begin(); iter != mGroups.end(); iter++)
	{
		if (::strcmpnocase(grp_name, (*iter)->GetNickName())==0)
		{
			grp = *iter;
			return true;
		}
	}

	return false;
}

// Do search
void CAddressBook::SearchAddress(const cdstring& name, CAdbkAddress::EAddressMatch match, CAdbkAddress::EAddressField field, CAddressList& addr_list)
{
	cdstring matchit(name);
	CAdbkAddress::ExpandMatch(match, matchit);

	// Iterate over all single addresses
	for(CAddressList::const_iterator iter = mAddresses.begin(); iter != mAddresses.end(); iter++)
	{
		bool result = false;

		CAdbkAddress* addr = dynamic_cast<CAdbkAddress*>(*iter);
		if (!addr)
			continue;
			
		// Do comparison of specific field
		result = addr->Search(matchit, field);

		// Check for matching item
		if (result)
			// Add copy to list
			addr_list.push_back(new CAdbkAddress(*addr));
	}
}
