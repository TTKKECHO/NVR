#include <iostream>
#include <fstream>
#include <stdbool.h> 
#include <thread>
#include <jsoncpp/json/json.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
// #include <amqp.h>
// #include <amqp_framing.h>
#include "rabbitmq.h"
#include "deviceExtend.h"
#include "Common.h"
#include "HCNetSDK.h"
#include "base64.h"
#include "alarm.h"
#include "facelib.h"
#include "background.h"
// #include "http_libcurl.h"



using namespace std;
DEVICE_INFO device_info;



int main()
 {
	//获取配置文件
	Json::Value config = getConfig();
	//注册超脑设备
	long user_id = NVR_Init(config[0]);
	//读取配置文件中设备编号
	device_info.device_id=config[2]["device_id"].asString();
	//开启rabbitmq后台监听线程
	thread ramq(thread_RAMQ);
	//检查超脑设备是否注册成功
	while(user_id < 0)			//小于0说明注册失败
	{
		//上传离线报警信息
		uploadWarnning("offline");
		printf("\n[ERROR]设备初始化失败！\n");
		//等待30s重新注册设备
		sleep(30);
		config = getConfig();
		user_id = NVR_Init(config[0]);
	}
	//获取用户ID号
	device_info.user_id = user_id;

	//创建报警布防线程
	thread alarm(thread_ALARM,user_id);

	//等待报警、通信线程结束
	alarm.join();
	ramq.join();

    //logout
	NET_DVR_Logout(user_id);
	NET_DVR_Cleanup();
	return 0;
}