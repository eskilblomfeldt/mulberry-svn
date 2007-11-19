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

#ifndef CGUtils_H
#define CGUtils_H

#include <stdint.h>

namespace CGUtils
{

									// QD <--> CG Geometry Converters
	void	QDToHIPoint(
					const Point&	inQDPoint,
					HIPoint&		outHIPoint);
					
	void	HIToQDPoint(
					const HIPoint&	inHIPoint,
					Point&			outQDPoint);
					
	void	QDToHIRect(
					const Rect&		inQDRect,
					HIRect&			outHIRect);
					
	void	HIToQDRect(
					const HIRect&	inHIRect,
					Rect&			outQDRect);

	inline float	GetCGAlpha(const uint32_t& colour)
	{
		return ((colour & 0xFF000000) >> 24) / 255.0;
	}
	inline float	GetCGRed(const uint32_t& colour)
	{
		return ((colour & 0x00FF0000) >> 16) / 255.0;
	}
	inline float	GetCGGreen(const uint32_t& colour)
	{
		return ((colour & 0x0000FF00) >> 8) / 255.0;
	}
	inline float	GetCGBlue(const uint32_t& colour)
	{
		return (colour & 0x000000FF) / 255.0;
	}
	inline void		FlattenColours(float& red, float& green, float& blue)
	{
		red = (red + 1.0) / 2.5;
		green = (green + 1.0) / 2.5;
		blue = (blue + 1.0) / 2.5;
	}
	inline void		UnflattenColours(float& red, float& green, float& blue)
	{
		red = 2.5 * red - 1.0;
		if (red > 1.0)
			red = 1.0;
		else if (red < 0.0)
			red = 0.0;
		green = 2.5 * green - 1.0;
		if (green > 1.0)
			green = 1.0;
		else if (green < 0.0)
			green = 0.0;
		blue = 2.5 * blue - 1.0;
		if (blue > 1.0)
			blue = 1.0;
		else if (blue < 0.0)
			blue = 0.0;
	}

	inline RGBColor	GetQDColor(const uint32_t& colour)
	{
		RGBColor col;
		col.red = (colour & 0x00FF0000) >> 16;
		col.red |= (col.red << 8);
		col.green = (colour & 0x0000FF00) >> 8;
		col.green |= (col.green << 8);
		col.blue = (colour & 0x000000FF);
		col.blue |= (col.blue << 8);
		
		return col;
	}
	inline RGBColor	GetQDColor(float red, float green, float blue)
	{
		RGBColor col;
		col.red = (short) (255.0 * red);
		col.red |= (col.red << 8);
		col.green = (short) (255.0 * green);
		col.green |= (col.green << 8);
		col.blue = (short) (255.0 * blue);
		col.blue |= (col.blue << 8);
		
		return col;
	}

	inline uint32_t GetColor(const RGBColor& color)
	{
		return ((color.red & 0xFF00) << 8) | (color.green & 0xFF00) | ((color.blue & 0xFF00) >> 8);
	}

class	CGContextSaver {
public:
					CGContextSaver( CGContextRef inContext );
			
					~CGContextSaver();
	
	CGContextRef	Get() const
	{
		return mCGContext;
	}
				
	void			Save( CGContextRef inContext );
	
	void			Restore();
			
private:
	CGContextRef	mCGContext;
};

class	CGContextFromQD 
{
	friend class StAllowQuickdraw;

public:
	class StAllowQuickdraw
	{
	public:
		StAllowQuickdraw(CGContextFromQD& context, CGContextSaver& saver) :
			mContext(context), mSaver(saver)
		{
			mRestoreQuartz = (mContext.Get() != NULL);
			if (mRestoreQuartz)
			{
				mSaver.Restore();
				mContext.EndQuartz();
			}
		}
		~StAllowQuickdraw()
		{
			if (mRestoreQuartz)
			{
				mContext.BeginQuartz();
				mSaver.Save(mContext);
			}
		}
	
	private:
		CGContextFromQD&	mContext;
		CGContextSaver&		mSaver;
		bool				mRestoreQuartz;
	};
					CGContextFromQD( CGrafPtr inPort, bool transform_coords = true );
			
					~CGContextFromQD();
	
	operator CGContextRef() const
	{
		return mCGContext;
	}

	CGContextRef	Get() const
	{
		return mCGContext;
	}
			
private:
	CGrafPtr		mPort;
	CGContextRef	mCGContext;
	
	void BeginQuartz(bool transform_coords = true);
	void EndQuartz();
};

}

#endif	// CGUtils_H
