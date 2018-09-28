#pragma once

// Imports
#import "c:\genie2k\s560\tlb\DataAccess.tlb"
#import "c:\genie2k\s560\tlb\Analyze.tlb"

// Includes
#include "ProgramDefaults.h"
#include "Crackers.h"


class CAMobject
{
	CAMobject();
	~CAMobject();

private:
	CanberraDataAccessLib::IDataAccessPtr iPtr;
	HMEM dsc;
};