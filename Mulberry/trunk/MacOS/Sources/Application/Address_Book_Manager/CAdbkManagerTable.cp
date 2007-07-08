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

#include "CAdbkManagerTitleTable.h"
#include "CAdbkManagerView.h"
#include "CAdbkManagerWindow.h"
#include "CAdbkProtocol.h"
#include "CAddressBookDoc.h"
#include "CAddressBookManager.h"
#include "CAddressBookWindow.h"
#include "CAdminLock.h"
#include "CBetterScrollerX.h"
#include "CCommands.h"
#include "CMessage.h"
#include "CRemoteAddressBook.h"
#include "CResources.h"
#include "CPreferences.h"
#include "CTableMultiRowSelector.h"
#include "CTableRowGeometry.h"
#include "CTableView.h"
#include "CWaitCursor.h"

#include <LDropFlag.h>
#include <LNodeArrayTree.h>
#include <LTableArrayStorage.h>

#include <stdio.h>
#include <string.h>

const short cTextIndent = 22;

// __________________________________________________________________________________________________
// C L A S S __ C M A I L B O X T I T L E T A B L E
// __________________________________________________________________________________________________

// C O N S T R U C T I O N / D E S T R U C T I O N  M E T H O D S

// Default constructor
CAdbkManagerTable::CAdbkManagerTable()
{
	InitAdbkManagerTable();
}

// Default constructor - just do parents' call
CAdbkManagerTable::CAdbkManagerTable(LStream *inStream)
	: CHierarchyTableDrag(inStream)
{
	InitAdbkManagerTable();
}

// Default destructor
CAdbkManagerTable::~CAdbkManagerTable()
{
}

// O T H E R  M E T H O D S ____________________________________________________________________________

void CAdbkManagerTable::InitAdbkManagerTable()
{
	mTableGeometry = new CTableRowGeometry(this, mFrameSize.width, 18);
	mTableSelector = new CTableMultiRowSelector(this);
	//mTableStorage = new LTableArrayStorage(this, sizeof(CAdbkList::node_type*));
	mManager = NULL;
	mListChanging = false;
	mHierarchyCol = 0;
	mHasOthers = false;
}

// Get details of sub-panes
void CAdbkManagerTable::FinishCreateSelf()
{
	// Do inherited
	CHierarchyTableDrag::FinishCreateSelf();

	// Find table view in super view chain
	LView* super = GetSuperView();
	while(super && !dynamic_cast<CAdbkManagerView*>(super))
		super = super->GetSuperView();
	mTableView = dynamic_cast<CAdbkManagerView*>(super);

	// Set Drag & Drop pane to scroller
	mPane = GetSuperView();

	// Set Drag & Drop info
	SetTable(this, true);

	AddDropFlavor(cDragAddrType);
	AddDropFlavor(cDragAddrGrpType);
	AddDropFlavor(cDragMsgType);
	AddDropFlavor(kScrapFlavorTypeText);

	// Set appropriate Drag & Drop inset
	Rect ddInset = {1, 1, 1, 1};
	if (((CBetterScrollerX*) mPane)->HasVerticalScrollBar())
		ddInset.right += 15;
	if (((CBetterScrollerX*) mPane)->HasHorizontalScrollBar())
		ddInset.bottom += 15;
	SetHiliteInset(ddInset);

	// Set read only status of Drag and Drop
	SetDDReadOnly(false);

	// Make it fit to the superview
	AdaptToNewSurroundings();
}

// Handle key presses
Boolean CAdbkManagerTable::HandleKeyPress(const EventRecord &inKeyEvent)
{
	// Look for preview/full view based on key stroke
	char key_press = (inKeyEvent.message & charCodeMask);
	if (key_press == char_Enter)
		key_press = char_Return;

	// Determine whether preview is triggered
	CKeyModifiers mods(inKeyEvent.modifiers);
	const CUserAction& preview = mTableView->GetPreviewAction();
	if ((preview.GetKey() == key_press) &&
		(preview.GetKeyModifiers() == mods))
	{
		DoPreview();
		return true;
	}

	// Determine whether full view is triggered
	const CUserAction& fullview = mTableView->GetFullViewAction();
	if ((fullview.GetKey() == key_press) &&
		(fullview.GetKeyModifiers() == mods))
	{
		DoFullView();
		return true;
	}

	return CHierarchyTableDrag::HandleKeyPress(inKeyEvent);
}

