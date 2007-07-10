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

#ifndef H_CNewComponentTiming
#define H_CNewComponentTiming
#pragma once

#include "CNewComponentPanel.h"
#include <LListener.h>

#include "CICalendarRecurrence.h"

class CDateTimeZoneSelect;
class CNewEventTiming;
class CNewTimingPanel;
class CNumberEdit;

class LCheckBox;
class LLittleArrows;
class LPopupButton;
class LPushButton;
class LRadioButton;
class CStaticText;
class LTabsControl;

// ===========================================================================
//	CNewComponentTiming

class CNewComponentTiming : public CNewComponentPanel,
							public LListener
{
public:
	enum { class_ID = 'Ncti', pane_ID = 1814 };

						CNewComponentTiming(LStream *inStream);
	virtual				~CNewComponentTiming();

	virtual void	ListenToMessage(MessageT inMessage, void *ioParam);		// Respond to clicks in the icon buttons

	virtual void	SetEvent(const iCal::CICalendarVEvent& vevent);
	virtual void	GetEvent(iCal::CICalendarVEvent& vevent);
	virtual void	GetPeriod(iCal::CICalendarPeriod& period);

	virtual void	SetToDo(const iCal::CICalendarVToDo& vtodo);
	virtual void	GetToDo(iCal::CICalendarVToDo& vtodo);

	virtual	void	SetReadOnly(bool read_only);
			
protected:
	enum
	{
		eTimingView_ID = 'TVIE',

		eRepeats_ID = 'REAP',
		eRepeatsTabs_ID = 'RTAB',

		eOccursSimpleItems_ID = 'OCCS',

		eOccursInterval_ID = 'INUM',
		eOccursIntervalSpin_ID = 'INUm',
		eOccursFreq_ID = 'OCCU',
		eOccursGroup_ID = 'OGRP',
		eOccursForEver_ID = 'OEVR',
		eOccursCount_ID = 'OFOR',
		eOccursUntil_ID = 'OUNT',
		eOccursCounter_ID = 'ONUM',
		eOccursCounterSpin_ID = 'ONUm',
		eOccursDateTimeZone_ID = 'OCUS',
		
		eOccursAdvancedItems_ID = 'OCCA',
		
		eOccursDescription_ID = 'OCCD',
		eOccursEdit_ID = 'OCCE'

	};

	enum
	{
		eOccurs_Simple = 1,
		eOccurs_Advanced,
		eOccurs_Complex
	};

	enum
	{
		eOccurs_Yearly = 1,
		eOccurs_Monthly,
		eOccurs_Weekly,
		eOccurs_Daily,
		eOccurs_Hourly,
		eOccurs_Minutely,
		eOccurs_Secondly
	};

	enum
	{
		eOccurs_ForEver = 1,
		eOccurs_Count,
		eOccurs_Until
	};

	// UI Objects
	LView*					mTimingView;
	CNewTimingPanel*		mTimingPanel;

	LCheckBox*				mRepeats;
	LTabsControl*			mRepeatsTabs;

	LView*					mOccursSimpleItems;

	CNumberEdit*			mOccursInterval;
	LLittleArrows*			mOccursIntervalSpin;
	LPopupButton*			mOccursFreq;

	LRadioButton*			mOccursForEver;
	LRadioButton*			mOccursCount;
	LRadioButton*			mOccursUntil;

	CNumberEdit*			mOccursCounter;
	LLittleArrows*			mOccursCounterSpin;

	CDateTimeZoneSelect*	mOccursDateTimeZone;

	LView*					mOccursAdvancedItems;

	CStaticText*			mOccursDescription;
	LPushButton*			mOccursEdit;

	iCal::CICalendarRecurrence	mAdvancedRecur;
	cdstring				mComplexDescription;

	virtual void		FinishCreateSelf();
	
			void	DoRepeat(bool repeat);
			void	DoRepeatTab(UInt32 value);
			void	DoOccursGroup(UInt32 value);
			void	DoOccursEdit();
			
			void	SetRecurrence(const iCal::CICalendarRecurrenceSet* recurs);
			
			bool	GetRecurrence(iCal::CICalendarRecurrenceSet& recurs);
			
			void	SyncEnd();
			void	SyncDuration();
};

#endif
