#include "stdafx.h"
#include "CAMbase.h"


CAMbase::CAMbase()
{
	pDataAccess = CreateDataAccessInterface();
	DSC = ReturnDataSourceContext();
}


CAMbase::~CAMbase()
{
	if (IsOpen()) 
		pDataAccess->Close(close_mode);
	
	if (DSC != nullptr) 
		DSC = nullptr;
}


CanberraDataAccessLib::IDataAccessPtr CAMbase::CreateDataAccessInterface()
{
	CanberraDataAccessLib::IDataAccessPtr ptr{ nullptr };
	HRESULT hr{ ptr.CreateInstance(__uuidof(CanberraDataAccessLib::DataAccess)) };
	if (FAILED(hr))
		ptr = nullptr;
	return ptr;
}


HMEM CAMbase::ReturnDataSourceContext()
{
	return reinterpret_cast<HMEM*>(pDataAccess->AccessHandle);
}


void CAMbase::OpenFile(const std::string name,
	const CanberraDataAccessLib::OpenMode om,
	const CanberraDataAccessLib::CloseMode cm)
{
	SetFilename(name);
	SetModes(om, cm);
	pDataAccess->Open(file_name, open_mode, NULL);
}


void CAMbase::OpenFile(const std::string name)
{
	SetFilename(name);
	SetModes();
	pDataAccess->Open(file_name, open_mode, NULL);
}


void CAMbase::SetFilename(const std::string name)
{
	if (!name.empty())
		file_name = name.c_str();
}


void CAMbase::SetModes(const CanberraDataAccessLib::OpenMode om,
	const CanberraDataAccessLib::CloseMode cm)
{
	open_mode = om;
	close_mode = cm;
}


void CAMbase::SetModes()
{
	open_mode = CanberraDataAccessLib::OpenMode::dReadOnly;
	close_mode = CanberraDataAccessLib::CloseMode::dNoUpdate;
}


bool CAMbase::IsOpen()
{
	BOOL return_value = pDataAccess->IsOpen;
	return (return_value == TRUE) ? true : false;
}


std::string CAMbase::ReturnStringParam(ULONG param, USHORT record, USHORT buff_len)
{
	std::vector<char> buff;
	std::string retval;
	buff.reserve(buff_len + 1);	// extra space for null terminator, "just in case"
	SHORT ret = SadGetParam(DSC, param, record, 1, &buff[0], buff_len);
	if (ret)
	{
		ReportSadError(ret);
	}
	else
	{
		buff.push_back('\0');
		retval = &buff[0];
	}
	return retval;
}


LONG CAMbase::ReturnRecordCount(const USHORT class_code)
{
	USHORT count;
	SHORT ret = SadClassCount(DSC, class_code, &count);
	if (ret)
	{
		ReportSadError(ret);
		count = 0;
	}
	return count;
}


void CAMbase::ReportSadError(const SHORT error_code)
{
	SHORT srtv, sdummy;
	USHORT usdummy;
	ULONG ulrtv;
	SadGetStatus(DSC, &ulrtv, &sdummy, &usdummy);
	std::cerr << "SAD Error: ";
	std::cerr.setf(std::ios::hex, std::ios::basefield);  // set hex as the basefield
	std::cerr.setf(std::ios::showbase); 
	std::cerr << ulrtv << std::endl;
}
