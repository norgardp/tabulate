#pragma once

#import "c:\genie2k\s560\tlb\Analyze.tlb"

#include "CAMbase.h"
#include "DataStruct.h"
#include "LibraryStruct.h"
#include "OutputOptions.h"
#include <string>


class CNFobject :
	public CAMbase
{
public:
	CNFobject();
	CNFobject(const std::string datafile, const OutputOption opt);
	CNFobject(const std::string datafile, 
		const CanberraDataAccessLib::OpenMode om, 
		const CanberraDataAccessLib::CloseMode cm,
		const OutputOption opt);
	~CNFobject();

	void InitiateLibrarySearch(const std::string library);
	
private:
	void InitializeSequenceAnalyzer(); 
	void SetLibraryFilename(const std::string name);
	void SetOutputOption(const OutputOption option);

private:
	CanberraSequenceAnalyzerLib::ISequenceAnalyzerPtr pSequenceAnalyzer;
	_bstr_t library_file_name;
	DataStruct psData;
	std::vector<LibraryStruct> psLibrary;
	SHORT start_step{ 0 };
	OutputOption output_opt;

public:
	void InitiateOutputGenerator();
	std::string ExtractOutputLine();
	void WriteOutputLine(const std::string line);

};

