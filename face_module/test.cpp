#include "inc/FaceDete.h"

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
	MInt32 faceRect[4] = {0};

	while (cap.isOpened())
	{
		cap >> frame;
		facedete.DetectFaces(frame, detectedResult);

		// 获取当前帧有多少张识别出来的人脸
		int totalFaceNum = detectedResult.size();
		for (int i = 0; i < totalFaceNum;i++) {
			Json::Value currFace = detectedResult[std::to_string(i)];

			for (int j = 0; j < 4; j++) {
				faceRect[j] = currFace["rect"][j].asInt();
			} 
			facedete.DrawRetangle(frame, faceRect);
			cout << "NO." << i << endl;
			cout << "[currFace]" << currFace["rect"] << endl;
			cout << "[ID]" << currFace["id"] << endl;
			cout << "[Name]" << currFace["name"] << endl;
			cout << "[Major]" << currFace["major"] << endl;
			cout << "[confidence]" << currFace["confidence"] << endl;
			cout << "[pathInPreload]" << currFace["pathInPreload"] << endl;
			cout << "[age]" << currFace["age"] << endl;
			cout << "[gender]" << currFace["gender"] << endl;
			cout << "[liveinfo]" << currFace["liveinfo"] << endl;
		}

		imshow("show", frame);
		if (waitKey(33) == 27) break;

		detectedResult.clear();
	}
	facedete.UninitEngine();
	facedete.GetVersion();
	return 0;
}