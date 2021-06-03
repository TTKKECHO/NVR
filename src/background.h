#ifndef __BACKGROUND_H
#define __BACKGROUND_H	 

#include "HCNetSDK.h"
#include "Common.h"
#include "http_libcurl.h"
#include "alarm.h"

int thread_ALARM(long user_id);
int thread_WEB(long user_id);
#endif