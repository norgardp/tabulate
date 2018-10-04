#pragma once

#import "c:\genie2k\s560\tlb\Analyze.tlb"

// Standard libaray
#include <string>

// This program
#include "CAMbase.h"
#include "NLBobject.h"
#include "DataStruct.h"
#include "LibraryStruct.h"
#include "OutputOptions.h"


class CNFobject :
	public CAMbase
{
public:
	CNFobject();
	CNFobject(const std::string library_name, 
		const bool overwrite_mode, 
		const OutputOption output_option);
	~CNFobject();

	void CreateInstance(const std::string& data_name);
	std::string ReturnFormattedLine();

private:
	void InitializeSequenceAnalyzer(); 
	void SetLibraryFilename(const std::string library_name);
	void SetOverwriteMode(const bool save_mode);
	void SetOutputOption(const OutputOption output_option);
	void OpenDatafile(const std::string& datafile);
	void PerformAnalysis();
	void GetLibraryDimensions();
	void PopulateDataStructure();
	void ResizeDataStructure();
	void PopulateHeaderStructure();
	void PopulateNuclideData();


private:
	CanberraSequenceAnalyzerLib::ISequenceAnalyzerPtr pSequenceAnalyzer;
	_bstr_t library_name;
	OutputOption output_option;
	bool overwrite;

	DataStructure::DataStruct psData;
	std::vector<LibraryStruct> psLibrary;
	SHORT start_step{ 0 };
	LibraryDimension libDim;
};

