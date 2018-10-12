#include "stdafx.h"
#include "CNFobject.h"


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
	//PushEnergyToleranceToFile();
	//PushNIDLibraryToFile();
}


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
	for (LONG i{ 1 }; i < line_count; i++)
	{
		record = static_cast<USHORT>(i) + 1;
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
	
	return data;
}

void CNFobject::PushEnergyToleranceToFile()
{
	USHORT rec{ 1 };
	SetNumericParam(CAM_F_TOLERANCE, rec, energy_tolerance);
}


void CNFobject::PushNIDLibraryToFile()
{
	USHORT rec{ 1 };

}

void CNFobject::IDInterestingPeaks()
// Use the SadFindRecord() function find library supplied peaks based on peak
// energy and user supplied search tolerance. If a peak is found in data, then
// emplace the peak index into the psLibrary.PeakSearchResult vector.
{
	//SHORT match;
	//OjEnergy_T lib_peak;

	//lib_peak.rToler = static_cast<float>(energy_tolerance);
	size_t max_nuclides{ psLibrary.size() };
	size_t max_lines;
	for (size_t i{ 0 }; i < max_nuclides; i++)
	{
		max_lines = psLibrary.at(i).PeakEnergy.size();
		psLibrary.at(i).PeakSearchResult.resize(max_lines);
		for (size_t j{ 0 }; j < max_lines; j++)
		{
			//lib_peak.rEnergy = psLibrary.at(i).PeakEnergy.at(j);
			FindLibraryPeakInData(psLibrary.at(i).PeakEnergy.at(j), energy_tolerance, psLibrary.at(i).NuclideName);
			//match = ReturnPeakSearchIndex(lib_peak);
			//psLibrary.at(i).PeakSearchResult.at(j) = match;
		}
	}
}


std::string CNFobject::ReturnFormattedHeader()
{
	output_string.clear();
	WriteCommonLibraryNuclideData();
	
	return output_string;
}


std::string CNFobject::ReturnFormattedLine()
{  
	output_string.clear();
	WriteCommonData();
	WriteCommonDataType();
	WriteNuclideData();
	InsertEndline();
	return output_string;
}


// ============================================================================================

void CNFobject::WriteCommonData()
{
	std::stringstream ss;

	// TYPE INDEPENDENT COMMON FILE DATA
	SetStreamParameters(ss, fwf_filename, true);
	WriteStreamDataStr(ss, ReturnSimpleFilename(), false);

	SetStreamParameters(ss, fwf_acquisition_start, false);
	WriteStreamDataStr(ss, psData.AcquisitionStart, false);
}


void CNFobject::WriteCommonLibraryData()
{
	std::stringstream ss;

	std::string empty_string("Filename");
	SetStreamParameters(ss, fwf_filename, true);
	WriteStreamDataStr(ss, empty_string, false);

	empty_string.clear();
	empty_string = std::string("Acquisition Start");
	SetStreamParameters(ss, fwf_acquisition_start, false);
	WriteStreamDataStr(ss, empty_string, false);


}


void CNFobject::WriteCommonLibraryData2()
{
	std::stringstream ss;

	std::string empty_string(fwf_filename, ' ');
	SetStreamParameters(ss, fwf_filename, true);
	WriteStreamDataStr(ss, empty_string, false);

	empty_string.clear();
	empty_string = std::string(fwf_acquisition_start, ' ');
	SetStreamParameters(ss, fwf_acquisition_start, false);
	WriteStreamDataStr(ss, empty_string, false);


}


