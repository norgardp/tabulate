#include "stdafx.h"
#include "CNFobject.h"


CNFobject::CNFobject()
{
}


CNFobject::CNFobject(const std::string name, const OutputOption opt)
{
	OpenFile(name);
	SetOutputOption(opt);
}


CNFobject::CNFobject(const std::string name, 
	const CanberraDataAccessLib::OpenMode om, 
	const CanberraDataAccessLib::CloseMode cm,
	const OutputOption opt)
{
	OpenFile(name, om, cm);
	SetOutputOption(opt);
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
		library_file_name = name.c_str();
}


void CNFobject::SetOutputOption(const OutputOption option)
{
	output_opt = option;
}


void CNFobject::InitiateLibrarySearch(const std::string library)
{
	InitializeSequenceAnalyzer();
	SetLibraryFilename(library);
	_bstr_t fake;
	pSequenceAnalyzer->Analyze(pDataAccess, &start_step, library_file_name, 
		FALSE, FALSE, FALSE, TRUE, fake, NULL);
}


void CNFobject::InitiateOutputGenerator()
{
		std::string output{ ExtractOutputLine() };
		WriteOutputLine(output);
}


std::string CNFobject::ExtractOutputLine()
{
	std::string retstring;
	switch (output_opt)
	{
	case OutputOption::a:
		//_T("Sample ID and Percent Dead Time; for each energy line\n - Peak Area\n - Iterations\n - Peak FWHM (keV)\n - Peak Energy (keV)");
		break;

	case OutputOption::b:
		//_T("Real Time and Corrected Live Time; for each energy line\n - Peak Area\n - Peak FWHM (keV)");
		break;

	case OutputOption::c:
		//_T("Sample ID and Percent Dead Time; for each energy line\n - Peak Area\n - Error (%)\n - Peak FWHM (keV)\n - Peak Energy (keV)");
		break;

	case OutputOption::d:
		//_T("Real Time and Corrected Live Time; for each energy line\n - Peak Area\n - Peak FWHM (keV)\n - Error (%)");
		break;
	}

	return retstring;
}


void CNFobject::WriteOutputLine(const std::string line)
{
	// NOT DONE HERE
}

