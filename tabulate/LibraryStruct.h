#pragma once

#include <string>
#include <vector>
#include "citypes.h"

struct LibraryStruct
{
	std::string NuclideName;		// CAM_T_NCLNAME
	std::vector<FLOAT> PeakEnergy;	// CAM_F_NLENERGY
};
