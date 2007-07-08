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

#ifndef H_CNewEventDialog
#define H_CNewEventDialog
#pragma once

#include "CModelessDialog.h"
#include "CListener.h"

#include "CICalendarVEvent.h"

class LCheckBox;
class LPopupButton;
class LTabsControl;
class CTextFieldX;

class CCalendarPopup;
class CNewComponentPanel;
typedef std::vector<CNewComponentPanel*> CNewComponentPanelList;

// ===========================================================================
//	CNewEventDialog

class CNewEventDialog : public CModelessDialog, public CListener
{
public:
	enum { class_ID = 'EdEv', pane_ID = 1810 };

	enum EModelessAction
	{
		eNew,
		eEdit,
		eDuplicate
	};

	static void StartNew(const iCal::CICalendarDateTime& dtstart, const iCal::CICalendar* calin = NULL);
	static void StartEdit(const iCal::CICalendarVEvent& vevent);
	static void StartDuplicate(const iCal::CICalendarVEvent& vevent);

						CNewEventDialog(LStream *inStream);
	virtual				~CNewEventDialog();

	virtual void		ListenTo_Message(long msg, void* param);
	virtual void		ListenToMessage(MessageT inMessage, void *ioParam);

			cdstring	GetCurrentSummary() const;

protected:
	enum
	{
		eSummary_ID = 'SUMM',
		eCalendar_ID = 'CALP',
		eStatus_ID = 'STAT',

		eTabs_ID = 'TABS',
		eTabView_ID = 'VIEW',
		
		eChangeOrganised_ID = 'ORGE',
		eOK_ID = 'OKBT'
	};

	// UI Objects
	CTextFieldX*			mSummary;
	CCalendarPopup*			mCalendar;
	LPopupButton*			mStatus;

	LTabsControl*			mTabs;
	LView*					mTabView;
	
	LCheckBox*				mOrganiserEdit;

	EModelessAction			mAction;
	CNewComponentPanelList	mPanels;
	uint32_t				mCurrentPanel;
	iCal::CICalendarVEvent*	mVEvent;
	bool					mReadOnly;

	static void StartModeless(iCal::CICalendarVEvent& vevent, EModelessAction action);

			bool		ContainsEvent(const iCal::CICalendarVEvent& vevent) const;

	virtual void		FinishCreateSelf();

			void		InitPanels();
	
			void	SetAction(EModelessAction action)
			{
				mAction = action;
			}
			void	SetEvent(iCal::CICalendarVEvent& vevent);
			void	GetEvent(iCal::CICalendarVEvent& vevent);
			void	ChangedSummary();
			void	ChangedCalendar();

			void	SetReadOnly(bool read_only);

			void	DoTab(UInt32 value);

	virtual void	OnOK();
	virtual void	OnCancel();
			void	OnOrganiserEdit();

	virtual uint32_t&	TitleCounter()
	{
		return sTitleCounter;
	}

			bool	DoNewOK();
			bool	DoEditOK();
			void	DoCancel();
			
			bool	GetCalendar(iCal::CICalendarRef oldcal, iCal::CICalendarRef& newcal, iCal::CICalendar*& new_cal);

private:
	static uint32_t					sTitleCounter;
	static set<CNewEventDialog*>	sDialogs;
};

#endif
