/******************************************************************************
 JXTEBase.cpp

	We implement the routines required by JTextEditor that only depend on JX.

	We paste the following data types:
		XA_STRING, TEXT, text/x-jxstyled0

	We accept drops of the following data types:
		text/plain, text/x-jxstyled0

	To paste other formats, override TEGetExternalClipboard().

	To accept drops of other formats that can be pasted (and should therefore
		display an insertion caret), override TEXWillAcceptDrop() and
		TEXConvertDropData().

	To accept drops of other formats that cannot be pasted, override
		WillAcceptDrop() and -all- four HandleDND*() functions.

	BASE CLASS = JXScrollableWidget, JTextEditor

	Copyright � 1996-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXTEBase.h>
#include <JXSearchTextDialog.h>
#include <JXTEBlinkCaretTask.h>
#include <JXGoToLineDialog.h>
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXScrollbar.h>
#include <JXWindowPainter.h>
#include <JXPSPrinter.h>
#include <JXPTPrinter.h>
#include <JXSelectionManager.h>
#include <JXDNDManager.h>
#include <JXTextSelection.h>
#include <JXColormap.h>
#include <jXGlobals.h>
#include <jXActionDefs.h>
#include <jXKeysym.h>

#include <JFontManager.h>
#include <JString.h>
#include <jASCIIConstants.h>
#include <jStreamUtil.h>
#include <jFileUtil.h>
#include <jProcessUtil.h>
#include <jTime.h>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <jAssert.h>

JXTEBase::PartialWordModifier JXTEBase::itsPWMod = JXTEBase::kCtrlMetaPWMod;
JBoolean JXTEBase::itsWindowsHomeEndFlag         = kJTrue;
JBoolean JXTEBase::itsScrollCaretFlag            = kJFalse;

static const JCharacter* kDNDActionCopyDescrip = "copy the text";
static const JCharacter* kDNDActionMoveDescrip = "move the text";

static const JCharacter* kSelectionDataID = "JXTEBase";

struct MenuItemInfo
{
	JTextEditor::CmdIndex	cmd;
	const JCharacter*		id;
};

// JError data

const JCharacter* JXTEBase::kNoData            = "NoData::JXTEBase";
const JCharacter* JXTEBase::kDataNotCompatible = "DataNotCompatible::JXTEBase";

static const JCharacter* kDataNotCompatibleExtra = "DataNotCompatibleExtra::JXTEBase";

// Edit menu

static const JCharacter* kEditMenuTitleStr    = "Edit";
static const JCharacter* kEditMenuShortcutStr = "#E";

static const JCharacter* kMacEditMenuStr =
	"    Undo       %k Meta-Z       %i" kJXUndoAction
	"  | Redo       %k Meta-Shift-Z %i" kJXRedoAction
	"%l| Cut        %k Meta-X       %i" kJXCutAction
	"  | Copy       %k Meta-C       %i" kJXCopyAction
	"  | Paste      %k Meta-V       %i" kJXPasteAction
	"  | Clear                      %i" kJXClearAction
	"%l| Select all %k Meta-A       %i" kJXSelectAllAction;

static const JCharacter* kWinEditMenuStr =
	"    Undo       %h uz %k Ctrl-Z       %i" kJXUndoAction
	"  | Redo       %h r  %k Ctrl-Shift-Z %i" kJXRedoAction
	"%l| Cut        %h tx %k Ctrl-X       %i" kJXCutAction
	"  | Copy       %h c  %k Ctrl-C       %i" kJXCopyAction
	"  | Paste      %h pv %k Ctrl-V       %i" kJXPasteAction
	"  | Clear      %h l                  %i" kJXClearAction
	"%l| Select all %h a  %k Ctrl-A       %i" kJXSelectAllAction;

#define kCleanRightMarginAction		"CleanRightMarginCmd::JXTEBase"
#define kCoerceRightMarginAction	"CoerceRightMarginCmd::JXTEBase"
#define kShiftSelLeftAction			"ShiftSelLeftCmd::JXTEBase"
#define kShiftSelRightAction		"ShiftSelRightCmd::JXTEBase"
#define kForceShiftSelLeftAction	"ForceShiftSelLeftCmd::JXTEBase"
#define kToggleReadOnlyAction		"ToggleReadOnlyCmd::JXTEBase"

static const JCharacter* kMacAdjustMarginsMenuStr =
	"  Clean right margin  %k Meta-Return       %i" kCleanRightMarginAction
	"| Coerce right margin %k Meta-Shift-Return %i" kCoerceRightMarginAction
	"| Shift left          %k Meta-[            %i" kShiftSelLeftAction
	"| Shift right         %k Meta-]            %i" kShiftSelRightAction
	"| Force shift left    %k Meta-{            %i" kForceShiftSelLeftAction;

static const JCharacter* kWinAdjustMarginsMenuStr =
	"  Clean right margin  %h m %k Ctrl-Return       %i" kCleanRightMarginAction
	"| Coerce right margin %h n %k Ctrl-Shift-Return %i" kCoerceRightMarginAction
	"| Shift left          %h e %k Ctrl-[            %i" kShiftSelLeftAction
	"| Shift right         %h i %k Ctrl-]            %i" kShiftSelRightAction
	"| Force shift left    %h f %k Ctrl-{            %i" kForceShiftSelLeftAction;

static const JCharacter* kMacReadOnlyMenuStr =
	"Read only %b %i" kToggleReadOnlyAction;

static const JCharacter* kWinReadOnlyMenuStr =
	"Read only %b %h o %i" kToggleReadOnlyAction;

static const MenuItemInfo kEditMenuItemInfo[] =
{
	{ JTextEditor::kUndoCmd,      kJXUndoAction      },
	{ JTextEditor::kRedoCmd,      kJXRedoAction      },
	{ JTextEditor::kCutCmd,       kJXCutAction       },
	{ JTextEditor::kCopyCmd,      kJXCopyAction      },
	{ JTextEditor::kPasteCmd,     kJXPasteAction     },
	{ JTextEditor::kDeleteSelCmd, kJXClearAction     },
	{ JTextEditor::kSelectAllCmd, kJXSelectAllAction },

	{ JTextEditor::kCleanRightMarginCmd,  kCleanRightMarginAction  },
	{ JTextEditor::kCoerceRightMarginCmd, kCoerceRightMarginAction },
	{ JTextEditor::kShiftSelLeftCmd,      kShiftSelLeftAction      },
	{ JTextEditor::kShiftSelRightCmd,     kShiftSelRightAction     },
	{ JTextEditor::kForceShiftSelLeftCmd, kForceShiftSelLeftAction },
	{ JTextEditor::kToggleReadOnlyCmd,    kToggleReadOnlyAction    },
};
const JSize kEditMenuItemCount = sizeof(kEditMenuItemInfo)/sizeof(MenuItemInfo);

// used when setting images

enum
{
	kUndoIndex = 1, kRedoIndex,
	kCutIndex, kCopyIndex, kPasteIndex, kClearIndex,
	kSelectAllIndex
};

// Search menu

static const JCharacter* kSearchMenuTitleStr    = "Search";
static const JCharacter* kSearchMenuShortcutStr = "#S";

static const JCharacter* kMacSearchMenuStr =
	"    Find...                   %k Meta-F       %i" kJXFindDialogAction
	"  | Find previous             %k Meta-Shift-G %i" kJXFindPreviousAction
	"  | Find next                 %k Meta-G       %i" kJXFindNextAction
	"%l| Enter search string       %k Meta-E       %i" kJXEnterSearchTextAction
	"  | Enter replace string      %k Meta-Shift-E %i" kJXEnterReplaceTextAction
	"%l| Find selection backwards  %k Meta-Shift-H %i" kJXFindSelectionBackwardsAction
	"  | Find selection forward    %k Meta-H       %i" kJXFindSelectionForwardAction
	"%l| Find clipboard backwards  %k Ctrl-Shift-H %i" kJXFindClipboardBackwardsAction
	"  | Find clipboard forward    %k Ctrl-H       %i" kJXFindClipboardForwardAction;

static const JCharacter* kWinSearchMenuStr =
	"    Find...                   %h f %k Ctrl-F       %i" kJXFindDialogAction
	"  | Find previous             %h p %k Ctrl-Shift-G %i" kJXFindPreviousAction
	"  | Find next                 %h n %k Ctrl-G       %i" kJXFindNextAction
	"%l| Enter search string       %h s %k Ctrl-E       %i" kJXEnterSearchTextAction
	"  | Enter replace string      %h e %k Ctrl-Shift-E %i" kJXEnterReplaceTextAction
	"%l| Find selection backwards  %h b %k Ctrl-Shift-H %i" kJXFindSelectionBackwardsAction
	"  | Find selection forward    %h o %k Ctrl-H       %i" kJXFindSelectionForwardAction
	"%l| Find clipboard backwards  %h k %k Meta-Shift-H %i" kJXFindClipboardBackwardsAction
	"  | Find clipboard forward    %h w %k Meta-H       %i" kJXFindClipboardForwardAction;

static const MenuItemInfo kSearchMenuItemInfo[] =
{
	{ JTextEditor::kFindDialogCmd,            kJXFindDialogAction             },
	{ JTextEditor::kFindPreviousCmd,          kJXFindPreviousAction           },
	{ JTextEditor::kFindNextCmd,              kJXFindNextAction               },
	{ JTextEditor::kEnterSearchTextCmd,       kJXEnterSearchTextAction        },
	{ JTextEditor::kEnterReplaceTextCmd,      kJXEnterReplaceTextAction       },
	{ JTextEditor::kFindSelectionBackwardCmd, kJXFindSelectionBackwardsAction },
	{ JTextEditor::kFindSelectionForwardCmd,  kJXFindSelectionForwardAction   },
	{ JTextEditor::kFindClipboardBackwardCmd, kJXFindClipboardBackwardsAction },
	{ JTextEditor::kFindClipboardForwardCmd,  kJXFindClipboardForwardAction   }
};
const JSize kSearchMenuItemCount = sizeof(kSearchMenuItemInfo)/sizeof(MenuItemInfo);

// used when setting images

enum
{
	kSFindCmd = 1,
	kSFindPrevCmd,          kSFindNextCmd,
	kSEnterSearchStrCmd,    kSEnterReplaceStrCmd,
	kSFindSelectionBackCmd, kSFindSelectionFwdCmd,
	kSFindClipboardBackCmd, kSFindClipboardFwdCmd
};

// Search & Replace menu

static const JCharacter* kMacReplaceMenuStr =
	"    Find...                   %k Meta-F       %i" kJXFindDialogAction
	"  | Find previous             %k Meta-Shift-G %i" kJXFindPreviousAction
	"  | Find next                 %k Meta-G       %i" kJXFindNextAction
	"%l| Enter search string       %k Meta-E       %i" kJXEnterSearchTextAction
	"  | Enter replace string      %k Meta-Shift-E %i" kJXEnterReplaceTextAction
	"%l| Find selection backwards  %k Meta-Shift-H %i" kJXFindSelectionBackwardsAction
	"  | Find selection forward    %k Meta-H       %i" kJXFindSelectionForwardAction
	"%l| Find clipboard backwards  %k Ctrl-Shift-H %i" kJXFindClipboardBackwardsAction
	"  | Find clipboard forward    %k Ctrl-H       %i" kJXFindClipboardForwardAction
	"%l| Replace                   %k Meta-=       %i" kJXReplaceSelectionAction
	"  | Replace and find previous %k Meta-Shift-L %i" kJXReplaceFindPrevAction
	"  | Replace and find next     %k Meta-L       %i" kJXReplaceFindNextAction
	"%l| Replace all backwards                     %i" kJXReplaceAllBackwardsAction
	"  | Replace all forward                       %i" kJXReplaceAllForwardAction
	"  | Replace all in selection                  %i" kJXReplaceAllInSelectionAction;

static const JCharacter* kWinReplaceMenuStr =
	"    Find...                   %h f %k Ctrl-F       %i" kJXFindDialogAction
	"  | Find previous             %h p %k Ctrl-Shift-G %i" kJXFindPreviousAction
	"  | Find next                 %h n %k Ctrl-G       %i" kJXFindNextAction
	"%l| Enter search string       %h s %k Ctrl-E       %i" kJXEnterSearchTextAction
	"  | Enter replace string      %h e %k Ctrl-Shift-E %i" kJXEnterReplaceTextAction
	"%l| Find selection backwards  %h b %k Ctrl-Shift-H %i" kJXFindSelectionBackwardsAction
	"  | Find selection forward    %h o %k Ctrl-H       %i" kJXFindSelectionForwardAction
	"%l| Find clipboard backwards  %h k %k Meta-Shift-H %i" kJXFindClipboardBackwardsAction
	"  | Find clipboard forward    %h w %k Meta-H       %i" kJXFindClipboardForwardAction
	"%l| Replace                   %h r %k Ctrl-=       %i" kJXReplaceSelectionAction
	"  | Replace and find previous %h v %k Ctrl-Shift-L %i" kJXReplaceFindPrevAction
	"  | Replace and find next     %h x %k Ctrl-L       %i" kJXReplaceFindNextAction
	"%l| Replace all backwards     %h c                 %i" kJXReplaceAllBackwardsAction
	"  | Replace all forward       %h d                 %i" kJXReplaceAllForwardAction
	"  | Replace all in selection  %h i                 %i" kJXReplaceAllInSelectionAction;

static const MenuItemInfo kReplaceMenuItemInfo[] =
{
	{ JTextEditor::kFindDialogCmd,            kJXFindDialogAction             },
	{ JTextEditor::kFindPreviousCmd,          kJXFindPreviousAction           },
	{ JTextEditor::kFindNextCmd,              kJXFindNextAction               },
	{ JTextEditor::kEnterSearchTextCmd,       kJXEnterSearchTextAction        },
	{ JTextEditor::kEnterReplaceTextCmd,      kJXEnterReplaceTextAction       },
	{ JTextEditor::kFindSelectionBackwardCmd, kJXFindSelectionBackwardsAction },
	{ JTextEditor::kFindSelectionForwardCmd,  kJXFindSelectionForwardAction   },
	{ JTextEditor::kFindClipboardBackwardCmd, kJXFindClipboardBackwardsAction },
	{ JTextEditor::kFindClipboardForwardCmd,  kJXFindClipboardForwardAction   },
	{ JTextEditor::kReplaceSelectionCmd,      kJXReplaceSelectionAction       },
	{ JTextEditor::kReplaceFindPrevCmd,       kJXReplaceFindPrevAction        },
	{ JTextEditor::kReplaceFindNextCmd,       kJXReplaceFindNextAction        },
	{ JTextEditor::kReplaceAllBackwardCmd,    kJXReplaceAllBackwardsAction    },
	{ JTextEditor::kReplaceAllForwardCmd,     kJXReplaceAllForwardAction      },
	{ JTextEditor::kReplaceAllInSelectionCmd, kJXReplaceAllInSelectionAction  }
};
const JSize kReplaceMenuItemCount = sizeof(kReplaceMenuItemInfo)/sizeof(MenuItemInfo);

// used when setting images

enum
{
	kSRFindCmd = 1,
	kSRFindPrevCmd,          kSRFindNextCmd,
	kSREnterSearchStrCmd,    kSREnterReplaceStrCmd,
	kSRFindSelectionBackCmd, kSRFindSelectionFwdCmd,
	kSRFindClipboardBackCmd, kSRFindClipboardFwdCmd,
	kSRReplaceCmd,
	kSRReplaceAndFindPrevCmd, kSRReplaceAndFindNextCmd,
	kSRReplaceAllBackwardCmd, kSRReplaceAllForwardCmd,
	kSRReplaceAllInSelectionCmd
};

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXTEBase::JXTEBase
	(
	const Type			type,
	const JBoolean		breakCROnly,
	const JBoolean		pasteStyledText,
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXScrollableWidget(scrollbarSet, enclosure, hSizing, vSizing, x,y, w,h),
	JTextEditor(type, breakCROnly, pasteStyledText, kJFalse,
				GetFontManager(), GetColormap(),
				(GetColormap())->GetRedColor(),					// caret
				(GetColormap())->GetDefaultSelectionColor(),	// selection filled
				(GetColormap())->GetBlueColor(),				// selection outline
				(GetColormap())->GetBlackColor(),				// drag
				(GetColormap())->GetGray70Color(),				// whitespace
				GetApertureWidth()),

	itsWillPasteCustomFlag( kJFalse )
{
	itsEditMenu    = NULL;
	itsSearchMenu  = NULL;
	itsReplaceMenu = NULL;

	itsCanAdjustMarginsFlag  = kJFalse;
	itsCanToggleReadOnlyFlag = kJFalse;

	itsPSPrinter      = NULL;
	itsPSPrintName    = NULL;
	itsPTPrinter      = NULL;
	itsPTPrintName    = NULL;

	itsGoToLineDialog = NULL;

	itsDNDDragInfo    = NULL;
	itsDNDDropInfo    = NULL;

	itsBlinkTask = new JXTEBlinkCaretTask(this);
	assert( itsBlinkTask != NULL );
	TECaretShouldBlink(kJTrue);

	itsMinWidth = itsMinHeight = 0;
	RecalcAll(kJTrue);

	itsStyledText0XAtom =
		(GetDisplay())->RegisterXAtom(JXTextSelection::GetStyledText0XAtomName());

	if (type == kStaticText)
		{
		WantInput(kJFalse);
		}
	else
		{
		WantInput(kJTrue);
		SetDefaultCursor(kJXTextEditCursor);
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXTEBase::~JXTEBase()
{
	JXSearchTextDialog* dlog;
	if (JXGetSearchTextDialog(&dlog))
		{
		dlog->TEDeactivated(this);
		}

	delete itsPSPrintName;
	delete itsPTPrintName;
	delete itsBlinkTask;
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
JXTEBase::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	TEDraw(p, rect);
}

/******************************************************************************
 HandleMouseEnter

 ******************************************************************************/

