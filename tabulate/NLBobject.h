#pragma once

// Standard libraries
#include <string>
#include <vector>

// This program
#include "CAMbase.h"
#include "LibraryStruct.h"

// Genie
#include "citypes.h"
#include "cam_n.h"
#include "camdef.h"
#include "campdef.h"


class NLBobject :
	public CAMbase
{
public:
	NLBobject();
	~NLBobject();
	NLBobject(const std::string name);
	NLBobject(const _bstr_t name);

	size_t ReturnNuclideCount();
	void PopulateDataStruct();
	std::string ReturnFormattedLine(const size_t i);
	LibraryDimension& ReturnLibraryDimension();
	std::vector<LibraryStruct>& ReturnLibraryData();

private:
	void GetLibraryDimensions();
	void ResizeDataSetObject();
	void PopulateNuclideNames();
	void PopulateNuclideLines();
	std::string ReturnEnergyList(const size_t i);

private:
	std::vector<LibraryStruct> libData;
	LibraryDimension libDim;
};

