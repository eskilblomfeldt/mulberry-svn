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


// Header for CAdbkManagerTitleTable class

#ifndef __CADBKMANAGERTITLETABLE__MULBERRY__
#define __CADBKMANAGERTITLETABLE__MULBERRY__

#include "CTitleTableView.h"


// Classes
class	CAdbkManagerTitleTable : public CTitleTableView
{
public:
	enum
	{
		eAdbkManagerTitleIconOpen,
		eAdbkManagerTitleIconResolve,
		eAdbkManagerTitleIconSearch
	};

	enum
	{
		kPopupOpen = 1,
		kPopupResolve,
		kPopupSearch,
		kPopupInsertAfter,
		kPopupInsertBefore,
		kPopupDelete
	};

	CAdbkManagerTitleTable(JXScrollbarSet* scrollbarSet,
							 JXContainer* enclosure,
							 const HSizingOption hSizing, 
							 const VSizingOption vSizing,
							 const JCoordinate x, const JCoordinate y,
							 const JCoordinate w, const JCoordinate h);
	virtual 		~CAdbkManagerTitleTable();

protected:
	virtual void OnCreate();
	virtual void MenuChoice(TableIndexT col, bool sort_col, JIndex menu_item);
	virtual void OnRButtonDown(const JPoint& point, 
								 const JXButtonStates& buttonStates,
								 const JXKeyModifiers& modifiers);

	virtual void	DrawItem(JPainter* p, SColumnInfo& col_info, const JRect &cellRect);
};

#endif
