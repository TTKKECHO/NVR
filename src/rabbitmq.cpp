#include "rabbitmq.h"
#include <iostream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>




/** 
 * @brief   RAMQ构造函数
 * @author  liuguang
 * @date    2021/06/05
 * @param   NULL
 * @return  NULL
 */
RAMQ::RAMQ()
{
    amqp_connection_state_t sstate = amqp_new_connection();
    state = sstate;
}

/** 
 * @brief   RAMQ构造函数，仅构建连接对象state
 * @author  liuguang
 * @date    2021/06/05
 * @param   [in] server_url:broker地址
 * @param   [in] server_port:broker端口
 * @return  NULL
 */
RAMQ::RAMQ(std::string server_url,int server_port)
{
    amqp_connection_state_t sstate = amqp_new_connection();
    state = sstate;
    amqp_socket_t *socket = amqp_tcp_socket_new(state);
    assert(socket);
    url = server_url;
    port = server_port;
    int rc = amqp_socket_open(socket, server_url.c_str(), server_port);
    assert(rc == AMQP_STATUS_OK);
}

/** 
 * @brief   RAMQ构造函数
 * @author  liuguang
 * @date    2021/06/05
 * @param   [in] config:broker参数
 * @return  NULL
 */
 RAMQ::RAMQ(Json::Value config)
 {
    amqp_connection_state_t sstate = amqp_new_connection();
    state = sstate;
    amqp_socket_t *socket = amqp_tcp_socket_new(state);
    assert(socket);
    url = config["server_url"].asString();
    port = atoi(config["server_port"].asCString());
    int rc = amqp_socket_open(socket, url.c_str(),port);
    assert(rc == AMQP_STATUS_OK);
 }


/** 
 * @brief   RAMQ析构函数
 * @author  liuguang
 * @date    2021/06/05
 * @param   NULL
 * @return  NULL
 */
RAMQ::~RAMQ()
{
    close_and_destroy_connection(state);
}

/** 
 * @brief   参数配置(简单版)
 * @author  liuguang
 * @date    2021/06/05
 * @param   [in] config:RAMQ配置参数
 * @return  NULL
 */
int RAMQ::setconfig(Json::Value config)
{
    amqp_socket_t *socket = amqp_tcp_socket_new(state);
    assert(socket);
    url = config["server_url"].asString();
    port = atoi(config["server_port"].asCString());
    int rc = amqp_socket_open(socket, url.c_str(),port);
    assert(rc == AMQP_STATUS_OK);
    vhost = config["vhost"].asString();
    channel_max = std::atoi(config["channel_max"].asCString());
    username =config["username"].asString();
    password = config["password"].asString();
    exchange = config["exchange"].asString();
    key = config["key"].asString();
    connect(1048576);
    recv_queue = config["recv_queue"].asString();
    upload_queue = config["upload_queue"].asString();

    open_channel(CH_RECV);
    open_channel(CH_UPLOAD);
    return RAMQ_OK;
}

/** 
 * @brief   初始化RabbitMQ链接
 * @author  liuguang
 * @date    2021/06/10
 * @param   [in] host: 虚拟主机
 * @param   [in] usrname: 账号
 * @param   [in] pwd: 密码
 * @param   [in] channel_Max: 最大连接通道数，默认打开两个，0代表没有限制
 * @param   [in] frame_size：最大帧长度，默认值131072，增大这个值有助于提高吞吐，降低这个值有利于降低时延
 * @param   [in] heartbeat: 心跳间隔，默认0关闭心跳，只支持部分功能的心跳
 * @param   [in] sasl_method: 验证方式，默认AMQP_SASL_METHOD_PLAIN
 * @return  [int] 执行结果标志，RAMQ_ERROR失败，RAMQ_OK成功
 */
int RAMQ::connect(std::string host,std::string usrname,std::string pwd,int channel_Max,int frame_size,int heartbeat,amqp_sasl_method_enum sasl_method)
{
    reply = amqp_login(state,host.c_str(), channel_Max,frame_size,heartbeat, sasl_method,usrname.c_str(), pwd.c_str());
    assert(reply.reply_type == AMQP_RESPONSE_NORMAL);
    vhost = host;
    channel_max = channel_Max;
    username =usrname;
    password = pwd;
    return RAMQ_OK;


}

/** 
 * @brief   使用已有参数初始化RabbitMQ链接
 * @author  liuguang
 * @date    2021/06/10
 * @param   [in] frame_size：最大帧长度，默认值131072，增大这个值有助于提高吞吐，降低这个值有利于降低时延
 * @param   [in] heartbeat: 心跳间隔，默认0关闭心跳，只支持部分功能的心跳
 * @param   [in] sasl_method: 验证方式，默认AMQP_SASL_METHOD_PLAIN
 * @return  [int] 执行结果标志，RAMQ_ERROR失败，RAMQ_OK成功
 */
