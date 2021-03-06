/******************************************************************************
 JXScrollbar.cpp

	Scrollbars take integer values from 0 to a client specified maximum.
	(Sliders provide the general case.)

	JXScrolltab:

		Meta-left-click on thumb:      place tab
		left-click on tab:             jump to our saved position
		Meta-left-click on tab:        remove this tab
		Meta-Shift-left-click on tab:  remove all tabs
		right-click on either:         open menu with available choices

	To do:

		If scrolltabs get too close together, we could place consecutive ones
		on opposite edges of the scrollbar to produce a checkered pattern.
		To avoid any overlap at all, one could shift a colliding subset of
		scrolltabs by x pixels to minimize

			sum(((position[i] - true_position[i]) - x)^2)

		This gives x = sum(position[i] - true_position[i]) / n

	BASE CLASS = JXWidget

	Copyright � 1996-98 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXScrollbar.h>
#include <JXScrolltab.h>
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXTextMenu.h>
#include <JXWindowPainter.h>
#include <JXColormap.h>
#include <jXPainterUtil.h>
#include <jStreamUtil.h>
#include <JMinMax.h>
#include <jTime.h>
#include <jMath.h>
#include <jAssert.h>

const JCoordinate kMinThumbLength       = 12;
//const JSize kScaleThumbShrink           = 5;
const JCoordinate kMinDoubleArrowFactor = 6;

const JFloat kInitialScrollDelay    = 0.2;
const JFloat kContinuousScrollDelay = 0.05;

// setup information

const JFileVersion kCurrentSetupVersion = 1;
const JCharacter kSetupDataEndDelimiter = '\1';		// avoid conflict with JXScrollableWidget

	// version 1: stores itsValue

// JBroadcaster message types

const JCharacter* JXScrollbar::kScrolled = "Scrolled::JXScrollbar";

// Action menu

static const JCharacter* kActionMenuStr =
	"    Scroll here                       %k Middle-click"
	"  | Scroll to top"
	"  | Scroll to bottom"
	"%l| Place scrolltab at thumb location %k Meta-left-click on thumb"
	"  | Remove all scrolltabs             %k Meta-Shift-left-click on any scrolltab";

enum
{
	kScrollHereCmd = 1,
	kScrollTopCmd,
	kScrollBottomCmd,
	kNewScrolltabCmd,
	kRemoveAllTabsCmd
};

/******************************************************************************
 Constructor

 ******************************************************************************/

