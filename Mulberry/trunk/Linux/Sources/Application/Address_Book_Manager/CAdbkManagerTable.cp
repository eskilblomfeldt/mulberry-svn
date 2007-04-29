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

#include "CAdbkManagerView.h"
#include "CAdbkManagerWindow.h"
#include "CAdbkProtocol.h"
#include "CAddressBookManager.h"
#include "CAddressBookWindow.h"
#include "CAdminLock.h"
#include "CCommands.h"
#include "CIconLoader.h"
#include "CMessage.h"
#include "CMulberryApp.h"
#include "CMulberryCommon.h"
#include "CPreferences.h"
#include "CRemoteAddressBook.h"
#include "CServerBrowse.h"
#include "CTableRowSelector.h"
#include "CTableRowGeometry.h"
#include "CTitleTableView.h"
#include "CWaitCursor.h"

#include "StPenState.h"

#include <JPainter.h>
#include <JXImage.h>
#include <JXColormap.h>

#include <stdio.h>
#include <string.h>

const short cTextIndent = 26;

// __________________________________________________________________________________________________
// C L A S S __ C M A I L B O X T I T L E T A B L E
// __________________________________________________________________________________________________

// C O N S T R U C T I O N / D E S T R U C T I O N  M E T H O D S

// Default constructor
CAdbkManagerTable::CAdbkManagerTable(JXScrollbarSet* scrollbarSet, 
										JXContainer* enclosure,
										const HSizingOption hSizing, 
										const VSizingOption vSizing,
										const JCoordinate x, const JCoordinate y,
										const JCoordinate w, const JCoordinate h)
	: CHierarchyTableDrag(scrollbarSet, enclosure, hSizing, vSizing, x, y, w, h)
{
	SetBorderWidth(0);

	mManager = NULL;
	mListChanging = false;
	mHierarchyCol = 1;
	mHasOthers = false;

	// Create storage
	mTableGeometry = new CTableRowGeometry(this, 128, 18);
	mTableSelector = new CTableRowSelector(this);
	//mTableStorage = new LTableArrayStorage(this, sizeof(CAdbkList::node_type*));

	SetRowSelect(true);
}


// Default destructor
CAdbkManagerTable::~CAdbkManagerTable()
{
}

// O T H E R  M E T H O D S ____________________________________________________________________________


// Get details of sub-panes
void CAdbkManagerTable::OnCreate()
{
	const JXContainer* parent = GetEnclosure();
	while(parent && !dynamic_cast<const CAdbkManagerView*>(parent))
		parent = parent->GetEnclosure();
	mTableView = const_cast<CAdbkManagerView*>(dynamic_cast<const CAdbkManagerView*>(parent));

	CHierarchyTableDrag::OnCreate();

	CreateContextMenu(CMainMenu::eContextAdbkManager);
}

// Handle key presses
bool CAdbkManagerTable::HandleChar(const int key, const JXKeyModifiers& modifiers)
{
	// Look for preview/full view based on key stroke

	// Determine whether preview is triggered
	CKeyModifiers mods(modifiers);
	const CUserAction& preview = mTableView->GetPreviewAction();
	if ((preview.GetKey() == key) &&
		(preview.GetKeyModifiers() == mods))
	{
		DoPreview();
		return true;
	}

	// Determine whether full view is triggered
	const CUserAction& fullview = mTableView->GetFullViewAction();
	if ((fullview.GetKey() == key) &&
		(fullview.GetKeyModifiers() == mods))
	{
		DoFullView();
		return true;
	}
	
	return CHierarchyTableDrag::HandleChar(key, modifiers);
}