void CNFobject::WriteCommonLibraryTypeData()
{
	std::stringstream ss;
	std::string empty_string;

	switch (output_option)
	{
	case OutputOption::a:
	case OutputOption::c:
		empty_string.clear();
		empty_string = std::string("EReal");
		SetStreamParameters(ss, fwf_realtime, false);
		WriteStreamDataStr(ss, empty_string, false);

		empty_string.clear();
		empty_string = std::string("ELive");
		SetStreamParameters(ss, fwf_livetime, false);
		WriteStreamDataStr(ss, empty_string, false);
		break;

	case OutputOption::b:
	case OutputOption::d:
		empty_string.clear();
		empty_string = std::string("Sample ID");
		SetStreamParameters(ss, fwf_samplie_id, false);
		WriteStreamDataStr(ss, empty_string, false);

		empty_string.clear();
		empty_string = std::string("% Dead");
		SetStreamParameters(ss, fwf_deadtime_pct, false);
		WriteStreamDataStr(ss, empty_string, false);
		break;
	}
}


void CNFobject::WriteCommonLibraryTypeData2()
{
	std::stringstream ss;
	std::string empty_string;

	switch (output_option)
	{
	case OutputOption::a:
	case OutputOption::c:
		empty_string.clear();
		empty_string = std::string(fwf_realtime, ' ');
		SetStreamParameters(ss, fwf_realtime, false);
		WriteStreamDataStr(ss, empty_string, false);

		empty_string.clear();
		empty_string = std::string(fwf_livetime, ' ');
		SetStreamParameters(ss, fwf_livetime, false);
		WriteStreamDataStr(ss, empty_string, false);
		break;

	case OutputOption::b:
	case OutputOption::d:
		empty_string.clear();
		empty_string = std::string(fwf_samplie_id, ' ');
		SetStreamParameters(ss, fwf_samplie_id, false);
		WriteStreamDataStr(ss, empty_string, false);

		empty_string.clear();
		empty_string = std::string(fwf_deadtime_pct, ' ');
		SetStreamParameters(ss, fwf_deadtime_pct, false);
		WriteStreamDataStr(ss, empty_string, false);
		break;
	}
}


void CNFobject::WriteCommonLibraryNuclideData()
{
	bool last_file, last_line;
	bool last_element{ false };
	size_t max_file{ psLibrary.size() };
	size_t max_line;
	std::stringstream ss;
	std::string empty_string;
	for (size_t otype{ 0 }; otype < 2; otype++)
	{
		last_element = false;
		if (otype == 0)
		{
			WriteCommonLibraryData2();
			WriteCommonLibraryTypeData2();
		}
		else
		{
			WriteCommonLibraryData();
			WriteCommonLibraryTypeData();
		}
		
		for (size_t i{ 0 }; i < max_file; i++)
		{
			last_file = ((i + 1) == max_file) ? true : false;
			max_line = psLibrary.at(i).PeakEnergy.size();

			for (size_t j{ 0 }; j < max_line; j++)
			{
				last_line = ((j + 1) == max_line) ? true : false;
				if (last_file && last_line)
					last_element = true;
				if(otype == 0)
					WriteEmptyNuclideDataType(i, j, last_element);
				else
					WriteEmptyNuclideDataType2(i, j, last_element);
			}
		}
		InsertEndline();
	}
}


