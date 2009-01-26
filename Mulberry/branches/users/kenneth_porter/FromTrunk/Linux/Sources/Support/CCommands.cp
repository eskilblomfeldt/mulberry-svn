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

#ifndef __CCOMMANDS_H
#define __CCOMMANDS_H

#include "CCommands.h"

#include <jXActionDefs.h>

#define DECLARE_CMD(name)	const char* CCommand::c##name = #name "Menu";

DECLARE_CMD(FileNewMessage)
DECLARE_CMD(FileOpenMailbox)
DECLARE_CMD(FileOpenMessage)
DECLARE_CMD(FileOpenDraft)
DECLARE_CMD(FileImport)
DECLARE_CMD(FileExport)
DECLARE_CMD(FileClose)
DECLARE_CMD(FileSave)
DECLARE_CMD(FileSaveAs)
DECLARE_CMD(FileRevert)
DECLARE_CMD(FilePreferences)
DECLARE_CMD(FileDisconnect)
DECLARE_CMD(FileShowOutgoingQueues)
DECLARE_CMD(FilePageSetup)
DECLARE_CMD(FilePrint)
DECLARE_CMD(FileExit)
	
DECLARE_CMD(EditUndo)
DECLARE_CMD(EditRedo)
DECLARE_CMD(EditCut)
DECLARE_CMD(EditCopy)
DECLARE_CMD(EditPaste)
DECLARE_CMD(EditPasteAsQuote)
DECLARE_CMD(EditDelete)
DECLARE_CMD(EditSelectAll)
DECLARE_CMD(EditFind)
DECLARE_CMD(EditFindNext)
DECLARE_CMD(EditFindSel)
DECLARE_CMD(EditReplace)
DECLARE_CMD(EditReplaceFindNext)
DECLARE_CMD(EditReplaceAll)
DECLARE_CMD(EditTextMacros)
DECLARE_CMD(EditProperties)
DECLARE_CMD(EditChangePassword)
DECLARE_CMD(EditCheckSpelling)

DECLARE_CMD(EditOpenLink)
DECLARE_CMD(EditCopyLink)

DECLARE_CMD(MailboxCreate)
DECLARE_CMD(MailboxRename)
DECLARE_CMD(MailboxMarkContentsSeen)
DECLARE_CMD(MailboxDelete)
DECLARE_CMD(MailboxExpunge)
DECLARE_CMD(MailboxSynchronise)
DECLARE_CMD(MailboxClearDisconnected)
DECLARE_CMD(MailboxDisplayHierarchy)
	DECLARE_CMD(DispHNew)
	DECLARE_CMD(DispHEdit)
	DECLARE_CMD(DispHRemove)
	DECLARE_CMD(DispHReset)
	DECLARE_CMD(DispHRefresh)
DECLARE_CMD(MailboxCabinets)
	DECLARE_CMD(CabNew)
	DECLARE_CMD(CabEdit)
	DECLARE_CMD(CabRemove)
	DECLARE_CMD(CabAddWild)
	DECLARE_CMD(CabEditWild)
	DECLARE_CMD(CabRemoveItem)
DECLARE_CMD(MailboxSubscribe)
DECLARE_CMD(MailboxUnsubscribe)
DECLARE_CMD(MailboxCheckMail)
DECLARE_CMD(MailboxCheckFavourites)
DECLARE_CMD(MailboxSortBy)
	DECLARE_CMD(SortTo)
	DECLARE_CMD(SortFrom)
	DECLARE_CMD(SortReply)
	DECLARE_CMD(SortSender)
	DECLARE_CMD(SortCC)
	DECLARE_CMD(SortSubject)
	DECLARE_CMD(SortThread)
	DECLARE_CMD(SortDateSent)
	DECLARE_CMD(SortDateReceived)
	DECLARE_CMD(SortSize)
	DECLARE_CMD(SortFlags)
	DECLARE_CMD(SortNumber)
	DECLARE_CMD(SortSmartAddress)
	DECLARE_CMD(SortAttachments)
	DECLARE_CMD(SortParts)
	DECLARE_CMD(SortMatching)
	DECLARE_CMD(SortDisconnected)
DECLARE_CMD(MailboxGotoMessage)
DECLARE_CMD(MailboxLoadAllMessages)
DECLARE_CMD(MailboxUpdateMatch)
DECLARE_CMD(MailboxSearch)
DECLARE_CMD(MailboxSearchAgain)
DECLARE_CMD(MailboxNextSearch)
DECLARE_CMD(MailboxRules)
DECLARE_CMD(MailboxApplyRules)
DECLARE_CMD(MailboxMakeRule)

