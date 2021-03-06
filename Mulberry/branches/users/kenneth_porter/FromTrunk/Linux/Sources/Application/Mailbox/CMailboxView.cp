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


// Source for CMailboxView class

#include "CMailboxView.h"

#include "CContextMenu.h"
#include "CMailboxWindow.h"
#include "CMbox.h"
#include "CMessageView.h"
#include "CServerView.h"

#include "JXMultiImageCheckbox.h"

// __________________________________________________________________________________________________
// C L A S S __ C M A I L B O X W I N D O W
// __________________________________________________________________________________________________

// Static members

cdmutexprotect<CMailboxView::CMailboxViewList> CMailboxView::sMailboxViews;

// C O N S T R U C T I O N / D E S T R U C T I O N  M E T H O D S

// Default constructor
CMailboxView::CMailboxView(JXContainer* enclosure,
							const HSizingOption hSizing, const VSizingOption vSizing,
							const JCoordinate x, const JCoordinate y,
							const JCoordinate w, const JCoordinate h)
	: CTableView(enclosure, hSizing, vSizing, x, y, w, h)
{
	mToolbar = NULL;
	mServerView = NULL;
	mMessageView = NULL;
	mUsePreview = true;
	mPreviewInit = false;
	mUseSubstitute = false;
	mSubstituteIndex = 0;
	mSubstituteAccess = 1;
	mUseDynamicTab = false;
	mSortBtn = NULL;

	// Add to list
	{
		cdmutexprotect<CMailboxViewList>::lock _lock(sMailboxViews);
		sMailboxViews->push_back(this);
	}
}

// Default destructor
CMailboxView::~CMailboxView()
{
	// Remove from list
	cdmutexprotect<CMailboxViewList>::lock _lock(sMailboxViews);
	CMailboxViewList::iterator found = ::find(sMailboxViews->begin(), sMailboxViews->end(), this);
	if (found != sMailboxViews->end())
		sMailboxViews->erase(found);
}

// O T H E R  M E T H O D S ____________________________________________________________________________

// Setup help balloons
void CMailboxView::OnCreate()
{
	// Do inherited
	SetWindowInfo(eMboxColumnNumber, 32, false);
	CTableView::OnCreate();
}

void CMailboxView::Receive(JBroadcaster* sender, const Message& message) 
{
	if (message.Is(JXCheckbox::kPushed))
	{
		if (sender == mSortBtn)
		{
			GetTable()->OnSortDirection();
			return;
		}
	}
	CTableView::Receive(sender, message);
}
			
// Set preview pane
void CMailboxView::SetPreview(CMessageView* view)
{
	mMessageView = view;
	
	// Make sure preview's super-commander is the table to get command handling done right
	if (mMessageView != NULL)
		mMessageView->SetSuperCommander(GetTable());
}

// Force toggle of sort direction
void CMailboxView::ToggleShowBy()
{
	if (mSortBtn)
		mSortBtn->SetState(JBoolean(!mSortBtn->IsChecked()));
}

// Refresh Smart Address column
void CMailboxView::RefreshSmartAddress(void)
{
	// Force mailbox to reset smart address flags on all messages
	CMbox* mbox = GetMbox();
	if (!mbox)
		return;

	mbox->ClearSmartAddress();

	// Look for smart address column and refresh it
	short col = 1;
	for(CColumnInfoArray::const_iterator iter = mColumnInfo.begin();
		iter != mColumnInfo.end(); iter++)
	{
		SColumnInfo col_info = *iter;

		if (col_info.column_type == eMboxColumnSmart)
			GetTable()->RefreshCol(col);
		col++;
	}
}

void CMailboxView::FocusOwnerView()
{
	if (mServerView)
	{
		// If currently active, force focus to owner view
		if (HasFocus())
			mServerView->Focus();
	}
}