void
JXTEBase::HandleMouseEnter()
{
	ShowCursor();
	itsPrevMousePt = JPoint(0,0);

	JXScrollableWidget::HandleMouseEnter();
}

/******************************************************************************
 HandleMouseHere

 ******************************************************************************/

void
JXTEBase::HandleMouseHere
	(
	const JPoint&			pt,
	const JXKeyModifiers&	modifiers
	)
{
	if (pt != itsPrevMousePt)
		{
		ShowCursor();
		}
	itsPrevMousePt = pt;

	JXScrollableWidget::HandleMouseHere(pt, modifiers);
}

/******************************************************************************
 HandleMouseLeave

 ******************************************************************************/

void
JXTEBase::HandleMouseLeave()
{
	ShowCursor();
	JXScrollableWidget::HandleMouseLeave();
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
JXTEBase::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsSearchMenu != NULL || itsReplaceMenu != NULL)
		{
		(JXGetSearchTextDialog())->SetActiveTE(this);
		}

	const Type type = GetType();
	if (button > kJXRightButton)
		{
		ScrollForWheel(button, modifiers);
		}
	else if (type != kStaticText)
		{
		ShowCursor();

		if (button == kJXMiddleButton && type == kFullEditor)
			{
			TEHandleMouseDown(pt, 1, kJFalse, kJFalse);
			TEHandleMouseUp(kJFalse);
			Paste();
			}
		else if (button != kJXMiddleButton)
			{
			const JBoolean extendSelection = JI2B(
				button == kJXRightButton || modifiers.shift() );
			const JBoolean partialWord = JI2B(
				(itsPWMod == kCtrlMetaPWMod &&
				 modifiers.control() && modifiers.meta()) ||
				(itsPWMod != kCtrlMetaPWMod &&
				 modifiers.GetState(kJXMod2KeyIndex + itsPWMod - kMod2PWMod)));
			TEHandleMouseDown(pt, clickCount, extendSelection, partialWord);
			}
		}
}

/******************************************************************************
 HandleMouseDrag (virtual protected)

 ******************************************************************************/

void
JXTEBase::HandleMouseDrag
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	DNDDragInfo info(&pt, &buttonStates, &modifiers);
	itsDNDDragInfo = &info;

	TEHandleMouseDrag(pt);

	itsDNDDragInfo = NULL;
}

/******************************************************************************
 HandleMouseUp (virtual protected)

 ******************************************************************************/

void
JXTEBase::HandleMouseUp
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	TEHandleMouseUp(modifiers.meta());
}

/******************************************************************************
 HitSamePart (virtual protected)

 ******************************************************************************/

JBoolean
JXTEBase::HitSamePart
	(
	const JPoint& pt1,
	const JPoint& pt2
	)
	const
{
	return TEHitSamePart(pt1, pt2);
}

/******************************************************************************
 TEDisplayBusyCursor (virtual protected)

 ******************************************************************************/

void
JXTEBase::TEDisplayBusyCursor()
	const
{
	(JXGetApplication())->DisplayBusyCursor();
}

/******************************************************************************
 TEBeginDND (virtual protected)

 ******************************************************************************/

JBoolean
JXTEBase::TEBeginDND()
{
	assert( itsDNDDragInfo != NULL );

	JXTextSelection* data = new JXTextSelection(this, kSelectionDataID);
	assert( data != NULL );

	return BeginDND(*(itsDNDDragInfo->pt), *(itsDNDDragInfo->buttonStates),
					*(itsDNDDragInfo->modifiers), data);
}

/******************************************************************************
 DNDFinish (virtual protected)

 ******************************************************************************/

void
JXTEBase::DNDFinish
	(
	const JBoolean		isDrop,
	const JXContainer*	target
	)
{
	TEDNDFinished();
}

/******************************************************************************
 GetDNDAction (virtual protected)

	This is called repeatedly during the drag so the drop action can be
	changed based on the current target, buttons, and modifier keys.

 ******************************************************************************/

Atom
JXTEBase::GetDNDAction
	(
	const JXContainer*		target,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	const Type type = GetType();
	if (type == kFullEditor && modifiers.control())
		{
		return (GetDNDManager())->GetDNDActionAskXAtom();
		}
	else if (type == kFullEditor &&
			 ((target == this && !modifiers.meta()) ||
			  (target != this &&  modifiers.meta())))
		{
		return (GetDNDManager())->GetDNDActionMoveXAtom();
		}
	else
		{
		return (GetDNDManager())->GetDNDActionCopyXAtom();
		}
}

