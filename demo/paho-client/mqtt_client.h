# ifndef MQTT_CLIENT_H
# define MQTT_CLIENT_H
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


int mqtt_asyn_sub(mqtt_opts *m_opts, mqtt_conf *m_conf);
int mqtt_syn_pub(mqtt_opts *m_opts, mqtt_conf *m_conf, mqtt_msg *m_msg);

#endif
