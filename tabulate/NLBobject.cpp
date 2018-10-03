#include "stdafx.h"

#include "NLBobject.h"

NLBobject::NLBobject()
{
}


NLBobject::NLBobject(const std::string name)
{
	OpenFile(name);
	PopulateDataStruct();
}


NLBobject::~NLBobject()
{
	TRACE("\nNLBobject dtor\n");
}


void NLBobject::PopulateDataStruct()
{
	GetLibraryDimensions();
	ResizeDataSetObject();
	PopulateNuclideNames();
	PopulateNuclideLines();
}


void NLBobject::GetLibraryDimensions()
{
	nuclide_cnt = ReturnRecordCount(CAM_CLS_NUCL);
	line_cnt = ReturnRecordCount(CAM_CLS_NLINES);
}


void NLBobject::ResizeDataSetObject()
{
	libData.resize((size_t)nuclide_cnt);
}


void NLBobject::PopulateNuclideNames()
{
	USHORT record;
	for (USHORT i{ 0 }; i < nuclide_cnt; i++)
	{
		record = i + 1;
		libData.at(i).NuclideName = ReturnStringParam(CAM_T_NCLNAME, record, CAM_N_NCLNAME);
	}
}


void NLBobject::PopulateNuclideLines()
{
	USHORT record;
	LONG line{ 0 };
	LONG nuclide_no{ 0 };
	FLOAT energy{ 0.0 };
	for (USHORT i{ 0 }; i < line_cnt; i++)
	{
		record = i + 1;
		energy = ReturnNumericParam(CAM_F_NLENERGY, record, energy);
		nuclide_no = ReturnNumericParam(CAM_L_NLNUCL, record, nuclide_no);
		--nuclide_no;
		libData.at(nuclide_no).PeakEnergy.push_back(energy);
	}
}


size_t NLBobject::ReturnNuclideCount()
{
	size_t size{ (size_t)nuclide_cnt };
	return size;
}


std::string NLBobject::ReturnFormattedLine(const size_t i)
{
	std::string outstring{ libData.at(i).NuclideName };
	outstring += "      ";
	outstring += ReturnEnergyList(i);
	return outstring;
}


std::string NLBobject::ReturnEnergyList(const size_t i)
{
	size_t count{ libData.at(i).PeakEnergy.size() };
	std::string outstring;
	for (size_t k{ 0 }; k < count; k++)
	{
		outstring += std::to_string(libData.at(i).PeakEnergy.at(k));
		if(k < (count-1))
			outstring += ", ";
	}
	outstring += "\n";
	return outstring;
}