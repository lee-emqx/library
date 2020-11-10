#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "MQTTClient.h"


typedef struct mqtt_opts {
	int			keep_alive;
	int			clean_session;
	// TODO
} mqtt_opts;

typedef struct mqtt_msg {
	char				*payload;
	int				qos;
	int				retain;
} mqtt_msg;

typedef struct mqtt_conf {
	char 				*addr; /* address */
	char 				*cid; /* client ID */
	char 				*topic;
	char 				*payload;
	int  				qos;
	long 				timeout;
} mqtt_conf;

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

int main(int argc, char* argv[])
{
	mqtt_opts m_opts = {20, 1};
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
}