/******************************************************************************
 GetDNDAskActions (virtual protected)

	This is called when the value returned by GetDNDAction() changes to
	XdndActionAsk.  If GetDNDAction() repeatedly returns XdndActionAsk,
	this function is not called again because it is assumed that the
	actions are the same within a single DND session.

	This function must place at least 2 elements in askActionList and
	askDescriptionList.

	The first element should be the default action.

 ******************************************************************************/

void
JXTEBase::GetDNDAskActions
	(
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers,
	JArray<Atom>*			askActionList,
	JPtrArray<JString>*		askDescriptionList
	)
{
	assert( GetType() == kFullEditor );

	JXDNDManager* dndMgr = GetDNDManager();
	askActionList->AppendElement(dndMgr->GetDNDActionCopyXAtom());
	askActionList->AppendElement(dndMgr->GetDNDActionMoveXAtom());

	JString* s = new JString(kDNDActionCopyDescrip);
	assert( s != NULL );
	askDescriptionList->Append(s);

	s = new JString(kDNDActionMoveDescrip);
	assert( s != NULL );
	askDescriptionList->Append(s);
}

/******************************************************************************
 GetSelectionData (virtual protected)

	This is called when DND is terminated by a drop or when the target
	requests the data during a drag, but only if the delayed evaluation
	constructor for JXSelectionData was used.

	id is the string passed to the JXSelectionData constructor.

 ******************************************************************************/

void
JXTEBase::GetSelectionData
	(
	JXSelectionData*	data,
	const JCharacter*	id
	)
{
	if (strcmp(id, kSelectionDataID) == 0)
		{
		JXTextSelection* textData = dynamic_cast(JXTextSelection*, data);
		assert( textData != NULL );

		JString* text = new JString;
		assert( text != NULL );

		JRunArray<Font>* style = new JRunArray<Font>;
		assert( style != NULL );

		const JBoolean ok = GetSelection(text, style);
		assert( ok );
		textData->SetData(text, GetColormap(), style);

		if (GetType() == kFullEditor)
			{
			JIndexRange r;
			const JBoolean ok = GetSelection(&r);
			assert( ok );
			textData->SetTextEditor(this, r);
			}
		}
	else
		{
		JXScrollableWidget::GetSelectionData(data, id);
		}
}

/******************************************************************************
 WillAcceptDrop (virtual protected)

 ******************************************************************************/

JBoolean
JXTEBase::WillAcceptDrop
	(
	const JArray<Atom>&	typeList,
	Atom*				action,
	const Time			time,
	const JXWidget*		source
	)
{
	itsWillPasteCustomFlag = kJFalse;

	if (GetType() != kFullEditor)
		{
		return kJFalse;
		}
	else if (source == this)
		{
		return kJTrue;
		}
	else if (TEXWillAcceptDrop(typeList, *action, time, source))
		{
		itsWillPasteCustomFlag = kJTrue;
		return kJTrue;
		}
	else
		{
		JXDNDManager* dndMgr = GetDNDManager();
		JBoolean canGetStyledText, canGetText;
		Atom textType;
		return JI2B(
			(*action == dndMgr->GetDNDActionCopyXAtom() ||
			 *action == dndMgr->GetDNDActionMoveXAtom() ||
			 *action == dndMgr->GetDNDActionAskXAtom()) &&
			GetAvailDataTypes(typeList, &canGetStyledText, &canGetText, &textType));
		}
}

/******************************************************************************
 TEXWillAcceptDrop (virtual protected)

	Derived classes can override this to accept other data types that
	can be pasted.  Returning kJTrue guarantees that TEXConvertDropData()
	will be called, and this can be used to accept a different data type
	even when one of the default types (e.g. text/plain) is available.

	Data types that cannot be pasted must be implemented by overriding
	WillAcceptDrop() and -all- four HandleDND*() functions.

	When overriding this function, derived classes must also override
	TEXConvertDropData() to process the actual data that is dropped.

	source is non-NULL if the drag is between widgets in the same program.
	This provides a way for compound documents to identify drags between their
	various parts.

 ******************************************************************************/

JBoolean
JXTEBase::TEXWillAcceptDrop
	(
	const JArray<Atom>&	typeList,
	const Atom			action,
	const Time			time,
	const JXWidget*		source
	)
{
	return kJFalse;
}

/******************************************************************************
 HandleDNDEnter (virtual protected)

	This is called when the mouse enters the widget.

 ******************************************************************************/

void
JXTEBase::HandleDNDEnter()
{
	TEHandleDNDEnter();
}

/******************************************************************************
 HandleDNDHere (virtual protected)

	This is called while the mouse is inside the widget.

 ******************************************************************************/

void
JXTEBase::HandleDNDHere
	(
	const JPoint&	pt,
	const JXWidget*	source
	)
{
	TEHandleDNDHere(pt, JI2B(source == this));
}

/******************************************************************************
 HandleDNDLeave (virtual protected)

	This is called when the mouse leaves the widget without dropping data.

 ******************************************************************************/

void
JXTEBase::HandleDNDLeave()
{
	TEHandleDNDLeave();
}

/******************************************************************************
 HandleDNDDrop (virtual protected)

	This is called when the data is dropped.  The data is accessed via
	the selection manager, just like Paste.

 ******************************************************************************/

void
JXTEBase::HandleDNDDrop
	(
	const JPoint&		pt,
	const JArray<Atom>&	typeList,
	const Atom			action,
	const Time			time,
	const JXWidget*		source
	)
{
	DNDDropInfo info(&typeList, action, time);
	itsDNDDropInfo = &info;

	JXDNDManager* dndMgr  = GetDNDManager();
	const Atom copyAction = dndMgr->GetDNDActionCopyXAtom();
	if (!itsWillPasteCustomFlag &&
		action == dndMgr->GetDNDActionAskXAtom())
		{
		JArray<Atom> actionList;
		JPtrArray<JString> descriptionList(JPtrArrayT::kDeleteAll);
		if (!dndMgr->GetAskActions(&actionList, &descriptionList))
			{
			itsDNDDropInfo->action = copyAction;
			}
		else if (!dndMgr->ChooseDropAction(actionList, descriptionList,
										   &(itsDNDDropInfo->action)))
			{
			TEHandleDNDLeave();
			return;
			}
		}

	TEHandleDNDDrop(pt, JI2B(source == this),
					JI2B(itsDNDDropInfo->action == copyAction));

	itsDNDDropInfo = NULL;
}

/******************************************************************************
 TEPasteDropData (virtual protected)

	Get the data that was dropped and paste it in.

 ******************************************************************************/

void
JXTEBase::TEPasteDropData()
{
	assert( itsDNDDropInfo != NULL );

	JXDNDManager* dndMgr     = GetDNDManager();
	const Atom selectionName = dndMgr->GetDNDSelectionName();

	JString text;
	JRunArray<Font> style;
	if (itsWillPasteCustomFlag &&
		TEXConvertDropData(*(itsDNDDropInfo->typeList), itsDNDDropInfo->action,
						   itsDNDDropInfo->time, &text, &style))
		{
		JRunArray<Font>* s = (style.IsEmpty() ? NULL : &style);
		Paste(text, s);
		}
	else if (!itsWillPasteCustomFlag &&
			 GetSelectionData(selectionName, *(itsDNDDropInfo->typeList),
							  itsDNDDropInfo->time, &text, &style) == kJNoError)
		{
		JRunArray<Font>* s = (style.IsEmpty() ? NULL : &style);
		Paste(text, s);

		if (itsDNDDropInfo->action == dndMgr->GetDNDActionMoveXAtom())
			{
			(GetSelectionManager())->
				SendDeleteRequest(selectionName, itsDNDDropInfo->time);
			}
		}
}

/******************************************************************************
 TEXConvertDropData (virtual protected)

	Derived classes can override this to convert other data types into
	text and styles that can be pasted.  Return kJTrue to paste the contents
	of text and style.  To paste unstyled text, leave style empty.

	This function will only be called if the derived class implemented
	TEXWillAcceptDrop() and returned kJTrue.

 ******************************************************************************/

