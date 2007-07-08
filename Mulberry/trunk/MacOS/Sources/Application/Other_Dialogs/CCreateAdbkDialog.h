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


// Header for CCreateAdbkDialog class

#ifndef __CCREATEADBKDIALOG__MULBERRY__
#define __CCREATEADBKDIALOG__MULBERRY__

#include <LDialogBox.h>

#include "cdstring.h"

// Constants

// Panes
const	PaneIDT		paneid_CreateAdbkDialog = 9513;
const	PaneIDT		paneid_CrAdbk = 'NAME';
const	PaneIDT		paneid_CrAdbkType = 'TYPE';
const	PaneIDT		paneid_CrAdbkPersonal = 'PERS';
const	PaneIDT		paneid_CrAdbkGlobal = 'GLOB';
const	PaneIDT		paneid_CrAdbkMover = 'MOVE';
const	PaneIDT		paneid_CrAdbkAccountBox = 'ACCB';
const	PaneIDT		paneid_CrAdbkAccount = 'ACCT';
const	PaneIDT		paneid_CrAdbkAccountPopup = 'ACCP';
const	PaneIDT		paneid_CrAdbkOpenOnStartup = 'OPEN';
const	PaneIDT		paneid_CrAdbkUseNicknames = 'NICK';
const	PaneIDT		paneid_CrAdbkUseSearch = 'SRCH';

// Mesages
const	MessageT	msg_CrAdbkAccountPopup = 'ACCP';

// Resources
const	ResIDT		RidL_CCreateAdbkDialogBtns = 9513;

// Type
class	CTextFieldX;
class	LCheckBox;
class	LPopupButton;
class	LRadioButton;
class	CStaticText;

class	CCreateAdbkDialog : public LDialogBox
{
private:
	CTextFieldX*		mAdbkName;
	LPane*				mTypeGroup;
	LRadioButton*		mPersonal;
	LRadioButton*		mGlobal;
	LView*				mMover;
	LView*				mAccountBox;
	CStaticText*		mAccount;
	LPopupButton*		mAccountPopup;
	LCheckBox*			mOpenOnStartup;
	LCheckBox*			mUseNicknames;
	LCheckBox*			mUseSearch;

	bool				mHasLocal;
	bool				mHasRemote;

public:
	enum { class_ID = 'CrAb' };

	struct SCreateAdbk
	{
		cdstring	name;
		bool		personal;
		cdstring	account;
		bool		open_on_startup;
		bool		use_nicknames;
		bool		use_search;
	};

					CCreateAdbkDialog();
					CCreateAdbkDialog(LStream *inStream);
	virtual 		~CCreateAdbkDialog();

	static bool PoseDialog(SCreateAdbk* details);

	virtual void	ListenToMessage(MessageT inMessage, void *ioParam);

protected:
	virtual void	FinishCreateSelf(void);					// Do odds & ends

			void	OnChangeAccount(long index);

	void SetDetails(SCreateAdbk* create);
	void GetDetails(SCreateAdbk* result);
	
private:
			void InitAccountMenu(void);
};

#endif
