#include "PTZControl.h"
#include <unistd.h>



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
