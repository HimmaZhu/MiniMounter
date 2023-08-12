#include "opencvmex.hpp"
#include "testCommand1.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <string>
#include <stdint.h>
#include <windows.h>
#include <map>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <opencv2/core/utils/logger.hpp>

using namespace cv;
using namespace std;

static bool camSetProperty(cv::VideoCapture* cam, int propId, double value)
{
    const auto ret = cam->set(propId, value);
    // Linux apparently is faster at USB communication than Windows, and since our DAQ
    // board is slow at clearing data from its control endpoint, not waiting a bit before
    // sending the next command will result in the old command being overridden (which breaks
    // our packet layout)
    // Waiting >100µs seems to generally work. We call the wait function on all platforms,
    // just in case some computers on Windows also manage to communicate with similar speeds then
    // Windows, but keep in mind that Windows may not be able to wait with microsecond accuracy and
    // may wait 1ms instead of our set value.

    // TODO: Make sure this doesn't break things on Windows. It really shouldn't!
    
    Sleep(1); 
    //std::this_thread::sleep_for(128us);
    return ret;
}

bool sendCommands(long long int tempPacket)
{
    bool success = false;
//    QList<long> keys = sendCommandQueue.keys();
    //cout << "1-5: 0x" << hex<<tempPacket<<endl;//转成十六进制字符串
    //            cam->set(cv::CAP_PROP_GAMMA, tempPacket);
    success = camSetProperty(cam, cv::CAP_PROP_CONTRAST, (tempPacket & 0x00000000FFFF));
    success = camSetProperty(cam, cv::CAP_PROP_GAMMA, (tempPacket & 0x0000FFFF0000) >> 16) && success;
    success = camSetProperty(cam, cv::CAP_PROP_SHARPNESS, (tempPacket & 0xFFFF00000000) >> 32) && success;
    if (!success)
        cout << "Send setting failed";
    return success;
}
void mexFunction(int nlhs, mxArray* plhs[], int nrhs, mxArray* prhs[])
{
    double *a;
    double b,c;
    plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
    a=mxGetPr(plhs[0]);//得到第一个接收输出变量的地址
    b=*(mxGetPr(prhs[0]));
    c=*(mxGetPr(prhs[1]));
    int connectionState;
    connectionState = 0;
    cam = new cv::VideoCapture;
//    cam = a;

    auto apiPreference = cv::CAP_ANY;
    std::string apiName;
    //windows
    apiPreference = cv::CAP_DSHOW;
    apiName = "DSHOW";
    if (cam->open(0, apiPreference)) {
        // we got our preferred backend!
        connectionState = 1;
        m_connectionType = apiName;
    }
    else {
        // connecting again using default backend
        if (cam->open(0)) {
            connectionState = 2;
            m_connectionType = "OTHER";
        }
    }  
   bool success = false;
   
   success = sendCommands(b) & sendCommands(c);
   *a=double(success);
   cam->release();
    //cout<<"hello world"<<endl;
    return ;
}

