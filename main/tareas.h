/*Archivo que inlcuye las definiciones de las tareas usadas a lo 
largo del proyecto para hacer lecturas del sensor, tomar muestras,
realizar media movil con estas muestras y mandar los datos por MQTT*/

#define WINDOW_SIZE 5                          		//Tamaño de la ventana de valores para la media movil
#define SAMPLE_FREQ CONFIG_SAMPLE_FREQ              //Frecuencia de muestreo del sensor
#define SEND_FREQ 	CONFIG_SEND_FREQ              	//Frecuencia de muestreo del sensor
#define N_SAMPLES   3                          		//Cantidad de lecturas que conforman una muestra

#define TWDT_TIMEOUT_S          50					//Timeout del Watchdog



esp_timer_handle_t periodic_timer;					//Timer que monitoriza los sensores de CO2 y temperatura			
esp_timer_handle_t periodic_timer_sending;			//Timer que onitoriza la tarea que envia los datos fuera


typedef struct filter_out {							//Estructura de datos que sale de los filtros hacia el controlador 

	uint16_t media;									//Valor que se envia
	char id;										//ID de la medida
	
} filter_t;

typedef struct filter_args {						//Parametros de entrada que recibe la tarea filtro 

	QueueHandle_t Qin;								//Cola que lee de la tarea sensor correspondiente
	QueueHandle_t Qout;								//Cola en la que se escriben los datos para el controlador
	char id;										//ID de la medida
	
} filter_args_t;

typedef struct sensor_args {						//Parametros de entrada que recibe la tarea sensor 

	QueueHandle_t Qout;								//Cola en la que se escriben los datos para el controlador
	char id;										//ID del sensor. Sensor CO2 = 1 y sensor temperatura = 2
	
} sensor_args_t;

typedef struct controller_args {					//Parametros de entrada que recibe la tarea controlador 

	QueueHandle_t Qout;								
	QueueHandle_t Qin;								
	
} controller_args_t;

typedef struct data {						//Estructura de datos que el controlador envia por MQTT al broker 

	uint16_t dato_CO2;								//Se corresponde con un ID = 1
	uint16_t dato_temperatura;						//Se corresponde con un ID = 2
	
} data_out;



/*Funcion que crea las colas correspondientes para permitir la comunicacion entre tareas, los 
timers que monitorizan estas tareas y las propias tareas*/
void init_tasks();