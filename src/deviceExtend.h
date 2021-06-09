#ifndef __DEVICEEXTEND_H
#define __CAPPI__DEVICEEXTEND_HCTURE_H	 

#include <iostream>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include "HCNetSDK.h"


int CapImg(int lUserID,int channel,char* src);
void zoomin(int lUserID,int channel,int sleep_time=10000);
void zoomout(int lUserID,int channel,int sleep_time=10000);




#endif
