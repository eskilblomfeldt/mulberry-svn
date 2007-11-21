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

#if !defined(AFX_MACPROGRESSCTRL_H__603BBF44_B19C_11D3_90FA_0020AFBC499D__INCLUDED_)
#define AFX_MACPROGRESSCTRL_H__603BBF44_B19C_11D3_90FA_0020AFBC499D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MacProgressCtrl.h : header file
//
//	CMacProgressCtrl class, version 1.0
//
//	Copyright (c) 1999 Paul M. Meidinger (pmmeidinger@yahoo.com)
//
// Feel free to modifiy and/or distribute this file, but
// do not remove this header.
//
// I would appreciate a notification of any bugs discovered or 
// improvements that could be made.
//
// This file is provided "as is" with no expressed or implied warranty.
//
//	History:
//		PMM	12/21/1999		Initial implementation.		

/////////////////////////////////////////////////////////////////////////////
// CMacProgressCtrl window

class CMacProgressCtrl : public CProgressCtrl
{
// Construction
public:
	CMacProgressCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMacProgressCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL GetIndeterminate();
	void SetIndeterminate(BOOL bIndeterminate = TRUE);
	COLORREF GetColor();
	void SetColor(COLORREF crColor);
	virtual ~CMacProgressCtrl();

	void GetRange(int& nLower, int& nUpper);
	int GetPos();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMacProgressCtrl)
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	int		m_nIndOffset;
	BOOL	m_bIndeterminate;
	UINT	mTimerID;
	
	void DrawVerticalBar(CDC *pDC, const CRect rect);
	void DrawHorizontalBar(CDC *pDC, const CRect rect);
	void DeletePens();
	void CreatePens();

	CPen m_penColor;
	CPen m_penColorLight;
	CPen m_penColorLighter;
	CPen m_penColorDark;
	CPen m_penColorDarker;

	CPen m_penColorGray;
	CPen m_penColorGrayLight;
	CPen m_penColorGrayLighter;
	CPen m_penColorGrayDark;
	CPen m_penColorGrayDarker;

	CPen m_penDkShadow;
	CPen m_penShadow;
	CPen m_penLiteShadow;
	void GetColors();

	COLORREF m_crColor;
	COLORREF m_crColorLight;
	COLORREF m_crColorLighter;
	COLORREF m_crColorLightest;
	COLORREF m_crColorDark;
	COLORREF m_crColorDarker;

	COLORREF m_crColorGray;
	COLORREF m_crColorGrayLight;
	COLORREF m_crColorGrayLighter;
	COLORREF m_crColorGrayLightest;
	COLORREF m_crColorGrayDark;
	COLORREF m_crColorGrayDarker;

	COLORREF m_crDkShadow;
	COLORREF m_crShadow;
	COLORREF m_crLiteShadow;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

inline void CMacProgressCtrl::GetRange(int& nLower, int& nUpper)
	{ ASSERT(::IsWindow(m_hWnd));
		nLower = ::SendMessage(m_hWnd, PBM_SETRANGE, 0, NULL); 
		nUpper = ::SendMessage(m_hWnd, PBM_SETRANGE, 1, NULL); }
inline int CMacProgressCtrl::GetPos()
	{ ASSERT(::IsWindow(m_hWnd)); return (int) LOWORD(::SendMessage(m_hWnd, PBM_GETPOS, 0L, 0L)); }

#endif // !defined(AFX_MACPROGRESSCTRL_H__603BBF44_B19C_11D3_90FA_0020AFBC499D__INCLUDED_)
