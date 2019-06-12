#pragma once
#include <iostream>
#include <opencv2\opencv.hpp>

#include "arcsoft_face_sdk.h"
#include "amcomdef.h"
#include "asvloffscreen.h"
#include "merror.h"
#include "dirent.h"

using namespace cv;
using namespace std;

class DetectedResult {
public:
	DetectedResult() {
		feature.feature = new MByte();
		ageInfo.ageArray = new MInt32();
		genderInfo.genderArray = new MInt32();
		livenessInfo.isLive = new MInt32();
		// ... 
	}
	~DetectedResult() {
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

	// 以下类型要注意是否包含指针

	ASF_FaceFeature feature;
	ASF_AgeInfo ageInfo;
	ASF_GenderInfo genderInfo;
	ASF_LivenessInfo livenessInfo;
	// ...

public:
	// 当前人脸的置信度
	MFloat confidenceLevel;

	// 当前人脸的位置
	// faceRect[0],faceRect[1]: (x1,y1)
	// faceRect[2],faceRect[3]: (x2,y2)
	// (x1,y1)───────────┐
	//    │              │
	//    │              │
	//    │              │
	//    └───────────(x2,y2) 
	MInt32 faceRect[4];
};

class FaceDete
{
public:
	FaceDete();

	~FaceDete();

	void SetAPPID(const string appid);

	void SetSDKKey(const string sdkkey);

	void SetPreloadPath(string path);

	void Activation();

	void InitEngine(); 

	void UninitEngine();

	/*
	* 获取版本信息函数
	*/
	const ASF_VERSION* GetVersion();

	/*
	* @ 参数
	*	所指定的预加载文件路径
	* @ 返回值
	*	>0 加载成功的数量
	*	-1 加载失败，有可能是路径错误
	*/
	size_t Loadregface();

	/*
	* @ 参数
	*	image [可能]包含人脸的图像
	*	detectedResult 识别出的结果
	*	opt 是否将识别结果保存在preload数据集中
	*		true 将识别结果保存在preload数据集中
	*		false 不将识别结果保存在preload数据集中
	* @ 无返回
	*/
	void DetectFaces(Mat image, vector<DetectedResult>& detectedResultVec,bool opt=false);

	/*
	* @参数
	*	f 是待匹配特征,它将和preLoadFeatureVec进行比较
	* @返回值
	*	返回 f 与preLoadFeatureVec匹配最高的索引
	*/
	size_t CompareFeature(const ASF_FaceFeature& f);


	// 以下的函数暂用于debug/test测试
	void DrawRetangle(Mat& frame, MInt32 faceRect[4]);

private:
	MRESULT res;
	MHandle handle;
	string preloadPath;
	vector <ASF_FaceFeature> preLoadFeatureVec;

	char * APPID;
	char * SDKKey;
};

