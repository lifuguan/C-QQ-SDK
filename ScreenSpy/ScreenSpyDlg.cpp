
// ScreenSpyDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ScreenSpy.h"
#include "ScreenSpyDlg.h"
#include "afxdialogex.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CScreenSpyDlg::CScreenSpyDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CScreenSpyDlg::IDD, pParent)
{
	//

	//*******************************************	//
	m_pngMask.LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_SC_MASK));
	m_pngDot.LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_SC_DOT));
	m_pngAction.LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_SC_ACTION));
	m_width=GetSystemMetrics(SM_CXSCREEN);
	m_height=GetSystemMetrics(SM_CYSCREEN);

	
    //初始化像皮筋类
	m_RectTracker.m_nStyle=CRectTracker::resizeInside|CRectTracker::dottedLine;  
	m_RectTracker.m_rect.SetRect(-1,-1,-1,-1);

    m_hCursor=AfxGetApp()->LoadCursor(ARROW_COLOR);      
	
	m_bDraw=FALSE;
	m_bStartDraw=FALSE;
	m_bShowInfo = FALSE;
    m_startPt=0;
    
	//获取屏幕分辩率
	m_xScreen = GetSystemMetrics(SM_CXSCREEN);
	m_yScreen = GetSystemMetrics(SM_CYSCREEN);

	//截取屏幕到位图中
	CRect rect(0, 0,m_xScreen,m_yScreen);
	m_pBitmap=CBitmap::FromHandle(CopyScreenToBitmap(&rect));
    m_backGround = m_pBitmap;
	//要更新的区域
	m_rgn.CreateRectRgn(0,0,50,50);

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CScreenSpyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CScreenSpyDlg, CDialog)
	//{{AFX_MSG_MAP(CScreenSpyDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	//ON_WM_ERASEBKGND()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScreenSpyDlg message handlers

BOOL CScreenSpyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//把对化框设置成全屏顶层窗口
	SetWindowPos(&wndTopMost,0,0,m_xScreen,m_yScreen,SWP_SHOWWINDOW);
	//捕获按键消息窗口,将对话框的句柄传递到CCatchScreenApp中
	((CScreenSpyApp*)AfxGetApp())->m_hwndDlg=m_hWnd;

	SetIcon(m_hIcon, TRUE);			
	SetIcon(m_hIcon, FALSE);		
	

	//////////////////////////////////////////////////////////////////////////
	m_curSel[0]=32646;//IDC_SIZEALL;
	m_curSel[1]=32642;//IDC_SIZENWSE;
	m_curSel[2]=32645;//IDC_SIZENS;
	m_curSel[3]=32643;//IDC_SIZENWSE;
	m_curSel[4]=32644;//IDC_SIZEWE;
	m_curSel[5]=32644;//IDC_SIZEWE;
	m_curSel[6]=32643;//IDC_SIZENWSE;
	m_curSel[7]=32645;//IDC_SIZENS;
	m_curSel[8]=32642;//IDC_SIZENWSE;

	m_iActionMask=0;

	m_bSelect=false;
	m_bSelected=false;
	m_bLeftDown=false;
	m_iCapture=0;

	for(int i=0;i<9;i++)
	{
		m_rcSel[i].left=0;
		m_rcSel[i].top=0;
		m_rcSel[i].right=0;
		m_rcSel[i].bottom=0;
	}
	//////////////////////////////////////////////////////////////////////////
	return TRUE; 
}

void CScreenSpyDlg::OnPaint() 
{
	
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
	
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{		
		CPaintDC dc(this);
		if(m_bShowInfo&&m_bStartDraw)
		{
			CRect rect;
			m_RectTracker.GetTrueRect(&rect);
			ShowInfo(rect);
		}
		//画出像皮筋矩形
		if(m_bStartDraw)
		{
			//m_RectTracker.Draw(&dc);
		}
		DrawFrame();
		//CDialog::OnPaint();
	}
	
}


