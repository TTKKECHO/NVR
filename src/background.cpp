#include "background.h"
#include "deviceExtend.h"
#include <unistd.h>
#include <string>
#include <jsoncpp/json/json.h>



// static std::string messageid;

/**
 * @brief   设备报警处理函数
 * @author  liuguang
 * @date    2021/05/27 
 * @param   [in] user_id: 设备登录ID
 * @return  [int] 0为失败
 */
int thread_ALARM(long user_id)
{
    printf("进入报警布防线程...\n");
    //设置报警回调函数
    NET_DVR_SetDVRMessageCallBack_V31(MessageCallback, NULL);
    //设置报警参数
    LONG lHandle;
    NET_DVR_SETUPALARM_PARAM  struAlarmParam={0};
    struAlarmParam.dwSize=sizeof(struAlarmParam);
	struAlarmParam.byAlarmTypeURL=0<<3;
	// struAlarmParam.byAlarmTypeURL=0;
	//struAlarmParam.byFaceAlarmDetection = 0;
    //其他报警布防参数不需要设置

    //开始布防
    lHandle = NET_DVR_SetupAlarmChan_V41(user_id, & struAlarmParam);
    if (lHandle < 0)
    {
        printf("NET_DVR_SetupAlarmChan_V41 error, %d\n", NET_DVR_GetLastError());
        return 0;
    }
    printf("[SUCCESS]布防成功，开始监听设备报警数据\n");
    while(true)
    {
        sleep(86400); //等待过程中，如果设备上传报警信息，在报警回调函数里面接收和处理报警信息
    }
    //撤销布防上传通道
    if (!NET_DVR_CloseAlarmChan_V30(lHandle))
    {
        printf("NET_DVR_CloseAlarmChan_V30 error, %d\n", NET_DVR_GetLastError());
        return 0;
    }
    return 1;
}
/**
 * @brief   网络通信线程（不使用）
 * @author  liuguang
 * @date    2021/05/31
 * @param   [in] user_id ：设备登录ID
 * @return  [long] 设备登录ID
 */
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


/**
 * @brief   RAMQ通信线程
 * @author  liuguang
 * @date    2021/05/31
 * @param   [in] NULL
 * @return  [int] 0为失败，1为成功
 */
int thread_RAMQ()
{
    Json::Value config = getConfig();   //读取配置文件
    RAMQ request;                       //创建rabbitmq通信对象
	request.setconfig(config[1]);       //配置rabbitmq连接参数
    
    Json::Value message;                //创建json变量
    Json::Reader reader;                //创建json读取器
    printf("\n建立网络连接通道...\n");
    int status=0;   
	while(1)
    {
        status = request.receive();     //监听信息
        if(status == RAMQ_OK)           //成功接收信息
        {
            if(reader.parse(request.response,message))      //将消息内容转化为json格式
            {
                if(  message.isObject() && !message["header"].isNull() )    //判断消息是否为空以及是否符合格式
                {
                    // messageid = message["header"]["messageId"].asString();
                    status = selectFun(message);                            //根据消息内容选择执行的命令
                }
            }
            if (status ==1)request.ack();   //命令执行成功后，消费消息
        }
    }
    printf("\n[ERROR]网络通道断开，RAMQ线程退出\n");
	return 1;
}