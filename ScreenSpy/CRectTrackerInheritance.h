
/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
橡皮筋类的派生类
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/

#ifndef  _CRECTTRACKERINHERITANCE_H_
#define  _CRECTTRACKERINHERITANCE_H_

class CRectTrackerInheritance : public CRectTracker
{
public:
	CRectTrackerInheritance();
	~CRectTrackerInheritance();
protected:
	void DrawTrackerRect( LPCRECT lpRect, CWnd* pWndClipTo,CDC* pDC, CWnd* pWnd );
private:
};

#endif

