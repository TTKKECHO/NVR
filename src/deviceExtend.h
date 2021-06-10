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

typedef  std::uint64_t hash_t;  
constexpr hash_t basis = 0xCBF29CE484222325ull;        
constexpr hash_t prime = 0x100000001B3ull;         

hash_t hash_( char  const * str);
constexpr hash_t hash_compile_time( char  const * str, hash_t last_value = basis);
constexpr unsigned  long  long  operator  ""  _hash( char  const * p,  size_t );


int CapImg(long lUserID,int channel);
void zoomin(int lUserID,int channel,int sleep_time=10000);
void zoomout(int lUserID,int channel,int sleep_time=10000);

void selectFun(Json::Value message,long user_id);
void selectQueryFun(std::string name,Json::Value payload,long user_id);
void selectUploadFun(std::string name,Json::Value payload,long user_id);
void selectControlFun(std::string name,Json::Value payload,long user_id);
void selectSetConfigFun(std::string name,Json::Value payload,long user_id);

int setLogInfo(Json::Value payload);
int setCapture(Json::Value payload);
int setRemoteInfo(Json::Value payload);
int setProjectInfo(Json::Value payload);







#endif
