#include "Common.h"
#include <fstream>
#include <iostream>
#include <string.h>
#include <iconv.h>
#include <time.h>
#include <sys/time.h>

using namespace std;




/**
 * @brief   打印设备SDK版本信息
 * @author  liuguang
 * @date    2021/05/27 
 * @param   [NULL]
 * @return  [NULL]
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
 * @brief   GBK转UTF8
 * @author  liuguang
 * @date    2021/05/24
 * @param   [in] gbkdata：GBK数据指针
 * @param   [in] len_gbk：GBK数据长度
 * @param   [in] utfdata：UTF8数据指针
 * @param   [in] len_utf：UTF8数据长度（GBK数据长度+2）
 * @return  [int] 1为成功，0为失败
 */
int GBK_TO_UTF8(char *gbkdata,size_t len_gbk,char *utfdata,size_t len_utf)
{
	iconv_t cd;
	char **pin = &gbkdata;
	char ** pout = &utfdata;
	cd = iconv_open("utf8","gbk");
	if(cd == 0)
		return 0;
	memset(utfdata,0,len_utf);
	if(iconv(cd,pin,&len_gbk,pout,&len_utf)==1)
		return 0;
	iconv_close(cd);
	*pout = 0;
	return 1;
}

/**
 * @brief   获取图片在设备内的名称
 * @author  liuguang
 * @date    2021/05/24
 * @param   [in] buffer：硬盘录像机返回的包含图片url的数据
 * @param   [in] filename：图片名称
 * @param   [in] len：图片大小
 * @return  [int] 0为失败，1为成功
 */
int ReturnFileName(BYTE *buffer,std::string &filename,int &len)
{
    int status = 0;
    char *faceFileName = (char*)buffer;
    char fileBuffer[4096];
    status = GBK_TO_UTF8(faceFileName,strlen(faceFileName),fileBuffer,strlen(faceFileName)+2);
    if(status == 0) return status;
    std::string str(fileBuffer);
    int index_head,index_tail;
    index_head = str.find("=");
    index_tail = str.find("&");
    //printf("\nhead:%d\ttail:%d\n",index_head,index_tail);

    std::string result;
    result=str.substr(index_head+1,index_tail-index_head-1);
    len = atoi(str.substr(index_tail+6,str.size()).c_str());
    filename = result;
    return status;
}

/**
 * @brief   获取图片在设备内的名称
 * @author  liuguang
 * @date    2021/05/24
 * @param   [in] url：硬盘录像机返回的包含图片url的数据
 * @param   [in] filename：图片名称
 * @param   [in] len：图片大小
 * @return  [int] 0为失败，1为成功
 */
int ReturnFileName(std::string url,std::string &filename,int &len)
{

    std::string str=url;
    int index_head,index_tail;
    index_head = str.find("=");
    index_tail = str.find("&");
    if(!index_head && !index_tail) return 0;
    //printf("\nhead:%d\ttail:%d\n",index_head,index_tail);
    std::string result;
    result=str.substr(index_head+1,index_tail-index_head-1);
    len = atoi(str.substr(index_tail+6,str.size()).c_str());
    filename = result;
    return 1;
}


/**
 * @brief   char 转 uchar (没用)
 * @author  liuguang
 * @date    2021/05/31
 * @param   [in] str_char ：char指针
 * @param   [in] str_uchar ：uchar指针
 * @param   [in] length ：字符串长度 
 * @return  NULL
 */
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

/**
 * @brief   获取本地时间
 * @author  liuguang
 * @date    2021/05/31
 * @param   [in] type ：返回时间类型，SEC到秒，MSEC毫秒，USEC微秒 
 * @return  [string] 时间，格式 YYYYMMDDHHMMSS(mmuu)
 */
std::string getLocalTime(int type)
{
    std::string result;
    time_t now;
    tm *info;
    time(&now);
    info = localtime(&now);
    char time_sec[15];
    strftime(time_sec,15,"%Y%m%d%H%M%S",info);
    time_sec[14]='\0';
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
            char time_msec[20];
            sprintf(time_msec,"%14s.%03ld",time_sec,tv.tv_usec/1000);
            // printf("\n msec:%ld\n",tv.tv_usec);
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


/**
 * @brief   获取时间戳
 * @author  liuguang
 * @date    2021/05/31 
 * @param   [in] type ：返回时间类型，SEC到秒，MSEC毫秒，USEC微秒 
 * @return  [string] 时间戳
 */
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


/**
 * @brief   获取配置文件内容
 * @author  liuguang
 * @date    2021/05/31 
 * @param   [NULL]
 * @return  [Json::Value] 配置文件内容
 */
Json::Value getConfig()
{
    Json::Reader jsreader;
	Json::Value jsObject;

	ifstream f;
	f.open("./src/config.json",ios::in|ios::out);
    if(!f.is_open()){return jsObject;}
    if(!jsreader.parse(f,jsObject,false)){return jsObject;}
    f.close();
    return jsObject;
	
}


/**
 * @brief   设置配置文件内容
 * @author  liuguang
 * @date    2021/05/31
 * @param   [in] data ：配置文件内容
 * @return  [int] 1为成功，0为失败
 */
int setConfig(Json::Value data)
{
    Json::StyledWriter writer;
    std::string strdata = writer.write(data);
    ofstream f;
	f.open("./src/config.json",ios::in|ios::out);
    if(!f.is_open()){return 0;}
    f << strdata;
    f.close();
    return 1;
}


/**
 * @brief   初始化硬盘录像机
 * @author  liuguang
 * @date    2021/05/31
 * @param   [in] config ：录像机配置文件内容
 * @return  [long] 设备登录ID，-1为失败
 */
long NVR_Init(Json::Value config)
{
    /************************声明变量************************/
    NET_DVR_USER_LOGIN_INFO LoginInfo  = {0};					//设备登录参数
    NET_DVR_DEVICEINFO_V40  DeviceInfo = {0};					//设备信息
	// NET_DVR_COMPRESSIONCFG_V30 device_compression = {0};
	LONG user_id;
	
	//load LoginInfo
	LoginInfo.bUseAsynLogin = false;
    LoginInfo.wPort = atoi(config["port"].asCString());
    memcpy(LoginInfo.sDeviceAddress, config["device_ip"].asCString(), NET_DVR_DEV_ADDRESS_MAX_LEN);
    memcpy(LoginInfo.sUserName,config["username"].asCString() , NAME_LEN);
    memcpy(LoginInfo.sPassword, config["password"].asCString(), NAME_LEN);
	
/************************Start************************/
	printf("[DEVICE INIT]Device Initialization...\n");
	//设备初始化，设置超时、重连时间
	NET_DVR_Init();
	NET_DVR_SetConnectTime(2000,1);
	NET_DVR_SetReconnect(10000,true);
	//获取设备sdk版本号
    Demo_SDK_Version();
	//设置日志
    NET_DVR_SetLogToFile(3, (char*)"./sdkLog");
    //注册设备
    user_id = NET_DVR_Login_V40(&LoginInfo, &DeviceInfo);
    if (user_id < 0)
    {
        printf("Login error, %d\n", NET_DVR_GetLastError());
        printf("Press any key to quit...\n");
        NET_DVR_Cleanup();
        return HPR_ERROR;
    }
    return user_id;
}


