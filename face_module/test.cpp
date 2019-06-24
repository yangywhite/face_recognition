#include "inc/FaceDete.h"

#define _DEBUG TRUE

int main() {
	FaceDete facedete;
	facedete.SetAPPID("a4e18xLPPvPkB76rXtYM5GVraNduE3Q7vUnGPFLfhSj");
	facedete.SetSDKKey("Fbu8Y5KNdMGpph8MrJc4GWceasdTeoGuCx3Qd4oRP6vs");
	facedete.GetVersion();
	facedete.Activation();
	facedete.InitEngine();

	facedete.SetPreloadPath("sample");
	facedete.SetConfLevel((MFloat)0.8);

	if (facedete.Loadregface() == -1) {
		cerr << "Error path" << endl;
		return 1;
	}

	cv::VideoCapture cap(0);
	if (!cap.isOpened())
		return -1;
	cv::Mat frame;
	Json::Value detectedResult;

	while (cap.isOpened())
	{
		cap >> frame;

		facedete.DetectFaces(frame, detectedResult);

		detectedResult.clear();

	}
	facedete.UninitEngine();
	facedete.GetVersion();
	return 0;
}