//	Respond to commands
bool CAdbkManagerTable::ObeyCommand(unsigned long cmd, SMenuCommandChoice* menu)
{
	switch (cmd)
	{
	case CCommand::eEditProperties:
	case CCommand::eToolbarDetailsBtn:
		OnAddressBookProperties();
		return true;
		
	case CCommand::eAddressesNew:
	case CCommand::eToolbarAdbkMgrNewBtn:
		OnNewAddressBook();
		return true;

	case CCommand::eAddressesOpen:
	case CCommand::eToolbarAdbkMgrOpenBtn:
		OnOpenAddressBook();
		return true;
		
	case CCommand::eAddressesRename:
		OnRenameAddressBook();
		return true;
		
	case CCommand::eAddressesDelete:
	case CCommand::eToolbarAdbkMgrDeleteBtn:
		OnDeleteAddressBook();
		return true;
		
	case CCommand::eAddressesLogin:
		OnLoginAddressBook();
		return true;
		
	case CCommand::eAddressesLogout:
		OnLogoutAddressBook();
		return true;
		
	case CCommand::eAddressesRefresh:
		OnRefreshAddressBook();
		return true;
		
	case CCommand::eAddressesSynchronise:
		OnSynchroniseAddressBook();
		return true;
		
	case CCommand::eAddressesClearDisconnected:
		OnClearDisconnectAddressBook();
		return true;

	case CCommand::eToolbarAdbkMgrSearchBtn:
		OnSearchAddressBook();
		return true;
		
	default:;
	}
	
	return CHierarchyTableDrag::ObeyCommand(cmd, menu);
}

void CAdbkManagerTable::UpdateCommand(unsigned long cmd, CCmdUI* cmdui)
{
	switch (cmd)
	{

	// These ones must have a selection
	case CCommand::eEditProperties:
	case CCommand::eToolbarDetailsBtn:
	case CCommand::eAddressesRename:
	case CCommand::eAddressesDelete:
	case CCommand::eToolbarAdbkMgrDeleteBtn:
		OnUpdateAllAdbk(cmdui);
		return;

	case CCommand::eAddressesSynchronise:
		OnUpdateDisconnectedSelection(cmdui);
		return;

	case CCommand::eAddressesClearDisconnected:
		OnUpdateClearDisconnectedSelection(cmdui);
		return;
		
		// These can only have logged on protocol selection
	case CCommand::eAddressesNew:
	case CCommand::eToolbarAdbkMgrNewBtn:
		OnUpdateAlways(cmdui);
		return;

	case CCommand::eAddressesOpen:
	case CCommand::eToolbarAdbkMgrOpenBtn:
		OnUpdateAdbkSelection(cmdui);
		return;

		// These can only have logged on protocol selection
	case CCommand::eAddressesLogin:
		OnUpdateLoggedOutSelection(cmdui);
		return;

		// These can only have logged off protocol selection
	case CCommand::eAddressesRefresh:
	case CCommand::eAddressesLogout:
		OnUpdateLoggedInSelection(cmdui);
		return;
		 
	// Always available
	case CCommand::eToolbarAdbkMgrSearchBtn:
		OnUpdateAlways(cmdui);
		return;

	default:;
	}

	CHierarchyTableDrag::UpdateCommand(cmd, cmdui);
}

// Keep titles in sync
void CAdbkManagerTable::ScrollImageBy(SInt32 inLeftDelta, SInt32 inTopDelta, bool inRefresh)
{
	// Find titles in owner chain
	mTableView->GetBaseTitles()->ScrollImageBy(inLeftDelta, 0, inRefresh);

	CHierarchyTableDrag::ScrollImageBy(inLeftDelta, inTopDelta, inRefresh);
}

