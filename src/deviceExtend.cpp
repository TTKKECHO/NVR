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


//消息id
static std::string messageid;



/**选择命令执行函数**/

/**
 * @brief  	命令类型选择
 * @author 	liuguang
 * @date 	2021/06/16
 * @param   message:消息类型，查询、控制、设置
 * @return 	[int] DE_ERROR为失败,DE_OK为成功
 */
int selectFun(Json::Value message)
{
    int status = DE_ERROR;
	messageid = message["header"]["messageId"].asString();      //获取消息id，写入静态全局变量
    //命令类型选择
	switch(hash_(message["header"]["namespace"].asCString()))
	{
		case "xinke.device.query"_hash:		{	status = selectQueryFun		(message["header"]["name"].asString(),message["payload"]);		}break;
		case "xinke.device.control"_hash:	{	status = selectControlFun	(message["header"]["name"].asString(),message["payload"]);		}break;
		case "xinke.device.setconfig"_hash:	{	status = selectSetConfigFun	(message["header"]["name"].asString(),message["payload"]);		}break;
		default:break;
	}
    return status;
}


/**
 * @brief  	设置命令选择
 * @author 	liuguang
 * @date 	2021/06/16
 * @param   name:命令名称
 * @param   payload:参数
 * @return 	[int] DE_ERROR为失败,DE_OK为成功
 */
int selectSetConfigFun(std::string name,Json::Value payload)
{
    int status = DE_ERROR;
	switch(hash_(name.c_str()))
	{
		case "device.loginfo"_hash:		{	status = setLogInfo		(payload);		}break;
		case "device.remoteinfo"_hash:	{	status = setRemoteInfo	(payload);		}break;
		case "device.projectinfo"_hash:	{	status = setProjectInfo	(payload);		}break;
		//case "device.capture"_hash:		{	setCapture		(payload);		}break;
		
		default:break;
	}
    return status;
}


/**
 * @brief  	查询命令选择
 * @author 	liuguang
 * @date 	2021/06/16
 * @param   name:命令名称
 * @param   payload:参数
 * @return 	[int] DE_ERROR为失败,DE_OK为成功
 */
int selectQueryFun(std::string name,Json::Value payload)
{
	int status = 0 ;
	switch(hash_(name.c_str()))
	{
		case "device.loginfo"_hash:		    {	status = returnQueryInfo(QUERY_LOG);	    }break;
		// case "device.capture"_hash:		    {	status = returnQueryInfo(QUERY_CAPTURE);    }break;
		case "device.remoteinfo"_hash:	    {	status = returnQueryInfo(QUERY_REMOTE);	    }break;
		case "device.projectinfo"_hash:	    {	status = returnQueryInfo(QUERY_PROJECT);    }break;
		case "device.logstatus"_hash:	    {	status = returnLogStatusInfo();			    }break;
		case "device.facelibinfo"_hash:	    {	status = returnFacelibInfo();			    }break;
		case "device.workerinfo"_hash:	    {	status = returnWorkerInfo(payload);		    }break;
		case "device.workerlistinfo"_hash:	{	status = returnWorkerListInfo(payload);		}break;
		default:break;
	}
    return status;
}

/**
 * @brief  	控制命令选择
 * @author 	liuguang
 * @date 	2021/06/16
 * @param   name:命令名称
 * @param   payload:参数
 * @return 	[int] DE_ERROR为失败,DE_OK为成功
 */
int selectControlFun(std::string name,Json::Value payload)
{
	int status = 0 ;
	switch(hash_(name.c_str()))
	{
		case "device.addface"_hash:		    {	status = addFace(payload);	}break;
		case "device.capture"_hash:		    {	status = CapImg(std::atoi(payload["channel"].asCString()));}break;
		case "device.ptzcontrol"_hash:	    {	status = returnPtzCtrl(payload);	        }break;
		case "device.deleteface"_hash:	    {	status = deleteFace(payload);	            }break;
		case "device.addfacelib"_hash:	    {	status = returnAddFacelib(payload);	        }break;
		case "device.deletefacelib"_hash:	{	status = returnDeleteFacelib(payload);	    }break;
		default:break;
	}
    return status;
}

//设置设备参数

/**
 * @brief  	设置登录信息
 * @author 	liuguang
 * @date 	2021/06/16
 * @param   payload：参数信息
 * @return 	0为失败，1为成功
 */
int setLogInfo(Json::Value payload)
{
	int status = DE_ERROR;
	Json::Value data = getConfig();
    data[0]["port"] 	 = payload["port"]		;
	data[0]["username"]  = payload["username"]	;
    data[0]["password"]  = payload["password"]	;
	data[0]["device_ip"] = payload["device_ip"]	;

	status = setConfig(data);
	returnResult("device.loginfo",status);
	return status;
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
	int status = DE_ERROR;
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
	status = setConfig(data);
	returnResult("device.loginfo",status);
	return status;
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
	int status = DE_ERROR;
	Json::Value data = getConfig();
	data[2]["project"] 	= payload["project"];
	data[2]["section"] 	= payload["section"];
	data[2]["facelib"]	= payload["facelib"];
    data[2]["device_id"]= payload["device_id"];
	data[2]["facelib_fdid"]	= payload["facelib_fdid"];
	status = setConfig(data);
	returnResult("device.loginfo",status);
	return status;
}


//设备控制函数

/**
 * @brief  	设备截图函数
 * @author 	liuguang
 * @date 	2021/06/16
 * @param   [in] channel: 通道号
 * @return 	0为失败，1为成功
 */
int CapImg(int channel)
{
	int status = DE_ERROR;
	Json::Value config,message,header,payload;
	config = getConfig();
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
		payload["result"] = "ERROR";

    }
	else{
		status = DE_OK;
		payload["result"] = "success";

	}
	
	std::string imgdata = base64_encode(buffer,len);
	header["namespace"] = "xinke.device.upload";
	header["name"] = "device.capture";
	header["messageId"] = messageid;
	
	payload["image"] = imgdata;
	payload["device_id"] = config[2]["device_id"];
	payload["channel"] = std::to_string(channel);
	payload["timestamp"] = getLocalTime();
	message["header"] = header;
	message["payload"] = payload;

	status = returnRAMQ(message);
	return status;
}


/**
 * @brief  	设备主动上传截图函数
 * @author 	liuguang
 * @date 	2021/06/18
 * @param   [in] channel: 通道号
 * @param   [in] quality: 图片质量
 * @param   [in] picsize: 图片大小
 * @return 	DE_ERROR为失败，DE_OK为成功
 */
