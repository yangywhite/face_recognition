# face_recognition
face_recognition, a popular tool in many applications, we used now is based on ArcFace created by [ArcSoft](https://www.arcsoft.com.cn/). 

# Getting Started
- OpenCV 3.0 +
- Arcface 2.1 +
- Visual Studio 2015 +
- jsoncpp 

# Configure VS in your project
Change your configuration into `X64`.

1. Add include path
    - `face_module\inc`
    - `arcface\v2.1\inc`
    - `opencv\build\include`
    - `third_party\jsoncpp\inc`
  
2. Add library path
    - `opencv\opencv\build\include`
    - `opencv\build\x64\vc15\lib`
    - `X64/*`
    - `third_party\jsoncpp\lib`
  
3. Add libraries(compiled by msvc2015)
    - `opencv_world346d.lib`
    - `libarcsoft_face_engine.lib`
    - `face_module.lib`
    - `jsoncpp.lib`

3. Test simple sample file
    - copy the `test.cpp` into your favorite IDE.

# Usage
1. Add target images
    - add images into `sample/`(only one person contains best)
    - change your image's *name* into *number+.jpg/png*(such as 1.jpg or 3.png)
2. Add target info
    - in `stuTable.json`, add info in json pattern:
    ``` json
        {
    // ↓ number.jpg/png  
        "3" : {
            "id": "12345678",
            "name":"alita",
            "major":"computer"
        },
        ...

        "4" : {
            "id": "87654321",
            "name":"hahahah",
            "major":"science"
        }
    }
    ```
3. Code
    ``` C++
        // custom info 
        cout << "[ID]" << currFace["id"] << endl;
        cout << "[Name]" << currFace["name"] << endl;
        cout << "[Major]" << currFace["major"] << endl;
        // essential info
        cout << "[confidence]" << currFace["confidence"] << endl;
        cout << "[pathInPreload]" << currFace["pathInPreload"] << endl;
        cout << "[age]" << currFace["age"] << endl;
        cout << "[gender]" << currFace["gender"] << endl;
        cout << "[liveinfo]" << currFace["liveinfo"] << endl;
    ```