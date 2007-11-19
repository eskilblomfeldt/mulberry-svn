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

#include "CGUtils.h"

namespace CGUtils {

#pragma mark -

// ---------------------------------------------------------------------------
//	QDToHIPoint
/**
	Converts a Quickdraw Point to a HIPoint
	
	@param	inQDPoint		Quickdraw Point
	@param	outHIPoint		Corresponding HIPoint							*/

void
QDToHIPoint(
	const Point&	inQDPoint,
	HIPoint&		outHIPoint)
{
	outHIPoint.x = inQDPoint.h;
	outHIPoint.y = inQDPoint.v;
}


// ---------------------------------------------------------------------------
//	HIToQDPoint
/**
	Converts a HIPoint to a Quickdraw Point
	
	@param	inHIPoint		HIPoint
	@param	outQDPoint		Corresponding Quickdraw Point
	
	Truncates the HIPoint coordinates from 32 to 16 bits					*/

void
HIToQDPoint(
	const HIPoint&	inHIPoint,
	Point&			outQDPoint)
{
	outQDPoint.h = inHIPoint.x;
	outQDPoint.v = inHIPoint.y;
}


// ---------------------------------------------------------------------------
//	QDToHIRect
/**
	Converts a Quickdraw Rect to a HIRect
	
	@param	inQDRect		Quickdraw Rect
	@param	outHIRect		Corresponding HIRect							*/

void
QDToHIRect(
	const Rect&		inQDRect,
	HIRect&			outHIRect)
{
	outHIRect.origin.x = inQDRect.left;
	outHIRect.origin.y = inQDRect.top;
	outHIRect.size.width = inQDRect.right - inQDRect.left;
	outHIRect.size.height = inQDRect.bottom - inQDRect.top;
}


// ---------------------------------------------------------------------------
//	HIToQDRect
/**
	Converts a HIRect to a Quickdraw REct
	
	@param	inHIRect		HIRect
	@param	outQDRect		Corresponding Quickdraw Rect
	
	Truncates the HIPoint coordinates from 32 to 16 bits					*/

void
HIToQDRect(
	const HIRect&	inHIRect,
	Rect&			outQDRect)
{
	outQDRect.left = inHIRect.origin.x;
	outQDRect.top = inHIRect.origin.y;
	outQDRect.right = inHIRect.origin.x + inHIRect.size.width;
	outQDRect.bottom = inHIRect.origin.y + inHIRect.size.height;
}

#pragma mark -

#define QDxToCGx(portBounds, qdX)  ( (qdX) )
#define QDyToCGy(portBounds, qdY)  ( ((portBounds).bottom - (portBounds).top ) - (qdY) )

CGContextFromQD::CGContextFromQD(GrafPtr inPort, bool transform_coords)
{
	mPort = inPort;
	
	BeginQuartz(transform_coords);
}

CGContextFromQD::~CGContextFromQD()
{
	EndQuartz();
}

void CGContextFromQD::BeginQuartz(bool transform_coords)
{
	::QDBeginCGContext(mPort, &mCGContext);

	Rect portRect;
	::GetPortBounds(mPort, &portRect);

	// Make sure clip region is sync'd
	StRegion clipRgn;
    clipRgn = ::GetPortClipRegion(mPort, clipRgn);
	::ClipCGContextToRegion(mCGContext, &portRect, clipRgn);

	if (transform_coords)
	{
		//  Move the CG origin to the upper left of the port
		//::SyncCGContextOriginWithPort(mCGContext, mPort);
		::CGContextTranslateCTM(mCGContext, QDxToCGx(portRect, 0), QDyToCGy(portRect, 0));

	    //  Flip the y axis so that positive Y points down
	    //  Note that this will cause text drawn with Core Graphics
	    //  to draw upside down
	    ::CGContextScaleCTM( mCGContext, 1.0, -1.0 );
	}
}

void CGContextFromQD::EndQuartz()
{
	if (mCGContext != NULL)
	{
		::CGContextSynchronize(mCGContext);
		::QDEndCGContext(mPort, &mCGContext);
		mCGContext = NULL;
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	CGContextSaver
/**
	Constructor from a CGContextRef
	
	@param	inContext	CGContext to save									*/

CGContextSaver::CGContextSaver(
	CGContextRef	inContext)
{
	mCGContext = NULL;
	Save(inContext);
}


// ---------------------------------------------------------------------------
//	~CGContextSaver
/**
	Destructor. Restores saved CGContext									*/

CGContextSaver::~CGContextSaver()
{
	Restore();
}


// ---------------------------------------------------------------------------
//	Save
/**
	Saves the input CGContext and restores the one formerly being saved
	
	@param	inContext	CGContext to save									*/

void
CGContextSaver::Save(
	CGContextRef	inContext)
{
	if (mCGContext != NULL) {
		Restore();
	}
	
	::CGContextSaveGState(inContext);
	mCGContext = inContext;
}


// ---------------------------------------------------------------------------
//	Restore
/**
	Restores the CGContext that was being saved								*/

void
CGContextSaver::Restore()
{
	if (mCGContext != NULL) {
		::CGContextRestoreGState(mCGContext);
		mCGContext = NULL;
	}
}


} // namespace CGUtils