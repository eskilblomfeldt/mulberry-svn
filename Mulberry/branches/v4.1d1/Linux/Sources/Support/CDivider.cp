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


// CDivider.cp - UI widget that implements a 3D divider

#include "CDivider.h"

#include "StPenState.h"

#include <JXWindowPainter.h>
#include <JXColormap.h>

CDivider::CDivider(JXContainer* enclosure,
			   const HSizingOption hSizing, const VSizingOption vSizing,
			   const JCoordinate x, const JCoordinate y,
			   const JCoordinate w, const JCoordinate h)
	: JXDecorRect(enclosure, hSizing, vSizing, x,y, w,h)
{
	SetBorderWidth(1);
}

void CDivider::DrawBorder(JXWindowPainter& p, const JRect&	frame)
{
	StPenState save(&p);

	JRect actual_rect(frame);
	bool vertical = actual_rect.height() > actual_rect.width();

	// Line #1
	p.SetPenColor(p.GetColormap()->GetInactiveLabelColor());
	p.Line(actual_rect.left,
			actual_rect.top,
			vertical ? actual_rect.left : actual_rect.right - 1,
			vertical ? actual_rect.bottom - 1 : actual_rect.top);

	// Line #2
	p.SetPenColor(p.GetColormap()->GetWhiteColor());
	p.Line(actual_rect.left + (vertical ? 1 : 0),
			actual_rect.top + (vertical ? 0 : 1),
			vertical ? actual_rect.left + 1 : actual_rect.right - 1,
			vertical ? actual_rect.bottom - 1 : actual_rect.top + 1);
}
