#include "stdafx.h"
#include "CNFobject.h"


// ========================================================================
//
// ========================= OBJECT INSTANTIATION =========================
//
// ========================================================================


CNFobject::CNFobject()
{
}


CNFobject::CNFobject(const DataStructure::InitializationOptions& options)
{
	InitializeSequenceAnalyzer();
	UnpackInitilizerStruct(options);
}


CNFobject::CNFobject(const std::string lib_name, 
	const bool overwrite, 
	const OutputOption output_opt)
{
	InitializeSequenceAnalyzer();
	SetLibraryFilename(lib_name);
	SetOverwriteMode(overwrite);
	SetOutputOption(output_opt);
}


CNFobject::~CNFobject()
{
}


void CNFobject::InitializeSequenceAnalyzer()
{
	HRESULT hr{ pSequenceAnalyzer.CreateInstance(__uuidof(CanberraSequenceAnalyzerLib::SequenceAnalyzer)) };
	if (FAILED(hr))
		pSequenceAnalyzer = nullptr;
}


void CNFobject::SetLibraryFilename(const std::string name)
{
	if (!name.empty())
		library_name = name.c_str();
}


void CNFobject::SetAnalysisFilename(const std::string name)
{
	if (!name.empty())
		analysis_name = name.c_str();
}


void CNFobject::SetEnergyTolerance(const double tol)
{
	energy_tolerance = tol;
}


void CNFobject::SetOutputOption(const OutputOption option)
{
	output_option = option;
}


void CNFobject::SetOverwriteMode(const bool mode)
{
	overwrite = mode;
}


void CNFobject::SetLibraryDimensions(const std::vector<LibraryStruct>& library_struc)
{
	psLibrary = library_struc;
}


void CNFobject::UnpackInitilizerStruct(const DataStructure::InitializationOptions& init)
{
	SetLibraryFilename(init.Library);
	SetAnalysisFilename(init.Analysis);
	SetOverwriteMode(init.OverwriteMode);
	SetOutputOption(init.OutputFormat);
	SetEnergyTolerance(init.EnergyTolerance);
	SetLibraryDimensions(init.NuclidesInLibrary);
}


void CNFobject::CreateInstance(const std::string& datafile)
{
	OpenDatafile(datafile);
	PerformAnalysis();
	PopulateDataStructure();
}


void CNFobject::OpenDatafile(const std::string& datafile)
{
	CanberraDataAccessLib::OpenMode open_mode;
	CanberraDataAccessLib::CloseMode close_mode;

	if (!datafile.empty())
	{
		if (overwrite)
		{
			open_mode = CanberraDataAccessLib::OpenMode::dReadWrite;
			close_mode = CanberraDataAccessLib::CloseMode::dUpdate;
		}
		else
		{
			// If overwrite == false, then the data file still needs to be open
			// in read/write mode to permit the modification of the energy
			// tolerance used for the NID routine.
			open_mode = CanberraDataAccessLib::OpenMode::dReadWrite;
			close_mode = CanberraDataAccessLib::CloseMode::dNoUpdate;
		}
	}
	OpenFile(datafile, open_mode, close_mode);
}


// ===============================================================================
//
// ========================= NUCLIDE ANALYSIS and IMPORT =========================
//
// ===============================================================================


void CNFobject::PerformAnalysis()
{
	_bstr_t fake;
	if (pSequenceAnalyzer != nullptr)
		pSequenceAnalyzer->Analyze(pDataAccess, &start_step, analysis_name, 
			FALSE, FALSE, FALSE, TRUE, fake, NULL);
}


void CNFobject::PopulateDataStructure()
{
	ResizeDataStructure();
	PopulateHeaderStructure();
	PopulateNuclideData();
	output_string.clear();
	IDInterestingPeaks();
}


void CNFobject::ResizeDataStructure()
{
	psData.Nuclides.resize((size_t)libDim.NumberOfNuclides);
}