int RAMQ::connect(int frame_size,int heartbeat,amqp_sasl_method_enum sasl_method)
{
    reply = amqp_login(state,vhost.c_str(), channel_max,frame_size,heartbeat, sasl_method,username.c_str(), password.c_str());
    assert(reply.reply_type == AMQP_RESPONSE_NORMAL);
    return RAMQ_OK;

}

/** 
 * @brief   使用Json参数初始化RabbitMQ链接
 * @author  liuguang
 * @date    2021/06/10
 * @param   [in] config Json格式参数
 * @param   -vhost: 虚拟主机
 * @param   -username: 账号
 * @param   -password: 密码
 * @param   -channel_max: 最大连接通道数，默认打开两个，0代表没有限制
 * @param   [in] frame_size：最大帧长度，默认值131072，增大这个值有助于提高吞吐，降低这个值有利于降低时延
 * @param   [in] heartbeat: 心跳间隔，默认0关闭心跳，只支持部分功能的心跳
 * @param   [in] sasl_method: 验证方式，默认AMQP_SASL_METHOD_PLAIN
 * @return  [int] 执行结果标志，RAMQ_ERROR失败，RAMQ_OK成功
 */
int RAMQ::connect(Json::Value config,int frame_size,int heartbeat,amqp_sasl_method_enum sasl_method)
{
    vhost = config["vhost"].asString();
    channel_max = std::atoi(config["channel_max"].asCString());
    username =config["username"].asString();
    password = config["password"].asString();
    reply = amqp_login(state,vhost.c_str(), channel_max,frame_size,heartbeat, sasl_method,username.c_str(), password.c_str());
    assert(reply.reply_type == AMQP_RESPONSE_NORMAL);
    return RAMQ_OK;


}


/** 
 * @brief   声明交换机（还没写）
 * @author  liuguang
 * @date    2021/06/10
 * @param   [in] name 交换机名称
 * @param   [in] type: 交换机类型
 * @return  [int] 执行结果标志，RAMQ_ERROR失败，RAMQ_OK成功

 */
int RAMQ::declare_exchange(std::string name,std::string type)
{
    return RAMQ_OK;
}

/** 
 * @brief   声明队列
 * @author  liuguang
 * @date    2021/06/10
 * @param   [in] name 队列名称
 * @param   [in] channel: 绑定通道
 * @param   [in] passive: 检测队列是否已存在。如果为1, 队列已存在返回成功，不存在返回错误，不自动创建队列；为0，队列不存在自动创建队列
 * @param   [in] durable: 1持久化；0非持久化
 * @param   [in] exclusive: 1当前连接不在时，队列自动删除 0当前连接不在时，队列不自动删除
 * @param   [in] auto_delete: 默认为 0没有consumer队列不自动删除；1没有consumer时，队列自动删除 
 * @return  [int] 执行结果标志，RAMQ_ERROR失败，RAMQ_OK成功
 */
void RAMQ::declare_queue(std::string name,int channel,int passive,int durable,int exclusive,int auto_delete)
{
    amqp_queue_declare_ok_t *res = amqp_queue_declare(
        state, channel, amqp_cstring_bytes(name.c_str()),passive, durable,exclusive,auto_delete, amqp_empty_table);
    assert(res != NULL);
}


/** 
 * @brief   打开通道
 * @author  liuguang
 * @date    2021/06/10
 * @param   [in] channel_id 打开通道的id
 * @return  [int] 执行结果标志，RAMQ_ERROR失败，RAMQ_OK成功
 */
void RAMQ::open_channel(int channel_id)
{
    if(channel_id>channel_max){printf("\n[ERROR]超出最大通道数量\n");};
    amqp_channel_open_ok_t *res =  amqp_channel_open(state,channel_id);
    assert(res != NULL);
}

/** 
 * @brief   发送消息（使用默认参数）
 * @author  liuguang
 * @date    2021/06/05
 * @param   NULL
 * @return  [int] RAMQ_EOORR为失败，RAMQ_OK为成功
 */
