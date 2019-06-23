#include "inc/FaceDete.h"

int main() {
	FaceDete facedete;
	facedete.SetAPPID("a4e18xLPPvPkB76rXtYM5GVraNduE3Q7vUnGPFLfhSj");
	facedete.SetSDKKey("Fbu8Y5KNdMGpph8MrJc4GWceasdTeoGuCx3Qd4oRP6vs");
	facedete.SetPreloadPath("sample");
	facedete.SetConfLevel((MFloat)0.8);

	facedete.GetVersion();
	facedete.Activation();
	facedete.InitEngine();
	if (facedete.Loadregface() == -1) {
		cerr << "Error path" << endl;
		return 1;
	}

	cv::VideoCapture cap(0);
	if (!cap.isOpened())
		return -1;
	cv::Mat frame;

	vector<DetectedResult> detectedResultVec;
	while (cap.isOpened())
	{

		cap >> frame;
		facedete.DetectFaces(frame, detectedResultVec, false);

		// 循环比对识别结果
		size_t index = 0;
		for (vector<DetectedResult>::iterator it = detectedResultVec.begin();
			it != detectedResultVec.end(); ++it) {
			// 别对特征
			index = facedete.CompareFeature(it->feature, it->confidenceLevel);
			cout << "The number of the person currently authenticated is " << index << endl;
			// 在当前帧上画出来
			//if (index != 0)
			facedete.DrawRetangle(frame, it->faceRect);
		}

		imshow("result", frame);
		if (waitKey(30) >= 0)
			break;
		detectedResultVec.clear();
	}
	facedete.UninitEngine();
	facedete.GetVersion();
	return 0;
}