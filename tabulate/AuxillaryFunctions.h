#pragma once

#include <string>

// Imports
#import "c:\genie2k\s560\tlb\DataAccess.tlb"
#import "c:\genie2k\s560\tlb\Analyze.tlb"

// Includes
#include "ProgramDefaults.h"
#include "Crackers.h"

enum class ftype
{
	data = 3100,
	library
};


class LIBreader
{
public:
	LIBreader();
	LIBreader(const CString lib);
	~LIBreader();

	void CreateLIBObject(const CString lib);
	bool IsOpen();
	CString ReturnListing();

private:
	CanberraDataAccessLib::IDataAccessPtr CreateDataAccessPtr();
	bool SetFilename(const std::string lib);
	bool OpenFile();
	void CloseFile();
	HMEM ReturnDataSourceContext();
	void GetPeaksCount();
	void GetNuclidesCount();

private:
	CanberraDataAccessLib::IDataAccessPtr pLib;
	HMEM dsc;
	_bstr_t filename;
	LONG cntPeaks;
	LONG cntNuclides;

};


class CAMobject
{
public:
	CAMobject();
	CAMobject(const CString data, const CString library);
	~CAMobject();

	void CreateCAMObject(const CString data, const CString library);
	void CreateCAMObject(const CString library);
	bool IsOpen();
	bool IsReady();
	CString ReturnListing();

private:
	CanberraDataAccessLib::IDataAccessPtr CreateDataAccessPtr();
	bool SetFileName(const std::string name, const ftype type);
	bool OpenFile();
	HMEM ReturnDataSourceContext();
	void LoadDataForAnalysis();
	void ExecuteSeqAnalyzer();
	void GetPeaksCount();
	void GetNuclidesCount();
	CString ListPeaks(const int i);
	CString ListPeakEnergy(const int i);

private:
	CanberraDataAccessLib::IDataAccessPtr pDataAccess;
	CanberraSequenceAnalyzerLib::ISequenceAnalyzerPtr pSeqAnalyzer;
	ftype thetype;
	HMEM dsc;
	bstr_t datFilename;
	bstr_t libFilename;
	SHORT startStep{ 1 };
	bool ReadyForProcessing{ false };
	LONG cntPeaks{ 0 };
	LONG cntNuclides{ 0 };
};
