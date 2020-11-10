#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mqtt_client.h"

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
	printf("%s", message->payload);
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

void mqtt_set_config(mqtt_conf *m_config, char *address, char *clientid, int qos, long timeout)
{
	/*
	 * TODO
	 * maybe we should get memmory to save data of config
	 */
	m_config->addr = address;
	m_config->cid = clientid;
	m_config->qos = qos;
	m_config->timeout = timeout;
}

void mqtt_set_msg(MQTTClient_message *msg, mqtt_msg *m_msg)
{

	msg->payload = m_msg->payload;
	msg->payloadlen = strlen(m_msg->payload);
	msg->qos = m_msg->qos;
	msg->retained = m_msg->retain;
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

int mqtt_syn_pub(mqtt_opts *m_opts, mqtt_conf *m_conf, mqtt_msg *m_msg)
{

	MQTTClient client;
	MQTTClient_connectOptions opts = MQTTClient_connectOptions_initializer;
	MQTTClient_message pub_msg = MQTTClient_message_initializer;
	MQTTClient_deliveryToken token;
	mqtt_set_opts(&opts, m_opts);	
	mqtt_set_msg(&pub_msg, m_msg);
	int rc;

	MQTTClient_create(&client, m_conf->addr, m_conf->cid,
			MQTTCLIENT_PERSISTENCE_NONE, NULL);

	if ((rc = MQTTClient_connect(client, &opts)) != MQTTCLIENT_SUCCESS)
	{
		printf("Failed to connect, return code %d\n", rc);
		exit(-1);
	}

	MQTTClient_publishMessage(client, m_conf->topic, &pub_msg, &token);

	printf("Waiting for up to %d seconds for publication of %s\n"
			"on topic %s for client with ClientID: %s\n",
			(int)(m_conf->timeout/1000), m_msg->payload, m_conf->topic, m_conf->cid);

	rc = MQTTClient_waitForCompletion(client, token, m_conf->timeout);
	printf("Message with delivery token %d delivered\n", token);

	MQTTClient_disconnect(client, 10000);
	MQTTClient_destroy(&(client));
	return rc;

}


void usage() 
{
	puts("usage: mqtt_client [pub/sub]");
}

int main(int argc, char* argv[])
{
	mqtt_opts m_opts = {20, 1};
	if (argc != 2) {
		usage();
		return -1;
	} else if (!strcmp("pub", argv[1])) {
		mqtt_conf m_conf = {
			"tcp://localhost:1883",
			"ExampleClientPub",
			"MQTT Examples",
			"Hello World!",
			1,
			10000L
		};

		mqtt_msg m_msg = {
			"Hello World!",
			1,
			0
		};

		return mqtt_syn_pub(&m_opts, &m_conf, &m_msg);

	} else if (!strcmp("sub", argv[1])) {
		mqtt_conf m_conf = {
			"tcp://localhost:1883",
			"ExampleClientSub",
			"MQTT Examples",
			"Hello World!",
			1,
			10000L
		};
   		return mqtt_asyn_sub(&m_opts, &m_conf);

	} else {
		usage();
		return -1;
	}

	return 0;
}