//	Respond to commands
Boolean CAdbkManagerTable::ObeyCommand(CommandT inCommand,void *ioParam)
{
	bool cmdHandled = true;

	switch (inCommand)
	{
	case cmd_Properties:
	case cmd_ToolbarDetailsBtn:
		OnAddressBookProperties();
		break;

	case cmd_NewAddressBook:
	case cmd_ToolbarAdbkMgrNewBtn:
		OnNewAddressBook();
		break;

	case cmd_OpenAddressBook:
	case cmd_ToolbarAdbkMgrOpenBtn:
		OnOpenAddressBook();
		break;

	case cmd_RenameAddressBook:
		OnRenameAddressBook();
		break;

	case cmd_DeleteAddressBook:
	case cmd_ToolbarAdbkMgrDeleteBtn:
		OnDeleteAddressBook();
		break;

	case cmd_LogonAddressBook:
		OnLoginAddressBook();
		break;

	case cmd_LogoffAddressBook:
		OnLogoutAddressBook();
		break;

	case cmd_RefreshAddressBook:
		OnRefreshAddressBook();
		break;

	case cmd_SynchroniseAddressBook:
		OnSynchroniseAddressBook();
		break;

	case cmd_ClearDisconnectAddressBook:
		OnClearDisconnectAddressBook();
		break;

	case cmd_ToolbarAdbkMgrSearchBtn:
		OnSearchAddressBook();
		break;

	default:
		cmdHandled = CHierarchyTableDrag::ObeyCommand(inCommand, ioParam);
		break;
	}

	return cmdHandled;
}

//	Pass back status of a (menu) command
void CAdbkManagerTable::FindCommandStatus(
	CommandT	inCommand,
	Boolean		&outEnabled,
	Boolean		&outUsesMark,
	UInt16		&outMark,
	Str255		outName)
{
	outUsesMark = false;

	switch (inCommand)
	{
	// These ones must have a selection
	case cmd_Properties:
	case cmd_ToolbarDetailsBtn:
	case cmd_RenameAddressBook:
	case cmd_DeleteAddressBook:
	case cmd_ToolbarAdbkMgrDeleteBtn:
		outEnabled = TestSelectionAnd((TestSelectionPP) &CAdbkManagerTable::TestSelectionAdbk);
		break;

	case cmd_SynchroniseAddressBook:
		outEnabled = TestSelectionAnd((TestSelectionPP) &CAdbkManagerTable::TestSelectionAdbkDisconnected);
		break;

	case cmd_ClearDisconnectAddressBook:
		outEnabled = TestSelectionAnd((TestSelectionPP) &CAdbkManagerTable::TestSelectionAdbkClearDisconnected);
		break;

	// These can only have logged on protocol selection
	case cmd_NewAddressBook:
	case cmd_ToolbarAdbkMgrNewBtn:
		outEnabled = true;
		break;

	case cmd_OpenAddressBook:
	case cmd_ToolbarAdbkMgrOpenBtn:
		// Remap command id for tests
		if (inCommand == cmd_ToolbarAdbkMgrNewBtn)
			inCommand = cmd_NewAddressBook;

		if (IsSingleSelection())
		{
			STableCell aCell(0,0);
			GetNextSelectedCell(aCell);
			CAdbkProtocol* proto = GetCellAdbkProtocol(aCell.row);
			outEnabled = ((proto == NULL) || ((inCommand == cmd_NewAddressBook) ? proto->IsLoggedOn() : false));
		}
		else if (!IsSelectionValid())
		{
			outEnabled = (inCommand == cmd_NewAddressBook) ? false :
							!mHasOthers && !CAdminLock::sAdminLock.mNoLocalAdbks;
		}
		else
			outEnabled = TestSelectionAnd((TestSelectionPP) &CAdbkManagerTable::TestSelectionAdbk);
		break;

	// These can only have logged on protocol selection
	case cmd_LogonAddressBook:
		if (IsSingleSelection())
		{
			STableCell aCell(0,0);
			GetNextSelectedCell(aCell);
			CAdbkProtocol* proto = GetCellAdbkProtocol(aCell.row);
			outEnabled = ((proto != NULL) && !proto->IsLoggedOn());
		}
		else
			outEnabled = false;
		break;

	// These can only have logged off protocol selection
	case cmd_RefreshAddressBook:
	case cmd_LogoffAddressBook:
		if (IsSingleSelection())
		{
			STableCell aCell(0,0);
			GetNextSelectedCell(aCell);
			CAdbkProtocol* proto = GetCellAdbkProtocol(aCell.row);
			outEnabled = ((proto != NULL) && proto->IsLoggedOn());
		}
		else
			outEnabled = false;
		break;

	// Always available
	case cmd_ToolbarAdbkMgrSearchBtn:
		outEnabled = true;
		break;

	default:
		CHierarchyTableDrag::FindCommandStatus(inCommand, outEnabled, outUsesMark, outMark, outName);
		break;
	}
}

