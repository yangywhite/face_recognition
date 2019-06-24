# face_recognition
face_recognition, a popular tool in many applications, we used now is based on ArcFace created by [ArcSoft](https://www.arcsoft.com.cn/). 


# Getting Start
- OpenCV 3.0 +
- Arcface 2.1 +
- Visual Studio 2015 +

# Configure VS in your project
Change your configuration into `X64`.

1. Add include path
    - `face_module\face_module\inc`
    - `arcface\v2.1\inc`
    - `opencv\build\include`
2. Add Library path
    - `opencv\opencv\build\include`
    - `opencv\build\x64\vc15\lib`
    - `X64/*`
3. Add library files
    - `opencv_world346d.lib`
    - `libarcsoft_face_engine.lib`
    - `face_module.lib`
