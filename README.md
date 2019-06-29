# face_recognition
face_recognition, a popular tool in many applications, we used now is based on ArcFace created by [ArcSoft](https://www.arcsoft.com.cn/). 

# Getting Started
- OpenCV 3.0 +
- Arcface 2.1 +
- Visual Studio 2015
- [Jsoncpp](https://github.com/open-source-parsers/jsoncpp) 

# Compiling from source
- Add `OPENCV` and `ARCFACE` into environment, which will be used in `.vcxproj`:
  - `OPENCV : ...\opencv\opencv\build`
  - `ARCFACE: ...\arcface\v2.1`
- Open `face_module.sln`

By the way, maybe you need to change the name of lib of OpenCV because the version we included is different(OpenCV346 on my platform). To avoid this problem, I suggest you install the same version of OpenCV.

# Usage
1. Add target images
    - add images into `sample/`(only one person contains is best)
    - change your *image's name* into *number.jpg/png*(such as 3.jpg or 4.png)
2. Add target info
    -  add info in json pattern in `stuTable.json`(Do not change this name):
    ``` json
        {
        "3" : {
            "id": "123456",
            "name":"Alita",
            "major":"Computer"
        },
        "4" : {
            "id": "789101112",
            "name":"XiaoMing",
            "major":"Not Decide"
        }
    }
    ```
3. Code
``` C++
    cv::VideoCapture cap(0);
    if (!cap.isOpened())
        return -1;
    cv::Mat frame;

    Json::Value detectedResult;
    // Store the position of face
    MInt32 faceRect[4] = {0};
    Json::Value currFace;

    while (cap.isOpened())
    {
        cap >> frame;
        facedete.DetectFaces(frame, detectedResult);

        int totalFaceNum = detectedResult.size();
        for (int i = 0; i < totalFaceNum;i++) {
            currFace = detectedResult[std::to_string(i)];

            for (int j = 0; j < 4; j++)
                faceRect[j] = currFace["rect"][j].asInt();
            facedete.DrawRetangle(frame, faceRect);

            cout << "NO." << i << endl;
            cout << "[currFace]" << currFace["rect"] << endl;
            // Custome data
            cout << "[ID]" << currFace["id"] << endl;
            cout << "[Name]" << currFace["name"] << endl;
            cout << "[Major]" << currFace["major"] << endl;
            // Essential data
            cout << "[confidence]" << currFace["confidence"] << endl;
            cout << "[pathInPreload]" << currFace["pathInPreload"] << endl;
            cout << "[age]" << currFace["age"] << endl;
            cout << "[gender]" << currFace["gender"] << endl;
            cout << "[liveinfo]" << currFace["liveinfo"] << endl;

        }// end for inside

        imshow("show", frame);
        if (waitKey(33) == 27) break;

        detectedResult.clear();
    }// end while for frame capture 
```