JXScrollbar::JXScrollbar
	(
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXWidget(enclosure, hSizing, vSizing, x,y, w,h)
{
	itsValue        = 0;
	itsStepSize     = 1;
	itsPageStepSize = 10;
	itsMaxValue     = 0;

	itsContScrollDelay = kContinuousScrollDelay;

	itsScrolltabList              = NULL;
	itsIgnoreScrolltabDeletedFlag = kJFalse;

	itsActionMenu = NULL;

	itsDragAction = kInvalidClick;
	itsDecrPushedFlag     = itsIncrPushedFlag     = kJFalse;
	itsDecrPagePushedFlag = itsIncrPagePushedFlag = kJFalse;

	SetBorderWidth(kJXDefaultBorderWidth);
	SetBackColor((GetColormap())->GetDefaultSliderBackColor());

	itsSpeedScrollCursor = GetSpeedScrollCursor();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXScrollbar::~JXScrollbar()
{
	itsIgnoreScrolltabDeletedFlag = kJTrue;
	DeleteEnclosedObjects();				// tabs talk to us when deleted
	delete itsScrolltabList;
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
JXScrollbar::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	if (IsHorizontal())
		{
		DrawHoriz(p);
		}
	else
		{
		DrawVert(p);
		}

	// draw the thumb

	JXDrawUpFrame(p, itsThumbRect, GetBorderWidth(), kJTrue,
				  (p.GetColormap())->GetDefaultBackColor());
}

// private

void
JXScrollbar::DrawHoriz
	(
	JPainter& p
	)
{
	p.SetFilling(kJTrue);

	const JRect bounds = GetBounds();

	JCoordinate arrowWidth = bounds.height();
	itsDecrArrowRect[0] = JRect(bounds.top, bounds.left,
								bounds.bottom, bounds.left + arrowWidth);
	itsIncrArrowRect[0] = JRect(bounds.top, bounds.right - arrowWidth,
								bounds.bottom, bounds.right);

	if (bounds.width() > kMinDoubleArrowFactor * arrowWidth)
		{
		arrowWidth *= 2;
		itsDecrArrowRect[1] = JRect(bounds.top, bounds.right - arrowWidth,
									bounds.bottom, itsIncrArrowRect[0].left);
		itsIncrArrowRect[1] = JRect(bounds.top, itsDecrArrowRect[0].right,
									bounds.bottom, bounds.left + arrowWidth);
		}
	else
		{
		itsDecrArrowRect[1] = JRect(0,0,0,0);
		itsIncrArrowRect[1] = JRect(0,0,0,0);
		}

	// // available length - thumb length = max value - min value
	// thumb length = (page size / document size) * available length

	const JCoordinate availLength = bounds.width() - 2*arrowWidth;
	JCoordinate thumbLength =
//		availLength - (itsMaxValue - kMinValue + kScaleThumbShrink-1)/kScaleThumbShrink;
		(itsPageStepSize * availLength) / (itsMaxValue + itsPageStepSize - kMinValue);
	if (thumbLength < kMinThumbLength)
		{
		thumbLength = kMinThumbLength;
		}
	const JCoordinate minThumbLoc = bounds.left + arrowWidth;
	const JCoordinate maxThumbLoc = bounds.right - arrowWidth - thumbLength;
	JFloat thumbLocScale          = 0.0;
	if (itsMaxValue > kMinValue)	// avoid divide by zero
		{
		thumbLocScale =
			(maxThumbLoc - minThumbLoc)/(JFloat)(itsMaxValue - kMinValue);
		}
	JCoordinate thumbLoc =
		minThumbLoc + JRound(thumbLocScale * (itsValue - kMinValue));
	if (thumbLoc > maxThumbLoc)
		{
		thumbLoc = maxThumbLoc;
		}
	if (thumbLoc < minThumbLoc)
		{
		thumbLoc = minThumbLoc;
		}
	itsThumbRect = JRect(bounds.top, thumbLoc,
						 bounds.bottom, thumbLoc + thumbLength);
	itsThumbDragRect = JRect(bounds.top, bounds.left + arrowWidth,
							 bounds.bottom, bounds.right - arrowWidth);

	itsDecrementPageRect = JRect(bounds.top, itsThumbDragRect.left,
								 bounds.bottom, itsThumbRect.left);
	itsIncrementPageRect = JRect(bounds.top, itsThumbRect.right,
								 bounds.bottom, itsThumbDragRect.right);

	// draw the arrows

	const JSize borderWidth     = GetBorderWidth();
	const JColorIndex backColor = (p.GetColormap())->GetDefaultBackColor();

	if (itsDecrPushedFlag)
		{
		JXDrawDownArrowLeft(p, itsDecrArrowRect[0], borderWidth, kJTrue, backColor);
		JXDrawDownArrowLeft(p, itsDecrArrowRect[1], borderWidth, kJTrue, backColor);
		}
	else
		{
		JXDrawUpArrowLeft(p, itsDecrArrowRect[0], borderWidth, kJTrue, backColor);
		JXDrawUpArrowLeft(p, itsDecrArrowRect[1], borderWidth, kJTrue, backColor);
		}

	if (itsIncrPushedFlag)
		{
		JXDrawDownArrowRight(p, itsIncrArrowRect[0], borderWidth, kJTrue, backColor);
		JXDrawDownArrowRight(p, itsIncrArrowRect[1], borderWidth, kJTrue, backColor);
		}
	else
		{
		JXDrawUpArrowRight(p, itsIncrArrowRect[0], borderWidth, kJTrue, backColor);
		JXDrawUpArrowRight(p, itsIncrArrowRect[1], borderWidth, kJTrue, backColor);
		}

	// place the scrolltabs

	if (itsScrolltabList != NULL)
		{
		const JSize tabCount = itsScrolltabList->GetElementCount();
		for (JIndex i=1; i<=tabCount; i++)
			{
			(itsScrolltabList->NthElement(i))->
				PlaceHoriz(minThumbLoc, maxThumbLoc, thumbLocScale);
			}
		}
}

void
JXScrollbar::DrawVert
	(
	JPainter& p
	)
{
	p.SetFilling(kJTrue);

	const JRect bounds = GetBounds();

	JCoordinate arrowHeight = bounds.width();
	itsDecrArrowRect[0] = JRect(bounds.top, bounds.left,
								bounds.top + arrowHeight, bounds.right);
	itsIncrArrowRect[0] = JRect(bounds.bottom - arrowHeight, bounds.left,
								bounds.bottom, bounds.right);

	if (bounds.height() > kMinDoubleArrowFactor * arrowHeight)
		{
		arrowHeight *= 2;
		itsDecrArrowRect[1] = JRect(bounds.bottom - arrowHeight, bounds.left,
									itsIncrArrowRect[0].top, bounds.right);
		itsIncrArrowRect[1] = JRect(itsDecrArrowRect[0].bottom, bounds.left,
									bounds.top + arrowHeight, bounds.right);
		}
	else
		{
		itsDecrArrowRect[1] = JRect(0,0,0,0);
		itsIncrArrowRect[1] = JRect(0,0,0,0);
		}

	// // available length - thumb length = max value - min value
	// thumb length = (page size / document size) * available length

	const JCoordinate availLength = bounds.height() - 2*arrowHeight;
	JCoordinate thumbLength =
//		availLength - (itsMaxValue - kMinValue + kScaleThumbShrink-1)/kScaleThumbShrink;
		(itsPageStepSize * availLength) / (itsMaxValue + itsPageStepSize - kMinValue);
	if (thumbLength < kMinThumbLength)
		{
		thumbLength = kMinThumbLength;
		}
	const JCoordinate minThumbLoc = bounds.top + arrowHeight;
	const JCoordinate maxThumbLoc = bounds.bottom - arrowHeight - thumbLength;
	JFloat thumbLocScale          = 0.0;
	if (itsMaxValue > kMinValue)	// avoid divide by zero
		{
		thumbLocScale =
			(maxThumbLoc - minThumbLoc)/(JFloat)(itsMaxValue - kMinValue);
		}
	JCoordinate thumbLoc =
		minThumbLoc + JRound(thumbLocScale * (itsValue - kMinValue));
	if (thumbLoc > maxThumbLoc)
		{
		thumbLoc = maxThumbLoc;
		}
	if (thumbLoc < minThumbLoc)
		{
		thumbLoc = minThumbLoc;
		}
	itsThumbRect = JRect(thumbLoc, bounds.left,
						 thumbLoc + thumbLength, bounds.right);
	itsThumbDragRect = JRect(bounds.top + arrowHeight, bounds.left,
							 bounds.bottom - arrowHeight, bounds.right);

	itsDecrementPageRect = JRect(itsThumbDragRect.top, bounds.left,
								 itsThumbRect.top, bounds.right);
	itsIncrementPageRect = JRect(itsThumbRect.bottom, bounds.left,
								 itsThumbDragRect.bottom, bounds.right);

	// draw the arrows

	const JSize borderWidth     = GetBorderWidth();
	const JColorIndex backColor = (p.GetColormap())->GetDefaultBackColor();

	if (itsDecrPushedFlag)
		{
		JXDrawDownArrowUp(p, itsDecrArrowRect[0], borderWidth, kJTrue, backColor);
		JXDrawDownArrowUp(p, itsDecrArrowRect[1], borderWidth, kJTrue, backColor);
		}
	else
		{
		JXDrawUpArrowUp(p, itsDecrArrowRect[0], borderWidth, kJTrue, backColor);
		JXDrawUpArrowUp(p, itsDecrArrowRect[1], borderWidth, kJTrue, backColor);
		}

	if (itsIncrPushedFlag)
		{
		JXDrawDownArrowDown(p, itsIncrArrowRect[0], borderWidth, kJTrue, backColor);
		JXDrawDownArrowDown(p, itsIncrArrowRect[1], borderWidth, kJTrue, backColor);
		}
	else
		{
		JXDrawUpArrowDown(p, itsIncrArrowRect[0], borderWidth, kJTrue, backColor);
		JXDrawUpArrowDown(p, itsIncrArrowRect[1], borderWidth, kJTrue, backColor);
		}

	// place the scrolltabs

	if (itsScrolltabList != NULL)
		{
		const JSize tabCount = itsScrolltabList->GetElementCount();
		for (JIndex i=1; i<=tabCount; i++)
			{
			(itsScrolltabList->NthElement(i))->
				PlaceVert(minThumbLoc, maxThumbLoc, thumbLocScale);
			}
		}
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
JXScrollbar::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
	JXDrawDownFrame(p, frame, GetBorderWidth());
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
JXScrollbar::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	itsDragAction = kInvalidClick;

	Redraw();	// make sure rects are valid

	if (button == kJXRightButton)
		{
		itsDragThumbOffset = itsThumbRect.topLeft() - itsThumbRect.center();
		itsHereValue       = ClickToValue(pt + itsDragThumbOffset);
		OpenActionMenu(pt, buttonStates, modifiers);
		}
	else if (button == kJXButton4)
		{
		StepArrow(-1, JXKeyModifiers(GetDisplay()));
		}
	else if (button == kJXButton5)
		{
		StepArrow(+1, JXKeyModifiers(GetDisplay()));
		}
	else if (itsDecrArrowRect[0].Contains(pt) || itsDecrArrowRect[1].Contains(pt))
		{
		itsDragAction     = kDecrementValue;
		itsDecrPushedFlag = kJTrue;		// redraw correctly
		StepArrow(-1, modifiers);
		ScrollWait(kInitialScrollDelay);
		itsDecrPushedFlag = kJFalse;		// ignore first HandleMouseDrag()
		}
	else if (itsIncrArrowRect[0].Contains(pt) || itsIncrArrowRect[1].Contains(pt))
		{
		itsDragAction     = kIncrementValue;
		itsIncrPushedFlag = kJTrue;		// redraw correctly
		StepArrow(+1, modifiers);
		ScrollWait(kInitialScrollDelay);
		itsIncrPushedFlag = kJFalse;		// ignore first HandleMouseDrag()
		}
	else if (button == kJXMiddleButton && itsThumbDragRect.Contains(pt))
		{
		itsDragAction      = kDragThumb;
		itsDragThumbOffset = itsThumbRect.topLeft() - itsThumbRect.center();
		SetValue(ClickToValue(pt + itsDragThumbOffset));
		Redraw();
		}
	else if (itsThumbRect.Contains(pt))
		{
		if (modifiers.meta())
			{
			PlaceScrolltab();
			}
		else
			{
			itsDragAction      = kDragThumb;
			itsDragThumbOffset = itsThumbRect.topLeft() - pt;
			}
		}
	else if (itsDecrementPageRect.Contains(pt))
		{
		itsDragAction         = kDecrementPage;
		itsDecrPagePushedFlag = kJFalse;		// ignore first HandleMouseDrag()
		StepPage(-1);
		ScrollWait(kInitialScrollDelay);
		}
	else if (itsIncrementPageRect.Contains(pt))
		{
		itsDragAction         = kIncrementPage;
		itsIncrPagePushedFlag = kJFalse;		// ignore first HandleMouseDrag()
		StepPage(+1);
		ScrollWait(kInitialScrollDelay);
		}
}

/******************************************************************************
 HandleMouseDrag (virtual protected)

 ******************************************************************************/

void
JXScrollbar::HandleMouseDrag
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsDragAction == kDragThumb)
		{
		SetValue(ClickToValue(pt + itsDragThumbOffset));
		Redraw();
		}
	else if (itsDragAction == kDecrementValue)
		{
		const JBoolean newDecrPushedFlag =
			JI2B( itsDecrArrowRect[0].Contains(pt) || itsDecrArrowRect[1].Contains(pt) );
		if (itsDecrPushedFlag != newDecrPushedFlag)
			{
			itsDecrPushedFlag = newDecrPushedFlag;
			Redraw();
			}
		else if (itsDecrPushedFlag)
			{
			StepArrow(-1, modifiers);
			ScrollWait(itsContScrollDelay);
			}
		}
	else if (itsDragAction == kIncrementValue)
		{
		const JBoolean newIncrPushedFlag =
			JI2B( itsIncrArrowRect[0].Contains(pt) || itsIncrArrowRect[1].Contains(pt) );
		if (itsIncrPushedFlag != newIncrPushedFlag)
			{
			itsIncrPushedFlag = newIncrPushedFlag;
			Redraw();
			}
		else if (itsIncrPushedFlag)
			{
			StepArrow(+1, modifiers);
			ScrollWait(itsContScrollDelay);
			}
		}
	else if (itsDragAction == kDecrementPage &&
			 itsDecrementPageRect.Contains(pt))
		{
		if (itsDecrPagePushedFlag)
			{
			StepPage(-1);
			ScrollWait(itsContScrollDelay);
			}
		else
			{
			itsDecrPagePushedFlag = kJTrue;
			}
		}
	else if (itsDragAction == kIncrementPage &&
			 itsIncrementPageRect.Contains(pt))
		{
		if (itsIncrPagePushedFlag)
			{
			StepPage(+1);
			ScrollWait(itsContScrollDelay);
			}
		else
			{
			itsIncrPagePushedFlag = kJTrue;
			}
		}
}

