#include "stdafx.h"

#include "NLBobject.h"

NLBobject::NLBobject()
{
}


NLBobject::NLBobject(const std::string name)
{
	OpenFile(name);
}


NLBobject::NLBobject(const _bstr_t name)
{
	OpenFile(name);
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
	libDim.NumberOfNuclides = ReturnRecordCount(CAM_CLS_NUCL);
	libDim.NumberOfLines = ReturnRecordCount(CAM_CLS_NLINES);
}


void NLBobject::ResizeDataSetObject()
{
	libData.resize((size_t)libDim.NumberOfNuclides);
}


void NLBobject::PopulateNuclideNames()
{
	USHORT record;
	for (USHORT i{ 0 }; i < libDim.NumberOfNuclides; i++)
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
	for (USHORT i{ 0 }; i < libDim.NumberOfLines; i++)
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
	size_t size{ (size_t)libDim.NumberOfNuclides };
	return size;
}


std::string NLBobject::ReturnFormattedLine(const size_t i)
{
	std::string outstring{ libData.at(i).NuclideName };
	size_t current_size{ outstring.size() };
	for (size_t j{ 15 - current_size }; j > 0; j--)
		outstring.push_back(' ');
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


LibraryDimension& NLBobject::ReturnLibraryDimension()
{
	return libDim;
}


std::vector<LibraryStruct>& NLBobject::ReturnLibraryData()
{
	return libData;
}