void CNFobject::WriteEmptyNuclideDataType(const size_t i, const size_t j, const bool last_element)
{
	std::stringstream ss;
	std::string local_string;

	switch (output_option)
	{
	case OutputOption::a:
		// Nuclide data: peak area, iterations, FWHM, energy
		local_string = psLibrary.at(i).NuclideName;
		SetStreamParameters(ss, fwf_peak_area, false);
		WriteStreamDataStr(ss, local_string, false);
		local_string = std::string(fwf_iterations, ' ');
		SetStreamParameters(ss, fwf_iterations, false);
		WriteStreamDataStr(ss, local_string, false);
		local_string = std::string(fwf_peak_fwhm, ' ');
		SetStreamParameters(ss, fwf_peak_fwhm, false);
		WriteStreamDataStr(ss, local_string, false);
		local_string = std::string(fwf_peak_energy, ' ');
		SetStreamParameters(ss, fwf_peak_energy, false);
		WriteStreamDataStr(ss, local_string, last_element);
		break;

	case OutputOption::b:
		// Nulcide data: peak area, FWHM
		local_string = psLibrary.at(i).NuclideName;
		SetStreamParameters(ss, fwf_peak_area, false);
		WriteStreamDataStr(ss, local_string, false);
		local_string = std::string(fwf_peak_fwhm, ' ');
		SetStreamParameters(ss, fwf_peak_fwhm, false);
		WriteStreamDataStr(ss, local_string, last_element);
		break;

	case OutputOption::c:
		// Nuclide data: peak area, error (%), FWHM, peak energy
		local_string = psLibrary.at(i).NuclideName;
		SetStreamParameters(ss, fwf_peak_area, false);
		WriteStreamDataStr(ss, local_string, false);
		local_string = std::string(fwf_peak_error, ' ');
		SetStreamParameters(ss, fwf_peak_error, false);
		WriteStreamDataStr(ss, local_string, false);
		local_string = std::string(fwf_peak_fwhm, ' ');
		SetStreamParameters(ss, fwf_peak_fwhm, false);
		WriteStreamDataStr(ss, local_string, false);
		local_string = std::string(fwf_peak_energy, ' ');
		SetStreamParameters(ss, fwf_peak_energy, false);
		WriteStreamDataStr(ss, local_string, last_element);
		break;

	case OutputOption::d:
		// Nuclide data: peak area, FWHM, error (%)
		local_string = psLibrary.at(i).NuclideName;
		SetStreamParameters(ss, fwf_peak_area, false);
		WriteStreamDataStr(ss, local_string, false);
		local_string = std::string(fwf_peak_fwhm, ' ');
		SetStreamParameters(ss, fwf_peak_fwhm, false);
		WriteStreamDataStr(ss, local_string, false);
		local_string = std::string(fwf_peak_error, ' ');
		SetStreamParameters(ss, fwf_peak_error, false);
		WriteStreamDataStr(ss, local_string, last_element);
		break;
	}
}


void CNFobject::WriteEmptyNuclideDataType2(const size_t i, const size_t j, const bool last_element)
{
	std::stringstream ss;
	std::string local_string;

	switch (output_option)
	{
	case OutputOption::a:
		// Nuclide data: peak area, iterations, FWHM, energy
		//local_string = std::to_string(psLibrary.at(i).PeakEnergy.at(j));
		SetStreamParameters(ss, fwf_peak_area, fwf_precision_energy);
		WriteStreamData(ss, psLibrary.at(i).PeakEnergy.at(j), false);
		ss << std::right; 
		local_string = "IT";
		SetStreamParameters(ss, fwf_iterations, false);
		WriteStreamDataStr(ss, local_string, false);
		ss << std::right; 
		local_string = "FWHM";
		SetStreamParameters(ss, fwf_peak_fwhm, false);
		WriteStreamDataStr(ss, local_string, false);
		ss << std::right; 
		local_string = "Energy";
		SetStreamParameters(ss, fwf_peak_energy, false);
		WriteStreamDataStr(ss, local_string, last_element);
		break;

	case OutputOption::b:
		// Nulcide data: peak area, FWHM
		local_string = std::to_string(psLibrary.at(i).PeakEnergy.at(j));
		SetStreamParameters(ss, fwf_peak_area, fwf_precision_area);
		WriteStreamDataStr(ss, local_string, false);
		ss << std::right;
		local_string = "FWHM";
		SetStreamParameters(ss, fwf_peak_fwhm, false);
		WriteStreamDataStr(ss, local_string, last_element);
		break;

	case OutputOption::c:
		// Nuclide data: peak area, error (%), FWHM, peak energy
		local_string = std::to_string(psLibrary.at(i).PeakEnergy.at(j));
		SetStreamParameters(ss, fwf_peak_area, fwf_precision_area);
		WriteStreamDataStr(ss, local_string, false);
		ss << std::right;
		local_string = "%Err";
		SetStreamParameters(ss, fwf_peak_error, false);
		WriteStreamDataStr(ss, local_string, false);
		ss << std::right;
		local_string = "FWHM";
		SetStreamParameters(ss, fwf_peak_fwhm, false);
		WriteStreamDataStr(ss, local_string, false);
		ss << std::right;
		local_string = "Energy";
		SetStreamParameters(ss, fwf_peak_energy, false);
		WriteStreamDataStr(ss, local_string, last_element);
		break;

	case OutputOption::d:
		// Nuclide data: peak area, FWHM, error (%)
		local_string = std::to_string(psLibrary.at(i).PeakEnergy.at(j));
		SetStreamParameters(ss, fwf_peak_area, fwf_precision_area);
		WriteStreamDataStr(ss, local_string, false);
		ss << std::right;
		local_string = "FWHM";
		SetStreamParameters(ss, fwf_peak_fwhm, false);
		WriteStreamDataStr(ss, local_string, false);
		ss << std::right;
		local_string = "%Err";
		SetStreamParameters(ss, fwf_peak_error, false);
		WriteStreamDataStr(ss, local_string, last_element);
		break;
	}
}


