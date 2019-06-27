# face_recognition
face_recognition, a popular tool in many applications, we used now is based on ArcFace created by [ArcSoft](https://www.arcsoft.com.cn/). 

# Getting Started
- OpenCV 3.0 +
- Arcface 2.1 +
- Visual Studio 2015 +
- [Jsoncpp](https://github.com/open-source-parsers/jsoncpp) 

# Configure VS in your project
Change your configuration into `X64`.

1. Add include path
    - `face_module\inc`
    - `arcface\v2.1\inc`
    - `opencv\build\include`
    - `third_party\jsoncpp\inc`
  
2. Add library path
    - `X64\`
    - `arcface\v2.1\lib\X64`
    - `opencv\build\x64\vc15\lib`
    - `third_party\jsoncpp\lib`
  
3. Add libraries **(MSVC 2015 64bit)**
    - `face_module.lib`
    - `libarcsoft_face_engine.lib`
    - `opencv_world346d.lib`
    - `jsoncpp.lib`

4. Test sample file
    - copy the `test.cpp` into your favorite IDE.

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