JBoolean
JXTEBase::TEXConvertDropData
	(
	const JArray<Atom>&	typeList,
	const Atom			action,
	const Time			time,
	JString*			text,
	JRunArray<Font>*	style
	)
{
	return kJFalse;
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
JXTEBase::Activate()
{
	JXScrollableWidget::Activate();
	if (IsActive() && HasFocus())
		{
		TEActivate();
		if ((GetWindow())->HasFocus())
			{
			itsBlinkTask->Reset();
			(JXGetApplication())->InstallIdleTask(itsBlinkTask);
			TEActivateSelection();
			}
		}
}

/******************************************************************************
 Deactivate (virtual)

 ******************************************************************************/

void
JXTEBase::Deactivate()
{
	JXScrollableWidget::Deactivate();
	if (!IsActive())
		{
		TEDeactivate();
		(JXGetApplication())->RemoveIdleTask(itsBlinkTask);
		TEDeactivateSelection();
		}
}

/******************************************************************************
 HandleFocusEvent (virtual protected)

 ******************************************************************************/

void
JXTEBase::HandleFocusEvent()
{
	JXScrollableWidget::HandleFocusEvent();
	TEActivate();
	if (IsActive() && (GetWindow())->HasFocus())
		{
		itsBlinkTask->Reset();
		(JXGetApplication())->InstallIdleTask(itsBlinkTask);
		TEActivateSelection();

		if (itsSearchMenu != NULL || itsReplaceMenu != NULL)
			{
			(JXGetSearchTextDialog())->SetActiveTE(this);
			}
		}
}

/******************************************************************************
 HandleUnfocusEvent (virtual protected)

 ******************************************************************************/

void
JXTEBase::HandleUnfocusEvent()
{
	JXScrollableWidget::HandleUnfocusEvent();
	TEDeactivate();
	(JXGetApplication())->RemoveIdleTask(itsBlinkTask);
	TEDeactivateSelection();

	if (itsSearchMenu != NULL || itsReplaceMenu != NULL)
		{
		(JXGetSearchTextDialog())->TEDeactivated(this);
		}
}

/******************************************************************************
 HandleWindowFocusEvent (virtual protected)

 ******************************************************************************/

void
JXTEBase::HandleWindowFocusEvent()
{
	JXScrollableWidget::HandleWindowFocusEvent();
	TEActivateSelection();
	if (IsActive() && HasFocus())
		{
		itsBlinkTask->Reset();
		(JXGetApplication())->InstallIdleTask(itsBlinkTask);
		}
}

/******************************************************************************
 HandleWindowUnfocusEvent (virtual protected)

 ******************************************************************************/

void
JXTEBase::HandleWindowUnfocusEvent()
{
	JXScrollableWidget::HandleWindowUnfocusEvent();
	TEHideCaret();
	TEDeactivateSelection();
	(JXGetApplication())->RemoveIdleTask(itsBlinkTask);
}

/******************************************************************************
 TECaretShouldBlink (virtual protected)

 ******************************************************************************/

void
JXTEBase::TECaretShouldBlink
	(
	const JBoolean blink
	)
{
	itsBlinkTask->ShouldBlink(blink);
}

/******************************************************************************
 HandleKeyPress (virtual)

	We handle all the edit shortcuts here because we won't always have
	an Edit menu.

 ******************************************************************************/

void
JXTEBase::HandleKeyPress
	(
	const int				origKey,
	const JXKeyModifiers&	origModifiers
	)
{
	if (itsSearchMenu != NULL || itsReplaceMenu != NULL)
		{
		(JXGetSearchTextDialog())->SetActiveTE(this);
		}

	int key                  = origKey;
	JXKeyModifiers modifiers = origModifiers;
	if (itsWindowsHomeEndFlag)
		{
		RemapWindowsHomeEnd(&key, &modifiers);
		}

	const JBoolean controlOn = modifiers.control();
	const JBoolean metaOn    = modifiers.meta();
	const JBoolean shiftOn   = modifiers.shift();

	const Type type = GetType();
	if (type == kFullEditor && !controlOn && !metaOn &&
		((GetDisplay())->GetLatestButtonStates()).AllOff())
		{
		HideCursor();
		}

	JBoolean processed = kJFalse;
	char typedKey;

	if (type == kFullEditor &&
		(((key == 'z' || key == 'Z') && !controlOn &&  metaOn && !shiftOn) ||
		 (key == JXCtrl('Z')         &&  controlOn && !metaOn && !shiftOn)))
		{
		Undo();
		processed = kJTrue;
		}
	else if (type == kFullEditor &&
			 (((key == 'x' || key == 'X') && !controlOn &&  metaOn && !shiftOn) ||
			  (key == JXCtrl('X')         &&  controlOn && !metaOn && !shiftOn)))
		{
		Cut();
		processed = kJTrue;
		}
	else if (((key == 'c' || key == 'C') && !controlOn &&  metaOn && !shiftOn) ||
			 (key == JXCtrl('C')         &&  controlOn && !metaOn && !shiftOn))
		{
		Copy();
		processed = kJTrue;
		}
	else if (type == kFullEditor &&
			 (((key == 'v' || key == 'V') && !controlOn &&  metaOn && !shiftOn) ||
			  (key == JXCtrl('V')         &&  controlOn && !metaOn && !shiftOn)))
		{
		Paste();
		processed = kJTrue;
		}
	else if (((key == 'a' || key == 'A') && !controlOn &&  metaOn && !shiftOn) ||
			 (key == JXCtrl('A')         &&  controlOn && !metaOn && !shiftOn))
		{
		SelectAll();
		processed = kJTrue;
		}

	else if (key == kJLeftArrow || key == kJRightArrow ||
			 key == kJUpArrow   || key == kJDownArrow)
		{
		CaretMotion motion = kMoveByCharacter;
		if ((itsPWMod == kCtrlMetaPWMod && controlOn && metaOn) ||
			(itsPWMod != kCtrlMetaPWMod &&
			 modifiers.GetState(kJXMod2KeyIndex + itsPWMod - kMod2PWMod)))
			{
			motion = kMoveByPartialWord;
			}
		else if (controlOn)
			{
			motion = kMoveByWord;
			}
		else if (metaOn)
			{
			motion = kMoveByLine;
			}

		if (type == kFullEditor || shiftOn || motion != kMoveByCharacter)
			{
			processed = TEHandleKeyPress(key, shiftOn, motion);
			}
		else
			{
			processed = kJFalse;
			}
		}

	else if (type == kFullEditor &&
			 !controlOn && !metaOn &&
			 OKToPassToJTE(key, &typedKey))
		{
		processed = TEHandleKeyPress(typedKey, shiftOn, kMoveByCharacter);
		}

	if (!processed)
		{
		if (itsScrollCaretFlag)
			{
			// move caret to top/bottom if already scrolled there

			const JRect ap = GetAperture();
			const JRect b  = GetBounds();

			if ((key == XK_Page_Up || key == XK_KP_Page_Up) &&
				 ap.top == b.top)
				{
				SetCaretLocation(1);
				}
			else if ((key == XK_Page_Down || key == XK_KP_Page_Down) &&
					  ap.bottom == b.bottom)
				{
				SetCaretLocation(GetTextLength()+1);
				}
			}

		int k = key;
		if (k == ' ')
			{
			k = XK_Page_Down;
			}
		else if (k == kJDeleteKey)
			{
			k = XK_Page_Up;
			}

		JXScrollableWidget::HandleKeyPress(k, modifiers);

		if (itsScrollCaretFlag)
			{
			// move caret to top/bottom regardless of where it was

			if (key == XK_Home || key == XK_KP_Home)
				{
				SetCaretLocation(1);
				}
			else if (key == XK_End || key == XK_KP_End)
				{
				SetCaretLocation(GetTextLength()+1);
				}
			}
		}
}

/******************************************************************************
 OKToPassToJTE (private)

	Map Latin 1,2,3,4,9 to 8-bit ASCII.

 ******************************************************************************/

JBoolean
JXTEBase::OKToPassToJTE
	(
	const int	key,
	char*		c
	)
	const
{
	const int flags = 0xFF00 & key;
	*c              = 0x00FF & key;
	return JI2B(*c != 0 &&
				(flags == 0x0000 || flags == 0x0100 || flags == 0x0200 ||
				 flags == 0x0300 || flags == 0x1300));
}

/******************************************************************************
 RemapWindowsHomeEnd (private)

	Home/End		=> beginning/end of line
	Ctrl-Home/End	=> scroll to top/bottom

 ******************************************************************************/

void
JXTEBase::RemapWindowsHomeEnd
	(
	int*			key,
	JXKeyModifiers*	modifiers
	)
	const
{
	if ((*key == XK_Home || *key == XK_KP_Home ||
		 *key == XK_End  || *key == XK_KP_End) &&
		modifiers->control())
		{
		modifiers->SetState(kJXControlKeyIndex, kJFalse);
		}
	else if (*key == XK_Home || *key == XK_KP_Home)
		{
		*key = kJLeftArrow;
		modifiers->SetState(kJXMetaKeyIndex, kJTrue);
		}
	else if (*key == XK_End || *key == XK_KP_End)
		{
		*key = kJRightArrow;
		modifiers->SetState(kJXMetaKeyIndex, kJTrue);
		}
}

/******************************************************************************
 MoveCaretToEdge

	key can be either kJUpArrow or kJDownArrow.

 ******************************************************************************/

void
JXTEBase::MoveCaretToEdge
	(
	const int key
	)
{
	JFontID id;
	JSize size;
	JFontStyle style;
	GetDefaultFont(&id, &size, &style);
	const JSize h = (3 * (GetFontManager())->GetLineHeight(id, size, style)) / 4;

	JPoint pt;
	if (key == kJUpArrow)
		{
		pt    = (GetAperture()).topLeft();
		pt.y += h;
		}
	else if (key == kJDownArrow)
		{
		pt    = (GetAperture()).bottomLeft();
		pt.y -= h;
		}
	else
		{
		return;
		}

	const CaretLocation caretLoc = CalcCaretLocation(pt);
	SetCaretLocation(caretLoc.charIndex);
}

/******************************************************************************
 HandleShortcut (virtual)

 ******************************************************************************/

void
JXTEBase::HandleShortcut
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsSearchMenu != NULL || itsReplaceMenu != NULL)
		{
		(JXGetSearchTextDialog())->SetActiveTE(this);
		}
	JXScrollableWidget::HandleShortcut(key, modifiers);
}

/******************************************************************************
 AdjustCursor (virtual protected)

	Show the default cursor during drag-and-drop.

 ******************************************************************************/

void
JXTEBase::AdjustCursor
	(
	const JPoint&			pt,
	const JXKeyModifiers&	modifiers
	)
{
	if (TEWillDragAndDrop(pt, kJFalse, modifiers.meta()))
		{
		DisplayCursor(kJXDefaultCursor);
		}
	else
		{
		JXScrollableWidget::AdjustCursor(pt, modifiers);
		}
}

/******************************************************************************
 BoundsMoved (virtual protected)

 ******************************************************************************/

void
JXTEBase::BoundsMoved
	(
	const JCoordinate dx,
	const JCoordinate dy
	)
{
	JXScrollableWidget::BoundsMoved(dx, dy);

	if (itsScrollCaretFlag && dy > 0 &&
		GetLineTop(GetLineForChar(GetInsertionIndex())) > (GetAperture()).bottom)
		{
		MoveCaretToEdge(kJDownArrow);
		}
	else if (itsScrollCaretFlag && dy < 0 &&
			 GetLineBottom(GetLineForChar(GetInsertionIndex())) < (GetAperture()).top)
		{
		MoveCaretToEdge(kJUpArrow);
		}
}

/******************************************************************************
 ApertureResized (virtual protected)

 ******************************************************************************/

void
JXTEBase::ApertureResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXScrollableWidget::ApertureResized(dw,dh);
	TESetBoundsWidth(GetApertureWidth());
	TESetGUIBounds(itsMinWidth, itsMinHeight, -1);
}

/******************************************************************************
 TERefresh (virtual protected)

	Not inline because it is virtual.

 ******************************************************************************/

void
JXTEBase::TERefresh()
{
	Refresh();
}

/******************************************************************************
 TERefreshRect (virtual protected)

	Not inline because it is virtual.

 ******************************************************************************/

void
JXTEBase::TERefreshRect
	(
	const JRect& rect
	)
{
	RefreshRect(rect);
}

/******************************************************************************
 TERedraw (virtual protected)

	Not inline because it is virtual.

 ******************************************************************************/

void
JXTEBase::TERedraw()
{
	Redraw();
}

/******************************************************************************
 TESetGUIBounds (virtual protected)

	Keep the bounds at least as large as the aperture.

 ******************************************************************************/

void
JXTEBase::TESetGUIBounds
	(
	const JCoordinate w,
	const JCoordinate h,
	const JCoordinate changeY
	)
{
	itsMinWidth  = w;
	itsMinHeight = h;

	const JRect ap = GetApertureGlobal();

	JCoordinate width = ap.width();
	if (width < w)
		{
		width = w;
		}

	JCoordinate height = ap.height();
	if (height < h)
		{
		height = h;
		}

	JXScrollbar *hScrollbar, *vScrollbar;
	if (changeY >= 0 && GetScrollbars(&hScrollbar, &vScrollbar))
		{
		const JCoordinate origH = GetBoundsHeight();
		if (height < origH)
			{
			vScrollbar->PrepareForLowerMaxValue(changeY, origH - height);
			}
		else if (height > origH)
			{
			vScrollbar->PrepareForHigherMaxValue(changeY, height - origH);
			}
		}

	SetBounds(width, height);
}

/******************************************************************************
 TEWidthIsBeyondDisplayCapacity (virtual protected)

 ******************************************************************************/

JBoolean
JXTEBase::TEWidthIsBeyondDisplayCapacity
	(
	const JSize width
	)
	const
{
	return JI2B( width > 16000 );	// X uses 2 bytes for coordinate value
}

/******************************************************************************
 TEScrollToRect (virtual protected)

 ******************************************************************************/

JBoolean
JXTEBase::TEScrollToRect
	(
	const JRect&	rect,
	const JBoolean	centerInDisplay
	)
{
	if (centerInDisplay && rect.right <= GetApertureWidth())
		{
		JRect r = rect;
		r.left  = 0;
		return ScrollToRectCentered(r, kJFalse);
		}
	else if (centerInDisplay)
		{
		return ScrollToRectCentered(rect, kJFalse);
		}
	else
		{
		return ScrollToRect(rect);
		}
}

