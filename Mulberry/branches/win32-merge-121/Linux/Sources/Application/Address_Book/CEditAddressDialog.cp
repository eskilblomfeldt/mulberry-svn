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


// CEditAddressDialog.cpp : implementation file
//


#include "CEditAddressDialog.h"

#include "CAdbkAddress.h"
#include "CStringUtils.h"
#include "CTextDisplay.h"
#include "CTextField.h"

#include <JXStaticText.h>
#include <JXTextButton.h>
#include <JXUpRect.h>
#include <JXWindow.h>

#include <jXGlobals.h>

#include <cassert>

/////////////////////////////////////////////////////////////////////////////
// CEditAddressDialog dialog


CEditAddressDialog::CEditAddressDialog(JXDirector* supervisor)
	: CDialogDirector(supervisor)
{
}

/////////////////////////////////////////////////////////////////////////////
// CEditAddressDialog message handlers

void CEditAddressDialog::OnCreate()
{
// begin JXLayout

    JXWindow* window = new JXWindow(this, 545,290, "");
    assert( window != NULL );
    SetWindow(window);

    JXUpRect* obj1 =
        new JXUpRect(window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 545,290);
    assert( obj1 != NULL );

    JXStaticText* obj2 =
        new JXStaticText("Full Name:", obj1,
                    JXWidget::kHElastic, JXWidget::kVElastic, 5,12, 70,20);
    assert( obj2 != NULL );

    mFullName =
        new CTextInputField(obj1,
                    JXWidget::kHElastic, JXWidget::kVElastic, 85,10, 200,20);
    assert( mFullName != NULL );

    JXStaticText* obj3 =
        new JXStaticText("Nick-Name:", obj1,
                    JXWidget::kHElastic, JXWidget::kVElastic, 5,37, 80,20);
    assert( obj3 != NULL );

    mNickName =
        new CTextInputField(obj1,
                    JXWidget::kHElastic, JXWidget::kVElastic, 85,35, 200,20);
    assert( mNickName != NULL );

    JXStaticText* obj4 =
        new JXStaticText("Email:", obj1,
                    JXWidget::kHElastic, JXWidget::kVElastic, 5,62, 55,20);
    assert( obj4 != NULL );

    mEmail =
        new CTextInputField(obj1,
                    JXWidget::kHElastic, JXWidget::kVElastic, 85,60, 200,20);
    assert( mEmail != NULL );

    JXStaticText* obj5 =
        new JXStaticText("Company:", obj1,
                    JXWidget::kHElastic, JXWidget::kVElastic, 5,87, 65,20);
    assert( obj5 != NULL );

    mCompany =
        new CTextInputField(obj1,
                    JXWidget::kHElastic, JXWidget::kVElastic, 85,85, 200,20);
    assert( mCompany != NULL );

    JXStaticText* obj6 =
        new JXStaticText("Phone Work:", obj1,
                    JXWidget::kHElastic, JXWidget::kVElastic, 295,12, 85,20);
    assert( obj6 != NULL );

    mPhoneWork =
        new CTextInputField(obj1,
                    JXWidget::kHElastic, JXWidget::kVElastic, 380,10, 150,20);
    assert( mPhoneWork != NULL );

    JXStaticText* obj7 =
        new JXStaticText("Phone Home:", obj1,
                    JXWidget::kHElastic, JXWidget::kVElastic, 295,35, 85,20);
    assert( obj7 != NULL );

    mPhoneHome =
        new CTextInputField(obj1,
                    JXWidget::kHElastic, JXWidget::kVElastic, 380,35, 150,20);
    assert( mPhoneHome != NULL );

    JXStaticText* obj8 =
        new JXStaticText("Fax:", obj1,
                    JXWidget::kHElastic, JXWidget::kVElastic, 295,62, 85,20);
    assert( obj8 != NULL );

    mFax =
        new CTextInputField(obj1,
                    JXWidget::kHElastic, JXWidget::kVElastic, 380,60, 150,20);
    assert( mFax != NULL );

    JXStaticText* obj9 =
        new JXStaticText("Address:", obj1,
                    JXWidget::kHElastic, JXWidget::kVElastic, 5,112, 60,20);
    assert( obj9 != NULL );

    mAddress =
        new CTextInputDisplay(obj1,
                    JXWidget::kHElastic, JXWidget::kVElastic, 85,110, 260,50);
    assert( mAddress != NULL );

    JXStaticText* obj10 =
        new JXStaticText("URLs:", obj1,
                    JXWidget::kHElastic, JXWidget::kVElastic, 5,167, 55,20);
    assert( obj10 != NULL );

    mURL =
        new CTextInputDisplay(obj1,
                    JXWidget::kHElastic, JXWidget::kVElastic, 85,165, 260,50);
    assert( mURL != NULL );

    JXStaticText* obj11 =
        new JXStaticText("Notes:", obj1,
                    JXWidget::kHElastic, JXWidget::kVElastic, 5,222, 55,20);
    assert( obj11 != NULL );

    mNotes =
        new CTextInputDisplay(obj1,
                    JXWidget::kHElastic, JXWidget::kVElastic, 85,220, 260,50);
    assert( mNotes != NULL );

    mCancelBtn =
        new JXTextButton("Cancel", obj1,
                    JXWidget::kHElastic, JXWidget::kVElastic, 365,255, 70,25);
    assert( mCancelBtn != NULL );
    mCancelBtn->SetShortcuts("^[");

    mOKBtn =
        new JXTextButton("OK", obj1,
                    JXWidget::kHElastic, JXWidget::kVElastic, 455,255, 70,25);
    assert( mOKBtn != NULL );
    mOKBtn->SetShortcuts("^M");

// end JXLayout

	window->SetTitle("Address Edit");
	SetButtons(mOKBtn, mCancelBtn);
}