int RAMQ::publish()
{
    amqp_basic_properties_t properties;
    properties._flags = 0;
    properties._flags |= AMQP_BASIC_DELIVERY_MODE_FLAG;
    properties.delivery_mode = AMQP_DELIVERY_NONPERSISTENT;
    properties.message_id = amqp_cstring_bytes(getLocalTime().c_str());
    int res;
    int times = 0;
    while(1)
    {
        times++;
        res = amqp_basic_publish(state,CH_UPLOAD,amqp_cstring_bytes(exchange.c_str()),amqp_cstring_bytes(key.c_str()),1,0,&properties,amqp_cstring_bytes(message.c_str()));
        if(res == AMQP_STATUS_OK || times == 100)
        {
            break;
        }
    }
    switch(res)
    {
        case AMQP_STATUS_OK:{
            int len = message.length();
            printf("\n[发送成功]消息长度为:%d\n",len);
            return RAMQ_OK;
            }break;
        case AMQP_STATUS_TIMER_FAILURE : printf("\n系统计时器设施返回错误，消息未被发送。\n");break;
        case AMQP_STATUS_HEARTBEAT_TIMEOUT : printf("\n等待broker的心跳连接超时，消息未被发送\n");break;
        case AMQP_STATUS_NO_MEMORY : printf("\n属性中的table太大而不适合单个框架，消息未被发送\n");break;
        case AMQP_STATUS_TABLE_TOO_BIG : printf("\n属性中的table太大而不适合单个框架，消息未被发送\n");break;
        case AMQP_STATUS_CONNECTION_CLOSED : printf("\n连接被关闭。\n");break;
        case AMQP_STATUS_SSL_ERROR : printf("\n发生SSL错误。\n");break;
        case AMQP_STATUS_TCP_ERROR : printf("\n发生TCP错误，errno或WSAGetLastError\n");break;
        default : printf("\n[ERROR%d]发送失败，请检查网络状态\n",res);break;
    }
    return RAMQ_ERROR;

}


/** 
 * @brief   发送消息(输入字符串数据)
 * @author  liuguang
 * @date    2021/06/15
 * @param   [in] data:消息内容
 * @return  [int] RAMQ_EOORR为失败，RAMQ_OK为成功
 */
int RAMQ:: publish(std::string data)
{
    message = data;
    return publish();
}

/** 
 * @brief   发送消息(Json格式)
 * @author  liuguang
 * @date    2021/06/15
 * @param   [in] data:消息内容
 * @return  [int] RAMQ_EOORR为失败，RAMQ_OK为成功
 */
int RAMQ:: publish(Json::Value data)
{
    Json::StyledWriter writer;
    std::string jsdata = writer.write(data);
    message = jsdata;
    return publish();
}

/** 
 * @brief   发送消息(Json格式，带routing_key)
 * @author  liuguang
 * @date    2021/06/15
 * @param   [in] data:消息内容
 * @param   [in] routing_key:路由键
 * @return  [int] RAMQ_EOORR为失败，RAMQ_OK为成功
 */
int RAMQ:: publish(Json::Value data,std::string routing_key)
{
    Json::StyledWriter writer;
    std::string jsdata = writer.write(data);
    message = jsdata;
    amqp_basic_properties_t properties;
    properties._flags = 0;
    properties._flags |= AMQP_BASIC_DELIVERY_MODE_FLAG;
    properties.delivery_mode = AMQP_DELIVERY_NONPERSISTENT;
    properties.message_id = amqp_cstring_bytes(getLocalTime().c_str());

    int res;
    int times = 0;
    while(1)
    {
        times++;
        res = amqp_basic_publish(state,CH_UPLOAD,amqp_cstring_bytes(exchange.c_str()),amqp_cstring_bytes(routing_key.c_str()),1,0,&properties,amqp_cstring_bytes(jsdata.c_str()));
        if(res == AMQP_STATUS_OK || times == 100)
        {
            break;
        }
    }
    switch(res)
    {
        case AMQP_STATUS_OK:{
            int len = message.length();
            printf("\n[发送成功]消息长度为:%d\n",len);
            return RAMQ_OK;
            
            }break;
        case AMQP_STATUS_TIMER_FAILURE : printf("\n系统计时器设施返回错误，消息未被发送。\n");break;
        case AMQP_STATUS_HEARTBEAT_TIMEOUT : printf("\n等待broker的心跳连接超时，消息未被发送\n");break;
        case AMQP_STATUS_NO_MEMORY : printf("\n属性中的table太大而不适合单个框架，消息未被发送\n");break;
        case AMQP_STATUS_TABLE_TOO_BIG : printf("\n属性中的table太大而不适合单个框架，消息未被发送\n");break;
        case AMQP_STATUS_CONNECTION_CLOSED : printf("\n连接被关闭。\n");break;
        case AMQP_STATUS_SSL_ERROR : printf("\n发生SSL错误。\n");break;
        case AMQP_STATUS_TCP_ERROR : printf("\n发生TCP错误，errno或WSAGetLastError\n");break;
        default : printf("\n[ERROR%d]发送失败，请检查网络状态\n",res);break;
    }
    return RAMQ_ERROR;
}

