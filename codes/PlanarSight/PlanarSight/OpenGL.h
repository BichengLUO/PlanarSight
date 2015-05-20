#pragma once
#include "Rendering.h"

// COpenGL

class COpenGL : public CWnd
{
	DECLARE_DYNAMIC(COpenGL)

public:
	COpenGL();
	virtual ~COpenGL();

protected:
	DECLARE_MESSAGE_MAP()
	HDC hdc;
	HGLRC hglrc;
	int MySetPixelFormat(HDC hdc);
	void RenderScene();

public:
	Rendering *rendering;

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};