void CNFobject::PopulateHeaderStructure()
{
	psData.SampleID = ReturnStringParam(CAM_T_SIDENT, 0, CAM_N_SIDENT);
	psData.AcquisitionStart = ReturnTimeSParam(CAM_X_ASTIME, 0);
	psData.AcquisitionStart.pop_back();		// remove trailing '\0'
	psData.LiveTime = ReturnTimeNParam(CAM_X_ELIVE, 0);
	psData.RealTime = ReturnTimeNParam(CAM_X_EREAL, 0);
	psData.DeadTimePct = 100 * (psData.RealTime - psData.LiveTime) / psData.RealTime;
}


void CNFobject::PopulateNuclideData()
{
	USHORT record;
	LONG line_count = ReturnRecordCount(CAM_CLS_PEAK);
	LONG Ldummy;
	DOUBLE Ddummy;
	double PPCTIME{ 0 };

	for (LONG i{ 0 }; i < line_count; i++)
	{
		record = static_cast<USHORT>(i) + 1;
		PPCTIME = ReturnTimeNParam(CAM_X_PPELIVE, record);
		psData.Nuclides.push_back(ReturnNuclideInformation(record));
	}
}


DataStructure::NuclideStructure::NuclideStruct CNFobject::ReturnNuclideInformation(const USHORT i)
{
	LONG Ldummy;
	FLOAT Fdummy;
	DataStructure::NuclideStructure::NuclideStruct data;
	
	data.Iterations = ReturnNumericParam(CAM_L_PSITER, i, Ldummy);
	data.Area = ReturnNumericParam(CAM_F_PSAREA, i, Fdummy);
	data.Energy = ReturnNumericParam(CAM_F_PSENERGY, i, Fdummy);
	data.FWHM = ReturnNumericParam(CAM_F_PSFWHM, i, Fdummy);
	data.Rate = ReturnNumericParam(CAM_F_PSCTSS, i, Fdummy);
	data.Error = ReturnNumericParam(CAM_F_PSCERR, i, Fdummy);

	// PAN 2020-Jan-06; test for multiplet in peak-search algorithm output
	// Trying to figure out why data is missing from Ruth-Ann's data when
	// TABULATE works on it.
	//long psmult = ReturnNumericParam(CAM_L_PSPMULT, i, Ldummy);

	return data;
}


void CNFobject::IDInterestingPeaks()
{
	size_t max_nuclides{ psLibrary.size() };
	size_t max_lines;
	FLOAT tolerance{ static_cast<FLOAT>(energy_tolerance) };
	for (size_t i{ 0 }; i < max_nuclides; i++)
	{
		max_lines = psLibrary.at(i).PeakEnergy.size();
		psLibrary.at(i).PeakSearchResult.resize(max_lines);
		for (size_t j{ 0 }; j < max_lines; j++)
			FindLibraryPeakInData(psLibrary.at(i).PeakEnergy.at(j), tolerance, psLibrary.at(i).NuclideName);
	}
}


void CNFobject::FindLibraryPeakInData(const FLOAT centroid, const FLOAT tolerance,
	std::string nuclide_name)
{
	FLOAT hilimit{ centroid + tolerance / 2 };
	FLOAT lolimit{ centroid - tolerance / 2 };
	size_t total_found_peaks{ psData.Nuclides.size() };
	bool peak_was_not_found{ true };

	for (size_t i{ 0 }; i < total_found_peaks; i++)
	{
		FLOAT peak_energy{ psData.Nuclides.at(i).Energy };
		if ((peak_energy > lolimit) && (peak_energy < hilimit))
		{
			// peak was found
			peak_was_not_found = false;
			psDataFound.Nuclides.push_back(psData.Nuclides.at(i));
			psData.Nuclides.erase(psData.Nuclides.begin() + i);
			total_found_peaks--;
			i--;
			size_t current_iterator = psDataFound.Nuclides.size() - 1;
			psDataFound.Nuclides.at(current_iterator).NucName = nuclide_name;
			psDataFound.Nuclides.at(current_iterator).IdealEnergy = centroid;
			break;
		}
	}

	if (peak_was_not_found)
	{
		NoData = MakeBlank();
		NoData.Nuclides.at(0).NucName = nuclide_name;
		NoData.Nuclides.at(0).IdealEnergy = centroid;
		psDataFound.Nuclides.push_back(NoData.Nuclides.at(0));
	}
}


