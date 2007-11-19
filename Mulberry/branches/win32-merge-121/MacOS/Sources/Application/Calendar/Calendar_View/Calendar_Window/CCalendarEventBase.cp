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

#include "CCalendarEventBase.h"

#include "CActionManager.h"
#include "CCalendarTableBase.h"
#include "CCalendarViewBase.h"
#include "CCommands.h"
#include "CContextMenu.h"
#include "CErrorDialog.h"
#include "CListener.h"
#include "CMulberryCommon.h"
#include "CNewEventDialog.h"
#include "CResources.h"
#include "CXStringResources.h"

#include "CCalendarStoreManager.h"
#include "CICalendarUtils.h"
#include "CITIPProcessor.h"

#include "MyCFString.h"

// ---------------------------------------------------------------------------
//	CCalendarEventBase														  [public]
/**
	Default constructor */

CCalendarEventBase::CCalendarEventBase(const SPaneInfo	&inPaneInfo) :
	LPane(inPaneInfo)
{
	mTable = NULL;
	mAllDay = true;
	mStartsInCol = true;
	mEndsInCol = true;
	mIsSelected = false;
	mIsCancelled = false;
	mIsNow = false;
	mHoriz = true;
	mColumnSpan = 1;
	mPreviousLink = NULL;
	mNextLink = NULL;
	mColour = 0;
}

// ---------------------------------------------------------------------------
//	~CCalendarEventBase														  [public]
/**
	Destructor */

CCalendarEventBase::~CCalendarEventBase()
{
}

#pragma mark -

void CCalendarEventBase::FinishCreateSelf()
{
	// Do inherited
	LPane::FinishCreateSelf();

	CContextMenuAttachment::AddUniqueContext(this, 1830, dynamic_cast<LCommander*>(GetSuperView()));
}

void CCalendarEventBase::Select(bool select)
{
	if (mIsSelected != select)
	{
		mIsSelected = select;

		// Force immediate redraw
		Draw(NULL);
	}
}

const char cTickMarkU2713_UTF8[] = 
{
	0xE2, 0x9C, 0x93, 0x00
};

const char cCrossMarkU2713_UTF8[] = 
{
	0xE2, 0x9C, 0x93, 0x00
};

void CCalendarEventBase::SetDetails(iCal::CICalendarComponentExpandedShared& event, CCalendarTableBase* table, const char* title, bool all_day, bool start_col, bool end_col, bool horiz)
{
	mVEvent = event;
	mPeriod = iCal::CICalendarPeriod(mVEvent->GetInstanceStart(), mVEvent->GetInstanceEnd());
	mTable = table;
	cdstring name;
	if (event->GetMaster<iCal::CICalendarVEvent>()->GetStatus() == iCal::eStatus_VEvent_Confirmed)
	{
		name = cTickMarkU2713_UTF8;
	}
	else if (event->GetMaster<iCal::CICalendarVEvent>()->GetStatus() == iCal::eStatus_VEvent_Tentative)
	{
		name = "?? ";
	}
	name += title;
	mTitle = MyCFString(name.c_str(), kCFStringEncodingUTF8);
	mAllDay = all_day;
	mStartsInCol = start_col;
	mEndsInCol = end_col;
	mHoriz = horiz;
	mIsCancelled = (event->GetMaster<iCal::CICalendarVEvent>()->GetStatus() == iCal::eStatus_VEvent_Cancelled);
	mHasAlarm = event->GetMaster<iCal::CICalendarVEvent>()->HasEmbeddedComponent(iCal::CICalendarComponent::eVALARM);
	mAttendeeState = iCal::CITIPProcessor::GetAttendeeState(*event->GetMaster<iCal::CICalendarVEvent>());

	// Setup a help tag
	SetupTagText();
	
	// Determine colour
	iCal::CICalendar* cal = iCal::CICalendar::GetICalendar(event->GetMaster<iCal::CICalendarVEvent>()->GetCalendar());
	if (cal)
	{
		mColour = calstore::CCalendarStoreManager::sCalendarStoreManager->GetCalendarColour(cal);
	}
}

