#include <iostream>
#include <fstream>
#include <stdbool.h> 
#include <jsoncpp/json/json.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "Common.h"
#include "HCNetSDK.h"
#include "CapPicture.h"
#include "PTZControl.h"
#include "http_libcurl.h"
#include "base64.h"
#include "alarm.h"
#include "facelib.h"
// #include <curl/curl.h>
// #include <curl/easy.h>

using namespace std;

int main()
{




/************************声明变量************************/
    NET_DVR_USER_LOGIN_INFO LoginInfo  = {0};					//设备登录参数
    NET_DVR_DEVICEINFO_V40  DeviceInfo = {0};					//设备信息
	NET_DVR_COMPRESSIONCFG_V30 device_compression = {0};
	LONG user_id;
	DWORD len_dword;
	int res;
	char funChoose = 'r';
	char channelChoose = 'r';
    int ip_channel;
	
	//load LoginInfo
	LoginInfo.bUseAsynLogin = false;
    LoginInfo.wPort = 8000;
	memcpy(LoginInfo.sDeviceAddress, "192.168.0.32", NET_DVR_DEV_ADDRESS_MAX_LEN);
    memcpy(LoginInfo.sUserName, "admin", NAME_LEN);
    memcpy(LoginInfo.sPassword, "211aAT518", NAME_LEN);
	//string login_ip;
	//printf("输入ip：");
	//cin>>login_ip;
	//memcpy(LoginInfo.sDeviceAddress, login_ip.c_str(), NET_DVR_DEV_ADDRESS_MAX_LEN);
	
/************************Start************************/
	printf("Device Initialization...\n");
	//设备初始化，设置超时、重连时间
	http_init();
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
	


	//cout<<now<<endl;
	//get_FDLibList(user_id);
	//get_FDLib(user_id,"EE96E5C51DE14E3EA22ABA2FED01E5DA"); 
	get_FDLibPics(user_id,"EE96E5C51DE14E3EA22ABA2FED01E5DA");
	//add_face(user_id,"EE96E5C51DE14E3EA22ABA2FED01E5DA","./Image/2.png");



#if ALARM
	//设置报警回调函数
    NET_DVR_SetDVRMessageCallBack_V31(MessageCallback, NULL);
  
    //启用布防
    LONG lHandle;
    NET_DVR_SETUPALARM_PARAM  struAlarmParam={0};
    struAlarmParam.dwSize=sizeof(struAlarmParam);
	struAlarmParam.byAlarmTypeURL=1;
	//struAlarmParam.byFaceAlarmDetection = 0;
    //其他报警布防参数不需要设置

    lHandle = NET_DVR_SetupAlarmChan_V41(user_id, & struAlarmParam);
    if (lHandle < 0)
    {
        printf("NET_DVR_SetupAlarmChan_V41 error, %d\n", NET_DVR_GetLastError());
        NET_DVR_Logout(user_id);
        NET_DVR_Cleanup(); 
        return 0;
    }
  
    sleep(50000); //等待过程中，如果设备上传报警信息，在报警回调函数里面接收和处理报警信息

    //撤销布防上传通道
    if (!NET_DVR_CloseAlarmChan_V30(lHandle))
    {
        printf("NET_DVR_CloseAlarmChan_V30 error, %d\n", NET_DVR_GetLastError());
        NET_DVR_Logout(user_id);
        NET_DVR_Cleanup(); 
        return 0;
    }
#endif





	printf("通道起始号:%d\r\n",DeviceInfo.struDeviceV30.byStartChan);
    printf("IP通道个数: %d\n", DeviceInfo.struDeviceV30.byIPChanNum + DeviceInfo.struDeviceV30.byHighDChanNum * 256);
	printf("IP通道起始号:%d\r\n",DeviceInfo.struDeviceV30.byStartDChan);
	//NET_DVR_GetPicture(user_id,(char*)"ch00001_00010000019004882483200094680",(char*)"./Image/test.jpeg");
	
/************************人脸库管理************************/
#if ADDFACE
	NET_DVR_FACELIB_COND *struInput =new NET_DVR_FACELIB_COND();
	char fdid[256]="65C01B18FE9B48FCB1866F151196D204";
	struInput->dwSize = sizeof(NET_DVR_FACELIB_COND);
	strcpy(struInput->szFDID,fdid) ;
	struInput->byConcurrent = 0;
	struInput->byCover = 1;
	struInput->byCustomFaceLibID = 0;



	NET_DVR_SEND_PARAM_IN struImageInfo =  {0};
	fstream f;
	string img;
	f.open("Image/2.jpg",ios::in|ios::binary);
	if(f)
	{
		f.seekg(0,f.end);
		unsigned int length = f.tellg();
		f.seekg(0,f.beg);
		char * buffer = new char[length];
		cout<<"length:"<<length<<endl;
		f.read(buffer,length);
		struImageInfo.pSendData=(BYTE*)buffer;
		cout<<"flag"<<endl;
		struImageInfo.dwSendDataLen = length;
		f.close();
	}
	else{
		cout<<"Open File Failed"<<endl;
	}
	cout<< "imgdata:"<<struImageInfo.pSendData<<endl;

	struImageInfo.byPicType=1;
	struImageInfo.byPicURL=0;
	struImageInfo.byRes1 = 0;
	struImageInfo.dwPicMangeNo=0;
	BYTE picname[32]="Image/2.jpg";
	memcpy(struImageInfo.sPicName,picname,sizeof(picname));

	NET_DVR_TIME_V30 uploadTime;
	uploadTime.wYear = 2021;
	uploadTime.byMonth = 5;
	uploadTime.byDay = 25;
	uploadTime.byHour = 11;
	uploadTime.byMinute = 43;
	uploadTime.bySecond = 45;
	uploadTime.byISO8601 = 0;
	uploadTime.wMilliSec = 100;
	uploadTime.cTimeDifferenceH =8;
	uploadTime.cTimeDifferenceM = 30;
	struImageInfo.struTime = uploadTime;
	cout<<"flag2"<<endl;

	//struImageInfo->pSendAppendData ;
	fstream f2;
	string test2;
	f2.open("Image/send.xml",ios::in|ios::binary);
	if(f)
	{
		f2.seekg(0,f.end);
		unsigned int length = f2.tellg();
		f2.seekg(0,f2.beg);
		char * buffer = new char[length];
		cout<<"length："<<length<<endl;
		f2.read(buffer,length);
		struImageInfo.dwSendAppendDataLen = length;
		struImageInfo.pSendAppendData=(BYTE*)buffer;
		struImageInfo.dwSendAppendDataLen = length;
		f2.close();
	}
	cout<<"flag3"<<endl;
	
	BYTE byres[192]="0";
	memcpy(struImageInfo.byRes,byres,sizeof(byres));
	
	int uploaHandle=NET_DVR_UploadFile_V40(user_id,IMPORT_DATA_TO_FACELIB,struInput,sizeof(NET_DVR_FACELIB_COND),NULL,NULL,0);
	cout<<" 1 uploadhandle error:"<<NET_DVR_GetLastError()<<endl;
	cout<<"uploadHandle:"<<uploaHandle<<endl;
	if(uploaHandle==-1){cout<<"uploadhandle error:"<<NET_DVR_GetLastError()<<endl;}
	sleep(1);
	cout<<" 2  error:"<<NET_DVR_GetLastError()<<endl;

	int status2= NET_DVR_UploadSend(uploaHandle,&struImageInfo,NULL);
	cout<<"uploadsend status:"<<status2<<endl;
	if(!status2){cout<<"uploadsend error:"<<NET_DVR_GetLastError()<<endl;}
	sleep(1);
	cout<<" 3  error:"<<NET_DVR_GetLastError()<<endl;
	LPDWORD progress = {0};
	while(1)
	{

		int status=NET_DVR_GetUploadState(uploaHandle,progress);
		cout<<"uploadState status:"<<status<<endl;
		cout<<"进度："<<progress<<endl;
		if(status==-1){
			cout<<"upload error2:"<<NET_DVR_GetLastError()<<endl;
			break;
		}
		sleep(100);
	} 
	NET_DVR_UploadClose(uploaHandle);

	
	return 0;
#endif




	res = NET_DVR_GetDVRConfig(user_id,NET_DVR_GET_COMPRESSCFG_V30,DeviceInfo.struDeviceV30.byStartChan,&device_compression,sizeof(NET_DVR_COMPRESSIONCFG_V30),&len_dword);
	if(!res)
	{
		printf("Get comppression error...\n");
		NET_DVR_Logout(user_id);
		NET_DVR_Cleanup();
		return HPR_ERROR;
	}
	printf("device_compression:%d\r\n",device_compression.struNormHighRecordPara.dwVideoBitrate);
	printf("video_bitrate:%d\r\n",device_compression.struNormHighRecordPara.dwVideoBitrate);
	
	while('q'!=channelChoose)
	{
		printf("输入通道号:");
		cin>>channelChoose;	
		ip_channel=DeviceInfo.struDeviceV30.byStartChan;
		//ip_channel=channelChoose;//-'0'+32;		
		while('q'!=funChoose)
		{
			printf("功能选择\n");
			printf("1、聚焦+\n");
			printf("2、聚焦-\n");
			printf("3、截图-\n");
			printf("      q, Quit.\n");
			printf("Input:");
			cin>>funChoose;
			switch(funChoose)
			{
				case '1':zoomin(user_id,1,1);break;
				case '2':zoomout(user_id,DeviceInfo.struDeviceV30.byStartChan,1);break;
				case '3':CapImg(user_id, ip_channel,(char*)"./Image/1.jpeg");break;
				default:break;	
			}	
		}
		funChoose='r';
	}
	
	
	



    //logout
    NET_DVR_Logout(user_id);
    NET_DVR_Cleanup();
	
	
	
	return 0;
}