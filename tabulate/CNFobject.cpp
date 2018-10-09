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
	psData.LiveTime = ReturnTimeNParam(CAM_X_ELIVE, 0);
	psData.RealTime = ReturnTimeNParam(CAM_X_EREAL, 0);
	psData.DeadTimePct = 100 * (psData.RealTime - psData.LiveTime) / psData.RealTime;
}


void CNFobject::PopulateNuclideData()
{
	USHORT record;
	USHORT line_count = ReturnRecordCount(CAM_CLS_PEAK);
	for (USHORT i{ 1 }; i < line_count; i++)
	{
		record = i + 1;
		psData.Nuclides.push_back(ReturnNuclideInformation(record));
	}
}


DataStructure::NuclideStructure::NuclideStruct CNFobject::ReturnNuclideInformation(const USHORT i)
{
	LONG Ldummy;
	FLOAT Fdummy;
	DataStructure::NuclideStructure::NuclideStruct data;
	
	data.Iterations.push_back(ReturnNumericParam(CAM_L_PSITER, i, Ldummy));
	data.Area.push_back(ReturnNumericParam(CAM_F_PSAREA, i, Fdummy));
	data.Energy.push_back(ReturnNumericParam(CAM_F_PSENERGY, i, Fdummy));
	data.FWHM.push_back(ReturnNumericParam(CAM_F_PSFWHM, i, Fdummy));
	data.Rate.push_back(ReturnNumericParam(CAM_F_PSCTSS, i, Fdummy));
	data.Error.push_back(ReturnNumericParam(CAM_F_PSCERR, i, Fdummy));
	
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
	LONG nuclide_lines;
	SHORT match;
	USHORT nuclide_match;
	OjEnergy_T lib_peak;

	lib_peak.rToler = energy_tolerance;
	for (LONG i{ 0 }; i < psLibrary.size(); i++)
	{
		nuclide_lines = psLibrary.at(i).PeakEnergy.size();
		psLibrary.at(i).PeakSearchResult.resize(nuclide_lines);
		for (LONG j{ 0 }; j < nuclide_lines; j++)
		{
			lib_peak.rEnergy = psLibrary.at(i).PeakEnergy.at(j);
			match = ReturnPeakSearchIndex(lib_peak);
			psLibrary.at(i).PeakSearchResult.at(j) = match;
		}
	}
}

std::string CNFobject::ReturnFormattedLine()
{  
	WriteCommonData();
	WriteNuclideData();
	return output_string;
}


void CNFobject::WriteCommonData()
{
	switch (output_option)
	{
	case OutputOption::a:
	case OutputOption::c:
		// Common data: Filename, Acquisition start (DD:MMM:YYYY HH:MM:SS), Real Time, Live Time
		WriteCommonData_Type1();
		break;

	case OutputOption::b:
	case OutputOption::d:
		// Common data: Filename, Acquisition start (DD:MMM:YYYY HH:MM:SS), Sample ID, % Dead Time
		WriteCommonData_Type2();
		break;
	}
}


void CNFobject::AppendToOutputString(std::string the_string, const size_t field_width, const bool add_comma)
{
	output_string += ReturnFixedWidthString(the_string, field_width);
	if (add_comma)
		AddComma();
}


void CNFobject::WriteCommonData_Type1()
{
	std::string local = ReturnSimpleFilename();
	AppendToOutputString(local, fwf_filename, true);

	local.clear();
	local = psData.AcquisitionStart;
	AppendToOutputString(local, fwf_acquisition_start, true);

	local.clear();
	local = ReturnFixedFieldWidthString(psData.RealTime, fwf_precision_time);
	AppendToOutputString(local, fwf_realtime, true);
	
	local.clear();
	local = ReturnFixedFieldWidthString(psData.LiveTime, fwf_precision_time);
	AppendToOutputString(local, fwf_livetime, true);
}


