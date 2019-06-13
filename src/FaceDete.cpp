#include "../inc/FaceDete.h"

FaceDete::FaceDete():
	APPID(),
	SDKKey()
{
}

FaceDete::~FaceDete()
{
	if (APPID) {
		delete APPID;
	}
	if (SDKKey) {
		delete SDKKey;
	}
}

void FaceDete::Activation()
{
	res = ASFActivation(APPID, SDKKey);
	if (MOK != res && MERR_ASF_ALREADY_ACTIVATED != res)
		printf("ASFActivation fail: %d\n", res);
	else
		printf("ASFActivation sucess: %d\n", res);
}

void FaceDete::InitEngine()
{
	handle = NULL;
	MInt32 mask = ASF_FACE_DETECT | ASF_FACERECOGNITION | ASF_AGE | ASF_GENDER | ASF_FACE3DANGLE | ASF_LIVENESS;
	res = ASFInitEngine(ASF_DETECT_MODE_IMAGE, ASF_OP_0_ONLY, 16, 10, mask, &handle);
	if (res != MOK)
		printf("ASFInitEngine fail: %d\n", res);
	else
		printf("ASFInitEngine sucess: %d\n", res);
}

size_t FaceDete::Loadregface()
{
	// ...
	return 0;
}

void FaceDete::UninitEngine()
{
	MRESULT res = ASFUninitEngine(handle);
	if (res != MOK)
		printf("ALUninitEngine fail: %d\n", res);
	else
		printf("ALUninitEngine sucess: %d\n", res);
}

const ASF_VERSION* FaceDete::GetVersion()
{
	const ASF_VERSION* pVersionInfo = ASFGetVersion(handle);
	return pVersionInfo;
}


void FaceDete::DetectFaces(Mat image, vector<DetectedResult>& detectedResultVec, bool opt)
{
}

//人脸对比
size_t FaceDete::CompareFeature(const ASF_FaceFeature& f)
{
	// ... 
	return 0;
}

void FaceDete::DrawRetangle(Mat & frame, MInt32 faceRect[4])
{
}

//框定人脸
void DrawRetangle(Mat& frame, MInt32 faceRect[4])
{
	// ... 
}

void FaceDete::SetAPPID(const string appid)
{
	APPID = new char[appid.size() + 1]();
	strncpy(APPID, appid.data(), appid.size());
	APPID[appid.size()] = '\0';
}

void FaceDete::SetSDKKey(const string sdkkey)
{
	APPID = new char[sdkkey.size() + 1]();
	strncpy(APPID, sdkkey.data(), sdkkey.size());
	APPID[sdkkey.size()] = '\0';
}

void FaceDete::SetPreloadPath(string path)
{
	this->preloadPath = path;
}