void CEditAddressDialog::SetFields(CAdbkAddress* addr, bool allow_edit)
{
	mNickName->SetText(addr->GetADL());
	mFullName->SetText(addr->GetName());
	mEmail->SetText(addr->GetMailAddress());
	mCompany->SetText(addr->GetCompany());
	mPhoneWork->SetText(addr->GetPhone(CAdbkAddress::eWorkPhoneType));
	mPhoneHome->SetText(addr->GetPhone(CAdbkAddress::eHomePhoneType));
	mFax->SetText(addr->GetPhone(CAdbkAddress::eFaxType));
	mAddress->SetText(addr->GetAddress(CAdbkAddress::eDefaultAddressType));
	mURL->SetText(addr->GetURL());
	mNotes->SetText(addr->GetNotes());
	
	if (!allow_edit)
	{
		mNickName->SetReadOnly(true);
		mFullName->SetReadOnly(true);
		mEmail->SetReadOnly(true);
		mCompany->SetReadOnly(true);
		mPhoneWork->SetReadOnly(true);
		mPhoneHome->SetReadOnly(true);
		mFax->SetReadOnly(true);
		mAddress->SetReadOnly(true);
		mURL->SetReadOnly(true);
		mNotes->SetReadOnly(true);
	}
}

bool CEditAddressDialog::GetFields(CAdbkAddress* addr)
{
	// Nick-name cannot contain '@' and no spaces surrounding it
	cdstring nickname(mNickName->GetText());
	::strreplace(nickname.c_str_mod(), "@", '*');
	nickname.trimspace();

	addr->SetADL(nickname);
	addr->SetName(mFullName->GetText());
	addr->CopyMailAddress(mEmail->GetText());
	addr->SetCompany(mCompany->GetText());
	addr->SetPhone(mPhoneWork->GetText(), CAdbkAddress::eWorkPhoneType);
	addr->SetPhone(mPhoneHome->GetText(), CAdbkAddress::eHomePhoneType);
	addr->SetPhone(mFax->GetText(), CAdbkAddress::eFaxType);
	addr->SetAddress(mAddress->GetText(), CAdbkAddress::eDefaultAddressType);
	addr->SetURL(mURL->GetText());
	addr->SetNotes(mNotes->GetText());
	
	return true;
}

bool CEditAddressDialog::PoseDialog(CAdbkAddress* addr, bool allow_edit)
{
	bool result = false;

	CEditAddressDialog* dlog = new CEditAddressDialog(JXGetApplication());
	dlog->OnCreate();
	dlog->SetFields(addr, allow_edit);

	// Test for OK
	if (dlog->DoModal(false) == kDialogClosed_OK)
	{
		// Get the address
		dlog->GetFields(addr);

		result = true;
		dlog->Close();
	}

	return result;
}
