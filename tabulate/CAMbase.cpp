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


void CAMbase::OpenFile(const std::string& name,
	const CanberraDataAccessLib::OpenMode om,
	const CanberraDataAccessLib::CloseMode cm)
{
	CreateDataAccessInterface();

	SetFilename(name);
	SetModes(om, cm);
	pDataAccess->Open(file_name, open_mode, NULL);
	
	CreateDataSourceContext();
}


void CAMbase::OpenFile(const std::string& name)
{
	CreateDataAccessInterface();
	
	SetFilename(name);
	SetModes();
	pDataAccess->Open(file_name, open_mode, NULL);
	
	CreateDataSourceContext();
}


void CAMbase::OpenFile(const _bstr_t& name)
{
	std::string ret{ ConvertBSTR(name) };
	OpenFile(ret);
}


void CAMbase::SetFilename(const std::string& name)
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


double CAMbase::ReturnCAMTime(ULONG param, USHORT record)
{
	double cam_time;
	SHORT ret = SadGetParam(DSC, param, record, 1, &cam_time, sizeof(cam_time));
	if (ret != CSI_Okay)
	{
		ReportSadError(ret);
		cam_time = 0.0;
	}
	return cam_time;
}


std::string CAMbase::ReturnTimeSParam(ULONG param, USHORT record)
{
	double cam_time;
	cam_time = ReturnCAMTime(param, record);
	return ConvertCAMTimeToString(cam_time);
}


double CAMbase::ReturnTimeNParam(ULONG param, USHORT record)
{
	return ReturnCAMTime(param, record);
}


void CAMbase::ReportSadError(const SHORT error_code)
{
	SHORT sdummy;
	USHORT usdummy;
	ULONG ulrtv;
	SadGetStatus(DSC, &ulrtv, &sdummy, &usdummy);
	TRACE("\nSAD Error: 0x%10x\n", ulrtv);
}


std::string CAMbase::ConvertBSTR(const _bstr_t& str)
{
	size_t len = str.length();
	std::vector<char> outstring;
	outstring.resize(len);
	strcpy_s(&outstring[0], len + 1, str);
	std::string ret{ &outstring[0] };
	return ret;
}


std::string CAMbase::ConvertCAMTimeToString(const double& cam_time)
{
	tm time;
	fUtlCAMToCTime(cam_time, &time);
	return ConvertCTimeToString(time);
}


std::string CAMbase::ConvertCTimeToString(const tm& t)
{
	std::string time_fmt{ "%d-%b-%Y %H:%M:%S" };
	std::vector<char> time_str;
	time_str.resize(GENIE_MAX_TIMESTRING);
	strftime(&time_str[0], time_str.size(), time_fmt.c_str(), &t);
	std::string ret(time_str.data(), time_str.size());
	return ret;
}


void CAMbase::SetStringParam(ULONG param, USHORT record, std::string& data, USHORT buff_len)
{
	std::string locdata{ data };
	SHORT ret = SadPutParam(DSC, param, record, 1, &locdata[0], buff_len);
	if (ret)
		ReportSadError(ret);
}

int CAMbase::ReturnPeakSearchIndex(const OjEnergy_T& find_me)
{
	OjEnergy_T loc_findme = find_me;
	int ret;
	USHORT match;
	FLAG found;

	ret = SadFindRecord(DSC, SEEK_Peak, &loc_findme, 1, &match, &found);
	if (ret != CSI_Okay)
	{
		ReportSadError(ret);
		ret = -2;
	}
	else
	{
		if (found == TRUE)
			ret = match;
		else
			ret = -1;
	}
	return ret;
}


std::string CAMbase::ReturnFilename()
{
	std::string out{ file_name };
	return out;
}