/******************************************************************************
 HandleMouseUp

 ******************************************************************************/

void
JXScrollbar::HandleMouseUp
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	itsDragAction = kInvalidClick;
	itsDecrPushedFlag     = itsIncrPushedFlag     = kJFalse;
	itsDecrPagePushedFlag = itsIncrPagePushedFlag = kJFalse;
	Refresh();
}

/******************************************************************************
 StepArrow (private)

 ******************************************************************************/

void
JXScrollbar::StepArrow
	(
	const JCoordinate		count,
	const JXKeyModifiers&	modifiers
	)
{
	if (modifiers.shift())
		{
		DisplayCursor(itsSpeedScrollCursor);
		AdjustValue(count * itsPageStepSize/2);
		}
	else
		{
		DisplayCursor(kJXDefaultCursor);
		StepLine(count);
		}
}

/******************************************************************************
 AdjustCursor (virtual protected)

 ******************************************************************************/

void
JXScrollbar::AdjustCursor
	(
	const JPoint&			pt,
	const JXKeyModifiers&	modifiers
	)
{
	if (modifiers.shift() &&
		(itsDecrArrowRect[0].Contains(pt) || itsDecrArrowRect[1].Contains(pt) ||
		 itsIncrArrowRect[0].Contains(pt) || itsIncrArrowRect[1].Contains(pt)))
		{
		DisplayCursor(itsSpeedScrollCursor);
		}
	else
		{
		JXWidget::AdjustCursor(pt, modifiers);
		}
}

