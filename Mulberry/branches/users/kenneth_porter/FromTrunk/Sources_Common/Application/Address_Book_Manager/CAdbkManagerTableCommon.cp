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


// Source for CAdbkManagerTable class

#include "CAdbkManagerTable.h"

#include "CAdbkManagerWindow.h"
#include "CAdbkPropDialog.h"
#include "CAdbkProtocol.h"
#include "CAdbkSearchWindow.h"
#include "CAddressBookDoc.h"
#include "CAddressBookManager.h"
#include "CAddressBookView.h"
#include "CAddressBookWindow.h"
#if __dest_os == __mac_os || __dest_os == __mac_os_x
#include "CBalloonDialog.h"
#include "CCommands.h"
#endif
#include "CCreateAdbkDialog.h"
#include "CErrorHandler.h"
#include "CGetStringDialog.h"
#include "CLog.h"
#include "CMulberryApp.h"
#include "CMulberryCommon.h"
#include "CRemoteAddressBook.h"
#if __dest_os == __mac_os || __dest_os == __mac_os_x
#include "CResources.h"
#endif
#include "CPreferences.h"
#include "CTaskClasses.h"

#if __dest_os == __mac_os || __dest_os == __mac_os_x
#include "CStringResources.h"
#include "CTableMultiRowSelector.h"

#include <LDropFlag.h>
#include <LNodeArrayTree.h>
#include <LTableArrayStorage.h>
#else
#include "StValueChanger.h"
#endif

#include <stdio.h>
#include <string.h>

// C O N S T R U C T I O N / D E S T R U C T I O N  M E T H O D S

// O T H E R  M E T H O D S ____________________________________________________________________________

void CAdbkManagerTable::ListenTo_Message(long msg, void* param)
{
	// Only do these if the parent view is open
	if (!mTableView->IsOpen())
		return;

	switch (msg)
	{
	case CAddressBookManager::eBroadcast_NewAdbkAccount:
		AddProtocol(static_cast<CAdbkProtocol*>(param));
		break;
	case CAddressBookManager::eBroadcast_RemoveAdbkAccount:
		RemoveProtocol(static_cast<CAdbkProtocol*>(param));
		break;
	case CAdbkProtocol::eBroadcast_ClearList:
		ClearProtocol(static_cast<CAdbkProtocol*>(param));
		break;
	case CAdbkProtocol::eBroadcast_RefreshList:
		RefreshProtocol(static_cast<CAdbkProtocol*>(param));
		break;
	case CINETProtocol::eBroadcast_Logoff:
		LogoffProtocol(static_cast<CAdbkProtocol*>(param));
		break;
	}
}

// Test for selected adbk
bool CAdbkManagerTable::TestSelectionAdbk(TableIndexT row)
{
	// This is an adbk
	return IsCellAdbk(row);
}

// Test for selected adbk
bool CAdbkManagerTable::TestSelectionAdbkDisconnected(TableIndexT row)
{
	// This is an adbk
	if (IsCellAdbk(row))
	{
		CRemoteAddressBook* adbk = dynamic_cast<CRemoteAddressBook*>(GetCellAdbk(row));

		return adbk && adbk->GetProtocol()->CanDisconnect() && !adbk->GetProtocol()->IsDisconnected();
	}
	
	return false;
}

// Test for selected adbk
bool CAdbkManagerTable::TestSelectionAdbkClearDisconnected(TableIndexT row)
{
	// This is an adbk
	if (IsCellAdbk(row))
	{
		CRemoteAddressBook* adbk = dynamic_cast<CRemoteAddressBook*>(GetCellAdbk(row));

		return adbk && adbk->GetProtocol()->CanDisconnect();
	}
	
	return false;
}

// Set the address book manager
void CAdbkManagerTable::SetManager(CAddressBookManager* manager)
{
	// Save server
	mManager = manager;
	
	if (mManager)
	{
		mManager->Add_Listener(this);

		// Make sure its open so items will actually be displayed
		mTableView->SetOpen();

		// Calculate number of rows for first time
		ResetTable();
	}
}

