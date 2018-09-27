
// tabulateDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "ProgramDefaults.h"
#include <vector>

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
	CButton OutputOption;
	CEdit PSEnergyTolerance;
	CStatic OutputDescription;
	CListCtrl LibraryContent;
	CListBox DataFileListing;
	CListBox LibraryFileListing;

	afx_msg void OnBnClickedRadioOptA();
	afx_msg void OnBnClickedRadioOptb();
	afx_msg void OnBnClickedRadioOptc();
	afx_msg void OnBnClickedRadioOptd();

private:
	void SetEnergyTolerance(const float f);
	void ListDirectory(CListBox* listbox, const LPCTSTR directory, const LPCTSTR filter);

	CString DataDirectory;
	std::vector<CString> LibFiles;
	int LibFileCount;
	std::vector<CString> DatFiles;
	int DatFileCount;
	float EnergyTolerance;
	
public:
	afx_msg void OnBnClickedBtnSelectDir();
	CStatic labelDataDir;
	CStatic labelLibDir;
private:
	void VectorizeDirectoryListing(std::vector<CString>* ptrDirectoryListing, const CListBox* ptrListBox);
	int GetListBoxCount(CListBox* listbox);
	void PrepareVectorForList(std::vector<CString>* listvector, int newcount);
public:
	afx_msg void OnBnClickedBtnFileinsert();
	afx_msg void OnBnClickedBtnFileremove();
private:
	void PopulateListBoxItems(CListBox* listbox, std::vector<CString>* data);
	bool LocateVectorDuplicateEntry(const std::vector<CString>* localvector, const CString testvalue);
public:
	afx_msg void OnEnKillfocusEditEnergytol();
};
