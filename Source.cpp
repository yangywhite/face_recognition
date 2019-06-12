#include "FaceDete.h"

int main() {
	FaceDete facedete;
	facedete.SetAPPID("a4e18xLPPvPkB76rXtYM5GVraNduE3Q7vUnGPFLfhSj");
	facedete.SetSDKKey("a4e18xLPPvPkB76rXtYM5GVraNduE3Q7vUnGPFLfhSj");
	facedete.SetPreloadPath(".//preload//");

	facedete.GetVersion();
	facedete.Activation();
	facedete.InitEngine();
	facedete.Loadregface();

	cv::VideoCapture cap(0);
	if (!cap.isOpened()) 
		return -1;
	cv::Mat frame;
	vector<DetectedResult> detectedResultVec;

	while (cap.isOpened())
	{
		cap >> frame;
		facedete.DetectFaces(frame, detectedResultVec);
		
		// 循环比对识别结果
		size_t index;
		for (vector<DetectedResult>::iterator it = detectedResultVec.begin();
			it != detectedResultVec.end(); ++it) {
			// 别对特征
			index = facedete.CompareFeature(it->feature);
			cout << "此人的在数据集中的编号是" << index << endl;
			// 在当前帧上画出来
			facedete.DrawRetangle(frame,it->faceRect);
		}

		imshow("result", frame);
		if (waitKey(30) >= 0)
			break;
	}
	facedete.UninitEngine();
	return 0;
}