/* MQTT (over TCP) Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

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
#include "esp_sleep.h"
#include "mqtt.h"
#include "cJSON.h"
#include "provisionamiento.h"
#include "esp_vfs_fat.h"
#include "server.h"
#include "deep.h"
#include "tareas.h"

#define LOG_VERBOSITY CONFIG_LOG_DEFAULT_LEVEL      //Verbosidad de los logs


static char* TAG = "main";

// Handle of the wear levelling library instance
static wl_handle_t s_wl_handle = WL_INVALID_HANDLE;

// Mount path for the partition
const char *base_path = "/spiflash";

//Puntero al fichero. Global para poder trabajar con él desde donde sea
FILE *f;


int log_vprintf ( const char * format, va_list arg ){


    return (vfprintf (f, format, arg));


}


//Función principal del código. Se ejecuta de forma infinita. No crea tareas
void sistema_particiones(){



    /**************** MONTAJE DE LA PARTICION *****************************************/

    ESP_LOGI(TAG, "Mounting FAT filesystem");
    const esp_vfs_fat_mount_config_t mount_config = {
            .max_files = 4,
            .format_if_mount_failed = true,
            .allocation_unit_size = CONFIG_WL_SECTOR_SIZE
    };

    esp_err_t err = esp_vfs_fat_spiflash_mount(base_path, "storage", &mount_config, &s_wl_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount FATFS (%s)", esp_err_to_name(err));
        return;
    }

    /*******************************************************************/



    //Se abre un fichero de texto en la partición montada en modo de escritura binaria
    f = fopen("/spiflash/log.txt", "wb");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return;
    }



    /*******************************************************************/



    //Con esta función redirigimos los logs a la función log_vprintf
    esp_log_level_set(TAG, LOG_VERBOSITY);
    //esp_log_set_vprintf(log_vprintf);

}


void app_main(void)
{

    #if CONFIG_PM_ENABLE
            esp_pm_config_esp32_t pm_config = {
                     //Frecuencia máxima a 240Hz
                    .max_freq_mhz = 240,    
                     //Frecuencia mínima a 40Hz
                    .min_freq_mhz = 40,
            #if CONFIG_FREERTOS_USE_TICKLESS_IDLE
                        //Establecemos Light Sleep automático
                        .light_sleep_enable = true
            #endif
                };
                ESP_ERROR_CHECK( esp_pm_configure(&pm_config) );
    #endif
                
    sistema_particiones();


    //Arranque WiFi driver y proceso provisionamiento. Configuración
    //en modo Station unicamente
    init_provisioning();

    //start_timer();

    //Iniciar el bus I2C como master
    ESP_ERROR_CHECK(i2c_master_init());
    //Comando init para iniciar la lectura. No se recoge el valor devuelto
    i2c_master_lecture_CO2(SGP30_HIGH, SGP30_INIT_LOW);

    //Se inicia el cliente MQTT
    mqtt_app_start();
    //Se inicia el servidor web
    start_webserver();

}
