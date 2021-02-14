/*Este archivo recoge las APIs del servidor https que se ha implementado*/


//#include "/home/ubuntu/esp/esp-idf/components/esp_https_server/include/esp_https_server.h"
#include <esp_https_server.h>

/*Uri root que proporciona info general sobre el servidor*/
esp_err_t root_get_handler(httpd_req_t *req);


/*Uri que proporciona el estado de los sensores*/
esp_err_t sensor_get_handler(httpd_req_t *req);


/*Uri que cambia automaticamente el estado del sensor de temperatura*/
esp_err_t temperature_get_handler(httpd_req_t *req);


/*Uri que cambia automaticamente el estado del sensor de CO2*/
esp_err_t CO2_get_handler(httpd_req_t *req);


/*Esta funcion comienza el servidor web*/
httpd_handle_t start_webserver(void);


/*Esta funcion recibe eventos que indican que el servidor se ha desconectado*/
void disconnect_handler(void* arg, esp_event_base_t event_base, 
                               int32_t event_id, void* event_data);


/*Funcion que detiene el servidor web. Es llamada por la anterior*/
void stop_webserver(httpd_handle_t server);
