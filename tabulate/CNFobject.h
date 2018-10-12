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
	std::string ReturnFormattedHeader();

private:
	// Initialization functions
	void InitializeSequenceAnalyzer(); 
	void SetLibraryFilename(const std::string library_name);
	void SetAnalysisFilename(const std::string analysis_name);
	void SetOverwriteMode(const bool save_mode);
	void SetOutputOption(const OutputOption output_option);
	void SetEnergyTolerance(const double tolerance);
	void SetLibraryDimensions(const std::vector<LibraryStruct>& library_struc);
	void UnpackInitilizerStruct(const DataStructure::InitializationOptions& init_options);
	void OpenDatafile(const std::string& datafile);

	// Nuclide analysis and data import
	void PerformAnalysis();
	void PopulateDataStructure();
	void ResizeDataStructure();
	void PopulateHeaderStructure();
	void PopulateNuclideData();
	DataStructure::NuclideStructure::NuclideStruct ReturnNuclideInformation(const USHORT i);
	void IDInterestingPeaks();
	void FindLibraryPeakInData(const FLOAT centroid, const FLOAT tolerance, const std::string nuclide_name);

	// Header file generating functions
	void WriteNuclideHeader(std::stringstream& ss, const bool use_descriptors);
	void WriteHeaderCommonData(std::stringstream& ss, const bool use_descriptors);
	void WriteHeaderModeData(std::stringstream& ss, const bool use_descriptors);
	void WriteHeaderNuclide(std::stringstream& ss, const size_t i, const bool use_descriptors, const bool last_line);
	void WriteHeaderPeakName(std::stringstream& ss, const size_t i);
	void WriteHeaderPeakEnergy(std::stringstream& ss, const size_t i);

	// Main body generating functions
	void WriteDataCommon(std::stringstream& ss);
	void WriteDataModeData(std::stringstream& ss);
	void WriteDataNuclide(std::stringstream& ss);
	void WriteDataNuclideMode(std::stringstream& ss, const size_t i, const bool final_element);

	// Helper functions for generating output
	void SetStreamParameters(std::stringstream& ss, const size_t field_width, const bool left_align);
	void SetStreamParameters(std::stringstream& ss, const size_t field_width, const size_t precision); 
	void WriteStreamDataStr(std::stringstream& ss, std::string& the_string, const bool last);
	template <typename T>
	void WriteStreamData(std::stringstream& ss, T& param, const bool last);
	void InsertEndline(std::stringstream& ss);

	// Miscellaneous helper files
	std::string ReturnSimpleFilename();
	DataStructure::DataStruct MakeBlank();

	// Formatted output generators
	void WritePeakArea(std::stringstream& ss, const size_t i, const bool last_element);
	void WritePeakArea(std::stringstream& ss, const std::string user_string, const bool last_element);
	void WritePeaksearchIterations(std::stringstream& ss, const size_t i, const bool last_element);
	void WritePeaksearchIterations(std::stringstream& ss, const std::string user_string, const bool last_element);
	void WritePeakWidth(std::stringstream& ss, const size_t i, const bool last_element);
	void WritePeakWidth(std::stringstream& ss, const std::string user_string, const bool last_element);
	void WritePeakEnergy(std::stringstream& ss, const size_t i, const bool last_element);
	void WritePeakEnergy(std::stringstream& ss, const std::string user_string, const bool last_element);
	void WritePeakEnergy(std::stringstream& ss, bool last_element); 
	void WritePeakError(std::stringstream& ss, const size_t i, const bool last_element);
	void WritePeakError(std::stringstream& ss, const std::string user_string, const bool last_element);
	void WriteCAMFilename(std::stringstream& ss);
	void WriteCAMFilename(std::stringstream& ss, std::string user_string);
	void WriteCAMAcquisitionTime(std::stringstream& ss);
	void WriteCAMAcquisitionTime(std::stringstream& ss, std::string user_string);
	void WriteCAMRealTime(std::stringstream& ss);
	void WriteCAMRealTime(std::stringstream& ss, std::string user_string);
	void WriteCAMLiveTime(std::stringstream& ss);
	void WriteCAMLiveTime(std::stringstream& ss, std::string user_string);
	void WriteCAMDeadTime(std::stringstream& ss);
	void WriteCAMDeadTime(std::stringstream& ss, std::string user_string);
	void WriteCAMSampleID(std::stringstream& ss);
	void WriteCAMSampleID(std::stringstream& ss, std::string user_string);

private:
	CanberraSequenceAnalyzerLib::ISequenceAnalyzerPtr pSequenceAnalyzer;
	_bstr_t library_name;
	_bstr_t analysis_name;
	OutputOption output_option;
	bool overwrite;
	double energy_tolerance;

	DataStructure::DataStruct psData;
	DataStructure::DataStruct psDataFound;
	DataStructure::DataStruct NoData;
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
		output_string.append(",");
}
