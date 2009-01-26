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

#include "CNewComponentDescription.h"

#include "CTextDisplay.h"
#include "CTextFieldX.h"

// ---------------------------------------------------------------------------
//	CNewComponentDescription														  [public]
/**
	Default constructor */

CNewComponentDescription::CNewComponentDescription(LStream *inStream) :
	CNewComponentPanel(inStream)
{
}


// ---------------------------------------------------------------------------
//	~CNewComponentDescription														  [public]
/**
	Destructor */

CNewComponentDescription::~CNewComponentDescription()
{
}

#pragma mark -

void CNewComponentDescription::FinishCreateSelf()
{
	// Get UI items
	mLocation = dynamic_cast<CTextFieldX*>(FindPaneByID(eLocation_ID));
	mDescription = dynamic_cast<CTextDisplay*>(FindPaneByID(eDescription_ID));
}

void CNewComponentDescription::SetEvent(const iCal::CICalendarVEvent& vevent)
{
	// Set the relevant fields
	
	mLocation->SetText(vevent.GetLocation());
	mDescription->SetText(vevent.GetDescription());
}

void CNewComponentDescription::SetToDo(const iCal::CICalendarVToDo& vtodo)
{
	// Set the relevant fields
	
	mLocation->SetText(vtodo.GetLocation());
	mDescription->SetText(vtodo.GetDescription());
}

void CNewComponentDescription::GetEvent(iCal::CICalendarVEvent& vevent)
{
	// Do descriptive items
	{
		cdstring location = mLocation->GetText();
		cdstring description;
		mDescription->GetText(description);
		
		vevent.EditDetails(description, location);
	}
}

void CNewComponentDescription::GetToDo(iCal::CICalendarVToDo& vtodo)
{
	// Do descriptive items
	{
		cdstring location = mLocation->GetText();
		cdstring description;
		mDescription->GetText(description);
		
		vtodo.EditDetails(description, location);
	}
}

void CNewComponentDescription::SetReadOnly(bool read_only)
{
	mReadOnly = read_only;

	mLocation->SetReadOnly(read_only);
	mDescription->SetReadOnly(read_only);
}