// Click
void CCalendarEventBase::ClickSelf(const SMouseDownEvent &inMouseDown)
{
	// Make sure it is selected
	if ((inMouseDown.macEvent.modifiers & (shiftKey | cmdKey)) != 0)
	{
		SClickEvent_Message _msg(this, true);
		Broadcast_Message(eBroadcast_ClickEvent, &_msg);
	}
	else if (!mIsSelected)
	{
		SClickEvent_Message _msg(this, false);
		Broadcast_Message(eBroadcast_ClickEvent, &_msg);
	}
	else
		mTable->SwitchTarget(mTable);

	// Broadcast double-click
	if ((GetClickCount() > 1) && !inMouseDown.delaySelect)
	{
		Broadcast_Message(eBroadcast_EditEvent, this);
	}
}

void CCalendarEventBase::DrawSelf()
{
	CGUtils::CGContextFromQD inContext(GetMacPort());
	CGUtils::CGContextSaver _cg(inContext);

	Rect qdrect;
	CalcLocalFrameRect(qdrect);
	HIRect rect;
	CGUtils::QDToHIRect(qdrect, rect);
	
	_cg.Restore();
	if (mHoriz)
		DrawHorizFrame(inContext, rect);
	else
		DrawVertFrame(inContext, rect);
	_cg.Save(inContext);

	// Draw title
	rect.origin.x += 3.0;
	rect.size.width -= 3.0;
	Rect box;
	CGUtils::HIToQDRect(rect, box);
	if (box.bottom - box.top < 16)
	{
		short height_adjust = (16 - (box.bottom - box.top)) / 2;
		box.top -= height_adjust;
		box.bottom = box.top + 16;
	}
	::CGContextSetGrayFillColor(inContext, mIsSelected ? 1.0 : 0.0, 1.0);
	MyCFString trunc(mTitle, kCFStringEncodingUTF8);
	if (mHoriz)
		::TruncateThemeText(trunc, kThemeSmallSystemFont, kThemeStateActive, rect.size.width, truncEnd, NULL);
	::DrawThemeTextBox(trunc, kThemeSmallSystemFont, kThemeStateActive, !mHoriz, &box, mHoriz && mAllDay ? teJustCenter : teJustLeft, inContext);
	
	// Strike out text if status is cancelled
	if (mIsCancelled)
	{
		CGUtils::QDToHIRect(qdrect, rect);
		rect = ::CGRectInset(rect, 4.0, 4.0);
		::CGContextBeginPath(inContext);
		if (mHoriz)
		{
			::CGContextMoveToPoint(inContext, rect.origin.x, rect.origin.y + rect.size.height / 2);
			::CGContextAddLineToPoint(inContext, rect.origin.x + rect.size.width, rect.origin.y + rect.size.height / 2);
		}
		else
		{
			::CGContextMoveToPoint(inContext, rect.origin.x, rect.origin.y);
			::CGContextAddLineToPoint(inContext, rect.origin.x + rect.size.width, rect.origin.y + rect.size.height);
			::CGContextMoveToPoint(inContext, rect.origin.x + rect.size.width, rect.origin.y);
			::CGContextAddLineToPoint(inContext, rect.origin.x, rect.origin.y + rect.size.height);
		}
		::CGContextSetGrayStrokeColor(inContext, 0.0, 1.0);
		::CGContextStrokePath(inContext);
		::CGContextClosePath(inContext);
	}
}

