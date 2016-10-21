
// ScreenSpy.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CScreenSpyApp:
// 有关此类的实现，请参阅 ScreenSpy.cpp
//

class CScreenSpyApp : public CWinApp
{
public:
	CScreenSpyApp();

// 重写
public:
	HWND m_hwndDlg;//传递命令的Dialog
	virtual BOOL InitInstance();
	ULONG_PTR m_gdiToken;  
// 实现

	DECLARE_MESSAGE_MAP()
	virtual BOOL ProcessMessageFilter(int code, LPMSG lpMsg);
};

extern CScreenSpyApp theApp;