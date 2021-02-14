#include <deep.h>

#define HOUR_BEGINNING CONFIG_END 
#define MINUTES_BEGINNING 0
#define SECONDS_BEGINNING 0

#define HOUR_END CONFIG_BEGINNING
#define MINUTES_END 0
#define SECONDS_END 0

static const char *TAG = "deep sleep";

//Timer para contar tiempo hasta en modo Deep Sleep
esp_timer_handle_t timer;

void time_sync_notification_cb(struct timeval *tv)
{
    ESP_LOGI(TAG, "Notification of a time synchronization event");
}

void deep_sleep_periodic_timer_callback(void *pvParameters)
{

    int *deep_sleep_sec = (int *) pvParameters;
    ESP_LOGW(TAG, "Entering deep sleep for the following %d seconds", *deep_sleep_sec);
    esp_deep_sleep(1000000LL * (*deep_sleep_sec));
    
}

void initialize_sntp()
{
    ESP_LOGI(TAG, "Initializing SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_set_time_sync_notification_cb(time_sync_notification_cb);
    sntp_init();
}

void obtain_time()
{

    initialize_sntp();

    // wait for time to be set
    time_t now = 0;
    struct tm timeinfo = { 0 };
    int retry = 0;
    const int retry_count = 10;
    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count) {
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    time(&now);
    localtime_r(&now, &timeinfo);
}



void start_timer()
{

    time_t now;
    struct tm timeinfo;
    struct tm time_beginnig;
    struct tm time_end;

    obtain_time();
    // update 'now' variable with current time
    time(&now);
    
    double seconds, seconds_diference, current_seconds,
    						beginning_seconds, end_seconds;

    //Static para que la variable no muera junto con esta
	//funcion y se pueda utilizar en el futuro
    static int precarga_timer;

    /*Set timezone to Spain Standard Time
    The following link contains most of the time zones: 
    https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv*/

    setenv("TZ", "CET-1CEST", 1);
    tzset();
    localtime_r(&now, &timeinfo);
    localtime_r(&now, &time_beginnig);
    localtime_r(&now, &time_end);

    //Se establece la hora en la que se entra al Deep Sleep
    time_end.tm_hour = HOUR_END;
    time_end.tm_min = MINUTES_END;
    time_end.tm_sec = SECONDS_END;

    //Se establece la hora en que se sale del Deep Sleep
    time_beginnig.tm_hour = HOUR_BEGINNING;
    time_beginnig.tm_min = MINUTES_BEGINNING;
    time_beginnig.tm_sec = SECONDS_BEGINNING;

    current_seconds = mktime(&timeinfo);
    end_seconds = mktime(&time_end);
    beginning_seconds = mktime(&time_beginnig);
    seconds = end_seconds - current_seconds;
    seconds_diference = end_seconds - beginning_seconds;


    if( seconds < 0 || seconds > seconds_diference){

    	if( seconds > seconds_diference ){
    		precarga_timer = beginning_seconds - current_seconds;
    		deep_sleep_periodic_timer_callback(&precarga_timer);
    	}

    	if (seconds < 0 ){

    		// 24h = 86400s
    		precarga_timer = ( 86400 - seconds_diference ) + seconds;
    		deep_sleep_periodic_timer_callback(&precarga_timer);
    	}
    }

    precarga_timer = 86400 - seconds_diference;
   
    /*Una vez contados los segundos hasta entrar en modo Deep Sleep, se
    pone un timer para contar este tiempo*/
    const esp_timer_create_args_t periodic_timer_args = {
            //Funcion de callback del timer
            .callback = &deep_sleep_periodic_timer_callback,
            .arg = &precarga_timer,
            .name = "periodic"
    };

    //Se crea el timer
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &timer));
    //Precarga del high resolution timer con los segundos restantes hasta Deep Sleep
    ESP_ERROR_CHECK(esp_timer_start_periodic(timer, seconds * 1000000));

}