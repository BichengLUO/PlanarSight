// OpenGL.cpp : 实现文件
//

#include "stdafx.h"
#include "PlanarSight.h"
#include "OpenGL.h"

GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat mat_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
GLfloat mat_diffuse[] = { 0.7, 0.5, 0.5, 1.0 };
GLfloat mat_specular[] = { 0.5, 0.5, 0.5, 1.0 };
GLfloat no_shininess[] = { 0.0 };
GLfloat low_shininess[] = { 5.0 };
GLfloat hig_shininess[] = { 50.0 };
GLfloat mat_emission[] = { 1.0, 0.8, 0.0, 0.2 };

int wood_tex_id;
int wood2_tex_id;
int floor_tex_id;
int visp_tex_id;

// COpenGL

IMPLEMENT_DYNAMIC(COpenGL, CWnd)

COpenGL::COpenGL()
{
	rendering = new Rendering;
	frame = CRect(10, 10, 750, 630);
	outterWallBtn = NULL;
	innerWallBtn = NULL;
	monsterBtn = NULL;
	startBtn = NULL;
	endBtn = NULL;
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
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
//	ON_WM_KEYDOWN()
//	ON_WM_SYSKEYDOWN()
ON_WM_MOUSEMOVE()
ON_WM_LBUTTONUP()
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

	wood_tex_id = loadTex(_T("wood.png"));
	wood2_tex_id = loadTex(_T("wood2.png"));
	floor_tex_id = loadTex(_T("floor.png"));
	visp_tex_id = loadTexAlpha(_T("visp.png"));
	SetUp2D();

	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	SetTimer(1, 10, NULL);
	return 0;
}

void COpenGL::SetUp2D()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, frame.Width(), 0, frame.Height(), -50, 50);

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);

	glDisable(GL_DEPTH_TEST);
}

void COpenGL::SetUp3D()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, frame.Width() / (GLfloat)frame.Height(), 0.1f, 1000);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	gluLookAt(frame.Width() / 2.0, -50, 300, frame.Width() / 2.0, frame.Height() / 2.0, 0, 0, 1, 0);

	setup_light();
	setup_fog();
	glEnable(GL_DEPTH_TEST);
}

void COpenGL::setup_light()
{
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_position[] = { 300, 300, 200, 0 };
	GLfloat light_specular[] = { 0.7, 0.7, 0.7, 1.0 };
	GLfloat light_ambient[] = { 0.5, 0.5, 0.5, 1.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_SPECULAR, no_mat);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}

void COpenGL::setup_material()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
	glMaterialfv(GL_FRONT, GL_SHININESS, no_shininess);
	glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
}

void COpenGL::setup_fog()
{
	GLfloat fogColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glFogi(GL_FOG_MODE, GL_LINEAR);        // Fog Mode
	glFogfv(GL_FOG_COLOR, fogColor);            // Set Fog Color
	glFogf(GL_FOG_DENSITY, 0.05f);              // How Dense Will The Fog Be
	glHint(GL_FOG_HINT, GL_NICEST);          // Fog Hint Value
	glFogf(GL_FOG_START, 300.0f);             // Fog Start Depth
	glFogf(GL_FOG_END, 1000.0f);               // Fog End Depth
	glEnable(GL_FOG);                   // Enables GL_FOG
}


void COpenGL::RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (rendering->show3DView)
	{
		setup_material();
		glMatrixMode(GL_MODELVIEW);
		glTranslatef(frame.Width() / 2.0, frame.Height() / 2.0, 0);
		glRotated(0.1, 0, 0, 1);
		glTranslatef(-frame.Width() / 2.0, -frame.Height() / 2.0, 0);
	}	
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
	if (rendering->gameOver)
	{
		rendering->gameOver = false;
		MessageBox(TEXT("Game Over"), NULL, MB_OK);
		rendering->gameStart = true;
	}
	CWnd::OnTimer(nIDEvent);
}


void COpenGL::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (!rendering->gameStart)
	{
		if (rendering->drawMonster)
		{
			rendering->drawMonster = false;
		}
		else
		{
			bool flag = rendering->loopFinished();
			if (rendering->drawInnerWall)
			{
				rendering->drawInnerWall = false;
				if (!flag)
					MessageBox(_T("Illegal inner wall"), NULL, MB_OK);
			}
			else if (rendering->drawOuterWall)
			{
				rendering->drawOuterWall = false;
				if (!flag)
					MessageBox(_T("Self intersected outter wall!"), NULL, MB_OK);
			}
		}

		if (rendering->basePolygon->loopArray.size() == 0)
			outterWallBtn->EnableWindow(true);
		else
		{
			innerWallBtn->EnableWindow(true);
			monsterBtn->EnableWindow(true);
			startBtn->EnableWindow(true);
		}
	}

	CWnd::OnRButtonDown(nFlags, point);
}