// Do preview of address book
void CAdbkManagerTable::PreviewAddressBook(bool clear)
{
	// Ignore if no preview pane
	if (!mTableView->GetPreview())
		return;

	CAddressBook* adbk = NULL;
	ulvector sels;
	if (!clear)
	{
		// Add selection to list
		CFlatAdbkList selected;
		if (DoToSelection1((DoToSelection1PP) &CAdbkManagerTable::AddSelectionToList, &selected) && (selected.size() == 1))
			adbk = selected.front();
	}
	
	// Do preview of specific address book only if it exists or its being cleared
	if (adbk || clear)
		PreviewAddressBook(adbk, clear);
}

// Do preview of address book
void CAdbkManagerTable::PreviewAddressBook(CAddressBook* adbk, bool clear)
{
	// Ignore if no preview pane
	if (!mTableView->GetPreview())
		return;

	bool changed = clear;
	ulvector sels;
	if (!clear)
	{
		// Check whether different
		CAddressBook* old_adbk = mTableView->GetPreview()->GetAddressBook();
		cdstring old_url = old_adbk ? old_adbk->GetURL(true) : cdstring::null_str;
		if ((adbk ? adbk->GetURL(true) : cdstring::null_str) != old_url)
		{
			// Set flag to force update
			changed = true;
		}
	}

	// Only do preview if there has been a change
	if (changed)
	{
		// Always clear out message preview immediately, otherwise
		// do preview at idle time to prevent re-entrant network calls
		if (adbk)
		{
			// Give it to preview (if its NULL the preview will be cleared)
			CAddressBookPreviewTask* task = new CAddressBookPreviewTask(mTableView->GetPreview(), adbk);
			task->Go();
		}
		else
			// do immediate update of preview
			mTableView->GetPreview()->SetAddressBook(adbk);
	}
}

// Get properties of mailbox
void CAdbkManagerTable::OnAddressBookProperties(void)
{
	CFlatAdbkList selected;

	try
	{
		// Add selection to list
		if (DoToSelection1((DoToSelection1PP) &CAdbkManagerTable::AddSelectionToList, &selected))
		{
			// Check all addressbooks first
			for(CFlatAdbkList::iterator iter = selected.begin(); iter != selected.end(); iter++)
			{
				long new_msgs = 0;

				// IMPORTANT: if the connection is lost the addressbooks are removed by the protocol
				// being logged off. Must protect against this case
				const CAdbkProtocol* proto = NULL;
				CRemoteAddressBook* radbk = dynamic_cast<CRemoteAddressBook*>(*iter);
				if (radbk)
					proto = radbk->GetProtocol();

				// Must not allow one failure to stop others

				// Get extension info
				try
				{
					(*iter)->CheckMyRights();
				}
				catch (...)
				{
					CLOG_LOGCATCH(...);

					// Throw up if proto has died
					if (!proto->IsLoggedOn())
					{
						CLOG_LOGRETHROW;
						throw;
					}
				}

				try
				{
					(*iter)->CheckACLs();
				}
				catch (...)
				{
					CLOG_LOGCATCH(...);

					// Throw up if proto has died
					if (!proto->IsLoggedOn())
					{
						CLOG_LOGRETHROW;
						throw;
					}
				}
			}

#if __dest_os == __mac_os || __dest_os == __mac_os_x
			// Create the dialog
			CBalloonDialog	theHandler(paneid_AdbkPropDialog, this);
			CAdbkPropDialog* prop_dlog = (CAdbkPropDialog*) theHandler.GetDialog();
			prop_dlog->SetAdbkList(&selected);
			long set = 1;
			prop_dlog->ListenToMessage(msg_SetPropAdbkGeneral, &set);
			theHandler.StartDialog();

			// Let DialogHandler process events
			while (true)
			{
				MessageT	hitMessage = theHandler.DoDialog();

				if (hitMessage == msg_OK)
				{
					break;
				}
				else if (hitMessage == msg_Cancel)
				{
					break;
				}
			}
#elif __dest_os == __win32_os
			// Create the dialog
			CAdbkPropDialog dlog(this);
			dlog.SetAdbkList(&selected);

			// Let DialogHandler process events
			dlog.DoModal();
#elif __dest_os == __linux_os
			CAdbkPropDialog::PoseDialog(&selected);
#else
#error __dest_os
#endif
		}
	}
	catch (...)
	{
		CLOG_LOGCATCH(...);

		// Catch errors to prevent further processing
	}

	// Always refresh
	RefreshSelection();

#if __dest_os == __win32_os
	// Set focus back to table
	SetFocus();
#endif

#if __dest_os == __linux_os
	Focus();
#endif

} // CAdbkManagerTable::DoAddressBookProperties

