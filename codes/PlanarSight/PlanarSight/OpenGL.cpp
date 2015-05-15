// OpenGL.cpp : 实现文件
//

#include "stdafx.h"
#include "PlanarSight.h"
#include "OpenGL.h"


// COpenGL

IMPLEMENT_DYNAMIC(COpenGL, CWnd)

COpenGL::COpenGL()
{
	rendering = new Rendering;
}

COpenGL::~COpenGL()
{
	delete rendering;
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hglrc);                 //删除渲染描述表
	::ReleaseDC(m_hWnd, hdc);                //释放设备描述表
}


BEGIN_MESSAGE_MAP(COpenGL, CWnd)
	ON_WM_CREATE()
	ON_WM_TIMER()
END_MESSAGE_MAP()



// COpenGL 消息处理程序




int COpenGL::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	MySetPixelFormat(::GetDC(m_hWnd));
	//CPaintDC dc(this);   //绘图设备描述表
	//创建绘图描述表
	hdc = ::GetDC(m_hWnd);
	//创建渲染描述表
	hglrc = wglCreateContext(hdc);
	//使绘图描述表为当前调用线程的当前绘图描述表
	wglMakeCurrent(hdc, hglrc);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 100, 0, 100, -50, 50);

	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	SetTimer(1, 10, NULL);
	return 0;
}


void COpenGL::RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	rendering->draw();
	SwapBuffers(hdc);
}

int COpenGL::MySetPixelFormat(HDC hdc)
{
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),  //pfd结构的大小 
		1,                             //版本号
		PFD_DRAW_TO_WINDOW |             //支持在窗口中绘图
		PFD_SUPPORT_OPENGL |             //支持OpenGl
		PFD_DOUBLEBUFFER,               //双缓存模式
		PFD_TYPE_RGBA,                  //RGBA颜色模式
		24,                             //24位颜色深度
		0, 0, 0, 0, 0, 0,                    //忽略颜色位
		0,                              //没用非透明度缓存
		0,                              //忽略移位位
		0,                              //无累加缓存
		0, 0, 0, 0,                        //忽略累加位
		32,                             //32位深度缓存
		0,                              //无模版缓存
		0,                              //无辅助缓存
		PFD_MAIN_PLANE,                 //主层
		0,                              //保留
		0, 0, 0                          //忽略层，可见性和损毁掩模
	};
	int iPixelFormat;
	//为设备描述表得到最匹配的像素格式
	if ((iPixelFormat = ChoosePixelFormat(hdc, &pfd)) == 0)
	{
		MessageBox(TEXT("ChoosePixelFormat Failed"), NULL, MB_OK);
		return 0;
	}
	//设置最匹配的像素格式为当前的像素格式
	if (SetPixelFormat(hdc, iPixelFormat, &pfd) == FALSE)
	{
		MessageBox(TEXT("SetPixelFormat Failed"), NULL, MB_OK);
	}
	return 1;
}


void COpenGL::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	RenderScene();
	CWnd::OnTimer(nIDEvent);
}
