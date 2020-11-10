#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MQTTClient.h"

typedef struct mqtt_opts {
	int			keep_alive;
	int			clean_session;
	// TODO
} mqtt_opts;

typedef struct mqtt_conf {
	char 				*addr; /* address */
	char 				*cid; /* client ID */
	char 				*topic;
	char 				*payload;
	int  				qos;
	long 				timeout;
} mqtt_conf;

volatile MQTTClient_deliveryToken deliveredtoken;

void delivered(void *context, MQTTClient_deliveryToken dt)
{
	printf("Message with token value %d delivery confirmed\n", dt);
	deliveredtoken = dt;
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
	int i;
	char* payloadptr;
	printf("Message arrived\n");
	printf("     topic: %s\n", topicName);
	printf("   message: ");
	payloadptr = message->payload;
	for(i=0; i<message->payloadlen; i++)
	{
		putchar(*payloadptr++);
	}
	putchar('\n');
	MQTTClient_freeMessage(&message);
	MQTTClient_free(topicName);
	return 1;
}

void connlost(void *context, char *cause)
{
	printf("\nConnection lost\n");
	printf("     cause: %s\n", cause);
}


void mqtt_set_opts(MQTTClient_connectOptions *opts, mqtt_opts *m_opts)
{ 	
	opts->keepAliveInterval = m_opts->keep_alive;
	opts->cleansession = m_opts->clean_session;
}


int mqtt_asyn_sub(mqtt_opts *m_opts, mqtt_conf *m_conf)
{
	MQTTClient client;
	MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
	mqtt_set_opts(&conn_opts, m_opts);
	int rc;
	int ch;

	MQTTClient_create(&client, m_conf->addr, m_conf->cid,
			MQTTCLIENT_PERSISTENCE_NONE, NULL);

	MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);
	if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
	{
		printf("Failed to connect, return code %d\n", rc);
		exit(EXIT_FAILURE);
	}
	printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n"
			"Press Q<Enter> to quit\n\n", m_conf->topic, m_conf->cid, m_conf->qos);

	MQTTClient_subscribe(client, m_conf->topic, m_conf->qos);

	do
	{
		ch = getchar();
	} while(ch!='Q' && ch != 'q');
	MQTTClient_disconnect(client, 10000);
	MQTTClient_destroy(&client);
	return rc;

}


int main(int argc, char* argv[])
{
	mqtt_opts m_opts = {20, 1};
	mqtt_conf m_conf = {
		"tcp://localhost:1883",
		"ExampleClientSub",
		"MQTT Examples",
		"Hello World!",
		1,
		10000L
	};

	return mqtt_asyn_sub(&m_opts, &m_conf);
}
