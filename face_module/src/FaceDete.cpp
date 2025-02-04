#include "../inc/FaceDete.h"

FaceDete::FaceDete() :
	APPID(), SDKKey()
{
	SetAPPID("a4e18xLPPvPkB76rXtYM5GVraNduE3Q7vUnGPFLfhSj");
	SetSDKKey("Fbu8Y5KNdMGpph8MrJc4GWceasdTeoGuCx3Qd4oRP6vs");

	if (Activation()) {
		cerr <<"Activation() failed."<< endl;
	}
	if (InitEngine()) {
		cerr << "InitEngine() failed." << endl;
	}
	peopleInfo.clear();
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

int FaceDete::Activation()
{
	res = ASFActivation(APPID, SDKKey);

	if (MOK != res && MERR_ASF_ALREADY_ACTIVATED != res) {
#ifdef _DEBUG		
		cerr << "ASFActivation fail:" << res << endl;
#endif // _DEBUG
		return 1;
	}
	else{
#ifdef _DEBUG
		cout << "ASFActivation sucess:" << res << endl;
#endif
		return 0;
	}
}

int FaceDete::InitEngine()
{
	handle = NULL;
	MInt32 mask = ASF_FACE_DETECT | ASF_FACERECOGNITION | ASF_AGE | ASF_GENDER | ASF_FACE3DANGLE | ASF_LIVENESS;
	res = ASFInitEngine(ASF_DETECT_MODE_IMAGE, ASF_OP_0_ONLY, 16, 10, mask, &handle);
	
	if (res != MOK) {
#ifdef _DEBUG
		cerr << "ASFInitEngine fail:" << res << endl;
#endif
		return 1;
	}
	else {
#ifdef _DEBUG
		cout << "ASFInitEngine sucess:" << res << endl;
#endif
		return 0;
	}
}

int FaceDete::UninitEngine()
{
	MRESULT res = ASFUninitEngine(handle);

	if (res != MOK) {
#ifdef _DEBUG
		cerr << "ALUninitEngine fail:" << res << endl;
#endif
		return 1;
	}
	else {
#ifdef _DEBUG
		cout << "ALUninitEngine sucess:" << res << endl;
#endif
		return 0;
	}
}

const ASF_VERSION* FaceDete::GetVersion()
{
	return ASFGetVersion(handle);
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
	cout << "Registration done!" << endl;
	for (int i = 0; i < preLoadVec.size(); ++i)
		cout << "[" << i << "]" << preLoadVec.at(i).filename << endl;
#endif // _DEBUG

	string jsonPath = preloadPath + "\\" + string("peopleInfo.json");
#ifdef _DEBUG
	cout <<"[JSON]"<< jsonPath << endl;
#endif // _DEBUG
	std::ifstream file(jsonPath);
	if (!file.is_open()){
		return -1;
	}
	Json::CharReaderBuilder reader;
	JSONCPP_STRING errs;
	if (!Json::parseFromStream(reader, file, &peopleInfo, &errs)) {
#ifdef _DEBUG
		cout << "[READER JSON ERROR]" << errs << endl;
#endif // _DEBUG
		return 0;
	}
	file.close();

	return (int)preLoadVec.size();
}

int FaceDete::DetectFaces(Mat& frame, Json::Value &detectedResult)
{
	detectedResult.clear();
	//--------------------------------------------
	//				检测(Detection)
	//--------------------------------------------
	cv::resize(frame, frame, Size(frame.cols - frame.cols % 4, frame.rows));

	ASF_MultiFaceInfo	multiFaceInfo = { 0 };
	ASF_SingleFaceInfo singleFaceInfo = { 0 };
	ASF_FaceFeature localFeature = { 0 };
	ASF_FaceFeature copyFeature = { 0 };

	res = ASFDetectFaces(handle, frame.cols, frame.rows,
		ASVL_PAF_RGB24_B8G8R8, frame.data, &multiFaceInfo);

	if (MOK != res)
	{
#ifdef _DEBUG
		cerr << "ASFFaceFeatureExtract 1 fail:" << res << endl;
#endif // _DEBUG
		return -1;
	}

	vector<DetectedResult>detectedResultVec;

#ifdef _DEBUG
	cout << "multiFaceInfo.faceNum:" << multiFaceInfo.faceNum << endl;
#endif // _DEBUG

	// 分别识别每张人脸
	for (MInt32 i = 0; i < multiFaceInfo.faceNum; i++) {
		singleFaceInfo.faceRect.left = multiFaceInfo.faceRect[i].left;
		singleFaceInfo.faceRect.top = multiFaceInfo.faceRect[i].top;
		singleFaceInfo.faceRect.right = multiFaceInfo.faceRect[i].right;
		singleFaceInfo.faceRect.bottom = multiFaceInfo.faceRect[i].bottom;
		singleFaceInfo.faceOrient = multiFaceInfo.faceOrient[i];

		res = ASFFaceFeatureExtract(handle, frame.cols, frame.rows,
			ASVL_PAF_RGB24_B8G8R8, frame.data, &singleFaceInfo, &localFeature);

		if (MOK != res)
		{
#ifdef _DEBUG
			cerr << "asffacefeatureextract 1 fail:" << res << endl;
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
		detectedResult.feature.featureSize = localFeature.featureSize;
		detectedResult.feature.feature = new MByte[localFeature.featureSize]{0};
		memcpy(detectedResult.feature.feature, localFeature.feature, localFeature.featureSize);

		MInt32 processMask = ASF_AGE | ASF_GENDER | ASF_FACE3DANGLE | ASF_LIVENESS;
		res = ASFProcess(handle, frame.cols, frame.rows,
			ASVL_PAF_RGB24_B8G8R8, frame.data, &multiFaceInfo, processMask);
		if (res != MOK) {
#ifdef _DEBUG
			cerr << "ASFProcess fail:" << res << endl;
#endif
			continue;
		}
		// 1.获取年龄
		ASF_AgeInfo localAgeInfo = { 0 };
		res = ASFGetAge(handle, &localAgeInfo);
		if (res != MOK) {
#ifdef _DEBUG
			cerr << "ASFGetAge fail:" << res << endl;
#endif
		}
		else {
			detectedResult.ageInfo.num = localAgeInfo.num;
			if (detectedResult.ageInfo.num != 0) {
				detectedResult.ageInfo.ageArray = new MInt32[localAgeInfo.num]{0};
				memcpy(detectedResult.ageInfo.ageArray, localAgeInfo.ageArray, detectedResult.ageInfo.num);
			}// end != 0
		}// end if

		// 2.获取性别
		ASF_GenderInfo localGenderInfo = { 0 };
		res = ASFGetGender(handle, &localGenderInfo);
		if (res != MOK) {
#ifdef _DEBUG
			cerr << "ASFGetGender fail:" << res << endl;
#endif
		}
		else {
			detectedResult.genderInfo.num = localGenderInfo.num;
			if (detectedResult.genderInfo.num != -1) {
				detectedResult.genderInfo.genderArray = new MInt32[localGenderInfo.num]{0};
				memcpy(detectedResult.genderInfo.genderArray, localGenderInfo.genderArray, localGenderInfo.num);
			}
		}// end if

		// 3.获取活体信息
		ASF_LivenessInfo localLivenessInfo = { 0 };
		res = ASFGetLivenessScore(handle, &localLivenessInfo);
		if (res != MOK) {
#ifdef _DEBUG
			cerr << "ASFGetLivenessScore fail:" << res << endl;
#endif
		}
		else {
			detectedResult.livenessInfo.num = localLivenessInfo.num;
			if (detectedResult.livenessInfo.num != -1) {
				detectedResult.livenessInfo.isLive = new MInt32[localLivenessInfo.num]{0};
				memcpy(detectedResult.livenessInfo.isLive, localLivenessInfo.isLive, detectedResult.livenessInfo.num);
			}
		}// end if
		detectedResultVec.push_back(detectedResult);

#ifdef _DEBUG
		cout
			<< "NO" << detectedResultVec.size()
			<< "Age:" << *detectedResult.ageInfo.ageArray
			<< endl;
#endif

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
	Json::Value tmpPeopleInto;
	int personIndex = 0;
	for (size_t i = 0; i != detectedResultVec.size(); ++i) {

		if (detectedResultVec[i].identifiable == true) {

			// 利用识别结果中的对perload的索引
			strIndex = std::to_string(detectedResultVec[i].indexInPreload);
			
			// 赋值临时变量，避免修改原本的值
			tmpPeopleInto = Json::Value(peopleInfo[strIndex]);

			// 添加人脸位置
			for (int j = 0; j < 4; j++)
				tmpPeopleInto["rect"].append(detectedResultVec[i].faceRect[j]);

			// 添加置信度
			tmpPeopleInto["confidence"] = detectedResultVec[i].confidenceLevel;
			
			// 添加加载库的路径
			tmpPeopleInto["pathInPreload"] = detectedResultVec[i].pathInPreload;

			// 添加年龄
			tmpPeopleInto["age"] = detectedResultVec[i].ageInfo.ageArray[0];
			
			// 添加性别
			tmpPeopleInto["gender"] = detectedResultVec[i].genderInfo.genderArray[0];

			// 添加活体信息
			tmpPeopleInto["liveinfo"] = detectedResultVec[i].livenessInfo.isLive[0];

			detectedResult[std::to_string(personIndex)] = tmpPeopleInto;

			personIndex++;
		}
	}

#ifdef _DEBUG
	cout << "[DetectedResult]" << detectedResult << endl;
#endif

	return 0;
}

int FaceDete::CompareFeature(DetectedResult& result)
{
	MFloat maxConfidence = 0.0f;

	// 循环识别,取得置信度最大的索引
	for (size_t i = 0; i != preLoadVec.size(); i++) {

		res = ASFFaceFeatureCompare(handle, &result.feature, &preLoadVec[i].feature, &result.confidenceLevel);

		if (res != MOK){
#ifdef _DEBUG
			cerr << "ASFFaceFeatureCompare fail:" << res << endl;
#endif
			return -1;
		}// end if 

		// 防止置信度计算后降低
		if (result.confidenceLevel < maxConfidence) {
			result.confidenceLevel = maxConfidence;
		}

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
	rectangle(frame, Rect(faceRect[0], faceRect[1], (faceRect[2] - faceRect[0]), (faceRect[3] - faceRect[1])), Scalar(0, 255, 255), 2);
}

void FaceDete::GetFeaturefromImage(Mat & image, ASF_FaceFeature &feature)
{
	cv::resize(image, image, Size(image.cols - image.cols % 4, image.rows));
	ASF_MultiFaceInfo	multiFaceInfo = { 0 };
	ASF_SingleFaceInfo singleFaceInfo = { 0 };

	res = ASFDetectFaces(handle, image.cols, image.rows,
		ASVL_PAF_RGB24_B8G8R8, image.data, &multiFaceInfo);

	if (MOK != res)
	{
		cerr << "ASFFaceFeatureExtract 1 fail:" << res << endl;
		// Do nothing with @feature
		return;
	}
	// 仅选取第一个所识别的结果
	singleFaceInfo.faceRect.left = multiFaceInfo.faceRect[0].left;
	singleFaceInfo.faceRect.top = multiFaceInfo.faceRect[0].top;
	singleFaceInfo.faceRect.right = multiFaceInfo.faceRect[0].right;
	singleFaceInfo.faceRect.bottom = multiFaceInfo.faceRect[0].bottom;
	singleFaceInfo.faceOrient = multiFaceInfo.faceOrient[0];

	ASF_FaceFeature localfeature;
	res = ASFFaceFeatureExtract(handle, image.cols, image.rows,
		ASVL_PAF_RGB24_B8G8R8, image.data, &singleFaceInfo, &localfeature);

	feature.featureSize = localfeature.featureSize;
	feature.feature = new MByte[localfeature.featureSize]{0};
	memcpy(feature.feature, localfeature.feature, localfeature.featureSize);
}

void FaceDete::SetAPPID(const char appid[])
{
	APPID = new char[strlen(appid) + 1]();
	strcpy_s(APPID, strlen(appid) + 1, appid);
}

void FaceDete::SetSDKKey(const char sdkkey[])
{
	SDKKey = new char[strlen(sdkkey) + 1]();
	strcpy_s(SDKKey, strlen(sdkkey) + 1, sdkkey);
}

void FaceDete::SetConfLevel(MFloat Level)
{
	this->threshold_confidenceLevel = Level;
}

size_t FaceDete::GetRestrSize()
{
	return preLoadVec.size();
}

void FaceDete::SetPreloadPath(string path)
{
	this->preloadPath = path;
}
