#include "stdafx.h"
#include "CRectTrackerInheritance.h"


CRectTrackerInheritance::CRectTrackerInheritance()
{

}

CRectTrackerInheritance::~CRectTrackerInheritance()
{
}

#include "ScreenSpyDlg.h"
void CRectTrackerInheritance::DrawTrackerRect( LPCRECT lpRect, CWnd* pWndClipTo,CDC* pDC, CWnd* pWnd )
{	    
	//更新窗口,消除TRACK时的虚线框
	((CScreenSpyDlg *)pWnd)->PaintWindow();
	((CScreenSpyDlg *)pWnd)->SendMessage(WM_MOUSEMOVE);		
}

