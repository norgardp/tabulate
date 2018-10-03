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

private:
	std::vector<LibraryStruct> libData;
	LONG nuclide_cnt;
	LONG line_cnt;

public:
	size_t ReturnNuclideCount();
	std::string ReturnFormattedLine(const size_t i);

private:
	void PopulateDataStruct();
	void GetLibraryDimensions();
	void ResizeDataSetObject();
	void PopulateNuclideNames();
	void PopulateNuclideLines();
	std::string ReturnEnergyList(const size_t i);
};

