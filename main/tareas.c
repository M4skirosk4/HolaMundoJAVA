#include <stdio.h>
#include <stdlib.h>
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
#include "esp_timer.h"
#include "esp_pm.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "driver/i2c.h"
#include "sdkconfig.h"
#include "i2c.h"
#include "mqtt.h"
#include "esp_vfs_fat.h"
#include "tareas.h"
#include "esp_task_wdt.h"

static char* TAG = "task";

int contador_timer = 0;       						//Contador para monitorizar los sensores de CO2 y temperatura
int contador_timer_sending = 0;


//Coeficientes que se aplican al calculo de la ventana movil
static float coeficientes_ventana[] = {0.05, 0.10, 0.15, 0.25, 0.45};

//Coeficientes para calcular la media entre tres lecturas consecutivas
static float coeficientes_media[] = {1.0 / 3.0, 1.0 / 3.0, 1.0 / 3.0};

						
uint16_t media(uint16_t array[], int dimension, float coeficientes[]){

    float media = 0.0;

    for (int i = 0; i < dimension; i++){

        media += array[i] * coeficientes[i];
    }

    return ( (uint16_t) media);

}


void periodic_timer_callback()
{

    if (contador_timer == CONFIG_SAMPLE_FREQ )
        contador_timer = 0;

    else{
        contador_timer++;
    }
    
}

void periodic_timer_sending_callback()
{

    if (contador_timer_sending == CONFIG_SEND_FREQ )
        contador_timer_sending = 0;

    else{
        contador_timer_sending++;
    }
    
}


