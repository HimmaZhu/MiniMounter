#ifndef _CONNECTTOCAM_H
#define _CONNECTTOCAM_H

// #include <C:\Users\Dell\Downloads\opencv345\build\include\opencv2\core\core.hpp>
// #include <C:\Users\Dell\Downloads\opencv345\build\include\opencv2\highgui.hpp>
// // #include <C:\Users\Dell\Downloads\opencv345\build\include\opencv2\highgui\highgui.hpp>
// // #include <C:\Users\Dell\Downloads\opencv345\build\include\opencv2\highgui\highgui_c.hpp>
// #include <C:\Users\Dell\Downloads\opencv345\build\include\opencv2\opencv.hpp>
// #include <C:\Users\Dell\Downloads\opencv345\build\include\opencv2\imgproc.hpp>
// #include <C:\Users\Dell\Downloads\opencv345\build\include\opencv2\videoio.hpp>
// #include <C:\Users\Dell\Downloads\opencv454\opencv\build\include\opencv2\core\utils\logger.hpp>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui.hpp>
// #include <C:\Users\Dell\Downloads\opencv345\build\include\opencv2\highgui\highgui.hpp>
// #include <C:\Users\Dell\Downloads\opencv345\build\include\opencv2\highgui\highgui_c.hpp>
#include <opencv2\opencv.hpp>
#include <opencv2\imgproc.hpp>
#include <opencv2\videoio.hpp>
#include <opencv2\core\utils\logger.hpp>
#include<iostream>
#include<string>
#include<vector>
//#include <windows.h>
#include <stdint.h>
#include <map>

// #include "ConnectToCam.cpp"
using namespace std;

int m_cameraID;
cv::VideoCapture* cam;
vector<long> sendCommandQueueOrder;
map<long, vector<uint8_t>> sendCommandQueue;

double m_playbackFPS;
std::string m_playbackFolderPath;
std::string m_playbackFilePrefix;
int m_playbackFileIndex;

bool m_isStreaming;
bool m_stopStreaming;

int m_expectedWidth;
int m_expectedHeight;
double m_pixelClock;
std::string m_connectionType;

bool connect2Camera(int cameraID);
int connect2Video(std::string folderPath, std::string filePrefix, float playbackFPS);
static bool camSetProperty(cv::VideoCapture* cam, int propId, double value);
void setPropertyI2C(long preambleKey, vector<uint8_t> packet);
bool sendCommands();
bool attemptReconnect();

//void video_capture_test();
#endif
