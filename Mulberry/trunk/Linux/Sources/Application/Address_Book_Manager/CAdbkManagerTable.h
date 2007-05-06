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
#include "vector.h"

// Classes
class CAdbkProtocol;
class CAdbkManagerView;
class CAdbkManagerWindow;
class CAddressBook;
class CAddressBookManager;
class CAddressBookWindow;
class CKeyModifiers;

class CAdbkManagerTable : public CHierarchyTableDrag, public CListener
{

	friend class CAdbkManagerWindow;
	friend class CAdbkManagerView;

private:
	CAddressBookManager*	mManager;				// Manager controlling window
	CAdbkManagerView*		mTableView;				// Owner view
	bool					mListChanging;			// In the process of changing the list
	bool					mHasOthers;				// Indicates 'Other' item exists
	bool					mDropFirst;				// Indicates first item in drop
	bool					mDropSort;				// Drop with sort
	CAddressBook*			mDropAdbk;				// Address book to drop into
	CAddressBookWindow*		mDropAdbkWnd;			// Address book window dropped onto
	bool					mAddressAdded;			// Indicates address added
	bool					mGroupAdded;			// Indicates group added
	vector<void*>			mData;					// data

public:

	CAdbkManagerTable(JXScrollbarSet* scrollbarSet, 
						JXContainer* enclosure,
						const HSizingOption hSizing, 
						const VSizingOption vSizing,
						const JCoordinate x, const JCoordinate y,
						const JCoordinate w, const JCoordinate h);
	virtual 		~CAdbkManagerTable();

	virtual void	ListenTo_Message(long msg, void* param);	// Respond to list changes

	SColumnInfo GetColumnInfo(TableIndexT col);

protected:
	virtual void OnCreate();
	virtual bool HandleChar(const int key, const JXKeyModifiers& modifiers);
	virtual bool ObeyCommand(unsigned long cmd, SMenuCommandChoice* menu = NULL);
	virtual void UpdateCommand(unsigned long cmd, CCmdUI* cmdui);

	// Common updaters
	void	OnUpdateAllAdbk(CCmdUI* cmdui);
	void	OnUpdateAdbkSelection(CCmdUI* cmdui);		
	void	OnUpdateLoggedInSelection(CCmdUI* cmdui);	
	void	OnUpdateLoggedOutSelection(CCmdUI* cmdui);	
	void	OnUpdateDisconnectedSelection(CCmdUI* cmdui);	
	void	OnUpdateClearDisconnectedSelection(CCmdUI* cmdui);	

	bool TestSelectionAdbk(TableIndexT row);						// Test for selected adbk
	bool TestSelectionAdbkDisconnected(TableIndexT row);			// Test for selected adbk
	bool TestSelectionAdbkClearDisconnected(TableIndexT row);		// Test for selected adbk

	virtual void	LClickCell(const STableCell& inCell, const JXKeyModifiers& mods); // Clicked item
	virtual void	LDblClickCell(const STableCell& inCell, const JXKeyModifiers& mods); // Double Clicked item

	virtual void DrawCell(JPainter* pDC, const STableCell& inCell,
								const JRect& inLocalRect);// Draw the message info

public:
	virtual void		ScrollImageBy(SInt32 inLeftDelta, SInt32 inTopDelta, bool inRefresh);

	virtual void		CollapseRow(UInt32 inWideOpenRow);
	virtual void		DeepCollapseRow(UInt32 inWideOpenRow);
	
	virtual void		ExpandRow(UInt32 inWideOpenRow);
	virtual void		DeepExpandRow(UInt32 inWideOpenRow);

	virtual void		ProcessExpansion(UInt32 inWideOpenRow, bool expand);

	void	SetManager(CAddressBookManager* manager);				// Set the mail server

protected:
	void		DoSingleClick(TableIndexT row, const CKeyModifiers& mods);				// Handle click user action
	void		DoDoubleClick(TableIndexT row, const CKeyModifiers& mods);				// Handle double-click user action
	void		DoPreview();
	void		DoPreview(CAddressBook* adbk);
	void		DoFullView();

	void	PreviewAddressBook(bool clear = false);					// Do preview of address book
	void	PreviewAddressBook(CAddressBook* adbk, bool clear = false);		// Do preview of address bookw

	void	OnAddressBookProperties();							// Get properties of selected address books

	void	OnNewAddressBook();

	void	OnOpenAddressBook();
	bool	OpenAddressBook(TableIndexT row);						// Display a specified address book

	void	OnRenameAddressBook();
	bool	RenameAddressBook(TableIndexT row);						// Rename a specified address book

	void	OnDeleteAddressBook();
	bool	DeleteAddressBook(TableIndexT row);						// Delete a specified address book

	void	OnSearchAddressBook();								// Search address books

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
	bool					IsCellAdbk(TableIndexT row);							// Check for adbk
	CAdbkList::node_type*	GetCellNode(TableIndexT row);			// Get the selected node
	CAddressBook*			GetCellAdbk(TableIndexT row);					// Get the selected adbk
	CAdbkProtocol*			GetCellAdbkProtocol(TableIndexT row);			// Get the selected adbk protocol

	void*	GetCellData(TableIndexT woRow);							// Get the selected adbk

	JIndex GetPlotIcon(const CAdbkList::node_type* node,
									CAdbkProtocol* proto);			// Get appropriate icon id
	void	SetTextStyle(JPainter* pDC, const CAdbkList::node_type* node,
							CAdbkProtocol* proto, bool& strike);	// Get appropriate text style
	bool 	UsesBackgroundColor(const CAdbkList::node_type* node) const;
	JColorIndex GetBackgroundColor(const CAdbkList::node_type* node) const;

	bool	AddSelectionToList(TableIndexT row,
										CFlatAdbkList* list);		// Add selected address books to list

public:
	virtual void	DoSelectionChanged();

	virtual void	ResetTable();								// Reset the table from the mboxList
	virtual void	ClearTable();								// Clear the table

	virtual void	AddNode(const CAdbkList::node_type* node,
							TableIndexT& row, bool child, bool refresh = false);	// Add a node to the list
	virtual void	AddChildren(const CAdbkList::node_type* node,
							TableIndexT& parent_row, bool refresh = false);			// Add child nodes to the list
	virtual void	RemoveChildren(TableIndexT& parent_row, bool refresh = false);	// Remove child nodes from the list
	virtual void	RemoveRows(UInt32 inHowMany, TableIndexT inFromRow, bool inRefresh);

			void	AddProtocol(CAdbkProtocol* proto);
			void	RemoveProtocol(CAdbkProtocol* proto);
			void	ClearProtocol(CAdbkProtocol* proto);
			void	RefreshProtocol(CAdbkProtocol* proto);
			void	LogoffProtocol(CAdbkProtocol* proto);

// Drag methods
protected:
#if NOTYET
	virtual bool	IsCopyCursor(DragReference inDragRef);			// Use copy cursor?
	virtual bool	IsDropCell(DragReference inDragRef, STableCell row);// Can cell be dropped into
	virtual void	DoDragReceive(DragReference	inDragRef);			// Get multiple text items
	virtual void	DropDataIntoCell(FlavorType theFlavor,
										char* drag_data,
										Size data_size,
										const STableCell& theCell);		// Drop data into cell
#endif
};

#endif