DECLARE_CMD(MessagesReadPrev)
DECLARE_CMD(MessagesReadNext)
DECLARE_CMD(MessagesDelAndReadNext)
DECLARE_CMD(MessagesCopyAndReadNext)
DECLARE_CMD(MessagesReply)
DECLARE_CMD(MessagesReplyToSender)
DECLARE_CMD(MessagesReplyToFrom)
DECLARE_CMD(MessagesReplyToAll)
DECLARE_CMD(MessagesForward)
DECLARE_CMD(MessagesBounce)
DECLARE_CMD(MessagesReject)
DECLARE_CMD(MessagesSendAgain)
DECLARE_CMD(MessagesCreateDigest)
DECLARE_CMD(MessagesCopyTo)
DECLARE_CMD(MessagesCopyNow)
DECLARE_CMD(MessagesFlags)
	DECLARE_CMD(FlagsSeen)
	DECLARE_CMD(FlagsImportant)
	DECLARE_CMD(FlagsAnswered)
	DECLARE_CMD(FlagsDeleted)
	DECLARE_CMD(FlagsDraft)
	DECLARE_CMD(FlagsLabel1)
	DECLARE_CMD(FlagsLabel2)
	DECLARE_CMD(FlagsLabel3)
	DECLARE_CMD(FlagsLabel4)
	DECLARE_CMD(FlagsLabel5)
	DECLARE_CMD(FlagsLabel6)
	DECLARE_CMD(FlagsLabel7)
	DECLARE_CMD(FlagsLabel8)
DECLARE_CMD(MessagesViewParts)
DECLARE_CMD(MessagesExtractParts)
DECLARE_CMD(MessagesDecodeEmbedded)
	DECLARE_CMD(DecodeBinHex)
	DECLARE_CMD(DecodeUU)
DECLARE_CMD(MessagesVerifyDecrypt)
DECLARE_CMD(MessagesDelete)
DECLARE_CMD(MessagesShowHeader)

DECLARE_CMD(DraftIncludeOrig)
DECLARE_CMD(DraftInsertSig)
DECLARE_CMD(DraftAddCc)
DECLARE_CMD(DraftAddBcc)
DECLARE_CMD(DraftAttachFile)
DECLARE_CMD(DraftNewTextPart)
	DECLARE_CMD(NewTextPlain)
	DECLARE_CMD(NewTextEnriched)
	DECLARE_CMD(NewTextHTML)
DECLARE_CMD(DraftNewMultipart)
	DECLARE_CMD(NewMultiMixed)
	DECLARE_CMD(NewMultiParallel)
	DECLARE_CMD(NewMultiDigest)
	DECLARE_CMD(NewMultiAlt)
DECLARE_CMD(DraftCurrentTextPart)
	DECLARE_CMD(CurrentTextPlain)
	DECLARE_CMD(CurrentTextPlainWrapped)
	DECLARE_CMD(CurrentTextEnriched)
	DECLARE_CMD(CurrentTextHTML)
DECLARE_CMD(DraftSend)
DECLARE_CMD(DraftAppendTo)
DECLARE_CMD(DraftAppendNow)
DECLARE_CMD(DraftCopyOriginal)
DECLARE_CMD(DraftSign)
DECLARE_CMD(DraftEncrypt)
DECLARE_CMD(DraftNotifications)
	DECLARE_CMD(NotifyReceipt)
	DECLARE_CMD(NotifySuccess)
	DECLARE_CMD(NotifyFailure)
	DECLARE_CMD(NotifyDelay)
	DECLARE_CMD(NotifyEntireMsg)
DECLARE_CMD(DraftWrap)
DECLARE_CMD(DraftUnwrap)
DECLARE_CMD(DraftQuote)
DECLARE_CMD(DraftUnquote)
DECLARE_CMD(DraftRequote)
DECLARE_CMD(DraftShiftLeft)
DECLARE_CMD(DraftShiftRight)
DECLARE_CMD(DraftFont)
DECLARE_CMD(DraftSize)
DECLARE_CMD(DraftStyle)
	DECLARE_CMD(StylePlain)
	DECLARE_CMD(StyleBold)
	DECLARE_CMD(StyleItalic)
	DECLARE_CMD(StyleUnderline)
DECLARE_CMD(DraftAlignment)
	DECLARE_CMD(AlignLeft)
	DECLARE_CMD(AlignCenter)
	DECLARE_CMD(AlignRight)
	DECLARE_CMD(AlignJustified)
DECLARE_CMD(DraftColour)
	DECLARE_CMD(ColourBlack)
	DECLARE_CMD(ColourRed)
	DECLARE_CMD(ColourGreen)
	DECLARE_CMD(ColourBlue)
	DECLARE_CMD(ColourYellow)
	DECLARE_CMD(ColourCyan)
	DECLARE_CMD(ColourMagenta)
	DECLARE_CMD(ColourMulberry)
	DECLARE_CMD(ColourWhite)
	DECLARE_CMD(ColourOther)

DECLARE_CMD(AddressesAddBookMan)
DECLARE_CMD(AddressesNew)
DECLARE_CMD(AddressesOpen)
DECLARE_CMD(AddressesRename)
DECLARE_CMD(AddressesImport)
DECLARE_CMD(AddressesExport)
DECLARE_CMD(AddressesDelete)
DECLARE_CMD(AddressesLogin)
DECLARE_CMD(AddressesLogout)
DECLARE_CMD(AddressesRefresh)
DECLARE_CMD(AddressesSynchronise)
DECLARE_CMD(AddressesClearDisconnected)
DECLARE_CMD(AddressesExpandAddresses)
DECLARE_CMD(AddressesCaptureAddress)
DECLARE_CMD(AddressSearch)

