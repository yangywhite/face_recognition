#include "pch.h"
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include "arcsoft_face_sdk.h"
#include "amcomdef.h"
#include "asvloffscreen.h"
#include "merror.h"
#include <direct.h>
#include <iostream>  
#include <stdarg.h>
#include <string>
#include <opencv2\opencv.hpp>
#include "cxcore.h"
#include <vector>
#include <dirent.h>

using namespace cv;
using namespace std;
#pragma comment(lib, "libarcsoft_face_engine.lib")

char APPID[] = "a4e18xLPPvPkB76rXtYM5GVraNduE3Q7vUnGPFLfhSj";
char SDKKey[] = "Fbu8Y5KNdMGpph8MrJc4GWceasdTeoGuCx3Qd4oRP6vs";

#define SafeFree(p) { if ((p)) free(p); (p) = NULL; }
#define SafeArrayDelete(p) { if ((p)) delete [] (p); (p) = NULL; } 
#define SafeDelete(p) { if ((p)) delete (p); (p) = NULL; } 

//裁剪图片
void CutIplImage(IplImage* src, IplImage* dst, int x, int y)
{
	CvSize size = cvSize(dst->width, dst->height);//区域大小
	cvSetImageROI(src, cvRect(x, y, size.width, size.height));//设置源图像ROI
	cvCopy(src, dst); //复制图像
	cvResetImageROI(src);//源图像用完后，清空ROI
}


class ArcFaceEngine
{
public:
	ArcFaceEngine() {}
	~ArcFaceEngine() {}

	void Activation();//激活接口函数 
	void InitEngine();//初始化接口函数 
	void DetectFaces1(IplImage* img1, ASF_FaceFeature* copyfeature);//人脸数据库加载
	void UninitEngine();//反初始化函数
	const ASF_VERSION* GetVersion();//获取版本信息函数
	void DetectFaces2(IplImage* img2);//摄像头人脸位置及特征信息获取
	void FaceFeatureCompare(ASF_FaceFeature& copyfeature1, ASF_FaceFeature& copyfeature2);
	void Process(IplImage* image);//获取年龄等其他信息
	void Retangle(Mat& frame);
	void loadregface();


    ASF_FaceFeature copyfeature2 = { 0 };
	MFloat confidenceLevel;
	typedef std::vector<ASF_FaceFeature*> feature;
	feature features;
private:
	MRESULT res;
	MHandle handle;

	ASF_MultiFaceInfo	detectedFaces1 = { 0 };
	ASF_SingleFaceInfo SingleDetectedFaces1 = { 0 };
	ASF_FaceFeature feature1 = { 0 };
	//ASF_FaceFeature copyfeature2 = { 0 };
	int a = 0;
	int b = 0;
	int c = 0;
	int d = 0;

	ASF_MultiFaceInfo	detectedFaces2 = { 0 };
	ASF_SingleFaceInfo SingleDetectedFaces2 = { 0 };
	ASF_FaceFeature feature2 = { 0 };
};

//激活接口函数 
void ArcFaceEngine::Activation()
{
	res = ASFActivation(APPID, SDKKey);
	if (MOK != res && MERR_ASF_ALREADY_ACTIVATED != res)
		printf("ASFActivation fail: %d\n", res);
	else
		printf("ASFActivation sucess: %d\n", res);
}

//初始化接口函数 

void ArcFaceEngine::InitEngine()
{
	handle = NULL;
	MInt32 mask = ASF_FACE_DETECT | ASF_FACERECOGNITION | ASF_AGE | ASF_GENDER | ASF_FACE3DANGLE | ASF_LIVENESS;
	res = ASFInitEngine(ASF_DETECT_MODE_IMAGE, ASF_OP_0_ONLY, 16, 10, mask, &handle);
	if (res != MOK)
		printf("ASFInitEngine fail: %d\n", res);
	else
		printf("ASFInitEngine sucess: %d\n", res);
}

//人脸数据库加载
void ArcFaceEngine::DetectFaces1(IplImage* img1, ASF_FaceFeature* copyfeature)
{
	IplImage* cutImg1 = cvCreateImage(cvSize(img1->width - img1->width % 4, img1->height), IPL_DEPTH_8U, img1->nChannels);
	CutIplImage(img1, cutImg1, 0, 0);
	res = ASFDetectFaces(handle, cutImg1->width, cutImg1->height, ASVL_PAF_RGB24_B8G8R8, (MUInt8*)cutImg1->imageData, &detectedFaces1);

	if (MOK == res)
	{
		SingleDetectedFaces1.faceRect.left = detectedFaces1.faceRect[0].left;
		SingleDetectedFaces1.faceRect.top = detectedFaces1.faceRect[0].top;
		SingleDetectedFaces1.faceRect.right = detectedFaces1.faceRect[0].right;
		SingleDetectedFaces1.faceRect.bottom = detectedFaces1.faceRect[0].bottom;
		SingleDetectedFaces1.faceOrient = detectedFaces1.faceOrient[0];

		res = ASFFaceFeatureExtract(handle, cutImg1->width, cutImg1->height, ASVL_PAF_RGB24_B8G8R8, (MUInt8*)cutImg1->imageData, &SingleDetectedFaces1, &feature1);
		if (MOK != res)
			printf("ASFFaceFeatureExtract 1 fail: %d\n", res);//人脸特征提取失败
	}
	else
		printf("ASFDetectFaces 1 fail: %d\n", res);//人脸位置信息获取失败，图像中没有人脸
	
	copyfeature->featureSize = feature1.featureSize;
	copyfeature->feature = (MByte *)malloc(feature1.featureSize);
	memset(copyfeature->feature, 0, feature1.featureSize);
	memcpy(copyfeature->feature, feature1.feature, feature1.featureSize);
	

	features.push_back(copyfeature);

	cvReleaseImage(&cutImg1);
	
	if (img1) {
		cvReleaseImage(&img1);
	}

	//SafeFree(copyfeature->feature);		//释放内存
}