int uploadCapImg(int channel,int quality,int picsize)
{
	int status = DE_ERROR;
	RAMQ request;
	Json::Value config,message,header,payload;
	Json::StyledWriter writer;
	config = getConfig();
	request.setconfig(config[1]);
	NET_DVR_JPEGPARA strPicPara = {0};
    strPicPara.wPicQuality = quality;
    strPicPara.wPicSize = picsize;
	char buffer[BUFFERSIZE];
	unsigned int len;
	LPDWORD size = &len;
    bool iRet = NET_DVR_CaptureJPEGPicture_NEW(device_info.user_id,channel, &strPicPara, buffer,BUFFERSIZE,size);
	if (!iRet)
    {
        printf("pyd1---NET_DVR_CaptureJPEGPicture error, %d\n", NET_DVR_GetLastError());
        return DE_ERROR;
    }
	std::string imgdata = base64_encode(buffer,len);
	header["namespace"] = "xinke.device.upload";
	header["name"] = "device.capture";
	header["messageId"] = getLocalTime();
	
	
	payload["image"] = imgdata;
	payload["device_id"] = config[2]["device_id"];
	payload["channel"] = std::to_string(channel);
    payload["timestamp"] = getLocalTime();
	message["header"] = header;
	message["payload"] = payload;

	std::string data = request.message = writer.write(message);
	status = request.publish(data);
	return status;
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
	int status =DE_ERROR;
	status = NET_DVR_PTZControl_Other(device_info.user_id,channel,cmd,0);
	if(!status) return status;
	usleep(sleep_time);
	status = NET_DVR_PTZControl_Other(device_info.user_id,channel,cmd,1);
	return status;
}

/**
 * @brief  	人脸库增加人脸
 * @author 	liuguang
 * @date 	2021/06/18
 * @param   [in] data：人脸数据
 * @return 	DE_ERROR为失败，DE_OK为成功
 */
int addFace(Json::Value data)
{
    std::string PID;
    Json::Value message,header,payload,worker;
    header["name"] = "device.addface";
    header["messageId"] = messageid;
    header["namespace"] = "xinke.device.control";
    payload["device_id"] = data["device_id"];
    payload["facelib_fdid"] = data["facelib_fdid"];
    // payload["facelib"] = device_info.facelib;
    // payload["device_id"] = device_info.device_id;

    int size = data["data"].size();
    int status = DE_ERROR;

    for (int i = 0;i<size;i++)
    {
        status = addWorkerInfo(data["facelib_fdid"].asString(),data["data"][i],PID);
        worker["name"] = data["data"][i]["name"].asString();
        worker["PID"] = PID;
        if(status == DE_OK)
        {
            payload["success"].append(worker);
        }
        else
        {
            payload["error"].append(worker);
        }
    }
    payload["timestamp"] = getLocalTime();
    message["header"] = header;
    message["payload"] = payload;
    status = returnRAMQ(message);
    return status;
}


/**
 * @brief  	人脸库删除人脸
 * @author 	liuguang
 * @date 	2021/06/23
 * @param   [in] data：人脸数据
 * @return 	DE_ERROR为失败，DE_OK为成功
 */
int deleteFace(Json::Value data)
{
    std::string PID;
    Json::Value message,header,payload;
    header["name"] = "device.deleteface";
    header["messageId"] = messageid;
    header["namespace"] = "xinke.device.control";
    payload["device_id"] = data["device_id"];
    payload["facelib_fdid"] = data["facelib_fdid"];
    // payload["facelib"] = device_info.facelib;
    // payload["device_id"] = device_info.device_id;

    int size = data["data"].size();
    int status = DE_ERROR;
    for (int i = 0;i<size;i++)
    {
        status = deleteWorkerInfo(data["facelib_fdid"].asString(),data["data"][i].asString());
        if(status == DE_OK)
        {
            payload["success"].append(data["data"][i]);
        }
        else
        {
            payload["error"].append(data["data"][i]);
        }
    }
    payload["timestamp"] = getLocalTime();
    message["header"] = header;
    message["payload"] = payload;
    status = returnRAMQ(message);
    return status;

}


/**
 * @brief  	增加人脸库
 * @author 	liuguang
 * @date 	2021/06/23
 * @param   [in] name：人脸库名称
 * @param   [in] thresholdValue:检测阈值,阈值越大检测准确率越低, 范围[0,100]
 * @param   [out] FDID：添加成功后返回人脸库FDID
 * @return 	DE_ERROR为失败，DE_OK为成功
 */
int addFacelib(std::string name,std::string thresholdValue,std::string &FDID)
{
    int status = DE_ERROR;
    char lpInBuffer[1024] = {'\0'};
    int urlsize = snprintf(lpInBuffer, 1024,"<CreateFDLibList version=\"2.0\" xmlns=\"http://www.isapi.org/ver20/XMLSchema\"><CreateFDLib><id></id><name>%s</name><thresholdValue>%s</thresholdValue><customInfo></customInfo><customFaceLibID></customFaceLibID></CreateFDLib></CreateFDLibList>",name.c_str(),thresholdValue.c_str());
    printf("lpInBuffer:%d,%d\n%s\n", urlsize,(int)strlen(lpInBuffer),lpInBuffer);
 
    //新增人脸库
    ///*
    NET_DVR_XML_CONFIG_INPUT lpInputParam = {0};
    lpInputParam.dwSize = sizeof(lpInputParam);    
    char requestUrl[] = "POST /ISAPI/Intelligent/FDLib";
    lpInputParam.lpRequestUrl = requestUrl;
    lpInputParam.dwRequestUrlLen = strlen(requestUrl);
    lpInputParam.lpInBuffer = lpInBuffer;
    lpInputParam.dwInBufferSize = strlen(lpInBuffer);
    lpInputParam.dwRecvTimeOut = 0;
    lpInputParam.byForceEncrpt = 0;
            
    NET_DVR_XML_CONFIG_OUTPUT lpOutputParam = {0};
    lpOutputParam.dwSize = sizeof(lpOutputParam);
    char *FDLibBaseCfgList = new char[BUFFERSIZE_WORKER];
    lpOutputParam.lpOutBuffer = FDLibBaseCfgList;   
    lpOutputParam.dwOutBufferSize = BUFFERSIZE_WORKER;
    
    
    bool ret = NET_DVR_STDXMLConfig(device_info.user_id, &lpInputParam, &lpOutputParam);
    if(!ret)
    {
        printf("[FACELIB ALARM]deviceExtend.cpp addFacelib NET_DVR_STDXMLConfig ERROR:%d\n",NET_DVR_GetLastError()); 
        delete []FDLibBaseCfgList;
		return DE_ERROR;
    }    

    std::string outbuffer((char*)lpOutputParam.lpOutBuffer);
    int head = outbuffer.find("<FDID>");
    int tail = outbuffer.find("</FDID>");
    FDID = outbuffer.substr(head+6,tail-head-6);
    // printf("dd:%s\n\n",outbuffer.c_str());
    if(FDID == "createFailed")
    {
        return DE_ERROR;
    }
    delete []FDLibBaseCfgList;
    return DE_OK;
}


