
// InjectionDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "Injection.h"
#include "InjectionDlg.h"
#include "afxdialogex.h"

#include <TlHelp32.h>  

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CInjectionDlg 对话框



CInjectionDlg::CInjectionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_INJECTION_DIALOG, pParent)
	, m_strshuoming(_T(""))
	, m_dwPid(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CInjectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SHUOMING, m_strshuoming);
	DDX_Text(pDX, IDC_PID, m_dwPid);
}

BEGIN_MESSAGE_MAP(CInjectionDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(ID_ZHURU, &CInjectionDlg::OnBnClickedZhuru)
END_MESSAGE_MAP()


// CInjectionDlg 消息处理程序

BOOL CInjectionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

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

	// TODO: 在此添加额外的初始化代码

	HWND hwnd = ::FindWindow(L"扫雷", L"扫雷");
	if (hwnd==NULL)
	{
		m_strshuoming.Format(L"未找到扫雷进程，请手动输入PID！");
	}
	else
	{
		m_strshuoming.Format(L"已经找到扫雷进程，请开始注入。");
		GetWindowThreadProcessId(hwnd, &m_dwPid);
	}


	UpdateData(FALSE);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CInjectionDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CInjectionDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CInjectionDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CInjectionDlg::OnBnClickedZhuru()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	HANDLE		hProcess;
	TCHAR		DllPath[MAX_PATH] = { 0 };
	PVOID		lpDllBuf;
	HANDLE		hReThread;
	HMODULE		hMod;


	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_dwPid);

	GetCurrentDirectory(MAX_PATH, DllPath);
	StrCatBuff(DllPath, L"\\辅助dll.dll", MAX_PATH);

	lpDllBuf = VirtualAllocEx(hProcess, NULL, 4096, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	if (lpDllBuf==NULL)
	{
		MessageBox(L"分配内存失败", L"错误", MB_OK);
	}

	WriteProcessMemory(hProcess, lpDllBuf, DllPath, 4096, NULL);

	hMod = GetModuleHandle(L"kernel32.dll");
	FARPROC pThreadProc = GetProcAddress(hMod, "LoadLibraryW");

	hReThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pThreadProc, lpDllBuf, 0, NULL);

	if (hReThread!=NULL)
	{
		MessageBox(L"注入成功", L"提示", MB_OK);
	}
	else
	{
		MessageBox(L"注入失败", L"错误", MB_OK);
	}

	WaitForSingleObject(hReThread, INFINITE);
	VirtualFreeEx(hProcess, lpDllBuf, 4096, MEM_DECOMMIT);
	CloseHandle(hReThread);
	CloseHandle(hProcess);
	return;
}