/******************************************************************************
 ClickToValue (private)

	Convert a click in itsThumbDragRect to a scrollbar value.

 ******************************************************************************/

JCoordinate
JXScrollbar::ClickToValue
	(
	const JPoint& pt
	)
	const
{
	if (GetFrameWidth() > GetFrameHeight())
		{
		return ClickToValueHoriz(pt);
		}
	else
		{
		return ClickToValueVert(pt);
		}
}

// private

JCoordinate
JXScrollbar::ClickToValueHoriz
	(
	const JPoint& pt
	)
	const
{
	const JCoordinate minX = itsThumbDragRect.left;
	const JCoordinate maxX = itsThumbDragRect.right - itsThumbRect.width();
	if (pt.x <= minX)
		{
		return kMinValue;
		}
	if (pt.x >= maxX)
		{
		return itsMaxValue;
		}
	else
		{
		const JFloat pixelToValue =
			(itsMaxValue - kMinValue)/(JFloat)(maxX - minX);
		return (kMinValue + JRound(pixelToValue*(pt.x - minX)));
		}
}

JCoordinate
JXScrollbar::ClickToValueVert
	(
	const JPoint& pt
	)
	const
{
	const JCoordinate minY = itsThumbDragRect.top;
	const JCoordinate maxY = itsThumbDragRect.bottom - itsThumbRect.height();
	if (pt.y <= minY)
		{
		return kMinValue;
		}
	if (pt.y >= maxY)
		{
		return itsMaxValue;
		}
	else
		{
		const JFloat pixelToValue =
			(itsMaxValue - kMinValue)/(JFloat)(maxY - minY);
		return (kMinValue + JRound(pixelToValue*(pt.y - minY)));
		}
}