// ======================================================================
//
// ========================= HEADER FILE OUTPUT =========================
//
// ======================================================================

// Output file header construction looks generally like this:
// [header info supplied by primary routine]
//                ,                       ,                  ,       , NA - 24,  ,      , CO - 60,   ,
// Filename       ,      Acquisition start,         Sample ID, % Dead, 1368.63, IT, FWHM,  661.32, IT, FWHM
// [-------------------------------------] [------------------------] [-------------------//---------------]
//  Header-Common                           Header-Mode                Header-Nuclide
//  Line 1: empty                           empty                      nuclide name + empty fields
//  Line 2: descriptors                     mode-specific descriptors  field descriptors
//
std::string CNFobject::ReturnFormattedHeader()
{
	output_string.clear();
	std::stringstream ss;

	bool use_descriptors{ true };
	WriteNuclideHeader(ss, !use_descriptors);
	WriteNuclideHeader(ss, use_descriptors);
	return output_string;
}


void CNFobject::WriteNuclideHeader(std::stringstream& ss, const bool use_descriptors)
{
	WriteHeaderCommonData(ss, use_descriptors);
	WriteHeaderModeData(ss, use_descriptors);
	
	size_t max_nuclides{ psDataFound.Nuclides.size() };
	bool last_line{ false };
	for (size_t i{ 0 }; i < max_nuclides; i++)
	{
		if (i == (max_nuclides - 1))
			last_line = true;
		WriteHeaderNuclide(ss, i, use_descriptors, last_line);
	}
	InsertEndline(ss);
}


void CNFobject::WriteHeaderCommonData(std::stringstream& ss, const bool use_descriptors)
{
	std::string filename_to_pass, acquisition_to_pass;

	if (use_descriptors)
	{
		filename_to_pass = std::string("Filename");
		acquisition_to_pass = std::string("Acquisition Start");
	}
	else
	{
		filename_to_pass = std::string(fwf_filename, ' ');
		acquisition_to_pass = std::string(fwf_acquisition_start, ' ');
	}

	WriteCAMFilename(ss, filename_to_pass);
	WriteCAMAcquisitionTime(ss, acquisition_to_pass);
}


void CNFobject::WriteHeaderModeData(std::stringstream& ss, const bool use_descriptors)
{
	
	std::string param1_to_pass, param2_to_pass;

	switch (output_option)
	{
	case OutputOption::b:
	case OutputOption::d:
		param1_to_pass = (use_descriptors) ? std::string("EReal") : std::string(fwf_realtime, ' ');
		param2_to_pass = (use_descriptors) ? std::string("ELive") : std::string(fwf_livetime, ' ');
		WriteCAMRealTime(ss, param1_to_pass);
		WriteCAMLiveTime(ss, param2_to_pass);
		break;

	case OutputOption::a:
	case OutputOption::c:
		param1_to_pass = (use_descriptors) ? std::string("Sample ID") : std::string(fwf_sample_id, ' ');
		param2_to_pass = (use_descriptors) ? std::string("% Dead") : std::string(fwf_deadtime_pct, ' ');
		WriteCAMSampleID(ss, param1_to_pass);
		WriteCAMDeadTime(ss, param2_to_pass);
		break;
	}
}


