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


// Source for CCreateAdbkDialog class

#include "CCreateAdbkDialog.h"

#include "CAdminLock.h"
#include "CAdbkProtocol.h"
#include "CAddressBookManager.h"
#include "CMulberryCommon.h"
#include "CPreferences.h"
#include "CResizeNotifier.h"
#include "CTextField.h"

#include "TPopupMenu.h"

#include <JXWindow.h>
#include <JXUpRect.h>
#include <JXStaticText.h>
#include <JXTextButton.h>
#include <JXTextRadioButton.h>
#include <JXRadioGroup.h>
#include <JXTextCheckbox.h>

#include <jXGlobals.h>

#include <cassert>

// C O N S T R U C T I O N / D E S T R U C T I O N  M E T H O D S

// Default constructor
CCreateAdbkDialog::CCreateAdbkDialog(JXDirector* supervisor)
	: CDialogDirector(supervisor)
{
	mHasLocal = false;
	mHasRemote = false;
}

// Default destructor
CCreateAdbkDialog::~CCreateAdbkDialog()
{
}

// O T H E R  M E T H O D S ____________________________________________________________________________

// Get details of sub-panes
void CCreateAdbkDialog::OnCreate()
{
// begin JXLayout

    JXWindow* window = new JXWindow(this, 435,295, "");
    assert( window != NULL );
    SetWindow(window);

    CResizeNotifier* obj1 =
        new CResizeNotifier(window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 435,295);
    assert( obj1 != NULL );

    JXStaticText* obj2 =
        new JXStaticText("Address Book Name:", obj1,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 15,15, 145,20);
    assert( obj2 != NULL );

    mAdbkName =
        new CTextInputField(obj1,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 15,35, 390,20);
    assert( mAdbkName != NULL );

    mTypeGroup =
        new JXRadioGroup(obj1,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,75, 415,60);
    assert( mTypeGroup != NULL );

    mTypeLabel =
        new JXStaticText("Type:", obj1,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 15,65, 40,15);
    assert( mTypeLabel != NULL );

    mPersonal =
        new JXTextRadioButton(ePersonal, "Personal Address Book", mTypeGroup,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 165,20);
    assert( mPersonal != NULL );

    mGlobal =
        new JXTextRadioButton(eGlobal, "Global Address Book", mTypeGroup,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,29, 150,20);
    assert( mGlobal != NULL );

    JXStaticText* obj3 =
        new JXStaticText("In Account:", obj1,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 10,152, 80,20);
    assert( obj3 != NULL );

    mAccountMenu =
        new HPopupMenu( "", obj1,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 95,150, 250,25);
    assert( mAccountMenu != NULL );

    mAccount =
        new CTextInputField(obj1,
                    JXWidget::kHElastic, JXWidget::kVElastic, 95,150, 310,20);
    assert( mAccount != NULL );

    mOpenOnStartup =
        new JXTextCheckbox("Open on Startup", obj1,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 10,185, 130,20);
    assert( mOpenOnStartup != NULL );

    mUseNicknames =
        new JXTextCheckbox("Use for Nick-names", obj1,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 10,205, 145,20);
    assert( mUseNicknames != NULL );

    mUseSearch =
        new JXTextCheckbox("Use for Searching and Expansion", obj1,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 10,225, 220,20);
    assert( mUseSearch != NULL );

    mCancelBtn =
        new JXTextButton("Cancel", obj1,
                    JXWidget::kFixedRight, JXWidget::kFixedBottom, 255,260, 70,25);
    assert( mCancelBtn != NULL );
    mCancelBtn->SetShortcuts("^[");

    mOKBtn =
        new JXTextButton("OK", obj1,
                    JXWidget::kFixedRight, JXWidget::kFixedBottom, 345,260, 70,25);
    assert( mOKBtn != NULL );
    mOKBtn->SetShortcuts("^M");

// end JXLayout

	window->SetTitle("Create Address Book");
	SetButtons(mOKBtn, mCancelBtn);

	ListenTo(obj1);

	mAccount->Deactivate();
	InitAccountMenu();
	ListenTo(mAccountMenu);

}

void CCreateAdbkDialog::Receive(JBroadcaster* sender, const Message& message)
{
	if (message.Is(JXMenu::kItemSelected) && sender == mAccountMenu)
	{
    	const JXMenu::ItemSelected* is = dynamic_cast<const JXMenu::ItemSelected*>(&message);
    	OnAccountPopup(is->GetIndex());
    	return;
	}

	CDialogDirector::Receive(sender, message);
}

