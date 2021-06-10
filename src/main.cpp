#include <iostream>
#include <fstream>
#include <stdbool.h> 
#include <thread>
#include <jsoncpp/json/json.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "Common.h"
#include "HCNetSDK.h"
#include "http_libcurl.h"
#include "base64.h"
#include "alarm.h"
#include "facelib.h"
#include "background.h"
#include <amqp.h>
#include <amqp_framing.h>
#include "rabbitmq.h"
#include "deviceExtend.h"



using namespace std;
amqp_connection_state_t state;
DEVICE_INFO device_info;


int main()
 {
	
	Json::Value config = getConfig();
	long user_id = NVR_Init(config[0]);
	device_info.device_id=config[2]["device_id"].asString();
	if(user_id < 0)
	{
		printf("\n[ERROR]设备初始化失败！\n");
		return 0;
	}
	get_FDLib_capabilities(user_id);
	CapImg(user_id,33);

	//创建报警布防线程与网络通信线程
	//thread alarm(thread_ALARM,user_id);
	thread ramq(thread_RAMQ,user_id);
	//alarm.join();
	ramq.join();

    //logout
	NET_DVR_Logout(user_id);
	NET_DVR_Cleanup();
	return 0;
}