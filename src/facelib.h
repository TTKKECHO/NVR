#ifndef __FACELIB_H
#define __FACELIB_H	 

#include <fstream>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <libxml2/libxml/xmlmemory.h>
#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/tree.h>
#include "HCNetSDK.h"
#include <jsoncpp/json/json.h>


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

int add_face(long lUserID,std::string FDID,std::string filename);
int returnFileType(std::string filepath);
int get_FDLibList(long lUserID);
int get_FDLib(long lUserID,char *FDID);
int get_FDLibPics(long lUserID,char *FDID);
int add_face(long lUserID,std::string FDID,std::string filepath,Json::Value jsObject);
int get_FDLib_capabilities(long lUserID);
xmlBufferPtr get_facelib_appenddata(Json::Value jsObject);



#endif