/******************************************************************************
 TEScrollForDrag (virtual protected)

	Not inline because it is virtual.

 ******************************************************************************/

JBoolean
JXTEBase::TEScrollForDrag
	(
	const JPoint& pt
	)
{
	return ScrollForDrag(pt);
}

/******************************************************************************
 TESetVertScrollStep (virtual protected)

 ******************************************************************************/

void
JXTEBase::TESetVertScrollStep
	(
	const JCoordinate vStep
	)
{
	SetVertStepSize(vStep);
	SetVertPageStepContext(vStep);
}

/******************************************************************************
 TEClipboardChanged (virtual protected)

 ******************************************************************************/

void
JXTEBase::TEClipboardChanged()
{
	if (HasSelection())
		{
		JString* text = new JString;
		assert( text != NULL );

		JRunArray<Font>* style = new JRunArray<Font>;
		assert( style != NULL );

		const JBoolean ok = GetSelection(text, style);
		assert( ok );

		JXTextSelection* data = new JXTextSelection(GetDisplay(), text, GetColormap(), style);
		assert( data != NULL );

		if (!(GetSelectionManager())->SetData(kJXClipboardName, data))
			{
			(JGetUserNotification())->ReportError("Unable to copy to the X Clipboard.");
			}
		}
}

/******************************************************************************
 TEOwnsClipboard (virtual protected)

	We told JTextEditor not to store the clipboard data.

 ******************************************************************************/

JBoolean
JXTEBase::TEOwnsClipboard()
	const
{
	return kJFalse;
}

/******************************************************************************
 TEGetExternalClipboard (virtual protected)

	Returns kJTrue if there is something pasteable on the system clipboard.

 ******************************************************************************/

JBoolean
JXTEBase::TEGetExternalClipboard
	(
	JString*			text,
	JRunArray<Font>*	style
	)
	const
{
	const JError err = GetSelectionData(kJXClipboardName, CurrentTime, text, style);
	if (err.OK())
		{
		return kJTrue;
		}
	else
		{
		if (err != kNoData)
			{
			err.ReportIfError();
			}
		return kJFalse;
		}
}

/******************************************************************************
 GetAvailDataTypes (private)

	Returns kJTrue if it can find a data type that we understand.

 ******************************************************************************/

JBoolean
JXTEBase::GetAvailDataTypes
	(
	const JArray<Atom>&	typeList,
	JBoolean*			canGetStyledText,
	JBoolean*			canGetText,
	Atom*				textType
	)
	const
{
	*canGetStyledText = kJFalse;
	*canGetText       = kJFalse;
	*textType         = None;

	JXSelectionManager* selMgr = GetSelectionManager();

	const JSize typeCount = typeList.GetElementCount();
	for (JIndex i=1; i<=typeCount; i++)
		{
		const Atom type = typeList.GetElement(i);
		if (type == XA_STRING ||
			type == selMgr->GetMimePlainTextXAtom() ||
			(!(*canGetText) && type == selMgr->GetTextXAtom()))
			{
			*canGetText = kJTrue;
			*textType   = type;
			}

		// By checking WillPasteStyledText(), we avoid wasting time
		// parsing style information.

		else if (type == itsStyledText0XAtom && WillPasteStyledText())
			{
			*canGetStyledText = kJTrue;
			}
		}

	return JConvertToBoolean( *canGetStyledText || *canGetText );
}

/******************************************************************************
 GetSelectionData (private)

	Returns kJTrue if there is something pasteable in the given selection.

 ******************************************************************************/

JError
JXTEBase::GetSelectionData
	(
	const Atom			selectionName,
	const Time			time,
	JString*			text,
	JRunArray<Font>*	style
	)
	const
{
	JArray<Atom> typeList;
	if ((GetSelectionManager())->
			GetAvailableTypes(selectionName, time, &typeList))
		{
		return GetSelectionData(selectionName, typeList, time, text, style);
		}
	else
		{
		return NoData();
		}
}

/******************************************************************************
 GetSelectionData (private)

	Returns kJTrue if there is something pasteable in the given selection.

 ******************************************************************************/

JError
JXTEBase::GetSelectionData
	(
	const Atom			selectionName,
	const JArray<Atom>&	typeList,
	const Time			time,
	JString*			text,
	JRunArray<Font>*	style
	)
	const
{
	text->Clear();
	style->RemoveAll();

	JBoolean canGetStyledText, canGetText;
	Atom textType;
	if (GetAvailDataTypes(typeList, &canGetStyledText, &canGetText, &textType))
		{
		JXSelectionManager* selMgr = GetSelectionManager();

		JBoolean gotData = kJFalse;
		JError err       = JNoError();

		Atom returnType, textReturnType;
		unsigned char* data = NULL;
		JSize dataLength;
		JXSelectionManager::DeleteMethod delMethod;

		if (!gotData && canGetStyledText &&
			selMgr->GetData(selectionName, time, itsStyledText0XAtom,
							&returnType, &data, &dataLength, &delMethod))
			{
			if (returnType == itsStyledText0XAtom)
				{
				gotData = kJTrue;
				const std::string s(reinterpret_cast<char*>(data), dataLength);
				std::istringstream input(s);
				if (!ReadPrivateFormat(input, this, text, style))
					{
					err = DataNotCompatible();
					}
				}
			selMgr->DeleteData(&data, delMethod);
			}

		if (!gotData && canGetText &&
			selMgr->GetData(selectionName, time, textType,
							&textReturnType, &data, &dataLength, &delMethod))
			{
			if (textReturnType == XA_STRING ||
				textReturnType == selMgr->GetMimePlainTextXAtom())
				{
				gotData = kJTrue;
				*text = JString(reinterpret_cast<JCharacter*>(data), dataLength);
				}
			selMgr->DeleteData(&data, delMethod);
			}

		if (!gotData)
			{
			err = DataNotCompatible(canGetText ? textReturnType : None, GetDisplay());
			}

		return err;
		}
	else
		{
		return NoData();
		}
}

/******************************************************************************
 DataNotCompatible::DataNotCompatible

 ******************************************************************************/

JXTEBase::DataNotCompatible::DataNotCompatible
	(
	const Atom	type,
	JXDisplay*	display
	)
	:
	JError(kDataNotCompatible)
{
	if (type != None)
		{
		assert( display != NULL );

		const JCharacter* map[] =
			{
			"atom", XGetAtomName(*display, type)
			};
		JString msg = JGetString(kDataNotCompatible);
		msg += JGetString(kDataNotCompatibleExtra, map, sizeof(map));
		SetMessage(msg, kJTrue);
		}
}

/******************************************************************************
 AppendEditMenu

	Call this to let us create the Edit menu for text editing.

 ******************************************************************************/

#include <jx_edit_undo.xpm>
#include <jx_edit_redo.xpm>
#include <jx_edit_cut.xpm>
#include <jx_edit_copy.xpm>
#include <jx_edit_paste.xpm>
#include <jx_edit_clear.xpm>
#include <jx_edit_clean_right_margin.xpm>
#include <jx_edit_coerce_right_margin.xpm>
#include <jx_edit_shift_left.xpm>
#include <jx_edit_shift_right.xpm>
#include <jx_edit_force_shift_left.xpm>
#include <jx_edit_read_only.xpm>

JXTextMenu*
JXTEBase::AppendEditMenu
	(
	JXMenuBar*		menuBar,
	const JBoolean	showAdjustMarginsCmds,
	const JBoolean	allowAdjustMargins,
	const JBoolean	showToggleReadOnly,
	const JBoolean	allowToggleReadOnly
	)
{
	assert( itsEditMenu == NULL );

	// create basic menu

	itsEditMenu = menuBar->AppendTextMenu(kEditMenuTitleStr);
	if (JXMenu::GetDefaultStyle() == JXMenu::kMacintoshStyle)
		{
		itsEditMenu->SetMenuItems(kMacEditMenuStr, "JXTEBase");
		}
	else
		{
		itsEditMenu->SetShortcuts(kEditMenuShortcutStr);
		itsEditMenu->SetMenuItems(kWinEditMenuStr, "JXTEBase");
		}

	itsEditMenu->SetItemImage(kUndoIndex,  jx_edit_undo);
	itsEditMenu->SetItemImage(kRedoIndex,  jx_edit_redo);
	itsEditMenu->SetItemImage(kCutIndex,   jx_edit_cut);
	itsEditMenu->SetItemImage(kCopyIndex,  jx_edit_copy);
	itsEditMenu->SetItemImage(kPasteIndex, jx_edit_paste);
	itsEditMenu->SetItemImage(kClearIndex, jx_edit_clear);

	itsEditMenu->SetUpdateAction(JXMenu::kDisableAll);
	ListenTo(itsEditMenu);

	// append margin adjustment commands

	if (showAdjustMarginsCmds)
		{
		itsCanAdjustMarginsFlag = allowAdjustMargins;

		const JSize itemCount = itsEditMenu->GetItemCount();
		itsEditMenu->ShowSeparatorAfter(itemCount);

		if (JXMenu::GetDefaultStyle() == JXMenu::kMacintoshStyle)
			{
			itsEditMenu->AppendMenuItems(kMacAdjustMarginsMenuStr, "JXTEBase");
			}
		else
			{
			itsEditMenu->AppendMenuItems(kWinAdjustMarginsMenuStr, "JXTEBase");
			}

		itsEditMenu->SetItemImage(itemCount+1, jx_edit_clean_right_margin);
		itsEditMenu->SetItemImage(itemCount+2, jx_edit_coerce_right_margin);
		itsEditMenu->SetItemImage(itemCount+3, jx_edit_shift_left);
		itsEditMenu->SetItemImage(itemCount+4, jx_edit_shift_right);
		itsEditMenu->SetItemImage(itemCount+5, jx_edit_force_shift_left);
		}

	// append "read only" checkbox

	if (showToggleReadOnly)
		{
		itsCanToggleReadOnlyFlag = allowToggleReadOnly;

		const JSize itemCount = itsEditMenu->GetItemCount();
		itsEditMenu->ShowSeparatorAfter(itemCount);

		if (JXMenu::GetDefaultStyle() == JXMenu::kMacintoshStyle)
			{
			itsEditMenu->AppendMenuItems(kMacReadOnlyMenuStr, "JXTEBase");
			}
		else
			{
			itsEditMenu->AppendMenuItems(kWinReadOnlyMenuStr, "JXTEBase");
			}

		itsEditMenu->SetItemImage(itemCount+1, jx_edit_read_only);
		}

	return itsEditMenu;
}

/******************************************************************************
 ShareEditMenu

	Call this to let us share the Edit menu with other objects.

	The JXTextMenu passed to the second version must have ID's
	assigned to the standard items:
		Undo, Redo, Cut, Copy, Paste, Clear, Select All

 ******************************************************************************/

JXTextMenu*
JXTEBase::ShareEditMenu
	(
	JXTEBase*		te,
	const JBoolean	allowAdjustMargins,
	const JBoolean	allowToggleReadOnly
	)
{
	assert( itsEditMenu == NULL && te->itsEditMenu != NULL );

	itsEditMenu              = te->itsEditMenu;
	itsCanAdjustMarginsFlag  = allowAdjustMargins;
	itsCanToggleReadOnlyFlag = allowToggleReadOnly;

	ListenTo(itsEditMenu);
	return itsEditMenu;
}