void CNFobject::WriteHeaderNuclide(std::stringstream& ss, const size_t i, const bool use_descriptors, const bool last_line)
{
	std::string param1, param2, param3;
	size_t max_nuclides{ psDataFound.Nuclides.size() };
	
	if (use_descriptors)
		WriteHeaderPeakEnergy(ss, i);
	else
		WriteHeaderPeakName(ss, i);
		

	switch (output_option)
	{
	case OutputOption::a:
		param1 = (use_descriptors) ? std::string("IT") : std::string(fwf_iterations, ' ');
		param2 = (use_descriptors) ? std::string("FWHM") : std::string(fwf_peak_fwhm, ' ');
		param3 = (use_descriptors) ? std::string("Energy") : std::string(fwf_peak_energy, ' ');
		WritePeaksearchIterations(ss, param1, false);
		WritePeakWidth(ss, param2, false);
		WritePeakEnergy(ss, param3, last_line);
		break;

	case OutputOption::b:
		param1 = (use_descriptors) ? std::string("FWHM") : std::string(fwf_peak_fwhm, ' ');
		WritePeakWidth(ss, param1, last_line);
		break;

	case OutputOption::c:
		param1 = (use_descriptors) ? std::string("%Err") : std::string(fwf_peak_error, ' ');
		param2 = (use_descriptors) ? std::string("FWHM") : std::string(fwf_peak_fwhm, ' ');
		param3 = (use_descriptors) ? std::string("Energy") : std::string(fwf_peak_energy, ' ');
		WritePeakError(ss, param1, false);
		WritePeakWidth(ss, param2, false);
		WritePeakEnergy(ss, param3, last_line);
		break;

	case OutputOption::d:
		param1 = (use_descriptors) ? std::string("FWHM") : std::string(fwf_peak_fwhm, ' ');
		param2 = (use_descriptors) ? std::string("%Err") : std::string(fwf_peak_error, ' ');
		WritePeakWidth(ss, param1, false);
		WritePeakError(ss, param2, last_line);
		break;
	}
}


// =======================================================================
//
// ========================= NUCLIDE DATA OUTPUT =========================
//
// =======================================================================


std::string CNFobject::ReturnFormattedLine()
{
	output_string.clear();
	std::stringstream ss;

	WriteDataCommon(ss);
	WriteDataModeData(ss);
	WriteDataNuclide(ss);
	InsertEndline(ss);
	return output_string;
}


void CNFobject::WriteDataCommon(std::stringstream& ss)
{
	WriteCAMFilename(ss);
	WriteCAMAcquisitionTime(ss);
}


void CNFobject::WriteDataModeData(std::stringstream& ss)
{
	switch (output_option)
	{
	case OutputOption::b:
	case OutputOption::d:
		WriteCAMRealTime(ss);
		WriteCAMLiveTime(ss);
		break;

	case OutputOption::a:
	case OutputOption::c:
		WriteCAMSampleID(ss);
		WriteCAMDeadTime(ss);
		break;
	}
}


void CNFobject::WriteDataNuclide(std::stringstream& ss)
{
	bool last_element{ false };
	size_t max_nuclides{ psDataFound.Nuclides.size() };
	for (size_t i{ 0 }; i < max_nuclides; i++)
	{
		if (i == (max_nuclides - 1))
			last_element = true;
		WriteDataNuclideMode(ss, i, last_element);
	}
}


void CNFobject::WriteDataNuclideMode(std::stringstream& ss, const size_t i, const bool final_element)
{
	switch (output_option)
	{
	case OutputOption::a:
		WritePeakArea(ss, i, false);
		WritePeaksearchIterations(ss, i, false);
		WritePeakWidth(ss, i, false);
		WritePeakEnergy(ss, i, final_element);
		break;

	case OutputOption::b:
		WritePeakArea(ss, i, false);
		WritePeakWidth(ss, i, final_element);
		break;

	case OutputOption::c:
		WritePeakArea(ss, i, false);
		WritePeakError(ss, i, false);
		WritePeakWidth(ss, i, false);
		WritePeakEnergy(ss, i, final_element);
		break;

	case OutputOption::d:
		WritePeakArea(ss, i, false);
		WritePeakWidth(ss, i, false);
		WritePeakError(ss, i, final_element);
		break;
	}
}


// ==============================================================================
//
// ========================= OUTPUT FORMATING FUNCTIONS =========================
//
// ==============================================================================


// --------------> Header File Stuff
void CNFobject::WriteHeaderPeakName(std::stringstream& ss, const size_t i)
{
	SetStreamParameters(ss, fwf_peak_area, false);
	WriteStreamDataStr(ss, psDataFound.Nuclides.at(i).NucName, false);
}