void CNFobject::WriteCommonDataType()
{
	std::stringstream ss;

	switch (output_option)
	{
	case OutputOption::a:
	case OutputOption::c:
		SetStreamParameters(ss, fwf_realtime, fwf_precision_time);
		WriteStreamData(ss, psData.RealTime, false);
		
		SetStreamParameters(ss, fwf_livetime, fwf_precision_time);
		WriteStreamData(ss, psData.LiveTime, false);
		break;

	case OutputOption::b:
	case OutputOption::d:
		SetStreamParameters(ss, fwf_samplie_id, true);
		WriteStreamData(ss, psData.SampleID, false);
		
		SetStreamParameters(ss, fwf_deadtime_pct, fwf_precision_time);
		WriteStreamData(ss, psData.DeadTimePct, false);
		break;
	}
}

/*
void CNFobject::WriteNuclideData()
{
	bool last_file, last_line;
	bool last_element{ false };
	size_t max_file{ psLibrary.size() };
	size_t max_line;
	size_t max_peaks{ psData.Nuclides.size() };
	std::stringstream ss;
	int use_nuclide;

	for (size_t i{ 0 }; i < max_file; i++)
	{
		last_file = ((i + 1) == max_file) ? true : false;
		max_line = psLibrary.at(i).PeakEnergy.size();
		for (size_t j{ 0 }; j < max_line; j++)
		{
			last_line = ((j + 1) == max_line) ? true : false;
			if (last_file && last_line)
				last_element = true;
			use_nuclide = psLibrary.at(i).PeakSearchResult.at(j);
			WriteNuclideDataType(use_nuclide, last_element);
		}

	}
}
*/

void CNFobject::WriteNuclideData()
{
	std::stringstream ss;
	bool last_element{ false };
	size_t max_nuclides{ psDataFound.Nuclides.size() };
	for (size_t i{ 0 }; i < max_nuclides; i++)
	{
		if (i == (max_nuclides - 1))
			last_element = true;
		WriteNuclideDataType(ss, i, last_element);
	}
}


void CNFobject::WriteNuclideDataType(std::stringstream& ss, const size_t i, const bool final_element)
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


void CNFobject::WritePeakArea(std::stringstream& ss, const size_t i, const bool final_element)
{
	SetStreamParameters(ss, fwf_peak_area, fwf_precision_area);
	WriteStreamData(ss, psDataFound.Nuclides.at(i).Area, final_element);
}


void CNFobject::WritePeakEnergy(std::stringstream& ss, const size_t i, const bool final_element)
{
	SetStreamParameters(ss, fwf_peak_energy, fwf_precision_energy);
	WriteStreamData(ss, psDataFound.Nuclides.at(i).Energy, final_element);
}


