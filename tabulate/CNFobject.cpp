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


std::string CNFobject::ReturnFormattedLine()
{
	// NOT DONE HERE ================================================================== NOT DONE HERE
	return std::string();
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
{
	LONG nuclide_lines;
	SHORT match;
	USHORT nuclide_match;
	OjEnergy_T lib_peak;
	// ====================================================================> STOPPED HERE 8 Oct
	// ====================================================================> Next step, verify loop works the way I need it to
	lib_peak.rToler = energy_tolerance;
	for (LONG i{ 0 }; i < psLibrary.size(); i++)
	{
		nuclide_lines = psLibrary.at(i).PeakEnergy.size();
		for (LONG j{ 0 }; j < nuclide_lines; j++)
		{
			lib_peak.rEnergy = psLibrary.at(i).PeakEnergy.at(j);
			match = ReturnPeakSearchIndex(lib_peak);
			if (match >= 0)
				AppendToOutputString((USHORT)match);
		}
	}
}


void CNFobject::AppendToOutputString(const USHORT match)
{
	// nothing here yet
}