#include "..\inc\DetectedResult.h"

DetectedResult::DetectedResult()
{
	feature.feature = nullptr;
	feature.feature = nullptr;
	genderInfo.genderArray = nullptr;
	livenessInfo.isLive = nullptr;
	confidenceLevel = 0;
	identifiable = false;
	indexInPreload = -1;
	// ... 
}


DetectedResult::~DetectedResult()
{

}
