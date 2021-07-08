#ifndef __COMMON_H
#define __COMMON_H	 

#include <fstream>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <jsoncpp/json/json.h>
#include <vector>
#include <map>
#include "HCNetSDK.h"

#define ALARM 0
#define ADDFACE 0


//存储设备基础信息
struct DEVICE_INFO
{
    std::string device_id;
    long user_id;
};

//时间精度标志
enum TIME_TYPE
{
    SEC=0,
    MSEC,
    USEC
};



extern DEVICE_INFO device_info;
extern std::map<std::string,std::string>worker_map;

void Demo_SDK_Version();
int GBK_TO_UTF8(char *gbkdata,size_t len_gbk,char *utfdata,size_t len_utf);
//std::string ReturnFileName(BYTE *buffer);
int ReturnFileName(BYTE *buffer,std::string &filename,int &len);
int ReturnFileName(std::string url,std::string &filename,int &len);

void charToUChar(char* str_char,unsigned char * str_uchar,int length);
Json::Value getConfig();
int setConfig(Json::Value data);
std::string getTimeStamp(int type = SEC);
std::string getLocalTime(int type = SEC);
long NVR_Init(Json::Value config);

        
       



#endif