#include "inc/FaceDete.h"

int main() {
	FaceDete facedete;

	facedete.SetPreloadPath("sample");
	facedete.SetConfLevel((MFloat)0.8);

	if (facedete.Loadregface() == 0)
		return -1;

	cv::VideoCapture cap(0);
	if (!cap.isOpened())
		return -1;

	cv::Mat frame;
	Json::Value detectedResult;
	MInt32 faceRect[4] = {0};
	Json::Value currFace;

	while (cap.isOpened())
	{
		cap >> frame;
		facedete.DetectFaces(frame, detectedResult);

		int totalFaceNum = detectedResult.size();
		for (int i = 0; i < totalFaceNum;i++) {
			currFace = detectedResult[std::to_string(i)];

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