DECLARE_CMD(CalendarManager)
DECLARE_CMD(CalendarSubscribe)
DECLARE_CMD(CalendarCreate)
DECLARE_CMD(CalendarRename)
DECLARE_CMD(CalendarRefresh)
DECLARE_CMD(CalendarDelete)
DECLARE_CMD(CalendarAddWeb)
DECLARE_CMD(CalendarRefreshWeb)
DECLARE_CMD(CalendarUploadWeb)
DECLARE_CMD(CalendarNewEvent)
DECLARE_CMD(CalendarNewToDo)
DECLARE_CMD(CalendarEditItem)
DECLARE_CMD(CalendarDuplicateItem)
DECLARE_CMD(CalendarDeleteItem)
DECLARE_CMD(CalendarGotoToday)
DECLARE_CMD(CalendarGotoDate)

DECLARE_CMD(WindowsCascade)
DECLARE_CMD(WindowsTileHor)
DECLARE_CMD(WindowsTileVert)
DECLARE_CMD(WindowsSaveDefault)
DECLARE_CMD(WindowsResetToDefault)
DECLARE_CMD(WindowsExpandHeader)
DECLARE_CMD(WindowsShowParts)
DECLARE_CMD(WindowsShowQuickSearch)
DECLARE_CMD(WindowsToolbars)
DECLARE_CMD(ToolbarsButtons)
DECLARE_CMD(ToolbarsSmall)
DECLARE_CMD(ToolbarsIcons)
DECLARE_CMD(ToolbarsCaptions)
DECLARE_CMD(ToolbarsBoth)
DECLARE_CMD(WindowsLayout)
DECLARE_CMD(Layout3Pane)
DECLARE_CMD(LayoutSingle)
DECLARE_CMD(LayoutListVert)
DECLARE_CMD(LayoutListHoriz)
DECLARE_CMD(LayoutItemsVert)
DECLARE_CMD(LayoutItemsHoriz)
DECLARE_CMD(LayoutPreviewVert)
DECLARE_CMD(LayoutPreviewHoriz)
DECLARE_CMD(LayoutAllVert)
DECLARE_CMD(LayoutAllHoriz)
DECLARE_CMD(WindowsShowList)
DECLARE_CMD(WindowsShowItems)
DECLARE_CMD(WindowsShowPreview)
DECLARE_CMD(WindowsOptions)
DECLARE_CMD(WindowsStatus)
DECLARE_CMD(WindowsDynamicStart)

DECLARE_CMD(HelpHelpTopics)
DECLARE_CMD(HelpAboutMul)
DECLARE_CMD(HelpAboutPlugins)
DECLARE_CMD(HelpDetailedTooltips)
DECLARE_CMD(HelpMulberryOnline)
DECLARE_CMD(HelpMulberryFAQ)
DECLARE_CMD(HelpMulberryUpdates)
DECLARE_CMD(HelpMulberrySupport)

DECLARE_CMD(AddressNew)
DECLARE_CMD(AddressEdit)
DECLARE_CMD(AddressDelete)
DECLARE_CMD(AddressNewMessage)

DECLARE_CMD(CalendarNewMessage)
DECLARE_CMD(CalendarFreeBusy)
DECLARE_CMD(CalendarInvite)
DECLARE_CMD(CalendarCompleted)

DECLARE_CMD(RulesNew)
DECLARE_CMD(RulesEdit)
DECLARE_CMD(RulesDelete)
DECLARE_CMD(RulesWriteScript)

	// Common toolbar
DECLARE_CMD(ToolbarNewLetter)
DECLARE_CMD(ToolbarNewLetterOption)
DECLARE_CMD(ToolbarDetailsBtn)
DECLARE_CMD(ToolbarMailboxExpungeBtn)
DECLARE_CMD(ToolbarSearchMailboxBtn)
DECLARE_CMD(ToolbarMessageReplyBtn)
DECLARE_CMD(ToolbarMessageReplyOptionBtn)
DECLARE_CMD(ToolbarMessageForwardBtn)
DECLARE_CMD(ToolbarMessageForwardOptionBtn)
DECLARE_CMD(ToolbarMessageBounceBtn)
DECLARE_CMD(ToolbarMessageRejectBtn)
DECLARE_CMD(ToolbarMessageSendAgainBtn)
DECLARE_CMD(ToolbarMessageDigestBtn)
DECLARE_CMD(ToolbarMessageCopyBtn)
DECLARE_CMD(ToolbarMessageCopyOptionBtn)
DECLARE_CMD(ToolbarMessageCopyPopup)
DECLARE_CMD(ToolbarMessageCopyOptionPopup)
DECLARE_CMD(ToolbarMessageDeleteBtn)
DECLARE_CMD(ToolbarMessageHeaderBtn)
DECLARE_CMD(ToolbarFileSaveBtn)
DECLARE_CMD(ToolbarMessagePrintBtn)

	// 3 pane toolbar
DECLARE_CMD(ToolbarZoomList)
DECLARE_CMD(ToolbarZoomItems)
DECLARE_CMD(ToolbarZoomPreview)
DECLARE_CMD(ToolbarCloseList)
DECLARE_CMD(ToolbarCloseItems)
DECLARE_CMD(ToolbarClosePreview)
DECLARE_CMD(ToolbarShowList)
DECLARE_CMD(ToolbarShowItems)
DECLARE_CMD(ToolbarShowPreview)

	// Server toolbar