/************************************************************************/
/* 
显示相关的信息  inRect为传进来的橡皮筋矩形
*/
/************************************************************************/
void CScreenSpyDlg::ShowInfo(CRect &inRect)
{
	//截取矩形大小信息离鼠标间隔
	const int space=3;
		
	CClientDC dc(this);
	//设置字体颜色大小
	CPoint pt;
	CPen pen(PS_SOLID,1,RGB(147,147,147));
	dc.SetTextColor(RGB(255,255,255));
	CFont cfont;
	cfont.CreatePointFont(90,"宋体");
	dc.SelectObject(&cfont);

	//得到字体宽度和高度
	GetCursorPos(&pt);
	dc.SetBkMode(TRANSPARENT);
	TEXTMETRIC tm;
	int charHeight;
	CSize size;
	int	lineLength;
	dc.GetTextMetrics(&tm);
	charHeight = tm.tmHeight+tm.tmExternalLeading + 10;
	size=dc.GetTextExtent("%d x %d",strlen(" %d x %d   "));
	lineLength=size.cx;

	//初始化矩形, 以保证写下一行文字  20*50
	CRect rect(inRect.left,inRect.top-20,inRect.left+70,inRect.top);

	int x = GetDeviceCaps(dc, HORZRES);
	int y = GetDeviceCaps(dc, VERTRES);

	//创建临时矩形
	CRect rectTemp;
	//当矩形到达桌面边缘时调整方向和大小
	if((pt.x+rect.Width())>=x)
	{
		//桌面上方显示不下矩形
		rectTemp=rect;
		rectTemp.left=rect.left-rect.Width()-space*2;
		rectTemp.right=rect.right-rect.Width()-space*2;;
		rect=rectTemp;
	}

	if((pt.y-rect.Height())<=0)
	{
		//桌面右方显示不下矩形
		rectTemp=rect;
		rectTemp.top=rect.top+rect.Height()+space*2;;
		rectTemp.bottom=rect.bottom+rect.Height()+space*2;;
		rect=rectTemp;

	}

	//创建灰色画刷画矩形
	dc.SelectObject((HBRUSH)GetStockObject(GRAY_BRUSH));
	dc.Rectangle(rect);
	rect.top+=2;
	//在矩形中显示文字	
	CString string;
	string.Format(" %d x %d",inRect.Width(),inRect.Height());
	dc.TextOut(rect.left,rect.top,string);
}

HCURSOR CScreenSpyDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CScreenSpyDlg::OnOK() 
{
	
}

void CScreenSpyDlg::OnCancel() 
{
	if(m_bStartDraw)
	{
		m_bStartDraw=FALSE;
		m_bDraw=FALSE;
		m_RectTracker.m_rect.SetRect(-1,-1,-1,-1);
		PaintWindow();
	}
	else
	{
		CDialog::OnCancel();
	}
}

void CScreenSpyDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	   if(m_bDraw)
	   {
			m_RectTracker.m_rect.SetRect(m_startPt.x+1,m_startPt.y+1,point.x,point.y);
			m_bStartDraw=TRUE;

			PaintWindow();
	   }


	   //////////////////////////////////////////////////////////////////////////
	   if(m_bSelect)
	   {
		   m_rcSel[0].right=point.x;
		   m_rcSel[0].bottom=point.y;
		   DrawFrame();
	   }
	   if(m_bSelected)
	   {
		   if(m_iCapture==0)
		   {
			   for(int i=8;i>=0;i--)
			   {
				   if(PtInRect(m_rcSel[i],point))
				   {
					   SetCursor(LoadCursor(NULL,MAKEINTRESOURCE(m_curSel[i])));
					   break;
				   }
			   }
			   int iOldMask=m_iActionMask;m_iActionMask=0;
			   if(PtInRect(m_rcAction[0],point)) m_iActionMask=0x001;
			   if(PtInRect(m_rcAction[1],point)) m_iActionMask=0x010;
			   if(PtInRect(m_rcAction[2],point)) m_iActionMask=0x100;
			   if(iOldMask!=m_iActionMask) DrawFrame();
		   }
		   else 
		   {
			   if(m_iCapture==9)
				   SetCursor(LoadCursor(NULL,MAKEINTRESOURCE(m_curSel[0])));
			   else
				   SetCursor(LoadCursor(NULL,MAKEINTRESOURCE(m_curSel[m_iCapture])));
			   int x=point.x-m_ptLButtonDown.x;
			   int y=point.y-m_ptLButtonDown.y;
			   switch(m_iCapture)
			   {
			   case 1:
				   m_rcSel[0].left+=x;
				   m_rcSel[0].top+=y;
				   break;
			   case 2:
				   m_rcSel[0].top+=y;
				   break;
			   case 3:
				   m_rcSel[0].top+=y;
				   m_rcSel[0].right+=x;
				   break;
			   case 4:
				   m_rcSel[0].left+=x;
				   break;
			   case 5:
				   m_rcSel[0].right+=x;
				   break;
			   case 6:
				   m_rcSel[0].left+=x;
				   m_rcSel[0].bottom+=y;
				   break;
			   case 7:
				   m_rcSel[0].bottom+=y;
				   break;
			   case 8:
				   m_rcSel[0].right+=x;
				   m_rcSel[0].bottom+=y;
				   break;
			   case 9:
				   m_rcSel[0].left+=x;
				   m_rcSel[0].right+=x;
				   m_rcSel[0].top+=y;
				   m_rcSel[0].bottom+=y;
				   break;
			   }
			   m_ptLButtonDown=point;
			   DrawFrame();
		   }	
	   }


	   //////////////////////////////////////////////////////////////////////////
	CDialog::OnMouseMove(nFlags, point);
}

void CScreenSpyDlg::saveToFile(CString strSaveFile)
{
	CString strExt=".png";
	if(strSaveFile=="")
	{
		CFileDialog fd(FALSE,"PNG","未命名",4|2,"PNG 图片格式 (*.png)|*.png|BMP 图片格式 (*.bmp)|*.bmp|JPG 图片格式 (*.jpg)|*.jpg||");
		if(fd.DoModal()==IDCANCEL)
			return;
		strExt=fd.GetFileExt();
		strExt.Trim();strExt.MakeLower();
		strSaveFile=fd.GetPathName();
	}



	//////////////////////////////////////////////////////////////////////////
	CDC *dc=GetDC();
	CDC BufferDC;
	BufferDC.CreateCompatibleDC(dc);
	CBitmap BufferBmp;
	BufferBmp.CreateCompatibleBitmap(dc,m_rcSel[0].Width(),m_rcSel[0].Height());
	BufferDC.SelectObject(&BufferBmp);
	CDC pdc;
	pdc.CreateCompatibleDC(&BufferDC);
	pdc.SelectObject(m_backGround);
	BufferDC.BitBlt(0,0,m_rcSel[0].Width(),m_rcSel[0].Height(),&pdc,m_rcSel[0].left,m_rcSel[0].top,SRCCOPY);
	pdc.DeleteDC();

	CImage image;
	image.Attach(BufferBmp);
	if(strExt=="bmp")
		image.Save(strSaveFile, Gdiplus::ImageFormatBMP);
	else if(strExt=="jpg")
		image.Save(strSaveFile, Gdiplus::ImageFormatJPEG);
	else
		image.Save(strSaveFile, Gdiplus::ImageFormatPNG);

	BufferDC.DeleteDC();
	BufferBmp.DeleteObject();
	ReleaseDC(dc);
	//////////////////////////////////////////////////////////////////////////

	m_rcSel[0].right=m_rcSel[0].left;
	m_rcSel[0].bottom=m_rcSel[0].top;
	m_bSelected=false;

	CDialog::OnCancel();

}

void CScreenSpyDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int nHitTest;
	nHitTest=m_RectTracker.HitTest(point);
	m_bShowInfo= TRUE;
