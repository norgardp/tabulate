#pragma once

// Standard library
#include <string>
#include <vector>

// This program
#include "OutputOptions.h"

// Genie
#include "citypes.h"

namespace DataStructure
{
	namespace NuclideStructure
	{
		struct NuclideStruct
		{
			FLOAT Energy;		// CAM_F_PSENERGY
			FLOAT Area;			// CAM_F_PSAREA
			FLOAT FWHM;			// CAM_F_PSFWHM
			LONG Iterations;	// CAM_L_PSITER
			FLOAT Rate;			// CAM_F_PSCTSS
			FLOAT Error;		// CAM_F_PSCERR
			std::string NucName;// from LibraryStruct
			FLOAT IdealEnergy;	// frmo LibratyStruct
		};
	}

	struct DataStruct
	{
		std::string SampleID;			// CAM_L_SIDENT
		double SampleQuantity;			// CAM_F_SQUANT
		std::string AcquisitionStart;	// CAM_X_ASTIME
		double RealTime;				// CAM_X_EREAL
		double LiveTime;				// CAM_X_ELIVE
		double DeadTimePct;				// 100*(Treal-Tlive)/Treal
		std::vector<DataStructure::NuclideStructure::NuclideStruct> Nuclides;
	};

	struct InitializationOptions
	{
		std::string Library;
		std::string Analysis;
		bool OverwriteMode;
		OutputOption OutputFormat;
		double EnergyTolerance;
		std::vector<LibraryStruct> NuclidesInLibrary;
	};

}