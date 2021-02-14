#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include "mqtt.h"
#include "cJSON.h"
#include "cbor.h"

#include "tareas.h"


#define NUM 3

static char* TAG = "mqtt client";

static char topic[80];

static bool temperatura = true;
static bool CO2 = true;

esp_mqtt_client_handle_t client;    //Cliente MQTT

void sensors_state(bool *temperature, bool* co2){

    temperatura = *temperature;
    CO2 = *co2;

}

void publish( void *pvParameters ){

    data_out *paquete = (data_out *) pvParameters;

    cJSON *root = cJSON_CreateObject();

    if(temperatura == true && CO2 == true){

        cJSON_AddNumberToObject(root, "CO2", paquete->dato_CO2);
        cJSON_AddNumberToObject(root, "Temperatura", paquete->dato_temperatura);
        cJSON_AddStringToObject(root, "Estado_CO2", "On");
        cJSON_AddStringToObject(root, "Estado_Temperatura", "On");
        const char *sys_info = cJSON_Print(root);
        esp_mqtt_client_publish(client, topic, sys_info, 0, 1, 0);
        free((void *)sys_info);
        cJSON_Delete(root);
        
}

    else if(temperatura == false && CO2 == true){

        cJSON_AddNumberToObject(root, "CO2", paquete->dato_CO2);
        cJSON_AddStringToObject(root, "Estado_CO2", "On");
        cJSON_AddStringToObject(root, "Estado_Temperatura", "Off");
        const char *sys_info = cJSON_Print(root);
        esp_mqtt_client_publish(client, topic, sys_info, 0, 1, 0);
        free((void *)sys_info);
        cJSON_Delete(root);
        
}

    else if(temperatura == true && CO2 == false){

        cJSON_AddNumberToObject(root, "Temperatura", paquete->dato_temperatura);
        cJSON_AddStringToObject(root, "Estado_CO2", "Off");
        cJSON_AddStringToObject(root, "Estado_Temperatura", "On");
        const char *sys_info = cJSON_Print(root);
        esp_mqtt_client_publish(client, topic, sys_info, 0, 1, 0);
        free((void *)sys_info);
        cJSON_Delete(root);
            
}

    else{
        const char *sys_info = cJSON_Print(root);
        cJSON_AddStringToObject(root, "Estado_CO2", "Off");
        cJSON_AddStringToObject(root, "Estado_Temperatura", "Off");
        esp_mqtt_client_publish(client, topic, sys_info, 0, 1, 0);
        free((void *)sys_info);
        cJSON_Delete(root);
    }
        

    /*
    char char_CO2[4];
    char char_temperatura[5];

    itoa(paquete->dato_CO2, char_CO2, 10);
    itoa(paquete->dato_temperatura, char_temperatura, 10);

    CborEncoder root_encoder;
    uint8_t buf[29];
    //Mapa
    CborEncoder map_encoder;
    // 1. Codificador CBOR.
    cbor_encoder_init(&root_encoder, buf, 29, 0);
    // 2. Creación del Mapa.
    cbor_encoder_create_map(&root_encoder, &map_encoder, 2); 
    // 3. Valor de CO2 y temperatura
    cbor_encode_text_stringz(&map_encoder, "CO2");
    cbor_encode_text_stringz(&map_encoder, cbor_CO2);
    cbor_encode_text_stringz(&map_encoder, "Temperatura");
    cbor_encode_text_stringz(&map_encoder, cbor_temperatura);
    // 4. Cerrar el Mapa
    cbor_encoder_close_container(&root_encoder, &map_encoder);
    esp_mqtt_client_publish(client, "/airquality/CO2", (char*)buf, 0, 1, 0);*/
    }




esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    static int contador = 0;
    // your_context_t *context = event->context;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            msg_id = esp_mqtt_client_subscribe(client, topic, 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            contador++;
            if(contador == NUM)
                esp_restart();
            break;
        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            //Una vez suscrito al topic, se crean las tareas para empezar a tomar medidas
            init_tasks();
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
    return ESP_OK;
}

void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    mqtt_event_handler_cb(event_data);
}

void mqtt_app_start()
{   
    strcat(topic, CONFIG_FACULTAD);
    strcat(topic, "/");
    strcat(topic, CONFIG_PISO);
    strcat(topic, "/");
    strcat(topic, CONFIG_AULA);


    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = CONFIG_BROKER_URL,
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);
}