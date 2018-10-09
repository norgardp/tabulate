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
	void AppendToOutputString(const USHORT nuclide_no);
	void WriteCommonData();
	void WriteCommonData_Type1();
	void WriteCommonData_Type2();
	std::string ReturnSimpleFilename();
	std::string ReturnFixedWidthString(std::string& the_string, const size_t field_width);
	void AddComma();
	void WriteNuclideData();
	void WriteNuclideData_Type1();
	void AppendToOutputString(std::string the_string, const size_t field_width, const bool add_comma);
	std::string ReturnPeakArea(const USHORT nuclide, const USHORT line);
	std::string ReturnIterations(const USHORT nuclide, const USHORT line);
	std::string ReturnPeakFWHM(const USHORT nuclide, const USHORT line);
	std::string ReturnPeakEnergy(const USHORT nuclide, const USHORT line);
	std::string ReturnFixedIntegerString(LONG& the_number);
	LONG ConvertFLOATtoLONG(const FLOAT& the_float);

	template<typename T>
	std::string ReturnFixedFieldWidthString(T& the_number, const size_t precision);

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

template<typename T>
std::string CNFobject::ReturnFixedFieldWidthString(T& the_number, const size_t precision)
{
	std::stringstream ss;
	ss << std::fixed << std::setprecision(precision) << the_number;
	return ss.str();
}
