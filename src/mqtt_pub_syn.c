#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "MQTTClient.h"

// #define ADDRESS     "tcp://localhost:1883"
// #define CLIENTID    "ExampleClientPub"
// #define TOPIC       "MQTT Examples"
// #define PAYLOAD     "Hello World!"
// #define QOS         1
// #define TIMEOUT     10000L

char *ADDRESS =   "tcp://localhost:1883";
char *CLIENTID =  "ExampleClientPub";
char *TOPIC    =  "MQTT Examples";
char *PAYLOAD  =  "Hello World!";
int  QOS      =  1;
long TIMEOUT  =  10000L;

typedef struct mqtt_comm {
	MQTTClient 			client;
	MQTTClient_connectOptions 	opts;
	MQTTClient_message 		msg;
	MQTTClient_deliveryToken 	token;
} mqtt_comm;

// typedef struct mqtt_config {
// 	char *ADDRESS =   "tcp://localhost:1883";
// 	char CLIENTID =  "ExampleClientPub";
// 	char TOPIC    =  "MQTT Examples";
// 	char PAYLOAD  =  "Hello World!";
// 	char QOS      =  1;
// 	char TIMEOUT  =  10000L;
// } mqtt_config;


void mqtt_set_comm(mqtt_comm *mc)
{
	MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
	MQTTClient_message pub_msg = MQTTClient_message_initializer;
	mc->opts = conn_opts;
	mc->msg = pub_msg;
}


int main(int argc, char* argv[])
{

	mqtt_comm mc;
	mqtt_set_comm(&mc);
	int rc;

	MQTTClient_create(&(mc.client), ADDRESS, CLIENTID,
			MQTTCLIENT_PERSISTENCE_NONE, NULL);

	mc.opts.keepAliveInterval = 20;
	mc.opts.cleansession = 1;

	if ((rc = MQTTClient_connect((mc.client), &(mc.opts))) != MQTTCLIENT_SUCCESS)
	{
		printf("Failed to connect, return code %d\n", rc);
		exit(-1);
	}
	mc.msg.payload = PAYLOAD;
	mc.msg.payloadlen = strlen(PAYLOAD);
	mc.msg.qos = QOS;
	mc.msg.retained = 0;

	MQTTClient_publishMessage((mc.client), TOPIC, &(mc.msg), &(mc.token));
	puts("KKKKKK");
	printf("Waiting for up to %d seconds for publication of %s\n"
			"on topic %s for client with ClientID: %s\n",
			(int)(TIMEOUT/1000), PAYLOAD, TOPIC, CLIENTID);
	rc = MQTTClient_waitForCompletion((mc.client), mc.token, TIMEOUT);
	printf("Message with delivery token %d delivered\n", mc.token);
	MQTTClient_disconnect(mc.client, 10000);
	MQTTClient_destroy(&(mc.client));
	return rc;
}


/* #include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "MQTTClient.h"

#define ADDRESS     "tcp://localhost:1883"
#define CLIENTID    "ExampleClientPub"
#define TOPIC       "MQTT Examples"
#define PAYLOAD     "Hello World!"
#define QOS         1
#define TIMEOUT     10000L

int main(int argc, char* argv[])
{
MQTTClient client;
MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
MQTTClient_message pubmsg = MQTTClient_message_initializer;
MQTTClient_deliveryToken token;
int rc;

MQTTClient_create(&client, ADDRESS, CLIENTID,
MQTTCLIENT_PERSISTENCE_NONE, NULL);
conn_opts.keepAliveInterval = 20;
conn_opts.cleansession = 1;

if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
{
printf("Failed to connect, return code %d\n", rc);
exit(-1);
}
pubmsg.payload = PAYLOAD;
pubmsg.payloadlen = strlen(PAYLOAD);
pubmsg.qos = QOS;
pubmsg.retained = 0;
MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
printf("Waiting for up to %d seconds for publication of %s\n"
"on topic %s for client with ClientID: %s\n",
(int)(TIMEOUT/1000), PAYLOAD, TOPIC, CLIENTID);
rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
printf("Message with delivery token %d delivered\n", token);
MQTTClient_disconnect(client, 10000);
MQTTClient_destroy(&client);
return rc;
} */