void
JXTEBase::ShareEditMenu
	(
	JXTextMenu*		menu,
	const JBoolean	allowAdjustMargins,
	const JBoolean	allowToggleReadOnly
	)
{
	assert( itsEditMenu == NULL && menu != NULL );

	itsEditMenu              = menu;
	itsCanAdjustMarginsFlag  = allowAdjustMargins;
	itsCanToggleReadOnlyFlag = allowToggleReadOnly;

	ListenTo(itsEditMenu);
}

/******************************************************************************
 AppendSearchMenu

	Call this to let us create the Search menu for text editing.

	*** This cannot be used if a Search & Replace menu already exists.

 ******************************************************************************/

#include <jx_find.xpm>
#include <jx_find_previous.xpm>
#include <jx_find_next.xpm>
#include <jx_find_selection_backwards.xpm>
#include <jx_find_selection_forward.xpm>
#include <jx_replace_selection.xpm>

JXTextMenu*
JXTEBase::AppendSearchMenu
	(
	JXMenuBar* menuBar
	)
{
	assert( itsSearchMenu == NULL && itsReplaceMenu == NULL );

	itsSearchMenu = menuBar->AppendTextMenu(kSearchMenuTitleStr);
	if (JXMenu::GetDefaultStyle() == JXMenu::kMacintoshStyle)
		{
		itsSearchMenu->SetMenuItems(kMacSearchMenuStr, "JXTEBase");
		}
	else
		{
		itsSearchMenu->SetShortcuts(kSearchMenuShortcutStr);
		itsSearchMenu->SetMenuItems(kWinSearchMenuStr, "JXTEBase");
		}

	itsSearchMenu->SetItemImage(kSFindCmd,              jx_find);
	itsSearchMenu->SetItemImage(kSFindPrevCmd,          jx_find_previous);
	itsSearchMenu->SetItemImage(kSFindNextCmd,          jx_find_next);
	itsSearchMenu->SetItemImage(kSFindSelectionBackCmd, jx_find_selection_backwards);
	itsSearchMenu->SetItemImage(kSFindSelectionFwdCmd,  jx_find_selection_forward);

	itsSearchMenu->SetUpdateAction(JXMenu::kDisableAll);
	ListenTo(itsSearchMenu);

	return itsSearchMenu;
}

/******************************************************************************
 AppendSearchReplaceMenu

	Call this to let us create the Search & Replace menu for text editing.

	*** This cannot be used if a Search menu already exists.

 ******************************************************************************/

JXTextMenu*
JXTEBase::AppendSearchReplaceMenu
	(
	JXMenuBar* menuBar
	)
{
	assert( itsSearchMenu == NULL && itsReplaceMenu == NULL );

	itsReplaceMenu = menuBar->AppendTextMenu(kSearchMenuTitleStr);
	if (JXMenu::GetDefaultStyle() == JXMenu::kMacintoshStyle)
		{
		itsReplaceMenu->SetMenuItems(kMacReplaceMenuStr, "JXTEBase");
		}
	else
		{
		itsReplaceMenu->SetShortcuts(kSearchMenuShortcutStr);
		itsReplaceMenu->SetMenuItems(kWinReplaceMenuStr, "JXTEBase");
		}

	itsReplaceMenu->SetItemImage(kSRFindCmd,              jx_find);
	itsReplaceMenu->SetItemImage(kSRFindPrevCmd,          jx_find_previous);
	itsReplaceMenu->SetItemImage(kSRFindNextCmd,          jx_find_next);
	itsReplaceMenu->SetItemImage(kSRFindSelectionBackCmd, jx_find_selection_backwards);
	itsReplaceMenu->SetItemImage(kSRFindSelectionFwdCmd,  jx_find_selection_forward);
	itsReplaceMenu->SetItemImage(kSRReplaceCmd,           jx_replace_selection);

	itsReplaceMenu->SetUpdateAction(JXMenu::kDisableAll);
	ListenTo(itsReplaceMenu);

	return itsReplaceMenu;
}

/******************************************************************************
 ShareSearchMenu

	Call this to let us share the Search or Search & Replace menu
	with other objects.

	The JXTextMenu passed to the versions taking JXTextMenu* must have ID's
	assigned to the standard items.

 ******************************************************************************/

JXTextMenu*
JXTEBase::ShareSearchMenu
	(
	JXTEBase* te
	)
{
	assert( itsSearchMenu == NULL && itsReplaceMenu == NULL &&
			(te->itsSearchMenu != NULL || te->itsReplaceMenu != NULL) );

	itsSearchMenu  = te->itsSearchMenu;
	itsReplaceMenu = te->itsReplaceMenu;

	if (itsSearchMenu != NULL)
		{
		ListenTo(itsSearchMenu);
		return itsSearchMenu;
		}
	else if (itsReplaceMenu != NULL)
		{
		ListenTo(itsReplaceMenu);
		return itsReplaceMenu;
		}
	else
		{
		assert( 0 /* should never happen */ );
		return NULL;
		}
}

void
JXTEBase::ShareSearchMenu
	(
	JXTextMenu* menu
	)
{
	assert( itsSearchMenu == NULL && itsReplaceMenu == NULL && menu != NULL );

	itsSearchMenu = menu;
	ListenTo(itsSearchMenu);
}

void
JXTEBase::ShareSearchReplaceMenu
	(
	JXTextMenu* menu
	)
{
	assert( itsSearchMenu == NULL && itsReplaceMenu == NULL && menu != NULL );

	itsReplaceMenu = menu;
	ListenTo(itsReplaceMenu);
}

/******************************************************************************
 Receive (virtual protected)

	Listen for menu update requests and menu selections.

 ******************************************************************************/

