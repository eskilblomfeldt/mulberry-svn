/*
    Copyright (c) 2007-2009 Cyrus Daboo. All rights reserved.
    
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

#ifndef H_CNewComponentDetails
#define H_CNewComponentDetails
#pragma once

#include "CNewComponentPanel.h"
#include "CTabPanel.h"

#include "CCmdEdit.h"
#include "CSubPanelController.h"

class CNewTimingPanel;

// ===========================================================================
//	CNewComponentDetails

class CNewComponentDetails : public CNewComponentPanel
{
public:
						CNewComponentDetails();
	virtual				~CNewComponentDetails();

	virtual void	SetEvent(const iCal::CICalendarVEvent& vevent, const iCal::CICalendarComponentExpanded* expanded);
	virtual void	GetEvent(iCal::CICalendarVEvent& vevent);
	virtual void	GetPeriod(iCal::CICalendarPeriod& period);

	virtual void	SetToDo(const iCal::CICalendarVToDo& vtodo, const iCal::CICalendarComponentExpanded* expanded);
	virtual void	GetToDo(iCal::CICalendarVToDo& vtodo);

	virtual	void	SetReadOnly(bool read_only);
			
	const CNewTimingPanel* GetTimingPanel() const
		{ return mTimingPanel; }

protected:
	// UI Objects
	enum { IDD = IDD_CALENDAR_NEW_DETAILS };
	CSubPanelController		mTimingView;
	CNewTimingPanel*		mTimingPanel;

	CCmdEdit			mLocation;
	CCmdEdit			mDescription;

	// Generated message map functions
	//{{AFX_MSG(CDurationSelect)
	virtual BOOL	OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