// Click in the cell
void CAdbkManagerTable::LClickCell(const STableCell& inCell, const JXKeyModifiers& modifiers)
{
	// Determine which heading it is
	SColumnInfo col_info = mTableView->GetColumnInfo()[inCell.col - 1];

	switch(col_info.column_type)
	{
	case eAdbkColumnOpen:
	case eAdbkColumnResolve:
	case eAdbkColumnSearch:
	{
		CAdbkList::node_type* node = GetCellNode(inCell.row);

		// Do status flag
		if (node->IsSelectable())
		{
			bool set;
			switch(col_info.column_type)
			{
			case eAdbkColumnOpen:
				set = !node->GetSelectData()->IsOpenOnStart();
				node->GetSelectData()->SetFlags(CAddressBook::eOpenOnStart, set);
				CAddressBookManager::sAddressBookManager->SyncAddressBook(node->GetSelectData(), set);
		
				// Change prefs list
				CPreferences::sPrefs->ChangeAddressBookOpenOnStart(node->GetSelectData(), set);
				break;

			case eAdbkColumnResolve:
				set = !node->GetSelectData()->IsLookup();
				node->GetSelectData()->SetFlags(CAddressBook::eLookup, set);
				CAddressBookManager::sAddressBookManager->SyncAddressBook(node->GetSelectData(), set);
		
				// Change prefs list
				CPreferences::sPrefs->ChangeAddressBookLookup(node->GetSelectData(), set);
				break;

			case eAdbkColumnSearch:
				set = !node->GetSelectData()->IsSearch();
				node->GetSelectData()->SetFlags(CAddressBook::eSearch, set);
				CAddressBookManager::sAddressBookManager->SyncAddressBook(node->GetSelectData(), set);
		
				// Change prefs list
				CPreferences::sPrefs->ChangeAddressBookSearch(node->GetSelectData(), set);
				break;
			}
			
			RefreshRow(inCell.row);
		}
		break;
	}
	default:
		CHierarchyTableDrag::LClickCell(inCell, modifiers);
		DoSingleClick(inCell.row, CKeyModifiers(modifiers));
		break;
	}
}

void CAdbkManagerTable::LDblClickCell(const STableCell& inCell, const JXKeyModifiers& modifiers)
{
	DoDoubleClick(inCell.row, CKeyModifiers(modifiers));
}

// Draw the titles
void CAdbkManagerTable::DrawCell(JPainter* pDC, const STableCell& inCell,
									const JRect& inLocalRect)
{
	// Not if changing
	if (mListChanging)
		return;

	// Get cell data item
	int	woRow = GetWideOpenIndex(inCell.row + TABLE_ROW_ADJUST);
	CAdbkList::node_type* node = GetCellNode(inCell.row);
	cdstring theTxt;

	// Erase to ensure drag hightlight is overwritten
	unsigned long bkgnd = 0x00FFFFFF;
	if (UsesBackgroundColor(node))
	{
		StPenState save(pDC);
		JRect bgrect(inLocalRect);
		//If we don't make the rectangle smaller, there will be no border between
		//color stripes.  If we just use the table row's border, then the tree
		//lines get broken up.
		bgrect.bottom -=1;
		
		JColorIndex bkgnd_index = GetBackgroundColor(node);
		pDC->SetPenColor(bkgnd_index);
		pDC->SetFilling(kTrue);
		pDC->Rect(bgrect);
		
		JSize r, g, b;
		GetColormap()->GetRGB(bkgnd_index, &r, &g, &b);
		bkgnd = ((r & 0xFF00) << 8) | (g & 0xFF00) | ((b & 0xFF00) >> 8);
	}

	// Determine which heading it is
	SColumnInfo col_info = mTableView->GetColumnInfo()[inCell.col - 1];
	switch(col_info.column_type)
	{
	case eAdbkColumnName:
		{
		// Get suitable icon
		ResIDT iconID = GetPlotIcon(node, GetCellAdbkProtocol(inCell.row));
		JXImage* icon = CIconLoader::GetIcon(iconID, this, 16, bkgnd);

		// Determine name to use
		theTxt = node->IsSelectable() ? node->GetSelectData()->GetName() : *node->GetNoSelectData();

		// Draw the string
		bool strike = false;
		SetTextStyle(pDC, node, GetCellAdbkProtocol(inCell.row), strike);
		int text_start = DrawHierarchyRow(pDC, inCell.row, inLocalRect, theTxt, icon);

		// Always cache column number
		mHierarchyCol = inCell.col;
		break;
		}
	case eAdbkColumnOpen:
		if (node->IsSelectable())
		{
			ResIDT iconID;
			if (node->GetSelectData()->IsOpenOnStart())
				iconID = IDI_DIAMONDTICKED;
			else
				iconID = IDI_DIAMOND;
			JXImage* icon = CIconLoader::GetIcon(iconID, this, 16, bkgnd);
			pDC->Image(*icon, icon->GetBounds(), inLocalRect.left, inLocalRect.top + mIconOrigin);
		}
		break;
		
	case eAdbkColumnResolve:
		if (node->IsSelectable())
		{
			ResIDT iconID;
			if (node->GetSelectData()->IsLookup())
				iconID = IDI_DIAMONDTICKED;
			else
				iconID = IDI_DIAMOND;
			JXImage* icon = CIconLoader::GetIcon(iconID, this, 16, bkgnd);
			pDC->Image(*icon, icon->GetBounds(), inLocalRect.left, inLocalRect.top + mIconOrigin);
		}
		break;
		
	case eAdbkColumnSearch:
		if (node->IsSelectable())
		{
			ResIDT iconID;
			if (node->GetSelectData()->IsSearch())
				iconID = IDI_DIAMONDTICKED;
			else
				iconID = IDI_DIAMOND;
			JXImage* icon = CIconLoader::GetIcon(iconID, this, 16, bkgnd);
			pDC->Image(*icon, icon->GetBounds(), inLocalRect.left, inLocalRect.top + mIconOrigin);
		}
		break;
		
	}
}


