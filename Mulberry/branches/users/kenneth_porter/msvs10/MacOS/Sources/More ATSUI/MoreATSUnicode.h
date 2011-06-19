/*	File:		MoreATSUnicode.h	Description:	Author:		AD	Copyright:	Copyright � 1998-2000 by Apple Computer, Inc., All Rights Reserved.	Disclaimer:	IMPORTANT:  This Apple software is supplied to you by Apple Computer, Inc.				("Apple") in consideration of your agreement to the following terms, and your				use, installation, modification or redistribution of this Apple software				constitutes acceptance of these terms.  If you do not agree with these terms,				please do not use, install, modify or redistribute this Apple software.				In consideration of your agreement to abide by the following terms, and subject				to these terms, Apple grants you a personal, non-exclusive license, under Apple�s				copyrights in this original Apple software (the "Apple Software"), to use,				reproduce, modify and redistribute the Apple Software, with or without				modifications, in source and/or binary forms; provided that if you redistribute				the Apple Software in its entirety and without modifications, you must retain				this notice and the following text and disclaimers in all such redistributions of				the Apple Software.  Neither the name, trademarks, service marks or logos of				Apple Computer, Inc. may be used to endorse or promote products derived from the				Apple Software without specific prior written permission from Apple.  Except as				expressly stated in this notice, no other rights or licenses, express or implied,				are granted by Apple herein, including but not limited to any patent rights that				may be infringed by your derivative works or by other works in which the Apple				Software may be incorporated.				The Apple Software is provided by Apple on an "AS IS" basis.  APPLE MAKES NO				WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED				WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR				PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND OPERATION ALONE OR IN				COMBINATION WITH YOUR PRODUCTS.				IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL OR				CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE				GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)				ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION, MODIFICATION AND/OR DISTRIBUTION				OF THE APPLE SOFTWARE, HOWEVER CAUSED AND WHETHER UNDER THEORY OF CONTRACT, TORT				(INCLUDING NEGLIGENCE), STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN				ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.	Change History (most recent first):				10/14/1999	AD				changed macro names to avoid confusion with ATSUI 1.1											added font name finder											added ATSUI 1.2 accessors											added bulk accessors for QD style bits											conditionalize for different versions of the ATSUI headers											add font mapper that also returns the intrinsic style				 7/27/1999	KG				Updated for Metrowerks Codewarror Pro 2.1								 7/01/1998	AD				Created				*/#define ATSU_1_0	0x100#define ATSU_1_1	0x110#define ATSU_1_2	0x120// Set this to one of the other values if you're building with// pre-ATSUI 1.2 headers & libraries#define ATSU_TARG_VERSION ATSU_1_2#ifdef __MWERKS__// includes for MetroWerks CodeWarrior#include <MacTypes.h>#include <ATSUnicode.h>#include <Menus.h>#include <Quickdraw.h>#if (ATSU_TARG_VERSION >= ATSU_1_2)#include <UnicodeUtilities.h>#endif#else#ifdef __APPLE_CC__// includes for ProjectBuilder#include <Carbon/Carbon.h>#else// includes for MPW#include <Carbon.h>#include <CoreServices.h>#endif#endif#include <limits.h>/*** Getters and Setters ***//* We define individual functions and macros for getting and setting most known line controls and style attributes. They're   more readable than the method provided by the API, but marginally less efficient, especially if you're setting several   of them at once. The other feature of these accessors is that they filter out kATSUNotSetErr. Most of the time, you don't   care if you're getting a default value or one that's been explicitly set. When you do care, use the basic API instead.*/ /* Utility functions for style accessors */extern OSStatus atsuSetBooleanAttribute( ATSUStyle, Boolean, ATSUAttributeTag );extern OSStatus atsuSetFixedAttribute( ATSUStyle, Fixed, ATSUAttributeTag );extern OSStatus atsuSetFractAttribute( ATSUStyle, Fract, ATSUAttributeTag );extern OSStatus atsuSetIntAttribute( ATSUStyle, SInt32, ATSUAttributeTag );extern OSStatus atsuSetShortAttribute( ATSUStyle, SInt16, ATSUAttributeTag );extern OSStatus atsuSetPtrAttribute( ATSUStyle, void *, ATSUAttributeTag );extern OSStatus atsuGetOneAttribute( ATSUStyle 				iStyle,									 ATSUAttributeTag 		iTag,									 ByteCount 				iExpectedValueSize,									 ATSUAttributeValuePtr 	oValue );/* Style accessors */extern OSStatus atsuSetQDStyleBits( ATSUStyle, StyleParameter );extern OSStatus atsuGetQDStyleBits( ATSUStyle iStyle, Style *oQDStyleBits );#define atsuSetQDBold( iStyle, iSetting )		atsuSetBooleanAttribute( iStyle, iSetting, kATSUQDBoldfaceTag )#define atsuGetQDBold( iStyle, oSettingPtr )	atsuGetOneAttribute( iStyle, kATSUQDBoldfaceTag, sizeof(Boolean), oSettingPtr )#define atsuSetQDItalic( iStyle, iSetting )		atsuSetBooleanAttribute( iStyle, iSetting, kATSUQDItalicTag )#define atsuGetQDItalic( iStyle, oSettingPtr )	atsuGetOneAttribute( iStyle, kATSUQDItalicTag, sizeof(Boolean), oSettingPtr )#define atsuSetQDUnderline( iStyle, iSetting )		atsuSetBooleanAttribute( iStyle, iSetting, kATSUQDUnderlineTag )#define atsuGetQDUnderline( iStyle, oSettingPtr )	atsuGetOneAttribute( iStyle, kATSUQDUnderlineTag, sizeof(Boolean), oSettingPtr )#define atsuSetQDCondensed( iStyle, iSetting )		atsuSetBooleanAttribute( iStyle, iSetting, kATSUQDCondensedTag )#define atsuGetQDCondensed( iStyle, oSettingPtr )	atsuGetOneAttribute( iStyle, kATSUQDCondensedTag, sizeof(Boolean), oSettingPtr )#define atsuSetQDExtended( iStyle, iSetting )		atsuSetBooleanAttribute( iStyle, iSetting, kATSUQDExtendedTag )#define atsuGetQDExtended( iStyle, oSettingPtr )	atsuGetOneAttribute( iStyle, kATSUQDExtendedTag, sizeof(Boolean), oSettingPtr )#define atsuSetFont( iStyle, iFontID )		atsuSetIntAttribute( iStyle, iFontID, kATSUFontTag )#define atsuGetFont( iStyle, oFontIDPtr )	atsuGetOneAttribute( iStyle, kATSUFontTag, sizeof(ATSUFontID), oFontIDPtr );#define atsuSetSize( iStyle, iSize )		atsuSetFixedAttribute( iStyle, iSize, kATSUSizeTag )#define atsuGetSize( iStyle, oSizePtr )		atsuGetOneAttribute( iStyle, kATSUSizeTag, sizeof(Fixed), oSizePtr );extern OSStatus atsuSetColor( ATSUStyle, RGBColor );#define atsuGetColor( iStyle, oColorPtr )	atsuGetOneAttribute( iStyle, kATSUColorTag, sizeof(RGBColor), oColorPtr );#define atsuSetLanguage( iStyle, iLanguage )	atsuSetShortAttribute( iStyle, iLanguage, kATSULanguageTag )#define atsuGetLanguage( iStyle, oLanguagePtr )	atsuGetOneAttribute( iStyle, kATSULanguageTag, sizeof(RegionCode), oLanguagePtr );#define atsuSetVerticalCharacter( iStyle, iVerticalChar )	atsuSetShortAttribute( iStyle, iVerticalChar, kATSUVerticalCharacterTag )#define atsuGetVerticalCharacter( iStyle, oVerticalCharPtr )	atsuGetOneAttribute( iStyle, kATSUVerticalCharacterTag, sizeof(ATSUVerticalCharacterType), oVerticalCharPtr );#define atsuSetImposedWidth( iStyle, iWidth )		atsuSetFixedAttribute( iStyle, iWidth, kATSUImposeWidthTag )#define atsuGetImposedWidth( iStyle, oWidthPtr )	atsuGetOneAttribute( iStyle, kATSUImposeWidthTag, sizeof(ATSUTextMeasurement), oWidthPtr );#define atsuSetBeforeWidthStreamShift( iStyle, iValue )		atsuSetFixedAttribute( iStyle, iValue, kATSUBeforeWithStreamShiftTag )#define atsuGetBeforeWidthStreamShift( iStyle, iValuePtr )	atsuGetOneAttribute( iStyle, kATSUBeforeWithStreamShiftTag, sizeof(Fixed), iValuePtr );#define atsuSetAfterWidthStreamShift( iStyle, iValue )		atsuSetFixedAttribute( iStyle, iValue, kATSUAfterWithStreamShiftTag )#define atsuGetAfterWidthStreamShift( iStyle, iValuePtr )	atsuGetOneAttribute( iStyle, kATSUAfterWithStreamShiftTag, sizeof(Fixed), iValuePtr );#define atsuSetCrossStreamShift( iStyle, iValue )		atsuSetFixedAttribute( iStyle, iValue, kATSUCrossStreamShiftTag )#define atsuGetCrossStreamShift( iStyle, iValuePtr )	atsuGetOneAttribute( iStyle, kATSUCrossStreamShiftTag, sizeof(Fixed), iValuePtr );#define atsuSetTracking( iStyle, iValue )		atsuSetFixedAttribute( iStyle, iValue, kATSUTrackingTag )#define atsuGetTracking( iStyle, iValuePtr )	atsuGetOneAttribute( iStyle, kATSUTrackingTag, sizeof(Fixed), iValuePtr );#define atsuSetHangingInhibitFactor( iStyle, iValue )		atsuSetFractAttribute( iStyle, iValue, kATSUHangingInhibitFactorTag )#define atsuGetHangingInhibitFactor( iStyle, iValuePtr )	atsuGetOneAttribute( iStyle, kATSUHangingInhibitFactorTag, sizeof(Fract), iValuePtr );#define atsuSetKerningInhibitFactor( iStyle, iValue )		atsuSetFractAttribute( iStyle, iValue, kATSUKerningInhibitFactorTag )#define atsuGetKerningInhibitFactor( iStyle, iValuePtr )	atsuGetOneAttribute( iStyle, kATSUKerningInhibitFactorTag, sizeof(Fract), iValuePtr );#define atsuSetDecompositionFactor( iStyle, iValue )		atsuSetFixedAttribute( iStyle, iValue, kATSUDecompositionFactorTag )#define atsuGetDecompositionFactor( iStyle, iValuePtr )		atsuGetOneAttribute( iStyle, kATSUDecompositionFactorTag, sizeof(Fixed), iValuePtr );extern OSStatus atsuSetBaselineClass( ATSUStyle, BslnBaselineClass );#define atsuGetBaselineClass( iStyle, oBaselineClassPtr )	atsuGetOneAttribute( iStyle, kATSUBaselineClassTag, sizeof(BslnBaselineClass), oBaselineClassPtr );extern OSStatus atsuSetPriorityJustOverride( ATSUStyle, ATSJustPriorityWidthDeltaOverrides );#define atsuGetPriorityJustOverride( iStyle, oBaselineClassPtr )	atsuGetOneAttribute( iStyle, kATSUPriorityJustOverrideTag, sizeof(ATSJustPriorityWidthDeltaOverrides), oBaselineClassPtr );																#define atsuSetNoLigatureSplit( iStyle, iSetting )		atsuSetBooleanAttribute( iStyle, iSetting, kATSUNoLigatureSplitTag )#define atsuGetNoLigatureSplit( iStyle, oSettingPtr )	atsuGetOneAttribute( iStyle, kATSUNoLigatureSplitTag, sizeof(Boolean), oSettingPtr )#define atsuSetNoCaretAngle( iStyle, iSetting )		atsuSetBooleanAttribute( iStyle, iSetting, kATSUNoCaretAngleTag )#define atsuGetNoCaretAngle( iStyle, oSettingPtr )	atsuGetOneAttribute( iStyle, kATSUNoCaretAngleTag, sizeof(Boolean), oSettingPtr )#define atsuSetSuppressCrossKerning( iStyle, iSetting )		atsuSetBooleanAttribute( iStyle, iSetting, kATSUSuppressCrossKerningTag )#define atsuGetSuppressCrossKerning( iStyle, oSettingPtr )	atsuGetOneAttribute( iStyle, kATSUSuppressCrossKerningTag, sizeof(Boolean), oSettingPtr )#define atsuSetNoOpticalAlignment( iStyle, iSetting )		atsuSetBooleanAttribute( iStyle, iSetting, kATSUNoOpticalAlignmentTag )#define atsuGetNoOpticalAlignment( iStyle, oSettingPtr )	atsuGetOneAttribute( iStyle, kATSUNoOpticalAlignmentTag, sizeof(Boolean), oSettingPtr )#define atsuSetForceHanging( iStyle, iSetting )		atsuSetBooleanAttribute( iStyle, iSetting, kATSUForceHangingTag )#define atsuGetForceHanging( iStyle, oSettingPtr )	atsuGetOneAttribute( iStyle, kATSUForceHangingTag, sizeof(Boolean), oSettingPtr )#define atsuSetNoSpecialJustification( iStyle, iSetting )		atsuSetBooleanAttribute( iStyle, iSetting, kATSUNoSpecialJustificationTag )#define atsuGetNoSpecialJustification( iStyle, oSettingPtr )	atsuGetOneAttribute( iStyle, kATSUNoSpecialJustificationTag, sizeof(Boolean), oSettingPtr )#if (ATSU_TARG_VERSION >= ATSU_1_2)#define atsuSetTextLocator( iStyle, iLoc )	atsuSetPtrLayoutControl( iStyle, kATSULineTextLocatorTag, iLoc )#define atsuGetTextLocator( iStyle, oLocPtr )	atsuGetOneLayoutControl( iStyle, kATSULineTextLocatorTag, sizeof(TextBreakLocatorRef), oLocPtr )#endif // ATSU_1_2/* Utility functions for layout control accessors */extern OSStatus atsuSetBooleanLayoutControl( ATSUTextLayout, Boolean, ATSUAttributeTag );extern OSStatus atsuSetFixedLayoutControl( ATSUTextLayout, Fixed, ATSUAttributeTag );extern OSStatus atsuSetFractLayoutControl( ATSUTextLayout, Fract, ATSUAttributeTag );extern OSStatus atsuSetShortLayoutControl( ATSUTextLayout, SInt16, ATSUAttributeTag );extern OSStatus atsuSetPtrLayoutControl( ATSUTextLayout, void *, ATSUAttributeTag );extern OSStatus atsuGetOneLayoutControl( ATSUTextLayout 		iLayout,										 ATSUAttributeTag 		iTag,										 ByteCount 				iExpectedValueSize,										 ATSUAttributeValuePtr 	oValue );/* Layout control accessors */#define atsuSetLayoutWidth( iLayout, iWidth )	atsuSetFixedLayoutControl( iLayout, iWidth, kATSULineWidthTag )#define atsuGetLayoutWidth( iLayout, oWidthPtr )	atsuGetOneLayoutControl( iLayout, kATSULineWidthTag, sizeof(ATSUTextMeasurement), oWidthPtr )#if (ATSU_TARG_VERSION >= ATSU_1_1)#define atsuSetLayoutAscent( iLayout, iAscent )		atsuSetFixedLayoutControl( iLayout, iAscent, kATSULineAscentTag )#define atsuGetLayoutAscent( iLayout, oAscentPtr )	atsuGetOneLayoutControl( iLayout, kATSULineAscentTag, sizeof(ATSUTextMeasurement), oAscentPtr )#define atsuSetLayoutDescent( iLayout, iDescent )	atsuSetFixedLayoutControl( iLayout, iDescent, kATSULineDescentTag )#define atsuGetLayoutDescent( iLayout, oDescentPtr )	atsuGetOneLayoutControl( iLayout, kATSULineDescentTag, sizeof(ATSUTextMeasurement), oDescentPtr )extern OSStatus atsuSetLayoutLineSpan( ATSUTextLayout iLayout, Fixed iAscent, Fixed iDescent );extern OSStatus atsuGetLayoutLineSpan( ATSUTextLayout iLayout, Fixed *oAscent, Fixed *oDescent );#endif // ATSU_1_1#define atsuSetLayoutRotation( iLayout, iAngle )	atsuSetFixedLayoutControl( iLayout, iAngle, kATSULineRotationTag )#define atsuGetLayoutRotation( iLayout, oAnglePtr )	atsuGetOneLayoutControl( iLayout, kATSULineRotationTag, sizeof(Fixed), oAnglePtr )#define atsuSetLayoutLineDirection( iLayout, iDirection )		atsuSetBooleanLayoutControl( iLayout, iDirection, kATSULineDirectionTag )#define atsuGetLayoutLineDirection( iLayout, oDirectionPtr )	atsuGetOneLayoutControl( iLayout, kATSULineDirectionTag, sizeof(Boolean), oDirectionPtr )#define atsuSetLayoutJustFactor( iLayout, iValue )		atsuSetFractLayoutControl( iLayout, iValue, kATSULineJustificationFactorTag )#define atsuGetLayoutJustFactor( iLayout, oValuePtr )	atsuGetOneLayoutControl( iLayout, kATSULineJustificationFactorTag, sizeof(Fract), oValuePtr )#define atsuSetLayoutFlushFactor( iLayout, iValue )		atsuSetFractLayoutControl( iLayout, iValue, kATSULineFlushFactorTag )#define atsuGetLayoutFlushFactor( iLayout, oValuePtr )	atsuGetOneLayoutControl( iLayout, kATSULineFlushFactorTag, sizeof(Fract), oValuePtr )extern OSStatus atsuSetLayoutBaselineValues( ATSUTextLayout, BslnBaselineRecord );#define atsuGetLayoutBaselineValues( iLayout, oValuePtr )	atsuGetOneLayoutControl( iLayout, kATSULineBaselineValuesTag, sizeof(BslnBaselineRecord), oValuePtr )#if (ATSU_TARG_VERSION >= ATSU_1_2)#define atsuSetLayoutLanguage( iLayout, iLang )	atsuSetShortLayoutControl( iLayout, kATSULineLanguageTag, iLang )#define atsuGetLayoutLanguage( iLayout, oLangPtr )	atsuGetOneLayoutControl( iLayout, kATSULineLanguageTag, sizeof(RegionCode), oLangPtr )#define atsuSetLayoutTextLocator( iLayout, iLoc )	atsuSetPtrLayoutControl( iLayout, kATSULineTextLocatorTag, iLoc )#define atsuGetLayoutTextLocator( iLayout, oLocPtr )	atsuGetOneLayoutControl( iLayout, kATSULineTextLocatorTag, sizeof(TextBreakLocatorRef), oLocPtr )#endif // ATSU_1_2extern OSStatus atsuSetLayoutOptions( ATSUTextLayout, UInt32 );#define atsuGetLayoutOptions( iLayout, oOptionsPtr )	atsuGetOneLayoutControl( iLayout, kATSULineLayoutOptionsTag, sizeof(UInt32), oOptionsPtr )#if (ATSU_TARG_VERSION >= ATSU_1_1)/* Utility functions for line control accessors */extern OSStatus atsuSetFixedLineControl( ATSUTextLayout, UniCharArrayOffset, Fixed, ATSUAttributeTag );extern OSStatus atsuSetFractLineControl( ATSUTextLayout, UniCharArrayOffset, Fract, ATSUAttributeTag );extern OSStatus atsuGetOneLineControl( ATSUTextLayout 			iLayout,									   UniCharArrayOffset		iLineStart,									   ATSUAttributeTag 		iTag,									   ByteCount 				iExpectedValueSize,									   ATSUAttributeValuePtr	oValue );/* Layout control accessors */#define atsuSetLineWidth( iLayout, iOffset, iWidth )	atsuSetFixedLineControl( iLayout, iOffset, iWidth, kATSULineWidthTag )#define atsuGetLineWidth( iLayout, iOffset, oWidthPtr)	atsuGetOneLineControl( iLayout, iOffset, kATSULineWidthTag, sizeof(ATSUTextMeasurement), oWidthPtr )#define atsuSetLineAscent( iLayout, iOffset, iAscent )		atsuSetFixedLineControl( iLayout, iOffset, iAscent, kATSULineAscentTag )#define atsuGetLineAscent( iLayout, iOffset, oAscentPtr)	atsuGetOneLineControl( iLayout, iOffset, kATSULineAscentTag, sizeof(ATSUTextMeasurement), oAscentPtr )#define atsuSetLineDescent( iLayout, iOffset, iDescent )	atsuSetFixedLineControl( iLayout, iOffset, iDescent, kATSULineDescentTag )#define atsuGetLineDescent( iLayout, iOffset, oDescentPtr)	atsuGetOneLineControl( iLayout, iOffset, kATSULineDescentTag, sizeof(ATSUTextMeasurement), oDescentPtr )extern OSStatus atsuSetLineSpan( ATSUTextLayout iLayout, UniCharArrayOffset iLineStart, Fixed iAscent, Fixed iDescent );extern OSStatus atsuGetLineSpan( ATSUTextLayout iLayout, UniCharArrayOffset iLineStart, Fixed *oAscent, Fixed *oDescent );#define atsuSetLineRotation( iLayout, iOffset, iAngle )		atsuSetFixedLineControl( iLayout, iOffset, iAngle, kATSULineRotationTag )#define atsuGetLineRotation( iLayout, iOffset, oAnglePtr)	atsuGetOneLineControl( iLayout, iOffset, kATSULineRotationTag, sizeof(Fixed), oAnglePtr )#define atsuSetLineJustFactor( iLayout, iOffset, iValue )		atsuSetFractLineControl( iLayout, iOffset, iValue, kATSULineJustificationFactorTag )#define atsuGetLineJustFactor( iLayout, iOffset, oValuePtr )	atsuGetOneLineControl( iLayout, iOffset, kATSULineJustificationFactorTag, sizeof(Fract), oValuePtr )#define atsuSetLineFlushFactor( iLayout, iOffset, iValue )		atsuSetFractLineControl( iLayout, iOffset, iValue, kATSULineFlushFactorTag )#define atsuGetLineFlushFactor( iLayout, iOffset, oValuePtr )	atsuGetOneLineControl( iLayout, iOffset, kATSULineFlushFactorTag, sizeof(Fract), oValuePtr )extern OSStatus atsuSetLineBaselineValues( ATSUTextLayout, UniCharArrayOffset, BslnBaselineRecord );#define atsuGetLineBaselineValues( iLayout, iOffset, oValuePtr )	atsuGetOneLineControl( iLayout, iOffset, kATSULineBaselineValuesTag, sizeof(BslnBaselineRecord), oValuePtr )extern OSStatus atsuSetLineOptions( ATSUTextLayout, UniCharArrayOffset, UInt32 );#define atsuGetLineOptions( iLayout, iOffset, oOptionsPtr )	atsuGetOneLineControl( iLayout, iOffset, kATSULineLayoutOptionsTag, sizeof(UInt32), oOptionsPtr )#endif // ATSU_1_1/*** Some convenient macros for counting things ***//*	Many things are counted by calling the bulk getter with no destination. I find an explicit counter to be more readable,	so I define macros for the "missing" counters.*/#define atsuCountStyleAttributes( iStyle, oTagValuePairCount) \	ATSUGetAllAttributes( iStyle, NULL, 0, oTagValuePairCount )#define atsuCountStyleFontFeatures( iStyle, oActualFeatureCount ) \	ATSUGetAllFontFeatures( iStyle, 0, NULL, NULL, oActualFeatureCount )#define atsuCountStyleFontVariations( iStyle, oActualVariationCount ) \	ATSUGetAllFontVariations( iStyle, 0, NULL, NULL, oActualVariationCount )#define atsuCountLayoutControls( iLayout, oTagValuePairCount) \	ATSUGetAllLayoutControls( iLayout, NULL, 0, oTagValuePairCount )#if (ATSU_TARG_VERSION >= ATSU_1_1)#define atsuCountLineControls( iLayout, iLineStart, oTagValuePairCount) \	ATSUGetAllLineControls( iLayout, iLineStart, NULL, 0, oTagValuePairCount )#endif // ATSU_1_1#define atsuCountSoftLineBreaks( iTextLayout, iRangeStart, iRangeLength, oBreakCount ) \	ATSUGetSoftLineBreaks( iTextLayout, iRangeStart, iRangeLength, 0, NULL, oBreakCount )	/*** Things that will probably end up in the real API ***//*	atsuCloneTextLayout	Create a new text layout that's a copy of the given one. Soft line breaks are maintained in the clone.	The source layout's refCon is not copied.		This can be used to create a layout that covers a subrange of the original one by setting iOffset and iLength	accordingly. To clone the entire range, it's most convenient to use kATSUFromTextBeginning and kATSUToTextEnd.	If expandToLineBoundary is set, the given subrange will be extended in both directions to the nearest soft breaks	or ends of original text span, whichever comes first.*/	extern OSStatus atsuCloneTextLayout(			ATSUTextLayout iTextLayout, UniCharArrayOffset iOffset, UniCharCount iLength, Boolean expandToLineBoundary,			ATSUTextLayout *oTextLayout );/*** Quickdraw interoperation ***//*	atsuSetStyleFromGrafPtr		Set the style's font, QD style and color from the given graf port, if possible. If clearExistingAttributes is set,	the style is cleared before setting new values, otherwise all other settings are kept. If graphPtr is NULL, the	current graf port is used.	If the port's font can't be used with ATSUI, the appropriate error is returned and the style is unchanged.*/	extern OSStatus atsuSetStyleFromGrafPtr( ATSUStyle iStyle, GrafPtr grafPtr, Boolean clearExistingAttributes );/*** Measurement ***//* A point with Fixed instead of integer coordinates */#if (ATSU_TARG_VERSION >= ATSU_1_2)typedef FixedPoint atsuFixedPt;#elsetypedef struct atsuFixedPt {	Fixed x;	Fixed y;} atsuFixedPt;#endif // ATSU_1_2/*	atsuMeasureRotatedTextImage		Return the "true" bounding box of a line of text. ATSUMeasureTextImage() always returns a box with edges	parallel to the coordinate axes. This routine instead returns the four corners of a tight bounding box	that is rotated the same way as the line. For rotations that are multiples of 90 degrees, the results	are identical.*/	extern OSStatus	atsuMeasureRotatedTextImage( ATSUTextLayout iTextLayout, 			UniCharArrayOffset iLineOffset,			UniCharCount iLineLength,			ATSUTextMeasurement iLocationX,			ATSUTextMeasurement iLocationY,			atsuFixedPt *p1,			atsuFixedPt *p2,			atsuFixedPt *p3,			atsuFixedPt *p4			);/*** Drawing ***//*	atsuDrawText	The equivalent to the traditional QD DrawText call. Draws the given text and the current pen loc using	the current port's settings, if possible. The pen is positioned at the end of the text when done.		This is a very inefficient way to use ATSUI, but can be convenient for very simple, relatively rare drawing.*/extern OSStatus atsuDrawText( ConstUniCharArrayPtr	iTextBuf,							  UniCharArrayOffset	iFirstChar,							  UniCharCount			iCharCount );/*	atsuTextBox	ATSUI version of TETextBox. Adds support for justification and 	simple vertical text (i.e. one orientation for CJK and non-CJK characters)		Text font and size are taken from the current port settings		flushness is as defined in <TextEdit.h>: teFlushLeft, teCenter, teFlushRight		justification is a Fract between 0 and 1		text orientation is the enum defined below*/	enum {	katsuHorizontalText = 0,	katsuVerticalText = 1};extern OSStatus atsuTextBox( ConstUniCharArrayPtr 	iText,							 UniCharCount 			iTextLength,							 const Rect *			iBox,							 short 					flushness,							 Fract					justification,							 short					textOrientation,							 Boolean				useFontFallBacks );/*	atsuDrawTextInBox		Draws a Text layout within the specified rect.	Assumes that the layout has already been initialized	and all required attributes and line controls have been set	by the client.*/extern OSStatus atsuDrawTextInBox( ATSUTextLayout iTextLayout, const Rect *iBox );/*** Font Mapping ***//*	atsuFONDtoFontID	This is just like ATSUFONDtoFontID except that it also returns the intrinsic	style of the selected font. This information is needed to correctly adjust the	QD style bits. See the implementation of atsuSetStyleFromGrafPtr for an example.		NB: On Mac OS 9 or later, this just calls through to FMGetFontFromFontFamilyInstance,	which is the preferred function to call.*/extern OSStatus atsuFONDtoFontID( short 			iFONDNumber,								  StyleParameter	iFONDStyle,								  ATSUFontID *		oFontID,								  StyleParameter *	oIntrinsicStyle );								  /*** Font Instances ***/typedef	short atsuInstanceIndex;enum {	katsuDefaultInstance = SHRT_MAX,	katsuNotAnInstance = -1};/*	atsuSetStyleFontInstance	Sets font and variations to correspond to the given font instance. Previous variation settings are cleared.	If the instance index is katsuDefaultInstance, no variations will be set after clearing.*/	extern OSStatus atsuSetStyleFontInstance( ATSUStyle, ATSUFontID, atsuInstanceIndex );/*	atsuGetStyleFontInstance	Returns the style's current font and determine which instance the variations correspond to. If no variations are set,	will return katsuDefaultInstance. If variations don't match any instance, returns katsuNotAnInstance.*/	extern OSStatus atsuGetStyleFontInstance( ATSUStyle theStyle, ATSUFontID* oFontID, atsuInstanceIndex* oInstanceIndex );/*** Standard Font Menu ***//*	atsuAppendFontMenu	Creates a hierarchial font menu and append it to the given menu handle. The top level of the menu will be families.	Submenus are used when there are several members of a family. Font instances are treated as first-class family members,	so they will appear mixed in with "legitimate" family members.		Submenu IDs are assigned sequentially starting with firstHierMenuID. The number of submenus created is returned in	oSubmenuCount. The menuLookupCookie should be used with atsuGetFontForFontMenuItem. You are responsible for disposing	of it when you not longer need the menu.*/	extern OSStatus atsuAppendFontMenu( MenuHandle theMenu, short firstHierMenuID, short* oSubmenuCount, void** menuLookupCookie );/*	atsuGetFontForFontMenuItem	Given a (sub) menu ID and item number within that menu, return the ATSUFontID and instance index corresponding to	that menu item.*/	extern ATSUFontID atsuGetFontForFontMenuItem( short menuID, short itemNum, void* menuLookupCookie, short* oFontInstance );/*	atsuDisposeFontMenuLookupCookie	Free up the storage associated with the lookup cookie once you no longer need the font menu.*/	extern OSStatus atsuDisposeFontMenuLookupCookie( void *menuLookupCookie );/*** Misc. Font Utilities ***//*	atsuFindBestFontName	Given a particular font and name code, and a preference for platform, script or language	(any may be "don't care" values), finds the best available for the current system. See the	implementation for details of the search heuristic. Note that the best available name may	not be in any Macintosh platform encoding. */extern OSStatus atsuFindBestFontName( ATSUFontID theFont,									  FontNameCode iNameCode,									  FontPlatformCode *ioPlatform,									  FontScriptCode *ioScript,									  FontLanguageCode *ioLanguage,									  ByteCount iMaxNameLength,									  Ptr oName,									  ByteCount *oNameLength,									  ItemCount *oNameIndex );/*** Misc. Font Utilities ***//*	atsuFindBestFontName	Given a particular font and name code, and a preference for platform, script or language	(any may be "don't care" values), finds the best available for the current system. See the	implementation for details of the search heuristic. Note that the best available name may	not be in any Macintosh platform encoding. */extern OSStatus atsuFindBestFontName( ATSUFontID theFont,									  FontNameCode iNameCode,									  FontPlatformCode *ioPlatform,									  FontScriptCode *ioScript,									  FontLanguageCode *ioLanguage,									  ByteCount iMaxNameLength,									  Ptr oName,									  ByteCount *oNameLength,									  ItemCount *oNameIndex );