// Called during idle
void CCreateAdbkDialog::InitAccountMenu(void)
{
	// Delete previous items
	mAccountMenu->RemoveAllItems();

	JIndex menu_pos = 1;
	if (!CAdminLock::sAdminLock.mNoLocalAdbks)
	{
		mAccountMenu->AppendItem(CPreferences::sPrefs->mLocalAdbkAccount.GetValue().GetName(), kFalse, kTrue);
		mHasLocal = true;
		menu_pos++;
	}
	
	// Add each adbk account (only IMSP/ACAP)
	for(CAddressAccountList::const_iterator iter = CPreferences::sPrefs->mAddressAccounts.GetValue().begin();
			iter != CPreferences::sPrefs->mAddressAccounts.GetValue().end(); iter++)
	{
		// Only if IMSP/ACAP
		if (((*iter)->GetServerType() != CINETAccount::eIMSP) &&
			((*iter)->GetServerType() != CINETAccount::eACAP))
			continue;

		// Add to menu
		mAccountMenu->AppendItem((*iter)->GetName(), kFalse, kTrue);
		mHasRemote = true;
		
		// Disable if not logged in
		if (!CAddressBookManager::sAddressBookManager->GetProtocol((*iter)->GetName())->IsLoggedOn())
			mAccountMenu->DisableItem(menu_pos);
		
		// Update menu id here after we have added an actual item
		menu_pos++;
	}

	mAccountMenu->SetToPopupChoice(kTrue, 1);
}

// Account popup changed
void CCreateAdbkDialog::OnAccountPopup(JIndex index)
{
	// Enable personal only for IMSP/ACAP accounts
	if (mHasLocal && (index == 1))
		mTypeGroup->Deactivate();
	else
	{
		mTypeGroup->Activate();
		
		cdstring protoname = mAccountMenu->GetItemText(mAccountMenu->GetValue()).GetCString();
		CAdbkProtocol* proto = CAddressBookManager::sAddressBookManager->GetProtocol(protoname);

		cdstring default_name = proto->GetUserPrefix();
		default_name += proto->GetAccount()->GetAuthenticator().GetAuthenticator()->GetActualUID();
		bool personal_allowed = !default_name.empty();

		if (!personal_allowed)
		{
			mPersonal->Deactivate();
			mTypeGroup->SelectItem(eGlobal);
		}
	}
}

// Set the details
void CCreateAdbkDialog::SetDetails(SCreateAdbk* create)
{
	// If no account use the popup
	if (create->account.empty())
	{
		mAccount->Hide();
		
		// Make sure subscribe option is properly setup
		OnAccountPopup(1);
	}
	else
	{
		mAccountMenu->Hide();
		mAccount->SetText(create->account);
		
		// Disable items if the one selected is the local account
		if (create->account == CPreferences::sPrefs->mLocalAdbkAccount.GetValue().GetName())
			mTypeGroup->Deactivate();
	}

	mOpenOnStartup->SetState(JBoolean(create->open_on_startup));
	mUseNicknames->SetState(JBoolean(create->use_nicknames));
	mUseSearch->SetState(JBoolean(create->use_search));
	
	// Hide type items if local
	if (mHasLocal && !mHasRemote)
	{
		JPoint pt1 = mTypeGroup->GetBoundsGlobal().topLeft();
		JPoint pt2 = mAccountMenu->GetBoundsGlobal().topLeft();
		JCoordinate resizeby = pt1.y - pt2.y;

		mTypeGroup->Hide();
		mTypeLabel->Hide();
		
		AdjustSize(0, resizeby);
	}
}

// Get the details
void CCreateAdbkDialog::GetDetails(SCreateAdbk* result)
{
	result->name = mAdbkName->GetText();

	result->personal = (mTypeGroup->GetSelectedItem() == ePersonal);
	result->open_on_startup = mOpenOnStartup->IsChecked();
	result->use_nicknames = mUseNicknames->IsChecked();
	result->use_search = mUseSearch->IsChecked();

	// Get account if not specified
	if (result->account.empty())
		result->account = mAccountMenu->GetItemText(mAccountMenu->GetValue());
}

// Do the dialog
bool CCreateAdbkDialog::PoseDialog(SCreateAdbk* details)
{
	bool result = false;

	CCreateAdbkDialog* dlog = new CCreateAdbkDialog(JXGetApplication());
	dlog->OnCreate();
	dlog->SetDetails(details);

	// Test for OK
	if (dlog->DoModal(false) == kDialogClosed_OK)
	{
		dlog->GetDetails(details);
		result = true;
		dlog->Close();
	}

	return result;
}
