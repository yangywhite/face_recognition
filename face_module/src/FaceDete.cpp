#define _CRT_SECURE_NO_WARNINGS
#include "../inc/FaceDete.h"

FaceDete::FaceDete() :
	APPID(), SDKKey()
{

}

FaceDete::~FaceDete()
{
	if (APPID) {
		delete[]APPID;
	}
	if (SDKKey) {
		delete[]SDKKey;
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

int FaceDete::Loadregface()
{
	struct dirent *ptr;
	DIR *dir;
	dir = opendir(preloadPath.c_str());
	if (dir == nullptr) {
		return -1;
	}
	Mat img;
	string filename;
	while ((ptr = readdir(dir)) != NULL)
	{
		// Skip the "." and ".." hidden files
		if (ptr->d_name[0] == '.')
			continue;

		filename = preloadPath + "\\" + string(ptr->d_name);
		img = imread(filename);
		// Check whether it is a image file
		if (img.empty())
			continue;
		PreloadInfo preloadInfo;
		GetFeaturefromImage(img, preloadInfo.feature);
		preloadInfo.filename = filename;
		preLoadVec.push_back(preloadInfo);
	}
	closedir(dir);

#ifdef _DEBUG
	cout << "registration done!" << endl;
	for (int i = 0; i < preLoadVec.size(); ++i)
		cout << "[" << i << "]" << preLoadVec.at(i).filename << endl;
#endif // _DEBUG

	// 加载人员json信息
	std::ifstream file(preloadPath + "//" +string("stuTable.json"));
	if (!file.is_open()){
		return -1;
	}
	file >> stuTable;
	file.close();

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

int FaceDete::DetectFaces(Mat& frame, Json::Value &detectedResult)
{
	//--------------------------------------------
	//				检测(Detection)
	//--------------------------------------------
	Mat resizeImage;
	cv::resize(frame, resizeImage, Size(frame.cols - frame.cols % 4, frame.rows));

	ASF_MultiFaceInfo	multiFaceInfo = { 0 };
	ASF_SingleFaceInfo singleFaceInfo = { 0 };
	ASF_FaceFeature feature = { 0 };
	ASF_FaceFeature copyfeature = { 0 };

	res = ASFDetectFaces(handle, resizeImage.cols, resizeImage.rows,
		ASVL_PAF_RGB24_B8G8R8, resizeImage.data, &multiFaceInfo);

	if (MOK != res)
	{
#ifdef _DEBUG
		printf("ASFFaceFeatureExtract 1 fail: %d\n", res);
#endif // _DEBUG
		return -1;
	}

	vector<DetectedResult>detectedResultVec;

	// 分别识别每张人脸
	for (MInt32 i = 0; i < multiFaceInfo.faceNum; i++) {
		singleFaceInfo.faceRect.left = multiFaceInfo.faceRect[i].left;
		singleFaceInfo.faceRect.top = multiFaceInfo.faceRect[i].top;
		singleFaceInfo.faceRect.right = multiFaceInfo.faceRect[i].right;
		singleFaceInfo.faceRect.bottom = multiFaceInfo.faceRect[i].bottom;
		singleFaceInfo.faceOrient = multiFaceInfo.faceOrient[i];

		res = ASFFaceFeatureExtract(handle, resizeImage.cols, resizeImage.rows,
			ASVL_PAF_RGB24_B8G8R8, resizeImage.data, &singleFaceInfo, &feature);

		if (MOK != res)
		{
#ifdef _DEBUG
			printf("asffacefeatureextract 1 fail: %d\n", res);
#endif
			continue;
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
		}// end for of copy loop

		MInt32 processMask = ASF_AGE | ASF_GENDER | ASF_FACE3DANGLE | ASF_LIVENESS;
		res = ASFProcess(handle, resizeImage.cols, resizeImage.rows,
			ASVL_PAF_RGB24_B8G8R8, resizeImage.data, &multiFaceInfo, processMask);
		if (res != MOK) {
#ifdef _DEBUG
			printf("ASFProcess fail: %d\n", res);
#endif
			continue;
		}
		// 1.获取年龄
		ASF_AgeInfo ageInfo = { 0 };
		res = ASFGetAge(handle, &ageInfo);
		if (res != MOK) {
#ifdef _DEBUG
			printf("ASFGetAge fail: %d\n", res);
#endif
		}
		else {
			detectedResult.ageInfo.num = ageInfo.num;
			if (detectedResult.ageInfo.num != 0) {
				detectedResult.ageInfo.ageArray = new MInt32[ageInfo.num];
				for (int i = 0; i < ageInfo.num; i++) {
					detectedResult.ageInfo.ageArray[i] = ageInfo.ageArray[i];
				}
			}
		}// end if

		// 2.获取性别
		ASF_GenderInfo genderInfo = { 0 };
		res = ASFGetGender(handle, &genderInfo);
		if (res != MOK) {
#ifdef _DEBUG
			printf("ASFGetGender fail: %d\n", res);
#endif
		}
		else {
			detectedResult.genderInfo.num = genderInfo.num;
			if (detectedResult.genderInfo.num != -1) {
				detectedResult.genderInfo.genderArray = new MInt32[genderInfo.num];
				for (int i = 0; i < genderInfo.num; i++) {
					detectedResult.genderInfo.genderArray[i] = genderInfo.genderArray[i];
				}
			}
		}// end if

		// 3.获取活体信息
		ASF_LivenessInfo livenessInfo = { 0 };
		res = ASFGetLivenessScore(handle, &livenessInfo);
		if (res != MOK) {
#ifdef _DEBUG
			printf("ASFGetLivenessScore fail: %d\n", res);
#endif
		}
		else {
			detectedResult.livenessInfo.num = livenessInfo.num;
			if (detectedResult.livenessInfo.num != -1) {
				detectedResult.livenessInfo.isLive = new MInt32[livenessInfo.num];
				for (int i = 0; i < livenessInfo.num; i++)
				{
					detectedResult.livenessInfo.isLive[i] = livenessInfo.isLive[i];
				}
			}
		}// end if
		detectedResultVec.push_back(detectedResult);
	}// end 分别识别每张人脸

	// --------------------------------------------
	//				识别(Identification)
	// --------------------------------------------
	// 特征对比
	for (size_t i = 0;i != detectedResultVec.size(); ++i) {
		CompareFeature(detectedResultVec[i]);
#ifdef _DEBUG
		if (detectedResultVec[i].identifiable == true) {
			cout
				<< "MATCHED" << endl
				<< "Source:" 
						<<"[path]"<< detectedResultVec[i].pathInPreload <<" "
						<<"[index]"<< detectedResultVec[i] .indexInPreload<< endl
				<< "Confidence:" << detectedResultVec[i].confidenceLevel
				<< endl;
		}
#endif
	} // end 特征对比

	// --------------------------------------------
	//			处理结果(Result Process)
	// --------------------------------------------
	string strIndex;
	Json::Value tempStuTable;
	for (size_t i = 0; i != detectedResultVec.size(); ++i) {
		if (detectedResultVec[i].identifiable == true) {

			strIndex = std::to_string(detectedResultVec[i].indexInPreload);
			
			tempStuTable = Json::Value(stuTable[strIndex]);

			for (int j = 0; j < 4; j++)
				tempStuTable["rect"].append(detectedResultVec[i].faceRect[j]);

			cout << tempStuTable << endl;
			detectedResult.copy(tempStuTable);
		}
	}
	return -1;
}

int FaceDete::CompareFeature(DetectedResult& result)
{
	MFloat maxConfidence = 0.0f;

	// 循环识别,取得置信度最大的索引
	for (size_t i = 0; i != preLoadVec.size(); i++) {

		res = ASFFaceFeatureCompare(handle, &result.feature, &preLoadVec[i].feature, &result.confidenceLevel);

		if (res != MOK){
#ifdef _DEBUG
			printf("ASFFaceFeatureCompare fail: %d\n", res);
#endif
			return -1;
		}// end if 

		if (result.confidenceLevel > threshold_confidenceLevel) {
			result.identifiable = true;

			if (result.confidenceLevel > maxConfidence) {
				maxConfidence = result.confidenceLevel;
				result.pathInPreload = preLoadVec[i].filename;
				result.indexInPreload = (int)i;
			}

		}// end if
		
	}//end 循环对比

	return 0;
}

void FaceDete::DrawRetangle(Mat& frame, MInt32 faceRect[4])
{
	rectangle(frame, Rect(faceRect[0], faceRect[1], (faceRect[2] - faceRect[0]), (faceRect[3] - faceRect[1])), Scalar(0, 0, 255), 4);
}

void FaceDete::GetFeaturefromImage(Mat & image, ASF_FaceFeature &feature)
{
	Mat reSizeImage;
	cv::resize(image, reSizeImage, Size(image.cols - image.cols % 4, image.rows));
	ASF_MultiFaceInfo	multiFaceInfo = { 0 };
	ASF_SingleFaceInfo singleFaceInfo = { 0 };

	res = ASFDetectFaces(handle, reSizeImage.cols, reSizeImage.rows,
		ASVL_PAF_RGB24_B8G8R8, reSizeImage.data, &multiFaceInfo);

	if (MOK != res)
	{
		printf("ASFFaceFeatureExtract 1 fail: %d\n", res);
		// Do nothing with @feature
		return;
	}
	// 仅选取第一个所识别的结果
	singleFaceInfo.faceRect.left = multiFaceInfo.faceRect[0].left;
	singleFaceInfo.faceRect.top = multiFaceInfo.faceRect[0].top;
	singleFaceInfo.faceRect.right = multiFaceInfo.faceRect[0].right;
	singleFaceInfo.faceRect.bottom = multiFaceInfo.faceRect[0].bottom;
	singleFaceInfo.faceOrient = multiFaceInfo.faceOrient[0];

	ASF_FaceFeature local_feature;
	res = ASFFaceFeatureExtract(handle, reSizeImage.cols, reSizeImage.rows,
		ASVL_PAF_RGB24_B8G8R8, reSizeImage.data, &singleFaceInfo, &local_feature);

	feature.featureSize = local_feature.featureSize;
	feature.feature = new MByte[local_feature.featureSize];
	for (int i = 0; i < feature.featureSize; i++) {
		feature.feature[i] = local_feature.feature[i];
	}

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
