#if !defined(AFX_CONNECTNODEDLG_H__CFB1B03F_0AA1_4607_AA96_A116B38E5495__INCLUDED_)
#define AFX_CONNECTNODEDLG_H__CFB1B03F_0AA1_4607_AA96_A116B38E5495__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConnectNodeDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConnectNodeDlg dialog
class CDrawDoc;
class DrawView;
class CConnectNodeDlg : public CDialog
{
// Construction
public:
	void GoToConnect( void );
	int m_nShowType;//0������ʾ 1��ʾGoTo�Ľڵ���
	CGoToObj *m_pGoToObj;
	void InitGoToDlg(CDrawDoc *pDoc, CGoToObj *pGoToObj, CDrawView *pView , int nShowMode );
	void InitBlockMember( HTREEITEM hParentItem ,CBlockObj *pObj );
	void InitGoToTree( void );
	void InitNodeTree( void );
	void InitDlg(CDrawDoc *pDoc, CNextNode * pNextNodeSelected, CDrawView *pView , int nShowMode );
	CConnectNodeDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConnectNodeDlg)
	enum { IDD = IDD_DIALOG_CONNECT };
	CTreeCtrl	m_nodeTree;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConnectNodeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConnectNodeDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkTreeNodelist(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CNextNode * m_pNextNodeSelected;
	CImageList m_ImageList;
	CDrawDoc * m_pDoc;
	int m_nShowMode;//��ʾ����ģʽ��0����ʾ����ű��ڵ㣻1����ʾchildblock�����ݣ�2����ʾEndCallBlock������
	CDrawView *m_pView;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONNECTNODEDLG_H__CFB1B03F_0AA1_4607_AA96_A116B38E5495__INCLUDED_)