// Get appropriate icon id
JIndex CAdbkManagerTable::GetPlotIcon(const CAdbkList::node_type* node, CAdbkProtocol* proto)
{
	if (!node->IsSelectable())
	{
		if (!proto)
			return IDI_SERVERLOCAL;
		else if (proto->CanDisconnect())
			return proto->IsDisconnected() ? IDI_SERVERDISCONNECTED : IDI_SERVERREMOTE;
		else if (proto->GetAccountType() == CINETAccount::eLocalAdbk)
			return IDI_SERVERLOCAL;
		else
			return IDI_SERVERREMOTE;
	}
	else
	{
		const CRemoteAddressBook* adbk = dynamic_cast<const CRemoteAddressBook*>(node->GetSelectData());
		if (adbk && adbk->GetProtocol()->CanDisconnect() && adbk->IsLocalAdbk())
			return adbk->IsCachedAdbk() ? IDI_ADDRESSFILECACHED : IDI_ADDRESSFILEUNCACHED;
		else
			return IDI_ADDRESSFILE;
	}
}

// Get text style
void CAdbkManagerTable::SetTextStyle(JPainter* pDC, const CAdbkList::node_type* node, CAdbkProtocol* proto, bool& strike)
{
	strike = false;
	JFontStyle text_style;
	short style = normal;

	if (!node->IsSelectable())
	{
		if (!proto || proto->IsLoggedOn())
		{
			GetColormap()-> JColormap::AllocateStaticColor(CPreferences::sPrefs->mServerOpenStyle.GetValue().color, &text_style.color);
			style = style | CPreferences::sPrefs->mServerOpenStyle.GetValue().style;
		}
		else
		{
			GetColormap()->JColormap::AllocateStaticColor(CPreferences::sPrefs->mServerClosedStyle.GetValue().color, &text_style.color);
			style = style | CPreferences::sPrefs-> mServerClosedStyle.GetValue().style;
		}
	}
	else
	{
		GetColormap()->JColormap::AllocateStaticColor(CPreferences::sPrefs->mMboxClosedStyle.GetValue().color, &text_style.color);
		style = style | CPreferences::sPrefs->mMboxClosedStyle.GetValue().style;
	}
	text_style.bold = JBoolean(style & bold);
	text_style.italic = JBoolean(style & italic);
	text_style.underlineCount = JBoolean((style & underline) ? 1 : 0);
	text_style.strike = JBoolean(style & strike);

	pDC->SetPenColor(text_style.color);

	pDC->SetFontStyle(text_style);
}