void
JXTEBase::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsEditMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		if (HasFocus())
			{
			UpdateEditMenu();
			}
		}
	else if (sender == itsEditMenu && message.Is(JXMenu::kItemSelected))
		{
		if (HasFocus())
			{
			const JXMenu::ItemSelected* selection =
				dynamic_cast(const JXMenu::ItemSelected*, &message);
			assert( selection != NULL );
			HandleEditMenu(selection->GetIndex());
			}
		}

	else if (sender == itsSearchMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		if (HasFocus())
			{
			UpdateSearchMenu();
			}
		}
	else if (sender == itsSearchMenu && message.Is(JXMenu::kItemSelected))
		{
		if (HasFocus())
			{
			const JXMenu::ItemSelected* selection =
				dynamic_cast(const JXMenu::ItemSelected*, &message);
			assert( selection != NULL );
			HandleSearchMenu(selection->GetIndex());
			}
		}

	else if (sender == itsReplaceMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		if (HasFocus())
			{
			UpdateReplaceMenu();
			}
		}
	else if (sender == itsReplaceMenu && message.Is(JXMenu::kItemSelected))
		{
		if (HasFocus())
			{
			const JXMenu::ItemSelected* selection =
				dynamic_cast(const JXMenu::ItemSelected*, &message);
			assert( selection != NULL );
			HandleReplaceMenu(selection->GetIndex());
			}
		}

	else if (sender == itsPSPrinter &&
			 message.Is(JPrinter::kPrintSetupFinished))
		{
		const JPrinter::PrintSetupFinished* info =
			dynamic_cast(const JPrinter::PrintSetupFinished*, &message);
		assert( info != NULL );
		if (info->Successful())
			{
			SetPSPrintFileName(itsPSPrinter->GetFileName());
			Print(*itsPSPrinter);
			}
		StopListening(itsPSPrinter);
		}

	else if (sender == itsPTPrinter &&
			 message.Is(JPrinter::kPrintSetupFinished))
		{
		const JPrinter::PrintSetupFinished* info =
			dynamic_cast(const JPrinter::PrintSetupFinished*, &message);
		assert( info != NULL );
		if (info->Successful())
			{
			SetPTPrintFileName(itsPTPrinter->GetFileName());
			itsPTPrinter->Print(GetText());
			}
		StopListening(itsPTPrinter);
		}

	else if (sender == itsGoToLineDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast(const JXDialogDirector::Deactivated*, &message);
		assert( info != NULL );
		if (info->Successful())
			{
			GoToLine(itsGoToLineDialog->GetLineIndex());
			}
		itsGoToLineDialog = NULL;
		}

	else
		{
		JXScrollableWidget::Receive(sender, message);
		JTextEditor::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateEditMenu (private)

 ******************************************************************************/

void
JXTEBase::UpdateEditMenu()
{
	assert( itsEditMenu != NULL );

	JString crmActionText, crm2ActionText;
	JBoolean isReadOnly;
	const JArray<JBoolean> enableFlags =
		GetCmdStatus(&crmActionText, &crm2ActionText, &isReadOnly);

	const JSize count = itsEditMenu->GetItemCount();
	for (JIndex i=1; i<=count; i++)
		{
		CmdIndex cmd;
		if (EditMenuIndexToCmd(i, &cmd))
			{
			JBoolean enable = kJTrue;
			if (cmd == kCleanRightMarginCmd)
				{
				itsEditMenu->SetItemText(i, crmActionText);
				enable = itsCanAdjustMarginsFlag;
				}
			else if (cmd == kCoerceRightMarginCmd)
				{
				itsEditMenu->SetItemText(i, crm2ActionText);
				enable = itsCanAdjustMarginsFlag;
				}
			else if (cmd == kShiftSelLeftCmd  ||
					 cmd == kShiftSelRightCmd ||
					 cmd == kForceShiftSelLeftCmd)
				{
				enable = itsCanAdjustMarginsFlag;
				}
			else if (cmd == kToggleReadOnlyCmd)
				{
				if (isReadOnly)
					{
					itsEditMenu->CheckItem(i);
					}
				enable = itsCanToggleReadOnlyFlag;
				}

			itsEditMenu->SetItemEnable(i, JI2B(enable && enableFlags.GetElement(cmd)));
			}
		}
}

/******************************************************************************
 HandleEditMenu (private)

 ******************************************************************************/

void
JXTEBase::HandleEditMenu
	(
	const JIndex index
	)
{
	assert( itsEditMenu != NULL );

	CmdIndex cmd;
	if (!EditMenuIndexToCmd(index, &cmd))
		{
		return;
		}

	if (itsSearchMenu != NULL || itsReplaceMenu != NULL)
		{
		(JXGetSearchTextDialog())->SetActiveTE(this);
		}

	if (cmd == kUndoCmd)
		{
		Undo();
		}
	else if (cmd == kRedoCmd)
		{
		Redo();
		}

	else if (cmd == kCutCmd)
		{
		Cut();
		}
	else if (cmd == kCopyCmd)
		{
		Copy();
		}
	else if (cmd == kPasteCmd)
		{
		Paste();
		}
	else if (cmd == kDeleteSelCmd)
		{
		DeleteSelection();
		}

	else if (cmd == kSelectAllCmd)
		{
		SelectAll();
		}

	else if (cmd == kCleanRightMarginCmd)
		{
		JIndexRange range;
		CleanRightMargin(kJFalse, &range);
		}
	else if (cmd == kCoerceRightMarginCmd)
		{
		JIndexRange range;
		CleanRightMargin(kJTrue, &range);
		}
	else if (cmd == kShiftSelLeftCmd)
		{
		TabSelectionLeft();
		}
	else if (cmd == kShiftSelRightCmd)
		{
		TabSelectionRight();
		}
	else if (cmd == kForceShiftSelLeftCmd)
		{
		TabSelectionLeft(1, kJTrue);
		}

	else if (cmd == kToggleReadOnlyCmd)
		{
		const Type type = GetType();
		if (type == kFullEditor)
			{
			SetType(kSelectableText);
			}
		else if (type == kSelectableText)
			{
			SetType(kFullEditor);
			}
		// don't change kStaticText
		}
}

/******************************************************************************
 Edit menu index <-> cmd

 ******************************************************************************/

#define ClassName    JXTEBase
#define IndexToCmdFn EditMenuIndexToCmd
#define CmdToIndexFn EditMenuCmdToIndex
#define MenuVar      itsEditMenu
#define CmdCount     kEditMenuItemCount
#define CmdIDList    kEditMenuItemInfo
#include <JXMenuItemIDUtil.th>
#undef ClassName
#undef IndexToCmdFn
#undef CmdToIndexFn
#undef MenuVar
#undef CmdCount
#undef CmdIDList

/******************************************************************************
 UpdateSearchMenu (private)

 ******************************************************************************/

void
JXTEBase::UpdateSearchMenu()
{
	assert( itsSearchMenu != NULL );

	JString crmActionText, crm2ActionText;
	JBoolean isReadOnly;
	JArray<JBoolean> enableFlags =
		GetCmdStatus(&crmActionText, &crm2ActionText, &isReadOnly);

	if (!IsEmpty())
		{
		enableFlags.SetElement(kFindSelectionBackwardCmd, kJTrue);
		enableFlags.SetElement(kFindSelectionForwardCmd,  kJTrue);
		}

	const JSize count = itsSearchMenu->GetItemCount();
	for (JIndex i=1; i<=count; i++)
		{
		CmdIndex cmd;
		if (SearchMenuIndexToCmd(i, &cmd))
			{
			itsSearchMenu->SetItemEnable(i, enableFlags.GetElement(cmd));
			}
		}
}

/******************************************************************************
 HandleSearchMenu (private)

 ******************************************************************************/

void
JXTEBase::HandleSearchMenu
	(
	const JIndex index
	)
{
	assert( itsSearchMenu != NULL );

	CmdIndex cmd;
	if (SearchMenuIndexToCmd(index, &cmd))
		{
		HandleSearchReplaceCmd(cmd);
		}
}

/******************************************************************************
 Search menu index <-> cmd

 ******************************************************************************/

#define ClassName    JXTEBase
#define IndexToCmdFn SearchMenuIndexToCmd
#define CmdToIndexFn SearchMenuCmdToIndex
#define MenuVar      itsSearchMenu
#define CmdCount     kSearchMenuItemCount
#define CmdIDList    kSearchMenuItemInfo
#include <JXMenuItemIDUtil.th>
#undef ClassName
#undef IndexToCmdFn
#undef CmdToIndexFn
#undef MenuVar
#undef CmdCount
#undef CmdIDList

/******************************************************************************
 UpdateReplaceMenu (private)

 ******************************************************************************/

void
JXTEBase::UpdateReplaceMenu()
{
	assert( itsReplaceMenu != NULL );

	JString crmActionText, crm2ActionText;
	JBoolean isReadOnly;
	JArray<JBoolean> enableFlags =
		GetCmdStatus(&crmActionText, &crm2ActionText, &isReadOnly);

	if (!IsEmpty())
		{
		enableFlags.SetElement(kFindSelectionBackwardCmd, kJTrue);
		enableFlags.SetElement(kFindSelectionForwardCmd,  kJTrue);
		}

	const JSize count = itsReplaceMenu->GetItemCount();
	for (JIndex i=1; i<=count; i++)
		{
		CmdIndex cmd;
		if (SearchReplaceMenuIndexToCmd(i, &cmd))
			{
			itsReplaceMenu->SetItemEnable(i, enableFlags.GetElement(cmd));
			}
		}
}

/******************************************************************************
 HandleReplaceMenu (private)

 ******************************************************************************/

void
JXTEBase::HandleReplaceMenu
	(
	const JIndex index
	)
{
	assert( itsReplaceMenu != NULL );

	CmdIndex cmd;
	if (SearchReplaceMenuIndexToCmd(index, &cmd))
		{
		HandleSearchReplaceCmd(cmd);
		}
}

/******************************************************************************
 HandleSearchReplaceCmd (private)

 ******************************************************************************/

void
JXTEBase::HandleSearchReplaceCmd
	(
	const CmdIndex cmd
	)
{
	(JXGetSearchTextDialog())->SetActiveTE(this);

	if (cmd == kFindDialogCmd)
		{
		(JXGetSearchTextDialog())->Activate();
		}
	else if (cmd == kFindPreviousCmd)
		{
		SearchBackward();
		}
	else if (cmd == kFindNextCmd)
		{
		SearchForward();
		}
	else if (cmd == kEnterSearchTextCmd)
		{
		EnterSearchSelection();
		}
	else if (cmd == kEnterReplaceTextCmd)
		{
		EnterReplaceSelection();
		}
	else if (cmd == kFindSelectionBackwardCmd)
		{
		if (!HasSelection())
			{
			JIndexRange r;
			TEGetDoubleClickSelection(GetInsertionIndex(), kJFalse, kJFalse, &r);
			SetSelection(r);
			}
		SearchSelectionBackward();
		}
	else if (cmd == kFindSelectionForwardCmd)
		{
		if (!HasSelection())
			{
			JIndexRange r;
			TEGetDoubleClickSelection(GetInsertionIndex(), kJFalse, kJFalse, &r);
			SetSelection(r);
			}
		SearchSelectionForward();
		}
	else if (cmd == kFindClipboardBackwardCmd)
		{
		SearchClipboardBackward();
		}
	else if (cmd == kFindClipboardForwardCmd)
		{
		SearchClipboardForward();
		}

	else if (cmd == kReplaceSelectionCmd)
		{
		ReplaceSelection();
		}
	else if (cmd == kReplaceFindPrevCmd)
		{
		ReplaceAndSearchBackward();
		}
	else if (cmd == kReplaceFindNextCmd)
		{
		ReplaceAndSearchForward();
		}
	else if (cmd == kReplaceAllBackwardCmd)
		{
		ReplaceAllBackward();
		}
	else if (cmd == kReplaceAllForwardCmd)
		{
		ReplaceAllForward();
		}
	else if (cmd == kReplaceAllInSelectionCmd)
		{
		ReplaceAllInSelection();
		}
}

/******************************************************************************
 Search menu index <-> cmd

 ******************************************************************************/

#define ClassName    JXTEBase
#define IndexToCmdFn SearchReplaceMenuIndexToCmd
#define CmdToIndexFn SearchReplaceMenuCmdToIndex
#define MenuVar      itsReplaceMenu
#define CmdCount     kReplaceMenuItemCount
#define CmdIDList    kReplaceMenuItemInfo
#include <JXMenuItemIDUtil.th>
#undef ClassName
#undef IndexToCmdFn
#undef CmdToIndexFn
#undef MenuVar
#undef CmdCount
#undef CmdIDList

/******************************************************************************
 TEHasSearchText (virtual)

	Call this to provide a JXPSPrinter object for this text.  This object
	does *not* take ownership of the printer object.

 ******************************************************************************/

JBoolean
JXTEBase::TEHasSearchText()
	const
{
	return (JXGetSearchTextDialog())->HasSearchText();
}

/******************************************************************************
 SearchForward

	Get the search parameters from the search dialog and look for the
	next match beyond the current position.  We beep if we can't find a match.

 ******************************************************************************/

JBoolean
JXTEBase::SearchForward
	(
	const JBoolean reportNotFound
	)
{
	JString searchStr, replaceStr;
	JBoolean searchIsRegex, caseSensitive, entireWord, wrapSearch;
	JBoolean replaceIsRegex, preserveCase;
	JRegex* regex;
	if (!(JXGetSearchTextDialog())->GetSearchParameters(
			&searchStr, &searchIsRegex, &caseSensitive, &entireWord, &wrapSearch,
			&replaceStr, &replaceIsRegex, &preserveCase,
			&regex))
		{
		return kJFalse;
		}

	if (GetType() != kFullEditor && !HasSelection())	// caret not visible
		{
		SetCaretLocation(1);
		}

	JBoolean found, wrapped;
	if (searchIsRegex)
		{
		(JXGetApplication())->DisplayBusyCursor();

		JArray<JIndexRange> submatchList;
		found = JTextEditor::SearchForward(*regex, entireWord,
										   wrapSearch, &wrapped, &submatchList);
		}
	else
		{
		found = JTextEditor::SearchForward(searchStr, caseSensitive, entireWord,
										   wrapSearch, &wrapped);
		}

	if (found)
		{
		TEScrollToSelection(kJTrue);
		}

	if ((!found && reportNotFound) || wrapped)
		{
		(GetDisplay())->Beep();
		}

	return found;
}

/******************************************************************************
 SearchBackward

	Get the search parameters from the search dialog and look for the
	match before the current position.  We beep if we can't find a match.

 ******************************************************************************/

JBoolean
JXTEBase::SearchBackward()
{
	JString searchStr, replaceStr;
	JBoolean searchIsRegex, caseSensitive, entireWord, wrapSearch;
	JBoolean replaceIsRegex, preserveCase;
	JRegex* regex;
	if (!(JXGetSearchTextDialog())->GetSearchParameters(
			&searchStr, &searchIsRegex, &caseSensitive, &entireWord, &wrapSearch,
			&replaceStr, &replaceIsRegex, &preserveCase,
			&regex))
		{
		return kJFalse;
		}

	if (GetType() != kFullEditor && !HasSelection())	// caret not visible
		{
		SetCaretLocation(GetTextLength()+1);
		}

	JBoolean found, wrapped;
	if (searchIsRegex)
		{
		(JXGetApplication())->DisplayBusyCursor();

		JArray<JIndexRange> submatchList;
		found = JTextEditor::SearchBackward(*regex, entireWord,
											wrapSearch, &wrapped, &submatchList);
		}
	else
		{
		found = JTextEditor::SearchBackward(searchStr, caseSensitive, entireWord,
											wrapSearch, &wrapped);
		}

	if (found)
		{
		TEScrollToSelection(kJTrue);
		}

	if (!found || wrapped)
		{
		(GetDisplay())->Beep();
		}

	return found;
}

/******************************************************************************
 Search for clipboard

 ******************************************************************************/

JBoolean
JXTEBase::SearchClipboardForward()
{
	JString text;
	JRunArray<Font> style;
	if (TEGetExternalClipboard(&text, &style))
		{
		(JXGetSearchTextDialog())->SetSearchText(text);
		return SearchForward();
		}
	else
		{
		return kJFalse;
		}
}

JBoolean
JXTEBase::SearchClipboardBackward()
{
	JString text;
	JRunArray<Font> style;
	if (TEGetExternalClipboard(&text, &style))
		{
		(JXGetSearchTextDialog())->SetSearchText(text);
		return SearchBackward();
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 EnterSearchSelection

	Copy the selection into the search field of the SearchTextDialog.
	Returns kJTrue if there was something selected.

 ******************************************************************************/

JBoolean
JXTEBase::EnterSearchSelection()
{
	JString selText;
	if (GetSelection(&selText))
		{
		(JXGetSearchTextDialog())->SetSearchText(selText);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 EnterReplaceSelection

	Copy the selection into the replace field of the SearchTextDialog.
	Returns kJTrue if there was something selected.

 ******************************************************************************/

JBoolean
JXTEBase::EnterReplaceSelection()
{
	JString selText;
	if (GetSelection(&selText))
		{
		(JXGetSearchTextDialog())->SetReplaceText(selText);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 ReplaceSelection

	Replace the current selection with the replace string from the
	SearchTextDialog.  Returns kJFalse if nothing is selected or if the
	current selection doesn't match the search string.

 ******************************************************************************/

JBoolean
JXTEBase::ReplaceSelection()
{
	JString searchStr, replaceStr;
	JBoolean searchIsRegex, caseSensitive, entireWord, wrapSearch;
	JBoolean replaceIsRegex, preserveCase;
	JRegex* regex;
	if (GetType() != kFullEditor ||
		!(JXGetSearchTextDialog())->GetSearchParameters(
			&searchStr, &searchIsRegex, &caseSensitive, &entireWord, &wrapSearch,
			&replaceStr, &replaceIsRegex, &preserveCase,
			&regex))
		{
		return kJFalse;
		}

	JArray<JIndexRange> submatchList;
	if (searchIsRegex &&
		SelectionMatches(*regex, entireWord, &submatchList))
		{
		JTextEditor::ReplaceSelection(replaceStr, preserveCase,
									  replaceIsRegex, *regex, submatchList);
		return kJTrue;
		}
	else if (!searchIsRegex &&
			 SelectionMatches(searchStr, caseSensitive, entireWord))
		{
		submatchList.AppendElement(JIndexRange(1, searchStr.GetLength()));
		JTextEditor::ReplaceSelection(replaceStr, preserveCase,
									  replaceIsRegex, *regex, submatchList);
		return kJTrue;
		}

	(GetDisplay())->Beep();
	return kJFalse;
}

/******************************************************************************
 ReplaceAllForward

	Replace every occurrence of the search string with the replace string,
	starting from the current location.  Returns kJTrue if it replaced anything.

 ******************************************************************************/

JBoolean
JXTEBase::ReplaceAllForward()
{
	JString searchStr, replaceStr;
	JBoolean searchIsRegex, caseSensitive, entireWord, wrapSearch;
	JBoolean replaceIsRegex, preserveCase;
	JRegex* regex;
	if (GetType() == kFullEditor &&
		(JXGetSearchTextDialog())->GetSearchParameters(
			&searchStr, &searchIsRegex, &caseSensitive, &entireWord, &wrapSearch,
			&replaceStr, &replaceIsRegex, &preserveCase,
			&regex))
		{
		(JXGetApplication())->DisplayBusyCursor();

		return JTextEditor::ReplaceAllForward(
					searchStr, searchIsRegex, caseSensitive, entireWord, wrapSearch,
					replaceStr, replaceIsRegex, preserveCase, *regex);
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 ReplaceAllBackward

	Replace every occurrence of the search string with the replace string,
	starting from the current location.  Returns kJTrue if it replaced anything.

 ******************************************************************************/

JBoolean
JXTEBase::ReplaceAllBackward()
{
	JString searchStr, replaceStr;
	JBoolean searchIsRegex, caseSensitive, entireWord, wrapSearch;
	JBoolean replaceIsRegex, preserveCase;
	JRegex* regex;
	if (GetType() == kFullEditor &&
		(JXGetSearchTextDialog())->GetSearchParameters(
			&searchStr, &searchIsRegex, &caseSensitive, &entireWord, &wrapSearch,
			&replaceStr, &replaceIsRegex, &preserveCase,
			&regex))
		{
		(JXGetApplication())->DisplayBusyCursor();

		return JTextEditor::ReplaceAllBackward(
					searchStr, searchIsRegex, caseSensitive, entireWord, wrapSearch,
					replaceStr, replaceIsRegex, preserveCase, *regex);
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 ReplaceAllInSelection

	Replace every occurrence of the search string with the replace string,
	restricting the search to the current selection.  Returns kJTrue if it
	replaced anything.

 ******************************************************************************/

JBoolean
JXTEBase::ReplaceAllInSelection()
{
	JString searchStr, replaceStr;
	JBoolean searchIsRegex, caseSensitive, entireWord, wrapSearch;
	JBoolean replaceIsRegex, preserveCase;
	JRegex* regex;
	JIndexRange searchRange;
	if (GetType() == kFullEditor &&
		GetSelection(&searchRange) &&
		(JXGetSearchTextDialog())->GetSearchParameters(
			&searchStr, &searchIsRegex, &caseSensitive, &entireWord, &wrapSearch,
			&replaceStr, &replaceIsRegex, &preserveCase,
			&regex))
		{
		(JXGetApplication())->DisplayBusyCursor();

		return JTextEditor::ReplaceAllForward(
					searchStr, searchIsRegex, caseSensitive, entireWord, wrapSearch,
					replaceStr, replaceIsRegex, preserveCase, *regex,
					searchRange);
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 SetPSPrinter

	Call this to provide a JXPSPrinter object for this text.  This object
	does *not* take ownership of the printer object.

 ******************************************************************************/

void
JXTEBase::SetPSPrinter
	(
	JXPSPrinter* p
	)
{
	if (itsPSPrinter != NULL)
		{
		StopListening(itsPSPrinter);
		}
	itsPSPrinter = p;
}

/******************************************************************************
 GetPSPrintFileName

	Call this to get the file name used in the Print Setup dialog.

 ******************************************************************************/

const JString&
JXTEBase::GetPSPrintFileName()
	const
{
	if (itsPSPrintName == NULL)
		{
		JXTEBase* me = const_cast<JXTEBase*>(this);
		me->itsPSPrintName = new JString;
		assert( itsPSPrintName != NULL );
		}

	return *itsPSPrintName;
}

/******************************************************************************
 SetPSPrintFileName

	Call this to set the file name used in the Print Setup dialog.

 ******************************************************************************/

void
JXTEBase::SetPSPrintFileName
	(
	const JCharacter* fileName
	)
{
	GetPSPrintFileName();		// create itsPSPrintName
	*itsPSPrintName = fileName;
}

/******************************************************************************
 HandlePSPageSetup

	You must call SetPSPrinter() before using this routine.

 ******************************************************************************/

void
JXTEBase::HandlePSPageSetup()
{
	assert( itsPSPrinter != NULL );

	itsPSPrinter->BeginUserPageSetup();
}

/******************************************************************************
 PrintPS

	You must call SetPSPrinter() before using this routine.

 ******************************************************************************/

void
JXTEBase::PrintPS()
{
	assert( itsPSPrinter != NULL );

	itsPSPrinter->SetFileName(GetPSPrintFileName());
	itsPSPrinter->BeginUserPrintSetup();
	ListenTo(itsPSPrinter);
}

/******************************************************************************
 Print footer (virtual protected)

	Overrides of JTextEditor functions.

 ******************************************************************************/

JCoordinate
JXTEBase::GetPrintFooterHeight
	(
	JPagePrinter& p
	)
	const
{
	return JRound(1.5 * p.GetLineHeight());
}

void
JXTEBase::DrawPrintFooter
	(
	JPagePrinter&		p,
	const JCoordinate	footerHeight
	)
{
	JRect pageRect = p.GetPageRect();
	pageRect.top   = pageRect.bottom - footerHeight;

	JString pageNumberStr(p.GetPageIndex(), 0);
	pageNumberStr.Prepend("Page ");

	p.String(pageRect, pageNumberStr,
			 JPainter::kHAlignCenter, JPainter::kVAlignBottom);
}

/******************************************************************************
 SetPTPrinter

	Call this to provide a JXPTPrinter object for this text.  This object
	does *not* take ownership of the printer object.

 ******************************************************************************/

void
JXTEBase::SetPTPrinter
	(
	JXPTPrinter* p
	)
{
	if (itsPTPrinter != NULL)
		{
		StopListening(itsPTPrinter);
		}
	itsPTPrinter = p;
}

/******************************************************************************
 GetPTPrintFileName

	Call this to get the file name used in the Print Setup dialog.

 ******************************************************************************/

const JString&
JXTEBase::GetPTPrintFileName()
	const
{
	if (itsPTPrintName == NULL)
		{
		JXTEBase* me = const_cast<JXTEBase*>(this);
		me->itsPTPrintName = new JString;
		assert( itsPTPrintName != NULL );
		}

	return *itsPTPrintName;
}

/******************************************************************************
 SetPTPrintFileName

	Call this to set the file name used in the Print Setup dialog.

 ******************************************************************************/

void
JXTEBase::SetPTPrintFileName
	(
	const JCharacter* fileName
	)
{
	GetPTPrintFileName();		// create itsPTPrintName
	*itsPTPrintName = fileName;
}

/******************************************************************************
 HandlePTPageSetup

	You must call SetPTPrinter() before using this routine.

 ******************************************************************************/

void
JXTEBase::HandlePTPageSetup()
{
	assert( itsPTPrinter != NULL );

	itsPTPrinter->BeginUserPageSetup();
}

/******************************************************************************
 PrintPT

	You must call SetPTPrinter() before using this routine.

 ******************************************************************************/

void
JXTEBase::PrintPT()
{
	assert( itsPTPrinter != NULL );

	itsPTPrinter->SetFileName(GetPTPrintFileName());
	itsPTPrinter->BeginUserPrintSetup();
	ListenTo(itsPTPrinter);
}

/******************************************************************************
 AskForLine

	Opens dialog window to ask user which line to go to.

 ******************************************************************************/

void
JXTEBase::AskForLine()
{
	assert( itsGoToLineDialog == NULL );

	const JIndex charIndex = GetInsertionIndex();
	const JIndex lineIndex = GetLineForChar(charIndex);
	const JSize lineCount  = GetLineCount();

	JXDirector* sup = (GetWindow())->GetDirector();
	itsGoToLineDialog = new JXGoToLineDialog(sup, lineIndex, lineCount);
	assert( itsGoToLineDialog != NULL );
	itsGoToLineDialog->BeginDialog();
	ListenTo(itsGoToLineDialog);
}

/******************************************************************************
 JXTEBase::PartialWordModifier

 ******************************************************************************/

istream&
operator>>
	(
	istream&						input,
	JXTEBase::PartialWordModifier&	pwMod
	)
{
	long temp;
	input >> temp;
	pwMod = (JXTEBase::PartialWordModifier) temp;
	assert( pwMod == JXTEBase::kCtrlMetaPWMod ||
			pwMod == JXTEBase::kMod2PWMod || pwMod == JXTEBase::kMod3PWMod ||
			pwMod == JXTEBase::kMod4PWMod || pwMod == JXTEBase::kMod5PWMod );
	return input;
}

ostream&
operator<<
	(
	ostream&							output,
	const JXTEBase::PartialWordModifier	pwMod
	)
{
	output << (long) pwMod;
	return output;
}
