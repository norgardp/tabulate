
// tabulateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tabulate.h"
#include "tabulateDlg.h"
#include "afxdialogex.h"
#include "ProgramDefaults.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CtabulateDlg dialog



CtabulateDlg::CtabulateDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TABULATE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CtabulateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_FILEINSERT, InsertFile);
	DDX_Control(pDX, IDC_BTN_FILEREMOVE, RemoveFile);
	DDX_Control(pDX, IDC_BTN_SELECTDIR, SelectDataDir);
	DDX_Control(pDX, IDC_EDIT_ENERGYTOL, PSEnergyTolerance);
	DDX_Control(pDX, IDC_OUTPUTDESC, OutputDescription);
	DDX_Control(pDX, IDC_RADIO_OPTA, OutputOptionRB);
	DDX_Control(pDX, IDC_LIST_CNFFILESFORANALYSIS, DataFileListing);
	DDX_Control(pDX, IDC_LIST_LIBFILESINDIR, LibraryFileListing);
	DDX_Control(pDX, IDC_STC_DATDIR, labelDataDir);
	DDX_Control(pDX, IDC_STC_LIBDIR, labelLibDir);
	DDX_Control(pDX, IDC_LIST_LIBCONTENT, LibraryContentListing);
	DDX_Control(pDX, IDC_CHK_OVERWRITE, OverwriteMode);
}

