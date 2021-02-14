/*Este archivo contiene las definiciones de las funciones que se 
van a utilizar a lo largo del proyecto relativas al uso y a la 
configuración del protocolo MQTT utilizado para realizar el envio de datos*/
#include "mqtt_client.h"


/*Esta funcion maneja todos los eventos relacionados con la gestion del
protocolo MQTT*/
esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event);



void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);



/*Funcion para configurar e iniciar el cliente MQTT*/
void mqtt_app_start();



//Funcion que se encarga de publicar el valor correspondiente en el topic
void publish(void *pvParameters);



//Esta funcion recibe el estado de los sensores para modificarlos
void sensors_state(bool *temperature, bool* co2);