/**
 * @brief  	删除人脸库
 * @author 	liuguang
 * @date 	2021/06/23
 * @param   [in] data：人脸库数据
 * @return 	DE_ERROR为失败，DE_OK为成功
 */
int deleteFacelib(std::string FDID)
{
    int status = DE_ERROR;
 
    //新增人脸库
    ///*
    NET_DVR_XML_CONFIG_INPUT lpInputParam = {0};
    lpInputParam.dwSize = sizeof(lpInputParam); 
    std::string query = "DELETE /ISAPI/Intelligent/FDLib/"+FDID;
    printf("\nFDID:%s\n",FDID.c_str());
    lpInputParam.lpRequestUrl = (char*)query.c_str();
    lpInputParam.dwRequestUrlLen = query.length();
    lpInputParam.dwRecvTimeOut = 0;
    lpInputParam.byForceEncrpt = 0;
            
    NET_DVR_XML_CONFIG_OUTPUT lpOutputParam = {0};
    lpOutputParam.dwSize = sizeof(lpOutputParam);
    char *FDLibBaseCfgList = new char[100];
    lpOutputParam.lpOutBuffer = FDLibBaseCfgList;   
    lpOutputParam.dwOutBufferSize = 100;
    bool ret = NET_DVR_STDXMLConfig(device_info.user_id, &lpInputParam, &lpOutputParam);
    if(!ret)
    {
        printf("\n[FACELIB ALARM]deviceExtend.cpp deleteFacelib NET_DVR_STDXMLConfig ERROR:%d",NET_DVR_GetLastError()); 
        delete []FDLibBaseCfgList;

		return DE_ERROR;
    }   
    delete []FDLibBaseCfgList;

    return DE_OK;
}


//返回命令执行结果


/**
 * @brief   RAMQ通用返回函数
 * @author  liuguang
 * @date    2021/06/21
 * @param   [in] data ：信息内容
 * @return  [int] 0为失败，1为成功
 */
int returnRAMQ(Json::Value data)
{
	RAMQ request;
    Json::Value config = getConfig();
    Json::StyledWriter writer;
    request.setconfig(config[1]);
	request.message= writer.write(data);
    int status = request.publish_with_id(messageid);
	return status;
}

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
    Json::Value config = getConfig();
    Json::Value message,header,payload;
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
    int status = returnRAMQ(message);
	return DE_OK;
}


/**
 * @brief   云台控制并返回结果
 * @author  liuguang
 * @date    2021/06/18
 * @param   [in] payload ：控制参数
 * @return  [int] DE_ERROR为失败，DE_OK为成功
 */
int returnPtzCtrl(Json::Value data)
{
	int channel = atoi(data["channel"].asCString());
	int cmd = atoi(data["type"].asCString());
	int sleep_time = atoi(data["duration"].asCString());
	int status = ptzControl(channel,cmd,sleep_time);

    Json::Value config = getConfig();
    Json::Value message,header,payload;
    header["name"] = "device.ptzcontrol";
    header["messageId"] = messageid;
    header["namespace"] = "xinke.device.control";
    payload["timestamp"] = getLocalTime();
	switch(status)
	{
		case DE_ERROR	:payload["result"] = "error"	;break;
		case DE_OK		:payload["result"] = "success"	;break;
		default:break;
	}
    message["header"]=header;
    message["payload"]=payload;
    status = returnRAMQ(message);
	return status;
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
	Json::Value data = getConfig();
	Json::Value message,header,payload;

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
    int status = returnRAMQ(message);
	return status;
}

/**
 * @brief  	返回查在线信息
 * @author 	liuguang
 * @date 	2021/06/16
 * @param   NULL
 * @return 	0为失败，1为成功
 */
int returnLogStatusInfo()
{
	int status = 0 ;
	Json::Value data = getConfig();
	Json::Value message,header,payload;
	header["messageId"] = messageid;
	header["name"] = "device.logstatus";
    header["namespace"] = "xinke.device.query";
	
	status = checkLogStatus();
	if(status == DE_OK)
	{payload["result"] = "success";}
	else
	{payload["result"] = "error";}
    payload["timestamp"] = getLocalTime();
    message["header"]=header;
    message["payload"]=payload;
    status = returnRAMQ(message);
	return status;
}

/**
 * @brief  	返回查询人脸库信息
 * @author 	liuguang
 * @date 	2021/06/16
 * @param   NULL
 * @return 	0为失败，1为成功
 */
int returnFacelibInfo()
{
	Json::Value message,header,payload;
	int status = DE_ERROR;
    std::map<std::string,std::string>facelib_map;

	header["messageId"] = messageid;
	header["name"] = "device.facelibinfo"	;
    header["namespace"] = "xinke.device.query";


    status = getFacelibInfo(facelib_map);
	if(status){payload["result"] = "success";}
    else 
    {
        payload["result"] = "error";
        payload["timestamp"] = getLocalTime();
        message["header"]=header;
        message["payload"]=payload;
	    status = returnRAMQ(message);
        return status;
    }

	int size = facelib_map.size();
	payload["facelib_total"] = std::to_string(size);
	for(auto it:facelib_map)
	{
        Json::Value each;
		each["name"]=it.first;
        each["fdid"]=it.second;
        payload["faceliblist"].append(each);
	}
    payload["timestamp"] = getLocalTime();
    message["header"]=header;
    message["payload"]=payload;
	status = returnRAMQ(message);
	return status;
}


/**
 * @brief  	返回查询人脸库的人员信息
 * @author 	liuguang
 * @date 	2021/06/24
 * @param   [in]data:查询条件
 * @return 	0为失败，1为成功
 */
int returnWorkerListInfo(Json::Value data)
{
    Json::Value message,header,payload;
	int status = DE_ERROR;
    std::map<std::string,std::string>worker_map;

	header["messageId"] = messageid;
	header["name"] = "device.workerlistinfo"	;
    header["namespace"] = "xinke.device.query";
    status = getWorkerList(data["facelib_fdid"].asString(),worker_map);
	if(status){payload["result"] = "success";}
    else 
    {
        payload["result"] = "error";
        payload["timestamp"] = getLocalTime();
        message["header"]=header;
        message["payload"]=payload;
	    status = returnRAMQ(message);
        return status;
    }

	int size = worker_map.size();
	payload["worker_total"] = std::to_string(size);
	for(auto it:worker_map)
	{
        Json::Value each;
		each["pid"]=it.first;
        each["name"]=it.second;
        payload["workerlist"].append(each);
	}

    payload["timestamp"] = getLocalTime();
    message["header"]=header;
    message["payload"]=payload;

	status = returnRAMQ(message);
	return status;
}

