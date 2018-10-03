#pragma once

#include <string>
#include <vector>
#include <iostream>

// Imports
#import "c:\genie2k\s560\tlb\DataAccess.tlb"

// Includes
#include "ProgramDefaults.h"
#include "crackers.h"
#include "citypes.h"
#include "spasst.h"
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
	LONG ReturnRecordCount(const USHORT class_code); 
	std::string ReturnStringParam(ULONG param, USHORT  record, USHORT buff_len);
	template<typename T>
	const T& ReturnNumericParam(ULONG param, USHORT record, const T& type);

private:
	void CreateDataAccessInterface();
	void CreateDataSourceContext();
	void SetFilename(const std::string name);
	void SetModes();
	void SetModes(const CanberraDataAccessLib::OpenMode om,
		const CanberraDataAccessLib::CloseMode cm);
	void ReportSadError(const SHORT error_code);

protected:
	CanberraDataAccessLib::IDataAccessPtr pDataAccess;
	CanberraDataAccessLib::OpenMode open_mode;
	CanberraDataAccessLib::CloseMode close_mode;
	_bstr_t file_name;
	HMEM DSC;
};


template<typename T>
const T& CAMbase::ReturnNumericParam(ULONG param, USHORT record, const T& type)
{
	T retval;
	SHORT ret = SadGetParam(DSC, param, record, 1, &retval, sizeof(type));
	if (ret)
		ReportSadError(ret);
	return retval;
}