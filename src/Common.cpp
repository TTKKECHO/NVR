#include "Common.h"
#include <fstream>
#include <iostream>
#include <string.h>
#include <iconv.h>
#include <time.h>
#include <sys/time.h>

using namespace std;


/**
 * @Description：打印设备SDK版本信息
 * @Author:liuguang
 * @Date:2021/05/27
 */
void Demo_SDK_Version()
{
    unsigned int uiVersion = NET_DVR_GetSDKBuildVersion();

    char strTemp[1024] = {0};
    sprintf(strTemp, "HCNetSDK V%d.%d.%d.%d\n", \
        (0xff000000 & uiVersion)>>24, \
        (0x00ff0000 & uiVersion)>>16, \
        (0x0000ff00 & uiVersion)>>8, \
        (0x000000ff & uiVersion));
    printf("%s",strTemp);
}


/**
 * @Description：GBK转UTF8
 * @Author:liuguang
 * @Date:2021/05/24
 * @Param:
 *      gbkdata：GBK数据指针
 *      len_gbk：GBK数据长度
 *      utfdata：UTF8数据指针
 *      len_utf：UTF8数据长度（GBK数据长度+2）
 * @return:0为成功，-1为失败
 */
int GBK_TO_UTF8(char *gbkdata,size_t len_gbk,char *utfdata,size_t len_utf)
{
	iconv_t cd;
	char **pin = &gbkdata;
	char ** pout = &utfdata;
	cd = iconv_open("utf8","gbk");
	if(cd == 0)
		return -1;
	memset(utfdata,0,len_utf);
	if(iconv(cd,pin,&len_gbk,pout,&len_utf)==1)
		return -1;
	iconv_close(cd);
	*pout = 0;
	return 0;
}

/**
 * @Description：获取图片在设备内的名称
 * @Author:liuguang
 * @Date:2021/05/24
 * @Param:
 *     buffer：硬盘录像机返回的包含图片url的数据
 * @return:图片名称
 */
std::string ReturnFileName(BYTE *buffer)
{

    char *faceFileName = (char*)buffer;
    char fileBuffer[4096];
    GBK_TO_UTF8(faceFileName,strlen(faceFileName),fileBuffer,strlen(faceFileName)+2);
    std::string str(fileBuffer);
    int index_head,index_tail;
    index_head = str.find("=");
    index_tail = str.find("&");
    std::string result;
    result=str.substr(index_head+1,index_tail-index_head-1);
    return result;
}

void charToUChar(char* str_char,unsigned char* str_uchar,int length)
{
	for (int i = 0;  i < length; i++) 
	{
        *str_uchar = (unsigned char)(*str_char);
		str_uchar++;
        str_char++;
	}
    return;
}


std::string getLocalTime(int type)
{
    std::string result;
    time_t now;
    tm *info;
    time(&now);
    info = localtime(&now);
    char time_sec[14];
    strftime(time_sec,14,"%Y%m%d%H%M%S",info);
    timeval tv;
    gettimeofday(&tv,NULL);
    switch(type)
    {
        case SEC:
        { 
            result = std::string(time_sec);
        }
        break;
        case MSEC:
        {
            char time_msec[18];
            sprintf(time_msec,"%s.%ld",time_sec,tv.tv_usec/1000);
            result = std::string(time_msec);
        }
        break;
        case USEC:
        {
            char time_usec[21];
            sprintf(time_usec,"%s.%ld",time_sec,tv.tv_usec);
            result = std::string(time_usec);

        }
        break;
        default:break;
    }  
    return result;

}

std::string getTimeStamp(int type )
{
    std::string result;
    timeval tv;
    gettimeofday(&tv,NULL);
    switch(type)
    {
        case SEC:
        { 
            char time_sec[10];
            sprintf(time_sec,"%ld",tv.tv_sec);
            result = std::string(time_sec);
        }
        break;
        case MSEC:
        {
            char time_msec[13];
            sprintf(time_msec,"%ld",tv.tv_sec*1000+tv.tv_usec/1000);
            result = std::string(time_msec);
        }
        break;
        case USEC:
        {
            char time_usec[16];
            sprintf(time_usec,"%ld",tv.tv_sec*1000000+tv.tv_usec);
            result = std::string(time_usec);

        }
        break;
        default:break;
    }  
    return result;



}