void CCalendarEventBase::DrawHorizFrame(CGUtils::CGContextFromQD& inContext, HIRect& rect)
{
	CGUtils::CGContextSaver _cg(inContext);

	// Adjust for ends
	if (mStartsInCol)
	{
		rect.origin.x += 3;
		rect.size.width -= 3;
	}
	else
	{
		rect.origin.x += 1;
		rect.size.width -= 1;
	}
	if (mEndsInCol)
		rect.size.width -= 2;
	else
		rect.size.width -= 1;
	
	CGMutablePathRef path = ::CGPathCreateMutable();
		
	// Draw left end (rounded if starts, flat otherwise)
	if (mStartsInCol)
	{
		if (mAllDay)
		{
			::CGPathMoveToPoint(path, NULL, rect.origin.x + rect.size.height / 2.0, rect.origin.y + rect.size.height);
			::CGPathAddArc(path, NULL, rect.origin.x + rect.size.height/ 2.0, rect.origin.y + rect.size.height/ 2.0, rect.size.height/ 2.0, pi / 2.0, 1.5 * pi, 0);
		}
		else
		{
			::CGPathMoveToPoint(path, NULL, rect.origin.x, rect.origin.y + rect.size.height);
			::CGPathAddLineToPoint(path, NULL, rect.origin.x, rect.origin.y);
		}
	}
	else
	{
		::CGPathMoveToPoint(path, NULL, rect.origin.x + rect.size.height / 2.0, rect.origin.y + rect.size.height);
		::CGPathAddLineToPoint(path, NULL, rect.origin.x, rect.origin.y + rect.size.height);
		::CGPathAddLineToPoint(path, NULL, rect.origin.x + rect.size.height / 6.0, rect.origin.y +  rect.size.height * 5.0 / 6.0);
		::CGPathAddLineToPoint(path, NULL, rect.origin.x, rect.origin.y + rect.size.height * 4.0 / 6.0);
		::CGPathAddLineToPoint(path, NULL, rect.origin.x + rect.size.height / 6.0, rect.origin.y + rect.size.height * 3.0 / 6.0);
		::CGPathAddLineToPoint(path, NULL, rect.origin.x, rect.origin.y + rect.size.height * 2.0 / 6.0);
		::CGPathAddLineToPoint(path, NULL, rect.origin.x + rect.size.height / 6.0, rect.origin.y + rect.size.height / 6.0);
		::CGPathAddLineToPoint(path, NULL, rect.origin.x, rect.origin.y);
	}

	// Draw top line
	if (mAllDay)
		::CGPathAddLineToPoint(path, NULL, rect.origin.x + rect.size.width - rect.size.height / 2.0, rect.origin.y);
	else
		::CGPathAddLineToPoint(path, NULL, rect.origin.x + rect.size.width, rect.origin.y);

	// Draw right end (rounded if starts, flat otherwise)
	if (mEndsInCol)
	{
		if (mAllDay)
			::CGPathAddArc(path, NULL, rect.origin.x + rect.size.width - rect.size.height/ 2.0, rect.origin.y + rect.size.height/ 2.0, rect.size.height/ 2.0, 1.5 * pi, pi / 2.0, 0);
		else
			::CGPathAddLineToPoint(path, NULL, rect.origin.x + rect.size.width, rect.origin.y + rect.size.height);
	}
	else
	{
		::CGPathAddLineToPoint(path, NULL, rect.origin.x + rect.size.width - rect.size.height / 6.0, rect.origin.y);
		::CGPathAddLineToPoint(path, NULL, rect.origin.x + rect.size.width, rect.origin.y + rect.size.height / 6.0);
		::CGPathAddLineToPoint(path, NULL, rect.origin.x + rect.size.width - rect.size.height / 6.0, rect.origin.y + rect.size.height * 2.0 / 6.0);
		::CGPathAddLineToPoint(path, NULL, rect.origin.x + rect.size.width, rect.origin.y + rect.size.height * 3.0 / 6.0);
		::CGPathAddLineToPoint(path, NULL, rect.origin.x + rect.size.width - rect.size.height / 6.0, rect.origin.y + rect.size.height * 4.0 / 6.0);
		::CGPathAddLineToPoint(path, NULL, rect.origin.x + rect.size.width, rect.origin.y + rect.size.height * 5.0 / 6.0);
		::CGPathAddLineToPoint(path, NULL, rect.origin.x + rect.size.width - rect.size.height / 6.0, rect.origin.y + rect.size.height);
	}

	// Draw bottom line
	if (mAllDay)
		::CGPathAddLineToPoint(path, NULL, rect.origin.x + rect.size.height / 2.0, rect.origin.y + rect.size.height);
	else
		::CGPathAddLineToPoint(path, NULL, rect.origin.x, rect.origin.y + rect.size.height);
	
	// Close path
	::CGPathCloseSubpath(path);

	// Use unsaturated colour for selected item
	float red = CGUtils::GetCGRed(mColour);
	float green = CGUtils::GetCGGreen(mColour);
	float blue = CGUtils::GetCGBlue(mColour);
	if (mIsSelected)
	{
		CGUtils::UnflattenColours(red, green, blue);
		::CGContextSetRGBFillColor(inContext, red, green, blue, 1.0);
		::CGContextSetRGBStrokeColor(inContext, red * 0.6, green * 0.6, blue * 0.6, 1.0);
	}
	else
	{
		::CGContextSetRGBFillColor(inContext, red, green, blue, 1.0);
		::CGContextSetRGBStrokeColor(inContext, red * 0.6, green * 0.6, blue * 0.6, 1.0);
	}
	
	::CGContextAddPath(inContext, path);
	::CGContextDrawPath(inContext, kCGPathFillStroke);
	
	// Check for now marker
	if (!mAllDay && mIsNow)
	{
		::CGContextSetRGBStrokeColor(inContext, 0.95, 0.0, 0.0, 1.0);
		::CGContextAddPath(inContext, path);
		::CGContextStrokePath(inContext);
	}

	::CGPathRelease(path);

	// Display alarm indicator
	if (mHasAlarm)
	{
		// Temporarily turn of Quartz whilst we draw a QD icon
		{
			CGUtils::CGContextFromQD::StAllowQuickdraw _qd(inContext, _cg);

			Rect plotFrame;
			CGUtils::HIToQDRect(rect, plotFrame);
			plotFrame.right -= 2;
			plotFrame.left = plotFrame.right - 16;
			plotFrame.bottom = plotFrame.top + 16;
			::Ploticns(&plotFrame, atNone, ttNone, ICNx_AlarmedEvent);
		}
		
		// Offset right edge of text
		rect.size.width -= 16;
	}

	// Display attendee indicator
	if (mAttendeeState != iCal::CITIPProcessor::eNone)
	{
		// Temporarily turn of Quartz whilst we draw a QD icon
		{
			CGUtils::CGContextFromQD::StAllowQuickdraw _qd(inContext, _cg);

			Rect plotFrame;
			CGUtils::HIToQDRect(rect, plotFrame);
			plotFrame.right -= 2;
			plotFrame.left = plotFrame.right - 16;
			plotFrame.bottom = plotFrame.top + 16;

			SInt16 theResID;
			switch(mAttendeeState)
			{
			case iCal::CITIPProcessor::eNone:
				theResID = ICNx_AttendedEvent;
				break;
			case iCal::CITIPProcessor::eAllAccepted:
				theResID = ICNx_AttendedEvent;
				break;
			case iCal::CITIPProcessor::eSomeAccepted:
				theResID = ICNx_AttendeeNeedsAction;
				break;
			case iCal::CITIPProcessor::eSomeDeclined:
				theResID = ICNx_DeclinedEvent;
				break;
			case iCal::CITIPProcessor::eIHaveAccepted:
				theResID = ICNx_AttendeeIsMeEvent;
				break;
			case iCal::CITIPProcessor::eIHaveNotAccepted:
				theResID = ICNx_AttendeeNeedsAction;
				break;
			}
			
			::Ploticns(&plotFrame, atNone, ttNone, theResID);
		}
		
		// Offset right edge of text
		rect.size.width -= 16;
	}
}