// New address book
void CAdbkManagerTable::OnNewAddressBook(void)
{
	STableCell aCell(0,0);
	GetNextSelectedCell(aCell);
	TableIndexT row = aCell.row;
	if (!IsSelectionValid() ||
		mHasOthers && (row > 1) ||
		!mHasOthers && (row > 0))
	{
		CAdbkProtocol* proto = NULL;
		if (IsSelectionValid())
		{
			TableIndexT	woRow = GetWideOpenIndex(row + TABLE_ROW_ADJUST);

			CAdbkList::node_type* node = (CAdbkList::node_type*) GetCellData(woRow);
			proto = GetCellAdbkProtocol(row);

			// If proto is null we have an address book - so get its protocol
			if (!proto)
			{
				CRemoteAddressBook* radbk = dynamic_cast<CRemoteAddressBook*>(node->GetSelectData());
				proto = radbk ? radbk->GetProtocol() : NULL;
			}

			// Must have a proto
			if (!proto)
				return;
		}

		// Get name for new address book
		CCreateAdbkDialog::SCreateAdbk details;
		details.personal = true;
		details.account = proto ? proto->GetAccountName() : cdstring::null_str;
		details.open_on_startup = true;
		details.use_nicknames = true;
		details.use_search = true;
		
		if (CCreateAdbkDialog::PoseDialog(&details))
		{
			// Get the chosen protocol
			proto = mManager->GetProtocol(details.account);

		
			// never allow personal on local account
			if ((details.account == CPreferences::sPrefs->mLocalAdbkAccount.GetValue().GetName()) ||
				(details.account == CPreferences::sPrefs->mOSAdbkAccount.GetValue().GetName()))
				details.personal = false;

			// Adjust name if required
			cdstring adbk_name = details.name;
			if (details.personal)
			{
				cdstring default_name = proto->GetUserPrefix();
				default_name += proto->GetAccount()->GetAuthenticator().GetAuthenticator()->GetActualUID();

				// Add user id to address book name
				adbk_name = default_name;
				adbk_name += proto->GetSeparator();
				adbk_name += details.name;
			}

			CRemoteAddressBook* adbk = NULL;
			try
			{
				// Create address book
				adbk = new CRemoteAddressBook(proto, adbk_name);

				// New it
				adbk->New();

				// Set chosen flags
				adbk->SetFlags(CAddressBook::eOpenOnStart, details.open_on_startup);
				adbk->SetFlags(CAddressBook::eLookup, details.use_nicknames);
				adbk->SetFlags(CAddressBook::eSearch, details.use_search);

				// Change prefs list
				mManager->SyncAddressBook(adbk, true);
				CPreferences::sPrefs->ChangeAddressBookOpenOnStart(adbk, details.open_on_startup);
				CPreferences::sPrefs->ChangeAddressBookLookup(adbk, details.use_nicknames);
				CPreferences::sPrefs->ChangeAddressBookSearch(adbk, details.use_search);

				// Update table

				// Always force table to logged off state
				int pos = 0;

				if (mManager->FindProtocol(proto, pos))
				{
					TableIndexT temp = pos;
					CAdbkList::node_type* proto_node = (CAdbkList::node_type*) GetCellData(temp);

					// Iterate over all adbk manager windows
					cdmutexprotect<CAdbkManagerView::CAdbkManagerViewList>::lock _lock(CAdbkManagerView::sAdbkManagerViews);
					for(CAdbkManagerView::CAdbkManagerViewList::iterator iter = CAdbkManagerView::sAdbkManagerViews->begin(); iter != CAdbkManagerView::sAdbkManagerViews->end(); iter++)
					{
						(*iter)->GetTable()->RemoveChildren(temp, true);
						(*iter)->GetTable()->AddChildren(proto_node, temp, false);
						(*iter)->GetTable()->RefreshRow(row);
						(*iter)->GetTable()->RefreshRowsBelow(temp);
					}
				}
			}
			catch (...)
			{
				CLOG_LOGCATCH(...);

				// Remove from manager
				if (adbk)
					adbk->Close();
				delete adbk;
				adbk = NULL;
			}
		}
	}
	else
#if __dest_os == __mac_os || __dest_os == __mac_os_x
		CMulberryApp::sApp->MakeNewAddressBookDoc();
#elif __dest_os == __win32_os || __dest_os == __linux_os
		CMulberryApp::sApp->OnAppNewLocalAddressBook();
#else
#error __dest_os
#endif
}

