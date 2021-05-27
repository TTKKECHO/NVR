#ifndef __HTTP_LIBCURL_H
#define __HTTP_LIBCURL_H

#include <curl/curl.h>
#include <curl/easy.h>
#include <string>


//curl的使用模式，默认使用easycurl（multicul还没写）
enum CURL_TYPE
{
    EASYCURL = 0,
    MULTICURL
};

//curl是否接受文件头，默认不接收
enum RECV_HEADER
{
    OFF = 0,
    ON
};


class HLC
{
private:
    CURL *curl;
    int  curl_type;
public:
    std::string url;
    CURLcode res0;
    std::string response;
    std::string data;
    curl_slist *header;
    int connect_timeout;
    int recv_timecout;



public:
    HLC(int type = EASYCURL);
    ~HLC();
    void headers(std::string params);
    void post(std::string URL="",int header_type=OFF);
    void connectTimeOut(int time);
    void recvTimeOut(int time);



};

void http_init(long type = CURL_GLOBAL_DEFAULT);


#endif