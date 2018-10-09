#pragma once

// Standard library
#include <string>
#include <vector>

// Genie
#include "citypes.h"

struct LibraryStruct
{
	std::string NuclideName;		// CAM_T_NCLNAME
	std::vector<FLOAT> PeakEnergy;	// CAM_F_NLENERGY
	std::vector<USHORT> PeakSearchResult;
};

struct LibraryDimension
{
	LONG NumberOfNuclides;
	LONG NumberOfLines;
};