// Open address book
void CAdbkManagerTable::OnOpenAddressBook(void)
{
	if (IsSingleSelection() || !mHasOthers && !IsSelectionValid())
	{
		STableCell aCell;
		GetFirstSelection(aCell);
		if (mHasOthers && (aCell.row > 1) ||
			!mHasOthers && (aCell.row > 0))
		{
			// Open each selected address book
			DoToSelection((DoToSelectionPP) &CAdbkManagerTable::OpenAddressBook);
		}
		else
#if __dest_os == __mac_os || __dest_os == __mac_os_x
			CMulberryApp::sApp->ChooseAddressBookDoc();
#elif __dest_os == __win32_os || __dest_os == __linux_os
			CMulberryApp::sApp->OnAppOpenLocalAddressBook();
#else
#error __dest_os
#endif
	}
	else
		// Display each selected address book
		DoToSelection((DoToSelectionPP) &CAdbkManagerTable::OpenAddressBook);
}

// Display a specified address book
bool CAdbkManagerTable::OpenAddressBook(TableIndexT row)
{
	// Get node
	TableIndexT	woRow = GetWideOpenIndex(row + TABLE_ROW_ADJUST);

	CAdbkList::node_type* node = (CAdbkList::node_type*) GetCellData(woRow);

	// Only open selectable
	if (node->IsSelectable())
	{
		// Open it
		return CAddressBookWindow::OpenWindow(node->GetSelectData());
	}
	else
	{
		// Look for closed protocol
		CAdbkProtocol* proto = GetCellAdbkProtocol(row);
		if (proto && !proto->IsLoggedOn())
		{
			bool result = true;
			try
			{
				// Logon and load list
				mManager->StartProtocol(proto);

				// Always expand a server row when logging in
				ExpandRow(woRow);
			}
			catch (...)
			{
				CLOG_LOGCATCH(...);

				result = false;
			}
			return result;
		}
		else
			return false;
	}
}

// Rename address books
void CAdbkManagerTable::OnRenameAddressBook(void)
{
	// Display each selected message
	DoToSelection((DoToSelectionPP) &CAdbkManagerTable::RenameAddressBook);
}

// Rename specified address book
bool CAdbkManagerTable::RenameAddressBook(TableIndexT row)
{
	bool failagain = false;
	CAddressBook* adbk = GetCellAdbk(row);

	do
	{
		failagain = false;

		// Get a new name for the mailbox (use old name as starter)
		cdstring new_name;
		new_name = adbk->GetName();
#if __dest_os == __mac_os || __dest_os == __mac_os_x
		if (CGetStringDialog::PoseDialog("Alerts::Adbk::Rename", new_name))
#else
		if (CGetStringDialog::PoseDialog("Alerts::Adbk::RenameTitle", "Alerts::Adbk::Rename", new_name))
#endif
		{
			try
			{
				adbk->Rename(new_name);
				RefreshRow(row);
			}
			catch (...)
			{
				CLOG_LOGCATCH(...);

				failagain = true;
			}
		}
	}
	while (failagain);

	return false;
}

// Delete address books
void CAdbkManagerTable::OnDeleteAddressBook(void)
{
	// Check that this is what we want to do
	if (CErrorHandler::PutCautionAlertRsrc(true, "Alerts::Adbk::ReallyDelete") == CErrorHandler::Ok)
	{
		// Display each selected message
		DoToSelection((DoToSelectionPP) &CAdbkManagerTable::DeleteAddressBook);
	}
}

// Delete specified address book
bool CAdbkManagerTable::DeleteAddressBook(TableIndexT row)
{
	CAddressBook* adbk = GetCellAdbk(row);

	try
	{
		adbk->Delete();
	}
	catch (...)
	{
		CLOG_LOGCATCH(...);

	}

	return false;
}

// Search address books
void CAdbkManagerTable::OnSearchAddressBook(void)
{
#if __dest_os == __mac_os || __dest_os == __mac_os_x
	CMulberryApp::sApp->ObeyCommand(cmd_SearchAddressBook);
#elif __dest_os == __win32_os || __dest_os == __linux_os
	CMulberryApp::sApp->OnAppSearchAddress();
#else
#error __dest_os
#endif
}