/******************************************************************************
 SetValue

 ******************************************************************************/

void
JXScrollbar::SetValue
	(
	const JCoordinate value
	)
{
	JCoordinate trueValue = value;
	if (trueValue < kMinValue)
		{
		trueValue = kMinValue;
		}
	if (trueValue > itsMaxValue)
		{
		trueValue = itsMaxValue;
		}

	if (trueValue != itsValue)
		{
		itsValue = trueValue;
		Refresh();
		Broadcast(Scrolled(itsValue));
		}
}

/******************************************************************************
 SetMaxValue

 ******************************************************************************/

void
JXScrollbar::SetMaxValue
	(
	const JCoordinate maxValue
	)
{
	if (maxValue != itsMaxValue && maxValue >= kMinValue)
		{
		// remove scrolltabs that are beyond the new maximum

		if (itsScrolltabList != NULL && maxValue < itsMaxValue)
			{
			const JSize tabCount = itsScrolltabList->GetElementCount();
			for (JIndex i=tabCount; i>=1; i--)
				{
				JXScrolltab* tab = itsScrolltabList->NthElement(i);
				if (tab->GetValue() > maxValue)
					{
					delete tab;
					}
				}
			}

		// set new maximum value

		itsMaxValue = maxValue;
		if (itsValue > itsMaxValue)
			{
			itsValue = itsMaxValue;
			Broadcast(Scrolled(itsValue));
			}
		Refresh();
		}
}

