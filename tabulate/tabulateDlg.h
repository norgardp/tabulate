
// tabulateDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include <vector>
#include <string>

#include "ProgramDefaults.h"
#include "CNFobject.h"
#include "NLBobject.h"

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
	CButton OutputOptionRB;
	CEdit PSEnergyTolerance;
	CStatic OutputDescription;
	CListBox DataFileListing;
	CListBox LibraryFileListing;
	CListBox LibraryContentListing;
	CStatic labelDataDir;
	CStatic labelLibDir;
	CButton OverwriteMode;

	afx_msg void OnBnClickedRadioOptA();
	afx_msg void OnBnClickedRadioOptb();
	afx_msg void OnBnClickedRadioOptc();
	afx_msg void OnBnClickedRadioOptd();
	afx_msg void OnBnClickedBtnFileinsert();
	afx_msg void OnBnClickedBtnFileremove();
	afx_msg void OnEnKillfocusEditEnergytol();
	afx_msg void OnBnClickedBtnSelectDir();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedOk();

private:
	void SetEnergyTolerance(const double f);
	void ListDirectory(CListBox* listbox, const LPCTSTR directory, const LPCTSTR filter);
	bool ReturnOverwriteState();

	CString DataDirectory;
	std::vector<CString> LibFiles;
	int LibFileCount;
	std::vector<CString> DatFiles;
	int DatFileCount;
	double EnergyTolerance;
	//CNFobject dataset;
	//NLBobject libdataset;

private:
	void VectorizeDirectoryListing(std::vector<CString>* ptrDirectoryListing, const CListBox* ptrListBox);
	int GetListBoxCount(CListBox* listbox);
	void PrepareVectorForList(std::vector<CString>* listvector, int newcount);
	void PopulateListBoxItems(CListBox* listbox, std::vector<CString>* data);
	bool LocateVectorDuplicateEntry(const std::vector<CString>* localvector, const CString testvalue);
	CString GetListBoxSelection(const int i);
	void SetListboxScrollbar(CListBox& theListbox);
	OutputOption ReturnOutputOption();
	std::string ReturnLibraryFilename();
	
};
