/*Este archivo contiene las definiciones de las funciones que se 
han utilizado para llevar a cabo el proceso de provisionamiento 
del modulo ESP32 via puerto serie. A tarves de este puerto, se le
proporcionan las credenciales del punto del acceso al que se quiere
conectar el ESP32, siendo esto necesario una unica vez, puesto que 
estos datos se almacenan en la memoria flash del ESP32*/


/*Funcion manejadora de los eventos WiFi que se produzcan*/
void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);


/*Funcion que se encarga de inicializar el driver WiFi y configurarlo
en modo Station con las credenciales que recibe como parametros. Ademas,
si la funcion no fuera capaz de llevar a cabo este proceso con exito, se
realizaria un borrado de la memoria flash para eliminar las credenciales 
y un reseteo del ESP32, para volver a iniciar el proceso de provisionamiento*/
void wifi_init_sta(char* ssid, char* password);


/*Esta funcion se encarga de verificar si el ESP32 ha sido provisionado ya 
anteriormente con las credenciales del punto de acceso. Si es asi, en el parametro
provisioned se escribe un True, escribiendose ademas estas credenciales en los 
parametros ssid y password  De lo contrario, se escribe un False.*/
esp_err_t app_prov_is_provisioned(bool *provisioned, char* ssid, char* password);


/*Funcion para provisionar via puerto serie al ESP32 con las credenciales del 
punto de acceso. Estas credenciales se escriben cada una por separado en el 
parametro array*/
void get_from_stdin(char* sentence, char* array);


/*Funcion para comenzar el proceso entero de provisionamiento e inicializacion
del driver del WiFi*/
void init_provisioning();