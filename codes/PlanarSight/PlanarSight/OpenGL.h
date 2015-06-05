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
	void setup_light();
	void setup_material();
	GLuint loadTex(LPCTSTR file_addr);

public:
	Rendering *rendering;
	CRect frame; 
	CWnd* outterWallBtn;
	CWnd* innerWallBtn;
	CWnd* monsterBtn;
	CWnd* startBtn;
	CWnd* endBtn;

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	void SetUp2D();
	void SetUp3D();
};