const float cRoundRadius = 8.0;
const float cJaggedEdgeHeight = 6.0;

void CCalendarEventBase::DrawVertFrame(CGUtils::CGContextFromQD& inContext, HIRect& rect)
{
	CGUtils::CGContextSaver _cg(inContext);

	rect = ::CGRectInset(rect, 1.0, 1.0);

	float h_radius = rect.size.height >= 16 ? cRoundRadius : rect.size.height / 2.0;
	float w_radius = rect.size.width >= 16 ? cRoundRadius : rect.size.width / 2.0;
	float radius = std::min(h_radius, w_radius);

	CGMutablePathRef path = ::CGPathCreateMutable();
	
	if (mStartsInCol)
	{
		// Top-left corner
		::CGPathMoveToPoint(path, NULL, rect.origin.x, rect.origin.y + radius);
		::CGPathAddArc(path, NULL, rect.origin.x + radius, rect.origin.y + radius, radius, pi, 1.5 * pi, 0);
		
		// Top line
		::CGPathAddLineToPoint(path, NULL, rect.origin.x + rect.size.width - radius, rect.origin.y);

		// Top-right corner
		::CGPathAddArc(path, NULL, rect.origin.x + rect.size.width - radius, rect.origin.y + radius, radius, 1.5 * pi, 0.0, 0);
	}
	else
	{
		// Draw jagged top edge
		::CGPathMoveToPoint(path, NULL, rect.origin.x, rect.origin.y + radius);
		uint32_t ctr = 0;
		for(float x_pos = rect.origin.x; x_pos < rect.origin.x + rect.size.width; x_pos += cJaggedEdgeHeight, ctr++)
			::CGPathAddLineToPoint(path, NULL, x_pos, rect.origin.y + ((ctr % 2 == 0) ? 0.0 : cJaggedEdgeHeight));
		
		// Draw last segment
		::CGPathAddLineToPoint(path, NULL, rect.origin.x + rect.size.width, rect.origin.y + ((ctr % 2 == 1) ? 0.0 : cJaggedEdgeHeight));
	}

	// Right line
	::CGPathAddLineToPoint(path, NULL, rect.origin.x + rect.size.width, rect.origin.y + rect.size.height - radius);

	if (mEndsInCol)
	{
		// Bottom-right corner
		::CGPathAddArc(path, NULL, rect.origin.x + rect.size.width - radius, rect.origin.y + rect.size.height - radius, radius, 0.0, pi / 2.0, 0);
		
		// Bottom line
		::CGPathAddLineToPoint(path, NULL, rect.origin.x + radius, rect.origin.y + rect.size.height);

		// Bottom-left corner
		::CGPathAddArc(path, NULL, rect.origin.x + radius, rect.origin.y + rect.size.height - radius, radius, pi / 2.0, pi, 0);
	}
	else
	{
		// Draw jagged bottom edge
		uint32_t ctr = 0;
		for(float x_pos = rect.origin.x + rect.size.width; x_pos > rect.origin.x; x_pos -= cJaggedEdgeHeight, ctr++)
			::CGPathAddLineToPoint(path, NULL, x_pos, rect.origin.y + rect.size.height - ((ctr % 2 == 1) ? 0.0 : cJaggedEdgeHeight));
		
		// Draw last segment
		::CGPathAddLineToPoint(path, NULL, rect.origin.x, rect.origin.y + rect.size.height - ((ctr % 2 == 0) ? 0.0 : cJaggedEdgeHeight));
	}

	// Left line
	::CGPathAddLineToPoint(path, NULL, rect.origin.x, rect.origin.y + radius);

	// Close the path
	::CGPathCloseSubpath(path);

	// Use unsaturated colour for selected item
	float red = CGUtils::GetCGRed(mColour);
	float green = CGUtils::GetCGGreen(mColour);
	float blue = CGUtils::GetCGBlue(mColour);
	if (mIsSelected)
	{
		CGUtils::UnflattenColours(red, green, blue);
		::CGContextSetRGBFillColor(inContext, red, green, blue, 1.0);
		::CGContextSetRGBStrokeColor(inContext, red * 0.6, green * 0.6, blue * 0.6, 1.0);
	}
	else
	{
		::CGContextSetRGBFillColor(inContext, red, green, blue, 1.0);
		::CGContextSetRGBStrokeColor(inContext, red * 0.6, green * 0.6, blue * 0.6, 1.0);
	}
	
	::CGContextAddPath(inContext, path);
	::CGContextDrawPath(inContext, kCGPathFillStroke);

	// Check for now marker
	if (mIsNow)
	{
		::CGContextSetRGBStrokeColor(inContext, 0.95, 0.0, 0.0, 1.0);
		::CGContextAddPath(inContext, path);
		::CGContextStrokePath(inContext);
	}

	::CGPathRelease(path);
	
	// Adjust rect for jaggies/round corners
	if (mStartsInCol)
	{
		rect.origin.y += 2.0;
		rect.size.height -= 2.0;
	}
	else
	{
		rect.origin.y += cJaggedEdgeHeight;
		rect.size.height -= cJaggedEdgeHeight;
	}
	if (mEndsInCol)
	{
		rect.size.height -= 2.0;
	}
	else
	{
		rect.size.height -= cJaggedEdgeHeight;
	}

	// Display alarm indicator
	if (mHasAlarm)
	{
		// Temporarily turn of Quartz whilst we draw a QD icon
		{
			CGUtils::CGContextFromQD::StAllowQuickdraw _qd(inContext, _cg);

			Rect plotFrame;
			CGUtils::HIToQDRect(rect, plotFrame);
			plotFrame.right -= 2;
			plotFrame.left = plotFrame.right - 16;
			if (plotFrame.bottom - plotFrame.top < 16)
			{
				plotFrame.top = (plotFrame.top + plotFrame.bottom) / 2 - 8;
			}
			plotFrame.bottom = plotFrame.top + 16;
			::Ploticns(&plotFrame, atNone, ttNone, ICNx_AlarmedEvent);
		}
		
		// Offset right edge of text
		rect.size.width -= 16;
	}

	// Display attendee indicator
	if (mAttendeeState != iCal::CITIPProcessor::eNone)
	{
		// Determine whether to draw alongside or below any alarm
		int32_t horiz_offset = (mHasAlarm && (rect.size.height >= 32)) ? -16 : 0;
		int32_t vert_offset = (mHasAlarm && (rect.size.height >= 32)) ? 16 : 0;

		// Temporarily turn of Quartz whilst we draw a QD icon
		{
			CGUtils::CGContextFromQD::StAllowQuickdraw _qd(inContext, _cg);

			Rect plotFrame;
			CGUtils::HIToQDRect(rect, plotFrame);
			plotFrame.right -= 2 + horiz_offset;
			plotFrame.left = plotFrame.right - 16;
			if (plotFrame.bottom - plotFrame.top < 16)
			{
				plotFrame.top = (plotFrame.top + plotFrame.bottom) / 2 - 8;
			}
			plotFrame.top += vert_offset;
			plotFrame.bottom = plotFrame.top + 16;
			
			SInt16 theResID;
			switch(mAttendeeState)
			{
			case iCal::CITIPProcessor::eNone:
				theResID = ICNx_AttendedEvent;
				break;
			case iCal::CITIPProcessor::eAllAccepted:
				theResID = ICNx_AttendedEvent;
				break;
			case iCal::CITIPProcessor::eSomeAccepted:
				theResID = ICNx_AttendeeNeedsAction;
				break;
			case iCal::CITIPProcessor::eSomeDeclined:
				theResID = ICNx_DeclinedEvent;
				break;
			case iCal::CITIPProcessor::eIHaveAccepted:
				theResID = ICNx_AttendeeIsMeEvent;
				break;
			case iCal::CITIPProcessor::eIHaveNotAccepted:
				theResID = ICNx_AttendeeNeedsAction;
				break;
			}
			
			::Ploticns(&plotFrame, atNone, ttNone, theResID);
		}
		
		// Offset right edge of text
		if (vert_offset == 0)
			rect.size.width -= 16;
	}
	
}
