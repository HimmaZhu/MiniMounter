#include "opencvmex.hpp"
// ﻿#include "mex.h"
#include "ConnectToCam.h"
// #include <C:\Users\Dell\Downloads\opencv345\build\include\opencv2\core\core.hpp>
// #include <C:\Users\Dell\Downloads\opencv345\build\include\opencv2\highgui.hpp>
// // #include <C:\Users\Dell\Downloads\opencv345\build\include\opencv2\highgui\highgui.hpp>
// // #include <C:\Users\Dell\Downloads\opencv345\build\include\opencv2\highgui\highgui_c.hpp>
// #include <C:\Users\Dell\Downloads\opencv345\build\include\opencv2\opencv.hpp>
// #include <C:\Users\Dell\Downloads\opencv345\build\include\opencv2\imgproc.hpp>
// #include <C:\Users\Dell\Downloads\opencv345\build\include\opencv2\videoio.hpp>
// #include <C:\Users\Dell\Downloads\opencv345\build\include\opencv2\core\utils\logger.hpp>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui.hpp>
// #include <C:\Users\Dell\Downloads\opencv345\build\include\opencv2\highgui\highgui.hpp>
// #include <C:\Users\Dell\Downloads\opencv345\build\include\opencv2\highgui\highgui_c.hpp>
#include <opencv2\opencv.hpp>
#include <opencv2\imgproc.hpp>
#include <opencv2\videoio.hpp>
#include <opencv2\core\utils\logger.hpp>
#include <string>
#include <stdint.h>
// #include <windows.h>
#include <map>
// #include <opencv2\opencv.hpp>
#include <iostream>
#include <algorithm>
#include <windows.h>
// #include <chrono>
// 
// 
// using namespace std::chrono_literals;
using namespace cv;
using namespace std;


void setPropertyI2C(long preambleKey, vector<uint8_t> packet)
{
    // add newEvent to the queue for sending new settings to camera
    // overwrites data of previous preamble event that has not been sent to camera yet
    if (!sendCommandQueue.count(preambleKey))
        sendCommandQueueOrder.push_back(preambleKey);
    sendCommandQueue[preambleKey] = packet;
}

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

bool sendCommands()
{
    //    QList<long> keys = sendCommandQueue.keys();
    bool success;
    success = false;
    long key;
    std::vector<uint8_t> packet;
    long long int tempPacket;
    //    qDebug() << "New Loop";
    //    qDebug() << "Queue length is " << sendCommandQueueOrder.length();
    while (!sendCommandQueueOrder.empty()) {
        key = sendCommandQueueOrder.at(0);//T &QVector::first(), returns a reference to the first item in the vector
        packet = sendCommandQueue[key];
        //std::cout << packet;  //调试packet
        if (packet.size() < 6) {
            tempPacket = (long long int)packet[0]; // address
            tempPacket |= (((long long int)packet.size()) & 0xFF) << 8; // data length  |=按位或并
            for (int j = 1; j < packet.size(); j++)
                tempPacket |= ((long long int)packet[j]) << (8 * (j + 1));
            //cout << "1-5: 0x" << hex<<tempPacket<<endl;//转成十六进制字符串
            //            cam->set(cv::CAP_PROP_GAMMA, tempPacket);
            success = camSetProperty(cam, cv::CAP_PROP_CONTRAST, (tempPacket & 0x00000000FFFF));
            success = camSetProperty(cam, cv::CAP_PROP_GAMMA, (tempPacket & 0x0000FFFF0000) >> 16) && success;
            success = camSetProperty(cam, cv::CAP_PROP_SHARPNESS, (tempPacket & 0xFFFF00000000) >> 32) && success;
            if (!success)
                cout << "Send setting failed";

            sendCommandQueue.erase(0);  //sendCommandQueue.remove(key);
            sendCommandQueueOrder.erase(sendCommandQueueOrder.begin());   //sendCommandQueueOrder.removeFirst();
        }
        else if (packet.size() == 6) {
            tempPacket = (long long int)packet[0] | 0x01; // address with bottom bit flipped to 1 to indicate a full 6 byte package
            for (int j = 1; j < packet.size(); j++)
                tempPacket |= ((long long int)packet[j]) << (8 * (j));
            //cout << "6: 0x" << std::string::number(tempPacket, 16);

            //            success = cam->set(cv::CAP_PROP_GAIN, 0x1122ff20);

            success = camSetProperty(cam, cv::CAP_PROP_CONTRAST, (tempPacket & 0x00000000FFFF));
            success = camSetProperty(cam, cv::CAP_PROP_GAMMA, (tempPacket & 0x0000FFFF0000) >> 16) && success;
            success = camSetProperty(cam, cv::CAP_PROP_SHARPNESS, (tempPacket & 0xFFFF00000000) >> 32) && success;
            if (!success)
                cout << "Send setting failed";

            sendCommandQueue.erase(0);  //sendCommandQueue.remove(key);
            sendCommandQueueOrder.erase(sendCommandQueueOrder.begin());   //sendCommandQueueOrder.removeFirst();
        }
        else {
            //TODO: Handle packets longer than 6 bytes
            sendCommandQueue.erase(0);  //sendCommandQueue.remove(key);
            sendCommandQueueOrder.erase(sendCommandQueueOrder.begin());   //sendCommandQueueOrder.removeFirst();
        }

    }
    return success;

}



