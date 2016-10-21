
// ScreenSpy.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "ScreenSpy.h"
#include "ScreenSpyDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CScreenSpyApp

BEGIN_MESSAGE_MAP(CScreenSpyApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CScreenSpyApp 构造

CScreenSpyApp::CScreenSpyApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
	m_hwndDlg = NULL;
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CScreenSpyApp 对象

CScreenSpyApp theApp;


// CScreenSpyApp 初始化

BOOL CScreenSpyApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	GdiplusStartupInput gdi;  
	GdiplusStartup(&m_gdiToken,&gdi,NULL);   

	AfxEnableControlContainer();

	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	CScreenSpyDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}

	// 删除上面创建的 shell 管理器。
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}


#define SHIFTED 0x8000 
BOOL CScreenSpyApp::ProcessMessageFilter(int code, LPMSG lpMsg)
{
	if ( NULL!= m_hwndDlg)
	{
		if((lpMsg->hwnd==m_hwndDlg) || ::IsChild(m_hwndDlg,lpMsg->hwnd))
		{
			//如果消息是WM_KEYDOWN
			//用方向键调整位置
			if(lpMsg->message==WM_KEYDOWN)
			{
				CRect rect(0,0,0,0);
				CScreenSpyDlg * pDlg=(CScreenSpyDlg *)AfxGetMainWnd();
				rect=pDlg->m_RectTracker.m_rect;

				//如果Shift键按下则方向键调整大小
				BOOL isShifeDowm=FALSE;
				int nVirtKey;
				nVirtKey = GetKeyState(VK_SHIFT); 
				if (nVirtKey & SHIFTED) 
					isShifeDowm=TRUE;

				switch(lpMsg->wParam)
				{
				case VK_UP:
					//如果按下Shift,则只调整一边
					if(!isShifeDowm)
						rect.top-=1;
					rect.bottom-=1;
					pDlg->m_RectTracker.m_rect=rect;
					//pDlg->PaintWindow();
					break;
				case VK_DOWN:
					rect.top+=1;
					if(!isShifeDowm)
						rect.bottom+=1;
					pDlg->m_RectTracker.m_rect=rect;
					//pDlg->PaintWindow();
					break;
				case VK_LEFT:
					if(!isShifeDowm)
						rect.left-=1;
					rect.right-=1;
					pDlg->m_RectTracker.m_rect=rect;
					//pDlg->PaintWindow();
					break;
				case VK_RIGHT:
					rect.left+=1;
					if(!isShifeDowm)
						rect.right+=1;
					pDlg->m_RectTracker.m_rect=rect;
					//pDlg->PaintWindow();
					break;
				}
			}
		}
	}

	return CWinApp::ProcessMessageFilter(code, lpMsg);
}
