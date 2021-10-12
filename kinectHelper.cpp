#include "header.h"
#include "kinectHelper.h"

void getKinectImage(App& aux) {

    aux.Init();

    aux.Tick();

    aux.Shutdown();
}

void App::Init()
{
    //put initialization stuff here
    HRESULT hr;

    //get the kinect sensor
    hr = GetDefaultKinectSensor(&m_sensor);
    if (FAILED(hr)) {
        printf("Failed to find the kinect sensor!\n");
        exit(10);
    }
    m_sensor->Open();

    //get the depth frame source
    IDepthFrameSource* depthFrameSource;
    hr = m_sensor->get_DepthFrameSource(&depthFrameSource);
    if (FAILED(hr)) {
        printf("Failed to get the depth frame source.\n");
        exit(10);
    }

    //get depth frame description
    IFrameDescription* frameDesc;
    depthFrameSource->get_FrameDescription(&frameDesc);
    frameDesc->get_Width(&m_depthWidth);
    frameDesc->get_Height(&m_depthHeight);
    SafeRelease(frameDesc);

    //get the depth frame reader
    hr = depthFrameSource->OpenReader(&m_depthFrameReader);
    if (FAILED(hr)) {
        printf("Failed to open the depth frame reader!\n");
        exit(10);
    }
    //release depth frame source
    SafeRelease(depthFrameSource);

    //allocate depth buffer
    m_depthBuffer = new uint16[m_depthWidth * m_depthHeight];

    //get color frame source
    IColorFrameSource* colorFrameSource;
    hr = m_sensor->get_ColorFrameSource(&colorFrameSource);
    if (FAILED(hr)) {
        printf("Failed to get color frame source!\n");
        exit(10);
    }

    //get depth frame description
    IFrameDescription* frameDescC;
    colorFrameSource->get_FrameDescription(&frameDescC);
    frameDescC->get_Width(&m_colorWidth);
    frameDescC->get_Height(&m_colorHeight);
    SafeRelease(frameDescC);

    //get color frame reader
    hr = colorFrameSource->OpenReader(&m_colorFrameReader);
    if (FAILED(hr)) {
        printf("Failed to open color frame reader!\n");
        exit(10);
    }

    //release the color frame source
    SafeRelease(colorFrameSource);

    //allocate color buffer
    m_colorBuffer = new uint32[m_colorWidth * m_colorHeight];
}

void App::Tick()
{
    //put update and drawing stuff here
    HRESULT hr;

    //depth stuff
    IDepthFrame* depthFrame;
    hr = m_depthFrameReader->AcquireLatestFrame(&depthFrame);
    if (FAILED(hr)) 
        return;


    printf("Copying DEPTH data!\n");
    hr = depthFrame->CopyFrameDataToArray(
        m_depthWidth * m_depthHeight, m_depthBuffer);

    if (FAILED(hr)) {
        SafeRelease(depthFrame);
        printf("Something went wrong while copying DEPTH data!\n");
        return;
    }
    SafeRelease(depthFrame);

    //color stuff
    IColorFrame* colorFrame;
    hr = m_colorFrameReader->AcquireLatestFrame(&colorFrame);
    if (FAILED(hr))
        return;

    printf("Copying RGB data!\n");
    hr = colorFrame->CopyConvertedFrameDataToArray(
        m_colorWidth * m_colorHeight * 4, (BYTE*)m_colorBuffer, ColorImageFormat_Bgra);
    if (FAILED(hr)) {
        SafeRelease(colorFrame);
        printf("Something went wrong while copying RGB data!\n");
        return;
    }
    SafeRelease(colorFrame);

    // Show Depth Image
    depthHandlerKinect(m_depthBuffer,m_depthWidth,m_depthHeight);
    
    // Show RGB Image
    rgbHandlerKinect(m_colorBuffer,  m_colorWidth,  m_colorHeight);
}

void App::Shutdown() {
    //put cleaning up stuff here
    delete[] m_colorBuffer;
    SafeRelease(m_colorFrameReader);

    delete[] m_depthBuffer;
    SafeRelease(m_depthFrameReader);

    SafeRelease(m_sensor);
}

void App::depthHandlerKinect(uint16 depth[], int width, int height) {
    /*This function receives an array of integers wich contains the depth values
        and turn it on an image and then show/save as .png file*/

    int depthResolution = width * height;

    // directory management
    stringstream ss;
    ss << "Dataset/" << name << "/kinect/capture" << capture << "/depthValuesKinect.text";
    string filename = ss.str();
    ss.str("");

    // binary file to save values of depth image
    ofstream MyFile(filename, ios::out | ios::binary);
    MyFile.write((char*)&depth, sizeof(depth));
    MyFile.close();

    // display and save image
    Mat depthImageKinect = Mat(Size(width, height), CV_16UC1);
    memcpy(depthImageKinect.data, depth, depthResolution * sizeof(uint16_t));
    ss << "Dataset/" << name << "/kinect/capture" << capture << "/depthImageKinect.png";
    filename = ss.str();
    ss.str("");
    imwrite(filename, depthImageKinect);

    // normalization between 0 and 1
    Mat depthImageNormalized;
    depthImageKinect.convertTo(depthImageNormalized, CV_32F, 1.0 / 65535, 0);
    normalize(depthImageNormalized, depthImageNormalized, 0, 1, cv::NORM_MINMAX);

    // mirror the image
    cv::flip(depthImageNormalized, depthImageNormalized, 1);

    depthKinectImg = depthImageNormalized;

    // show image
    ss << "Depth Image - " << capture;
    namedWindow(ss.str(), WINDOW_NORMAL);
    imshow(ss.str(), depthImageNormalized);

    waitKey(0);
}

void App::rgbHandlerKinect(uint32 rgb[], int width, int height) {
    /*This function receives an array of integers wich contains the image
        and show/save the image as .png file*/

    // directory management
    stringstream ss;
    ss << "Dataset/" << name << "/kinect/capture" << capture << "/rgbValuesKinect.text";
    string filename = ss.str();
    ss.str("");
        
    // binary file to save values of rgb image
    ofstream MyFile(filename, ios::out | ios::binary);
    MyFile.write((char*)&rgb, sizeof(rgb));
    MyFile.close();

    // display and save image
    Mat rgbImageKinect = Mat(height, width, CV_8UC4, rgb).clone();
    cvtColor(rgbImageKinect, rgbImageKinect, COLOR_RGBA2BGR);
    ss << "Dataset/" << name << "/smartphone/capture" << capture << "/rgbImageKinect.png";
    filename = ss.str();
    ss.str("");
    imwrite(filename, rgbImageKinect);

    // mirror the image
    cv::flip(rgbImageKinect, rgbImageKinect, 1);

    rgbKinectImg = rgbImageKinect;

    // show image
    ss << "RGB Image - " << capture;
    namedWindow(ss.str(), WINDOW_NORMAL);
    imshow(ss.str(), rgbImageKinect);

    waitKey(0);
}