void CNFobject::WritePeakError(std::stringstream& ss, const size_t i, const bool final_element)
{
	SetStreamParameters(ss, fwf_peak_error, fwf_precision_error);
	WriteStreamData(ss, psDataFound.Nuclides.at(i).Error, final_element);
}


void CNFobject::WritePeakWidth(std::stringstream& ss, const size_t i, const bool final_element)
{
	SetStreamParameters(ss, fwf_peak_fwhm, fwf_precision_npa);
	WriteStreamData(ss, psDataFound.Nuclides.at(i).FWHM, final_element);
}


void CNFobject::WritePeaksearchIterations(std::stringstream& ss, const size_t i, const bool final_element)
{
	SetStreamParameters(ss, fwf_iterations, fwf_precision_iterations);
	WriteStreamData(ss, psDataFound.Nuclides.at(i).Iterations, final_element);
}

/*
void CNFobject::WriteNuclideDataType(const int line, const bool final_element)
{
	std::stringstream ss;
	DataStructure::DataStruct data;
	size_t lline;

	if (line >= 0)
	{
		data = psData;
		lline = line;
	}
	else
	{
		data = MakeBlank();
		lline = 0;
	}

	switch (output_option)
	{
	case OutputOption::a:
		// Nuclide data: peak area, iterations, FWHM, energy
		SetStreamParameters(ss, fwf_peak_area, fwf_precision_area);
		WriteStreamData(ss, data.Nuclides.at(lline).Area, false);
		SetStreamParameters(ss, fwf_iterations, fwf_precision_iterations);
		WriteStreamData(ss, data.Nuclides.at(lline).Iterations, false);
		SetStreamParameters(ss, fwf_peak_fwhm, fwf_precision_npa);
		WriteStreamData(ss, data.Nuclides.at(lline).FWHM, false);
		SetStreamParameters(ss, fwf_peak_energy, fwf_precision_energy);
		WriteStreamData(ss, data.Nuclides.at(lline).Energy, final_element);
		break;

	case OutputOption::b:
		// Nulcide data: peak area, FWHM
		SetStreamParameters(ss, fwf_peak_area, fwf_precision_area);
		WriteStreamData(ss, data.Nuclides.at(lline).Area, false);
		SetStreamParameters(ss, fwf_peak_fwhm, fwf_precision_npa);
		WriteStreamData(ss, data.Nuclides.at(lline).FWHM, final_element);
		break;

	case OutputOption::c:
		// Nuclide data: peak area, error (%), FWHM, peak energy
		SetStreamParameters(ss, fwf_peak_area, fwf_precision_area);
		WriteStreamData(ss, data.Nuclides.at(lline).Area, false);
		SetStreamParameters(ss, fwf_peak_error, fwf_precision_error);
		WriteStreamData(ss, data.Nuclides.at(lline).Error, false);
		SetStreamParameters(ss, fwf_peak_fwhm, fwf_precision_npa);
		WriteStreamData(ss, data.Nuclides.at(lline).FWHM, false);
		SetStreamParameters(ss, fwf_peak_energy, fwf_precision_energy);
		WriteStreamData(ss, data.Nuclides.at(lline).Energy, false);
		break;

	case OutputOption::d:
		// Nuclide data: peak area, FWHM, error (%)
		SetStreamParameters(ss, fwf_peak_area, fwf_precision_area);
		WriteStreamData(ss, data.Nuclides.at(lline).Area, false);
		SetStreamParameters(ss, fwf_peak_fwhm, fwf_precision_npa);
		WriteStreamData(ss, data.Nuclides.at(lline).FWHM, false);
		SetStreamParameters(ss, fwf_peak_error, fwf_precision_error);
		WriteStreamData(ss, data.Nuclides.at(lline).Error, false);
		break;
	}
}

*/


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


void CNFobject::InsertEndline()
{
	std::stringstream ss;
	ss << std::endl;
	output_string.append(ss.str());
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