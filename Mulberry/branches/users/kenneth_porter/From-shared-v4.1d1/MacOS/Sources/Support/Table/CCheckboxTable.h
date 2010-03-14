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


// Header for CCheckboxTable class

#ifndef __CCHECKBOXTABLE__MULBERRY__
#define __CCHECKBOXTABLE__MULBERRY__

#include "CTableDrag.h"
#include "cdstring.h"

// Consts

// Classes

class LArray;

class CCheckboxTable : public CTableDrag
{
public:
	enum { class_ID = 'ChTb' };

					CCheckboxTable();
					CCheckboxTable(LStream *inStream);
	virtual 		~CCheckboxTable();

protected:
	virtual void		FinishCreateSelf(void);							// Get details of sub-panes

public:
	virtual void		SetContents(const cdstrvect& items);
	virtual void		SetSelection(const boolvector& selection);
	virtual void		GetSelection(boolvector& selection) const;

protected:
			void		InitTextTable(void);

};

#endif