// Click in the cell
void CAdbkManagerTable::ClickCell(const STableCell& inCell,
								const SMouseDownEvent& inMouseDown)
{
	// Determine which heading it is
	SColumnInfo col_info = mTableView->GetColumnInfo()[inCell.col - 1];

	switch(col_info.column_type)
	{
	case eAdbkColumnOpen:
	case eAdbkColumnResolve:
	case eAdbkColumnSearch:
		TableIndexT	woRow = mCollapsableTree->GetWideOpenIndex(inCell.row + TABLE_ROW_ADJUST);

		CAdbkList::node_type* node = GetCellNode(inCell.row);

		// Do status flag
		if (!inMouseDown.delaySelect && node->IsSelectable())
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

			RefreshCell(inCell);
		}
		break;
	default:
		// Look at user action and determine what to do
		if ((GetClickCount() > 1) && !inMouseDown.delaySelect)
			DoDoubleClick(inCell.row, CKeyModifiers(inMouseDown.macEvent.modifiers));
		else
			DoSingleClick(inCell.row, CKeyModifiers(inMouseDown.macEvent.modifiers));
		break;
	}
}

// Click
void CAdbkManagerTable::ClickSelf(const SMouseDownEvent &inMouseDown)
{
	SwitchTarget(this);

	// Must set background color to grey if tracking drop flag
	StColorState	saveColors;

	STableCell	hitCell;
	SPoint32	imagePt;

	LocalToImagePoint(inMouseDown.whereLocal, imagePt);

	if (GetCellHitBy(imagePt, hitCell))
	{
		CAdbkList::node_type* node = GetCellNode(hitCell.row);
		if ((hitCell.col == mHierarchyCol) && UsesBackgroundColor(node))
		{
			FocusDraw();
			ApplyForeAndBackColors();
			::RGBBackColor(&GetBackgroundColor(node));
		}
	}

	CHierarchyTableDrag::ClickSelf(inMouseDown);
}