DECLARE_CMD(ToolbarServerLoginBtn)
DECLARE_CMD(ToolbarCreateMailboxBtn)
DECLARE_CMD(ToolbarCheckMailboxBtn)
DECLARE_CMD(ToolbarCabinetPopup)

	// Mailbox toolbar
DECLARE_CMD(ToolbarMailboxMatchBtn)
DECLARE_CMD(ToolbarMailboxMatchOptionBtn)
DECLARE_CMD(ToolbarMailboxSelectPopup)
DECLARE_CMD(ToolbarMailboxApplyRules)

	// SMTP toolbar
DECLARE_CMD(ToolbarSMTPHoldBtn)
DECLARE_CMD(ToolbarSMTPPriorityBtn)
DECLARE_CMD(ToolbarSMTPAccountsPopup)
DECLARE_CMD(ToolbarSMTPEnableBtn)

	// Message toolbar
DECLARE_CMD(ToolbarMessageFlagsBtn)
DECLARE_CMD(ToolbarMessageDeleteNextBtn)
DECLARE_CMD(ToolbarMessagePreviousBtn)
DECLARE_CMD(ToolbarMessageNextBtn)

	// Letter toolbar
DECLARE_CMD(ToolbarLetterAttachBtn)
DECLARE_CMD(ToolbarLetterSpellBtn)
DECLARE_CMD(ToolbarLetterReceiptBtn)
DECLARE_CMD(ToolbarLetterDSNBtn)
DECLARE_CMD(ToolbarLetterSignatureBtn)
DECLARE_CMD(ToolbarLetterSignBtn)
DECLARE_CMD(ToolbarLetterEncryptBtn)
DECLARE_CMD(ToolbarLetterSendBtn)
DECLARE_CMD(ToolbarLetterAddCcBtn)
DECLARE_CMD(ToolbarLetterAddBccBtn)
DECLARE_CMD(ToolbarLetterAppendNowBtn)
DECLARE_CMD(ToolbarLetterCopyOriginalBtn)
DECLARE_CMD(ToolbarLetterExternalBtn)
DECLARE_CMD(ToolbarWindowsExpandHeaderBtn)

	// Address Book Manager toolbar
DECLARE_CMD(ToolbarAdbkMgrNewBtn)
DECLARE_CMD(ToolbarAdbkMgrOpenBtn)
DECLARE_CMD(ToolbarAdbkMgrDeleteBtn)
DECLARE_CMD(ToolbarAdbkMgrSearchBtn)
DECLARE_CMD(ToolbarAdbkMgrPropertiesBtn)

	// Address Book toolbar
DECLARE_CMD(ToolbarAddressBookAddressBtn)
DECLARE_CMD(ToolbarAddressBookGroupBtn)
DECLARE_CMD(ToolbarAddressBookEditBtn)
DECLARE_CMD(ToolbarAddressBookDeleteBtn)

	// Calendar toolbar
DECLARE_CMD(ToolbarCalendarCreateBtn)
DECLARE_CMD(ToolbarCalendarNewEventBtn)
DECLARE_CMD(ToolbarCalendarNewToDoBtn)
DECLARE_CMD(ToolbarCalendarTodayBtn)
DECLARE_CMD(ToolbarCalendarGotoBtn)
DECLARE_CMD(ToolbarCalendarDayBtn)
DECLARE_CMD(ToolbarCalendarWorkBtn)
DECLARE_CMD(ToolbarCalendarWeekBtn)
DECLARE_CMD(ToolbarCalendarMonthBtn)
DECLARE_CMD(ToolbarCalendarYearBtn)
DECLARE_CMD(ToolbarCalendarSummaryBtn)
DECLARE_CMD(ToolbarCalendarTasksBtn)

	// Explicit commands
DECLARE_CMD(CommandMessageCopy)
DECLARE_CMD(CommandMessageMove)

DECLARE_CMD(CommandMailboxThreadAll)
DECLARE_CMD(CommandMailboxThreadMarkSeen)
DECLARE_CMD(CommandMailboxThreadMarkImportant)
DECLARE_CMD(CommandMailboxThreadMarkDeleted)
DECLARE_CMD(CommandMailboxThreadSkip)
	
DECLARE_CMD(CommandMailboxMatchUnseen)
DECLARE_CMD(CommandMailboxMatchImportant)
DECLARE_CMD(CommandMailboxMatchDeleted)
DECLARE_CMD(CommandMailboxMatchSentToday)
DECLARE_CMD(CommandMailboxMatchFrom)
DECLARE_CMD(CommandMailboxMatchSubject)
DECLARE_CMD(CommandMailboxMatchDate)

struct SCommandMap
{
	const char* title;
	unsigned long cmd;
};

#define DECLARE_MAP(name)	{CCommand::c##name, CCommand::e##name},

