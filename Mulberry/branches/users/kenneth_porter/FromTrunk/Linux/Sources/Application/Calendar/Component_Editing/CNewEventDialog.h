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

#include "CModelessDialog.h"
#include "CListener.h"

#include "CICalendarVEvent.h"

#include "HPopupMenu.h"

class CTabController;
class CTextInputField;
class JXTextCheckbox;
class JXTextButton;

class CCalendarPopup;
class CNewComponentPanel;
typedef vector<CNewComponentPanel*> CNewComponentPanelList;

// ===========================================================================
//	CNewEventDialog

class CNewEventDialog : public CModelessDialog, public CListener
{
public:
	enum EModelessAction
	{
		eNew,
		eEdit,
		eDuplicate
	};

	static void StartNew(const iCal::CICalendarDateTime& dtstart, const iCal::CICalendar* calin = NULL);
	static void StartEdit(const iCal::CICalendarVEvent& vevent);
	static void StartDuplicate(const iCal::CICalendarVEvent& vevent);

						CNewEventDialog(JXDirector* supervisor);
	virtual				~CNewEventDialog();

	virtual void		ListenTo_Message(long msg, void* param);

		cdstring		GetCurrentSummary() const;

protected:
	// UI Objects
// begin JXLayout

    CTextInputField* mSummary;
    CCalendarPopup*  mCalendar;
    HPopupMenu*      mStatus;
    CTabController*  mTabs;
    JXTextCheckbox*  mOrganiserEdit;
    JXTextButton*    mCancelBtn;
    JXTextButton*    mOKBtn;

// end JXLayout

	EModelessAction			mAction;
	CNewComponentPanelList	mPanels;
	uint32_t				mCurrentPanel;
	iCal::CICalendarVEvent*	mVEvent;
	bool					mReadOnly;

	static void StartModeless(iCal::CICalendarVEvent& vevent, EModelessAction action);

	virtual void		OnCreate();
	virtual void		Receive(JBroadcaster* sender, const Message& message);

			bool		ContainsEvent(const iCal::CICalendarVEvent& vevent) const;

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

			void	DoTab(JIndex value);

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
