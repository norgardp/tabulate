#pragma once

#include <string>
#include <vector>
#include "citypes.h"

struct NuclideStruct
{
	std::string NuclideName;
	std::vector<REAL> PeakEnergy;	// CAM_F_PSENERGY
	std::vector<REAL> PeakArea;		// CAM_F_PSAREA
	std::vector<REAL> PeakFWHM;		// CAM_F_PSFWHM
	std::vector<REAL> PeakError;	// CAM_F_PSCERR
	std::vector<LONG> PSIterations;	// CAM_L_PSITER
};

struct DataStruct
{
	std::string SampleID;			// CAM_L_SIDENT
	REAL SampleQuantity;			// CAM_F_SQUANT
	REAL RealTime;					// CAM_X_EREAL
	REAL LiveTime;					// CAM_X_ELIVE
	REAL PileupFactor;				// CAM_F_PILEUP
	REAL CorrectedLiveTime;			// Tlive/exp[PF*(Treal/Tlive - 1)]; PF = CAM_F_PILEUP
	REAL CorrectedLiveTime2;		// CAM_X_PPELIVE
	REAL DeadTimePct;				// 100*(Treal-Tlive)/Treal
	std::vector<NuclideStruct> Nuclides;
};