// 	PaintWindow();
// 	    //判断击中位置
// 		if(nHitTest<0)          
// 		{
// 			if(!m_bStartDraw)
// 			{
// 				m_startPt=point;
// 				m_bDraw=TRUE;						
// 				PaintWindow();
// 			}
// 		}
// 		else
// 		{
// 			if(m_bStartDraw)
// 			{
// 				m_RectTracker.Track(this,point,TRUE);
// 				SendMessage(WM_LBUTTONUP,NULL,NULL);
// 				PaintWindow();
// 			}
// 		}

	//////////////////////////////////////////////////////////////////////////
	m_ptLButtonDown=point;
	m_bLeftDown=true;
	if(m_bSelected)
	{
		if(PtInRect(m_rcAction[0],point))
		{//设置为背景
			CopyScreenToBitmap(&m_rcSel[0],TRUE);
			char szCurDir[MAX_PATH]={0};
			GetCurrentDirectory(MAX_PATH,szCurDir);
			strcat(szCurDir,"\\WallPaper.bmp");
			saveToFile(szCurDir);
			SystemParametersInfo(SPI_SETDESKWALLPAPER,0,szCurDir,0);
			m_bLeftDown=false;
		}
		if(PtInRect(m_rcAction[1],point))
		{
			saveToFile();
			m_bLeftDown=false;
		}
		if (PtInRect(m_rcAction[2],point))
		{
			OnRButtonDown(NULL,NULL);
		}

		for(int i=8;i>=0;i--)
		{
			if(PtInRect(m_rcSel[i],point))
			{
				if(i==0)
					m_iCapture=9;
				else
					m_iCapture=i;
				SetCursor(LoadCursor(NULL,MAKEINTRESOURCE(m_curSel[i])));
				break;
			}
		}
	}
	else
	{
		m_bSelect=true;
		m_rcSel[0].left=point.x;
		m_rcSel[0].top=point.y;
		m_rcSel[0].right=point.x;
		m_rcSel[0].bottom=point.y;
		DrawFrame();
	}

	//////////////////////////////////////////////////////////////////////////
	CDialog::OnLButtonDown(nFlags, point);
}

void CScreenSpyDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_bDraw=FALSE;
	//m_bShowInfo = FALSE;	
	//PaintWindow();

	//显示工具栏
	//OnShowToolBar();

	//////////////////////////////////////////////////////////////////////////
	if(!m_bLeftDown)
	{
		CDialog::OnLButtonUp(nFlags, point);
		return;
	}
	m_bLeftDown=false;
	m_bSelect=false;
	m_bSelected=true;
	m_iCapture=0;

	int x=m_rcSel[0].left;
	int y=m_rcSel[0].top;
	int w=m_rcSel[0].Width();
	int h=m_rcSel[0].Height();
	if(x>m_rcSel[0].right)x=m_rcSel[0].right;
	if(y>m_rcSel[0].bottom)y=m_rcSel[0].bottom;
	if(w<0)w=-w;
	if(h<0)h=-h;
	m_rcSel[0].left=x;m_rcSel[0].top=y;
	m_rcSel[0].right=x+w;m_rcSel[0].bottom=y+h;

	DrawFrame();
	if(PtInRect(m_rcSel,point))
		SetCursor(LoadCursor(NULL,IDC_SIZEALL));
	CDialog::OnLButtonUp(nFlags, point);
	//////////////////////////////////////////////////////////////////////////
}

void CScreenSpyDlg::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	int nHitTest;
	nHitTest=m_RectTracker.HitTest(point);

	if(nHitTest==8)
	{
       CopyScreenToBitmap(m_RectTracker.m_rect,TRUE);
	   PostQuitMessage(0);
	}

	//////////////////////////////////////////////////////////////////////////
	if(PtInRect(m_rcSel[0],point))
	{
		CString strFilename;
		CopyScreenToBitmap(&m_rcSel[0],TRUE);
		OnRButtonDown(NULL,NULL);
		//strFilename.Format("%s%s.png",gDlg->m_strMyDocumentsPath,CTime::GetCurrentTime().Format("%Y%m%d%H%M%S"));
		// saveToFile(strFilename);
	}
	//////////////////////////////////////////////////////////////////////////
	CDialog::OnLButtonDblClk(nFlags, point);
}


