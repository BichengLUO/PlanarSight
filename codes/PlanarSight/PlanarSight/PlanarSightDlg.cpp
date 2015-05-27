
// PlanarSightDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PlanarSight.h"
#include "PlanarSightDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CPlanarSightDlg 对话框



CPlanarSightDlg::CPlanarSightDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPlanarSightDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pDisplay = new COpenGL;

#ifdef _DEBUG
	AllocConsole();
	SetConsoleTitle(_T("Debug Output"));
	freopen("CONOUT$", "w", stdout);
#endif
}

void CPlanarSightDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_VISI_POLYGON, vis_pol_check);
	DDX_Control(pDX, IDC_SHOW_TRIANGULATION, showTriangulation);
}

BEGIN_MESSAGE_MAP(CPlanarSightDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_OUTER_WALLS, &CPlanarSightDlg::OnBnClickedOuterWalls)
	ON_BN_CLICKED(IDC_INNER_WALLS, &CPlanarSightDlg::OnBnClickedInnerWalls)
	ON_BN_CLICKED(IDC_MONSTERS, &CPlanarSightDlg::OnBnClickedMonsters)
	ON_BN_CLICKED(IDC_CLEAR, &CPlanarSightDlg::OnBnClickedClear)
	ON_BN_CLICKED(IDC_START_GAME, &CPlanarSightDlg::OnBnClickedStartGame)
	ON_BN_CLICKED(IDC_END_GAME, &CPlanarSightDlg::OnBnClickedEndGame)
	ON_BN_CLICKED(IDC_VISI_POLYGON, &CPlanarSightDlg::OnClickedVisiPolygon)
//	ON_WM_KEYDOWN()
ON_WM_KEYDOWN()
ON_WM_KEYUP()
ON_BN_CLICKED(IDC_SHOW_TRIANGULATION, &CPlanarSightDlg::OnBnClickedShowTriangulation)
END_MESSAGE_MAP()


// CPlanarSightDlg 消息处理程序

BOOL CPlanarSightDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	m_pDisplay->Create(NULL,
		NULL,
		WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE,
		m_pDisplay->frame,
		this,
		0);

	m_pDisplay->outterWallBtn = GetDlgItem(IDC_OUTER_WALLS);
	m_pDisplay->innerWallBtn = GetDlgItem(IDC_INNER_WALLS);
	m_pDisplay->monsterBtn = GetDlgItem(IDC_MONSTERS);
	m_pDisplay->startBtn = GetDlgItem(IDC_START_GAME);
	m_pDisplay->endBtn = GetDlgItem(IDC_END_GAME);


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CPlanarSightDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CPlanarSightDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CPlanarSightDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CPlanarSightDlg::OnBnClickedOuterWalls()
{
	// TODO:  在此添加控件通知处理程序代码
	m_pDisplay->rendering->drawOuterWall = true;
	GetDlgItem(IDC_OUTER_WALLS)->EnableWindow(false);
	//GetDlgItem(IDC_INNER_WALLS)->EnableWindow(false);
	//GetDlgItem(IDC_MONSTERS)->EnableWindow(false);
}


void CPlanarSightDlg::OnBnClickedInnerWalls()
{
	// TODO:  在此添加控件通知处理程序代码
	m_pDisplay->rendering->drawInnerWall = true;
	//GetDlgItem(IDC_OUTER_WALLS)->EnableWindow(false);
	GetDlgItem(IDC_INNER_WALLS)->EnableWindow(false);
	GetDlgItem(IDC_MONSTERS)->EnableWindow(false);
	GetDlgItem(IDC_START_GAME)->EnableWindow(false);
}


void CPlanarSightDlg::OnBnClickedMonsters()
{
	// TODO:  在此添加控件通知处理程序代码
	m_pDisplay->rendering->drawMonster = true;
	//GetDlgItem(IDC_OUTER_WALLS)->EnableWindow(false);
	GetDlgItem(IDC_INNER_WALLS)->EnableWindow(false);
	GetDlgItem(IDC_MONSTERS)->EnableWindow(false);
	GetDlgItem(IDC_START_GAME)->EnableWindow(false);
}