//反初始化
void ArcFaceEngine::UninitEngine()
{
	MRESULT res = ASFUninitEngine(handle);
	if (res != MOK)
		printf("ALUninitEngine fail: %d\n", res);
	else
		printf("ALUninitEngine sucess: %d\n", res);
}

//获取版本信息
const ASF_VERSION* ArcFaceEngine::GetVersion()
{
	const ASF_VERSION* pVersionInfo = ASFGetVersion(handle);
	return pVersionInfo;
}

/*
//当前人脸类
class CurrentFace
{
public:
	CurrentFace() {}
	~CurrentFace() {}

	void DetectFaces2(IplImage* img2);//摄像头人脸位置及特征信息获取
	void FaceFeatureCompare(ASF_FaceFeature& copyfeature1, ASF_FaceFeature& copyfeature2);
	void Process(IplImage* image);//获取年龄等其他信息
	void Retangle(Mat& frame);

	ASF_FaceFeature copyfeature2 = { 0 };
	MFloat confidenceLevel;
private:
	int a = 0;
	int b = 0;
	int c = 0;
	int d = 0;
	MRESULT res;
	MHandle handle;

	ASF_MultiFaceInfo	detectedFaces2 = { 0 };
	ASF_SingleFaceInfo SingleDetectedFaces2 = { 0 };
	ASF_FaceFeature feature2 = { 0 };
	
};
*/
//摄像头人脸位置及特征信息获取
void ArcFaceEngine::DetectFaces2(IplImage* img2)
{
	//ASF_FaceFeature copyfeature2 = { 0 };

	IplImage* cutImg2 = cvCreateImage(cvSize(img2->width - img2->width % 4, img2->height), IPL_DEPTH_8U, img2->nChannels);

	CutIplImage(img2, cutImg2, 0, 0);

	res = ASFDetectFaces(handle, cutImg2->width, cutImg2->height, ASVL_PAF_RGB24_B8G8R8, (MUInt8*)cutImg2->imageData, &detectedFaces2);
	
	if (MOK == res)
	{
		SingleDetectedFaces2.faceRect.left = detectedFaces2.faceRect[0].left;
		SingleDetectedFaces2.faceRect.top = detectedFaces2.faceRect[0].top;
		SingleDetectedFaces2.faceRect.right = detectedFaces2.faceRect[0].right;
		SingleDetectedFaces2.faceRect.bottom = detectedFaces2.faceRect[0].bottom;
		SingleDetectedFaces2.faceOrient = detectedFaces2.faceOrient[0];


		a = SingleDetectedFaces2.faceRect.left;
		b = SingleDetectedFaces2.faceRect.right;
		c = SingleDetectedFaces2.faceRect.top;
		d = SingleDetectedFaces2.faceRect.bottom;

		printf("%d %d %d %d\n", a, b, c, d);

		res = ASFFaceFeatureExtract(handle, cutImg2->width, cutImg2->height, ASVL_PAF_RGB24_B8G8R8, (MUInt8*)cutImg2->imageData, &SingleDetectedFaces2, &feature2);
		if (res == MOK)
		{
			//拷贝feature
			copyfeature2.featureSize = feature2.featureSize;
			copyfeature2.feature = (MByte *)malloc(feature2.featureSize);
			memset(copyfeature2.feature, 0, feature2.featureSize);
			memcpy(copyfeature2.feature, feature2.feature, feature2.featureSize);
		}
		else
			printf("ASFFaceFeatureExtract 2 fail: %d\n", res);
	}
	else
		printf("ASFDetectFaces 2 fail: %d\n", res);
}
//人脸对比
void ArcFaceEngine::FaceFeatureCompare(ASF_FaceFeature& copyfeature1, ASF_FaceFeature& copyfeature2)
{

	res = ASFFaceFeatureCompare(handle, &copyfeature1, &copyfeature2, &confidenceLevel);
	if (res != MOK)
		printf("ASFFaceFeatureCompare fail: %d\n", res);
	else
		printf("ASFFaceFeatureCompare sucess: %lf\n", confidenceLevel);
}

