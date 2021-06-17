#include "deviceExtend.h"

 /**
  * @brief 哈希函数，用来把字符串转成常数的(来自网络)
  */
hash_t hash_( char  const * str)         
{         
     hash_t ret{basis};      
     while (*str){         
         ret ^= *str;         
         ret *= prime;      
         str++;        
     }    
     return  ret;      
}
constexpr hash_t hash_compile_time( char  const * str, hash_t last_value )       
{         
     return  *str ? hash_compile_time(str+1, (*str ^ last_value) * prime) : last_value;        
}

constexpr unsigned  long  long  operator  ""  _hash( char  const * p,  size_t )        
{   
     return  hash_compile_time(p);      
}


std::string messageid;



/**选择命令执行函数**/

/**
 * @brief  	命令类型选择
 * @author 	liuguang
 * @date 	2021/06/16
 * @param   message:消息类型，查询、控制、设置
 * @return 	[NULL]
 */
void selectFun(Json::Value message)
{
	messageid = message["header"]["messageId"].asString();
	switch(hash_(message["header"]["namespace"].asCString()))
	{
		case "xinke.device.query"_hash:		{	selectQueryFun		(message["header"]["name"].asString(),message["payload"]);		}break;
		// case "xinke.device.upload"_hash:	{	selectUploadFun		(message["header"]["name"].asString(),message["payload"]);		}break;
		case "xinke.device.control"_hash:	{	selectControlFun	(message["header"]["name"].asString(),message["payload"]);		}break;
		case "xinke.device.setconfig"_hash:	{	selectSetConfigFun	(message["header"]["name"].asString(),message["payload"]);		}break;
		default:break;
	}

}


/**
 * @brief  	设置命令选择
 * @author 	liuguang
 * @date 	2021/06/16
 * @param   name:命令名称
 * @param   payload:参数
 * @return 	[NULL]
 */
void selectSetConfigFun(std::string name,Json::Value payload)
{
	switch(hash_(name.c_str()))
	{
		case "device.loginfo"_hash:		{	setLogInfo		(payload);		}break;
		case "device.capture"_hash:		{	setCapture		(payload);		}break;
		case "device.remoteinfo"_hash:	{	setRemoteInfo	(payload);		}break;
		case "device.projectinfo"_hash:	{	setProjectInfo	(payload);		}break;
		
		default:break;
	}

}


/**
 * @brief  	查询命令选择
 * @author 	liuguang
 * @date 	2021/06/16
 * @param   name:命令名称
 * @param   payload:参数
 * @return 	[NULL]
 */
void selectQueryFun(std::string name,Json::Value payload)
{
	int status = 0 ;
	switch(hash_(name.c_str()))
	{
		case "device.loginfo"_hash:		{	status = returnQueryInfo(QUERY_LOG);	}break;
		case "device.capture"_hash:		{	status = returnQueryInfo(QUERY_CAPTURE);}break;
		case "device.remoteinfo"_hash:	{	status = returnQueryInfo(QUERY_REMOTE);	}break;
		case "device.projectinfo"_hash:	{	status = returnQueryInfo(QUERY_PROJECT);}break;
		case "device.logstatus"_hash:	{	status = returnLogStatusInfo();			}break;
		case "device.facelibinfo"_hash:	{	status = returnFacelibInfo();			}break;
		case "device.workerinfo"_hash:	{	status = returnWorkerInfo();			}break;
		default:break;
	}
}

/**
 * @brief  	控制命令选择
 * @author 	liuguang
 * @date 	2021/06/16
 * @param   name:命令名称
 * @param   payload:参数
 * @return 	[NULL]
 */
void selectControlFun(std::string name,Json::Value payload)
{
	int status = 0 ;
	switch(hash_(name.c_str()))
	{
		case "device.addface"_hash:		{	status = returnQueryInfo(QUERY_LOG);	}break;
		case "device.capture"_hash:		{	status = returnQueryInfo(QUERY_CAPTURE);}break;
		case "device.ptzcontrol"_hash:	{	status = returnQueryInfo(QUERY_REMOTE);	}break;
		default:break;
	}

}

/**设置设备参数**/

/**
 * @brief  	设置登录信息
 * @author 	liuguang
 * @date 	2021/06/16
 * @param   payload：参数信息
 * @return 	0为失败，1为成功
 */
int setLogInfo(Json::Value payload)
{
	Json::Value data = getConfig();
    data[0]["port"] 	 = payload["port"]		;
	data[0]["username"]  = payload["username"]	;
    data[0]["password"]  = payload["password"]	;
	data[0]["device_ip"] = payload["device_ip"]	;
	data[2]["device_id"] = payload["device_id"]	;
	setConfig(data);
	return 1;
}

