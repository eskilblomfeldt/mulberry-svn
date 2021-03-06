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


// Header for CPropMailboxServer class

#ifndef __CPROPMAILBOXSERVER__MULBERRY__
#define __CPROPMAILBOXSERVER__MULBERRY__

#include "CMailboxPropPanel.h"

// Classes
class CStaticText;
class CTextDisplay;
class JXImageWidget;
class JXTextButton;

class CPropMailboxServer : public CMailboxPropPanel
{
public:
		CPropMailboxServer(JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h)
			: CMailboxPropPanel(enclosure, hSizing, vSizing, x, y, w, h) {}

	virtual void	OnCreate();								// Do odds & ends
	virtual void	SetMboxList(CMboxList* mbox_list);		// Set mbox list
	virtual void	SetProtocol(CMboxProtocol* protocol);	// Set protocol
	virtual void	ApplyChanges(void);						// Force update of mboxes

protected:
// begin JXLayout1

    JXImageWidget* mIconState;
    CStaticText*   mName;
    CStaticText*   mType;
    CTextDisplay*  mGreeting;
    CTextDisplay*  mCapability;
    CStaticText*   mConnection;
    JXTextButton*  mViewCert;

// end JXLayout1
	CMboxProtocol*	mProto;

	virtual void Receive(JBroadcaster* sender, const Message& message);
	
			void	OnViewCert();
};

#endif