void CNFobject::WriteHeaderPeakEnergy(std::stringstream& ss, const size_t i)
{
	SetStreamParameters(ss, fwf_peak_area, fwf_precision_energy);
	WriteStreamData(ss, psDataFound.Nuclides.at(i).IdealEnergy , false);
}
// <------------- Header File Stuff

void CNFobject::WritePeakArea(std::stringstream& ss, const size_t i, const bool final_element)
{
	SetStreamParameters(ss, fwf_peak_area, fwf_precision_area);
	WriteStreamData(ss, psDataFound.Nuclides.at(i).Area, final_element);
}


void CNFobject::WritePeakArea(std::stringstream& ss, const std::string user_string, const bool final_element)
{
	SetStreamParameters(ss, fwf_peak_area, fwf_precision_area);
	WriteStreamData(ss, user_string, final_element);
}


void CNFobject::WritePeakEnergy(std::stringstream& ss, const size_t i, const bool final_element)
{
	SetStreamParameters(ss, fwf_peak_energy, fwf_precision_energy);
	WriteStreamData(ss, psDataFound.Nuclides.at(i).Energy, final_element);
}


void CNFobject::WritePeakEnergy(std::stringstream& ss, const std::string user_string, const bool final_element)
{
	SetStreamParameters(ss, fwf_peak_energy, fwf_precision_energy);
	WriteStreamData(ss, user_string, final_element);
}


void CNFobject::WritePeakEnergy(std::stringstream& ss, bool final_element)
{
	SetStreamParameters(ss, fwf_peak_energy, fwf_precision_energy);
	WriteStreamData(ss, std::string(fwf_peak_energy, ' '), final_element);
}


void CNFobject::WritePeakError(std::stringstream& ss, const size_t i, const bool final_element)
{
	SetStreamParameters(ss, fwf_peak_error, fwf_precision_error);
	WriteStreamData(ss, psDataFound.Nuclides.at(i).Error, final_element);
}


void CNFobject::WritePeakError(std::stringstream& ss, const std::string user_string, const bool final_element)
{
	SetStreamParameters(ss, fwf_peak_error, fwf_precision_error);
	WriteStreamData(ss, user_string, final_element);
}


void CNFobject::WritePeakWidth(std::stringstream& ss, const size_t i, const bool final_element)
{
	SetStreamParameters(ss, fwf_peak_fwhm, fwf_precision_npa);
	WriteStreamData(ss, psDataFound.Nuclides.at(i).FWHM, final_element);
}


void CNFobject::WritePeakWidth(std::stringstream& ss, const std::string user_string, const bool final_element)
{
	SetStreamParameters(ss, fwf_peak_fwhm, fwf_precision_npa);
	WriteStreamData(ss, user_string, final_element);
}


void CNFobject::WritePeaksearchIterations(std::stringstream& ss, const size_t i, const bool final_element)
{
	SetStreamParameters(ss, fwf_iterations, fwf_precision_iterations);
	WriteStreamData(ss, psDataFound.Nuclides.at(i).Iterations, final_element);
}


void CNFobject::WritePeaksearchIterations(std::stringstream& ss, const std::string user_string, bool final_element)
{
	SetStreamParameters(ss, fwf_iterations, fwf_precision_iterations);
	WriteStreamData(ss, user_string, final_element);
}


void CNFobject::WriteCAMFilename(std::stringstream& ss)
{
	SetStreamParameters(ss, fwf_filename, true);
	WriteStreamDataStr(ss, ReturnSimpleFilename(), false);
}


void CNFobject::WriteCAMFilename(std::stringstream& ss, std::string user_string)
{
	SetStreamParameters(ss, fwf_filename, true);
	WriteStreamDataStr(ss, user_string, false);
}


void CNFobject::WriteCAMAcquisitionTime(std::stringstream& ss)
{
	SetStreamParameters(ss, fwf_acquisition_start, false);
	WriteStreamDataStr(ss, psData.AcquisitionStart, false);
}


void CNFobject::WriteCAMAcquisitionTime(std::stringstream& ss, std::string user_string)
{
	SetStreamParameters(ss, fwf_acquisition_start, false);
	WriteStreamDataStr(ss, user_string, false);
}


