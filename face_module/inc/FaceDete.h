#pragma once
#include "stdafx.h"

class FaceDete
{
public:
	FaceDete();

	~FaceDete();

	void SetAPPID(const char appid[]);

	void SetSDKKey(const char sdkkey[]);

	void SetPreloadPath(string path);

	void SetConfLevel(MFloat Level);

	void Activation();

	void InitEngine(); 

	void UninitEngine();

	const ASF_VERSION* GetVersion();

	/*
	* @ 参数
	*	所指定的预加载文件路径
	* @ 返回值
	*	>0 加载成功的数量
	*	-1 加载失败，有可能是路径错误
	*/
	int Loadregface();

	/*
	* @ 参数
	*	image [可能]包含人脸的图像
	*	detectedResult 
	* @ 无返回
	*/
	int DetectFaces(Mat &image, Json::Value &detectedResult);

	// 以下的函数暂用于debug/test测试
	void DrawRetangle(Mat& frame, MInt32 faceRect[4]);

private:

	void GetFeaturefromImage(Mat & image, ASF_FaceFeature &feature);

	int CompareFeature(DetectedResult& result);

private:
	MRESULT res;
	MHandle handle;
	string preloadPath;
	MFloat threshold_confidenceLevel;
	/*
	*	struct PreloadInfo {
	*		eature feature;
	*  		std::string filename;
	*	};
	*/
	vector <PreloadInfo> preLoadVec;

	Json::Value stuTable;

	char * APPID;
	char * SDKKey;
};

