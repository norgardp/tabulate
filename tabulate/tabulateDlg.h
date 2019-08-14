
// tabulateDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include <vector>
#include <string>
#include <fstream>

#include "ProgramDefaults.h"
#include "CNFobject.h"
#include "NLBobject.h"
#include "LibraryStruct.h"

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
	CEdit OutputFilename; 
	CEdit PSEnergyTolerance;
	CStatic OutputDescription;
	CStatic FileUnderConsideration;
	CListBox DataFileListing;
	CListBox LibraryFileListing;
	CListBox LibraryContentListing;
	CStatic labelDataDir;
	CStatic labelLibDir;
	CButton OverwriteMode;
	CButton AnalyzeButton;
	CListBox AnalysisFileListing;
	
public:
	afx_msg void OnBnClickedRadioOpta();
	afx_msg void OnBnClickedRadioOptb();
	afx_msg void OnBnClickedRadioOptc();
	afx_msg void OnBnClickedRadioOptd();
	afx_msg void OnBnClickedBtnFileinsert();
	afx_msg void OnBnClickedBtnFileremove();
	afx_msg void OnEnKillfocusEditEnergytol();
	afx_msg void OnBnClickedLibrarySelect();
	afx_msg void OnBnClickedBtnFileremoveall();
	afx_msg void OnBnClickedOk();
	afx_msg void OnEnKillfocusEdtFilename();
	afx_msg void OnLbnDblclkListLibfilesindir();

private:
	CString DataDirectory;
	CString AnalysisDirectory;
	std::vector<CString> DatFiles;
	std::vector<LibraryStruct> libraryData;
	int LibFileCount;
	int DatFileCount;
	double EnergyTolerance;
	bool output_file_status{ false };
	int OutputOptionRB{ 0 };

private:
	void SetEnergyTolerance(const double f);
	bool ReturnOverwriteState();
	void VectorizeDirectoryListing(std::vector<CString>* ptrDirectoryListing, const CListBox* ptrListBox);
	int GetListBoxCount(CListBox* listbox);
	void PrepareVectorForList(std::vector<CString>* listvector, int newcount);
	void PopulateListBoxItems(CListBox* listbox, std::vector<CString>* data);
	bool LocateVectorDuplicateEntry(const std::vector<CString>* localvector, const CString testvalue);
	CString GetListBoxSelection(const CListBox& listbox, const int i);
	void SetListboxColumnWidth(CListBox* listbox); 
	void SetListboxScrollbar(CListBox& theListbox);
	OutputOption ReturnOutputOption();
	std::string ReturnLibraryFilename();
	std::string ReturnDataFilename(const int i);
	std::string ReturnAnalysisFilename();
	DataStructure::InitializationOptions ObtainInitializationOptions();
	void ButtonHandler();
	CString CreateFilename(const CStatic& directory, const CEdit& filename);
	std::ofstream OpenFileForOutput();
	std::string ReturnHeaderData(const CString start_file, const CString end_file);
	void ListDirectory(CListBox* listbox, const LPCTSTR directory, const LPCTSTR filter);

};
