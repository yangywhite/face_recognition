#pragma once
#include "stdafx.h"
#include "DetectedResult.h"

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