void COpenGL::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	Point p;
	p.x = point.x;
	p.y = frame.Height() - point.y;

	if (rendering->gameStart)
	{
		if ((p - rendering->player.pos).length() < 7)
			rendering->moving = true;
	}
	else
	{
		if (rendering->drawOuterWall || rendering->drawInnerWall)
		{
			if (!rendering->addPointIntoLoopBuf(p))
				MessageBox(_T("Illagal point or segment"), NULL, MB_OK);
		}
		else if (rendering->drawMonster)
		{
			if (rendering->basePolygon->loopArray.size() == 0)
			{
				MessageBox(_T("Please draw a maze first!"), NULL, MB_OK);
				outterWallBtn->EnableWindow(true);
				innerWallBtn->EnableWindow(true);
				monsterBtn->EnableWindow(true);
			}
			else if (!rendering->addMonster(p))
				MessageBox(_T("The monster is out of the maze!"), NULL, MB_OK);
		}
	}
	
	CWnd::OnLButtonDown(nFlags, point);
}


void COpenGL::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (rendering->moving)
	{
		Point p;
		p.x = point.x;
		p.y = frame.Height() - point.y;
		if (!rendering->playerMoveTo(p))
			rendering->moving = false;
	}
	
	CWnd::OnMouseMove(nFlags, point);
}


void COpenGL::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (rendering->gameStart && rendering->moving)
	{
		Point p;
		p.x = point.x;
		p.y = frame.Height() - point.y;
		rendering->playerMoveTo(p);
		rendering->moving = false;		
	}
	CWnd::OnLButtonUp(nFlags, point);
}

GLuint COpenGL::loadTex(LPCTSTR file_addr)
{
	BITMAP bm;
	GLuint id = 0;
	CImage img;
	HRESULT hr = img.Load(file_addr);
	if (!SUCCEEDED(hr))
	{
		std::cout << "Image Loading Failed!\n";
		return NULL;
	}
	if (!GetObject(img.Detach(), sizeof(bm), &bm))
	{
		std::cout << "Bitmap Loading Failed!\n";
		return NULL;
	}
	for (int i = 0; i < bm.bmWidth * bm.bmHeight; i++)
	{
		((byte*)bm.bmBits)[3 * i] = 255 - ((byte*)bm.bmBits)[3 * i];
		((byte*)bm.bmBits)[3 * i + 1] = 255 - ((byte*)bm.bmBits)[3 * i + 1];
		((byte*)bm.bmBits)[3 * i + 2] = 255 - ((byte*)bm.bmBits)[3 * i + 2];
	}
	glGenTextures(1, &id);
	if (id)
	{
		glBindTexture(GL_TEXTURE_2D, id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//glTexImage2D(GL_TEXTURE_2D, 0, 3, bm.bmWidth, bm.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, bm.bmBits);
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, bm.bmWidth, bm.bmHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, bm.bmBits);
	}
	return id;
}

GLuint COpenGL::loadTexAlpha(LPCTSTR file_addr)
{
	BITMAP bm;
	GLuint id = 0;
	CImage img;
	HRESULT hr = img.Load(file_addr);
	if (!SUCCEEDED(hr))
	{
		std::cout << "Image Loading Failed!\n";
		return NULL;
	}
	if (!GetObject(img.Detach(), sizeof(bm), &bm))
	{
		std::cout << "Bitmap Loading Failed!\n";
		return NULL;
	}
	for (int i = 0; i < bm.bmWidth * bm.bmHeight; i++)
	{
		((byte*)bm.bmBits)[4 * i] = 255 - ((byte*)bm.bmBits)[4 * i];
		((byte*)bm.bmBits)[4 * i + 1] = 255 - ((byte*)bm.bmBits)[4 * i + 1];
		((byte*)bm.bmBits)[4 * i + 2] = 255 - ((byte*)bm.bmBits)[4 * i + 2];
	}
	glGenTextures(1, &id);
	if (id)
	{
		glBindTexture(GL_TEXTURE_2D, id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		gluBuild2DMipmaps(GL_TEXTURE_2D, 4, bm.bmWidth, bm.bmHeight, GL_BGRA_EXT, GL_UNSIGNED_BYTE, bm.bmBits);
	}
	return id;
}