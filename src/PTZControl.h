#ifndef __PTZCONNTROL_H
#define __PTZCONNTROL_H	 

#include <iostream>
#include <string>
#include <stdio.h>
#include "HCNetSDK.h"


void zoomin(int lUserID,int channel,int sleep_time=10000);
void zoomout(int lUserID,int channel,int sleep_time=10000);



#endif