/**
 * @brief  	返回查询人员信息
 * @author 	liuguang
 * @date 	2021/06/16
 * @param   [in] data:查询条件
 * @return 	0为失败，1为成功
 */
int returnWorkerInfo(Json::Value data)
{
    Json::Value message,header,payload,workerinfo;
	int status = DE_ERROR;

	header["messageId"] = messageid;
	header["name"] = "device.workerinfo"	;
    header["namespace"] = "xinke.device.query";
    status = getWorkerInfo(data["facelib_fdid"].asString(),data["pid"].asString(),workerinfo);
	if(status){payload["result"] = "success";}
    else 
    {
        payload["result"] = "error";
        payload["timestamp"] = getLocalTime();
        message["header"]=header;
        message["payload"]=payload;
	    status = returnRAMQ(message);
        return status;
    }
    payload["workerinfo"] = workerinfo;
    payload["timestamp"] = getLocalTime();
    message["header"]=header;
    message["payload"]=payload;

	status = returnRAMQ(message);
	return status;
}


/**
 * @brief  	返回人脸库新增结果
 * @author 	liuguang
 * @date 	2021/06/16
 * @param   [in] data:人脸库数据
 * @return 	0为失败，1为成功
 */
int returnAddFacelib(Json::Value data)
{
    Json::Value message,header,payload;
    int status =DE_ERROR;
    std::string name = data["name"].asString();
    std::string thresholdValue = data["thresholdValue"].asString();
    std::string FDID;
    header["messageId"] = messageid;
	header["name"] = "device.addfacelib"	;
    header["namespace"] = "xinke.device.control";

    status = addFacelib(name,thresholdValue,FDID);
    if(status)
    {
        payload["result"] = "success";
        payload["facelib_fdid"] = FDID;
    }
    else 
    {
        payload["result"] = "error";
    }
    payload["timestamp"] = getLocalTime();
    message["header"]=header;
    message["payload"]=payload;
	status = returnRAMQ(message);
	return status;

}

/**
 * @brief  	返回人脸库删除结果
 * @author 	liuguang
 * @date 	2021/06/16
 * @param   [in] data:人脸库数据
 * @return 	[int] 0为失败，1为成功
 */
int returnDeleteFacelib(Json::Value data)
{
    Json::Value message,header,payload;
    int status =DE_ERROR;
    std::string fdid = data["facelib_fdid"].asString();
    header["messageId"] = messageid;
	header["name"] = "device.deletefacelib"	;
    header["namespace"] = "xinke.device.control";
    printf("fdid:%s",fdid.c_str());
    status = deleteFacelib(fdid);
    if(status){payload["result"] = "success";}
    else 
    {
        payload["result"] = "error";
    }
    payload["timestamp"] = getLocalTime();
    message["header"]=header;
    message["payload"]=payload;
	status = returnRAMQ(message);
	return status;
}


//异常结果上传


/**
 * @brief   异常结果上传
 * @author  liuguang
 * @date    2021/06/24
 * @param   [in] warnning ：异常警告
 * @return  [int] 0为失败，1为成功
 */
int uploadWarnning(std::string warning)
{
    Json::Value message,header,payload;
    int status =DE_ERROR;
    header["messageId"] = getLocalTime();
	header["name"] = "device.warning"	;
    header["namespace"] = "xinke.device.upload";
    payload["warning"] = warning;
    payload["timestamp"] = getLocalTime();
    
    message["header"]=header;
    message["payload"]=payload;
    RAMQ request;
    Json::Value config = getConfig();
    Json::StyledWriter writer;
    request.setconfig(config[1]);
	request.message= writer.write(message);
    status = request.publish();
	return status;
}


//其他函数

/**
 * @brief   返回文件类型
 * @author  liuguang
 * @date    2021/05/31
 * @param   [in] filepath ：文件路径
 * @return  [int] 文件类型JPG/PNG/BMP/GIF/SWF/XML
 */
int returnFileType(std::string filepath)
{
    std::string filetype;
    filetype = filepath.substr(filepath.size()-3,filepath.size());
    if(filetype == "jpg" || filetype == "JPG") return JPG;
    if(filetype == "png" || filetype == "PNG") return PNG;
    if(filetype == "bmp" || filetype == "BMP") return BMP;
    if(filetype == "gif" || filetype == "GIF") return GIF;
    if(filetype == "swf" || filetype == "SWF") return SWF;
    if(filetype == "xml" || filetype == "XML") return XML;
    return -1;
}


/**
 * @brief   查询人脸库信息
 * @author  liuguang
 * @date    2021/06/21
 * @param   [out]facelib_map：人脸库信息
 * @return  [int] DE_ERROR为失败，DE_OK为成功
 */
int getFacelibInfo(std::map<std::string,std::string> &facelib_map)
{
    NET_DVR_XML_CONFIG_INPUT lpInputParam = {0};
    lpInputParam.dwSize = sizeof(lpInputParam);    
    char requestUrl[] = "GET /ISAPI/Intelligent/FDLib";
    lpInputParam.lpRequestUrl = requestUrl;
    lpInputParam.dwRequestUrlLen = strlen(requestUrl);
    lpInputParam.lpInBuffer = NULL;
    lpInputParam.dwInBufferSize = 0;
    lpInputParam.dwRecvTimeOut = 0;
    lpInputParam.byForceEncrpt = 0;
            
    NET_DVR_XML_CONFIG_OUTPUT lpOutputParam = {0};
    lpOutputParam.dwSize = sizeof(lpOutputParam);
    char *FDLibBaseCfgList = new char[BUFFERSIZE_FACELIB] ;
    lpOutputParam.lpOutBuffer = FDLibBaseCfgList;   
    lpOutputParam.dwOutBufferSize = BUFFERSIZE_FACELIB;
    lpOutputParam.dwReturnedXMLSize = 0;
    lpOutputParam.lpStatusBuffer = NULL;
    lpOutputParam.dwStatusSize = 0;
    
    bool ret = NET_DVR_STDXMLConfig(device_info.user_id, &lpInputParam, &lpOutputParam);
    if(!ret)
    {
        printf("\n[XML ALARM]deviceExtend.cpp getFacelibInfo NET_DVR_STDXMLConfig ERROR:%d",NET_DVR_GetLastError());
    	delete []FDLibBaseCfgList;
		return DE_ERROR;
    }    

    // printf("get_FDLiblist\n%s\n", FDLibBaseCfgList); 

    FDLibBaseCfgList[lpOutputParam.dwReturnedXMLSize] = '\0';
    xmlDocPtr xml = NULL;
    xmlNodePtr xml_node = NULL,xml_subnode = NULL,name_node =NULL,fdid_node=NULL;
    xml = xmlParseMemory(FDLibBaseCfgList,lpOutputParam.dwReturnedXMLSize);

    if(!xml)
    {
        printf("\n[XML ALARM]deviceExtend.cpp getFacelibInfo xml is null");
    }
    xml_node = xmlDocGetRootElement(xml);
    if(!xml_node)
    {
        printf("\n[XML ALARM]deviceExtend.cpp getFacelibInfo xml_node is null");
    }
    if(!xmlStrcmp(xml_node->name,BAD_CAST"FDLibBaseCfgList"))
    {        
        xml_subnode = xml_node->children;
        while(xml_subnode)
        {
            if(xmlStrcmp(xml_subnode->name,BAD_CAST"FDLibBaseCfg")) {xml_subnode = xml_subnode->next;continue;}
            name_node = xml_subnode->children;
            fdid_node = xml_subnode->children;
            while(xmlStrcmp(name_node->name,BAD_CAST"name"))
            {
                name_node = name_node->next;
            }
            while(xmlStrcmp(fdid_node->name,BAD_CAST"FDID"))
            {
                fdid_node = fdid_node->next;
            }
            facelib_map[std::string((char*)xmlNodeGetContent(name_node))] = std::string((char*)xmlNodeGetContent(fdid_node));
            xml_subnode = xml_subnode->next;
        }
    }
    delete []FDLibBaseCfgList;
    return DE_OK;
} 


