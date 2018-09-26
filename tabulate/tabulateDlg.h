
// tabulateDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CtabulateDlg dialog
class CtabulateDlg : public CDialogEx
{
// Construction
public:
	CtabulateDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TABULATE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CButton InsertFile;
	CButton RemoveFile;
	CButton SelectDataDir;
	CEdit PSEnergyTolerance;
	CStatic OutputDescription;
//	CListCtrl ControlDirListing;
	CListCtrl LibraryContent;
	CListCtrl LibDirContent;
	CListCtrl DataDirContent;
	afx_msg void OnLvnColumnclickListNlbFilelisting(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHdnItemclickListNlbFilelisting(NMHDR *pNMHDR, LRESULT *pResult);
};
