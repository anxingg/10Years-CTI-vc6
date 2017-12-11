// ObjSelectTool.h: interface for the CObjSelectTool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJSELECTTOOL_H__BB46A246_DA62_42BB_BE64_A71681B95237__INCLUDED_)
#define AFX_OBJSELECTTOOL_H__BB46A246_DA62_42BB_BE64_A71681B95237__INCLUDED_



#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000




class CDrawView;

class CObjSelectTool  
{
public:
	CObjSelectTool();
	virtual ~CObjSelectTool();

public:
	void OnMouseMove(CDrawView* pView, UINT nFlags, const CPoint& point);
	void OnLButtonUp(CDrawView* pView, UINT nFlags, const CPoint& point);
	void OnLButtonDown(CDrawView* pView, UINT nFlags, const CPoint& point);
	void OnLButtonDblClk(CDrawView* pView, UINT nFlags, const CPoint& point);
	
public:
	void OnMouseMove( CDrawView *pView , CPoint point, CDC *pDc);
private:	
	BOOL			m_bDown;//���������±�־
	CPoint			m_last;//����ƶ���ʱ������
	CPoint			m_down;//����������ʱ����


};

#endif // !defined(AFX_OBJSELECTTOOL_H__BB46A246_DA62_42BB_BE64_A71681B95237__INCLUDED_)