// Logon to servers
void CAdbkManagerTable::OnLoginAddressBook(void)
{
	// Login to server
	DoToSelection((DoToSelectionPP) &CAdbkManagerTable::LoginAddressBook);
	
	// Force toolbar update
	mTableView->RefreshToolbar();
}

// Logon to specified server
bool CAdbkManagerTable::LoginAddressBook(TableIndexT row)
{
	// Get node
	TableIndexT	woRow = GetWideOpenIndex(row + TABLE_ROW_ADJUST);

	CAdbkList::node_type* node = GetCellNode(row);

	// Look for closed protocol
	CAdbkProtocol* proto = GetCellAdbkProtocol(row);
	if (proto && !proto->IsLoggedOn())
	{
		bool result = true;
		try
		{
			// Logon and load list
			mManager->StartProtocol(proto);

			// Always expand a server row when logging in
			ExpandRow(woRow);
		}
		catch (...)
		{
			CLOG_LOGCATCH(...);

			result = false;
		}
		return result;
	}
	else
		return false;
}

// Logoff to servers
void CAdbkManagerTable::OnLogoutAddressBook(void)
{
	// Display each selected message
	DoToSelection((DoToSelectionPP) &CAdbkManagerTable::LogoutAddressBook);
	
	// Force toolbar update
	mTableView->RefreshToolbar();
}

// Logon to specified server
bool CAdbkManagerTable::LogoutAddressBook(TableIndexT row)
{
	// Get node
	TableIndexT	woRow = mCollapsableTree->GetWideOpenIndex(row + TABLE_ROW_ADJUST);

	CAdbkList::node_type* node = GetCellNode(row);

	// Look for logged on protocol
	CAdbkProtocol* proto = GetCellAdbkProtocol(row);
	if (proto && proto->IsLoggedOn())
	{
		bool result = true;
		try
		{
			// Logoff
			mManager->StopProtocol(proto);

			// Update display
			RefreshRow(row);
			CollapseRow(woRow);
		}
		catch (...)
		{
			CLOG_LOGCATCH(...);

			result = false;
		}
		return result;
	}
	else
		return false;
}

// Refresh servers
void CAdbkManagerTable::OnRefreshAddressBook(void)
{
	// Display each selected message
	DoToSelection((DoToSelectionPP) &CAdbkManagerTable::RefreshAddressBook);
}

// Refresh specified server
bool CAdbkManagerTable::RefreshAddressBook(TableIndexT row)
{
	// Get node
	TableIndexT	woRow = mCollapsableTree->GetWideOpenIndex(row + TABLE_ROW_ADJUST);

	CAdbkList::node_type* node = GetCellNode(row);

	// Look for logged on protocol
	CAdbkProtocol* proto = GetCellAdbkProtocol(row);
	if (proto && proto->IsLoggedOn())
	{
		bool result = true;
		try
		{
			// Stop redraws whilst refresh command executes
			{
				StValueChanger<bool> _noredraw(mListChanging, true);

				// Load list
				proto->RefreshList();
			}
		}
		catch (...)
		{
			CLOG_LOGCATCH(...);

			result = false;
		}
		return result;
	}
	else
		return false;
}

// Synchronise address book
void CAdbkManagerTable::OnSynchroniseAddressBook(void)
{
	// Display each selected message
	DoToSelection((DoToSelectionPP) &CAdbkManagerTable::SynchroniseAddressBook);
}

// Refresh specified server
bool CAdbkManagerTable::SynchroniseAddressBook(TableIndexT row)
{
	CAddressBook* adbk = GetCellAdbk(row);

	try
	{
		adbk->Synchronise(false);
	}
	catch (...)
	{
		CLOG_LOGCATCH(...);

	}

	RefreshRow(row);

	return false;
}

// Clear Disconnected address book
void CAdbkManagerTable::OnClearDisconnectAddressBook(void)
{
	// Check that this is what we want to do
	if (CErrorHandler::PutCautionAlertRsrc(true, "Alerts::Adbk::ReallyClearDisconnect") == CErrorHandler::Ok)
	{
		// Display each selected message
		DoToSelection((DoToSelectionPP) &CAdbkManagerTable::ClearDisconnectAddressBook);
	}
}

