/*Este archivo contiene implementada la funcionalidad de las funciones
utilizadas especificamente para manejar el bus I2C y llevar a cabo lecturas de datos*/

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
#include "esp_log.h"
#include "mqtt_client.h"
#include "cJSON.h"
#include "driver/i2c.h"
#include "sdkconfig.h"
#include "i2c.h"

static char* TAG = "i2c_master";


esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    i2c_param_config(i2c_master_port, &conf);
    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}


esp_err_t i2c_master_sensor_test_CO2(i2c_port_t i2c_num, uint8_t *data_h, 
										uint8_t *data_l, uint8_t addr_h, uint8_t addr_l)
{
    int ret;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd); 
    i2c_master_write_byte(cmd, SGP30_SENSOR_ADDR << 1 | I2C_MASTER_WRITE, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, addr_h, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, addr_l, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(i2c_num, cmd, I2C_TIMEOUT_MS / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    if (ret != ESP_OK) {
        return ret;
    }
    vTaskDelay(SENSOR_DELAY / portTICK_RATE_MS);


    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, SGP30_SENSOR_ADDR << 1 | I2C_MASTER_READ, ACK_CHECK_EN);
    i2c_master_read_byte(cmd, data_h, ACK_VAL);
    i2c_master_read_byte(cmd, data_l, NACK_VAL);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(i2c_num, cmd, I2C_TIMEOUT_MS / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}


uint16_t i2c_master_lecture_CO2(uint8_t addr_h, uint8_t addr_l)
{
    int ret;
    uint8_t sensor_data_h, sensor_data_l;
    static uint16_t bytes;

    ret = i2c_master_sensor_test_CO2(I2C_MASTER_NUM, &sensor_data_h, 
    						&sensor_data_l, addr_h, addr_l);

    if (ret == ESP_ERR_TIMEOUT) {

        ESP_LOGE(TAG, "I2C Timeout");

    } else if (ret == ESP_OK) {

        bytes = (sensor_data_h << 8 | sensor_data_l);

    } else {

        ESP_LOGW(TAG, "%s: No ack, sensor not connected...skip...", esp_err_to_name(ret));
    }

    
    return bytes;

}


esp_err_t i2c_master_sensor_test_temperature(i2c_port_t i2c_num, uint8_t *data_h, 
										uint8_t *data_l, uint8_t addr)
{
    int ret;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd); 
    i2c_master_write_byte(cmd, SI7021_SENSOR_ADDR << 1 | I2C_MASTER_WRITE, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, addr, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(i2c_num, cmd, I2C_TIMEOUT_MS / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    if (ret != ESP_OK) {
        return ret;
    }
    vTaskDelay(SENSOR_DELAY / portTICK_RATE_MS);


    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, SI7021_SENSOR_ADDR << 1 | I2C_MASTER_READ, ACK_CHECK_EN);
    i2c_master_read_byte(cmd, data_h, ACK_VAL);
    i2c_master_read_byte(cmd, data_l, NACK_VAL);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(i2c_num, cmd, I2C_TIMEOUT_MS / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}


uint16_t i2c_master_lecture_temperature(uint8_t addr)
{
    int ret;
    uint8_t sensor_data_h, sensor_data_l;
    static uint16_t bytes;

    ret = i2c_master_sensor_test_temperature(I2C_MASTER_NUM, &sensor_data_h, 
    						&sensor_data_l, addr);

    if (ret == ESP_ERR_TIMEOUT) {

        ESP_LOGE(TAG, "I2C Timeout");

    } else if (ret == ESP_OK) {

        bytes = ( ( ( 175.72 * (sensor_data_h << 8 | sensor_data_l) ) / 65536 ) - 46.85) * 100;

    } else {

        ESP_LOGW(TAG, "%s: No ack, sensor not connected...skip...", esp_err_to_name(ret));
    }

    
    return bytes;

}