/******************************************************************************
 ScrollWait

 ******************************************************************************/

void
JXScrollbar::ScrollWait
	(
	const JFloat delta
	)
	const
{
	(GetWindow())->Update();
	JWait(delta);
}

/******************************************************************************
 Scroll tab functions

 ******************************************************************************/

void
JXScrollbar::PlaceScrolltab()
{
	JXScrolltab* tab = new JXScrolltab(this, itsValue);
	assert( tab != NULL );
	// tab registers itself
}

void
JXScrollbar::RemoveAllScrolltabs()
{
	if (itsScrolltabList != NULL)
		{
		itsIgnoreScrolltabDeletedFlag = kJTrue;
		itsScrolltabList->DeleteAll();
		itsIgnoreScrolltabDeletedFlag = kJFalse;

		delete itsScrolltabList;
		itsScrolltabList = NULL;
		}
}

void
JXScrollbar::ScrolltabCreated
	(
	JXScrolltab* tab
	)
{
	if (itsScrolltabList == NULL)
		{
		itsScrolltabList = new JPtrArray<JXScrolltab>(JPtrArrayT::kForgetAll);
		assert( itsScrolltabList != NULL );
		itsScrolltabList->SetCompareFunction(CompareScrolltabValues);
		itsScrolltabList->SetSortOrder(JOrderedSetT::kSortAscending);
		}

	itsScrolltabList->InsertSorted(tab);
}

void
JXScrollbar::ScrolltabDeleted
	(
	JXScrolltab* tab
	)
{
	if (itsScrolltabList != NULL && !itsIgnoreScrolltabDeletedFlag)
		{
		itsScrolltabList->Remove(tab);
		if (itsScrolltabList->IsEmpty())
			{
			delete itsScrolltabList;
			itsScrolltabList = NULL;
			}
		}
}

/******************************************************************************
 ScrollToTab

	Allows widgets to provide shortcuts (e.g. Ctrl-#) to jump to particular tabs.

 ******************************************************************************/

void
JXScrollbar::ScrollToTab
	(
	const JIndex tabIndex
	)
{
	if (itsScrolltabList != NULL && itsScrolltabList->IndexValid(tabIndex))
		{
		(itsScrolltabList->NthElement(tabIndex))->ScrollToTab();
		}
}

/******************************************************************************
 PrepareForLowerMaxValue

	JXScrollableWidgets should call this before lowering the maximum value
	of a scrollbar so the scrolltabs will maintain the correct positions
	relative to the widget's contents.

	start and length specify the strip of the widget that is being removed.

 ******************************************************************************/

void
JXScrollbar::PrepareForLowerMaxValue
	(
	const JCoordinate	start,
	const JSize			length
	)
{
	if (itsScrolltabList != NULL && length > 0)
		{
		const JCoordinate delta = - (JCoordinate) length;

		const JSize tabCount = itsScrolltabList->GetElementCount();
		for (JIndex i=tabCount; i>=1; i--)
			{
			JXScrolltab* tab = itsScrolltabList->NthElement(i);
			if (tab->GetValue() >= start + (JCoordinate) length)
				{
				tab->AdjustValue(delta);
				}
			else if (tab->GetValue() >= start)
				{
				delete tab;
				}
			}
		}
}

/******************************************************************************
 PrepareForHigherMaxValue

	JXScrollableWidgets should call this before increasing the maximum value
	of a scrollbar so the scrolltabs will maintain the correct positions
	relative to the widget's contents.

	start and length specify the strip of the widget that is being inserted.

 ******************************************************************************/

