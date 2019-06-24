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

	MFloat GetConfLevel();

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
	int Loadregface();

	/*
	* @ 参数
	*	image [可能]包含人脸的图像
	*	detectedResult 识别出的结果
	*	opt 是否将识别结果保存在preload数据集中
	*		true 将识别结果保存在preload数据集中
	*		false 不将识别结果保存在preload数据集中
	* @ 无返回
	*/
	void DetectFaces(Mat &image, vector<DetectedResult>& detectedResultVec,bool opt=false);

	/*
	* @参数
	*	result 是通过DetectFaces检测后的识别结果
	* @返回值
	*	返回preLoadFeatureVec匹配满足阈值的索引
	*   -1 无匹配的，但是可能有两种情况
	*		1) 确实没有匹配的
	*		2) 匹配结果小于阈值
	*	>=0 匹配成功的索引 
	*		
	*/
	int CompareFeature(DetectedResult& result);

	// 以下的函数暂用于debug/test测试
	void DrawRetangle(Mat& frame, MInt32 faceRect[4]);

private:
	MRESULT res;
	MHandle handle;
	string preloadPath;
	MFloat threshold_confidenceLevel;
	vector <ASF_FaceFeature> preLoadFeatureVec;

	char * APPID;
	char * SDKKey;
};

