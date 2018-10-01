#include "stdafx.h"
#include "AuxillaryFunctions.h"


LIBreader::LIBreader()
{
}


LIBreader::LIBreader(const CString lib)
{
	CreateLIBObject(lib);
}


LIBreader::~LIBreader()
{
	if (pLib->IsOpen)
		pLib->Close(CanberraDataAccessLib::CloseMode::dNoUpdate);

	pLib = nullptr;
}


CanberraDataAccessLib::IDataAccessPtr LIBreader::CreateDataAccessPtr()
{
	CanberraDataAccessLib::IDataAccessPtr ptr{ nullptr };
	HRESULT hr{ -1 };
	hr = ptr.CreateInstance(__uuidof(CanberraDataAccessLib::DataAccess));
	if (FAILED(hr))
		ptr = nullptr;
	return ptr;
}


void LIBreader::CreateLIBObject(const CString lib)
{
	pLib = CreateDataAccessPtr();
	std::string local_string{ CW2A(lib) };
	if (SetFilename(local_string))
	{
		OpenFile();
		dsc = ReturnDataSourceContext();
	}
}


bool LIBreader::SetFilename(const std::string name)
{
	if (!name.empty())
		filename = name.c_str();
	return (name.empty() == true) ? false : true;
}


bool LIBreader::OpenFile()
{
	pLib->Open(filename, CanberraDataAccessLib::OpenMode::dReadOnly, 0);
	return (pLib == nullptr) ? false : true;
}


void LIBreader::CloseFile()
{
	pLib->Close(CanberraDataAccessLib::CloseMode::dNoUpdate);
}

HMEM LIBreader::ReturnDataSourceContext()
{
	return reinterpret_cast<HMEM*>(pLib->AccessHandle);
}


bool LIBreader::IsOpen()
{
	BOOL retval = pLib->IsOpen;
	return (retval == TRUE) ? true : false;
}


CString LIBreader::ReturnListing()
{
	CString retstring, temp;
	GetPeaksCount();
	GetNuclidesCount();
	retstring.Format(_T("Peaks: %d\nNuclides: %d\n"), cntPeaks, cntNuclides);
	// CloseFile();
	return retstring;
}


void LIBreader::GetPeaksCount()
{
	cntPeaks = pLib->GetNumberOfRecords(CanberraDataAccessLib::ClassCodes::CAM_CLS_NLINES,
		CanberraDataAccessLib::ParamCodes::CAM_T_NCLTITLE);
}

void LIBreader::GetNuclidesCount()
{
	LONG rec{ 3 };
	cntNuclides = pLib->GetNumberOfEntries(CanberraDataAccessLib::ClassCodes::CAM_CLS_NUCL, rec,
		CanberraDataAccessLib::ParamCodes::CAM_T_NCLNAME);
}
// ############################################################################

// --------------------------------------------------------
CAMobject::CAMobject()
{
}

// --------------------------------------------------------
CAMobject::CAMobject(const CString data, const CString library)
{
	CreateCAMObject(data, library);
}

// --------------------------------------------------------
CAMobject::~CAMobject()
{
	if (pDataAccess->IsOpen)
		pDataAccess->Close(CanberraDataAccessLib::CloseMode::dUpdate);
}


// ============================================================================
// FILE CREATION

CanberraDataAccessLib::IDataAccessPtr CAMobject::CreateDataAccessPtr()
{
	CanberraDataAccessLib::IDataAccessPtr ptr{ nullptr };
	HRESULT hr{ -1 };
	hr = ptr.CreateInstance(__uuidof(CanberraDataAccessLib::DataAccess));
	if (FAILED(hr))
		ptr = nullptr;
	return ptr;
}

void CAMobject::CreateCAMObject(const CString data, const CString library)
{
	pDataAccess = CreateDataAccessPtr();
	std::string localname{ CW2A(data) };
	if (SetFileName(localname, ftype::data))
	{
		LoadDataForAnalysis();
		ReadyForProcessing = IsReady();
	}

	localname = CW2A(library);
	ReadyForProcessing &= SetFileName(localname, ftype::library);
}

void CAMobject::CreateCAMObject(const CString library)
{
	std::string localname{ CW2A(library) };
	SetFileName(localname, ftype::library);
	LoadDataForAnalysis();
	ReadyForProcessing = IsReady();
}

// --------------------------------------------------------
bool CAMobject::SetFileName(const std::string name, const ftype type)
{
	if (!(name.empty()))
	{
		switch (type)
		{
		case ftype::data:
			datFilename = name.c_str();
			thetype = type;
			break;

		case ftype::library:
			libFilename = name.c_str();
			thetype = type;
			break;
		}
	}
	return (name.empty()) ? false : true;
}

// --------------------------------------------------------
bool CAMobject::OpenFile()
{
	switch (thetype)
	{
	case ftype::data:
		pDataAccess->Open(datFilename, CanberraDataAccessLib::OpenMode::dReadWrite, NULL);
		break;

	case ftype::library:
		pDataAccess->Open(libFilename, CanberraDataAccessLib::OpenMode::dReadOnly, NULL);
		break;
	}
	
	return (pDataAccess == nullptr) ? false : true;
}

// --------------------------------------------------------
HMEM CAMobject::ReturnDataSourceContext()
{
	return reinterpret_cast<HMEM*>(pDataAccess->AccessHandle);
}

// --------------------------------------------------------
bool CAMobject::IsOpen()
{
	BOOL result = pDataAccess->IsOpen;
	return (result == TRUE) ? true : false;
}

// --------------------------------------------------------
bool CAMobject::IsReady()
{
	return((IsOpen()) && (dsc != nullptr)) ? true : false;
}

// --------------------------------------------------------
void CAMobject::LoadDataForAnalysis()
{
	bool testval{ false };
	testval = OpenFile();
	if (testval)
		dsc = ReturnDataSourceContext();
}

// --------------------------------------------------------
void CAMobject::ExecuteSeqAnalyzer()
{
	
	if (ReadyForProcessing) 
	{
		_bstr_t empty{ _T("") };
		pSeqAnalyzer->Analyze(pDataAccess, &startStep, datFilename, TRUE, FALSE, FALSE, NULL, empty, NULL);
	}
}

// --------------------------------------------------------
CString CAMobject::ReturnListing()
{
	CString retstring, temp;
	GetPeaksCount();
	GetNuclidesCount();
	
	retstring.Format(_T("Peaks: %d\nNuclides: %d\n"), cntPeaks, cntNuclides);

	return retstring;
}


void CAMobject::GetPeaksCount()
{
	cntPeaks = pDataAccess->GetNumberOfRecords(CanberraDataAccessLib::ClassCodes::CAM_CLS_PEAK, 
		CanberraDataAccessLib::ParamCodes::CAM_T_NCLNAME);
}

void CAMobject::GetNuclidesCount()
{
	cntNuclides = pDataAccess->GetNumberOfRecords(CanberraDataAccessLib::ClassCodes::CAM_CLS_NUCL,
		CanberraDataAccessLib::ParamCodes::CAM_T_NCLNAME);
}

CString CAMobject::ListPeaks(const int index)
{
	_bstr_t bstring;
	CString cstring;

	for (int j{ 1 }; j < cntPeaks; j++)
	{
		bstring = pDataAccess->GetParam(CanberraDataAccessLib::ParamCodes::CAM_T_NCLNAME, index, 0);
		cstring += bstring.GetBSTR();
		cstring += _T("\n");
	}
	return cstring;
}