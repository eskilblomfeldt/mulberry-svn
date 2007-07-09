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


// Header for CPrefsMailbox class

#ifndef __CPREFSMAILBOX__MULBERRY__
#define __CPREFSMAILBOX__MULBERRY__

#include "CPrefsPanel.h"


// Constants

// Panes
const	PaneIDT		paneid_PrefsMailbox = 5003;
const	PaneIDT		paneid_MailboxTabs = 'TABS';

// Resources
const	ResIDT		RidL_CPrefsMailboxBtns = 5003;

// Mesages
const	MessageT	msg_MailboxTabs = 'TABS';

// Classes
class CPrefsSubTab;

class	CPrefsMailbox : public CPrefsPanel,
						public LListener
{
private:
	CPrefsSubTab*		mTabs;

public:
	enum { class_ID = 'Pmbx' };

					CPrefsMailbox();
					CPrefsMailbox(LStream *inStream);
	virtual 		~CPrefsMailbox();


protected:
	virtual void	FinishCreateSelf(void);					// Do odds & ends

public:
	virtual void	ListenToMessage(MessageT inMessage, void *ioParam);

	virtual void	ToggleICDisplay(void);					// Toggle display of IC - pure virtual
	virtual void	SetPrefs(CPreferences* copyPrefs);		// Set prefs
	virtual void	UpdatePrefs(void);						// Force update of prefs

};

#endif