void
JXScrollbar::PrepareForHigherMaxValue
	(
	const JCoordinate	start,
	const JSize			length
	)
{
	if (itsScrolltabList != NULL && length > 0)
		{
		const JSize tabCount = itsScrolltabList->GetElementCount();
		for (JIndex i=1; i<=tabCount; i++)
			{
			JXScrolltab* tab = itsScrolltabList->NthElement(i);
			if (tab->GetValue() >= start)
				{
				tab->AdjustValue(length);
				}
			}
		}
}

/******************************************************************************
 PrepareForScaledMaxValue

	JXScrollableWidgets should call this before changing the maximum value
	of a scrollbar due to a change in scale so the scrolltabs will maintain
	the correct positions relative to the widget's contents.

 ******************************************************************************/

void
JXScrollbar::PrepareForScaledMaxValue
	(
	const JFloat scaleFactor
	)
{
	if (itsScrolltabList != NULL && scaleFactor != 1.0)
		{
		const JSize tabCount = itsScrolltabList->GetElementCount();
		for (JIndex i=1; i<=tabCount; i++)
			{
			JXScrolltab* tab = itsScrolltabList->NthElement(i);
			tab->ScaleValue(scaleFactor);
			}
		}
}

/******************************************************************************
 StripMoved

	JXScrollableWidgets should call this when a strip of the widget is
	moved to a different location.

 ******************************************************************************/

void
JXScrollbar::StripMoved
	(
	const JCoordinate	origStart,
	const JSize			length,
	const JCoordinate	newStart
	)
{
JIndex i;

	if (itsScrolltabList != NULL && origStart != newStart)
		{
		JPtrArray<JXScrolltab> savedTabList(JPtrArrayT::kForgetAll);

		const JCoordinate origEnd = origStart + length-1;

		const JSize tabCount = itsScrolltabList->GetElementCount();
		for (i=tabCount; i>=1; i--)
			{
			JXScrolltab* tab    = itsScrolltabList->NthElement(i);
			const JCoordinate v = tab->GetValue();
			if (origStart <= v && v <= origEnd)
				{
				savedTabList.Append(tab);
				itsScrolltabList->RemoveElement(i);
				}
			}

		PrepareForLowerMaxValue(origStart, length);
		PrepareForHigherMaxValue(newStart, length);	// consider moving strip to end

		const JCoordinate delta = newStart - origStart;
		const JSize saveCount   = savedTabList.GetElementCount();
		for (i=1; i<=saveCount; i++)
			{
			JXScrolltab* tab = savedTabList.NthElement(i);
			tab->AdjustValue(delta);
			itsScrolltabList->InsertSorted(tab);
			}
		}
}

/******************************************************************************
 StripsSwapped

	JXScrollableWidgets should call this when two strips of the widget are
	swapped.

 ******************************************************************************/

void
JXScrollbar::StripsSwapped
	(
	const JCoordinate	start1,
	const JSize			length1,
	const JCoordinate	start2,
	const JSize			length2
	)
{
	JCoordinate s1, l1, s2, l2;
	if (start1 < start2)
		{
		s1 = start1;
		l1 = length1;
		s2 = start2;
		l2 = length2;
		}
	else
		{
		s1 = start2;
		l1 = length2;
		s2 = start1;
		l2 = length1;
		}

	StripMoved(s2, l2, s1);
	StripMoved(s1 + l2, l1, s2 + l2);
}

/******************************************************************************
 CompareScrolltabValues (static private)

 ******************************************************************************/

JOrderedSetT::CompareResult
JXScrollbar::CompareScrolltabValues
	(
	JXScrolltab* const & t1,
	JXScrolltab* const & t2
	)
{
	const JCoordinate v1 = t1->GetValue();
	const JCoordinate v2 = t2->GetValue();

	if (v1 < v2)
		{
		return JOrderedSetT::kFirstLessSecond;
		}
	else if (v1 == v2)
		{
		return JOrderedSetT::kFirstEqualSecond;
		}
	else
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}
}

/******************************************************************************
 ReadSetup

 ******************************************************************************/

void
JXScrollbar::ReadSetup
	(
	istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers <= kCurrentSetupVersion)
		{
		if (vers >= 1)
			{
			JCoordinate value;
			input >> value;
			SetValue(value);
			}

		RemoveAllScrolltabs();

		JSize tabCount;
		input >> tabCount;

		for (JIndex i=1; i<=tabCount; i++)
			{
			JCoordinate value;
			input >> value;

			JXScrolltab* tab = new JXScrolltab(this, value);
			assert( tab != NULL );
			// tab registers itself
			}
		}

	JIgnoreUntil(input, kSetupDataEndDelimiter);
}

