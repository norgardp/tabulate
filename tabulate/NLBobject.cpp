#include "stdafx.h"

#include "NLBobject.h"


NLBobject::NLBobject()
{
}


NLBobject::NLBobject(const std::string name)
{
	OpenFile(name);
}


NLBobject::~NLBobject()
{
}


void NLBobject::PopulateDataStruct()
{
	USHORT record;

	for (size_t i{ 0 }; i < 10; i++)
	{
		record = i + 1;
		libData.at(i).NuclideName = ReturnStringParam(CAM_T_NCLNAME, record, CAM_N_NCLNAME);
	}
	
}


void NLBobject::GetLibraryDimensions()
{

	//
}