/**
 * @brief   查询人脸库人员信息
 * @author  liuguang
 * @date    2021/06/21
 * @param   [in] FDID:人脸库FDID
 * @param   [out] worker_map:人脸库人员列表(pid:name)
 * @return  [int] DE_ERROR为失败，DE_OK为成功
 */
int getWorkerList(std::string FDID,std::map<std::string,std::string> &worker_map)
{
    char lpInBuffer[1024] = {'\0'};
	if(FDID == "") 
	{
        printf("\n[FACELIB ALARM]deviceExtend.cpp getWorkerList FDID is NULL");
		return DE_ERROR;
	}
    worker_map.clear();
    int urlsize = snprintf(lpInBuffer, 1024, "<FDSearchDescription version=\"2.0\" xmlns=\"http://www.std-cgi.org/ver20/XMLSchema\"><searchID>%s</searchID><searchResultPosition>0</searchResultPosition><maxResults>500</maxResults><FDID>%s</FDID><startTime></startTime><endTime></endTime><name></name></FDSearchDescription>",getTimeStamp(MSEC).c_str(),FDID.c_str());
    printf("lpInBuffer:%d,%d\n%s\n", urlsize,(int)strlen(lpInBuffer),lpInBuffer);
 
    //查询人脸库
    ///*
    NET_DVR_XML_CONFIG_INPUT lpInputParam = {0};
    lpInputParam.dwSize = sizeof(lpInputParam);    
    char requestUrl[] = "POST /ISAPI/Intelligent/FDLib/FDSearch";
    lpInputParam.lpRequestUrl = requestUrl;
    lpInputParam.dwRequestUrlLen = strlen(requestUrl);
    lpInputParam.lpInBuffer = lpInBuffer;
    lpInputParam.dwInBufferSize = strlen(lpInBuffer);
    lpInputParam.dwRecvTimeOut = 0;
    lpInputParam.byForceEncrpt = 0;
            
    NET_DVR_XML_CONFIG_OUTPUT lpOutputParam = {0};
    lpOutputParam.dwSize = sizeof(lpOutputParam);
    char *FDLibBaseCfgList = new char[BUFFERSIZE_WORKER];
    lpOutputParam.lpOutBuffer = FDLibBaseCfgList;   
    lpOutputParam.dwOutBufferSize = BUFFERSIZE_WORKER;
    
    
    bool ret = NET_DVR_STDXMLConfig(device_info.user_id, &lpInputParam, &lpOutputParam);
    if(!ret)
    {
        printf("\n[FACELIB ALARM]deviceExtend.cpp getWorkerList NET_DVR_STDXMLConfig ERROR:%d",NET_DVR_GetLastError()); 
        delete []FDLibBaseCfgList;
		return DE_ERROR;
    }    

    FDLibBaseCfgList[lpOutputParam.dwReturnedXMLSize] = '\0';

    xmlDocPtr xml = NULL;
    xmlNodePtr xml_node = NULL,xml_subnode = NULL;
    xmlKeepBlanksDefault(0);
    xml = xmlParseMemory(FDLibBaseCfgList,lpOutputParam.dwReturnedXMLSize);

    if(!xml)		{printf("\n[XML ALARM]deviceExtend.cpp getWorkerList xml is null");}
    xml_node = xmlDocGetRootElement(xml);
    if(!xml_node)	{printf("\n[XML ALARM]deviceExtend.cpp getWorkerList xml_node is null");}
    xml_subnode = xml_node->children;

    while(xmlStrcmp(xml_subnode->name,BAD_CAST"totalMatches"))
    {	xml_subnode = xml_subnode->next;	}
    
	if(std::string((char*)xmlNodeGetContent(xml_subnode)) == "0")
    {
        printf("\n no matches\n");
        delete []FDLibBaseCfgList;
        return DE_OK;
    }

    xml_subnode = xml_node->children;
    while(xmlStrcmp(xml_subnode->name,BAD_CAST"MatchList"))
    {
        xml_subnode = xml_subnode->next;
    }
    xml_subnode = xml_subnode->children;
    while(xml_subnode)
    {
        if(xmlStrcmp(xml_subnode->name,BAD_CAST"MatchElement"))
        {
            xml_subnode = xml_subnode->next;
            continue;
        }
        xmlNodePtr name_node = NULL,id_node = NULL;
        name_node = xml_subnode->children;
        id_node = xml_subnode->children;
        while(xmlStrcmp(name_node->name,BAD_CAST"name"))			
		{	name_node = name_node->next;}
        while(xmlStrcmp(id_node->name,BAD_CAST"PID"))	
		{	id_node = id_node->next;	}
        worker_map[std::string((char*)xmlNodeGetContent(id_node))] = std::string((char*)xmlNodeGetContent(name_node));
        xml_subnode = xml_subnode->next;
	}
    delete []FDLibBaseCfgList;
    return DE_OK;
}

/**
 * @brief   查询人脸库指定人员信息
 * @author  liuguang
 * @date    2021/06/24
 * @param   [in] FDID：查询人脸库的FDID
 * @param   [in] ID：设备自动生成的PID 或者自定义的customHumanID(customHumanID查询失败)
 * @param   [out] workerInfo：人员信息（xml格式字符串)
 * @return  [int] DE_ERROR为失败，DE_OK为成功
 */
