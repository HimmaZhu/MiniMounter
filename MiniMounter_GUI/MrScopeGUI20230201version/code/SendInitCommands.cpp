#include "opencvmex.hpp"
#include "SendInitCommands.h"
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


void setPropertyI2C(long preambleKey, vector<uint8_t> packet)
{
    // add newEvent to the queue for sending new settings to camera
    // overwrites data of previous preamble event that has not been sent to camera yet
    if (!sendCommandQueue.count(preambleKey))
        sendCommandQueueOrder.push_back(preambleKey);
    sendCommandQueue[preambleKey] = packet;
}

int processString2Int(std::string s)
{
    // Should return a uint8 type of value (0 to 255)
    bool ok = false;
    int value;
    int size = s.size();
    if (size == 0) {
       // cout << "No data in string to convert to int" << endl;
        value = SEND_COMMAND_ERROR;
        ok = false;
    }
    else if (s.substr(0, 2) == "0x") //s.left(2) substr(第几位开始，截取后字符串长度)
    {
        // HEX
        try
        {
            value = stoi(s.substr(2, s.length() - 2), 0, 16);
            ok = true;
        }
        catch (...)
        {
            ok = false;
        }
    }
    else if (s.substr(0, 2) == "0b") {
        // Binary
        try
        {
            value = stoi(s.substr(2, s.length() - 2), 0, 2);
            ok = true;
        }
        catch (...)
        {
            ok = false;
        }
    }
    else {
        try
        {
            //value = stoi(s.substr(2, s.length() - 2), 0, 10);
            value = stoi(s , 0, 10);
            ok = true;
        }
        catch (...)
        {
            ok = false;
        }
        //        qDebug() << "String is " << s;
        if (ok == false) {
            // This is then a string
            if (s == "I2C")
                value = PROTOCOL_I2C;
            else if (s == "SPI")
                value = PROTOCOL_SPI;
            else if (s == "valueH24")
                value = SEND_COMMAND_VALUE_H24;
            else if (s == "valueH16")
                value = SEND_COMMAND_VALUE_H16;
            else if (s == "valueH")
                value = SEND_COMMAND_VALUE_H;
            else if (s == "valueL")
                value = SEND_COMMAND_VALUE_L;
            else if (s == "value")
                value = SEND_COMMAND_VALUE;
            else if (s == "value2H")
                value = SEND_COMMAND_VALUE2_H;
            else if (s == "value2L")
                value = SEND_COMMAND_VALUE2_L;
            else
                value = SEND_COMMAND_ERROR;
            ok = true;
        }
    }
      //  cout<<"value "<<value<<endl;
    if (ok == true)
        return value;
    else
        return SEND_COMMAND_ERROR;
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
    bool success = false;
    long key;
    std::vector<uint8_t> packet;
    long long int tempPacket;
    //cout << "New Loop";
    //cout << "Queue length is " << sendCommandQueueOrder.size()<<endl;
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
            //cout << "Send setting succeeded";
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


vector<map<std::string, int>> parseSendCommand(vector<map<std::string, std::string>>  sendCommand)
{
    map<std::string, std::string> temp_map;
    map<std::string, std::string> ::iterator it;
    vector<map<std::string, int>> output;
    map<std::string, int> commandStructure;
    map<std::string, int> ::iterator it1;
    map<std::string, std::string> temp_map2;
    map<std::string, int> processed_map;
    std::string map_key;
    std::string map_value_string;
    int map_value;

    for (int i = 0; i < sendCommand.size(); i++) {
        temp_map2 = sendCommand[i];
        for (it = temp_map2.begin(); it != temp_map2.end(); it++) {
            //cout << it->first << "    " << it->second << endl;
             map_key = it->first;
            if (typeid((it->second)) == typeid(std::string)) {
                map_value = processString2Int((it->second));
                //cout << "map value "<<map_value << endl;
                commandStructure.insert(make_pair(map_key, map_value));
                //commandStructure.insert(make_pair(it->first, map_value));
            }
            else if (typeid((it->second)) == typeid(double)) {
                map_value = atoi(map_value_string.c_str());
                commandStructure.insert(make_pair(it->first, map_value));
            }

        }
        output.push_back(commandStructure);
        commandStructure.clear();
    }
    return output;
}

bool sendInitCommands(int cameraID)
{
    bool success = true;
// Sends out the commands in the miniscope json config file under Initialize
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

    vector<uint8_t> packet;
    long preambleKey;
    int tempValue;
    vector<map<std::string, std::string>>  sendCommand_strstr;
    vector<map<std::string, int>>  sendCommand;

    //initialize file
    vector<map<std::string, std::string>>  initialize;
    map<std::string, std::string> temp_map;
    temp_map.insert(make_pair("description", "Speed up i2c bus timer to 50us max"));
    temp_map.insert(make_pair("protocol", "I2C"));
    temp_map.insert(make_pair("addressW", "0xC0"));
    temp_map.insert(make_pair("regLength", "1"));
    temp_map.insert(make_pair("reg0", "0x22"));
    temp_map.insert(make_pair("dataLength", "1"));
    temp_map.insert(make_pair("data0", "0b00000010"));
    sendCommand_strstr.push_back(temp_map);
    temp_map.clear();
    temp_map.insert(make_pair("description", "Decrease BCC timeout, units in 2ms XX"));
    temp_map.insert(make_pair("protocol", "I2C"));
    temp_map.insert(make_pair("addressW", "0xC0"));
    temp_map.insert(make_pair("regLength", "1"));
    temp_map.insert(make_pair("reg0", "0x20"));
    temp_map.insert(make_pair("dataLength", "1"));
    temp_map.insert(make_pair("data0", "0b00001010"));
    sendCommand_strstr.push_back(temp_map);
    temp_map.clear();
    temp_map.insert(make_pair("description", "Make sure DES has SER ADDR"));
    temp_map.insert(make_pair("protocol", "I2C"));
    temp_map.insert(make_pair("addressW", "0xC0"));
    temp_map.insert(make_pair("regLength", "1"));
    temp_map.insert(make_pair("reg0", "0x07"));
    temp_map.insert(make_pair("dataLength", "1"));
    temp_map.insert(make_pair("data0", "0xB0"));
    sendCommand_strstr.push_back(temp_map);
    temp_map.clear();
    temp_map.insert(make_pair("description", "Speed up I2c bus timer to 50u Max"));
    temp_map.insert(make_pair("protocol", "I2C"));
    temp_map.insert(make_pair("addressW", "0xB0"));
    temp_map.insert(make_pair("regLength", "1"));
    temp_map.insert(make_pair("reg0", "0x0F"));
    temp_map.insert(make_pair("dataLength", "1"));
    temp_map.insert(make_pair("data0", "0b00000010"));
    sendCommand_strstr.push_back(temp_map);
    temp_map.clear();
    temp_map.insert(make_pair("description", "Decrease BCC timeout, units in 2ms"));
    temp_map.insert(make_pair("protocol", "I2C"));
    temp_map.insert(make_pair("addressW", "0xB0"));
    temp_map.insert(make_pair("regLength", "1"));
    temp_map.insert(make_pair("reg0", "0x1E"));
    temp_map.insert(make_pair("dataLength", "1"));
    temp_map.insert(make_pair("data0", "0b00001010"));
    sendCommand_strstr.push_back(temp_map);
    temp_map.clear();
    temp_map.insert(make_pair("description", "sets allowable i2c addresses to send through serializer"));
    temp_map.insert(make_pair("protocol", "I2C"));
    temp_map.insert(make_pair("addressW", "0xC0"));
    temp_map.insert(make_pair("regLength", "1"));
    temp_map.insert(make_pair("reg0", "0x08"));
    temp_map.insert(make_pair("dataLength", "2"));
    temp_map.insert(make_pair("device0", "Sensor"));
    temp_map.insert(make_pair("data0", "0xB8"));
    temp_map.insert(make_pair("device1", "LED Driver"));
    temp_map.insert(make_pair("data1", "0b10011000"));
    sendCommand_strstr.push_back(temp_map);
    temp_map.clear();
    temp_map.insert(make_pair("description", "sets sudo allowable i2c addresses to send through serializer"));
    temp_map.insert(make_pair("protocol", "I2C"));
    temp_map.insert(make_pair("addressW", "0xC0"));
    temp_map.insert(make_pair("regLength", "1"));
    temp_map.insert(make_pair("reg0", "0x10"));
    temp_map.insert(make_pair("dataLength", "2"));
    temp_map.insert(make_pair("device0", "Sensor"));
    temp_map.insert(make_pair("data0", "0xB8"));
    temp_map.insert(make_pair("device1", "LED Driver"));
    temp_map.insert(make_pair("data1", "0b10011000"));
    sendCommand_strstr.push_back(temp_map);
    temp_map.clear();
    temp_map.insert(make_pair("description", "Image sensor soft reset"));
    temp_map.insert(make_pair("protocol", "I2C"));
    temp_map.insert(make_pair("addressW", "0xB8"));
    temp_map.insert(make_pair("regLength", "1"));
    temp_map.insert(make_pair("reg0", "0x0C"));
    temp_map.insert(make_pair("dataLength", "2"));
    temp_map.insert(make_pair("data0", "0"));
    temp_map.insert(make_pair("data1", "1"));
    sendCommand_strstr.push_back(temp_map);
    temp_map.clear();
    temp_map.insert(make_pair("description", "Image sensor disable auto gain"));
    temp_map.insert(make_pair("protocol", "I2C"));
    temp_map.insert(make_pair("addressW", "0xB8"));
    temp_map.insert(make_pair("regLength", "1"));
    temp_map.insert(make_pair("reg0", "0xAF"));
    temp_map.insert(make_pair("dataLength", "2"));
    temp_map.insert(make_pair("data0", "0"));
    temp_map.insert(make_pair("data1", "0"));
    sendCommand_strstr.push_back(temp_map);
    temp_map.clear();
    map<std::string, std::string> commandstr;
//     for (int i = 0; i < sendCommand_strstr.size(); i++) {
//         commandstr = sendCommand_strstr[i];
//         cout<<commandstr["description"]<<endl;
//     }
    sendCommand = parseSendCommand(sendCommand_strstr);

    map<std::string, int> command;
    //cout<<"sendCommand size "<<sendCommand.size()<<endl;

    for (int i = 0; i < sendCommand.size(); i++) {
        // Loop through send commands
        command = sendCommand[i];
        packet.clear();
        if (command["protocol"] == PROTOCOL_I2C) {
            preambleKey = 0;
            packet.push_back(command["addressW"]);
            preambleKey = (preambleKey << 8) | packet.at(packet.size() - 1);
            //cout<<hex<<command["addressW"]<<endl;
            for (int j = 0; j < command["regLength"]; j++) {
                packet.push_back(command["reg" + std::to_string(j)]);
                preambleKey = (preambleKey << 8) | packet.at(packet.size() - 1);
            }
            for (int j = 0; j < command["dataLength"]; j++) {
                tempValue = command["data" + std::to_string(j)];
                //cout<<hex<<tempValue<<endl;
                packet.push_back(tempValue);
                preambleKey = (preambleKey << 8) | packet.at(packet.size() - 1);
            }
            //        qDebug() << packet;
            //        preambleKey = 0;
            //        for (int k = 0; k < (command["regLength"]+1); k++)
            //            preambleKey |= (packet[k]&0xFF)<<(8*k);
            setPropertyI2C(preambleKey, packet);
//             vector<map<std::string, int>>::iterator k = sendCommand.begin();
//             sendCommand.erase(k);
            if (!empty(sendCommandQueue)){
                success=success&sendCommands();
                //cout<<sendCommandQueueOrder.size()<<" !empty"<<endl;
            }
            sendCommandQueue.clear();
            //cout << i << " is sent successfully"<<endl;
            //cout << command << " is sent successfully";
        }
        else {
            cout << command["protocol"] << " initialize protocol not yet supported";
        }
            
    }
   
    cam->release();
    return success;
    
}

void mexFunction(int nlhs, mxArray* plhs[], int nrhs, mxArray* prhs[])
{
    //sendInitCommands(0);
    bool success = false;
    plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
    double *a;
    a=mxGetPr(plhs[0]);//得到第一个接收输出变量的地址
    success = sendInitCommands(0);
    *a=double(success);
    
    //cout<<"initialize success!"<<endl;
    return;
}