static void sensor(void *pvParameters){


	//Se suscribe la tarea al watchdog y despues se comprueba si se ha suscrito
	ESP_ERROR_CHECK(esp_task_wdt_add(NULL));
    ESP_ERROR_CHECK(esp_task_wdt_status(NULL));

    uint16_t lecturas[N_SAMPLES];
    uint16_t muestra;
    int lectura_index = 0;

    sensor_args_t *params = (sensor_args_t *) pvParameters;

    
    while (1) { 

    	//Resetear el watchdog
    	ESP_ERROR_CHECK(esp_task_wdt_reset());

        if(contador_timer == CONFIG_SAMPLE_FREQ){

        	//Si el ID = 1, entonces se trata del sensor de CO2
        	if ( params->id == 1){
        		lecturas[lectura_index] = i2c_master_lecture_CO2(SGP30_HIGH, SGP30_MEASURE_LOW);
        	}

        	//Si no, se trata del sensor de temperatura
        	else{
        		lecturas[lectura_index] = i2c_master_lecture_temperature(SI7021_READ_TEMP);
        	}

            lectura_index++;

            if(lectura_index == N_SAMPLES){

                lectura_index = 0;

                muestra = media(lecturas, N_SAMPLES, coeficientes_media);


                if( xQueueSendToBack( params->Qout, &muestra, 0 ) != pdTRUE ){

                    ESP_LOGE(TAG, "Failed to queued data to filter task from sensor");

                }

            }

            vTaskDelay(1000 / portTICK_PERIOD_MS);

        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
        
    }

}


static void filtro(void *pvParameters){


	//Se suscribe la tarea al watchdog y despues se comprueba si se ha suscrito
	ESP_ERROR_CHECK(esp_task_wdt_add(NULL));
    ESP_ERROR_CHECK(esp_task_wdt_status(NULL));

    uint16_t valor;
    bool average = false;
    uint16_t buffer_circular[WINDOW_SIZE];
    int write_index = 0;
    uint16_t media_mqtt = 0.0;
    

    //Estructura donde van los datos que se envian al controlador
    filter_t datos_enviados;

    filter_args_t *params = (filter_args_t *) pvParameters;

    datos_enviados.id = params->id;

    
    while (1) { 

    	//Resetear el watchdog
    	ESP_ERROR_CHECK(esp_task_wdt_reset());

        if( xQueueReceive( params->Qin, &valor, ( TickType_t ) 10 )  == pdTRUE  ){

            buffer_circular[write_index] = valor;
            write_index++;

            if(write_index == WINDOW_SIZE){
                write_index = 0;
                average = true;
            }

            if(average == true){

                media_mqtt = media(buffer_circular, WINDOW_SIZE, coeficientes_ventana);
                //ESP_LOGI(TAG, "Average of last four values: %d", media_mqtt);
                datos_enviados.media = media_mqtt;

                if( xQueueSendToBack( params->Qout, &datos_enviados, 0 ) != pdTRUE ){

                    ESP_LOGE(TAG, "Failed to queued data to controller task from filter");

                }
                

            }
        }    
    }
}


static void controlador(void *pvParameters){


	//Se suscribe la tarea al watchdog y despues se comprueba si se ha suscrito
	ESP_ERROR_CHECK(esp_task_wdt_add(NULL));
    ESP_ERROR_CHECK(esp_task_wdt_status(NULL));

	//Estructura donde se almacenan los datos que se van a enviar por MQTT
	data_out paquete_datos;

	//Estructura que contiene los datos enviados desde ls tareas filtro
	filter_t datos_recibidos;

	controller_args_t *params = (controller_args_t *) pvParameters;

	static uint8_t valores_recibidos = 0;

    while(1){

    	//Resetear el watchdog
    	ESP_ERROR_CHECK(esp_task_wdt_reset());

        if( xQueueReceive( params->Qin, &datos_recibidos, (TickType_t) 10 )  == pdTRUE ){

            if (datos_recibidos.id == 1){
            	paquete_datos.dato_CO2 = datos_recibidos.media;
            	valores_recibidos++;
            }
            
            else{
            	paquete_datos.dato_temperatura = datos_recibidos.media;
            	valores_recibidos++;
            }

            if (valores_recibidos == 2){
            	valores_recibidos = 0;

            	if( xQueueSendToBack( params->Qout, &paquete_datos, 0 ) != pdTRUE ){

                    ESP_LOGE(TAG, "Failed to queued data to sender task from controller");

                }
            }

        }

    }
}

static void sender(void *pvParameters){

	//Se suscribe la tarea al watchdog y despues se comprueba si se ha suscrito
	ESP_ERROR_CHECK(esp_task_wdt_add(NULL));
    ESP_ERROR_CHECK(esp_task_wdt_status(NULL));

    QueueHandle_t *cola = (QueueHandle_t *) pvParameters;

    data_out paquete_datos;

    while(1){

    	//Resetear el watchdog
    	ESP_ERROR_CHECK(esp_task_wdt_reset());

    	if(contador_timer_sending == CONFIG_SEND_FREQ){
	    	if( xQueueReceive( *cola, &paquete_datos, (TickType_t) 10 )  == pdTRUE )
	    		publish(&paquete_datos);
    	
    		}
		}

}


void init_tasks(){

	//Timer de los sensores junto con la funcion de callback
    const esp_timer_create_args_t periodic_timer_args = {
            //Funcion de callback del timer
            .callback = &periodic_timer_callback,
            .name = "periodic"
    };

    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    //Precarga del High Resolution Timer con 1 segundo
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, 1000000));

    //Timer de la tarea de envio junto con la funcion de callback
    const esp_timer_create_args_t periodic_timer_sending_args = {
            //Funcion de callback del timer
            .callback = &periodic_timer_sending_callback,
            .name = "periodic_sending"
    };

    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_sending_args, &periodic_timer_sending));
    //Precarga del High Resolution Timer con 1 segundo
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer_sending, 1000000));


    //Inicar la tarea del Watchdog habilitando la opcion de panic
    //en caso de que el watchdog llegue al timeout
    ESP_ERROR_CHECK(esp_task_wdt_init(TWDT_TIMEOUT_S, true));


    //Argumentos de las tareas filtro
    static filter_args_t f_temperature;
    static filter_args_t f_CO2;

    //Argumentos de las tareas sensor
    static sensor_args_t s_temperature;
    static sensor_args_t s_CO2;

    //Argumentos de la tarea controlador
    static controller_args_t controlador_args;

    //Sensor de CO2
    s_CO2.Qout = xQueueCreate(1, sizeof( uint16_t));
    s_CO2.id = 1;

    //Sensor de temperatura
    s_temperature.Qout = xQueueCreate(1, sizeof( uint16_t));
    s_temperature.id = 2;

    //Filtro de CO2
    f_CO2.Qin = s_CO2.Qout;
    f_CO2.Qout = xQueueCreate(2, sizeof( filter_t));;
    f_CO2.id = 1;

    //Filtro de temperatura
    f_temperature.Qin = s_temperature.Qout;
    f_temperature.Qout = f_CO2.Qout;
    f_temperature.id = 2;

    //Controlador
    int queue_size = 1;
    if(CONFIG_SEND_FREQ > (3 * CONFIG_SAMPLE_FREQ)){
    	queue_size = CONFIG_SEND_FREQ / (3 * CONFIG_SAMPLE_FREQ);
    }
    controlador_args.Qin = f_CO2.Qout;
    controlador_args.Qout = xQueueCreate(queue_size, sizeof( data_out));
    

    xTaskCreate(sensor, "sensor_temperature", 3072, &s_temperature, 3, NULL);
    xTaskCreate(filtro, "filtro_temperatura", 3072, &f_temperature, 2, NULL);
    xTaskCreate(sensor, "sensor_CO2", 3072, &s_CO2, 3, NULL);
    xTaskCreate(filtro, "filtro_CO2", 3072, &f_CO2, 2, NULL);
    xTaskCreate(controlador, "controlador", 3072, &controlador_args, 4, NULL);
    xTaskCreate(sender, "sender", 3072, &controlador_args.Qout, 4, NULL);
	
}