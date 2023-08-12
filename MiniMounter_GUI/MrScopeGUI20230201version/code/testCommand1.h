#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include<iostream>
#include<string>
#include<vector>
//#include <windows.h>
#include <stdint.h>
#include <map>
#include <opencv2/core/utils/logger.hpp>
using namespace std;

// ------- Defines for UVC to I2C communication ----------
// Will be replaced once Miniscope communication is moved to vendor class USB
#define PROTOCOL_I2C            -2
#define PROTOCOL_SPI            -3
#define SEND_COMMAND_VALUE_H    -5
#define SEND_COMMAND_VALUE_L    -6
#define SEND_COMMAND_VALUE      -6
#define SEND_COMMAND_VALUE_H16  -7
#define SEND_COMMAND_VALUE_H24  -8
#define SEND_COMMAND_VALUE2_H   -9
#define SEND_COMMAND_VALUE2_L   -10
#define SEND_COMMAND_ERROR      -20
// ---------------------------------------------------------

// ------------- ERRORS ----------------
#define VIDEODEVICES_JSON_LOAD_FAIL     1
// -------------------------------------

// ----- Size of frame buffer in RAM for each device -------
#define FRAME_BUFFER_SIZE   128
#define BASELINE_FRAME_BUFFER_SIZE  128
// ---------------------------------------------------------


void setPropertyI2C(long preambleKey, vector<uint8_t> packet);
int processString2Int(std::string s);
vector<map<std::string, int>> parseSendCommand(vector<map<std::string, std::string>>  sendCommand);
bool sendCommands(cv::VideoCapture* cam,long long int tempPacket);
static bool camSetProperty(cv::VideoCapture* cam, int propId, double value);
void Generateprops();
void handlePropChangedSignal(std::string type, double displayValue, double i2cValue, double i2cValue2,int cameraID);


vector<long> sendCommandQueueOrder;
map<long, vector<uint8_t>> sendCommandQueue;
cv::VideoCapture* cam;
int m_cameraID;
std::string m_connectionType;

map<string, vector<map<string, int>>> m_controlSendCommand;
double m_lastLED0Value;