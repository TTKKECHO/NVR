#include "rabbitmq.h"
#include <iostream>
#include <string>
#include <stdlib.h>
#include <unistd.h>




/** 
 * @brief   RAMQ构造函数
 * @author  liuguang
 * @date    2021/06/05
 * @param   [in] server_url:broker地址
 * @param   [in] server_port:broker端口
 * @return  NULL
 */
RAMQ::RAMQ(std::string server_url,int server_port)
{
    state = amqp_new_connection();
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
    printf("\n3\n");
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
 * @brief   初始化RabbitMQ链接
 * @author  liuguang
 * @date    2021/06/05
 * @param   [in] config
 * @param   -server_url: RabbitMQ服务端broker地址
 * @param   -port: 端口
 * @param   -vhost: 虚拟主机
 * @param   -username: 账号
 * @param   -password: 密码
 * @return  [amqp_connection_state_t] 链接对象
 */
amqp_connection_state_t RAMQ_Init(Json::Value config)
{
    amqp_connection_state_t state = amqp_new_connection();
    amqp_socket_t *socket = amqp_tcp_socket_new(state);
    assert(socket);
    int rc = amqp_socket_open(socket, config["server_url"].asCString(), AMQP_PROTOCOL_PORT);
    assert(rc == AMQP_STATUS_OK);

    amqp_rpc_reply_t rpc_reply = amqp_login(
        state, config["vhost"].asCString(), 10, AMQP_DEFAULT_FRAME_SIZE,
        AMQP_DEFAULT_HEARTBEAT, AMQP_SASL_METHOD_PLAIN, config["username"].asCString(), config["password"].asCString());
   
    assert(rpc_reply.reply_type == AMQP_RESPONSE_NORMAL);
    amqp_channel_open_ok_t *res = amqp_channel_open(state, CH_UPLOAD);
    assert(res != NULL);
    res = amqp_channel_open(state, CH_RECV);
    assert(res != NULL);
    
   // amqp_queue_bind(state,CH_RECV,amqp_cstring_bytes(config["recv_queue"].asCString()),amqp_cstring_bytes(config["exchange"].asCString()),amqp_cstring_bytes("RECV"),amqp_empty_table);
	amqp_queue_bind(state,CH_UPLOAD,amqp_cstring_bytes(config["upload_queue"].asCString()),amqp_cstring_bytes(config["exchange"].asCString()),amqp_cstring_bytes("UPLOAD"),amqp_empty_table);
    return state;
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
 * @return  [int] 执行结果标志，RAMQ_ERROR成功，RAMQ_OK失败
 */
int RAMQ::connect(std::string host,std::string usrname,std::string pwd,int channel_Max,int frame_size,int heartbeat,amqp_sasl_method_enum sasl_method)
{
    reply = amqp_login(state,host.c_str(), channel_Max,frame_size,heartbeat, sasl_method,usrname.c_str(), pwd.c_str());
    assert(reply.reply_type == AMQP_RESPONSE_NORMAL);
    vhost = host;
    channel_max = channel_Max;
    username =usrname;
    password = pwd;

}

/** 
 * @brief   使用已有参数初始化RabbitMQ链接
 * @author  liuguang
 * @date    2021/06/10
 * @param   [in] frame_size：最大帧长度，默认值131072，增大这个值有助于提高吞吐，降低这个值有利于降低时延
 * @param   [in] heartbeat: 心跳间隔，默认0关闭心跳，只支持部分功能的心跳
 * @param   [in] sasl_method: 验证方式，默认AMQP_SASL_METHOD_PLAIN
 * @return  [int] 执行结果标志，RAMQ_ERROR成功，RAMQ_OK失败
 */
int RAMQ::connect(int frame_size,int heartbeat,amqp_sasl_method_enum sasl_method)
{
    reply = amqp_login(state,vhost.c_str(), channel_max,frame_size,heartbeat, sasl_method,username.c_str(), password.c_str());
    assert(reply.reply_type == AMQP_RESPONSE_NORMAL);
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
 * @return  [int] 执行结果标志，RAMQ_ERROR成功，RAMQ_OK失败
 */
int RAMQ::connect(Json::Value config,int frame_size,int heartbeat,amqp_sasl_method_enum sasl_method)
{
    vhost = config["vhost"].asString();
    channel_max = std::atoi(config["channel_max"].asCString());
    username =config["username"].asString();
    password = config["password"].asString();
    reply = amqp_login(state,vhost.c_str(), channel_max,frame_size,heartbeat, sasl_method,username.c_str(), password.c_str());
    assert(reply.reply_type == AMQP_RESPONSE_NORMAL);
}


/** 
 * @brief   声明交换机（还没写）
 * @author  liuguang
 * @date    2021/06/10
 * @param   [in] name 交换机名称
 * @param   [in] type: 交换机类型
 * @return  [int] 执行结果标志，RAMQ_ERROR成功，RAMQ_OK失败
 */
int RAMQ::declare_exchange(std::string name,std::string type)
{




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
 * @return  [int] 执行结果标志，RAMQ_ERROR成功，RAMQ_OK失败
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
 * @return  [int] 执行结果标志，RAMQ_ERROR成功，RAMQ_OK失败
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
 * @param   [in] data:消息内容
 * @return  NULL
 */
void RAMQ:: publish(std::string data)
{
    message = data;
    amqp_basic_properties_t properties;
    properties._flags = 0;
    properties._flags |= AMQP_BASIC_DELIVERY_MODE_FLAG;
    properties.delivery_mode = AMQP_DELIVERY_NONPERSISTENT;
    int res;
    int times = 0;
    while(1)
    {
        times++;
        res = amqp_basic_publish(state,CH_UPLOAD,amqp_cstring_bytes(exchange.c_str()),amqp_cstring_bytes(key.c_str()),1,0,&properties,amqp_cstring_bytes(data.c_str()));
        if(res == AMQP_STATUS_OK || times == 100)
        {
            break;
        }
    }
    if(res != AMQP_STATUS_OK) printf("\n[ERROR]发送失败，请检查网络状态\n");
    switch(res)
    {
        case AMQP_STATUS_OK:printf("\n发送成功\n");break;
        case AMQP_STATUS_TIMER_FAILURE : printf("\n系统计时器设施返回错误，消息未被发送。\n");break;
        case AMQP_STATUS_HEARTBEAT_TIMEOUT : printf("\n等待broker的心跳连接超时，消息未被发送\n");break;
        case AMQP_STATUS_NO_MEMORY : printf("\n属性中的table太大而不适合单个框架，消息未被发送\n");break;
        case AMQP_STATUS_TABLE_TOO_BIG : printf("\n属性中的table太大而不适合单个框架，消息未被发送\n");break;
        case AMQP_STATUS_CONNECTION_CLOSED : printf("\n连接被关闭。\n");break;
        case AMQP_STATUS_SSL_ERROR : printf("\n发生SSL错误。\n");break;
        case AMQP_STATUS_TCP_ERROR : printf("\n发生TCP错误，errno或WSAGetLastError\n");break;
        default : printf("\n[ERROR]发送失败，请检查网络状态\n");break;
    }
}

/** 
 * @brief   消息接收（使用默认参数）
 * @author  liuguang
 * @date    2021/06/05
 * @param   [in] data:消息内容
 * @return  NULL
 */
int RAMQ::receive()
{
    amqp_message_t mes;
    amqp_method_t method;
    printf("\n [START]\n");
    while(1)
	{
        reply = amqp_basic_get(state,CH_RECV,amqp_cstring_bytes(recv_queue.c_str()),false);
        if(reply.reply_type==AMQP_RESPONSE_NORMAL && reply.reply.id!=AMQP_BASIC_GET_EMPTY_METHOD)
        {
            printf("\n [IN]\n");
            
            amqp_read_message(state,CH_RECV,&mes,0);
            char *body = new char[mes.body.len+1];
            memcpy(body, mes.body.bytes, mes.body.len);
            body[mes.body.len]='\0';
            response = std::string(body);
            printf("body:%s\n",body);
            amqp_destroy_message(&mes);
            method = reply.reply;
            s = (amqp_basic_ack_t*)method.decoded;
            //if(flag == 1)amqp_basic_ack(state,CH_RECV,s->delivery_tag,false);
            break;
        }
    }
    return RAMQ_OK;
}

int RAMQ::ack()
{
    amqp_basic_ack(state,CH_RECV,s->delivery_tag,false);
    return RAMQ_OK;
}

amqp_connection_state_t RAMQ();

amqp_connection_state_t setup_connection_and_channel(void) {
    amqp_connection_state_t connection_state_ = amqp_new_connection();
    amqp_socket_t *socket = amqp_tcp_socket_new(connection_state_);
    assert(socket);

    int rc = amqp_socket_open(socket, "192.168.2.47", AMQP_PROTOCOL_PORT);
    assert(rc == AMQP_STATUS_OK);

    amqp_rpc_reply_t rpc_reply = amqp_login(
        connection_state_, "/xinke", 10, AMQP_DEFAULT_FRAME_SIZE,
        AMQP_DEFAULT_HEARTBEAT, AMQP_SASL_METHOD_PLAIN, "zhangxiaozhou", "123456");
   
    assert(rpc_reply.reply_type == AMQP_RESPONSE_NORMAL);

    amqp_channel_open_ok_t *res = amqp_channel_open(connection_state_, 1);
    assert(res != NULL);
    res = amqp_channel_open(connection_state_, 2);
    assert(res != NULL);
    

    return connection_state_;
}

void close_and_destroy_connection(amqp_connection_state_t connection_state_) {
    amqp_rpc_reply_t rpc_reply = amqp_connection_close(connection_state_, AMQP_REPLY_SUCCESS);
    assert(rpc_reply.reply_type == AMQP_RESPONSE_NORMAL);

    int rc = amqp_destroy_connection(connection_state_);
    assert(rc == AMQP_STATUS_OK);
}

void basic_publish(amqp_connection_state_t connectionState_,
                   const char *message_) {
    amqp_bytes_t message_bytes = amqp_cstring_bytes(message_);

    amqp_basic_properties_t properties;
    properties._flags = 0;

    properties._flags |= AMQP_BASIC_DELIVERY_MODE_FLAG;
    properties.delivery_mode = AMQP_DELIVERY_NONPERSISTENT;

    int retval = amqp_basic_publish(
        connectionState_, 1, amqp_cstring_bytes(""),
        amqp_cstring_bytes("test"),
        /* mandatory=*/1,
        /* immediate=*/0, /* RabbitMQ 3.x does not support the "immediate" flag
                            according to
                            https://www.rabbitmq.com/specification.html */
        &properties, message_bytes);

    assert(retval == 0);
}

void queue_declare(amqp_connection_state_t connection_state_,
                   const char *queue_name_) {
    amqp_queue_declare_ok_t *res = amqp_queue_declare(
        connection_state_, 1, amqp_cstring_bytes(queue_name_),
        /*passive*/ 0,
        /*durable*/ 0,
        /*exclusive*/ 0,
        /*auto_delete*/ 1, amqp_empty_table);
    assert(res != NULL);
}

char *basic_get(amqp_connection_state_t connection_state_,
                const char *queue_name_, uint64_t *out_body_size_) {
    amqp_rpc_reply_t rpc_reply;
    amqp_time_t deadline;
    struct timeval timeout = {5, 0};
    int time_rc = amqp_time_from_now(&deadline, &timeout);
    assert(time_rc == AMQP_STATUS_OK);

    do {
        rpc_reply = amqp_basic_get(connection_state_, 1,
                                amqp_cstring_bytes(queue_name_), /*no_ack*/ 1);
    } while (rpc_reply.reply_type == AMQP_RESPONSE_NORMAL &&
            rpc_reply.reply.id == AMQP_BASIC_GET_EMPTY_METHOD &&
            amqp_time_has_past(deadline) == AMQP_STATUS_OK);

    assert(rpc_reply.reply_type == AMQP_RESPONSE_NORMAL);
    assert(rpc_reply.reply.id == AMQP_BASIC_GET_OK_METHOD);

    amqp_message_t message;
    rpc_reply = amqp_read_message(connection_state_, 1, &message, 0);
    assert(rpc_reply.reply_type == AMQP_RESPONSE_NORMAL);

    char *body =new char[message.body.len];
    memcpy(body, message.body.bytes, message.body.len);
    *out_body_size_ = message.body.len;
    amqp_destroy_message(&message);

    return body;
    }

void publish_and_basic_get_message(const char *msg_to_publish) {
    amqp_connection_state_t connection_state = setup_connection_and_channel();

    queue_declare(connection_state, "test");
    basic_publish(connection_state, msg_to_publish);

    uint64_t body_size;
    char *msg = basic_get(connection_state, "test", &body_size);
    assert(body_size == strlen(msg_to_publish));
    assert(strncmp(msg_to_publish, msg, body_size) == 0);
    free(msg);

    close_and_destroy_connection(connection_state);
}

char *consume_message(amqp_connection_state_t connection_state_,const char *queue_name_, uint64_t *out_body_size_) {
    amqp_basic_consume_ok_t *result =
        amqp_basic_consume(
            connection_state_, 1,
            amqp_cstring_bytes(queue_name_), amqp_empty_bytes,
            /*no_local*/ 0,
            /*no_ack*/ 1,
            /*exclusive*/ 0, amqp_empty_table
            );
    assert(result != NULL);

    amqp_envelope_t envelope;
    struct timeval timeout = {5, 0};
    amqp_rpc_reply_t rpc_reply =
        amqp_consume_message(connection_state_, &envelope, &timeout, 0);
    printf("\nrpc_reply:%d\n",rpc_reply.reply_type);
    assert(rpc_reply.reply_type == AMQP_RESPONSE_NORMAL);

    *out_body_size_ = envelope.message.body.len;
    char *body = new char[*out_body_size_];
    if (*out_body_size_) {
        memcpy(body, envelope.message.body.bytes, *out_body_size_);
    }

    amqp_destroy_envelope(&envelope);
    return body;
}

void publish_and_consume_message(const char *msg_to_publish) {
    amqp_connection_state_t connection_state = setup_connection_and_channel();

    queue_declare(connection_state, "test");
    basic_publish(connection_state, msg_to_publish);

    uint64_t body_size;
    char *msg = consume_message(connection_state, "test", &body_size);

    assert(body_size == strlen(msg_to_publish));
    assert(strncmp(msg_to_publish, msg, body_size) == 0);
    std::cout<<"msg:"<<msg_to_publish<<std::endl;
    free(msg);

    close_and_destroy_connection(connection_state);
}
