#include "background.h"
#include <unistd.h>
#include <jsoncpp/json/json.h>



int thread_ALARM(long user_id)
{
    printf("进入报警布防线程...\n");
    //设置报警回调函数
    NET_DVR_SetDVRMessageCallBack_V31(MessageCallback, NULL);
    //启用布防
    LONG lHandle;
    NET_DVR_SETUPALARM_PARAM  struAlarmParam={0};
    struAlarmParam.dwSize=sizeof(struAlarmParam);
	struAlarmParam.byAlarmTypeURL=0;
	//struAlarmParam.byFaceAlarmDetection = 0;
    //其他报警布防参数不需要设置

    lHandle = NET_DVR_SetupAlarmChan_V41(user_id, & struAlarmParam);
    if (lHandle < 0)
    {
        printf("NET_DVR_SetupAlarmChan_V41 error, %d\n", NET_DVR_GetLastError());
        return 0;
    }
    printf("[SUCCESS]布防成功，开始监听设备报警数据\n");
    while(true)
    {
        sleep(50000); //等待过程中，如果设备上传报警信息，在报警回调函数里面接收和处理报警信息
        //撤销布防上传通道
        if (!NET_DVR_CloseAlarmChan_V30(lHandle))
        {
            printf("NET_DVR_CloseAlarmChan_V30 error, %d\n", NET_DVR_GetLastError());
            return 0;
        }
    }
}


int thread_WEB(long user_id)
{
    printf("\n建立网络连接通道...\n");
    Json::Reader jsreader;
	Json::Value jsObject;
	http_init();
	HLC request;
	request.url="https://www.liuguang.space:10086/test";
	request.data="hell world!";
    while(true)
    {
        request.post();
        printf("\nrequest.response:\n");
        std::cout<<request.response<<std::endl;
        
        if(!jsreader.parse(request.response, jsObject, false))
        {
            std::cout<<"read json error!"<<std::endl;
            break;
        }
        sleep(10);
        request.response.clear();

    }
	printf("\n通信线程异常关闭\n");
    return 0;
	//std::cout<<"xml:"<<std::endl<<get_facelib_appenddata(jsObject)->content<<std::endl;





}