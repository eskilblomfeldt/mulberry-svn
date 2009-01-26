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


// Header for CAdbkManagerTable class

#ifndef __CADBKMANAGERTABLE__MULBERRY__
#define __CADBKMANAGERTABLE__MULBERRY__

#include "CHierarchyTableDrag.h"
#include "CListener.h"

#include "CAdbkList.h"
#include "cdstring.h"

// Consts

// Panes

// Messages

// Resources
const	ResIDT		Txtr_DefaultAdbkList = 1000;

// Classes
class CAdbkProtocol;
class CAdbkManagerView;
class CAdbkManagerWindow;
class CAddressBook;
class CAddressBookManager;
class CAddressBookWindow;
class CKeyModifiers;
class CRemoteAddressBook;

class CAdbkManagerTable : public CHierarchyTableDrag, public CListener
{
	friend class CAdbkManagerWindow;
	friend class CAdbkManagerView;

private:
	CAddressBookManager*	mManager;				// Manager controlling window
	CAdbkManagerView*		mTableView;				// Owner view
	bool					mListChanging;			// In the process of changing the list
	vector<void*>			mData;					// data
	short					mHierarchyCol;			// Column containing names
	bool					mHasOthers;				// Indicates 'Other' item exists
	bool					mDropFirst;				// Indicates first item in drop
	bool					mDropSort;				// Drop with sort
	CAddressBook*			mDropAdbk;				// Address book to drop into
	CRemoteAddressBook*		mDropRemoteAdbk;		// Remote address book opened
	CAddressBookWindow*		mDropAdbkWnd;			// Address book window dropped onto
	bool					mAddressAdded;			// Indicates address added
	bool					mGroupAdded;			// Indicates group added

public:
	enum { class_ID = 'AmLi' };

					CAdbkManagerTable();
					CAdbkManagerTable(LStream *inStream);
	virtual 		~CAdbkManagerTable();

private:
			void	InitAdbkManagerTable();

protected:
	virtual void		FinishCreateSelf();			// Get details of sub-panes
public:
	virtual Boolean		HandleKeyPress(const EventRecord &inKeyEvent);
	virtual Boolean		ObeyCommand(CommandT inCommand, void *ioParam = nil);
	virtual void		FindCommandStatus(CommandT inCommand,
											Boolean &outEnabled,
											Boolean &outUsesMark,
											UInt16 &outMark,
											Str255 outName);

	virtual void		ListenTo_Message(long msg, void* param);	// Respond to list changes

private:
	bool TestSelectionAdbk(TableIndexT row);						// Test for selected adbk
	bool TestSelectionAdbkDisconnected(TableIndexT row);			// Test for selected adbk
	bool TestSelectionAdbkClearDisconnected(TableIndexT row);		// Test for selected adbk

protected:

	virtual void	ClickSelf(const SMouseDownEvent &inMouseDown);	// Click
	virtual void	ClickCell(const STableCell& inCell,
								const SMouseDownEvent& inMouseDown);// Click in the cell

	virtual void	DrawCell(const STableCell &inCell,
								const Rect &inLocalRect);			// Draw the message info

	virtual void	HiliteCellActively(const STableCell &inCell,
										Boolean inHilite);
	virtual void	HiliteCellInactively(const STableCell &inCell,
										Boolean inHilite);

	virtual void	CalcCellFlagRect(const STableCell &inCell,
										Rect &outRect);						

public:
	virtual void		CollapseRow(TableIndexT inWideOpenRow);
	virtual void		DeepCollapseRow(TableIndexT inWideOpenRow);
	
	virtual void		ExpandRow(TableIndexT inWideOpenRow);
	virtual void		DeepExpandRow(TableIndexT inWideOpenRow);

	virtual void		ProcessExpansion(TableIndexT inWideOpenRow, bool expand);

	void	SetManager(CAddressBookManager* manager);			// Set the mail server

protected:
	virtual void	DoSelectionChanged(void);

	void		DoSingleClick(unsigned long row, const CKeyModifiers& mods);				// Handle click user action
	void		DoDoubleClick(unsigned long row, const CKeyModifiers& mods);				// Handle double-click user action
	void		DoPreview();
	void		DoPreview(CAddressBook* adbk);
	void		DoFullView();