//框定人脸
void ArcFaceEngine::Retangle(Mat& frame)
{
	rectangle(frame, Rect(a, c, (b - a), (d - c)), Scalar(0, 0, 255), 4);

	if (confidenceLevel > 0.5)
	{
		CvFont font;
		cvInitFont(&font, CV_FONT_HERSHEY_PLAIN, 1.5f, 1.5f, 0, 2, CV_AA);//设置显示的字体
		IplImage img = IplImage(frame);
		cvPutText(&img, "confidenceLevel > 0.5,true", cvPoint(a, c - 10), &font, CV_RGB(255, 0, 0));//红色字体注释
	}
	//printf("%d\n", a);
	//frame.release();
}

//获取年龄等其他信息
void ArcFaceEngine::Process(IplImage* img1)
{
	IplImage* cutImg1 = cvCreateImage(cvSize(img1->width - img1->width % 4, img1->height), IPL_DEPTH_8U, img1->nChannels);
	CutIplImage(img1, cutImg1, 0, 0);
	MInt32 processMask = ASF_AGE | ASF_GENDER | ASF_FACE3DANGLE | ASF_LIVENESS;
	res = ASFProcess(handle, cutImg1->width, cutImg1->height, ASVL_PAF_RGB24_B8G8R8, (MUInt8*)cutImg1->imageData, &detectedFaces2, processMask);
	if (res != MOK)
		printf("ASFProcess fail: %d\n", res);
	else
		printf("ASFProcess sucess: %d\n", res);

	// 获取年龄
	ASF_AgeInfo ageInfo = { 0 };
	res = ASFGetAge(handle, &ageInfo);
	if (res != MOK)
		printf("ASFGetAge fail: %d\n", res);
	else
		printf("ASFGetAge sucess: %d\n", res);

	// 获取性别
	ASF_GenderInfo genderInfo = { 0 };
	res = ASFGetGender(handle, &genderInfo);
	if (res != MOK)
		printf("ASFGetGender fail: %d\n", res);
	else
		printf("ASFGetGender sucess: %d\n", res);

	// 获取3D角度
	ASF_Face3DAngle angleInfo = { 0 };
	res = ASFGetFace3DAngle(handle, &angleInfo);
	if (res != MOK)
		printf("ASFGetFace3DAngle fail: %d\n", res);
	else
		printf("ASFGetFace3DAngle sucess: %d\n", res);

	//获取活体信息
	ASF_LivenessInfo livenessInfo = { 0 };
	res = ASFGetLivenessScore(handle, &livenessInfo);
	if (res != MOK)
		printf("ASFGetLivenessScore fail: %d\n", res);
	else
		printf("ASFGetLivenessScore sucess: %d\n", livenessInfo.isLive[0]);

}

void ArcFaceEngine::loadregface()
{
	struct dirent *ptr;
	DIR *dir;
	string PATH = "C:\\Users\\25230\\Desktop\\face";
	dir = opendir(PATH.c_str());
	vector<string> files;
	namedWindow("edges", 1);
	//从文件中读入图像
	Mat img;

	char filename[100];
	while ((ptr = readdir(dir)) != NULL)
	{
		//跳过'.'和'..'两个目录
		if (ptr->d_name[0] == '.')
			continue;
		
		sprintf_s(filename, "C:\\Users\\25230\\Desktop\\face\\%s", ptr->d_name);
		
		IplImage* img0 = cvLoadImage(filename, 1);
		ASF_FaceFeature* copyfeature = new ASF_FaceFeature;
		DetectFaces1(img0, copyfeature);

		files.push_back(ptr->d_name);
	}
	cout << "已注册列表：\n";
	for (int i = 0; i < files.size(); ++i)
	{
		cout << files[i] << endl;
	}
	closedir(dir);
	/*
	char filename[100];
	for (int i = 0; i < 4; i++)
	{
		printf("%d\n", i);
		sprintf_s(filename, "C:\\Users\\25230\\Desktop\\face\\%d.jpg", i);

		IplImage* img0 = cvLoadImage(filename, 1);
		ASF_FaceFeature* copyfeature = new ASF_FaceFeature;
		DetectFaces1(img0, copyfeature);
		//delete copyfeature;
	}
	*/
}
int main(int, char**)
{
	ArcFaceEngine p;
	p.Activation();
	p.InitEngine();
	p.loadregface();

	int i;
	cin >> i;
	if (i == 1)
	{
		cv::VideoCapture cap(0); //打开默认的摄像头号
		if (!cap.isOpened())  //检测是否打开成功
			return -1;
		//namedWindow("edges", 1);
		cv::Mat frame;

		while (1)
		{
			cap >> frame; // 从摄像头中获取新的一帧
			IplImage image(frame);
			IplImage* img = &image;
			
			p.DetectFaces2(img);//加载数据库
			for (int y = 0; y < 4; y++)
			{
				p.FaceFeatureCompare(*p.features[y], p.copyfeature2);
			}
			p.Retangle(frame);
			
			imshow("edges",frame);//显示
			if (waitKey(30) >= 0)
				break;
			frame.release();
		}
		p.GetVersion();
		p.UninitEngine();
	}
	return 0;
	//摄像头会在VideoCapture的析构函数中释放
}