// Clear Disconnected specified address book
bool CAdbkManagerTable::ClearDisconnectAddressBook(TableIndexT row)
{
	CRemoteAddressBook* adbk = static_cast<CRemoteAddressBook*>(GetCellAdbk(row));

	try
	{
		// Must close the address book if its open in disconnected mode
		if (adbk->GetProtocol()->IsDisconnected() && adbk->IsOpen())
			adbk->Close();

		// Now clear out disconnected cache
		adbk->ClearDisconnect();
	}
	catch (...)
	{
		CLOG_LOGCATCH(...);

	}

	RefreshRow(row);

	return false;
}

// Get the selected adbk name
bool CAdbkManagerTable::IsCellAdbk(TableIndexT row)
{
	CAdbkList::node_type* node = GetCellNode(row);

	return node->IsSelectable();
}

// Get the selected adbk
CAdbkList::node_type* CAdbkManagerTable::GetCellNode(TableIndexT row)
{
	TableIndexT	woRow = mCollapsableTree->GetWideOpenIndex(row + TABLE_ROW_ADJUST);
	return (CAdbkList::node_type*) GetCellData(woRow);
}

// Get the selected adbk
CAddressBook* CAdbkManagerTable::GetCellAdbk(TableIndexT row)
{
	CAdbkList::node_type* node = GetCellNode(row);

	// Return adbk
	return node->GetSelectData();
}

// Get the selected adbk protocol
CAdbkProtocol* CAdbkManagerTable::GetCellAdbkProtocol(TableIndexT row)
{
	TableIndexT	woRow = mCollapsableTree->GetWideOpenIndex(row + TABLE_ROW_ADJUST);

	CAdbkList::node_type* node = GetCellNode(row);

	UInt32	nestingLevel = mCollapsableTree->GetNestingLevel(woRow);

	// Return adbk protocol
	return (nestingLevel ? NULL : mManager->GetProtocol(*node->GetNoSelectData()));
}

// Add a node to the list
void CAdbkManagerTable::AddNode(const CAdbkList::node_type* node, TableIndexT& row, bool child, bool refresh)
{
	TableIndexT parent_row;

	if (child)
	{
		parent_row = AddLastChildRow(row, &node, sizeof(CAdbkList::node_type*), node->IsHierarchic(), refresh);
		mData.insert(mData.begin() + (parent_row - 1), (void*) node);
	}
	else
	{
		row = InsertSiblingRows(1, row, &node, sizeof(CAdbkList::node_type*), node->IsHierarchic(), refresh);
		parent_row = row;
		mData.insert(mData.begin() + (parent_row - 1), (void*) node);
	}

	// Insert children
	AddChildren(node, parent_row, refresh);
}

// Add child nodes to the list
void CAdbkManagerTable::AddChildren(const CAdbkList::node_type* node, TableIndexT& parent_row, bool refresh)
{
	// Insert children
	if (node->HasChildren())
		for(CAdbkList::node_list::const_iterator iter = node->GetChildren()->begin();
			iter != node->GetChildren()->end(); iter++)
			AddNode(*iter, parent_row, true, refresh);
}

// Remove child nodes from the list
void CAdbkManagerTable::RemoveChildren(TableIndexT& parent_row, bool refresh)
{
	// Sledge-hammer approach!
	{
		// Prevent window update during changes
		StDeferTableAdjustment changing(this);

		while(CountAllDescendents(parent_row))
		{
			RemoveRows(1, parent_row + 1, false);
		}
	}

	if (refresh)
		RefreshRowsBelow(parent_row);
}

// Add selected mboxes to list
bool CAdbkManagerTable::AddSelectionToList(TableIndexT row, CFlatAdbkList* list)
{
	CAdbkList::node_type* node = GetCellNode(row);

	// Ignore non-selectable
	if (!node->IsSelectable())
		return false;

	// Determine delete mailbox
	list->push_back(node->GetSelectData());

	return true;
}

// Add protocol at end of list
void CAdbkManagerTable::AddProtocol(CAdbkProtocol* proto)
{
	// Get list from manager
	const CAdbkList& adbks = mManager->GetAddressBooks();

	// Get last top-level sibling row
	TableIndexT row = 0;
	AddNode(adbks.GetChildren()->back(), row, true, true);
	
	proto->Add_Listener(this);
	
	// Expand row
	if (proto->GetAddressAccount()->GetExpanded())
	{
		int pos;
		if (mManager->FindProtocol(proto, pos))
		{
			// Remove this and all children
			TableIndexT woRow = pos;
			ExpandRow(woRow);
		}
	}
}

