#pragma once

#import "c:\genie2k\s560\tlb\Analyze.tlb"

#include "CAMbase.h"
#include "LibraryStruct.h"
#include "citypes.h"
#include "cam_n.h"
#include "camdef.h"
#include "campdef.h"

#include <string>
#include <vector>

class NLBobject :
	public CAMbase
{
public:
	NLBobject();
	~NLBobject();
	NLBobject(const std::string name);

private:
	std::string library_filename;
	std::vector<LibraryStruct> libData;
	

private:
	void PopulateDataStruct();
	void GetLibraryDimensions();
};