// Draw the titles
void CAdbkManagerTable::DrawCell(const STableCell &inCell,
								const Rect &inLocalRect)
{
	// Not if changing
	if (mListChanging)
		return;

	TableIndexT	woRow = mCollapsableTree->GetWideOpenIndex(inCell.row + TABLE_ROW_ADJUST);

	CAdbkList::node_type* node = GetCellNode(inCell.row);

	// Save text state in stack object
	StTextState		textState;
	StColorState	saveColors;
	StColorPenState::Normalize();
	cdstring		theTxt;

	// Set to required text
	UTextTraits::SetPortTextTraits(&mTextTraits);

	// Clip to cell frame & table frame
	Rect	clipper = mRevealedRect;
	PortToLocalPoint(topLeft(clipper));
	PortToLocalPoint(botRight(clipper));
	::SectRect(&clipper, &inLocalRect, &clipper);
	StClipRgnState	clip(clipper);

	// Erase to ensure drag hightlight is overwritten
	// Erase to ensure drag hightlight is overwritten
	FocusDraw();
	if (UsesBackgroundColor(node))
	{
		Rect greyer = inLocalRect;
		greyer.bottom = greyer.top + 1;
		::EraseRect(&greyer);
		::RGBBackColor(&GetBackgroundColor(node));
		greyer = inLocalRect;
		greyer.top++;
		::EraseRect(&greyer);
	}
	else
		::EraseRect(&inLocalRect);

	// Determine which heading it is
	SColumnInfo col_info = mTableView->GetColumnInfo()[inCell.col - 1];

	switch(col_info.column_type)
	{

	case eAdbkColumnName:
	{
		// Always cache column number
		mHierarchyCol = inCell.col;
		SetOneColumnSelect(mHierarchyCol);

		DrawDropFlag(inCell, woRow);

#if PP_Target_Carbon
		// Draw selection
		bool selected_state = DrawCellSelection(inCell);
#else
		bool selected_state = false;
#endif

		UInt32	nestingLevel = mCollapsableTree->GetNestingLevel(woRow);
		Rect	iconRect;
		iconRect.left = inLocalRect.left + mFirstIndent + nestingLevel * mLevelIndent;
		iconRect.right = iconRect.left + 16;
		iconRect.bottom = inLocalRect.bottom - mIconDescent;
		iconRect.top = iconRect.bottom - 16;
		::Ploticns(&iconRect, atNone, selected_state ? ttSelected : ttNone, GetPlotIcon(node, GetCellAdbkProtocol(inCell.row)));

		// Get name of item
		theTxt = node->IsSelectable() ? node->GetSelectData()->GetName() : *node->GetNoSelectData();

		// Draw the string
		bool strike = false;
		SetTextStyle(node, GetCellAdbkProtocol(inCell.row), strike);
		::MoveTo(iconRect.right - 2, inLocalRect.bottom - mTextDescent);
		::DrawClippedStringUTF8(theTxt, inLocalRect.right - iconRect.right - 2, eDrawString_Left);
		if (strike)
		{
			::MoveTo(iconRect.right + 2, (inLocalRect.top + inLocalRect.bottom)/2);
			::LineTo(inLocalRect.right, (inLocalRect.top + inLocalRect.bottom)/2);
		}
		break;
	}

	case eAdbkColumnOpen:
	case eAdbkColumnResolve:
	case eAdbkColumnSearch:
	{
		Rect iconRect;
		iconRect.left = inLocalRect.left;
		iconRect.right = iconRect.left + 16;
		iconRect.bottom = inLocalRect.bottom - mIconDescent;
		iconRect.top = iconRect.bottom - 16;

		// Do status flag
		if (node->IsSelectable())
		{
			bool ticked = false;
			switch(col_info.column_type)
			{
			case eAdbkColumnOpen:
				ticked = node->GetSelectData()->IsOpenOnStart();
				break;
			case eAdbkColumnResolve:
				ticked = node->GetSelectData()->IsLookup();
				break;
			case eAdbkColumnSearch:
				ticked = node->GetSelectData()->IsSearch();
				break;
			}

			if (ticked)
				::Ploticns(&iconRect, atNone, ttNone, ICNx_DiamondTicked);
			else
				::Ploticns(&iconRect, atNone, ttNone, ICNx_Diamond);
		}
		else
			::EraseRect(&iconRect);
		break;
	}

	default:
		break;
	}
}

// Draw or undraw active hiliting for a Cell
void CAdbkManagerTable::HiliteCellActively(const STableCell &inCell, Boolean inHilite)
{
	// Not if changing
	if (mListChanging)
		return;

	CHierarchyTableDrag::HiliteCellActively(inCell, inHilite);
}

// Draw or undraw inactive hiliting for a Cell
void CAdbkManagerTable::HiliteCellInactively(const STableCell &inCell, Boolean inHilite)
{
	// Not if changing
	if (mListChanging)
		return;

	CHierarchyTableDrag::HiliteCellInactively(inCell, inHilite);
}

void CAdbkManagerTable::CalcCellFlagRect(const STableCell &inCell, Rect &outRect)
{
	if (inCell.col == mHierarchyCol)
	{
		LHierarchyTable::CalcCellFlagRect(inCell, outRect);
		outRect.right = outRect.left + 16;
		outRect.bottom = outRect.top + 12;
		::OffsetRect(&outRect, 0, 2);
	}
	else
		::SetRect(&outRect, 0, 0, 0, 0);
}

