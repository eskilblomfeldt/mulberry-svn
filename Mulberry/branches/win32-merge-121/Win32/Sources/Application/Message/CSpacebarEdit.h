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


// CSpacebarEdit.h : header file
//

#ifndef __CSPACEBAREDIT__MULBERRY__
#define __CSPACEBAREDIT__MULBERRY__

#include "CCmdEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CSpacebarEdit window

class CMessageWindow;
class CCmdEditView;

class CSpacebarEdit : public CCmdEdit
{
// Construction
public:
	CSpacebarEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpacebarEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSpacebarEdit();

	virtual void SetMessageWindow(CMessageWindow* msgWindow)
					{ mMsgWindow = msgWindow; }

	// Generated message map functions
protected:
	//{{AFX_MSG(CSpacebarEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

private:
	CMessageWindow* mMsgWindow;

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif
