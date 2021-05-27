#include "http_libcurl.h"
#include <iostream>
#include <vector>


static size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);


/**
 * @Description:构造函数
 * @Author:liuguang
 * @Date:2021/05/27
 * @Param:
 *      type:curl初始化类型
 */
HLC::HLC(int type)
{
    
    curl_type = type;
    //添加默认请求头"Content-Type:"
    header = NULL;
    header=curl_slist_append(header,"Content-Type:");
    std::cout<<"curl Init..."<<std::endl;
    switch (type)
    {
    case EASYCURL:
        curl = curl_easy_init();
        break;
    case MULTICURL:
        curl = curl_multi_init();
        break;
    default:
        break;
    }
    //设置回调地址及超时时间
    curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,write_callback);
    curl_easy_setopt(curl,CURLOPT_WRITEDATA,&response);
    curl_easy_setopt( curl, CURLOPT_CONNECTTIMEOUT, 3 );
    curl_easy_setopt( curl, CURLOPT_TIMEOUT, 10);
}

/**
 * @Description: 析构函数
 * @Author:liuguang
 * @Date:2021/05/27
 */
HLC::~HLC()
{
    switch (curl_type)
    {
    case EASYCURL:
        curl_easy_cleanup(curl);
        break;
    case MULTICURL:
        curl_multi_cleanup(curl);
        break;
    default:
        break;
    }
}

/**
 * @Description: curl回调函数,以string类型接受数据
 * @Author:liuguang
 * @Date:2021/05/27
 * @Param:
 *      userdata:写入数据地址
 */
size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    
    std::string response_data(reinterpret_cast<const char*>(ptr), size * nmemb);
    *(std::string *)userdata += response_data;
    return size * nmemb;
}


/**
 * @Description: 全局初始化curl，仅需在主函数中初始化一次
 * @Author:liuguang
 * @Date:2021/05/27
 * @Param:
 *      type:初始化curl的方式，默认使用CURL_GLOBAL_DEFAULT模式，
 */
void http_init(long type )
{
    curl_global_init(type);

}


/**
 * @Description:设置请求头
 * @Author:liuguang
 * @Date:2021/05/27
 * @Param:
 *      params:请求头字符串，多个请求头中间用 ; 隔开
 */
void HLC::headers(std::string params)
{
    std::vector<std::string> head;
    int index = params.find(";");
    while(index>1)
    {
        head.push_back(params.substr(0,index-1));
        params=params.substr(index+1,params.size());
        index = params.find(";");
    }
    head.push_back(params);
    std::vector<std::string>::iterator item = head.begin();
    while(item != head.end()){
    header=curl_slist_append(header,(*item).c_str());
    item++;
    }
}

/**
 * @Description: 发送post请求
 * @Author:liuguang
 * @Date:2021/05/27
 * @Param:
 *      URL:目标链接地址，可在此设置，为空时使用.url作为参数
 *      header_type:是否获取文件头，默认不获取
 */
void HLC::post(std::string URL,int header_type)
{
    if(URL.size()){url=URL;}
    if(url.size()<5){
        printf("\nUrl is empty!Please check the url!\n");
    }
    if(curl)
	{
		curl_easy_setopt(curl,CURLOPT_URL,url.c_str());
        curl_easy_setopt(curl, CURLOPT_HEADER,header_type);
	    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
        curl_easy_setopt(curl, CURLOPT_POST, 1L); 
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE,data.size());
        curl_easy_setopt(curl,CURLOPT_POSTFIELDS,data.c_str());
		res0 = curl_easy_perform(curl);
		printf("res:%d\n",res0);
	}
    else{
        printf("\ncurl error!\n");
    }
}


/**
 * @Description: 设置连接超时时间
 * @Author:liuguang
 * @Date:2021/05/27
 */
void HLC::connectTimeOut(int time)
{
    connect_timeout=time;
    curl_easy_setopt( curl, CURLOPT_CONNECTTIMEOUT, time );
}



/**
 * @Description: 设置接收数据超时时间
 * @Author:liuguang
 * @Date:2021/05/27
 */
void HLC::recvTimeOut(int time)
{
    recv_timecout=time;
    curl_easy_setopt( curl, CURLOPT_TIMEOUT, time);
}