	void	PreviewAddressBook(bool clear = false);							// Do preview of address book
	void	PreviewAddressBook(CAddressBook* adbk, bool clear = false);		// Do preview of address book

	void	OnAddressBookProperties();								// Get properties of selected address books

	void	OnNewAddressBook();

	void	OnOpenAddressBook();
	bool	OpenAddressBook(TableIndexT row);						// Display a specified address book

	void	OnRenameAddressBook();
	bool	RenameAddressBook(TableIndexT row);						// Rename a specified address book

	void	OnDeleteAddressBook();
	bool	DeleteAddressBook(TableIndexT row);						// Delete a specified address book

	void	OnSearchAddressBook();									// Search address books

	void	OnLoginAddressBook();
	bool	LoginAddressBook(TableIndexT row);						// Logon to servers

	void	OnLogoutAddressBook();
	bool	LogoutAddressBook(TableIndexT row);						// Logoff servers

	void	OnRefreshAddressBook();
	bool	RefreshAddressBook(TableIndexT row);					// Refresh server list

	void	OnSynchroniseAddressBook();
	bool	SynchroniseAddressBook(TableIndexT row);				// Synchronise address book

	void	OnClearDisconnectAddressBook();
	bool	ClearDisconnectAddressBook(TableIndexT row);			// ClearDisconnected address book

private:
	bool	IsCellAdbk(TableIndexT row);							// Check for adbk
	CAdbkList::node_type*	GetCellNode(TableIndexT row);			// Get the selected node
	CAddressBook*	GetCellAdbk(TableIndexT row);					// Get the selected adbk
	CAdbkProtocol*	GetCellAdbkProtocol(TableIndexT row);			// Get the selected adbk protocol

	ResIDT	GetPlotIcon(const CAdbkList::node_type* node,
									CAdbkProtocol* proto);			// Get appropriate icon id
	void	SetTextStyle(const CAdbkList::node_type* node,
							CAdbkProtocol* proto, bool& strike);	// Get appropriate text style
	bool 	UsesBackgroundColor(const CAdbkList::node_type* node) const;
	const RGBColor& GetBackgroundColor(const CAdbkList::node_type* node) const;

	void*	GetCellData(TableIndexT woRow);							// Get the selected adbk

	bool	AddSelectionToList(TableIndexT row,
										CFlatAdbkList* list);		// Add selected address books to list

public:
	virtual void	ResetTable();								// Reset the table
	virtual void	ClearTable();								// Clear the table
	virtual void	ScrollImageBy(SInt32 inLeftDelta,
									SInt32 inTopDelta,
									Boolean inRefresh);				// Keep titles in sync

	virtual void	AddNode(const CAdbkList::node_type* node,
							TableIndexT& row, bool child, bool refresh = false);	// Add a node to the list
	virtual void	AddChildren(const CAdbkList::node_type* node,
							TableIndexT& parent_row, bool refresh = false);			// Add child nodes to the list
	virtual void	RemoveChildren(TableIndexT& parent_row, bool refresh = false);	// Remove child nodes from the list
	virtual void	RemoveRows(UInt32 inHowMany, TableIndexT inFromRow, Boolean inRefresh);

			void	AddProtocol(CAdbkProtocol* proto);
			void	RemoveProtocol(CAdbkProtocol* proto);
			void	ClearProtocol(CAdbkProtocol* proto);
			void	RefreshProtocol(CAdbkProtocol* proto);
			void	LogoffProtocol(CAdbkProtocol* proto);

// Drag methods
protected:
	virtual bool	IsCopyCursor(DragReference inDragRef);			// Use copy cursor?
	virtual bool	IsDropCell(DragReference inDragRef, STableCell row);// Can cell be dropped into
	virtual void	DoDragReceive(DragReference	inDragRef);			// Get multiple text items
	virtual void	DropDataIntoCell(FlavorType theFlavor,
										char* drag_data,
										Size data_size,
										const STableCell& theCell);		// Drop data into cell
};

#endif
