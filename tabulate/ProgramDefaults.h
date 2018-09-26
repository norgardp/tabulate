#pragma once
#include "stdafx.h"

// Default directories
static constexpr TCHAR* default_genie_data_directory = _T("c:\\genie2k\\camfiles\\");
static constexpr TCHAR* default_genie_library_directory = _T("c:\\genie2k\\ctlfiles\\");
static constexpr TCHAR* default_genie_control_directory = _T("c:\\genie2k\\ctlfiles\\");
static constexpr TCHAR* default_genie_report_directory = _T("c:\\genie2k\\repfiles\\");


// Output option descriptions
static constexpr TCHAR* output_option_a = _T("Sample ID and Percent Dead Time; for each energy line\n - Peak Area\n - Iterations\n - Peak FWHM (keV)\n - Peak Energy (keV)");
static constexpr TCHAR* output_option_b = _T("Real Time and Corrected Live Time; for each energy line\n - Peak Area\n - Peak FWHM (keV)");
static constexpr TCHAR* output_option_c = _T("Sample ID and Percent Dead Time; for each energy line\n - Peak Area\n - Error (%)\n - Peak FWHM (keV)\n - Peak Energy (keV)");
static constexpr TCHAR* output_option_d = _T("Real Time and Corrected Live Time; for each energy line\n - Peak Area\n - Peak FWHM (keV)\n - Error (%)");

// Energy tolerance used for library directed peak search
static constexpr float default_energy_tolerance = 2.50;