bool CAdbkManagerTable::UsesBackgroundColor(const CAdbkList::node_type* node) const
{
	return !node->IsSelectable();
}

JColorIndex CAdbkManagerTable::GetBackgroundColor(const CAdbkList::node_type* node) const
{
	JColorIndex ret;
	GetColormap()->JColormap::AllocateStaticColor(CPreferences::sPrefs->mServerBkgndStyle.GetValue().color, &ret);
	return ret;
}

SColumnInfo CAdbkManagerTable::GetColumnInfo(TableIndexT col)
{
	return mTableView->GetColumnInfo()[col - 1];
}

void CAdbkManagerTable::DoSelectionChanged()
{
	CHierarchyTableDrag::DoSelectionChanged();
	
	// Determine whether preview is triggered
	const CUserAction& preview = mTableView->GetPreviewAction();
	if (preview.GetSelection())
		DoPreview();

	// Determine whether full view is triggered
	const CUserAction& fullview = mTableView->GetFullViewAction();
	if (fullview.GetSelection())
		DoFullView();
}

// Handle single click
void CAdbkManagerTable::DoSingleClick(TableIndexT row, const CKeyModifiers& mods)
{
	// Determine whether preview is triggered
	const CUserAction& preview = mTableView->GetPreviewAction();
	if (preview.GetSingleClick() &&
		(preview.GetSingleClickModifiers() == mods))
		DoPreview();

	// Determine whether full view is triggered
	const CUserAction& fullview = mTableView->GetFullViewAction();
	if (fullview.GetSingleClick() &&
		(fullview.GetSingleClickModifiers() == mods))
		DoFullView();
}

// Handle double click
void CAdbkManagerTable::DoDoubleClick(TableIndexT row, const CKeyModifiers& mods)
{
	// Determine whether preview is triggered
	const CUserAction& preview = mTableView->GetPreviewAction();
	if (preview.GetDoubleClick() &&
		(preview.GetDoubleClickModifiers() == mods))
		DoPreview();

	// Determine whether full view is triggered
	const CUserAction& fullview = mTableView->GetFullViewAction();
	if (fullview.GetDoubleClick() &&
		(fullview.GetDoubleClickModifiers() == mods))
		DoFullView();
}

void CAdbkManagerTable::DoPreview()
{
	PreviewAddressBook();
}

void CAdbkManagerTable::DoPreview(CAddressBook* adbk)
{
	PreviewAddressBook(adbk);
}

// Just edit the item
void CAdbkManagerTable::DoFullView()
{
	OnOpenAddressBook();
}

// Reset the table from the mboxList
void CAdbkManagerTable::ResetTable(void)
{
	// Start cursor for busy operation
	CWaitCursor wait;

	// Delete all existing rows
	Clear();
	mHierarchyCol = 1;
	mData.clear();

	// Only if we have the manager
	if (!mManager)
		return;

	// Get list from manager
	const CAdbkList& adbks = mManager->GetAddressBooks();

	// Add each node
	TableIndexT row = 0;
	TableIndexT exp_row = 1;
	mHasOthers = true;
	ulvector expand_rows;
	for(CAdbkList::node_list::const_iterator iter = adbks.GetChildren()->begin();
		iter != adbks.GetChildren()->end(); iter++)
	{
		// Don't add empty first row
		if ((iter != adbks.GetChildren()->begin()) || (*iter)->CountChildren())
		{
			AddNode(*iter, row, false);

			// Listen to each protocol
			CAdbkProtocol* proto = GetCellAdbkProtocol(exp_row);
			if (proto)
				proto->Add_Listener(this);
			
			// Check expansion - always expand if pure local address account
			if (!proto || proto->GetAddressAccount()->GetExpanded())
			{
				// Add wide open row to expansion list which gets processed later
				TableIndexT	woRow = GetWideOpenIndex(exp_row + TABLE_ROW_ADJUST);
				expand_rows.push_back(woRow);
			}
			
			// Bump up exposed row counter
			exp_row++;
		}
		else
			mHasOthers = false;
	}

	// Do expansions
	if (expand_rows.size())
	{
		for(ulvector::const_iterator iter = expand_rows.begin(); iter != expand_rows.end(); iter++)
			ExpandRow(*iter);
	}
	
	// Always expand a single account
	else if (exp_row == 2)
		ExpandRow(1);

	// Refresh list
	Refresh();
}