//处理鼠标右键单击 .ESC. 
void CScreenSpyDlg::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if(m_bStartDraw)
	{
		//如果已经截取矩则清除截取矩形
		m_bStartDraw=FALSE;
		m_RectTracker.m_rect.SetRect(-1,-1,-1,-1);
		PaintWindow();
	}
	else
	{
		 PostQuitMessage(0);
	}
	CDialog::OnRButtonDown(nFlags, point);
}

BOOL CScreenSpyDlg::OnEraseBkgnd(CDC* pDC) 
{
	BITMAP bmp;
	m_pBitmap->GetBitmap(&bmp);

	CDC dcCompatible;
	dcCompatible.CreateCompatibleDC(pDC);
	dcCompatible.SelectObject(m_pBitmap);

	CRect rect;
	GetClientRect(&rect);
	pDC->BitBlt(0,0,rect.Width(),rect.Height(),&dcCompatible,0,0,SRCCOPY);

	return TRUE;
}

/************************************************************************/
/* 
加载工具栏并显示
*/
/************************************************************************/
void CScreenSpyDlg::OnShowToolBar()
{
	if (!m_ToolBar)
	{
		if (!m_ToolBar.CreateEx(this,TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_SIZE_FIXED|CBRS_LEFT|CBRS_TOOLTIPS) ||
			!m_ToolBar.LoadToolBar(IDR_TOOLBAR2))
		{
			TRACE("Failed to create toolbar!\n");
			return ;
		}		
	}
	m_ToolBar.ShowWindow(TRUE);

}
BOOL CScreenSpyDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (pWnd == this && m_RectTracker.SetCursor(this, nHitTest)) 
    {		
		return TRUE; 
	}
	else
	{
		//设置彩色光标
		SetCursor(m_hCursor);
		return TRUE;
	}
}