void CPlanarSightDlg::OnBnClickedClear()
{
	// TODO:  在此添加控件通知处理程序代码
	m_pDisplay->rendering->clear();
	GetDlgItem(IDC_OUTER_WALLS)->EnableWindow(true);
	GetDlgItem(IDC_INNER_WALLS)->EnableWindow(false);
	GetDlgItem(IDC_MONSTERS)->EnableWindow(false);
	GetDlgItem(IDC_START_GAME)->EnableWindow(true);
	GetDlgItem(IDC_END_GAME)->EnableWindow(false);
	m_pDisplay->rendering->gameStart = false;
}


void CPlanarSightDlg::OnBnClickedStartGame()
{
	// TODO:  在此添加控件通知处理程序代码
	GetDlgItem(IDC_INNER_WALLS)->EnableWindow(false);
	GetDlgItem(IDC_MONSTERS)->EnableWindow(false);
	GetDlgItem(IDC_START_GAME)->EnableWindow(false);
	GetDlgItem(IDC_END_GAME)->EnableWindow(true);
	m_pDisplay->SetActiveWindow();
	m_pDisplay->rendering->gameStart = true;
	if (!m_pDisplay->rendering->preprocessFinished)
		m_pDisplay->rendering->preprocess();
}


void CPlanarSightDlg::OnBnClickedEndGame()
{
	// TODO:  在此添加控件通知处理程序代码
	GetDlgItem(IDC_INNER_WALLS)->EnableWindow(true);
	GetDlgItem(IDC_MONSTERS)->EnableWindow(true);
	GetDlgItem(IDC_START_GAME)->EnableWindow(true);
	GetDlgItem(IDC_END_GAME)->EnableWindow(false);
	m_pDisplay->rendering->gameStart = false;
	m_pDisplay->rendering->preprocessFinished = false;
}


void CPlanarSightDlg::OnClickedVisiPolygon()
{
	// TODO:  在此添加控件通知处理程序代码
	if (vis_pol_check.GetCheck() == 0)
		m_pDisplay->rendering->showVisPolygon = false;
	else
		m_pDisplay->rendering->showVisPolygon = true;
}

BOOL CPlanarSightDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  在此添加专用代码和/或调用基类

	//判断是否为键盘消息
	//cout << pMsg->message << endl;
	//if (pMsg->message == WM_KEYDOWN)
	if (WM_KEYFIRST <= pMsg->message && pMsg->message <= WM_KEYLAST)
	{
		/*if (m_pDisplay->rendering->gameStart && pMsg->wParam >= VK_LEFT && pMsg->wParam <= VK_DOWN)
		{
			for (unsigned i = VK_LEFT; i <= VK_DOWN; i++)
				if (GetKeyState(i) < 0)
					m_pDisplay->rendering->playerWalk(i);
		}	*/

		if (m_pDisplay->rendering->gameStart)
		{
			if (GetKeyState('A') < 0)
				m_pDisplay->rendering->playerWalk('A');
			if (GetKeyState('W') < 0)
				m_pDisplay->rendering->playerWalk('W');
			if (GetKeyState('D') < 0)
				m_pDisplay->rendering->playerWalk('D');
			if (GetKeyState('S') < 0)
				m_pDisplay->rendering->playerWalk('S');
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CPlanarSightDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	MessageBox(_T("key"), NULL, MB_OK);

	CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CPlanarSightDlg::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	MessageBox(_T("key"), NULL, MB_OK);
	CDialogEx::OnKeyUp(nChar, nRepCnt, nFlags);
}


void CPlanarSightDlg::OnBnClickedShowTriangulation()
{
	// TODO:  在此添加控件通知处理程序代码
	if (showTriangulation.GetCheck() == 0)
		m_pDisplay->rendering->showTriangulation = false;
	else
		m_pDisplay->rendering->showTriangulation = true;
}