void CNFobject::WriteCommonData_Type2()
{
	std::string local = ReturnSimpleFilename();
	AppendToOutputString(local, fwf_filename, true);

	local.clear();
	local = psData.AcquisitionStart;
	AppendToOutputString(local, fwf_acquisition_start, true);

	local.clear();
	local = psData.SampleID;
	AppendToOutputString(local, fwf_samplie_id, true);

	local.clear();
	local = ReturnFixedFieldWidthString(psData.DeadTimePct, fwf_precision_percent);
	AppendToOutputString(local, fwf_deadtime_pct, true);
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


std::string CNFobject::ReturnFixedWidthString(std::string& the_string, const size_t field_width)
{
	std::stringstream ss;
	ss << std::setw(field_width) << the_string;
	return ss.str();
}


void CNFobject::AddComma()
{
	output_string += std::string(", ");
}


//std::string CNFobject::ReturnFixedFieldWidthString(FLOAT& the_number, const size_t precision)
//{
//	std::stringstream ss;
//	ss << std::fixed << std::setprecision(precision) << the_number;
//	return ss.str();
//}


void CNFobject::WriteNuclideData()
{
	switch (output_option)
	{
	case OutputOption::a:
		// Nuclide data: peak area, iterations, FWHM, peak energy
		WriteNuclideData_Type1();
		break;

	case OutputOption::b:
		// Nulcide data: peak area, FWHM

		break;

	case OutputOption::c:
		// Nuclide data: peak area, error (%), FWHM, peak energy

		break;

	case OutputOption::d:
		// Nuclide data: peak area, FWHM, error (%)
		
		break;
	}
}


void CNFobject::WriteNuclideData_Type1()
{
	std::string local;
	USHORT index_value;
	bool last_nuclide, last_line;
	bool do_comma{ true };

	for (LONG i{ 0 }; i < psLibrary.size(); i++)
	{
		if ((i + 1) == psLibrary.size())
			last_nuclide = true;

		for (LONG j{ 0 }; j < psLibrary.at(i).PeakEnergy.size(); j++)
		{
			if ((j + 1) == psLibrary.at(i).PeakEnergy.size())
			{
				last_line = true;
				if (last_nuclide && last_line)
					do_comma = false;
			}

			index_value = psLibrary.at(i).PeakSearchResult.at(j);
			
			local = ReturnPeakArea(i, j);
			AppendToOutputString(local, fwf_peak_area, true);

			local.clear();
			local = ReturnIterations(i, j);
			AppendToOutputString(local, fwf_iterations, true);

			
			local.clear();
			local = ReturnPeakFWHM(i, j);
			AppendToOutputString(local, fwf_peak_fwhm, true);

			local.clear();
			local = ReturnPeakEnergy(i, j);
			AppendToOutputString(local, fwf_peak_energy, do_comma);
		}
	}
}


std::string CNFobject::ReturnPeakArea(const USHORT nuclide, const USHORT line)
{
	FLOAT x = psData.Nuclides.at(nuclide).Area.at(line);
	LONG y = ConvertFLOATtoLONG(x);
	return std::string(ReturnFixedIntegerString(y));
}


LONG CNFobject::ConvertFLOATtoLONG(const FLOAT& the_float)
{
	return static_cast<LONG>(the_float);
}


std::string CNFobject::ReturnFixedIntegerString(LONG& the_number)
{
	std::stringstream ss;
	ss << the_number;
	return ss.str();
}


std::string CNFobject::ReturnIterations(const USHORT nuclide, const USHORT line)
{
	LONG ret = psData.Nuclides.at(nuclide).Iterations.at(line);
	return std::string(ReturnFixedIntegerString(ret));
}


std::string CNFobject::ReturnPeakFWHM(const USHORT nuclide, const USHORT line)
{
	FLOAT ret = psData.Nuclides.at(nuclide).FWHM.at(line);
	return std::string(ReturnFixedFieldWidthString(ret, fwf_precision_energy));
}


std::string CNFobject::ReturnPeakEnergy(const USHORT nuclide, const USHORT line)
{
	FLOAT ret = psData.Nuclides.at(nuclide).Energy.at(line);
	return std::string(ReturnFixedFieldWidthString(ret, fwf_precision_energy));
}