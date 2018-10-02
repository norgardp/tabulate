#pragma once

#include <string>
#include <vector>
#include <iostream>

// Imports
#import "c:\genie2k\s560\tlb\DataAccess.tlb"

// Includes
#include "ProgramDefaults.h"
#include "CITYPES.H"
#include "Spasst.h"
#include "sad.h"
#include "ci_files.h"
#include "campdef.h"
#include "cam_n.h"

class CAMbase
{
public:
	CAMbase();
	virtual ~CAMbase();
	bool IsOpen();
	void OpenFile(const std::string name,
		const CanberraDataAccessLib::OpenMode om,
		const CanberraDataAccessLib::CloseMode cm); 
	void OpenFile(const std::string name);
	std::string ReturnStringParam(ULONG param, USHORT  record, USHORT buff_len);

private:
	CanberraDataAccessLib::IDataAccessPtr CreateDataAccessInterface();
	HMEM ReturnDataSourceContext();
	void SetFilename(const std::string name);
	void SetModes();
	void SetModes(const CanberraDataAccessLib::OpenMode om,
		const CanberraDataAccessLib::CloseMode cm);

protected:
	CanberraDataAccessLib::IDataAccessPtr pDataAccess;
	CanberraDataAccessLib::OpenMode open_mode;
	CanberraDataAccessLib::CloseMode close_mode;
	_bstr_t file_name;
	HMEM DSC;
	
public:
	LONG ReturnRecordCount(const USHORT class_code);
private:
	void ReportSadError(const SHORT error_code);
};

