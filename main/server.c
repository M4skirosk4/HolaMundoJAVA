/* Simple HTTP + SSL Server Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include "esp_netif.h"
#include "esp_eth.h"
#include "protocol_examples_common.h"
#include "server.h"
#include "cJSON.h"
#include "mqtt.h"
#include "mqtt_client.h"



/* A simple example that demonstrates how to create GET and POST
 * handlers and start an HTTPS server.
*/

static char *TAG = "example";

static bool CO2 = true;                               //Estado del sensore de CO2. Por defecto, empieza funcionando
static bool temperature = true;                       //Estado del sensor de temperatura. Por defecto, empieza funcionando

/* An HTTP GET info/ handler */
esp_err_t root_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "application/json");
    cJSON *root = cJSON_CreateObject();
    cJSON *asignaturas = cJSON_CreateArray();
    char* nombre = "Jesus Rueda Huelamo";
    char* email = "jerued03@ucm.es";
    char* master = "MIOT";
    char* facultad = "Fdi UCM";
    cJSON *aniot = cJSON_CreateString("ANIOT");
    cJSON *rpi = cJSON_CreateString("RPI I");
    cJSON *rpii = cJSON_CreateString("RPI II");
    cJSON_AddStringToObject(root, "Student", nombre);
    cJSON_AddStringToObject(root, "Email", email);
    cJSON_AddStringToObject(root, "Master", master);
    cJSON_AddStringToObject(root, "Faculty", facultad);
    cJSON_AddItemToObject(root, "Subjects", asignaturas);
    cJSON_AddItemToArray(asignaturas, aniot);
    cJSON_AddItemToArray(asignaturas, rpi);
    cJSON_AddItemToArray(asignaturas, rpii);
    
    const char *sys_info = cJSON_Print(root);
    httpd_resp_sendstr(req, sys_info);
    free((void *)sys_info);
    cJSON_Delete(root);
    return ESP_OK;
}

esp_err_t sensor_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "application/json");
    cJSON *root = cJSON_CreateObject();

    cJSON_AddBoolToObject(root, "CO2", CO2);
    cJSON_AddBoolToObject(root, "Temperatura", temperature);

    const char *sys_info = cJSON_Print(root);
    httpd_resp_sendstr(req, sys_info);
    free((void *)sys_info);
    cJSON_Delete(root);
    return ESP_OK;
}


esp_err_t temperature_get_handler(httpd_req_t *req)
{
    
    temperature = !temperature;

    sensors_state(&temperature, &CO2);

    httpd_resp_sendstr(req, NULL);

    return ESP_OK;
}

esp_err_t CO2_get_handler(httpd_req_t *req)
{
    
    CO2 = !CO2;

    sensors_state(&temperature, &CO2);

    httpd_resp_sendstr(req, NULL);

    return ESP_OK;
}


httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server");

    httpd_ssl_config_t conf = HTTPD_SSL_CONFIG_DEFAULT();

    extern const unsigned char cacert_pem_start[] asm("_binary_cacert_pem_start");
    extern const unsigned char cacert_pem_end[]   asm("_binary_cacert_pem_end");
    conf.cacert_pem = cacert_pem_start;
    conf.cacert_len = cacert_pem_end - cacert_pem_start;

    extern const unsigned char prvtkey_pem_start[] asm("_binary_prvtkey_pem_start");
    extern const unsigned char prvtkey_pem_end[]   asm("_binary_prvtkey_pem_end");
    conf.prvtkey_pem = prvtkey_pem_start;
    conf.prvtkey_len = prvtkey_pem_end - prvtkey_pem_start;

    esp_err_t ret = httpd_ssl_start(&server, &conf);
    if (ESP_OK != ret) {
        ESP_LOGI(TAG, "Error starting server!");
        return NULL;
    }

    // Set URI handlers
    ESP_LOGI(TAG, "Registering URI handlers");

    httpd_uri_t root = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = root_get_handler
    };

    httpd_uri_t sensor = {
    .uri       = "/sensores",
    .method    = HTTP_GET,
    .handler   = sensor_get_handler
    };

    httpd_uri_t temperature = {
    .uri       = "/sensor/temperature",
    .method    = HTTP_GET,
    .handler   = temperature_get_handler
    };

    httpd_uri_t CO2 = {
    .uri       = "/sensor/CO2",
    .method    = HTTP_GET,
    .handler   = CO2_get_handler
    };
    

    httpd_register_uri_handler(server, &root);
    httpd_register_uri_handler(server, &sensor);
    httpd_register_uri_handler(server, &temperature);
    httpd_register_uri_handler(server, &CO2);
    
    return server;
}

void stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    httpd_ssl_stop(server);
}

void disconnect_handler(void* arg, esp_event_base_t event_base, 
                               int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server) {
        stop_webserver(*server);
        *server = NULL;
    }
}
