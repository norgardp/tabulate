#pragma once

// Imports
#import "c:\genie2k\s560\tlb\DataAccess.tlb"

// Standard library
#include <string>
#include <vector>
#include <iostream>

// This program
#include "ProgramDefaults.h"

// Genie
#include "crackers.h"
#include "citypes.h"
#include "spasst.h"
#include "sad.h"
#include "ci_files.h"
#include "campdef.h"
#include "cam_n.h"
#include "utility.h"

class CAMbase
{
public:
	CAMbase();
	virtual ~CAMbase();
	bool IsOpen();
	void OpenFile(const std::string& name,
		const CanberraDataAccessLib::OpenMode om,
		const CanberraDataAccessLib::CloseMode cm); 
	void OpenFile(const std::string& name);
	void OpenFile(const _bstr_t& name);
	LONG ReturnRecordCount(const USHORT class_code); 
	std::string ReturnStringParam(ULONG param, USHORT  record, USHORT buff_len);
	template<typename T>
	const T ReturnNumericParam(ULONG param, USHORT record, const T& type);
	
	std::string ReturnTimeSParam(ULONG param, USHORT record);
	double ReturnTimeNParam(ULONG param, USHORT record);

	template<typename T>
	void SetNumericParam(ULONG param, USHORT record, const T& type);
	void SetStringParam(ULONG param, USHORT record, std::string& data, USHORT buff_len);

	SHORT ReturnPeakSearchIndex(const OjEnergy_T& peak_to_find);

private:
	void CreateDataAccessInterface();
	void CreateDataSourceContext();
	void SetFilename(const std::string& name);
	void SetModes();
	void SetModes(const CanberraDataAccessLib::OpenMode om,
		const CanberraDataAccessLib::CloseMode cm);
	void ReportSadError(const SHORT error_code);
	std::string ConvertBSTR(const _bstr_t& str);
	double ReturnCAMTime(ULONG param, USHORT record);
	std::string ConvertCAMTimeToString(const double& cam_time);
	std::string ConvertCTimeToString(const tm& t);

protected:
	CanberraDataAccessLib::IDataAccessPtr pDataAccess;
	CanberraDataAccessLib::OpenMode open_mode;
	CanberraDataAccessLib::CloseMode close_mode;
	_bstr_t file_name;
	HMEM DSC;
};


template<typename T>
const T CAMbase::ReturnNumericParam(ULONG param, USHORT record, const T& type)
{
	T retval;
	SHORT ret = SadGetParam(DSC, param, record, 1, &retval, sizeof(type));
	if (ret)
		ReportSadError(ret);
	return retval;
}

template<typename T>
void CAMbase::SetNumericParam(ULONG param, USHORT record, const T& data)
{
	T locdata{ data };
	SHORT ret = SadPutParam(DSC, param, record, 1, &locdata, sizeof(locdata));
	if (ret)
		ReportSadError(ret);
}