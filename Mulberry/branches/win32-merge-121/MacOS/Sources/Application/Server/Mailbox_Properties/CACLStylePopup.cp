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


// Source for CACLStylePopup class

#include "CACLStylePopup.h"

#include "CGetStringDialog.h"
#include "CMailboxPropDialog.h"
#include "CMulberryCommon.h"
#include "CPreferences.h"
#include "CTextListChoice.h"
#include "CXStringResources.h"

// __________________________________________________________________________________________________
// C L A S S __ C A C L S T Y L E P O P U P
// __________________________________________________________________________________________________

// C O N S T R U C T I O N / D E S T R U C T I O N  M E T H O D S

// Constructor from stream
CACLStylePopup::CACLStylePopup(LStream *inStream)
		: CPickPopup(inStream)
{
}

// Default destructor
CACLStylePopup::~CACLStylePopup()
{
}


// O T H E R  M E T H O D S ____________________________________________________________________________

// Reset items
void CACLStylePopup::Reset(bool mbox)
{
	// Cache appropriate list
	mMbox = mbox;
	mList = (mbox ? &CPreferences::sPrefs->mMboxACLStyles : &CPreferences::sPrefs->mAdbkACLStyles);

	// Delete previous items
	MenuHandle menuH = GetMacMenuH();
	for(short i = ::CountMenuItems(menuH); i >= 4; i--)
		::DeleteMenuItem(menuH, i);

	// Add each style item
	short menu_pos = 4;
	for(SACLStyleList::const_iterator iter = mList->GetValue().begin(); iter != mList->GetValue().end(); iter++, menu_pos++)
		::AppendItemToMenu(menuH, menu_pos, (*iter).first, true);

	// Force max/min update
	SetMenuMinMax();

	// Force value off
	mValue = 0;
}

// Add new style
void CACLStylePopup::DoNewStyle(SACLRight rights)
{
	// Get a new name for the mailbox (use old name as starter)
	cdstring style_name;
	if (CGetStringDialog::PoseDialog("Alerts::Server::SaveStyle", style_name))
	{
		// Add style to list and menu
		SACLStyle style = make_pair(style_name, rights);
		mList->Value().push_back(style);
		mList->SetDirty();

		// Add last style item
		MenuHandle menuH = GetMacMenuH();
		short menu_pos = ::CountMenuItems(menuH) + 1;

		// Insert default item
		::AppendItemToMenu(menuH, menu_pos, style_name, true);

		SetMenuMinMax();
	}
}

// Delete existing styles
void CACLStylePopup::DoDeleteStyle()
{
	// Get list of styles
	cdstrvect items;
	for(SACLStyleList::const_iterator iter = mList->GetValue().begin(); iter != mList->GetValue().end(); iter++)
		items.push_back((*iter).first);

	// Do the dialog
	ulvector styles;
	if (CTextListChoice::PoseDialog("Alerts::Server::DeleteACLTitle", "Alerts::Server::DeleteACLDesc", NULL, false, false, false, false, items, cdstring::null_str, styles, "Alerts::Server::DeleteACLButton"))
	{
		// Remove in reverse order to stay in sync
		for(ulvector::reverse_iterator iter = styles.rbegin(); iter != styles.rend(); iter++)
		{
			mList->Value().erase(mList->Value().begin() + *iter);
			mList->SetDirty();
		}

		// Reset menu items
		Reset(mMbox);
	}
}
