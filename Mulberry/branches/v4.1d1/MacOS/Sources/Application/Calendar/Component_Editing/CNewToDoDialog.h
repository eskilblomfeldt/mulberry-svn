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

#ifndef H_CNewToDoDialog
#define H_CNewToDoDialog
#pragma once

#include "CModelessDialog.h"
#include "CListener.h"

#include "CICalendarVToDo.h"

class LBevelButton;
class LCheckBox;
class LTabsControl;
class CTextFieldX;

class CCalendarPopup;
class CDateTimeZoneSelect;
class CNewComponentPanel;
typedef std::vector<CNewComponentPanel*> CNewComponentPanelList;

// ===========================================================================
//	CNewToDoDialog

class CNewToDoDialog : public CModelessDialog, public CListener
{
public:
	enum { class_ID = 'EdTD', pane_ID = 1813 };

	enum EModelessAction
	{
		eNew,
		eEdit,
		eDuplicate
	};

	static void StartNew(const iCal::CICalendar* calin = NULL);
	static void StartEdit(const iCal::CICalendarVToDo& vtodo);
	static void StartDuplicate(const iCal::CICalendarVToDo& vtodo);

						CNewToDoDialog(LStream *inStream);
	virtual				~CNewToDoDialog();

	virtual void		ListenTo_Message(long msg, void* param);
	virtual void		ListenToMessage(MessageT inMessage, void *ioParam);

	iCal::CICalendarRef GetCurrentCalendar() const;
			cdstring	GetCurrentSummary() const;

protected:
	enum
	{
		eSummary_ID = 'SUMM',
		eCalendar_ID = 'CALP',
		eCompleted_ID = 'COMP',
		eCompletedDateTimeZone_ID = 'CDTS',
		eCompletedNow_ID = 'TNOW',

		eTabs_ID = 'TABS',
		eTabView_ID = 'VIEW',
		
		eChangeOrganised_ID = 'ORGE',
		eOK_ID = 'OKBT'
	};

	// UI Objects
	CTextFieldX*			mSummary;
	CCalendarPopup*			mCalendar;
	LCheckBox*				mCompleted;
	CDateTimeZoneSelect*	mCompletedDateTimeZone;
	LBevelButton*			mCompletedNow;

	LTabsControl*			mTabs;
	LView*					mTabView;
	
	LCheckBox*				mOrganiserEdit;

	EModelessAction			mAction;
	CNewComponentPanelList	mPanels;
	uint32_t				mCurrentPanel;
	iCal::CICalendarVToDo*	mVToDo;
	bool					mReadOnly;
	
	iCal::CICalendarDateTime	mActualCompleted;
	bool						mCompletedExists;

	static void StartModeless(iCal::CICalendarVToDo& vtodo, EModelessAction action);

			bool		ContainsToDo(const iCal::CICalendarVToDo& vtodo) const;

	virtual void		FinishCreateSelf();

			void		InitPanels();
	
			void	SetAction(EModelessAction action)
			{
				mAction = action;
			}
			void	SetToDo(iCal::CICalendarVToDo& vtodo);
			void	GetToDo(iCal::CICalendarVToDo& vtodo);
			void	ChangedSummary();
			void	ChangedCalendar();

			void	SetReadOnly(bool read_only);

			void	DoCompleted(bool completed);
			void	DoNow();
			void	DoTab(UInt32 value);
			
			// Complete actions
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
	static uint32_t				sTitleCounter;
	static set<CNewToDoDialog*>	sDialogs;
};

#endif
