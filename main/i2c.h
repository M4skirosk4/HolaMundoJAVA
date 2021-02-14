/*Este archivo contiene las definiciones de las funciones que se 
van a utilizar a lo largo del proyecto relativas al uso y a la 
configuración del bus I2C*/

#define _I2C_NUMBER(num) I2C_NUM_##num
#define I2C_NUMBER(num) _I2C_NUMBER(num)


#define DELAY_TIME_BETWEEN_ITEMS_MS 1000 					  /*!< delay time between different test items */


#define I2C_MASTER_SCL_IO CONFIG_I2C_MASTER_SCL               /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO CONFIG_I2C_MASTER_SDA               /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM I2C_NUMBER(CONFIG_I2C_MASTER_PORT_NUM) /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ CONFIG_I2C_MASTER_FREQUENCY        /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0                           /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0                           /*!< I2C master doesn't need buffer */

   
#define SGP30_SENSOR_ADDR 0x58								  /*Direccion de memoria del sensor SGP30*/
#define SGP30_HIGH 0x20    						              /*Direccion de memoria HIGH de los comandos init y measure*/
#define SGP30_MEASURE_LOW 0x08								  /*Direccion de memoria LOW del comando measure*/
#define SGP30_INIT_LOW 0x03									  /*Direccion de memoria LOW del comando init*/
#define SI7021_SENSOR_ADDR 0x40   							  /*Direccion de memoria del sensor SI7021*/
#define SI7021_READ_TEMP 0xF3    							  /*Direccion de memoria del comando de lectura del SI7021*/
#define SENSOR_DELAY 20


#define ACK_CHECK_EN 0x1                        			  /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0                       		   	  /*!< I2C master will not check ack from slave */
#define ACK_VAL 0x0                             			  /*!< I2C ack value */
#define NACK_VAL 0x1                            			  /*!< I2C nack value */
#define I2C_TIMEOUT_MS 1000


/*********************************************************************************************************/


/*Esta funcion sirve para llevar a cabo toda la configuacion
del bus I2C* tales como puertos a utilizar*/

esp_err_t i2c_master_init(void);




/*Esta funcion recibe como parametros un puerto y los dos valores 
de 1 byte donde se escribe la información leída por el sensor de 
CO2. Devuelve como parametro un valor que indica si la lectura se
ha podido llevar a cabo de forma satisfactoria o no*/

esp_err_t i2c_master_sensor_test_CO2(i2c_port_t i2c_num, uint8_t *data_h, 
							uint8_t *data_l, uint8_t addr_h, uint8_t addr_l);




/*Esta funcion llama a la anterior para realizar lecturas directamente
desde el sensor. Se encarga de recibir los datos leídos del sensor y 
convertirlos a un tipo de dato entero de 2 bytes, que es lo que devuelve*/

uint16_t i2c_master_lecture_CO2(uint8_t addr_h, uint8_t addr_l);




/*Esta funcion recibe como parametros un puerto y los dos valores 
de 1 byte donde se escribe la información leída por el sensor de 
CO2. Devuelve como parametro un valor que indica si la lectura se
ha podido llevar a cabo de forma satisfactoria o no*/

esp_err_t i2c_master_sensor_test_temperature(i2c_port_t i2c_num, uint8_t *data_h, 
							uint8_t *data_l, uint8_t addr);




/*Esta funcion llama a la anterior para realizar lecturas directamente
desde el sensor. Se encarga de recibir los datos leídos del sensor y 
convertirlos a un tipo de dato entero de 2 bytes, que es lo que devuelve*/

uint16_t i2c_master_lecture_temperature(uint8_t addr);