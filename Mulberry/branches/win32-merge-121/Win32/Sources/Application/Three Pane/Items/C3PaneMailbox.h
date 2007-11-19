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


// Header for C3PaneMailbox class

#ifndef __C3PANEMAILBOX__MULBERRY__
#define __C3PANEMAILBOX__MULBERRY__

#include "C3PaneViewPanel.h"

#include "CMailboxInfoView.h"

// Classes
class CMbox;

class C3PaneMailbox : public C3PaneViewPanel
{
	friend class C3PaneItems;

public:
					C3PaneMailbox();
	virtual 		~C3PaneMailbox();

	virtual void	Init();

			void	GetOpenItems(cdstrvect& items) const;
			void	CloseOpenItems();
			void	SetOpenItems(const cdstrvect& items);

	virtual void	DoClose();

	virtual bool	TestCloseAll();
	virtual void	DoCloseAll();

	virtual bool	TestCloseOne(unsigned long index);
	virtual void	DoCloseOne(unsigned long index);

	virtual bool	TestCloseOthers(unsigned long index);
	virtual void	DoCloseOthers(unsigned long index);

			void	SetUseSubstitute(bool subs);
			bool	GetUseSubstitute() const
		{ return mUseSubstitute; }
			bool	IsSubstituteLocked(unsigned long index) const;
			bool	IsSubstituteDynamic(unsigned long index) const;

	virtual bool	IsSpecified() const;

	virtual CBaseView*	GetBaseView() const;
	CMailboxInfoView*	GetMailboxView()
		{ return &mMailboxView; }

	virtual cdstring GetTitle() const;
	virtual UINT	GetIconID() const;

protected:
	CMailboxInfoView	mMailboxView;
	bool				mDoneInit;
	bool				mUseSubstitute;

	// Generated message map functions
	//{{AFX_MSG()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