BEGIN_MESSAGE_MAP(CtabulateDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_RADIO_OPTA, &CtabulateDlg::OnBnClickedRadioOptA)
	ON_BN_CLICKED(IDC_RADIO_OPTB, &CtabulateDlg::OnBnClickedRadioOptb)
	ON_BN_CLICKED(IDC_RADIO_OPTC, &CtabulateDlg::OnBnClickedRadioOptc)
	ON_BN_CLICKED(IDC_RADIO_OPTD, &CtabulateDlg::OnBnClickedRadioOptd)
	ON_BN_CLICKED(IDC_BTN_SELECTDIR, &CtabulateDlg::OnBnClickedBtnSelectDir)
	ON_BN_CLICKED(IDC_BTN_FILEINSERT, &CtabulateDlg::OnBnClickedBtnFileinsert)
	ON_BN_CLICKED(IDC_BTN_FILEREMOVE, &CtabulateDlg::OnBnClickedBtnFileremove)
	ON_EN_KILLFOCUS(IDC_EDIT_ENERGYTOL, &CtabulateDlg::OnEnKillfocusEditEnergytol)
	ON_BN_CLICKED(IDC_BUTTON1, &CtabulateDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDOK, &CtabulateDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CtabulateDlg message handlers

BOOL CtabulateDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// ===================== LOCAL INITIALIZATION =====================
	// Set default directories, populate listboxes and std::vectors of CStrings
	// containing valid filenames (library and data files); set default energy
	// search tolerance
	labelLibDir.SetWindowTextW(default_genie_library_directory);
	ListDirectory(&LibraryFileListing, default_genie_library_directory, default_library_extension);
	
	labelDataDir.SetWindowTextW(default_genie_data_directory);
	ListDirectory(&DataFileListing, default_genie_data_directory, default_data_extension);

	VectorizeDirectoryListing(&LibFiles, &LibraryFileListing); 
	VectorizeDirectoryListing(&DatFiles, &DataFileListing);

	SetEnergyTolerance(default_energy_tolerance);
	
	// ===================== LOCAL INITIALIZATION =====================

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CtabulateDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CtabulateDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CtabulateDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CtabulateDlg::OnBnClickedRadioOptA()
{
	OutputDescription.SetWindowTextW(output_option_a);
	UpdateData(FALSE);
}


void CtabulateDlg::OnBnClickedRadioOptb()
{
	OutputDescription.SetWindowTextW(output_option_b);
	UpdateData(FALSE);
}


void CtabulateDlg::OnBnClickedRadioOptc()
{
	OutputDescription.SetWindowTextW(output_option_c);
	UpdateData(FALSE);
}


void CtabulateDlg::OnBnClickedRadioOptd()
{
	OutputDescription.SetWindowTextW(output_option_d);
	UpdateData(FALSE);
}


void CtabulateDlg::SetEnergyTolerance(const double f)
{
	CString str;
	str.Format(_T("%1.2f"), f);
	PSEnergyTolerance.SetWindowTextW(str);
}


void CtabulateDlg::ListDirectory(CListBox* listbox, const LPCTSTR directory, 
	const LPCTSTR filter)
// Populate a listbox with the contents of a directory; the results are to be
// filtered based on the file extension.
{
	const TCHAR* localdir = directory;
	const TCHAR* localfilt = filter;
	
	::SetCurrentDirectory(localdir);
	listbox->ResetContent();
	listbox->Dir(0, localfilt);
}


void CtabulateDlg::OnBnClickedBtnSelectDir()
// Select a non-default DataDirectory. If the new selection is canceled make the
// previous directory the default.
{
	TCHAR previousDirectory[GENIE_MAX_PATH];
	::GetCurrentDirectory(GENIE_MAX_PATH, previousDirectory);

	::CFolderPickerDialog dlg(previousDirectory, 0, NULL, 0);
	if (dlg.DoModal() == IDOK)
	{
		DataDirectory = dlg.GetPathName();
		DataDirectory += _T("\\");
		ListDirectory(&DataFileListing, DataDirectory, default_data_extension);
		labelDataDir.SetWindowTextW(DataDirectory.MakeLower());
	}
	else
	{
		DataDirectory = previousDirectory;
	}

	VectorizeDirectoryListing(&DatFiles, &DataFileListing);
}


void CtabulateDlg::VectorizeDirectoryListing(std::vector<CString>* ptrDirectoryListing, 
	const CListBox* ptrListBox)
// Read contents of a listbox object into a std::vector object. 
{
	CString localstring;
	CListBox* localListbox = (CListBox*)ptrListBox;

	const int count{ GetListBoxCount(localListbox) };
	PrepareVectorForList(ptrDirectoryListing, count);
	for (int i{ 0 }; i < count; i++)
	{
		ptrListBox->GetText(i, localstring);
		ptrDirectoryListing->push_back(localstring);
	}
}


int CtabulateDlg::GetListBoxCount(CListBox* listbox)
{
	return listbox->GetCount();
}


void CtabulateDlg::PrepareVectorForList(std::vector<CString>* listvector, 
	int newcount)
{
	listvector->erase(listvector->begin(), listvector->end());
	listvector->reserve(newcount);
}


void CtabulateDlg::OnBnClickedBtnFileinsert()
// Insert items into the DataFile listbox. Duplicate entries will be filtered out
// and multiple selections are permitted during any individual attempt.
{
	CString path;

	CFileDialog dlg(TRUE, default_data_extension, NULL, 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT, 
		default_configuration_filter, NULL, 0, TRUE);

	if(dlg.DoModal() == IDOK)
	{
		POSITION pos(dlg.GetStartPosition());
		while (pos)
		{
			path = dlg.GetNextPathName(pos);
			path = ::PathFindFileNameW(path);
			if (LocateVectorDuplicateEntry(&DatFiles, path))
				DatFiles.push_back(::PathFindFileNameW(path));
		}
		PopulateListBoxItems(&DataFileListing, &DatFiles);
	}
}


void CtabulateDlg::OnBnClickedBtnFileremove()
// Remove items from the DataFile listbox. Multiple selections are permitted.
// A hard limit on the number of removed items PER ACTION is set by the defined
// GENIE_MAX_DELETE in the program defaults.
{
	// Get current selection and remvoe items from Vecotr AND CListBox
	int items[GENIE_MAX_DELETE];
	int itemcount = DataFileListing.GetSelItems(GENIE_MAX_DELETE, items);

	ASSERT(itemcount != LB_ERR);

	for (int i{ itemcount - 1 }; i >= 0; i--)
	{
		DatFiles.erase(DatFiles.begin() + items[i]);
	}
	PopulateListBoxItems(&DataFileListing, &DatFiles);
}


void CtabulateDlg::PopulateListBoxItems(CListBox* listbox, 
	std::vector<CString>* data)
// Push the contents of a std::vector<CString> into the listbox. This is used
// when adding or removing items from the listbox; its easier to manipulate the
// data in a std::vector than a listbox.
{
	listbox->ResetContent();
	for (size_t i{ 0 }; i < data->size(); i++)
		listbox->AddString(data->at(i));
}


bool CtabulateDlg::LocateVectorDuplicateEntry(const std::vector<CString>* data, 
	const CString testvalue)
// Determine of testvalue input occurs within the std::vector data. If it does, 
// return false, otherwise true. Used to test whether to include testvalue in
// the data vector.
{
	bool unique{ true };
	for (size_t i{ 0 }; i < data->size(); i++)
	{
		if (testvalue == data->at(i))
			unique = false;
	}
	return unique;
}


void CtabulateDlg::OnEnKillfocusEditEnergytol()
// Verify user supplied energy tolerance is within "reasonable" limits as set
// forth in the program defaults. If not, revert to default energy tolerance.
{
	CString windowtext;
	PSEnergyTolerance.GetWindowTextW(windowtext);
	double tempfloat = _ttof(windowtext);
	if ((tempfloat > GENIE_MIN_ETOL) && (tempfloat < GENIE_MAX_ETOL))
		EnergyTolerance = tempfloat;
	else
		SetEnergyTolerance(default_energy_tolerance);
}


CString CtabulateDlg::GetListBoxSelection(const int i)
{
	CString selected_text;
	LibraryFileListing.GetText(i, selected_text);
	return selected_text;
}


void CtabulateDlg::OnBnClickedButton1()
{
	CString string_to_add;
	std::string library_name{ ReturnLibraryFilename() };
	std::string formatted_data;
	int sel = LibraryFileListing.GetCurSel();
	if (sel != LB_ERR)
	{
		LibraryContentListing.ResetContent();

		NLBobject nlb(library_name);
		nlb.PopulateDataStruct();

		size_t nuclide_count{ nlb.ReturnNuclideCount() };
		for (size_t i{ 0 }; i < nuclide_count; i++)
		{
			formatted_data = nlb.ReturnFormattedLine(i);
			string_to_add = formatted_data.c_str();
			LibraryContentListing.AddString(string_to_add);
		}
		SetListboxScrollbar(LibraryContentListing);
	}
}


void CtabulateDlg::SetListboxScrollbar(CListBox& listbox)
// Resize horizontal scroll bar text extents to fit the text that is in the
// listbox.
{
	CString str;
	CSize sz;
	int dx{ 0 };
	TEXTMETRIC tm;
	CDC* pDC = listbox.GetDC();
	CFont* pFont = listbox.GetFont();

	CFont* pOldFont = pDC->SelectObject(pFont);
	pDC->GetTextMetrics(&tm);

	for (int i{ 0 }; i < listbox.GetCount(); i++)
	{
		listbox.GetText(i, str);
		sz = pDC->GetTextExtent(str);
		if (sz.cx > dx)
			dx = sz.cx;
	}

	pDC->SelectObject(pOldFont);
	listbox.ReleaseDC(pDC);

	listbox.SetHorizontalExtent(dx);
}

void CtabulateDlg::OnBnClickedOk()
{
	std::string libname{ ReturnLibraryFilename() }; 
	bool overwrite{ ReturnOverwriteState() };
	OutputOption output{ ReturnOutputOption() };
	
	int data_count{ LibraryContentListing.GetCount() };
	for (int i{ 0 }; i < data_count; i++)
	{
		CNFobject data(libname, overwrite, output);
		data.CreateInstance(ReturnDataFilename(i));		// < ----- Stopped here
	}

	CDialogEx::OnOK();
}


std::string CtabulateDlg::ReturnLibraryFilename()
{
	std::string ret;
	CString libfile{ default_genie_library_directory };
	int sel = LibraryFileListing.GetCurSel();
	if (sel != LB_ERR)
	{
		libfile += GetListBoxSelection(sel);
		ret = CW2A(libfile);
	}
	return ret;
}


std::string CtabulateDlg::ReturnDataFilename(const int i) // <-- Add to header file
{
	std::string ret;
	CString datfile{ default_genie_data_directory };
	int sel = DataFileListing.GetCurSel();
	if (sel != LB_ERR)
	{
		datfile += GetListBoxSelection(datalistbox, sel);	// < --- Create a generic version of this to work w/ ReturnDataFilename and ReturnLibraryFilename
		ret = CW2A(datfile);
	}
	return ret;
}


bool CtabulateDlg::ReturnOverwriteState()
{
	UINT overwrite{ OverwriteMode.GetState() };
	return (overwrite == BST_CHECKED) ? true : false;
}


OutputOption CtabulateDlg::ReturnOutputOption()
{
	OutputOption outoption;
	switch (OutputOptionRB.GetCheck())
	{
	case IDC_RADIO_OPTA:
		outoption = OutputOption::a;
		break;

	case IDC_RADIO_OPTB:
		outoption = OutputOption::b;
		break;

	case IDC_RADIO_OPTC:
		outoption = OutputOption::c;
		break;

	case IDC_RADIO_OPTD:
		outoption = OutputOption::d;
		break;

	default:
		outoption = OutputOption::none;
	}
	return outoption;
}