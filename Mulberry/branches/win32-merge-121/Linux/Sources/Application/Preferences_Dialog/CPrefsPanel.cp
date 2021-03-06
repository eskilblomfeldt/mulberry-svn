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


// CPrefsPanel.cpp : implementation file
//


#include "CPrefsPanel.h"

/////////////////////////////////////////////////////////////////////////////
// CPrefsPanel property page

CPrefsPanel::CPrefsPanel(JXContainer* enclosure,
												 const HSizingOption hSizing, 
												 const VSizingOption vSizing,
												 const JCoordinate x, const JCoordinate y,
												 const JCoordinate w, const JCoordinate h) 
	:JXWidgetSet(enclosure, hSizing, vSizing, x, y, w, h)
{
	mCopyPrefs = NULL;
}

CPrefsPanel::~CPrefsPanel()
{
}


// Reset params for DDX
void CPrefsPanel::ResetPrefs(CPreferences* prefs)
{
	SetPrefs(prefs);
}

/////////////////////////////////////////////////////////////////////////////
// CPrefsPanel message handlers

void CPrefsPanel::OnCreate()
{
	// set initial control states
	return;
}

void CPrefsPanel::EnableWidget(JXContainer* c, bool enable)
{
	if (enable) {
		c->Activate();
	} else {
		c->Deactivate();
	}
}
