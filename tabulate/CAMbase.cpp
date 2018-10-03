#include "stdafx.h"
#include "CAMbase.h"


CAMbase::CAMbase()
{
}


CAMbase::~CAMbase()
{
	if ((pDataAccess != nullptr) && (pDataAccess->IsOpen))
		pDataAccess->Close(close_mode);
	
	if (DSC != nullptr) 
		DSC = nullptr;

	TRACE("\nCAMbase dtor\n");
}


void CAMbase::CreateDataAccessInterface()
{
	HRESULT hr{ pDataAccess.CreateInstance(__uuidof(CanberraDataAccessLib::DataAccess)) };
	if (FAILED(hr)) 
	{
		pDataAccess = nullptr;
		TRACE("\nCAM Error: DataAccess interface not created");
	}
}


void CAMbase::CreateDataSourceContext()
{
	if (pDataAccess->IsOpen)
		DSC = reinterpret_cast<HMEM*>(pDataAccess->AccessHandle);
	else
		TRACE("CAM Error: file is not open");
}


void CAMbase::OpenFile(const std::string name,
	const CanberraDataAccessLib::OpenMode om,
	const CanberraDataAccessLib::CloseMode cm)
{
	CreateDataAccessInterface();

	SetFilename(name);
	SetModes(om, cm);
	pDataAccess->Open(file_name, open_mode, NULL);
	
	CreateDataSourceContext();
}


void CAMbase::OpenFile(const std::string name)
{
	CreateDataAccessInterface();
	
	SetFilename(name);
	SetModes();
	pDataAccess->Open(file_name, open_mode, NULL);
	
	CreateDataSourceContext();
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


LONG CAMbase::ReturnRecordCount(const USHORT class_code)
{
	USHORT count;
	SHORT ret = SadClassCount(DSC, class_code, &count);
	if (ret != CSI_Okay)
	{
		ReportSadError(ret);
		count = 0;
	}
	return count;
}


std::string CAMbase::ReturnStringParam(ULONG param, USHORT record, USHORT buff_len)
{
	std::string retval;
	std::vector<char> buff;
	buff.resize(buff_len + 1);	// extra space for null terminator, "just in case"
	std::fill(buff.begin(), buff.end(), '\0');
	SHORT ret = SadGetParam(DSC, param, record, 1, &buff[0], buff_len);
	if (ret != CSI_Okay)
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


void CAMbase::ReportSadError(const SHORT error_code)
{
	SHORT sdummy;
	USHORT usdummy;
	ULONG ulrtv;
	SadGetStatus(DSC, &ulrtv, &sdummy, &usdummy);
	TRACE("\nSAD Error: 0x%10x\n", ulrtv);
}
