/*******************************************************************************
 * Copyright (c) 2012, 2017 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution. 
 *
 * The Eclipse Public License is available at 
 *   http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at 
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial contribution
 *******************************************************************************/

#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MQTTClient.h"

#define ADDRESS     "tcp://localhost:1883"
#define CLIENTID    "RPI3_MQTT_SERV"
#define TOPIC       "inTopic6"
#define PAYLOAD     "Hello World!"
#define QOS         1
#define TIMEOUT     10000L

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
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char *server = "localhost";
    char *user = "root";
    char *password = "peri2019";
    char *database = "pperi";
    char query[1024];
    unsigned int sensor_id;
    float sensor_value;

    printf("Message arrived");
    if (message->payloadlen < 8) {
        printf("\n[E] Message has less than 8 bytes: IGNORED\n");
        goto release_mqtt;
    }
    printf("     in topic: %s\n", topicName);
    printf("   message: ");

    /*
    for(i=0; i<message->payloadlen; i++)
    {
        putchar(*payloadptr++);
    }
    putchar('\n');
    */
    
    /* TODO : register to database */

    /* Connect to database */
    
    conn = mysql_init(NULL);
    if (!mysql_real_connect(conn, server,
        user, password, database, 0, NULL, 0)) {

        fprintf(stderr, "%s\n", mysql_error(conn));
        goto release_mqtt;
    }

    /* prepare data */
    payloadptr = message->payload;


    sensor_id = *((int *) &payloadptr[0]);
    sensor_value = *((float *) &payloadptr[4]);

    printf("value = %f, capteur_id = %u\n", 
        sensor_value, sensor_id);

    /* send SQL query */
    
    sprintf(query, "INSERT INTO capteur_value (value, capteur_id) VALUES ('%f', '%u');", 
        sensor_value, sensor_id);
    printf("query : %s\n", query);
    if (mysql_query(conn, query)) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        goto release_connection;
    }

    /* IF NEED TO USE SELECT
    res = mysql_use_result(conn);

    if (!res) {
        fprintf(stderr, "result error\n");
        goto release_connection;   
    }
    //mysql_free_result(res);
    */



    /* close connection */
release_connection:
    mysql_close(conn);

release_mqtt:
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

void connlost(void *context, char *cause)
{
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}

int main(int argc, char* argv[])
{
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    int rc;
    int ch;

    MQTTClient_create(&client, ADDRESS, CLIENTID,
        MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }
    printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n"
           "Press Q<Enter> to quit\n\n", TOPIC, CLIENTID, QOS);
    MQTTClient_subscribe(client, TOPIC, QOS);

    do 
    {
        ch = getchar();
    } while(ch!='Q' && ch != 'q');

    MQTTClient_unsubscribe(client, TOPIC);
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    return rc;
}