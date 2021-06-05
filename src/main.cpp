#include <iostream>
#include <fstream>
#include <stdbool.h> 
#include <thread>
#include <jsoncpp/json/json.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "Common.h"
#include "HCNetSDK.h"
#include "CapPicture.h"
#include "PTZControl.h"
#include "http_libcurl.h"
#include "base64.h"
#include "alarm.h"
#include "facelib.h"
#include "background.h"
#include <amqp.h>
#include <amqp_framing.h>
#include "rabbitmq.h"



using namespace std;


#define RT 1

amqp_connection_state_t state;

int main()
 {


	Json::Value config1 = getConfig();
	state = RAMQ_Init(config1[1]);
	RAMQ request(state);
	request.set(config1[1],QUEUE_RECV);
	request.receive();
	return 0;


// 	amqp_connection_state_t mq;
// 	int socket = amqp_open_socket("www.liuguang.space",5672);
// 	amqp_set_sockfd(mq,socket);



	int rpc_reply;
	amqp_connection_state_t state = setup_connection_and_channel();
	//amqp_exchange_declare(state,1,amqp_cstring_bytes("liuguang.topic"),amqp_cstring_bytes("topic"),0,1,1,0,amqp_empty_table);
	printf("\nflag:0\n");
	//queue_declare(state,"topic.first");
	printf("\nflag:1\n");
	amqp_queue_bind(state,1,amqp_cstring_bytes("topic.first"),amqp_cstring_bytes("topicExchange"),amqp_cstring_bytes("hello"),amqp_empty_table);
	amqp_queue_bind(state,2,amqp_cstring_bytes("topic.second"),amqp_cstring_bytes("topicExchange"),amqp_cstring_bytes("world"),amqp_empty_table);
	printf("\nflag:2\n");
#if RT
	int i =10;
	while(i--)
	{
		string sdata;
		cout<<"发送："<<endl;
		cin>>sdata;
		amqp_bytes_t message_bytes = amqp_cstring_bytes(sdata.c_str());

		amqp_basic_properties_t properties;
		printf("\nflag:3\n");

		properties._flags = 0;

		properties._flags |= AMQP_BASIC_DELIVERY_MODE_FLAG;
		properties.delivery_mode = AMQP_DELIVERY_NONPERSISTENT;
		rpc_reply = amqp_basic_publish(state,2,amqp_cstring_bytes("topicExchange"),amqp_cstring_bytes("world"),1,0,&properties,message_bytes);


	}
	
#else
	//amqp_basic_consume(state,1,amqp_cstring_bytes("test queue"),amqp_cstring_bytes("liuguang"),false,false,false,amqp_empty_table);
	amqp_envelope_t envelope;
    struct timeval timeout = {5, 0};
    //amqp_rpc_reply_t reply = amqp_consume_message(state, &envelope, &timeout, 0);
	amqp_rpc_reply_t reply;
	while(1)
	{
		 reply = amqp_basic_get(state,1,amqp_cstring_bytes("topic.first"),false);
		cout<<"reply:"<<reply.reply_type<<endl;
		cout<<"reply:"<<reply.reply.id<<endl;
		if(reply.reply.id!=AMQP_BASIC_GET_EMPTY_METHOD)break;
		

	}
   	


	cout<<"haha"<<endl; 
	//if(reply.reply_type == AMQP_RESPONSE_NORMAL)
	//{
		//cout<<"success"<<endl;
		amqp_message_t mes;
		amqp_read_message(state,1,&mes,0);
	cout<<"hahaha"<<endl; 

		// uint64_t out_body_size_;
		// out_body_size_ = envelope.message.body.len;
		// char *body = new char[out_body_size_+1];
		// if (out_body_size_) {
		// 	memcpy(body, envelope.message.body.bytes, out_body_size_);
		// }
		// body[out_body_size_] ='\0';
		char *body = new char[mes.body.len+1];
		memcpy(body, mes.body.bytes, mes.body.len);
		body[mes.body.len]='\0';
		string data(body);
		printf("body:%s\n",body);
		printf("data:%s\n",data.c_str());
		amqp_destroy_message(&mes);
		amqp_method_t method = reply.reply;
		amqp_basic_ack_t *s;
		s = (amqp_basic_ack_t*)method.decoded;
		amqp_basic_ack(state,1,s->delivery_tag,false);
		//printf(" envelope.message.body.bytes:%s\n", envelope.message.body.bytes);

		//amqp_destroy_envelope(&envelope);

	//}
#endif
	// basic_publish(state,"");
	// uint64_t body_size;
	// char *res = basic_get(state,"test queue",&body_size);
	// cout<<"res:"<<res<<endl;
	//sleep(100);
 	// publish_and_basic_get_message("test message");
  	// publish_and_basic_get_message("TEST");

  	// publish_and_consume_message("");
  	// publish_and_consume_message("TEST");

	close_and_destroy_connection(state);
	return 0;



	Json::Value config = getConfig();
	http_init();
	long user_id = NVR_Init(config[0]);
	state = RAMQ_Init(config[1]);
	if(user_id < 0)
	{
		printf("\n[ERROR]设备初始化失败！\n");
		return 0;
	}
	get_FDLib_capabilities(user_id);

	//创建报警布防线程与网络通信线程
	thread alarm(thread_ALARM,user_id);
	thread web(thread_WEB,user_id);
	alarm.join();
	web.join();

    //logout
    NET_DVR_Logout(user_id);
    NET_DVR_Cleanup();
	
	return 0;
}