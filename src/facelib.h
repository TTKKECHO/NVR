#ifndef __FACELIB_H
#define __FACELIB_H	 

#include <fstream>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include "HCNetSDK.h"


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
int add_face(long lUserID,std::string FDID,std::string filepath);
int get_FDLib_capabilities(long lUserID);


#endif