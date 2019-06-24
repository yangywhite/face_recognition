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

		// 从图像中检测人脸
		facedete.DetectFaces(frame, detectedResultVec, false);

		// 循环比对识别结果
		int index = 0;
		for (vector<DetectedResult>::iterator it = detectedResultVec.begin();
			it != detectedResultVec.end(); ++it) {
			// 特征对比
			index = facedete.CompareFeature(*it);

			facedete.DrawRetangle(frame, it->faceRect);

			if (index == -1)
				cout << "NO MATCHED" << endl;
			else
				cout << "MATCHED: NO." << index << " in preload lib," <<"confidence:"<<it->confidenceLevel<< endl;
		} // end for

		imshow("result", frame);
		if (waitKey(30) >= 0)
			break;

		// 清空识别结果
		detectedResultVec.clear();
	}
	facedete.UninitEngine();
	facedete.GetVersion();
	return 0;
}