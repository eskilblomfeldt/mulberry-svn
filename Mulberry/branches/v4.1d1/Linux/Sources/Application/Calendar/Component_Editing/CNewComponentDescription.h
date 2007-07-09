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

#ifndef H_CNewComponentDescription
#define H_CNewComponentDescription

#include "CNewComponentPanel.h"

class CTextInputField;
class CTextInputDisplay;

// ===========================================================================
//	CNewComponentDescription

class CNewComponentDescription : public CNewComponentPanel
{
public:
	CNewComponentDescription(JXContainer* enclosure,
						const HSizingOption hSizing, const VSizingOption vSizing,
						const JCoordinate x, const JCoordinate y,
						const JCoordinate w, const JCoordinate h)
		: CNewComponentPanel(enclosure, hSizing, vSizing, x, y, w, h) {}
	virtual				~CNewComponentDescription() {}

	virtual void	OnCreate();

	virtual void	SetEvent(const iCal::CICalendarVEvent& vevent);
	virtual void	GetEvent(iCal::CICalendarVEvent& vevent);

	virtual void	SetToDo(const iCal::CICalendarVToDo& vtodo);
	virtual void	GetToDo(iCal::CICalendarVToDo& vtodo);

	virtual void	SetReadOnly(bool read_only);

protected:

	// UI Objects
// begin JXLayout1

    CTextInputField*   mLocation;
    CTextInputDisplay* mDescription;

// end JXLayout1

};

#endif
