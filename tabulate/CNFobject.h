#pragma once

#import "c:\genie2k\s560\tlb\Analyze.tlb"

// Standard libaray
#include <string>
#include <sstream>
#include <iomanip>

// This program
#include "CAMbase.h"
#include "NLBobject.h"
#include "DataStruct.h"
#include "LibraryStruct.h"
#include "OutputOptions.h"
#include "DataStruct.h"

class CNFobject :
	public CAMbase
{
public:
	CNFobject();
	CNFobject(const DataStructure::InitializationOptions& opts);
	CNFobject(const std::string library_name, 
		const bool overwrite_mode, 
		const OutputOption output_option);
	~CNFobject();

	void CreateInstance(const std::string& data_name);
	std::string ReturnFormattedLine();

private:
	void InitializeSequenceAnalyzer(); 
	void SetLibraryFilename(const std::string library_name);
	void SetAnalysisFilename(const std::string analysis_name);
	void SetOverwriteMode(const bool save_mode);
	void SetOutputOption(const OutputOption output_option);
	void SetEnergyTolerance(const double tolerance);
	void SetLibraryDimensions(const std::vector<LibraryStruct>& library_struc);
	void UnpackInitilizerStruct(const DataStructure::InitializationOptions& init_options);
	void OpenDatafile(const std::string& datafile);
	void PerformAnalysis();
	void PopulateDataStructure();
	void ResizeDataStructure();
	void PopulateHeaderStructure();
	void PopulateNuclideData();
	void PushEnergyToleranceToFile();
	void PushNIDLibraryToFile();
	DataStructure::NuclideStructure::NuclideStruct ReturnNuclideInformation(const USHORT i);
	void IDInterestingPeaks();

	void WriteCommonData();
	void WriteCommonDataType();
	void WriteNuclideData();
	void WriteNuclideDataType(const size_t line, const bool final_element);
	void SetStreamParameters(std::stringstream& ss, const size_t field_width);
	void SetStreamParameters(std::stringstream& ss, const size_t field_width, const size_t precision); 
	void WriteStreamDataStr(std::stringstream& ss, std::string& the_string, const bool last);
	template <typename T>
	void WriteStreamData(std::stringstream& ss, T& param, const bool last);
	std::string ReturnSimpleFilename();

private:
	CanberraSequenceAnalyzerLib::ISequenceAnalyzerPtr pSequenceAnalyzer;
	_bstr_t library_name;
	_bstr_t analysis_name;
	OutputOption output_option;
	bool overwrite;
	double energy_tolerance;

	DataStructure::DataStruct psData;
	std::vector<LibraryStruct> psLibrary;
	SHORT start_step{ 0 };
	LibraryDimension libDim;
	std::string output_string;
};

template <typename T>
void CNFobject::WriteStreamData(std::stringstream& ss, T& param, const bool last)
{
	ss << param;
	output_string.append(ss.str());
	if (!last)
		output_string.append(std::string(", "));
}