/**
 * @brief  	设置定时截图信息
 * @author 	liuguang
 * @date 	2021/06/16
 * @param   payload：参数信息
 * @return 	0为失败，1为成功
 */
int setCapture(Json::Value payload)
{
	//感觉会很麻烦
	return DE_OK;

}

/**
 * @brief  	设置RAMQ信息
 * @author 	liuguang
 * @date 	2021/06/16
 * @param   payload：参数信息
 * @return 	0为失败，1为成功
 */
int setRemoteInfo(Json::Value payload)
{
	Json::Value data = getConfig();
	data[1]["key"] 			= payload["key"]			;
	data[1]["vhost"] 		= payload["vhost"]			;
	data[1]["exchange"] 	= payload["exchange"]		;
	data[1]["password"] 	= payload["password"]		;
	data[1]["username"] 	= payload["username"]		;
	data[1]["recv_queue"] 	= payload["recv_queue"]		;
	data[1]["server_url"] 	= payload["server_url"]		;
	data[1]["channel_max"] 	= payload["channel_max"]	;
	data[1]["server_port"] 	= payload["server_port"]	;
	data[1]["upload_queue"] = payload["upload_queue"]	;
	setConfig(data);
	return 1;
}

/**
 * @brief  	设置项目信息
 * @author 	liuguang
 * @date 	2021/06/16
 * @param   payload：参数信息
 * @return 	0为失败，1为成功
 */
int setProjectInfo(Json::Value payload)
{
	Json::Value data = getConfig();
	data[2]["项目"] 	= payload["项目"];
	data[2]["标段"] 	= payload["标段"];
	data[0]["facelib"]	= payload["facelib"];
	setConfig(data);
	return 1;

}







/**设备控制函数**/

/**
 * @brief  	设备截图函数
 * @author 	liuguang
 * @date 	2021/06/16
 * @param   [in] channel: 通道号
 * @return 	0为失败，1为成功
 */
int CapImg(int channel)
{
	RAMQ request;
	Json::Value config,message;
	Json::StyledWriter writer;
	config = getConfig();
	request.setconfig(config[1]);
	NET_DVR_JPEGPARA strPicPara = {0};
    strPicPara.wPicQuality = 0;
    strPicPara.wPicSize =0;
	char buffer[BUFFERSIZE];
	unsigned int len;
	LPDWORD size = &len;
    bool iRet = NET_DVR_CaptureJPEGPicture_NEW(device_info.user_id,channel, &strPicPara, buffer,BUFFERSIZE,size);
	if (!iRet)
    {
        printf("pyd1---NET_DVR_CaptureJPEGPicture error, %d\n", NET_DVR_GetLastError());
        return HPR_ERROR;
    }
	std::string imgdata = base64_encode(buffer,len);
	// std::string ts ;
	// ts = base64_decode(imgdata);
	// std::ofstream h;
	// h.open("./Image/now.jpeg");
	// h<<ts;
	// h.close();
	message["data"] = getLocalTime();
	message["image"] = imgdata;
	request.message = writer.write(message);
	request.publish();
	return 0;
}