// Reset the table from the mboxList
void CAdbkManagerTable::ClearTable()
{
	// Delete all existing rows
	Clear();
	mData.clear();

	// Refresh list
	Refresh();
	
	// Force toolbar update
	SelectionChanged();
}

// Remove rows and adjust parts
void CAdbkManagerTable::RemoveRows(UInt32 inHowMany, TableIndexT inFromRow, bool inRefresh)
{
	// Count number to remove (this + descendents)
	UInt32 remove_count = CountAllDescendents(inFromRow) + 1;

	// Do standard removal
	CHierarchyTableDrag::RemoveRows(inHowMany, inFromRow, inRefresh);
	
	// Remove our data using count
	mData.erase(mData.begin() + (inFromRow - 1), mData.begin() + (inFromRow - 1 + remove_count));
}

// Get the selected adbk
void* CAdbkManagerTable::GetCellData(TableIndexT woRow)
{
	return mData.at(woRow - 1);
}

#if NOTYET
// Adjust cursor over drop area
bool CAdbkManagerTable::IsCopyCursor(DragReference inDragRef)
{
	return true;
}

// Test drag insert cursor
bool CAdbkManagerTable::IsDropCell(DragReference inDragRef, STableCell theCell)
{
	return IsValidCell(theCell) && IsCellAdbk(theCell.row);
}

// Handle multiple messages
void CAdbkManagerTable::DoDragReceive(DragReference inDragRef)
{
	// Init drop params
	mDropFirst = true;
	mDropSort = false;
	mDropAdbk = NULL;
	mDropAdbkWnd = NULL;
	mAddressAdded = false;
	mGroupAdded = false;

	try
	{
		// Do default action
		CHierarchyTableDrag::DoDragReceive(inDragRef);
	}
	catch (...)
	{
		CLOG_LOGCATCH(...);

		// Do not allow failure
	}

	// Always force window update
	if (mDropAdbkWnd)
		mDropAdbkWnd->ResetTable(mAddressAdded, mGroupAdded);
}

