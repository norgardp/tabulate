#pragma once
#include "stdafx.h"

#define GENIE_MAX_PATH 250
#define GENIE_MAX_DELETE 200
#define CNF_MAX_LENGTH 18
#define MAX_CFileDialog_FILE_COUNT 99
#define FILE_LIST_BUFFER_SIZE ((MAX_CFileDialog_FILE_COUNT*(MAX_PATH+1))+1)
#define LISTBOX_WIDTH_FACTOR 1.01

// Peak search energy tolerance acceptable limits
#define GENIE_MAX_ETOL 5.0
#define GENIE_MIN_ETOL 0.1

// Time string lengths
#define GENIE_MAX_TIMESTRING 21

// Default directories
static constexpr TCHAR* default_genie_data_directory = _T("c:\\genie2k\\camfiles\\");
static constexpr TCHAR* default_genie_library_directory = _T("c:\\genie2k\\camfiles\\");
static constexpr TCHAR* default_genie_control_directory = _T("c:\\genie2k\\ctlfiles\\");
static constexpr TCHAR* default_genie_report_directory = _T("c:\\genie2k\\repfiles\\");

// Default extension
static constexpr TCHAR* default_library_extension = _T("*.nlb");
static constexpr TCHAR* default_data_extension = _T("*.cnf");
static constexpr TCHAR* default_analysis_extension = _T("*.asf");

// Default filters
static constexpr TCHAR* default_configuration_filter = _T("Configuration Files (*.cnf)|*.cnf|");
static constexpr TCHAR* default_library_filter = _T("Nuclide Library Files (*.cnf)|*.nlb|(*.elb)|*.elb|");
static constexpr TCHAR* default_analysis_filter = _T("Analysis Sequence Files (*.asf)|*.asf|");

// Output option descriptions
static constexpr TCHAR* output_option_a = _T("Sample ID and Percent Dead Time; for each energy line\n - Peak Area\n - Iterations\n - Peak FWHM (keV)\n - Peak Energy (keV)");
static constexpr TCHAR* output_option_b = _T("Real Time and Corrected Live Time; for each energy line\n - Peak Area\n - Peak FWHM (keV)");
static constexpr TCHAR* output_option_c = _T("Sample ID and Percent Dead Time; for each energy line\n - Peak Area\n - Error (%)\n - Peak FWHM (keV)\n - Peak Energy (keV)");
static constexpr TCHAR* output_option_d = _T("Real Time, Corrected Live Time, and Percent Dead time;\n for each energy line\n - Peak Area\n - Peak FWHM (keV)\n - Error (%)");

// Energy tolerance used for library directed peak search
static constexpr double default_energy_tolerance = 2.50;


// Fixed field widths
// Output file fixed width field lengths: common data fields
static constexpr size_t fwf_filename = 30;
static constexpr size_t fwf_acquisition_start = 20;
static constexpr size_t fwf_realtime = 8;
static constexpr size_t fwf_livetime = 8;
static constexpr size_t fwf_sample_id = 16;
static constexpr size_t fwf_deadtime_pct = 6;

// Output file fixed width field lengths: nuclide specific fields
static constexpr size_t fwf_peak_area = 10;
static constexpr size_t fwf_peak_energy = 10;
static constexpr size_t fwf_peak_fwhm = 6;
static constexpr size_t fwf_iterations = 6;
static constexpr size_t fwf_peak_error = 6;

// Fixed precision for doubles
static constexpr size_t fwf_precision_area = 1;
static constexpr size_t fwf_precision_npa = 2; 
static constexpr size_t fwf_precision_time = 2;
static constexpr size_t fwf_precision_percent = 3;
static constexpr size_t fwf_precision_energy = 2;
static constexpr size_t fwf_precision_iterations = 1;
static constexpr size_t fwf_precision_error = 2;