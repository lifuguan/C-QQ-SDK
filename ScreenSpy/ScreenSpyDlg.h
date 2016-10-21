
// ScreenSpyDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "resource.h"
#include "CRectTrackerInheritance.h"

#include "PngImage.h"
// CScreenSpyDlg 对话框
class CScreenSpyDlg : public CDialogEx
{
// 构造
public:
	CScreenSpyDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SCREENSPY_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	/////////////////////////////////////////////////////////////////////////////////
	CPngImage2	m_pngMask;
	CPngImage2	m_pngDot;
	CPngImage2	m_pngAction;
	CBitmap		*m_backGround;
	int			m_iCapture;
	bool		m_bLeftDown;
	CPoint		m_ptLButtonDown;
	CRect		m_rcSel[9];
	UINT		m_curSel[9];

	int			m_iActionMask;
	CRect		m_rcAction[3];
	bool		m_bSelect;
	bool		m_bSelected;

	int			m_width;
	int			m_height;
	void DrawFrame(void);

	void saveToFile(CString strSaveFile="");
	/////////////////////////////////////////////////////////////////////////////
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	void	ShowInfo(CRect &inRect);
	HBITMAP CopyScreenToBitmap(LPRECT lpRect,BOOL bSave =FALSE );
	void    OnShowToolBar();
		
		
	DECLARE_MESSAGE_MAP()
public:
	
	CRectTrackerInheritance m_RectTracker; //橡皮筋类的派生类
	int		m_xScreen;
	int		m_yScreen;  //屏幕的分辨率
	CBitmap* m_pBitmap;	//背景位图
	HCURSOR m_hCursor;//光标
	CRect   m_RtClient;//客户区
	CRgn	m_rgn;//要更新的区域
	CPoint  m_startPt;//截取矩形左上角
	BOOL    m_bDraw;//是否为截取状态
	BOOL    m_bStartDraw;//是否可以开始绘制橡皮筋
	BOOL	m_bShowInfo;//是否显示相关信息
	CToolBar m_ToolBar;//工具栏

	void	PaintWindow();//重绘窗口

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnOK();
	afx_msg void OnCancel();

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
