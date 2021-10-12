#pragma once

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <memory>
#include <Kinect.h>
#include <chrono>
using namespace std::chrono;

// openCV
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace cv;

//some useful typedefs for explicit type sizes
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;
typedef char int8;
typedef short int16;
typedef int int32;
typedef long long int64;

//safe way of deleting a COM object
template<typename T>
void SafeRelease(T& ptr) { if (ptr) { ptr->Release(); ptr = nullptr; } }

class App
{
public:
    std::string name;
    int capture;

    void Init();
    void Tick();
    void Shutdown();

    cv::Mat depthKinectImg;
    cv::Mat rgbKinectImg;

    void depthHandlerKinect(uint16 depth[], int width, int height);
    void rgbHandlerKinect(uint32 rgb[], int width, int height);

    App() {
        this->name = "User";
    }
    App(std::string name) {
        this->name = name;
    }

    std::string getName() { return name; }

private:
    IKinectSensor* m_sensor = nullptr;
    IDepthFrameReader* m_depthFrameReader = nullptr;
    IColorFrameReader* m_colorFrameReader = nullptr;

    uint32* m_colorBuffer = nullptr;
    int m_colorWidth = 0, m_colorHeight = 0;

    uint16* m_depthBuffer = nullptr;
    int m_depthWidth = 0, m_depthHeight = 0;
};

void getKinectImage(App& aux);