void CNFobject::WriteCAMRealTime(std::stringstream& ss)
{
	SetStreamParameters(ss, fwf_realtime, fwf_precision_time);
	WriteStreamData(ss, psData.RealTime, false);
}


void CNFobject::WriteCAMRealTime(std::stringstream& ss, std::string user_string)
{
	SetStreamParameters(ss, fwf_realtime, fwf_precision_time);
	WriteStreamData(ss, user_string, false);
}


void CNFobject::WriteCAMLiveTime(std::stringstream& ss)
{
	SetStreamParameters(ss, fwf_livetime, fwf_precision_time);
	WriteStreamData(ss, psData.LiveTime, false);
}


void CNFobject::WriteCAMLiveTime(std::stringstream& ss, std::string user_string)
{
	SetStreamParameters(ss, fwf_livetime, fwf_precision_time);
	WriteStreamData(ss, user_string, false);
}


void CNFobject::WriteCAMDeadTime(std::stringstream& ss)
{
	SetStreamParameters(ss, fwf_deadtime_pct, fwf_precision_time);
	WriteStreamData(ss, psData.DeadTimePct, false);
}


void CNFobject::WriteCAMDeadTime(std::stringstream& ss, std::string user_string)
{
	SetStreamParameters(ss, fwf_deadtime_pct, fwf_precision_time);
	WriteStreamData(ss, user_string, false);
}


void CNFobject::WriteCAMSampleID(std::stringstream& ss)
{
	SetStreamParameters(ss, fwf_sample_id, true);
	WriteStreamDataStr(ss, psData.SampleID , false);
}


void CNFobject::WriteCAMSampleID(std::stringstream& ss, std::string user_string)
{
	SetStreamParameters(ss, fwf_sample_id, true);
	WriteStreamDataStr(ss, user_string, false);
}


// ===========================================================================
//
// ========================= OUTPUT STREAM FUNCTIONS =========================
//
// ===========================================================================


void CNFobject::SetStreamParameters(std::stringstream& ss, const size_t field_width, const bool left_align)
{
	std::stringstream().swap(ss);
	if (left_align)
		ss << std::left << std::setw(field_width);
	else
		ss << std::right << std::setw(field_width);
}


void CNFobject::SetStreamParameters(std::stringstream& ss, const size_t field_width, const size_t precision)
{
	std::stringstream().swap(ss);
	ss.precision(precision);
	ss << std::setw(field_width) << std::fixed;
}


void CNFobject::WriteStreamDataStr(std::stringstream& ss, std::string& param, const bool last)
{
	
	ss << param;
	std::string local_string(ss.str());
	output_string.append(local_string);
	if (!last)
		output_string.append(",");
}


void CNFobject::InsertEndline(std::stringstream& ss)
{
	std::stringstream().swap(ss);
	ss << std::endl;
	output_string.append(ss.str());
}


// ===========================================================================
//
// ========================= MISCELLANEOUS FUNCTIONS =========================
//
// ===========================================================================


std::string CNFobject::ReturnSimpleFilename()
{
	std::string filename = ReturnFilename();

	// Remove pathname if present
	const size_t last_slash_index = filename.find_last_of("\\/");
	if (std::string::npos != last_slash_index)
		filename.erase(0, last_slash_index + 1);

	// Remove file extension if present
	const size_t period_index = filename.rfind(".");
	if (std::string::npos != period_index)
		filename.erase(period_index);

	return filename;
}


DataStructure::DataStruct CNFobject::MakeBlank()
{
	DataStructure::DataStruct return_value;
	return_value.Nuclides.resize(1);
	return_value.Nuclides.at(0).Area = static_cast<FLOAT>(0.0);
	return_value.Nuclides.at(0).Energy = static_cast<FLOAT>(0.0);
	return_value.Nuclides.at(0).Error = static_cast<FLOAT>(0.0);
	return_value.Nuclides.at(0).FWHM = static_cast<FLOAT>(0.0);
	return_value.Nuclides.at(0).Iterations = static_cast<LONG>(0.0);
	return_value.Nuclides.at(0).Rate = static_cast<FLOAT>(0.0);
	return return_value;
}