/******************************************************************************
 SkipSetup (static)

 ******************************************************************************/

void
JXScrollbar::SkipSetup
	(
	istream& input
	)
{
	JIgnoreUntil(input, kSetupDataEndDelimiter);
}

/******************************************************************************
 WriteSetup

 ******************************************************************************/

void
JXScrollbar::WriteSetup
	(
	ostream& output
	)
	const
{
	output << ' ' << kCurrentSetupVersion;
	output << ' ' << itsValue;

	if (itsScrolltabList != NULL)
		{
		const JSize tabCount = itsScrolltabList->GetElementCount();
		output << ' ' << tabCount;

		for (JIndex i=1; i<=tabCount; i++)
			{
			output << ' ' << (itsScrolltabList->NthElement(i))->GetValue();
			}
		}
	else
		{
		output << " 0";
		}

	output << kSetupDataEndDelimiter;
}

/******************************************************************************
 OpenActionMenu (private)

 ******************************************************************************/

void
JXScrollbar::OpenActionMenu
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsActionMenu == NULL)
		{
		itsActionMenu = new JXTextMenu("", this, kFixedLeft, kFixedTop, 0,0, 10,10);
		assert( itsActionMenu != NULL );
		itsActionMenu->Hide();
		itsActionMenu->SetToHiddenPopupMenu(kJTrue);
		itsActionMenu->SetMenuItems(kActionMenuStr);
		itsActionMenu->SetUpdateAction(JXMenu::kDisableNone);
		ListenTo(itsActionMenu);
		}

	itsActionMenu->PopUp(this, pt, buttonStates, modifiers);
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
JXScrollbar::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsActionMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		HandleActionMenu(selection->GetIndex());
		}

	else
		{
		JXWidget::Receive(sender, message);
		}
}

/******************************************************************************
 HandleActionMenu (private)

 ******************************************************************************/

void
JXScrollbar::HandleActionMenu
	(
	const JIndex index
	)
{
	if (index == kScrollHereCmd)
		{
		SetValue(itsHereValue);
		}
	else if (index == kScrollTopCmd)
		{
		ScrollToMin();
		}
	else if (index == kScrollBottomCmd)
		{
		ScrollToMax();
		}

	else if (index == kNewScrolltabCmd)
		{
		PlaceScrolltab();
		}
	else if (index == kRemoveAllTabsCmd)
		{
		RemoveAllScrolltabs();
		}
}

/******************************************************************************
 Speed scroll cursor (private)

 ******************************************************************************/

#ifdef _MSC_VER
#pragma warning ( disable : 4305 4309 )
#endif

static const JCharacter* kSpeedScrollCursorName = "JXScrollbar::SpeedScroll";

static char speed_scroll_cursor_bits[] = {
   0x00, 0x00, 0x08, 0x00, 0x18, 0x00, 0x38, 0x00, 0x78, 0x00, 0xf8, 0x00,
   0xf8, 0x01, 0xf8, 0x03, 0xf8, 0x07, 0xf8, 0x00, 0xd8, 0x04, 0x88, 0x09,
   0x80, 0x09, 0x10, 0x13, 0x20, 0x13, 0x20, 0x20, 0x40, 0x29, 0x40, 0x4a,
   0x80, 0x52, 0x80, 0x14};

static char speed_scroll_cursor_mask_bits[] = {
   0x0c, 0x00, 0x1c, 0x00, 0x3c, 0x00, 0x7c, 0x00, 0xfc, 0x00, 0xfc, 0x01,
   0xfc, 0x03, 0xfc, 0x07, 0xfc, 0x0f, 0xfc, 0x0f, 0xfc, 0x1f, 0xfc, 0x1f,
   0xfc, 0x3f, 0xf8, 0x3f, 0xf8, 0x7f, 0xf0, 0x7f, 0xf0, 0xff, 0xe0, 0xff,
   0xe0, 0xff, 0xc0, 0xff};

static const JXCursor kSpeedScrollCursor =
{
	16,20, 3,1, speed_scroll_cursor_bits, speed_scroll_cursor_mask_bits
};

JCursorIndex
JXScrollbar::GetSpeedScrollCursor()
{
	return (GetDisplay())->CreateCustomCursor(kSpeedScrollCursorName,
											  kSpeedScrollCursor);
}
