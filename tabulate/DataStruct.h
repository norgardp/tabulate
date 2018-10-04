#pragma once

#include <string>
#include <vector>
#include "citypes.h"

namespace DataStructure
{
	namespace NuclideStructure
	{
		struct NuclideStruct
		{
			std::string NuclideName;
			std::vector<REAL> PeakEnergy;	// CAM_F_PSENERGY
			std::vector<REAL> PeakArea;		// CAM_F_PSAREA
			std::vector<REAL> PeakFWHM;		// CAM_F_PSFWHM
			std::vector<REAL> PeakError;	// CAM_F_PSCERR
			std::vector<LONG> PSIterations;	// CAM_L_PSITER
		};
	}

	struct DataStruct
	{
		std::string SampleID;			// CAM_L_SIDENT
		float SampleQuantity;			// CAM_F_SQUANT
		std::string AcquisitionStart;	// CAM_X_ASTIME
		double RealTime;				// CAM_X_EREAL
		double LiveTime;				// CAM_X_ELIVE
		double PileupFactor;			// CAM_F_PILEUP
		double CorrectedLiveTime;		// Tlive/exp[PF*(Treal/Tlive - 1)]; PF = CAM_F_PILEUP
		double CorrectedLiveTime2;		// CAM_X_PPELIVE
		double DeadTimePct;				// 100*(Treal-Tlive)/Treal
		std::vector<DataStructure::NuclideStructure::NuclideStruct> Nuclides;
	};

}