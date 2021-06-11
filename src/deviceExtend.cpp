#include "deviceExtend.h"

 

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

void selectFun(Json::Value message)
{
	switch(hash_(message["header"]["namespace"].asCString()))
	{
		case "xinke.device.query"_hash:		{	selectQueryFun		(message["header"]["name"].asString(),message["payload"]);		}break;
		case "xinke.device.upload"_hash:	{	selectUploadFun		(message["header"]["name"].asString(),message["payload"]);		}break;
		case "xinke.device.control"_hash:	{	selectControlFun	(message["header"]["name"].asString(),message["payload"]);		}break;
		case "xinke.device.setconfig"_hash:	{	selectSetConfigFun	(message["header"]["name"].asString(),message["payload"]);		}break;
		default:break;
	}

}

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

void selectQueryFun(std::string name,Json::Value payload)
{

}
void selectUploadFun(std::string name,Json::Value payload)
{

}
void selectControlFun(std::string name,Json::Value payload)
{

}


int setLogInfo(Json::Value payload)
{
	Json::Value data = getConfig();
	data[0]["device_ip"] = payload["device_ip"];
	data[0]["username"] = payload["username"];
    data[0]["password"] = payload["password"];
    data[0]["port"] = payload["port"];
	data[2]["device_id"] = payload["device_id"];
	setConfig(data);
	return 1;
}

int setCapture(Json::Value payload)
{



}
int setRemoteInfo(Json::Value payload)
{



}
int setProjectInfo(Json::Value payload)
{



}

int CapImg(int channel)
{
	Json::Value message;
	Json::StyledWriter writer;
	NET_DVR_JPEGPARA strPicPara = {0};
    strPicPara.wPicQuality = 0;
    strPicPara.wPicSize =0;
	char buffer[BUFFERSIZE];
	unsigned int len;
	LPDWORD size = &len;
    bool iRet = NET_DVR_CaptureJPEGPicture_NEW(device_info.user_id,channel, &strPicPara, buffer,BUFFERSIZE,size);
	if (!iRet)
    {
			printf("\nhere2!\n");

        printf("pyd1---NET_DVR_CaptureJPEGPicture error, %d\n", NET_DVR_GetLastError());
        return HPR_ERROR;
    }
	std::cout<<"len:"<<len<<std::endl;
	std::cout<<sizeof(LPDWORD)<<std::endl;
	printf("\nhere2!\n");
	std::string imgdata = base64_encode(buffer,len);
	std::string ts ;
	ts = base64_decode(imgdata);
	std::ofstream h;
	h.open("./Image/now.jpeg");
	h<<ts;
	h.close();
	message["data"] = getLocalTime();
	message["image"] = imgdata;
	request.message = writer.write(message);
	request.publish();
	return 0;
}

/*
void ZoomInWithSpeed(int lUserID,int channel,int sleep_time)
{
	NET_DVR_PTZControlWithSpeed_Other
}
*/
void zoomin(int lUserID,int channel,int sleep_time)
{
	NET_DVR_PTZControl_Other(lUserID,channel,ZOOM_IN,0);
	usleep(sleep_time);
	NET_DVR_PTZControl_Other(lUserID,channel,ZOOM_IN,1);
}

void zoomout(int lUserID,int channel,int sleep_time)
{
	NET_DVR_PTZControl_Other(lUserID,channel,ZOOM_OUT,0);
	usleep(sleep_time);
	NET_DVR_PTZControl_Other(lUserID,channel,ZOOM_OUT,1);
}
