#ifndef __ALARM_H
#define __ALARM_H	 

#include "HCNetSDK.h"
#include "Common.h"
#include "http_libcurl.h"
#include "rabbitmq.h"
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <sstream>


#define IMG_BUFFER 1024*1024
//时间解析宏定义
#define GET_YEAR(_time_)      (((_time_)>>26) + 2000) 
#define GET_MONTH(_time_)     (((_time_)>>22) & 15)
#define GET_DAY(_time_)       (((_time_)>>17) & 31)
#define GET_HOUR(_time_)      (((_time_)>>12) & 31) 
#define GET_MINUTE(_time_)    (((_time_)>>6)  & 63)
#define GET_SECOND(_time_)    (((_time_)>>0)  & 63)

BOOL CALLBACK MessageCallback(LONG lCommand, NET_DVR_ALARMER *pAlarmer, char *pAlarmInfo, DWORD dwBufLen, void* pUser);
std::string returnImgData(BYTE *buffer );
std::string returnImgData(std::string url);
int uploadAlarm(std::string name,Json::Value data);
std::string returntime(std::string src);
std::string returntRect(Json::Value src);


#endif