// Remove protocol from list
void CAdbkManagerTable::RemoveProtocol(CAdbkProtocol* proto)
{
	int pos;
	if (mManager->FindProtocol(proto, pos))
	{
		// Remove this and all children
		TableIndexT temp = pos;
		RemoveRows(1, temp, true);
	}
}

// Clear protocol list
void CAdbkManagerTable::ClearProtocol(CAdbkProtocol* proto)
{
	int pos;
	if (mManager->FindProtocol(proto, pos))
	{
		// Remove all children
		TableIndexT temp = pos;
		RemoveChildren(temp, true);
	}
}

// Add protocol children from list
void CAdbkManagerTable::RefreshProtocol(CAdbkProtocol* proto)
{
	int pos;
	if (mManager->FindProtocol(proto, pos))
	{
		// Remove this and all children
		TableIndexT woRow = pos;
		if (!HasChildren(woRow))
		{
			TableIndexT row = GetExposedIndex(woRow);
			if (row)
			{
				CAdbkList::node_type* node = GetCellNode(row - TABLE_ROW_ADJUST);
				AddChildren(node, woRow, true);
				RefreshRow(row - TABLE_ROW_ADJUST);
			}
		}
	}
}

// Remove protocol children from list
void CAdbkManagerTable::LogoffProtocol(CAdbkProtocol* proto)
{
	int pos;
	if (mManager->FindProtocol(proto, pos))
	{
		// Remove this and all children
		TableIndexT temp = pos;
		RemoveChildren(temp, true);
		TableIndexT exp_row = GetExposedIndex(temp);
		if (exp_row)
			RefreshRow(exp_row - TABLE_ROW_ADJUST);
	}
}

#pragma mark ____________________________Expand/collapse

void CAdbkManagerTable::CollapseRow(UInt32 inWideOpenRow)
{
	CHierarchyTableDrag::CollapseRow(inWideOpenRow);

	// Sync expansion state with prefs
	ProcessExpansion(inWideOpenRow, false);
}

void CAdbkManagerTable::DeepCollapseRow(UInt32 inWideOpenRow)
{
	CHierarchyTableDrag::DeepCollapseRow(inWideOpenRow);

	// Sync expansion state with prefs
	ProcessExpansion(inWideOpenRow, false);
}

void CAdbkManagerTable::ExpandRow(UInt32 inWideOpenRow)
{
	// Sync expansion state with prefs
	ProcessExpansion(inWideOpenRow, true);

	CHierarchyTableDrag::ExpandRow(inWideOpenRow);
}

void CAdbkManagerTable::DeepExpandRow(UInt32 inWideOpenRow)
{
	// Sync expansion state with prefs
	ProcessExpansion(inWideOpenRow, true);

	CHierarchyTableDrag::DeepExpandRow(inWideOpenRow);
}

void CAdbkManagerTable::ProcessExpansion(UInt32 inWideOpenRow, bool expand)
{
	// Only concerned with top-level items
	UInt32	nestingLevel = mCollapsableTree->GetNestingLevel(inWideOpenRow);
	if (nestingLevel == 0)
	{
		// Get adbk protocol
		CAdbkList::node_type* node = reinterpret_cast<CAdbkList::node_type*>(GetCellData(inWideOpenRow));
		CAdbkProtocol* proto = mManager->GetProtocol(*node->GetNoSelectData());
		if (proto)
		{
			bool was_expanded = proto->GetAddressAccount()->GetExpanded();
			if (was_expanded ^ expand)
			{
				proto->GetAddressAccount()->SetExpanded(expand);

				// Mark prefs as dirty
				if (proto->GetAccountType() == CAddressAccount::eLocalAdbk)
					CPreferences::sPrefs->mLocalAdbkAccount.SetDirty();
				else if (proto->GetAccountType() == CAddressAccount::eOSAdbk)
					CPreferences::sPrefs->mOSAdbkAccount.SetDirty();
				else
					CPreferences::sPrefs->mAddressAccounts.SetDirty();
			}
		}
	}
}
