#ifndef __COMMON_H
#define __COMMON_H	 

#include <fstream>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include "HCNetSDK.h"

#define ALARM 1
#define ADDFACE 0


void Demo_SDK_Version();
int GBK_TO_UTF8(char *gbkdata,size_t len_gbk,char *utfdata,size_t len_utf);
std::string ReturnFileName(BYTE *buffer);
void charToUChar(char* str_char,unsigned char * str_uchar,int length);


#endif