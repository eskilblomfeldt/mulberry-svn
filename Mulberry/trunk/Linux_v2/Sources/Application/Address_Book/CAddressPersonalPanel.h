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


// Header for CAddressPersonalPanel class

#ifndef __CADDRESSPERSONALPANEL__MULBERRY__
#define __CADDRESSPERSONALPANEL__MULBERRY__

#include "CAddressPanelBase.h"

// Classes
class CTextInputField;

class CAddressPersonalPanel : public CAddressPanelBase
{
public:
					CAddressPersonalPanel(JXContainer* enclosure,
							 const HSizingOption hSizing, const VSizingOption vSizing,
							 const JCoordinate x, const JCoordinate y,
							 const JCoordinate w, const JCoordinate h);
	virtual 		~CAddressPersonalPanel();

	virtual void	OnCreate();					// Do odds & ends

	virtual void	Focus();

protected:
// begin JXLayout1

    CTextInputField* mFullName;
    CTextInputField* mNickName;
    CTextInputField* mEmail;
    CTextInputField* mCompany;

// end JXLayout1

	virtual void	SetFields(const CAdbkAddress* addr);	// Set fields in dialog
	virtual bool	GetFields(CAdbkAddress* addr);			// Get fields from dialog
};

#endif
