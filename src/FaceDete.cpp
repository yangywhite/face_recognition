#define _CRT_SECURE_NO_WARNINGS
#include "..\inc\FaceDete.h"

FaceDete::FaceDete() :
	APPID(), SDKKey()
{

}

FaceDete::~FaceDete()
{
	if (APPID) {
		delete []APPID;
	}
	if (SDKKey) {
		delete []SDKKey;
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
	struct dirent *ptr;
	DIR *dir;
	dir = opendir(preloadPath.c_str());
	vector<string> files;

	Mat img;
	string filename;
	const char *a = "\\";
	vector<DetectedResult> detectedResultVec;

	while ((ptr = readdir(dir)) != NULL)
	{
		if (ptr->d_name[0] == '.')
			continue;

		filename = preloadPath + "\\" + string(ptr->d_name);
		img = imread(filename);
		if (img.empty())
			continue;
		//imshow("edge",img);
		DetectFaces(img, detectedResultVec, true);
		files.push_back(ptr->d_name);
	}
	cout << "reg done!" << endl;
	for (int i = 0; i < files.size(); ++i)
	{
		cout << files[i] << endl;
	}
	closedir(dir);
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


void FaceDete::DetectFaces(Mat& frame, vector<DetectedResult>& detectedResultVec, bool opt)
{
	Mat cutFrame;
	resize(frame, cutFrame, Size(frame.cols - frame.cols % 4, frame.rows));

	ASF_MultiFaceInfo	multiFaceInfo = { 0 };
	ASF_SingleFaceInfo singleFaceInfo = { 0 };
	ASF_FaceFeature feature = { 0 };

	ASF_FaceFeature copyfeature = { 0 };

	res = ASFDetectFaces(handle, cutFrame.cols, cutFrame.rows, ASVL_PAF_RGB24_B8G8R8, cutFrame.data, &multiFaceInfo);

	if (MOK != res)
	{
		//人脸检测失败
		printf("ASFFaceFeatureExtract 1 fail: %d\n", res);
		return;
	}

	if (opt == true) {

		for (MInt32 i = 0; i < multiFaceInfo.faceNum; i++) {
			singleFaceInfo.faceRect.left = multiFaceInfo.faceRect[i].left;
			singleFaceInfo.faceRect.top = multiFaceInfo.faceRect[i].top;
			singleFaceInfo.faceRect.right = multiFaceInfo.faceRect[i].right;
			singleFaceInfo.faceRect.bottom = multiFaceInfo.faceRect[i].bottom;
			singleFaceInfo.faceOrient = multiFaceInfo.faceOrient[i];

			res = ASFFaceFeatureExtract(handle, cutFrame.cols, cutFrame.rows, ASVL_PAF_RGB24_B8G8R8, cutFrame.data, &singleFaceInfo, &feature);

			if (MOK != res)
			{
				//人脸特征提取失败
				printf("asffacefeatureextract 1 fail: %d\n", res);
			}

			copyfeature.featureSize = feature.featureSize;
			copyfeature.feature = new MByte[feature.featureSize];
			for (int i = 0; i != feature.featureSize; i++) {
				copyfeature.feature[i] = feature.feature[i];
			}

			preLoadFeatureVec.push_back(copyfeature);
		}
		cutFrame.release();
	}
	else if (opt == false) {

		for (MInt32 i = 0; i < multiFaceInfo.faceNum; i++) {
			singleFaceInfo.faceRect.left = multiFaceInfo.faceRect[i].left;
			singleFaceInfo.faceRect.top = multiFaceInfo.faceRect[i].top;
			singleFaceInfo.faceRect.right = multiFaceInfo.faceRect[i].right;
			singleFaceInfo.faceRect.bottom = multiFaceInfo.faceRect[i].bottom;
			singleFaceInfo.faceOrient = multiFaceInfo.faceOrient[i];

			res = ASFFaceFeatureExtract(handle, cutFrame.cols, cutFrame.rows, ASVL_PAF_RGB24_B8G8R8, cutFrame.data, &singleFaceInfo, &feature);

			if (MOK != res)
			{
				//人脸特征提取失败
				printf("asffacefeatureextract 1 fail: %d\n", res);
			}
			// 获得所有分析数据

			DetectedResult detectedResult;

			// 获取人脸位置
			detectedResult.faceRect[0] = multiFaceInfo.faceRect[i].left;
			detectedResult.faceRect[1] = multiFaceInfo.faceRect[i].top;
			detectedResult.faceRect[2] = multiFaceInfo.faceRect[i].right;
			detectedResult.faceRect[3] = multiFaceInfo.faceRect[i].bottom;

			// 获取特征值
			detectedResult.feature.featureSize = feature.featureSize;
			detectedResult.feature.feature = new MByte[feature.featureSize];
			for (int i = 0; i < feature.featureSize; i++) {
				detectedResult.feature.feature[i] = feature.feature[i];
			}

			MInt32 processMask = ASF_AGE | ASF_GENDER | ASF_FACE3DANGLE | ASF_LIVENESS;
			res = ASFProcess(handle, cutFrame.cols, cutFrame.rows, ASVL_PAF_RGB24_B8G8R8, cutFrame.data, &multiFaceInfo, processMask);
			//res = ASFProcess(handle, cutImg1->width, cutImg1->height, ASVL_PAF_RGB24_B8G8R8, (MUInt8*)cutImg1->imageData, &multiFaceInfo, processMask);
			if (res != MOK)
				printf("ASFProcess fail: %d\n", res);
			else
				printf("ASFProcess sucess: %d\n", res);

			// 获取年龄
			ASF_AgeInfo ageInfo = { 0 };
			res = ASFGetAge(handle, &ageInfo);
			if (res != MOK)
				printf("ASFGetAge fail: %d\n", res);
			else {
				detectedResult.ageInfo.num = ageInfo.num;
				detectedResult.ageInfo.ageArray = new MInt32[ageInfo.num];
				for (int i = 0; i < ageInfo.num; i++) {
					detectedResult.ageInfo.ageArray[i] = ageInfo.ageArray[i];
				}
			}

			// 获取性别
			ASF_GenderInfo genderInfo = { 0 };
			res = ASFGetGender(handle, &genderInfo);
			if (res != MOK)
				printf("ASFGetGender fail: %d\n", res);
			else
			{
				detectedResult.genderInfo.num = genderInfo.num;
				detectedResult.genderInfo.genderArray = new MInt32[genderInfo.num];
				for (int i = 0; i < genderInfo.num; i++) {
					detectedResult.genderInfo.genderArray[i] = genderInfo.genderArray[i];
				}
			}
			//获取活体信息
			ASF_LivenessInfo livenessInfo = { 0 };
			res = ASFGetLivenessScore(handle, &livenessInfo);
			if (res != MOK)
				printf("ASFGetLivenessScore fail: %d\n", res);
			else
			{
				detectedResult.livenessInfo.num = livenessInfo.num;
				detectedResult.livenessInfo.isLive = new MInt32[livenessInfo.num];
				for (int i = 0; i < livenessInfo.num; i++)
				{
					detectedResult.livenessInfo.isLive[i] = livenessInfo.isLive[i];
				}
			}
			detectedResultVec.push_back(detectedResult);
		}
		cutFrame.release();
	}
}

//人脸对比
size_t FaceDete::CompareFeature(ASF_FaceFeature& p, MFloat confidenceLevel)
{
	for (size_t i = 0; i != preLoadFeatureVec.size(); i++) {
		res = ASFFaceFeatureCompare(handle, &p, &preLoadFeatureVec[i], &confidenceLevel);
		if (res != MOK)
			printf("ASFFaceFeatureCompare fail: %d\n", res);
		else
			printf("ASFFaceFeatureCompare sucess: %lf\n", confidenceLevel);
		if (confidenceLevel >= threshold_confidenceLevel)
			return i + 1;
	}
	return 0;
}



//框定人脸
void FaceDete::DrawRetangle(Mat& frame, MInt32 faceRect[4])
{
	rectangle(frame, Rect(faceRect[0], faceRect[1], (faceRect[2] - faceRect[0]), (faceRect[3] - faceRect[1])), Scalar(0, 0, 255), 4);
}

void FaceDete::SetAPPID(const char appid[])
{
	APPID = new char[strlen(appid) + 1]();
	strcpy(APPID, appid);
}

void FaceDete::SetSDKKey(const char sdkkey[])
{
	SDKKey = new char[strlen(sdkkey) + 1]();
	strcpy(SDKKey, sdkkey);
}

void FaceDete::SetConfLevel(MFloat Level)
{
	this->threshold_confidenceLevel = Level;
}

MFloat FaceDete::GetConfLevel()
{
	return threshold_confidenceLevel;
}

void FaceDete::SetPreloadPath(string path)
{
	this->preloadPath = path;
}
