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

class RAMQ
{
private:
    amqp_connection_state_t state;
    amqp_bytes_t exchange;
    std::string exchange_type;
    amqp_bytes_t key;
    std::string queue;
public:
    std::string message;
    std::string response;
    amqp_rpc_reply_t reply;
public:
    RAMQ(amqp_connection_state_t connect_state);
    ~RAMQ();
    void set(Json::Value config,int type);
    void publish(std::string data);
    void receive();

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