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

extern amqp_connection_state_t state;

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
    RAMQ_EOORR=0,
    RAMQ_OK
};

class RAMQ
{
private:
    amqp_connection_state_t state;
    std::string username;
    std::string password;
public:
    int port;
    int channel_max;
    std::string url;
    std::string vhost;
    std::string exchange;
    std::string exchange_type;
    std::string key;
    std::string upload_queue;
    std::string recv_queue;
    std::string message;
    std::string response;
    amqp_basic_ack_t *s;
    amqp_rpc_reply_t reply;
public:
    RAMQ(std::string server_url,int server_port = AMQP_PROTOCOL_PORT);
    RAMQ(Json::Value config);
    ~RAMQ();
    int connect(std::string host,std::string usrname,std::string pwd,int channel_Max = 2,int frame_size =AMQP_DEFAULT_FRAME_SIZE,int heartbeat = AMQP_DEFAULT_HEARTBEAT,amqp_sasl_method_enum sasl_method = AMQP_SASL_METHOD_PLAIN);
    int connect(int frame_size =AMQP_DEFAULT_FRAME_SIZE,int heartbeat = AMQP_DEFAULT_HEARTBEAT,amqp_sasl_method_enum sasl_method = AMQP_SASL_METHOD_PLAIN);
    int connect(Json::Value config,int frame_size =AMQP_DEFAULT_FRAME_SIZE,int heartbeat = AMQP_DEFAULT_HEARTBEAT,amqp_sasl_method_enum sasl_method = AMQP_SASL_METHOD_PLAIN);
    int declare_exchange(std::string name,std::string type);
    void declare_queue(std::string name,int channel,int passive=0,int durable=0,int exclusive=0,int auto_delete = 1);
    void queue_bind(int channel,std::string queue,std::string exChange,std::string Key);
    void open_channel(int channel_id);
    void setUsername(std::string usrname);
    void setPassword(std::string pwd);
    void publish(std::string data);
    void publish(std::string data,std::string routing_key);
    int receive();
    int receive(std::string queue);
    int ack();
};


amqp_connection_state_t RAMQ_Init(Json::Value config);
amqp_connection_state_t setup_connection_and_channel(void);
void close_and_destroy_connection(amqp_connection_state_t connection_state_);
void basic_publish(amqp_connection_state_t connectionState_,const char *message_);
void queue_declare(amqp_connection_state_t connection_state_,const char *queue_name_);
char *basic_get(amqp_connection_state_t connection_state_,const char *queue_name_, uint64_t *out_body_size_);
void publish_and_basic_get_message(const char *msg_to_publish);
char *consume_message(amqp_connection_state_t connection_state_,const char *queue_name_, uint64_t *out_body_size_) ;
void publish_and_consume_message(const char *msg_to_publish);






#endif