/** 
 * @brief   发送消息(默认参数，带消息id)
 * @author  liuguang
 * @date    2021/06/15
 * @param   [in] id:消息id
 * @return  [int] RAMQ_EOORR为失败，RAMQ_OK为成功
 */
int RAMQ::publish_with_id(std::string id)
{
    amqp_basic_properties_t properties;
    properties._flags = 0;
    properties._flags |= AMQP_BASIC_DELIVERY_MODE_FLAG;
    properties.delivery_mode = AMQP_DELIVERY_NONPERSISTENT;
    properties.message_id = amqp_cstring_bytes(id.c_str());
    int res;
    int times = 0;
    while(1)
    {
        times++;
        res = amqp_basic_publish(state,CH_UPLOAD,amqp_cstring_bytes(exchange.c_str()),amqp_cstring_bytes(key.c_str()),1,0,&properties,amqp_cstring_bytes(message.c_str()));
        if(res == AMQP_STATUS_OK || times == 100)
        {
            break;
        }
    }
    switch(res)
    {
        case AMQP_STATUS_OK:{printf("\n发送成功\n");return RAMQ_OK;}break;
        case AMQP_STATUS_TIMER_FAILURE : printf("\n系统计时器设施返回错误，消息未被发送。\n");break;
        case AMQP_STATUS_HEARTBEAT_TIMEOUT : printf("\n等待broker的心跳连接超时，消息未被发送\n");break;
        case AMQP_STATUS_NO_MEMORY : printf("\n属性中的table太大而不适合单个框架，消息未被发送\n");break;
        case AMQP_STATUS_TABLE_TOO_BIG : printf("\n属性中的table太大而不适合单个框架，消息未被发送\n");break;
        case AMQP_STATUS_CONNECTION_CLOSED : printf("\n连接被关闭。\n");break;
        case AMQP_STATUS_SSL_ERROR : printf("\n发生SSL错误。\n");break;
        case AMQP_STATUS_TCP_ERROR : printf("\n发生TCP错误，errno或WSAGetLastError\n");break;
        default : printf("\n[ERROR%d]发送失败，请检查网络状态\n",res);break;
    }
    return RAMQ_ERROR;


}

/** 
 * @brief   发送消息(字符串格式，带消息id)
 * @author  liuguang
 * @date    2021/06/15
 * @param   [in] data:消息内容
 * @param   [in] id:消息id
 * @return  [int] RAMQ_EOORR为失败，RAMQ_OK为成功
 */
int RAMQ::publish_with_id(std::string data,std::string id)
{
    message = data;
    return publish_with_id(id);
}

/** 
 * @brief   消息接收（使用默认参数）
 * @author  liuguang
 * @date    2021/06/05
 * @param   NULL
 * @return  [int] RAMQ_OK接收完毕
 */
int RAMQ::receive()
{
    amqp_message_t mes;
    amqp_method_t method;
    response.clear();
    while(1)
	{
        reply = amqp_basic_get(state,CH_RECV,amqp_cstring_bytes(recv_queue.c_str()),false);
        if(reply.reply_type==AMQP_RESPONSE_NORMAL && reply.reply.id!=AMQP_BASIC_GET_EMPTY_METHOD)
        {   
            amqp_read_message(state,CH_RECV,&mes,0);
            char *body = new char[mes.body.len+1];
            memcpy(body, mes.body.bytes, mes.body.len);
            body[mes.body.len]='\0';
            response = std::string(body);
            printf("\nYou have a new message:\n");
            amqp_destroy_message(&mes);
            method = reply.reply;
            s = (amqp_basic_ack_t*)method.decoded;
            //if(flag == 1)amqp_basic_ack(state,CH_RECV,s->delivery_tag,false);
            break;
        }
    }
    return RAMQ_OK;
}


/** 
 * @brief   消费消息
 * @author  liuguang
 * @date    2021/06/05
 * @param   NULL
 * @return  RAMQ_OK消费成功，RAMQ_OK消费失败
 */
int RAMQ::ack()
{
    int status = amqp_basic_ack(state,CH_RECV,s->delivery_tag,false);
    if(status == 0) return RAMQ_OK;
    return RAMQ_ERROR;
}

/** 
 * @brief   销毁连接
 * @author  liuguang
 * @date    2021/06/05
 * @param   [in] state:连接对象
 * @return  NULL
 */
void close_and_destroy_connection(amqp_connection_state_t state) 
{
    amqp_rpc_reply_t rpc_reply = amqp_connection_close(state, AMQP_REPLY_SUCCESS);
    assert(rpc_reply.reply_type == AMQP_RESPONSE_NORMAL);
    int rc = amqp_destroy_connection(state);
    assert(rc == AMQP_STATUS_OK);
}