/**
 * @brief 	云台控制函数
 * @author 	liuguang
 * @date 	2021/06/16
 * @return 	[int] DE_ERROR为失败，DE_OK为成功
 * @param	[in] channel :通道号
 * @param 	[in] sleep_time :命令持续时间
 * @param 	[in] cmd :云台控制命令 
 * @param LIGHT_PWRON 2 接通灯光电源 
 * @param WIPER_PWRON 3 接通雨刷开关 
 * @param FAN_PWRON 4 接通风扇开关 
 * @param HEATER_PWRON 5 接通加热器开关 
 * @param AUX_PWRON1 6 接通辅助设备开关 
 * @param AUX_PWRON2 7 接通辅助设备开关 
 * @param ZOOM_IN 11 焦距变大(倍率变大) 
 * @param ZOOM_OUT 12 焦距变小(倍率变小) 
 * @param FOCUS_NEAR 13 焦点前调 
 * @param FOCUS_FAR 14 焦点后调 
 * @param IRIS_OPEN 15 光圈扩大 
 * @param IRIS_CLOSE 16 光圈缩小 
 * @param TILT_UP 21 云台上仰 
 * @param TILT_DOWN 22 云台下俯 
 * @param PAN_LEFT 23 云台左转 
 * @param PAN_RIGHT 24 云台右转 
 * @param UP_LEFT 25 云台上仰和左转 
 * @param UP_RIGHT 26 云台上仰和右转 
 * @param DOWN_LEFT 27 云台下俯和左转 
 * @param DOWN_RIGHT 28 云台下俯和右转 
 * @param PAN_AUTO 29 云台左右自动扫描 
 * @param TILT_DOWN_ZOOM_IN  58 云台下俯和焦距变大(倍率变大) 
 * @param TILT_DOWN_ZOOM_OUT 59 云台下俯和焦距变小(倍率变小) 
 * @param PAN_LEFT_ZOOM_IN 60 云台左转和焦距变大(倍率变大) 
 * @param PAN_LEFT_ZOOM_OUT 61 云台左转和焦距变小(倍率变小) 
 * @param PAN_RIGHT_ZOOM_IN 62 云台右转和焦距变大(倍率变大) 
 * @param PAN_RIGHT_ZOOM_OUT 63 云台右转和焦距变小(倍率变小) 
 * @param UP_LEFT_ZOOM_IN 64 云台上仰和左转和焦距变大(倍率变大) 
 * @param UP_LEFT_ZOOM_OUT 65 云台上仰和左转和焦距变小(倍率变小) 
 * @param UP_RIGHT_ZOOM_IN 66 云台上仰和右转和焦距变大(倍率变大) 
 * @param UP_RIGHT_ZOOM_OUT 67 云台上仰和右转和焦距变小(倍率变小) 
 * @param DOWN_LEFT_ZOOM_IN 68 云台下俯和左转和焦距变大(倍率变大) 
 * @param DOWN_LEFT_ZOOM_OUT 69 云台下俯和左转和焦距变小(倍率变小) 
 * @param DOWN_RIGHT_ZOOM_IN  70 云台下俯和右转和焦距变大(倍率变大) 
 * @param  71 云台下俯和右转和焦距变小(倍率变小) 
 * @param TILT_UP_ZOOM_IN 72 云台上仰和焦距变大(倍率变大) 
 * @param TILT_UP_ZOOM_OUT 73 云台上仰和焦距变小(倍率变小) 
 */
int ptzControl(int channel,int cmd,int sleep_time)
{
	NET_DVR_PTZControl_Other(device_info.user_id,channel,cmd,0);
	usleep(sleep_time);
	NET_DVR_PTZControl_Other(device_info.user_id,channel,cmd,1);
	return DE_OK;

}



/**返回命令执行结果**/

/**
 * @brief   返回参数设置命令执行结果
 * @author  liuguang
 * @date    2021/06/16
 * @param   [in] name ：上传信息类型
 * @param   [in] data ：上传信息内容
 * @return  [int] 0为失败，1为成功
 */
int returnResult(std::string name , int result)
{
	RAMQ request;
    Json::Value config = getConfig();
    request.setconfig(config[1]);

    Json::Value message;
    Json::Value header;
    Json::Value payload;
    Json::StyledWriter writer;
    header["name"] = name;
    header["messageId"] = messageid;
    header["namespace"] = "xinke.device.setconfig";
    payload["timestamp"] = getLocalTime();
	switch(result)
	{
		case DE_ERROR	:payload["result"] = "error"	;break;
		case DE_OK		:payload["result"] = "success"	;break;
		default:break;
	}
    message["header"]=header;
    message["payload"]=payload;
    std::string message_data = writer.write(message);
    request.publish(message_data);
	return DE_OK;

}


/**
 * @brief  	返回查询信息
 * @author 	liuguang
 * @date 	2021/06/16
 * @param   type:查询类型
 * @return 	0为失败，1为成功
 */
int returnQueryInfo(int type)
{
	int status = 0;

	Json::Value data = getConfig();
	RAMQ request;
	request.setconfig(data[1]);

	Json::Value message;
    Json::Value header;
    Json::Value payload;
    Json::StyledWriter writer;

	switch(type)
	{
		case QUERY_LOG		:{header["name"] = "device.loginfo"		;payload = data[0];};break;
		case QUERY_REMOTE	:{header["name"] = "device.remoteinfo"	;payload = data[1];};break;
		case QUERY_PROJECT	:{header["name"] = "device.projectinfo"	;payload = data[2];};break;
		case QUERY_CAPTURE	:{header["name"] = "device.captureinfo"	;payload = data[3];};break;
		default:break;
	}

    header["messageId"] = messageid;
    header["namespace"] = "xinke.device.setconfig";
    payload["timestamp"] = getLocalTime();
	payload["result"] = "success";
    message["header"]=header;
    message["payload"]=payload;
    std::string message_data = writer.write(message);
    request.publish(message_data);
	
	
	return status;
}


int returnLogStatusInfo()
{



	return DE_OK;
}

int returnFacelibInfo()
{



	return DE_OK;

}

int returnWorkerInfo()
{



	return DE_OK;

}

int returnAddface()
{




	return DE_OK;

}

int returnCapture()
{




	return DE_OK;

}


int returnPtz()
{




	return DE_OK;

}