// Get appropriate icon id
ResIDT CAdbkManagerTable::GetPlotIcon(const CAdbkList::node_type* node, CAdbkProtocol* proto)
{
	if (!node->IsSelectable())
	{
		if (!proto)
			return ICNx_BrowseLocalHierarchy;
		else if (proto->CanDisconnect())
			return proto->IsDisconnected() ? ICNx_BrowseDisconnectedHierarchy : ICNx_BrowseRemoteHierarchy;
		else if ((proto->GetAccountType() == CINETAccount::eLocalAdbk) ||
					(proto->GetAccountType() == CINETAccount::eOSAdbk))
			return ICNx_BrowseLocalHierarchy;
		else
			return ICNx_BrowseRemoteHierarchy;
	}
	else
	{
		const CRemoteAddressBook* adbk = dynamic_cast<const CRemoteAddressBook*>(node->GetSelectData());
		if (adbk && adbk->GetProtocol()->CanDisconnect() && adbk->IsLocalAdbk())
			return adbk->IsCachedAdbk() ? ICNx_AddressBookCached : ICNx_AddressBookUncached;
		else
			return ICNx_AddressBook;
	}
}

// Get text style
void CAdbkManagerTable::SetTextStyle(const CAdbkList::node_type* node, CAdbkProtocol* proto, bool& strike)
{
	strike = false;

	// Select appropriate color and style of text
	if (UEnvironment::HasFeature(env_SupportsColor))
	{
		if (!node->IsSelectable())
		{
			bool color_set = false;
			RGBColor text_color;
			Style text_style = normal;

			if (!proto || proto->IsLoggedOn())
			{
				text_color = CPreferences::sPrefs->mServerOpenStyle.GetValue().color;
				color_set = true;
				text_style = text_style | CPreferences::sPrefs->mServerOpenStyle.GetValue().style & 0x007F;
				strike = strike || ((CPreferences::sPrefs->mServerOpenStyle.GetValue().style & 0x0080) != 0);
			}
			if (!color_set)
			{
				text_color = CPreferences::sPrefs->mServerClosedStyle.GetValue().color;
				color_set = true;
				text_style = text_style | CPreferences::sPrefs->mServerClosedStyle.GetValue().style & 0x007F;
				strike =  strike || ((CPreferences::sPrefs->mServerClosedStyle.GetValue().style & 0x0080) != 0);
			}

			::RGBForeColor(&text_color);
			::TextFace(text_style);
		}
		else
		{
			bool color_set = false;
			RGBColor text_color;
			Style text_style = normal;

			text_color = CPreferences::sPrefs->mMboxClosedStyle.GetValue().color;
			color_set = true;
			text_style = text_style | CPreferences::sPrefs->mMboxClosedStyle.GetValue().style & 0x007F;
			strike =  strike || ((CPreferences::sPrefs->mMboxClosedStyle.GetValue().style & 0x0080) != 0);

			::RGBForeColor(&text_color);
			::TextFace(text_style);
		}
	}
}

bool CAdbkManagerTable::UsesBackgroundColor(const CAdbkList::node_type* node) const
{
	return !node->IsSelectable();
}

const RGBColor& CAdbkManagerTable::GetBackgroundColor(const CAdbkList::node_type* node) const
{
	return CPreferences::sPrefs->mServerBkgndStyle.GetValue().color;
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
void CAdbkManagerTable::DoSingleClick(unsigned long row, const CKeyModifiers& mods)
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
void CAdbkManagerTable::DoDoubleClick(unsigned long row, const CKeyModifiers& mods)
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

// Reset the table
void CAdbkManagerTable::ResetTable()
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
	
	// Force toolbar update
	mTableView->RefreshToolbar();

} // CAdbkManagerTable::ResetTable

// Reset the table from the mboxList
void CAdbkManagerTable::ClearTable()
{
	// Delete all existing rows
	Clear();
	mData.clear();

	// Refresh list
	Refresh();
	
	// Force toolbar update
	mTableView->RefreshToolbar();
}

