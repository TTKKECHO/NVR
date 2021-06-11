#ifndef __COMMON_H
#define __COMMON_H	 

#include <fstream>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <jsoncpp/json/json.h>
#include "HCNetSDK.h"

#define ALARM 0
#define ADDFACE 0


struct DEVICE_INFO
{
    std::string device_id;
    std::string facelib;
    long user_id;
};

extern DEVICE_INFO device_info;
enum TIME_TYPE
{
    SEC=0,
    MSEC,
    USEC
};

void Demo_SDK_Version();
int GBK_TO_UTF8(char *gbkdata,size_t len_gbk,char *utfdata,size_t len_utf);
std::string ReturnFileName(BYTE *buffer);
void charToUChar(char* str_char,unsigned char * str_uchar,int length);
Json::Value getConfig();
int setConfig(Json::Value data);
std::string getTimeStamp(int type = SEC);
std::string getLocalTime(int type = SEC);
long NVR_Init(Json::Value config);

        
       



#endif