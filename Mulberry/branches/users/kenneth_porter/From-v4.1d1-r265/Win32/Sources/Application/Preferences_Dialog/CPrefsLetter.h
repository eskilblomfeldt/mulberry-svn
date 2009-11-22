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


// CPrefsLetter.h : header file
//

#ifndef __CPREFSLETTER__MULBERRY__
#define __CPREFSLETTER__MULBERRY__

#include "CPrefsPanel.h"
#include "CTabController.h"

/////////////////////////////////////////////////////////////////////////////
// CPrefsLetter dialog

class CPreferences;

class CPrefsLetter : public CPrefsPanel
{
	DECLARE_DYNCREATE(CPrefsLetter)

// Construction
public:
	CPrefsLetter();
	~CPrefsLetter();

// Dialog Data
	//{{AFX_DATA(CPrefsLetter)
	enum { IDD = IDD_PREFS_LETTER };
	CTabController	mTabs;
	//}}AFX_DATA

	void SetPrefs(CPreferences* prefs);					// Set up params for DDX
	void UpdatePrefs(CPreferences* prefs);				// Get params from DDX

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrefsLetter)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPrefsLetter)
	virtual void InitControls(void);
	virtual void SetControls(void);
	afx_msg void OnSelChangeLetterTabs(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif
