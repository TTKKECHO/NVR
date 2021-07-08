#ifndef __DEVICEEXTEND_H
#define __DEVICEEXTEND_H

#include <iostream>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fstream>
#include <libxml2/libxml/xmlmemory.h>
#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/tree.h>
#include <jsoncpp/json/json.h>
#include "base64.h"
#include "rabbitmq.h"
#include "HCNetSDK.h"
#include "Common.h"



#define BUFFERSIZE 5000000
#define BUFFERSIZE_FACELIB 2048
#define BUFFERSIZE_WORKER  2048*2048

typedef  std::uint64_t hash_t;  
constexpr hash_t basis = 0xCBF29CE484222325ull;        
constexpr hash_t prime = 0x100000001B3ull;         

hash_t hash_( char  const * str);
constexpr hash_t hash_compile_time( char  const * str, hash_t last_value = basis);
constexpr unsigned  long  long  operator  ""  _hash( char  const * p,  size_t );

//查询类型
enum QUERYTYPE
{
    QUERY_LOG = 0,
    QUERY_REMOTE ,
    QUERY_PROJECT ,
    QUERY_CAPTURE

};

//返回结果，DE_ERROR为0为失败，DE_OK为1为成功
enum DE_STATUS
{
    DE_ERROR = 0,
    DE_OK 
};

//图片格式：1- jpg，2- bmp，3- png，4- SWF，5- GIF
enum FILE_TYPE
{
    JPG=1,
    BMP,
    PNG,
    SWF,
    GIF,
    XML
};

//选择命令执行函数

int selectFun(Json::Value message);
int selectQueryFun(std::string name,Json::Value payload);
int selectControlFun(std::string name,Json::Value payload);
int selectSetConfigFun(std::string name,Json::Value payload);

//设置设备参数

int setLogInfo(Json::Value payload);
int setCapture(Json::Value payload);
int setRemoteInfo(Json::Value payload);
int setProjectInfo(Json::Value payload);

//设备控制函数

int CapImg(int channel);
int ptzControl(int channel,int cmd,int sleep_time=10000);
int addFace(Json::Value data);
int deleteFace(Json::Value data);
int addFacelib(std::string name,std::string thresholdValue,std::string &FDID);
int deleteFacelib(std::string FDID);

//返回命令执行结果

int returnRAMQ(Json::Value data);
int returnResult(std::string name , int result);
int returnPtzCtrl(Json::Value data);
int returnQueryInfo(int type);
int returnFacelibInfo();
int returnLogStatusInfo();
int returnWorkerInfo(Json::Value data);
int returnWorkerListInfo(Json::Value data);
int returnAddFacelib(Json::Value data);
int returnDeleteFacelib(Json::Value data);


//异常结果上传

int uploadWarnning(std::string warning);

//其他函数

int returnFileType(std::string filepath);
int getFacelibInfo(std::map<std::string,std::string> &facelib_map);
int getWorkerList(std::string FDID,std::map<std::string,std::string> &worker_map);
int getWorkerInfo(std::string FDID,std::string ID,Json::Value &workerInfo);
int getWorkerInfoWithPic(std::string FDID,std::string ID,Json::Value &workerinfo);
int getWorkerAppendInfo(std::string ID);
int addWorkerInfo(std::string FDID,Json::Value data,std::string &PID);
int deleteWorkerInfo(std::string FDID,std::string ID);
int checkLogStatus();
int uploadCapImg(int channel,int quality = 0 ,int picsize =0);

xmlBufferPtr get_facelib_appenddata(Json::Value jsObject);



#endif