int getWorkerInfo(std::string FDID,std::string ID,Json::Value &workerInfo)
{
    NET_DVR_XML_CONFIG_INPUT lpInputParam = {0};
    lpInputParam.dwSize = sizeof(lpInputParam);
	std::string query;
	query = "GET /ISAPI/Intelligent/FDLib/"+FDID+"/picture/"+ID;
    lpInputParam.lpRequestUrl = (char*)query.c_str();
    lpInputParam.dwRequestUrlLen = query.length();
    lpInputParam.lpInBuffer = NULL;
    lpInputParam.dwInBufferSize = 0;
    lpInputParam.dwRecvTimeOut = 0;
    lpInputParam.byForceEncrpt = 0;

	NET_DVR_XML_CONFIG_OUTPUT lpOutputParam = {0};
	lpOutputParam.dwSize = sizeof(lpOutputParam);
    char *FDLibBaseCfgList = new char[BUFFERSIZE_WORKER];
    lpOutputParam.lpOutBuffer = FDLibBaseCfgList;   
    lpOutputParam.dwOutBufferSize = BUFFERSIZE_WORKER;
    lpOutputParam.dwReturnedXMLSize = 0;
    lpOutputParam.lpStatusBuffer = NULL;
    lpOutputParam.dwStatusSize = 0;
	bool ret = NET_DVR_STDXMLConfig(device_info.user_id, &lpInputParam, &lpOutputParam);
    if(!ret)
    {
        printf("\n[XML ALARM]deviceExtend.cpp getWorkerInfo NET_DVR_STDXMLConfig fail,ERROR:%d\n", NET_DVR_GetLastError()); 
		return DE_ERROR;
    }
    std::string temp((char*)lpOutputParam.lpOutBuffer);
    printf("outbuffer:%s\n",lpOutputParam.lpOutBuffer);



    xmlDocPtr xml = NULL;
    xmlNodePtr xml_node = NULL,xml_subnode = NULL;
    xmlKeepBlanksDefault(0);
    xml = xmlParseMemory(FDLibBaseCfgList,lpOutputParam.dwReturnedXMLSize);

    if(!xml)		{printf("\n[XML ALARM]deviceExtend.cpp getWorkerList xml is null");}
    xml_node = xmlDocGetRootElement(xml);
    if(!xml_node)	{printf("\n[XML ALARM]deviceExtend.cpp getWorkerList xml_node is null");}
    xml_subnode = xml_node->children;
    printf("subnode:%s\n",xml_subnode->name);

    while(xmlStrcmp(xml_subnode->name,BAD_CAST"bornTime"))
    {	xml_subnode = xml_subnode->next;	}
    workerInfo["bornTime"] = std::string((char*)xmlNodeGetContent(xml_subnode));

    while(xmlStrcmp(xml_subnode->name,BAD_CAST"name"))
    {   xml_subnode = xml_subnode->next;    }
    workerInfo["name"] = std::string((char*)xmlNodeGetContent(xml_subnode));

    while(xmlStrcmp(xml_subnode->name,BAD_CAST"sex"))
    {   xml_subnode = xml_subnode->next;    }
    workerInfo["sex"] = std::string((char*)xmlNodeGetContent(xml_subnode));

    while(xmlStrcmp(xml_subnode->name,BAD_CAST"province"))
    {   xml_subnode = xml_subnode->next;    }
    workerInfo["province"] = std::string((char*)xmlNodeGetContent(xml_subnode));

    while(xmlStrcmp(xml_subnode->name,BAD_CAST"city"))
    {   xml_subnode = xml_subnode->next;    }
    workerInfo["city"] = std::string((char*)xmlNodeGetContent(xml_subnode));

    while(xmlStrcmp(xml_subnode->name,BAD_CAST"certificateType"))
    {   xml_subnode = xml_subnode->next;    }
    workerInfo["certificateType"] = std::string((char*)xmlNodeGetContent(xml_subnode));

    while(xmlStrcmp(xml_subnode->name,BAD_CAST"certificateNumber"))
    {   xml_subnode = xml_subnode->next;    }
    workerInfo["certificateNumber"] = std::string((char*)xmlNodeGetContent(xml_subnode));

    delete []FDLibBaseCfgList;
    return DE_OK;
}


/**
 * @brief   查询人脸库指定人员信息
 * @author  liuguang
 * @date    2021/06/24
 * @param   [in] FDID：查询人脸库的FDID
 * @param   [in] ID：设备自动生成的PID 或者自定义的customHumanID(customHumanID查询失败)
 * @param   [out] workerinfo：人员信息（json格式,带图片)
 * @return  [int] DE_ERROR为失败，DE_OK为成功
 */
int getWorkerInfoWithPic(std::string FDID,std::string ID,Json::Value &workerinfo)
{
    char lpInBuffer[1024] = {'\0'};
	if(FDID == "") 
	{
        printf("\n[FACELIB ALARM]deviceExtend.cpp getWorkerList FDID is NULL");
		return DE_ERROR;
	}
    //worker_map.clear();
    int urlsize = snprintf(lpInBuffer, 1024, "<FDSearchDescription version=\"2.0\" xmlns=\"http://www.std-cgi.org/ver20/XMLSchema\"><searchID>%s</searchID><searchResultPosition>0</searchResultPosition><maxResults>500</maxResults><FDID>%s</FDID><PID>%s</PID><startTime></startTime><endTime></endTime><name></name></FDSearchDescription>",getTimeStamp(MSEC).c_str(),FDID.c_str(),ID.c_str());
    printf("lpInBuffer:%d,%d\n%s\n", urlsize,(int)strlen(lpInBuffer),lpInBuffer);
 
    //查询人脸库
    ///*
    NET_DVR_XML_CONFIG_INPUT lpInputParam = {0};
    lpInputParam.dwSize = sizeof(lpInputParam);    
    char requestUrl[] = "POST /ISAPI/Intelligent/FDLib/FDSearch";
    lpInputParam.lpRequestUrl = requestUrl;
    lpInputParam.dwRequestUrlLen = strlen(requestUrl);
    lpInputParam.lpInBuffer = lpInBuffer;
    lpInputParam.dwInBufferSize = strlen(lpInBuffer);
    lpInputParam.dwRecvTimeOut = 0;
    lpInputParam.byForceEncrpt = 0;
            
    NET_DVR_XML_CONFIG_OUTPUT lpOutputParam = {0};
    lpOutputParam.dwSize = sizeof(lpOutputParam);
    char *FDLibBaseCfgList = new char[BUFFERSIZE_WORKER];
    lpOutputParam.lpOutBuffer = FDLibBaseCfgList;   
    lpOutputParam.dwOutBufferSize = BUFFERSIZE_WORKER;
    
    
    bool ret = NET_DVR_STDXMLConfig(device_info.user_id, &lpInputParam, &lpOutputParam);
    if(!ret)
    {
        printf("\n[FACELIB ALARM]deviceExtend.cpp getWorkerList NET_DVR_STDXMLConfig ERROR:%d",NET_DVR_GetLastError()); 
        delete []FDLibBaseCfgList;
		return DE_ERROR;
    }    
    printf("\n[RESULT]\n%s\n",lpOutputParam.lpOutBuffer);

    FDLibBaseCfgList[lpOutputParam.dwReturnedXMLSize] = '\0';

/*
    xmlDocPtr xml = NULL;
    xmlNodePtr xml_node = NULL,xml_subnode = NULL;
    xmlKeepBlanksDefault(0);
    xml = xmlParseMemory(FDLibBaseCfgList,lpOutputParam.dwReturnedXMLSize);

    if(!xml)		{printf("\n[XML ALARM]deviceExtend.cpp getWorkerList xml is null");}
    xml_node = xmlDocGetRootElement(xml);
    if(!xml_node)	{printf("\n[XML ALARM]deviceExtend.cpp getWorkerList xml_node is null");}
    xml_subnode = xml_node->children;

    while(xmlStrcmp(xml_subnode->name,BAD_CAST"totalMatches"))
    {	xml_subnode = xml_subnode->next;	}
    
	if(std::string((char*)xmlNodeGetContent(xml_subnode)) == "0")
    {
        printf("\n no matches\n");
        delete []FDLibBaseCfgList;
        return DE_OK;
    }

    xml_subnode = xml_node->children;
    while(xmlStrcmp(xml_subnode->name,BAD_CAST"MatchList"))
    {
        xml_subnode = xml_subnode->next;
    }
    xml_subnode = xml_subnode->children;
    while(xml_subnode)
    {
        if(xmlStrcmp(xml_subnode->name,BAD_CAST"MatchElement"))
        {
            xml_subnode = xml_subnode->next;
            continue;
        }
        xmlNodePtr name_node = NULL,id_node = NULL;
        name_node = xml_subnode->children;
        id_node = xml_subnode->children;
        while(xmlStrcmp(name_node->name,BAD_CAST"name"))			
		{	name_node = name_node->next;}
        while(xmlStrcmp(id_node->name,BAD_CAST"PID"))	
		{	id_node = id_node->next;	}
        worker_map[std::string((char*)xmlNodeGetContent(id_node))] = std::string((char*)xmlNodeGetContent(name_node));
        xml_subnode = xml_subnode->next;
	}


    */
    delete []FDLibBaseCfgList;
    return DE_OK;


}