//*********************增加的函数**********************************************************
//拷贝屏幕到位图中
HBITMAP CScreenSpyDlg::CopyScreenToBitmap(LPRECT lpRect,BOOL bSave)
//lpRect 代表选定区域
{
	HDC       hScrDC, hMemDC;      
	// 屏幕和内存设备描述表
	HBITMAP    hBitmap, hOldBitmap;   
	// 位图句柄
	int       nX, nY, nX2, nY2;      
	// 选定区域坐标
	int       nWidth, nHeight;
	
	// 确保选定区域不为空矩形
	if (IsRectEmpty(lpRect))
		return NULL;
	//为屏幕创建设备描述表
	hScrDC = CreateDC("DISPLAY", NULL, NULL, NULL);

	//为屏幕设备描述表创建兼容的内存设备描述表
	hMemDC = CreateCompatibleDC(hScrDC);
	// 获得选定区域坐标
	nX = lpRect->left;
	nY = lpRect->top;
	nX2 = lpRect->right;
	nY2 = lpRect->bottom;

	//确保选定区域是可见的
	if (nX < 0)
		nX = 0;
	if (nY < 0)
		nY = 0;
	if (nX2 > m_xScreen)
		nX2 = m_xScreen;
	if (nY2 > m_yScreen)
		nY2 = m_yScreen;
	nWidth = nX2 - nX;
	nHeight = nY2 - nY;
	// 创建一个与屏幕设备描述表兼容的位图
	hBitmap = CreateCompatibleBitmap(hScrDC, nWidth, nHeight);
	// 把新位图选到内存设备描述表中
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
	// 把屏幕设备描述表拷贝到内存设备描述表中
	if(bSave)
	{
		CDC dcCompatible;
		dcCompatible.CreateCompatibleDC(CDC::FromHandle(hMemDC));
		dcCompatible.SelectObject(m_pBitmap);
        
		BitBlt(hMemDC, 0, 0, nWidth, nHeight,
			dcCompatible, nX, nY, SRCCOPY);
	}
	else
	{
		BitBlt(hMemDC, 0, 0, nWidth, nHeight,
			hScrDC, nX, nY, SRCCOPY);
	}

	hBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);
	
	
	//得到屏幕位图的句柄
	//*************
	CDC *pMemDc = CDC::FromHandle(hMemDC);

		//m_pngMask.DrawImage(pMemDc,0,0,m_xScreen,m_yScreen,0,0,8,8);
		int x=nX;
		int y=nY;
		int w=nWidth;
		int h=nHeight;

	m_pngMask.DrawImage(pMemDc,0,0,nWidth,y,0,0,8,8);
	m_pngMask.DrawImage(pMemDc,0,y+h,nWidth,nHeight-y-h,0,0,8,8);
	m_pngMask.DrawImage(pMemDc,0,y,x,h,0,0,8,8);
	m_pngMask.DrawImage(pMemDc,x+w,y,nWidth-x-w,h,0,0,8,8);

	pMemDc->Draw3dRect(x,y,w,h,RGB(0,0,0),RGB(0,0,0));
	//////////////////////////////////////////////////////////////////////////
	CRect rc; rc.left=x+2;rc.top=y-24;rc.right=x+268;rc.bottom=y-2;
	if(rc.top<0){rc.top=y+2;rc.bottom=y+24;}
	m_pngMask.DrawImage(pMemDc,rc.left,rc.top,rc.Width(),rc.Height(),0,0,8,8);
	
	//////////////////////////////////////////////////////////////////////////
	m_rcAction[0].left=x+w-232;m_rcAction[0].top=y+h+2;
	m_rcAction[0].right=m_rcAction[0].left+130;m_rcAction[0].bottom=m_rcAction[1].top+26;
	m_pngAction.DrawImage(pMemDc,m_rcAction[0].left,m_rcAction[0].top,130,26,0,(m_iActionMask & 0x00F)*26,130,26);

	m_rcAction[1].left=x+w-102;m_rcAction[1].top=y+h+2;
	m_rcAction[1].right=m_rcAction[1].left+51;m_rcAction[1].bottom=m_rcAction[1].top+26;
	m_pngAction.DrawImage(pMemDc,m_rcAction[1].left,m_rcAction[1].top,51,26,130,((m_iActionMask & 0x0F0)>4)*26,51,26);

	m_rcAction[2].left=x+w-51;m_rcAction[2].top=y+h+2;
	m_rcAction[2].right=m_rcAction[2].left+51;m_rcAction[2].bottom=m_rcAction[2].top+26;
	m_pngAction.DrawImage(pMemDc,m_rcAction[2].left,m_rcAction[2].top,51,26,181,((m_iActionMask & 0xF00)>8)*26,51,26);
	//////////////////////////////////////////////////////////////////////////
	x=m_rcSel[0].left;
	y=m_rcSel[0].top;
	w=m_rcSel[0].Width();
	h=m_rcSel[0].Height();


	m_pngDot.DrawImage(pMemDc,x-2,y-2);
	m_rcSel[1].left=x-2;m_rcSel[1].top=y-2;

	m_pngDot.DrawImage(pMemDc,x+w/2-2,y-2);
	m_rcSel[2].left=x+w/2-2;m_rcSel[2].top=y-2;

	m_pngDot.DrawImage(pMemDc,x+w-3,y-2);
	m_rcSel[3].left=x+w-3;m_rcSel[3].top=y-2;

	m_pngDot.DrawImage(pMemDc,x-2,y+h/2-2);
	m_rcSel[4].left=x-2;m_rcSel[4].top=y+h/2-2;

	m_pngDot.DrawImage(pMemDc,x+w-3,y+h/2-2);
	m_rcSel[5].left=x+w-3;m_rcSel[5].top=y+h/2-2;

	m_pngDot.DrawImage(pMemDc,x-2,y+h-3);
	m_rcSel[6].left=x-2;m_rcSel[6].top=y+h-3;

	m_pngDot.DrawImage(pMemDc,x+w/2-2,y+h-3);
	m_rcSel[7].left=x+w/2-2;m_rcSel[7].top=y+h-3;

	m_pngDot.DrawImage(pMemDc,x+w-3,y+h-3);
	m_rcSel[8].left=x+w-3;m_rcSel[8].top=y+h-3;

	for(int i=1;i<9;i++)
	{
	m_rcSel[i].right=m_rcSel[i].left+5;
	m_rcSel[i].bottom=m_rcSel[i].top+5;
	}

	//////////////////////////////////////////////////////////////////////////
	//绘画界面
	CDC * pDC = CDC::FromHandle(hScrDC);
	pDC->BitBlt(0,0,nWidth,nHeight,pMemDc,0,0,SRCCOPY);
	//////////////////////////////////////////////////////////////////////////
	///

	//清除 
	DeleteDC(hScrDC);
	DeleteDC(hMemDC);
	// 返回位图句柄
	if(bSave)
	{				
		if (OpenClipboard()) 
		{
        //清空剪贴板
        EmptyClipboard();
        //把屏幕内容粘贴到剪贴板上,
        //hBitmap 为刚才的屏幕位图句柄
        SetClipboardData(CF_BITMAP, hBitmap);
        //关闭剪贴板
        CloseClipboard();
      }
	}
	return hBitmap;
}



