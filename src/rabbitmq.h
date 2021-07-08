#ifndef __RABBITMQ_H
#define __RABBITMQ_H	 

#include <iostream>
#include <string>
#include <stdio.h>
#include <amqp.h>
#include <amqp_framing.h>
#include <rabbitmq-c/amqp.h>
#include <rabbitmq-c/tcp_socket.h>
#include <assert.h>
#include <sys/time.h>
#include "amqp_time.h"
#include <unistd.h>
#include <string.h>
#include <jsoncpp/json/json.h>
#include "Common.h"

enum CHANNEL_TYPE
{
    CH_UPLOAD=1,
    CH_RECV
};

enum QUEUE_TYPE
{
    QUEUE_UPLOAD=0,
    QUEUE_RECV
};

enum RAMQ_RETURN
{
    RAMQ_ERROR=0,
    RAMQ_OK
};

class RAMQ
{
private:
    amqp_connection_state_t state;      //RAMQ连接对象
    std::string username;               //RAMQ账号
    std::string password;               //RAMQ密码
public:
    int port;                           //broker连接端口
    int channel_max;                    //最大通道数
    std::string url;                    //broker连接地址
    std::string vhost;                  //虚拟路由
    std::string exchange;               //交换机
    std::string exchange_type;          //交换机类型
    std::string key;                    //路由键
    std::string upload_queue;           //上传队列
    std::string recv_queue;             //接收队列
    std::string message;                //将要发送的信息
    std::string response;               //接收到的信息
    amqp_basic_ack_t *s;                //接收信息标志，用于消费消息
    amqp_rpc_reply_t reply;
public:
    RAMQ(std::string server_url,int server_port = AMQP_PROTOCOL_PORT);
    RAMQ(Json::Value config);
    RAMQ();
    ~RAMQ();
    int setconfig(Json::Value config);
    int connect(std::string host,std::string usrname,std::string pwd,int channel_Max = 2,int frame_size =AMQP_DEFAULT_FRAME_SIZE,int heartbeat = AMQP_DEFAULT_HEARTBEAT,amqp_sasl_method_enum sasl_method = AMQP_SASL_METHOD_PLAIN);
    int connect(int frame_size =AMQP_DEFAULT_FRAME_SIZE,int heartbeat = AMQP_DEFAULT_HEARTBEAT,amqp_sasl_method_enum sasl_method = AMQP_SASL_METHOD_PLAIN);
    int connect(Json::Value config,int frame_size =AMQP_DEFAULT_FRAME_SIZE,int heartbeat = AMQP_DEFAULT_HEARTBEAT,amqp_sasl_method_enum sasl_method = AMQP_SASL_METHOD_PLAIN);
    int declare_exchange(std::string name,std::string type);
    int publish();
    int publish(std::string data);
    int publish(Json::Value data);
    int publish(Json::Value data,std::string routing_key);
    int publish(std::string data,std::string routing_key);
    int publish_with_id(std::string id);
    int publish_with_id(std::string data,std::string id);

    int receive();
    int receive(std::string queue);
    int ack();

    void declare_queue(std::string name,int channel,int passive=0,int durable=0,int exclusive=0,int auto_delete = 1);
    void queue_bind(int channel,std::string queue,std::string exChange,std::string Key);
    void open_channel(int channel_id);
    void setUsername(std::string usrname);
    void setPassword(std::string pwd);
};

void close_and_destroy_connection(amqp_connection_state_t state) ;






#endif