bool connect2Camera(int cameraID)
{
    bool success = false;
    int connectionState;
    connectionState = 0;
    m_cameraID = cameraID;
    cam = new cv::VideoCapture;

    auto apiPreference = cv::CAP_ANY;
    std::string apiName;
    //windows
    apiPreference = cv::CAP_DSHOW;
    apiName = "DSHOW";
    if (cam->open(m_cameraID, apiPreference)) {
        // we got our preferred backend!
        connectionState = 1;
        m_connectionType = apiName;
    }
    else {
        // connecting again using default backend
        if (cam->open(m_cameraID)) {
            connectionState = 2;
            m_connectionType = "OTHER";
        }
    }
    //cout << connectionState << endl;
    // We need to make sure the MODE of the SERDES is correct
    // This needs to be done before any other commands are sent over SERDES
    // Currently this is for the 913/914 TI SERES
    // TODO: Probably should move this somewhere else
    std::vector<uint8_t> packet;
    if (m_pixelClock > 0 && connectionState != 0) {
        if (m_pixelClock <= 50) {
            // Set to 12bit low frequency in this case

            // DES
            packet.push_back(0xC0); // I2C Address
            packet.push_back(0x1F); // reg
            packet.push_back(0b00010000); // data
            setPropertyI2C(0, packet);

            // SER
            packet.clear();
            packet.push_back(0xB0); // I2C Address
            packet.push_back(0x05); // reg
            packet.push_back(0b00100000); // data
            setPropertyI2C(1, packet);

        }
        else {
            // Set to 10bit high frequency in this case

            // DES
            packet.clear();
            packet.push_back(0xC0); // I2C Address
            packet.push_back(0x1F); // reg
            packet.push_back(0b00010001); // data
            setPropertyI2C(0, packet);

            // SER
            packet.clear();
            packet.push_back(0xB0); // I2C Address
            packet.push_back(0x05); // reg
            packet.push_back(0b00100001); // data
            setPropertyI2C(1, packet);


        }
        success=sendCommands();
        //std::this_thread::sleep_for(500ms);
        Sleep(500); 


    }

    if (connectionState != 0) {
        cam->set(cv::CAP_PROP_FRAME_WIDTH, m_expectedWidth);
        cam->set(cv::CAP_PROP_FRAME_HEIGHT, m_expectedHeight);
        //std::this_thread::sleep_for(500ms);
        Sleep(500); 
    }

    cam->release();

    //return connectionState;
    return success;
}



void mexFunction(int nlhs, mxArray* plhs[], int nrhs, mxArray* prhs[])
{
    m_pixelClock = 26;
    m_expectedWidth = 752;
    m_expectedHeight = 480;
    bool success = false;
    plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
    double *a;
    a=mxGetPr(plhs[0]);//得到第一个接收输出变量的地址
    success = connect2Camera(0);
    *a=double(success);
    
//     double *a;
//     plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
//     a=mxGetPr(plhs[0]);//得到第一个接收输出变量的地址
//     b=*(mxGetPr(prhs[0]));
//     c=*(mxGetPr(prhs[1]));
//     *a=b+c;
    //cout<<success<<endl;
    return;
}
