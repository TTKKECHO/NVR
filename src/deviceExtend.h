#ifndef __DEVICEEXTEND_H
#define __DEVICEEXTEND_H

#include <iostream>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include "HCNetSDK.h"
#include "Common.h"
#include <jsoncpp/json/json.h>
#include "base64.h"
#include "rabbitmq.h"


#define BUFFERSIZE 5000000
typedef  std::uint64_t hash_t;  
constexpr hash_t basis = 0xCBF29CE484222325ull;        
constexpr hash_t prime = 0x100000001B3ull;         

hash_t hash_( char  const * str);
constexpr hash_t hash_compile_time( char  const * str, hash_t last_value = basis);
constexpr unsigned  long  long  operator  ""  _hash( char  const * p,  size_t );


enum QUERYTYPE
{
    QUERY_LOG = 0,
    QUERY_REMOTE ,
    QUERY_PROJECT ,
    QUERY_CAPTURE

};

enum DE_STATUS
{
    DE_ERROR = 0,
    DE_OK 
};




void zoomin(int lUserID,int channel,int sleep_time=10000);
void zoomout(int lUserID,int channel,int sleep_time=10000);

/**选择命令执行函数**/
void selectFun(Json::Value message);
void selectQueryFun(std::string name,Json::Value payload);
void selectControlFun(std::string name,Json::Value payload);
void selectSetConfigFun(std::string name,Json::Value payload);

/**设置设备参数**/
int setLogInfo(Json::Value payload);
int setCapture(Json::Value payload);
int setRemoteInfo(Json::Value payload);
int setProjectInfo(Json::Value payload);

/**设备控制函数**/
int CapImg(int channel);
int ptzControl(int channel,int cmd,int sleep_time=10000);
int addFace();

/**返回命令执行结果**/
int returnResult(std::string name , int result);
int returnQueryInfo(int type);
int returnLogStatusInfo();
int returnFacelibInfo();
int returnWorkerInfo();
int returnAddface();
int returnCapture();
int returnPtz();









#endif