//重画窗口
void CScreenSpyDlg::PaintWindow()
{
	//获取当全屏对话框窗口大小
	CRect rect1;
	GetWindowRect(rect1);

	CRgn rgn1,rgn2;
	rgn1.CreateRectRgnIndirect(rect1);
	
	InvalidateRect(rect1);
}

void CScreenSpyDlg::DrawFrame(void)
{
	CDC *dc=GetDC();

	//////////////////////////////////////////////////////////////////////////
	CDC BufferDC;
	BufferDC.CreateCompatibleDC(dc);
	CBitmap BufferBmp;
	BufferBmp.CreateCompatibleBitmap(dc,m_width,m_height);
	BufferDC.SelectObject(&BufferBmp);

	//////////////////////////////////////////////////////////////////////////
	CDC pdc;
	pdc.CreateCompatibleDC(&BufferDC);
	pdc.SelectObject(m_backGround);
	BufferDC.BitBlt(0,0,m_width,m_height,&pdc,0,0,SRCCOPY);
	pdc.DeleteDC();
	//////////////////////////////////////////////////////////////////////////
	if(m_rcSel[0].Width()==0 || m_rcSel[0].Height()==0)
		m_pngMask.DrawImage(&BufferDC,0,0,m_width,m_height,0,0,8,8);
	else
	{
		int x=m_rcSel[0].left;
		int y=m_rcSel[0].top;
		int w=m_rcSel[0].Width();
		int h=m_rcSel[0].Height();
		if(x>m_rcSel[0].right)x=m_rcSel[0].right;
		if(y>m_rcSel[0].bottom)y=m_rcSel[0].bottom;
		if(w<0)w=-w;
		if(h<0)h=-h;

		m_pngMask.DrawImage(&BufferDC,0,0,m_width,y,0,0,8,8);
		m_pngMask.DrawImage(&BufferDC,0,y+h,m_width,m_height-y-h,0,0,8,8);
		m_pngMask.DrawImage(&BufferDC,0,y,x,h,0,0,8,8);
		m_pngMask.DrawImage(&BufferDC,x+w,y,m_width-x-w,h,0,0,8,8);

		BufferDC.Draw3dRect(x,y,w,h,RGB(0,0,0),RGB(0,0,0));
		//////////////////////////////////////////////////////////////////////////
		CRect rc; rc.left=x+2;rc.top=y-24;rc.right=x+268;rc.bottom=y-2;
		if(rc.top<0){rc.top=y+2;rc.bottom=y+24;}
		m_pngMask.DrawImage(&BufferDC,rc.left,rc.top,rc.Width(),rc.Height(),0,0,8,8);
		CFont font;
		font.CreateFont(-12,0,0,0,400,0,0,0,134,3,2,1,2,TEXT("宋体"));
		BufferDC.SelectObject((HFONT)font);
		BufferDC.SetBkMode(TRANSPARENT);
		BufferDC.SetTextColor(RGB(255,255,255));
		CString stxt;stxt.Format(" 起始位置: %4d.%4d  区域大小: %4d * %4d",x,y,w,h);
		BufferDC.DrawText(stxt,rc,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
		//////////////////////////////////////////////////////////////////////////
		m_rcAction[0].left=x+w-232;m_rcAction[0].top=y+h+2;
		m_rcAction[0].right=m_rcAction[0].left+130;m_rcAction[0].bottom=m_rcAction[1].top+26;
		m_pngAction.DrawImage(&BufferDC,m_rcAction[0].left,m_rcAction[0].top,130,26,0,(m_iActionMask & 0x00F)*26,130,26);

		m_rcAction[1].left=x+w-102;m_rcAction[1].top=y+h+2;
		m_rcAction[1].right=m_rcAction[1].left+51;m_rcAction[1].bottom=m_rcAction[1].top+26;
		m_pngAction.DrawImage(&BufferDC,m_rcAction[1].left,m_rcAction[1].top,51,26,130,((m_iActionMask & 0x0F0)>4)*26,51,26);

		m_rcAction[2].left=x+w-51;m_rcAction[2].top=y+h+2;
		m_rcAction[2].right=m_rcAction[2].left+51;m_rcAction[2].bottom=m_rcAction[2].top+26;
		m_pngAction.DrawImage(&BufferDC,m_rcAction[2].left,m_rcAction[2].top,51,26,181,((m_iActionMask & 0xF00)>8)*26,51,26);
		//////////////////////////////////////////////////////////////////////////
		x=m_rcSel[0].left;
		y=m_rcSel[0].top;
		w=m_rcSel[0].Width();
		h=m_rcSel[0].Height();


		m_pngDot.DrawImage(&BufferDC,x-2,y-2);
		m_rcSel[1].left=x-2;m_rcSel[1].top=y-2;

		m_pngDot.DrawImage(&BufferDC,x+w/2-2,y-2);
		m_rcSel[2].left=x+w/2-2;m_rcSel[2].top=y-2;

		m_pngDot.DrawImage(&BufferDC,x+w-3,y-2);
		m_rcSel[3].left=x+w-3;m_rcSel[3].top=y-2;

		m_pngDot.DrawImage(&BufferDC,x-2,y+h/2-2);
		m_rcSel[4].left=x-2;m_rcSel[4].top=y+h/2-2;

		m_pngDot.DrawImage(&BufferDC,x+w-3,y+h/2-2);
		m_rcSel[5].left=x+w-3;m_rcSel[5].top=y+h/2-2;

		m_pngDot.DrawImage(&BufferDC,x-2,y+h-3);
		m_rcSel[6].left=x-2;m_rcSel[6].top=y+h-3;

		m_pngDot.DrawImage(&BufferDC,x+w/2-2,y+h-3);
		m_rcSel[7].left=x+w/2-2;m_rcSel[7].top=y+h-3;

		m_pngDot.DrawImage(&BufferDC,x+w-3,y+h-3);
		m_rcSel[8].left=x+w-3;m_rcSel[8].top=y+h-3;

		for(int i=1;i<9;i++)
		{
			m_rcSel[i].right=m_rcSel[i].left+5;
			m_rcSel[i].bottom=m_rcSel[i].top+5;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//绘画界面
	dc->BitBlt(0,0,m_width,m_height,&BufferDC,0,0,SRCCOPY);
	//////////////////////////////////////////////////////////////////////////
	//清理资源
	BufferDC.DeleteDC();
	BufferBmp.DeleteObject();
	ReleaseDC(dc);
}

BOOL CScreenSpyDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message==WM_KEYDOWN)
	{
		if(pMsg->wParam==VK_ESCAPE && m_bSelected)
		{
			if(m_bSelected)
			{
				OnRButtonDown(NULL,NULL);
				return TRUE;
			}
			else
			{
				CDialog::OnCancel();
				return TRUE;
			}
		}	
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}