/**
 * @brief   增加人脸库人员信息
 * @author  liuguang
 * @date    2021/06/21
 * @param   [in] FDID：人脸库FDID
 * @param   [in] data：增加的人脸库人员信息
 * @param   [out] PID：人脸上传成功后返回的PID,失败返回错误ID
 * @return  [int] DE_ERROR为失败，DE_OK为成功
 */
int addWorkerInfo(std::string FDID,Json::Value data,std::string &PID)
{
    //step 1:NET_DVR_UploadFile_V40
    std::string filename;
    std::string filepath = "./Image/2.png";
    filename = filepath.substr(0,filepath.size()-4);
    printf("%s\n",filename.c_str());
    NET_DVR_FACELIB_COND struInput = {0};
	char fdid[NET_SDK_MAX_FDID_LEN];
    sprintf(fdid,"%s",FDID.c_str());
    printf("%s\n",FDID.c_str());
	struInput.dwSize = sizeof(struInput);
	memcpy(struInput.szFDID,fdid,NET_SDK_MAX_FDID_LEN);
	struInput.byConcurrent = 0;
	struInput.byCover = 1;
	struInput.byCustomFaceLibID = 0;
     
    int uploaHandle = NET_DVR_UploadFile_V40(device_info.user_id,IMPORT_DATA_TO_FACELIB,&struInput,sizeof(NET_DVR_FACELIB_COND),NULL,NULL,0);
    
    if(-1 == uploaHandle)
    {
        printf("add_face fail, error:%d.\n", NET_DVR_GetLastError());          
		return DE_ERROR;
    }    
    printf("NET_DVR_UploadFile_V40\n%d\n", uploaHandle); 
    
    
    NET_DVR_SEND_PARAM_IN   struImageInfo = { 0 };
    
    //图片格式：1- jpg，2- bmp，3- png，4- SWF，5- GIF 
    struImageInfo.byPicType = returnFileType(filepath);
    std::string img_data = base64_decode(data["image"].asString());
    long length = img_data.length();
    struImageInfo.pSendData = (BYTE*)img_data.c_str();
    struImageInfo.dwSendDataLen = length;
    printf("%s:%ld,%d\n",filepath.c_str(),length,struImageInfo.dwSendDataLen); 
    xmlBufferPtr xmlAppenddata = get_facelib_appenddata(data);  
    length = xmlStrlen(xmlAppenddata->content);
    BYTE *pSendAppendData = new BYTE[length];
    memset(pSendAppendData, 0, length);
    memcpy(pSendAppendData, xmlAppenddata->content, length);
    //printf("%s\n%d\n",xmlAppenddata->content);
    xmlBufferFree(xmlAppenddata);
    
    struImageInfo.pSendAppendData = pSendAppendData;
    struImageInfo.dwSendAppendDataLen = length;

    printf("%s:%ld,%d\n",filename.c_str(),length,struImageInfo.dwSendAppendDataLen); 
    
    if (NET_DVR_UploadSend(uploaHandle, &struImageInfo, NULL) < 0)
    {
        delete[] pSendAppendData;
        printf("NET_DVR_UploadSend fail, error:%d.\n", NET_DVR_GetLastError()); 
		return DE_ERROR;
    }
     
    DWORD dwProgress = 0;
    int iStatus = -1;
    while (true) {
        iStatus = NET_DVR_GetUploadState(uploaHandle,&dwProgress);
		        
        if (iStatus == 1) {
            printf("上传成功\n");
            // NET_DVR_GetUploadResult(m_lUploadHandle, HCNetSDK.IMPORT_DATA_TO_FACELIB, 12);
            break;
        } else if (iStatus == 2) {
            printf("正在上传:%d\n",dwProgress);
            sleep(1);
        } else if (iStatus == 29) {
            printf("图片未识别到目标\n");
            break;
        } else {
            printf("其他错误：%d\n",iStatus);            
            break;
        }       
    }
    if(iStatus != 1)
    {
        delete[] pSendAppendData;
        PID = std::to_string(iStatus);
        NET_DVR_UploadClose(uploaHandle);
        return DE_ERROR;
    }
    NET_DVR_UPLOAD_FILE_RET outBuffer = {0};
    NET_DVR_GetUploadResult(uploaHandle,&outBuffer,sizeof(NET_DVR_UPLOAD_FILE_RET));
    std::string temp_pid((char*)outBuffer.sUrl);
    PID = temp_pid;
    delete[] pSendAppendData;
    NET_DVR_UploadClose(uploaHandle);
    return DE_OK;
}