// Drop data into cell
void CAdbkManagerTable::DropDataIntoCell(FlavorType theFlavor, char* drag_data,
										Size data_size, const STableCell& theCell)
{
	if (mDropFirst)
	{
		// Determine adbk to use for copy
		mDropAdbk = GetCellAdbk(theCell.row);

		// Does window already exist?
		mDropAdbkWnd = CAddressBookWindow::FindWindow(mDropAdbk);
		mDropSort = (mDropAdbkWnd != NULL);

		mDropFirst = false;
	}

	switch (theFlavor)
	{

		case cDragMsgType:
			CMessage* theMsg = *((CMessage**) drag_data);
			CEnvelope* theEnv = theMsg->GetEnvelope();
			ThrowIfNil_(theEnv);

			// Add From
			if (theEnv->GetFrom()->size())
			{
				// Add to list
				mDropAdbk->AddAddress(new CAdbkAddress(*theEnv->GetFrom()->front()), mDropSort);
				mAddressAdded = true;
			}

			// Add Reply-To
			if (theEnv->GetReplyTo()->size())
			{
				// Add to list
				mDropAdbk->AddAddress(new CAdbkAddress(*theEnv->GetReplyTo()->front()), mDropSort);
				mAddressAdded = true;
			}

			// Add all To: except me
			for(CAddressList::iterator iter = theEnv->GetTo()->begin(); iter != theEnv->GetTo()->end(); iter++)
			{
				// Add to list
				mDropAdbk->AddAddress(new CAdbkAddress(**iter), mDropSort);
				mAddressAdded = true;
			}

			// Add all Ccs except me
			for(CAddressList::iterator iter = theEnv->GetCC()->begin(); iter != theEnv->GetCC()->end(); iter++)
			{
				// Add to list
				mDropAdbk->AddAddress(new CAdbkAddress(**iter), mDropSort);
				mAddressAdded = true;
			}

			break;

		case cDragAddrType:
			CAddress* theAddr = *((CAddress**) drag_data);
			{
				// Add to list
				mDropAdbk->AddAddress(new CAdbkAddress(*theAddr), mDropSort);
				mAddressAdded = true;
			}
			break;

		case cDragAddrGrpType:
			CGroup* theGrp = *((CGroup**) drag_data);

			// Add group to new group
			mDropAdbk->AddGroup(new CGroup(*theGrp), mDropSort);
			mGroupAdded = true;
			break;

		case 'TEXT':

			// Parse text into list
			CAddressList list(drag_data, data_size);

			// Try to add these to table
			for(CAddressList::iterator iter = list.begin(); iter != list.end(); iter++)
			{
				// Now add to address book
				mDropAdbk->AddAddress(new CAdbkAddress(**iter), mDropSort);
				mAddressAdded = true;
			}
			break;
	}
}
#endif

void CAdbkManagerTable::OnUpdateAllAdbk(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TestSelectionAnd((TestSelectionPP) &CAdbkManagerTable::TestSelectionAdbk));
}

void CAdbkManagerTable::OnUpdateAdbkSelection(CCmdUI* pCmdUI)
{
	if (IsSingleSelection())
	{
		STableCell aCell(0,0);
		GetNextSelectedCell(aCell);
		CAdbkProtocol* proto = GetCellAdbkProtocol(aCell.row);
		pCmdUI->Enable((proto == NULL) || ((pCmdUI->mCmd == CCommand::eAddressesNew) ? proto->IsLoggedOn() : false));
	}
	else if (!IsSelectionValid())
	{
		pCmdUI->Enable((pCmdUI->mCmd == CCommand::eAddressesNew) ? false :
						!mHasOthers && !CAdminLock::sAdminLock.mNoLocalAdbks);
	}
	else
		pCmdUI->Enable(TestSelectionAnd((TestSelectionPP) &CAdbkManagerTable::TestSelectionAdbk));
}

void CAdbkManagerTable::OnUpdateLoggedInSelection(CCmdUI* pCmdUI)
{
	if (IsSingleSelection())
	{
		STableCell aCell(0,0);
		GetNextSelectedCell(aCell);
		CAdbkProtocol* proto = GetCellAdbkProtocol(aCell.row);
		pCmdUI->Enable((proto != NULL) && proto->IsLoggedOn());
	}
	else
		pCmdUI->Enable(false);
}

void CAdbkManagerTable::OnUpdateLoggedOutSelection(CCmdUI* pCmdUI)
{
	if (IsSingleSelection())
	{
		STableCell aCell(0,0);
		GetNextSelectedCell(aCell);
		CAdbkProtocol* proto = GetCellAdbkProtocol(aCell.row);
		pCmdUI->Enable((proto != NULL) && !proto->IsLoggedOn());
	}
	else
		pCmdUI->Enable(false);
}

void CAdbkManagerTable::OnUpdateDisconnectedSelection(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TestSelectionAnd((TestSelectionPP) &CAdbkManagerTable::TestSelectionAdbkDisconnected));
}

void CAdbkManagerTable::OnUpdateClearDisconnectedSelection(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TestSelectionAnd((TestSelectionPP) &CAdbkManagerTable::TestSelectionAdbkClearDisconnected));
}
