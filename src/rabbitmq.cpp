#include "rabbitmq.h"
#include <iostream>
#include <string>



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

    amqp_channel_open_ok_t *res = amqp_channel_open(state, CH_RECV);
    assert(res != NULL);
    res = amqp_channel_open(state, CH_UPLOAD);
    assert(res != NULL);
    amqp_queue_bind(state,CH_RECV,amqp_cstring_bytes(config["recv_queue"].asCString()),amqp_cstring_bytes(config["exchange"].asCString()),amqp_cstring_bytes("RECV"),amqp_empty_table);
	amqp_queue_bind(state,CH_UPLOAD,amqp_cstring_bytes(config["upload_queue"].asCString()),amqp_cstring_bytes(config["exchange"].asCString()),amqp_cstring_bytes("UPLOAD"),amqp_empty_table);
    return state;
}


/** 
 * @brief   RAMQ构造函数
 * @author  liuguang
 * @date    2021/06/05
 * @param   [in] connect_state:链接对象
 * @param   [in] connect_state:链接对象
 * @return  NULL
 */
RAMQ::RAMQ(amqp_connection_state_t connect_state):state(connect_state)
{

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
	
}

/** 
 * @brief   RAMQ析构函数
 * @author  liuguang
 * @date    2021/06/05
 * @param   [in] config:设置参数
 * @param   [in] type:
 * @return  NULL
 */
void RAMQ::set(Json::Value config,int type)
{
    exchange = amqp_cstring_bytes(config["exchange"].asCString());
    exchange_type = "topic";
    if(type == QUEUE_UPLOAD)
    {
        queue = config["upload_queue"].asString();
        key = amqp_cstring_bytes("UPLOAD");
        }
    else if (type == QUEUE_RECV)
    {
        queue = config["recv_queue"].asString();
        
        key = amqp_cstring_bytes("RECV");
        }
    else
        {printf("\n[ERROR]查无此type\n");return;}  
    
}

/** 
 * @brief   发送消息
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
        res = amqp_basic_publish(state,CH_UPLOAD,exchange,key,1,0,&properties,amqp_cstring_bytes(message.c_str()));
        if(res == AMQP_STATUS_OK || times == 100)
        {
            break;
        }

    }
    if(res != AMQP_STATUS_OK) printf("\n[ERROR]发送失败，请检查网络状态\n");
    switch(res)
    {
        case AMQP_STATUS_TIMER_FAILURE : printf("\n系统计时器设施返回错误，消息未被发送。\n");break;
        case AMQP_STATUS_HEARTBEAT_TIMEOUT : printf("\n等待broker的心跳连接超时，消息未被发送\n");break;
        case AMQP_STATUS_NO_MEMORY : printf("\n属性中的table太大而不适合单个框架，消息未被发送\n");break;
        case AMQP_STATUS_TABLE_TOO_BIG : printf("\n属性中的table太大而不适合单个框架，消息未被发送\n");break;
        case AMQP_STATUS_CONNECTION_CLOSED : printf("\n连接被关闭。\n");break;
        case AMQP_STATUS_SSL_ERROR : printf("\n发生SSL错误。\n");break;
        case AMQP_STATUS_TCP_ERROR : printf("\n发生TCP错误，errno或WSAGetLastError\n");break;
        default : printf("\n未知错误\n");break;
    }
}


void  RAMQ::receive()
{
    amqp_message_t mes;
    amqp_method_t method;
	amqp_basic_ack_t *s;
    printf("\n [START]\n");
    while(1)
	{
        amqp_envelope_t envelope;
        struct timeval timeout = {5, 0};
		reply = amqp_basic_get(state,CH_RECV,amqp_cstring_bytes(queue.c_str()),false);
        //std::cout<<"reply:"<<reply.reply_type<<std::endl;
		//std::cout<<"reply:"<<reply.reply.id<<std::endl;
		if(reply.reply_type==AMQP_RESPONSE_NORMAL && reply.reply.id!=AMQP_BASIC_GET_EMPTY_METHOD)
        {
            std::cout<<"haha"<<std::endl;
            amqp_read_message(state,CH_RECV,&mes,0);
            std::cout<<"haha2"<<std::endl;
            char *body = new char[mes.body.len+1];
            std::cout<<"haha3"<<std::endl;
            memcpy(body, mes.body.bytes, mes.body.len);
            body[mes.body.len]='\0';
            response = std::string(body);
            printf("body:%s\n",body);
            amqp_destroy_message(&mes);
            method = reply.reply;
            s = (amqp_basic_ack_t*)method.decoded;
            int flag =1;
            //printf("输入flag：");
           // std::cin>>flag;
            if(flag == 1)amqp_basic_ack(state,CH_RECV,s->delivery_tag,false);
        }
	}
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
