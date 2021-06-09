#include "deviceExtend.h"


int CapImg(int lUserID,int channel,char* src)
{
	NET_DVR_JPEGPARA strPicPara = {0};
    strPicPara.wPicQuality = 2;
    strPicPara.wPicSize = 0;
    int iRet;
    iRet = NET_DVR_CaptureJPEGPicture(lUserID,channel, &strPicPara, src);
	if (!iRet)
    {
		printf("Cap ERROR!!!!!!!!!!!!!!!!");
        printf("pyd1---NET_DVR_CaptureJPEGPicture error, %d\n", NET_DVR_GetLastError());
        return HPR_ERROR;
    }
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
