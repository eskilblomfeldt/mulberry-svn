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

#ifndef H_CDayWeekTable
#define H_CDayWeekTable
#pragma once

#include "CCalendarEventTableBase.h"

//#include "CDayWeekView.h"
#include "CDayWeekViewTimeRange.h"

#include "CICalendarUtils.h"
#include "CICalendarVEvent.h"
#include "CICalendarComponentExpanded.h"

#include <vector>

class CDayEvent;
typedef std::vector<CDayEvent*> CDayEventList;
typedef std::vector<pair<CDayEvent*, bool> > CDayEventSlotList;
typedef std::vector<CDayEventSlotList> CDayEventMatrixRow;
typedef std::vector<CDayEventMatrixRow> CDayEventMatrix;
typedef std::vector<CDayEventList> CDayEventColumn;

class CDayWeekTitleTable;

// ===========================================================================
//	CDayWeekTable

class CDayWeekTable : public CCalendarEventTableBase
{
public:
						CDayWeekTable();
	virtual				~CDayWeekTable();
	
	void				SetTitles(CDayWeekTitleTable* titles)
	{
		mTitles = titles;
	}

	void				ResetTable(iCal::CICalendarDateTime& date, int type, CDayWeekViewTimeRange::ERanges range);

	void				ScaleRows(uint32_t scale);

	void				AddEvents(iCal::CICalendarExpandedComponents& vevents);
	void				AddAllDayEvent(iCal::CICalendarComponentExpandedShared& vevent);
	void				AddTimedEvent(iCal::CICalendarComponentExpandedShared& vevent);

	const iCal::CICalendarDateTime&	GetCellStartDate(const STableCell& cell) const;
	const iCal::CICalendarDateTime&	GetCellEndDate(const STableCell& cell) const;
	const iCal::CICalendarDateTime&	GetTimedStartDate(const STableCell& cell) const;
	const iCal::CICalendarDateTime&	GetTimedEndDate(const STableCell& cell) const;
	bool				IsTodayColumn(const STableCell& cell) const
		{ return cell.col == mNowCell.col; }
	bool				IsNowRow(const STableCell& cell) const
		{ return cell.row == mNowCell.row; }

	iCal::CICalendarDateTime	GetClickDateTime(const CPoint& localPt) const;

protected:
	CDayWeekTitleTable*				mTitles;
	iCal::CICalendarDateTime		mDate;
	STableCell		 				mNowCell;
	CDayEventMatrixRow				mAllDayEvents;
	CDayEventColumn					mTimedEvents;
	uint32_t						mScaleRows;
	uint32_t						mStartHour;
	uint32_t						mEndHour;
	uint32_t						mNowMarker;
	uint32_t						mCurrentNowMarker;
	iCal::CICalendarDateTimeList	mCellDates;
	iCal::CICalendarDateTimeList	mTimedStartDates;
	iCal::CICalendarDateTimeList	mTimedEndDates;

	virtual void 		Idle();
			void		ResetToday();

	virtual void	DrawCell(CDC* pDC, const STableCell& inCell, const CRect& inLocalRect);							// Draw the items

	virtual void	SelectionChanged();

	virtual void	SelectEvent(bool next);

private:
			void	CacheCellDates();
			bool	CheckNow();
			void	CheckEventsNow();
			void	RefreshNow();

			void	InitEvents(TableIndexT rows, TableIndexT cols);
			void	AdjustSize();
			void	RepositionEvents();
			void	PositionAllDayEvent(CDayEvent* event, const STableCell& cell, size_t offset);
			void	PositionTimedEvent(CDayEvent* event, const STableCell& cell, int64_t top_secs);
			void	ClearEvents();

			void	RescaleHeight();

			// Overlap resolution
			void	ColumnateEvents();
			void	GetOverlaps(const CDayEventList& list, uint32_t index, std::vector<uint32_t>& overlaps) const;

protected:
	// message handlers
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg	void OnLButtonDblClk(UINT nFlags, CPoint point);			// Double-clicked item
	virtual void HandleContextMenu(CWnd*, CPoint point);

	DECLARE_MESSAGE_MAP()
};

#endif
