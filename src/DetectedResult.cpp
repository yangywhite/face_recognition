#include "DetectedResult.h"

DetectedResult::DetectedResult()
{
	feature.feature = new MByte();
	ageInfo.ageArray = new MInt32();
	genderInfo.genderArray = new MInt32();
	livenessInfo.isLive = new MInt32();
	// ... 
}


DetectedResult::~DetectedResult()
{
	if (feature.feature)
		delete feature.feature;
	if (ageInfo.ageArray)
		delete ageInfo.ageArray;
	if (genderInfo.genderArray)
		delete genderInfo.genderArray;
	if (livenessInfo.isLive)
		delete livenessInfo.isLive;
	// ... 
}