static SCommandMap sCmdMap[] =
{
	DECLARE_MAP(FileNewMessage)
	DECLARE_MAP(FileOpenMailbox)
	DECLARE_MAP(FileOpenMessage)
	DECLARE_MAP(FileOpenDraft)
	DECLARE_MAP(FileImport)
	DECLARE_MAP(FileExport)
	DECLARE_MAP(FileClose)
	DECLARE_MAP(FileSave)
	DECLARE_MAP(FileSaveAs)
	DECLARE_MAP(FileRevert)
	DECLARE_MAP(FilePreferences)
	DECLARE_MAP(FileDisconnect)
	DECLARE_MAP(FileShowOutgoingQueues)
	DECLARE_MAP(FilePageSetup)
	DECLARE_MAP(FilePrint)
	DECLARE_MAP(FileExit)
	
	DECLARE_MAP(EditUndo)
	DECLARE_MAP(EditRedo)
	DECLARE_MAP(EditCut)
	DECLARE_MAP(EditCopy)
	DECLARE_MAP(EditPaste)
	DECLARE_MAP(EditPasteAsQuote)
	DECLARE_MAP(EditDelete)
	DECLARE_MAP(EditSelectAll)
	DECLARE_MAP(EditFind)
	DECLARE_MAP(EditFindNext)
	DECLARE_MAP(EditFindSel)
	DECLARE_MAP(EditReplace)
	DECLARE_MAP(EditReplaceFindNext)
	DECLARE_MAP(EditReplaceAll)
	DECLARE_MAP(EditTextMacros)
	DECLARE_MAP(EditProperties)
	DECLARE_MAP(EditChangePassword)
	DECLARE_MAP(EditCheckSpelling)

	DECLARE_MAP(EditOpenLink)
	DECLARE_MAP(EditCopyLink)

	DECLARE_MAP(MailboxCreate)
	DECLARE_MAP(MailboxRename)
	DECLARE_MAP(MailboxMarkContentsSeen)
	DECLARE_MAP(MailboxDelete)
	DECLARE_MAP(MailboxExpunge)
	DECLARE_MAP(MailboxSynchronise)
	DECLARE_MAP(MailboxClearDisconnected)
	DECLARE_MAP(MailboxDisplayHierarchy)
		DECLARE_MAP(DispHNew)
		DECLARE_MAP(DispHEdit)
		DECLARE_MAP(DispHRemove)
		DECLARE_MAP(DispHReset)
		DECLARE_MAP(DispHRefresh)
	DECLARE_MAP(MailboxCabinets)
		DECLARE_MAP(CabNew)
		DECLARE_MAP(CabEdit)
		DECLARE_MAP(CabRemove)
		DECLARE_MAP(CabAddWild)
		DECLARE_MAP(CabEditWild)
		DECLARE_MAP(CabRemoveItem)
	DECLARE_MAP(MailboxSubscribe)
	DECLARE_MAP(MailboxUnsubscribe)
	DECLARE_MAP(MailboxCheckMail)
	DECLARE_MAP(MailboxCheckFavourites)
	DECLARE_MAP(MailboxSortBy)
		DECLARE_MAP(SortTo)
		DECLARE_MAP(SortFrom)
		DECLARE_MAP(SortReply)
		DECLARE_MAP(SortSender)
		DECLARE_MAP(SortCC)
		DECLARE_MAP(SortSubject)
		DECLARE_MAP(SortThread)
		DECLARE_MAP(SortDateSent)
		DECLARE_MAP(SortDateReceived)
		DECLARE_MAP(SortSize)
		DECLARE_MAP(SortFlags)
		DECLARE_MAP(SortNumber)
		DECLARE_MAP(SortSmartAddress)
		DECLARE_MAP(SortAttachments)
		DECLARE_MAP(SortParts)
		DECLARE_MAP(SortMatching)
		DECLARE_MAP(SortDisconnected)
	DECLARE_MAP(MailboxGotoMessage)
	DECLARE_MAP(MailboxLoadAllMessages)
	DECLARE_MAP(MailboxUpdateMatch)
	DECLARE_MAP(MailboxSearch)
	DECLARE_MAP(MailboxSearchAgain)
	DECLARE_MAP(MailboxNextSearch)
	DECLARE_MAP(MailboxRules)
	DECLARE_MAP(MailboxApplyRules)
	DECLARE_MAP(MailboxMakeRule)

	DECLARE_MAP(MessagesReadPrev)
	DECLARE_MAP(MessagesReadNext)
	DECLARE_MAP(MessagesDelAndReadNext)
	DECLARE_MAP(MessagesCopyAndReadNext)
	DECLARE_MAP(MessagesReply)
	DECLARE_MAP(MessagesReplyToSender)
	DECLARE_MAP(MessagesReplyToFrom)
	DECLARE_MAP(MessagesReplyToAll)
	DECLARE_MAP(MessagesForward)
	DECLARE_MAP(MessagesBounce)
	DECLARE_MAP(MessagesReject)
	DECLARE_MAP(MessagesSendAgain)
	DECLARE_MAP(MessagesCreateDigest)
	DECLARE_MAP(MessagesCopyTo)
	DECLARE_MAP(MessagesCopyNow)
	DECLARE_MAP(MessagesFlags)
		DECLARE_MAP(FlagsSeen)
		DECLARE_MAP(FlagsImportant)
		DECLARE_MAP(FlagsAnswered)
		DECLARE_MAP(FlagsDeleted)
		DECLARE_MAP(FlagsDraft)
		DECLARE_MAP(FlagsLabel1)
		DECLARE_MAP(FlagsLabel2)
		DECLARE_MAP(FlagsLabel3)
		DECLARE_MAP(FlagsLabel4)
		DECLARE_MAP(FlagsLabel5)
		DECLARE_MAP(FlagsLabel6)
		DECLARE_MAP(FlagsLabel7)
		DECLARE_MAP(FlagsLabel8)
	DECLARE_MAP(MessagesViewParts)
	DECLARE_MAP(MessagesExtractParts)
	DECLARE_MAP(MessagesDecodeEmbedded)
		DECLARE_MAP(DecodeBinHex)
		DECLARE_MAP(DecodeUU)
	DECLARE_MAP(MessagesVerifyDecrypt)
	DECLARE_MAP(MessagesDelete)
	DECLARE_MAP(MessagesShowHeader)

	DECLARE_MAP(DraftIncludeOrig)
	DECLARE_MAP(DraftInsertSig)
	DECLARE_MAP(DraftAddCc)
	DECLARE_MAP(DraftAddBcc)
	DECLARE_MAP(DraftAttachFile)
	DECLARE_MAP(DraftNewTextPart)
		DECLARE_MAP(NewTextPlain)
		DECLARE_MAP(NewTextEnriched)
		DECLARE_MAP(NewTextHTML)
	DECLARE_MAP(DraftNewMultipart)
		DECLARE_MAP(NewMultiMixed)
		DECLARE_MAP(NewMultiParallel)
		DECLARE_MAP(NewMultiDigest)
		DECLARE_MAP(NewMultiAlt)
	DECLARE_MAP(DraftCurrentTextPart)
		DECLARE_MAP(CurrentTextPlain)
		DECLARE_MAP(CurrentTextPlainWrapped)
		DECLARE_MAP(CurrentTextEnriched)
		DECLARE_MAP(CurrentTextHTML)
	DECLARE_MAP(DraftSend)
	DECLARE_MAP(DraftAppendTo)
	DECLARE_MAP(DraftAppendNow)
	DECLARE_MAP(DraftCopyOriginal)
	DECLARE_MAP(DraftSign)
	DECLARE_MAP(DraftEncrypt)
	DECLARE_MAP(DraftNotifications)
		DECLARE_MAP(NotifyReceipt)
		DECLARE_MAP(NotifySuccess)
		DECLARE_MAP(NotifyFailure)
		DECLARE_MAP(NotifyDelay)
		DECLARE_MAP(NotifyEntireMsg)
	DECLARE_MAP(DraftWrap)
	DECLARE_MAP(DraftUnwrap)
	DECLARE_MAP(DraftQuote)
	DECLARE_MAP(DraftUnquote)
	DECLARE_MAP(DraftRequote)
	DECLARE_MAP(DraftShiftLeft)
	DECLARE_MAP(DraftShiftRight)
	DECLARE_MAP(DraftFont)
	DECLARE_MAP(DraftSize)
	DECLARE_MAP(DraftStyle)
		DECLARE_MAP(StylePlain)
		DECLARE_MAP(StyleBold)
		DECLARE_MAP(StyleItalic)
		DECLARE_MAP(StyleUnderline)
	DECLARE_MAP(DraftAlignment)
		DECLARE_MAP(AlignLeft)
		DECLARE_MAP(AlignCenter)
		DECLARE_MAP(AlignRight)
		DECLARE_MAP(AlignJustified)
	DECLARE_MAP(DraftColour)
		DECLARE_MAP(ColourBlack)
		DECLARE_MAP(ColourRed)
		DECLARE_MAP(ColourGreen)
		DECLARE_MAP(ColourBlue)
		DECLARE_MAP(ColourYellow)
		DECLARE_MAP(ColourCyan)
		DECLARE_MAP(ColourMagenta)
		DECLARE_MAP(ColourMulberry)
		DECLARE_MAP(ColourWhite)
		DECLARE_MAP(ColourOther)

	DECLARE_MAP(AddressesAddBookMan)
	DECLARE_MAP(AddressesNew)
	DECLARE_MAP(AddressesOpen)
	DECLARE_MAP(AddressesRename)
	DECLARE_MAP(AddressesImport)
	DECLARE_MAP(AddressesExport)
	DECLARE_MAP(AddressesDelete)
	DECLARE_MAP(AddressesLogin)
	DECLARE_MAP(AddressesLogout)
	DECLARE_MAP(AddressesRefresh)
	DECLARE_MAP(AddressesSynchronise)
	DECLARE_MAP(AddressesClearDisconnected)
	DECLARE_MAP(AddressesExpandAddresses)
	DECLARE_MAP(AddressesCaptureAddress)
	DECLARE_MAP(AddressSearch)

	DECLARE_MAP(CalendarManager)
	DECLARE_MAP(CalendarSubscribe)
	DECLARE_MAP(CalendarCreate)
	DECLARE_MAP(CalendarRename)
	DECLARE_MAP(CalendarRefresh)
	DECLARE_MAP(CalendarDelete)
	DECLARE_MAP(CalendarAddWeb)
	DECLARE_MAP(CalendarRefreshWeb)
	DECLARE_MAP(CalendarUploadWeb)
	DECLARE_MAP(CalendarNewEvent)
	DECLARE_MAP(CalendarNewToDo)
	DECLARE_MAP(CalendarEditItem)
	DECLARE_MAP(CalendarDuplicateItem)
	DECLARE_MAP(CalendarDeleteItem)
	DECLARE_MAP(CalendarGotoToday)
	DECLARE_MAP(CalendarGotoDate)

	DECLARE_MAP(WindowsCascade)
	DECLARE_MAP(WindowsTileHor)
	DECLARE_MAP(WindowsTileVert)
	DECLARE_MAP(WindowsSaveDefault)
	DECLARE_MAP(WindowsResetToDefault)
	DECLARE_MAP(WindowsExpandHeader)
	DECLARE_MAP(WindowsShowParts)
	DECLARE_MAP(WindowsShowQuickSearch)
	DECLARE_MAP(WindowsToolbars)
		DECLARE_MAP(ToolbarsButtons)
		DECLARE_MAP(ToolbarsSmall)
		DECLARE_MAP(ToolbarsIcons)
		DECLARE_MAP(ToolbarsCaptions)
		DECLARE_MAP(ToolbarsBoth)
	DECLARE_MAP(WindowsLayout)
		DECLARE_MAP(Layout3Pane)
		DECLARE_MAP(LayoutSingle)
		DECLARE_MAP(LayoutListVert)
		DECLARE_MAP(LayoutListHoriz)
		DECLARE_MAP(LayoutItemsVert)
		DECLARE_MAP(LayoutItemsHoriz)
		DECLARE_MAP(LayoutPreviewVert)
		DECLARE_MAP(LayoutPreviewHoriz)
		DECLARE_MAP(LayoutAllVert)
		DECLARE_MAP(LayoutAllHoriz)
	DECLARE_MAP(WindowsShowList)
	DECLARE_MAP(WindowsShowItems)
	DECLARE_MAP(WindowsShowPreview)
	DECLARE_MAP(WindowsOptions)
	DECLARE_MAP(WindowsStatus)
	DECLARE_MAP(WindowsDynamicStart)

	DECLARE_MAP(HelpHelpTopics)
	DECLARE_MAP(HelpAboutMul)
	DECLARE_MAP(HelpAboutPlugins)
	DECLARE_MAP(HelpDetailedTooltips)
	DECLARE_MAP(HelpMulberryOnline)
	DECLARE_MAP(HelpMulberryFAQ)
	DECLARE_MAP(HelpMulberryUpdates)
	DECLARE_MAP(HelpMulberrySupport)

	DECLARE_MAP(AddressNew)
	DECLARE_MAP(AddressEdit)
	DECLARE_MAP(AddressDelete)
	DECLARE_MAP(AddressNewMessage)

	DECLARE_MAP(CalendarNewMessage)
	DECLARE_MAP(CalendarFreeBusy)
	DECLARE_MAP(CalendarInvite)
	DECLARE_MAP(CalendarCompleted)

	DECLARE_MAP(RulesNew)
	DECLARE_MAP(RulesEdit)
	DECLARE_MAP(RulesDelete)
	DECLARE_MAP(RulesWriteScript)

	// Common toolbar
	DECLARE_MAP(ToolbarNewLetter)
	DECLARE_MAP(ToolbarNewLetterOption)
	DECLARE_MAP(ToolbarDetailsBtn)
	DECLARE_MAP(ToolbarMailboxExpungeBtn)
	DECLARE_MAP(ToolbarSearchMailboxBtn)
	DECLARE_MAP(ToolbarMessageReplyBtn)
	DECLARE_MAP(ToolbarMessageReplyOptionBtn)
	DECLARE_MAP(ToolbarMessageForwardBtn)
	DECLARE_MAP(ToolbarMessageForwardOptionBtn)
	DECLARE_MAP(ToolbarMessageBounceBtn)
	DECLARE_MAP(ToolbarMessageRejectBtn)
	DECLARE_MAP(ToolbarMessageSendAgainBtn)
	DECLARE_MAP(ToolbarMessageDigestBtn)
	DECLARE_MAP(ToolbarMessageCopyBtn)
	DECLARE_MAP(ToolbarMessageCopyOptionBtn)
	DECLARE_MAP(ToolbarMessageCopyPopup)
	DECLARE_MAP(ToolbarMessageCopyOptionPopup)
	DECLARE_MAP(ToolbarMessageDeleteBtn)
	DECLARE_MAP(ToolbarMessageDeleteBtn)
	DECLARE_MAP(ToolbarFileSaveBtn)
	DECLARE_MAP(ToolbarMessagePrintBtn)

	// 3 pane toolbar
	DECLARE_MAP(ToolbarZoomList)
	DECLARE_MAP(ToolbarZoomItems)
	DECLARE_MAP(ToolbarZoomPreview)
	DECLARE_MAP(ToolbarCloseList)
	DECLARE_MAP(ToolbarCloseItems)
	DECLARE_MAP(ToolbarClosePreview)
	DECLARE_MAP(ToolbarShowList)
	DECLARE_MAP(ToolbarShowItems)
	DECLARE_MAP(ToolbarShowPreview)

	// Server toolbar
	DECLARE_MAP(ToolbarServerLoginBtn)
	DECLARE_MAP(ToolbarCreateMailboxBtn)
	DECLARE_MAP(ToolbarCheckMailboxBtn)
	DECLARE_MAP(ToolbarCabinetPopup)

	// Mailbox toolbar
	DECLARE_MAP(ToolbarMailboxMatchBtn)
	DECLARE_MAP(ToolbarMailboxMatchOptionBtn)
	DECLARE_MAP(ToolbarMailboxSelectPopup)
	DECLARE_MAP(ToolbarMailboxApplyRules)

	// SMTP toolbar
	DECLARE_MAP(ToolbarSMTPHoldBtn)
	DECLARE_MAP(ToolbarSMTPPriorityBtn)
	DECLARE_MAP(ToolbarSMTPAccountsPopup)
	DECLARE_MAP(ToolbarSMTPEnableBtn)

	// Message toolbar
	DECLARE_MAP(ToolbarMessageFlagsBtn)
	DECLARE_MAP(ToolbarMessageDeleteNextBtn)
	DECLARE_MAP(ToolbarMessagePreviousBtn)
	DECLARE_MAP(ToolbarMessageNextBtn)

	// Letter toolbar
	DECLARE_MAP(ToolbarLetterAttachBtn)
	DECLARE_MAP(ToolbarLetterSpellBtn)
	DECLARE_MAP(ToolbarLetterReceiptBtn)
	DECLARE_MAP(ToolbarLetterDSNBtn)
	DECLARE_MAP(ToolbarLetterSignatureBtn)
	DECLARE_MAP(ToolbarLetterSignBtn)
	DECLARE_MAP(ToolbarLetterEncryptBtn)
	DECLARE_MAP(ToolbarLetterSendBtn)
	DECLARE_MAP(ToolbarLetterAddCcBtn)
	DECLARE_MAP(ToolbarLetterAddBccBtn)
	DECLARE_MAP(ToolbarLetterAppendNowBtn)
	DECLARE_MAP(ToolbarLetterCopyOriginalBtn)
	DECLARE_MAP(ToolbarLetterExternalBtn)
	DECLARE_MAP(ToolbarWindowsExpandHeaderBtn)

	// Address Book Manager toolbar
	DECLARE_MAP(ToolbarAdbkMgrNewBtn)
	DECLARE_MAP(ToolbarAdbkMgrOpenBtn)
	DECLARE_MAP(ToolbarAdbkMgrDeleteBtn)
	DECLARE_MAP(ToolbarAdbkMgrSearchBtn)
	DECLARE_MAP(ToolbarAdbkMgrPropertiesBtn)

	// Address Book toolbar
	DECLARE_MAP(ToolbarAddressBookAddressBtn)
	DECLARE_MAP(ToolbarAddressBookGroupBtn)
	DECLARE_MAP(ToolbarAddressBookEditBtn)
	DECLARE_MAP(ToolbarAddressBookDeleteBtn)

	// Calendar toolbar
	DECLARE_MAP(ToolbarCalendarCreateBtn)
	DECLARE_MAP(ToolbarCalendarNewEventBtn)
	DECLARE_MAP(ToolbarCalendarNewToDoBtn)
	DECLARE_MAP(ToolbarCalendarTodayBtn)
	DECLARE_MAP(ToolbarCalendarGotoBtn)
	DECLARE_MAP(ToolbarCalendarDayBtn)
	DECLARE_MAP(ToolbarCalendarWorkBtn)
	DECLARE_MAP(ToolbarCalendarWeekBtn)
	DECLARE_MAP(ToolbarCalendarMonthBtn)
	DECLARE_MAP(ToolbarCalendarYearBtn)
	DECLARE_MAP(ToolbarCalendarSummaryBtn)
	DECLARE_MAP(ToolbarCalendarTasksBtn)

	// Explicit commands
	DECLARE_MAP(CommandMessageCopy)
	DECLARE_MAP(CommandMessageMove)

	DECLARE_MAP(CommandMailboxThreadAll)
	DECLARE_MAP(CommandMailboxThreadMarkSeen)
	DECLARE_MAP(CommandMailboxThreadMarkImportant)
	DECLARE_MAP(CommandMailboxThreadMarkDeleted)
	DECLARE_MAP(CommandMailboxThreadSkip)
	
	DECLARE_MAP(CommandMailboxMatchUnseen)
	DECLARE_MAP(CommandMailboxMatchImportant)
	DECLARE_MAP(CommandMailboxMatchDeleted)
	DECLARE_MAP(CommandMailboxMatchSentToday)
	DECLARE_MAP(CommandMailboxMatchFrom)
	DECLARE_MAP(CommandMailboxMatchSubject)
	DECLARE_MAP(CommandMailboxMatchDate)

	{NULL, 0}
};

unsigned long CCommand::CommandToCmd(const char* cmdname)
{
	SCommandMap* map = sCmdMap;
	unsigned long comp1 = *reinterpret_cast<const unsigned long*>(cmdname);
	while(map->title)
	{
		unsigned long comp2 = *reinterpret_cast<const unsigned long*>(map->title);
		if (comp1 == comp2)
		{
			if (!::strcmp(cmdname, map->title))
				return map->cmd;
		}

		map++;
	}

	return 0;
}

#endif
