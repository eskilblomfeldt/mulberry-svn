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


// CCalendarStoreDoc.h : interface of the CCalendarStoreDoc class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __CCalendarStoreDoc__MULBERRY__
#define __CCalendarStoreDoc__MULBERRY__

#include "CSDIDoc.h"

class CCalendarStoreDoc : public CSDIDoc
{
protected: // create from serialization only
	CCalendarStoreDoc() {}
	DECLARE_DYNCREATE(CCalendarStoreDoc)

// Attributes
public:

// Operations
public:

// Implementation
public:
	virtual ~CCalendarStoreDoc() {}

// Generated message map functions
protected:
	//{{AFX_MSG(CCalendarStoreDoc)
	afx_msg void	OnFileClose(void);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif
