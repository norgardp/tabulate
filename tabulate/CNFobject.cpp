#include "stdafx.h"
#include "CNFobject.h"


CNFobject::CNFobject()
{
	InitializeSequenceAnalyzer();
}


CNFobject::CNFobject(const std::string lib_name, 
	const bool overwrite, 
	const OutputOption output_opt)
{
	CNFobject();
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


void CNFobject::SetOutputOption(const OutputOption option)
{
	output_option = option;
}


void CNFobject::SetOverwriteMode(const bool mode)
{
	overwrite = mode;
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
			open_mode = CanberraDataAccessLib::OpenMode::dReadOnly;
			close_mode = CanberraDataAccessLib::CloseMode::dNoUpdate;
		}
	}
	OpenFile(datafile, open_mode, close_mode);
}


void CNFobject::PerformAnalysis()
{
	_bstr_t fake;
	if (pSequenceAnalyzer != nullptr)
		pSequenceAnalyzer->Analyze(pDataAccess, &start_step, library_name, 
			FALSE, FALSE, FALSE, TRUE, fake, NULL);
}


std::string CNFobject::ReturnFormattedLine()
{
	// NOT DONE HERE
	return std::string();
}


void CNFobject::GetLibraryDimensions()
{
	NLBobject nlb(library_name);
	libDim = nlb.ReturnLibraryDimension();
}


void CNFobject::PopulateDataStructure()
{
	GetLibraryDimensions();
	ResizeDataStructure();
	PopulateHeaderStructure();
	PopulateNuclideData();
}


void CNFobject::ResizeDataStructure()
{
	psData.Nuclides.resize((size_t)libDim.NumberOfNuclides);
}


void CNFobject::PopulateHeaderStructure()
{
	psData.SampleID = ReturnStringParam(CAM_T_SIDENT, 0, CAM_N_SIDENT);
	psData.SampleQuantity = ReturnNumericParam(CAM_F_SQUANT, 0, sizeof(psData.SampleQuantity));
	psData.AcquisitionStart = ReturnTimeSParam(CAM_X_ELIVE, 0);
	psData.LiveTime = ReturnTimeNParam(CAM_X_ELIVE, 0);
	psData.RealTime = ReturnTimeNParam(CAM_X_EREAL, 0);
	psData.PileupFactor = ReturnNumericParam(CAM_F_PILEUP, 0, sizeof(FLOAT));
	psData.CorrectedLiveTime2 = ReturnTimeNParam(CAM_X_PPELIVE, 0);

	psData.DeadTimePct = 100 * (psData.RealTime - psData.LiveTime) / psData.RealTime;

	double num = psData.LiveTime;
	double den = exp(psData.PileupFactor*(psData.RealTime / psData.LiveTime - 1));
	psData.CorrectedLiveTime = num / den;
}


void CNFobject::PopulateNuclideData()
{

}