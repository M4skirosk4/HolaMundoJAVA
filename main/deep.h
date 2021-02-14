/*Archivo que contiene las cabeceras de la funciones relativas al funcionamiento del
modo Deep Sleep, el cual por defecto esta establecido que funcione entre las 8:00 am
y las 10:00 pm*/
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "esp_sleep.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"
#include "esp_sntp.h"
#include "esp_timer.h"


/*A traves de esta funcion el ESP32 espera recibir una respuesta por parte del SNTP*/
void obtain_time(void);


/*Funcion para incializar el SNTP y obtener la hora local*/
void initialize_sntp(void);


/*Funcion de callback que sirve para notificar eventos por parte del SNTP*/
void time_sync_notification_cb(struct timeval *tv);


/*Callback del timer que cuenta el tiempo hasta que el ESP32 entra en Deep Sleep*/
void deep_sleep_periodic_timer_callback(void *pvParameters);


/*Funcion que establece el timer para entrar en modo Deep Sleep*/
void start_timer(void);