// Keep titles in sync
void CAdbkManagerTable::ScrollImageBy(SInt32 inLeftDelta, SInt32 inTopDelta, Boolean inRefresh)
{
	// Find titles in owner chain
	mTableView->GetBaseTitles()->ScrollImageBy(inLeftDelta, 0, inRefresh);

	CHierarchyTableDrag::ScrollImageBy(inLeftDelta, inTopDelta, inRefresh);
}

// Remove rows and adjust parts
void CAdbkManagerTable::RemoveRows(UInt32 inHowMany, TableIndexT inFromRow, Boolean inRefresh)
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
	mDropRemoteAdbk = NULL;
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
		mDropAdbkWnd->GetAddressBookView()->ResetTable();
	
	// Always reset if remote was opened
	if (mDropRemoteAdbk)
	{
		mDropRemoteAdbk->GetProtocol()->CloseAdbk(mDropRemoteAdbk);
		mDropRemoteAdbk->Clear();
		mDropRemoteAdbk->SetFlags(CAddressBook::eLoaded, false);
		mDropRemoteAdbk = NULL;
	}
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

		// Look for closed remote address book
		if (!mDropAdbk->IsOpen())
			mDropRemoteAdbk = dynamic_cast<CRemoteAddressBook*>(mDropAdbk);
		if (mDropRemoteAdbk)
			mDropRemoteAdbk->Read();

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
				CAddress* theAddr = theEnv->GetFrom()->front();
				if (!CPreferences::sPrefs->TestSmartAddress(*theAddr) &&
					!mDropAdbk->GetAddressList()->IsDuplicate(theAddr))
				{
					// Add to list
					mDropAdbk->AddAddress(new CAdbkAddress(*theAddr), mDropSort);
					mAddressAdded = true;
				}
			}

			// Add Reply-To
			if (theEnv->GetReplyTo()->size())
			{
				CAddress* theAddr = theEnv->GetReplyTo()->front();
				if (!CPreferences::sPrefs->TestSmartAddress(*theAddr) &&
					!mDropAdbk->GetAddressList()->IsDuplicate(theAddr))
				{
					// Add to list
					mDropAdbk->AddAddress(new CAdbkAddress(*theAddr), mDropSort);
					mAddressAdded = true;
				}
			}

			// Add all To: except me
			for(CAddressList::iterator iter = theEnv->GetTo()->begin(); iter != theEnv->GetTo()->end(); iter++)
			{
				CAddress* theAddr = *iter;
				if (!CPreferences::sPrefs->TestSmartAddress(*theAddr) &&
					!mDropAdbk->GetAddressList()->IsDuplicate(theAddr))
				{
					// Add to list
					mDropAdbk->AddAddress(new CAdbkAddress(*theAddr), mDropSort);
					mAddressAdded = true;
				}
			}

			// Add all Ccs except me
			for(CAddressList::iterator iter = theEnv->GetCC()->begin(); iter != theEnv->GetCC()->end(); iter++)
			{
				CAddress* theAddr = *iter;
				if (!CPreferences::sPrefs->TestSmartAddress(*theAddr) &&
					!mDropAdbk->GetAddressList()->IsDuplicate(theAddr))
				{
					// Add to list
					mDropAdbk->AddAddress(new CAdbkAddress(**iter), mDropSort);
					mAddressAdded = true;
				}
			}

			break;

		case cDragAddrType:
			CAddress* theAddr = *((CAddress**) drag_data);
			if (!CPreferences::sPrefs->TestSmartAddress(*theAddr) &&
				!mDropAdbk->GetAddressList()->IsDuplicate(theAddr))
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

		case kScrapFlavorTypeText:

			// Parse text into list
			CAddressList list(drag_data, data_size);

			// Try to add these to table
			for(CAddressList::iterator iter = list.begin(); iter != list.end(); iter++)
			{
				CAddress* theAddr = *iter;
				if (!CPreferences::sPrefs->TestSmartAddress(*theAddr) &&
					!mDropAdbk->GetAddressList()->IsDuplicate(theAddr))
				{
					// Now add to address book
					mDropAdbk->AddAddress(new CAdbkAddress(*theAddr), mDropSort);
					mAddressAdded = true;
				}
			}
			break;
	}
}