/**
 * @brief   删除人脸库人员信息
 * @author  liuguang
 * @date    2021/06/21
 * @param   [in] ID：设备自动生成的PID或者自定义的customHumanID
 * @return  [int] DE_ERROR为失败，DE_OK为成功
 */
int deleteWorkerInfo(std::string FDID,std::string ID)
{
	NET_DVR_XML_CONFIG_INPUT lpInputParam = {0};
    lpInputParam.dwSize = sizeof(lpInputParam);
	std::string query;
	query = "DELETE /ISAPI/Intelligent/FDLib/"+FDID+"/picture/"+ID;
    lpInputParam.lpRequestUrl = (char*)query.c_str();
    lpInputParam.dwRequestUrlLen = query.length();
    lpInputParam.lpInBuffer = NULL;
    lpInputParam.dwInBufferSize = 0;
    lpInputParam.dwRecvTimeOut = 0;
    lpInputParam.byForceEncrpt = 0;

	NET_DVR_XML_CONFIG_OUTPUT lpOutputParam = {0};
	lpOutputParam.dwSize = sizeof(lpOutputParam);
    lpOutputParam.dwReturnedXMLSize = 0;
    lpOutputParam.lpStatusBuffer = NULL;
    lpOutputParam.dwStatusSize = 0;
	bool ret = NET_DVR_STDXMLConfig(device_info.user_id, &lpInputParam, &lpOutputParam);
    if(!ret)
    {
        printf("\n[XML ALARM]deviceExtend.cpp NET_DVR_STDXMLConfig fail,ERROR:%d\n", NET_DVR_GetLastError()); 
		return DE_ERROR;
    }   
	return DE_OK;
}



/**
 * @brief   查询设备是否在线
 * @author  liuguang
 * @date    2021/06/21
 * @param   NULL
 * @return  [int] DE_ERROR为失败，DE_OK为成功
 */
int checkLogStatus()
{
	Json::Value config = getConfig();
	long user_id = NVR_Init(config[0]);
	if(user_id < 0)
	{
	// printf("\n\n离线\n\n");

		return DE_ERROR;
	}
	NET_DVR_Logout(user_id);
	printf("\n\n在线\n\n");
	return DE_OK;
}


/**
 * @brief   生成人脸附加xml信息
 * @author  liuguang
 * @date    2021/06/21
 * @param   [in] jsObject：json格式附加信息
 * @return  [xmlBufferPtr] 人脸附加xml信息
 */
xmlBufferPtr get_facelib_appenddata(Json::Value jsObject)
{    
    xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");  //定义文档和节点指针
    
    //创建一个root节点，设置其内容和属性，然后加入根结点
    xmlNodePtr root_node = xmlNewNode(NULL,BAD_CAST "FaceAppendData"); 
    xmlNewProp(root_node,BAD_CAST "version",BAD_CAST "2.0");    
    xmlNewProp(root_node,BAD_CAST "xmlns",BAD_CAST "http://www.hikvision.com/ver20/XMLSchema");
    
    xmlDocSetRootElement(doc,root_node);        //设置根节点
    xmlNodePtr son_node,content,grandson,great_grandson;
    
    //创建 bornTime 节点
    //<!--opt, xs:ISO8601 time,出生日期,例如:2014-12-12T00:00:00Z--> 
    son_node = xmlNewNode(NULL,BAD_CAST "bornTime");
    content = xmlNewText(BAD_CAST jsObject["bornTime"].asString().c_str());

    //content = xmlNewText(BAD_CAST "2000-01-01T00:00:00Z");
    xmlAddChild(son_node,content);
    xmlAddChild(root_node,son_node);

    //创建 name 节点
    //<!--req,xs:string,姓名--> 
    son_node = xmlNewNode(NULL,BAD_CAST "name");
    content = xmlNewText(BAD_CAST jsObject["name"].asString().c_str());
    //content = xmlNewText(BAD_CAST "赵敏");
    xmlAddChild(son_node,content);
    xmlAddChild(root_node,son_node);

    //创建 sex 节点
    //<!--opt,xs:string,"male,female",性别--> 
    son_node = xmlNewNode(NULL,BAD_CAST "sex");
    content = xmlNewText(BAD_CAST jsObject["sex"].asString().c_str());
    xmlAddChild(son_node,content);
    xmlAddChild(root_node,son_node);

    //创建 province 节点
    //<!--opt,xs:string,省份,代码索引具体请参考"全国各省份城市列表"--> 
    son_node = xmlNewNode(NULL,BAD_CAST "province");
    content = xmlNewText(BAD_CAST jsObject["province"].asString().c_str());
    xmlAddChild(son_node,content);
    xmlAddChild(root_node,son_node);
    
    //创建 city 节点
    //<!--opt,xs:string,城市,代码索引具体请参考"全国各省份城市列表"--> 
    son_node = xmlNewNode(NULL,BAD_CAST "city");
    content = xmlNewText(BAD_CAST jsObject["city"].asString().c_str());
    xmlAddChild(son_node,content);
    xmlAddChild(root_node,son_node);

    //创建 certificateType 节点
    //<!--opt,xs:string,"officerID,ID,passportID,other", 证件类型: OfficerID-军官证, ID-身份证, passportID-护照, other-其他--> 
    son_node = xmlNewNode(NULL,BAD_CAST "certificateType");
    content = xmlNewText(BAD_CAST jsObject["certificateType"].asString().c_str());
    xmlAddChild(son_node,content);
    xmlAddChild(root_node,son_node);

    //创建 certificateNumber 节点
    //<!--dep,xs:string,"证件号"--> 
    son_node = xmlNewNode(NULL,BAD_CAST "certificateNumber");
    content = xmlNewText(BAD_CAST jsObject["certificateNumber"].asString().c_str());
    xmlAddChild(son_node,content);
    xmlAddChild(root_node,son_node);
 
    //创建 phoneNumber 节点
    // <!--opt,xs:string, 电话号码-->
    son_node = xmlNewNode(NULL,BAD_CAST "phoneNumber");
    content = xmlNewText(BAD_CAST jsObject["phoneNumber"].asString().c_str());
    xmlAddChild(son_node,content);
    xmlAddChild(root_node,son_node);
    
    //创建 customHumanID 节点
    // <!--opt,xs:string,"自定义人员ID"--> 
    // son_node = xmlNewNode(NULL,BAD_CAST "customHumanID");
    // content = xmlNewText(BAD_CAST jsObject["customHumanID"].asString().c_str());
    // xmlAddChild(son_node,content);
    // xmlAddChild(root_node,son_node);
              
    xmlKeepBlanksDefault(0);
    
    xmlBufferPtr buffer = xmlBufferCreate();
    xmlNodeDump(buffer, doc, root_node, 0, 1);
       
    //xmlBufferFree(buffer); 

    //释放文档内节点动态申请的内存
    xmlFreeDoc(doc);

    return buffer;
}
