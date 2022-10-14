#include "conct_runtime_high.h"

#include <nvs_flash.h>
#include <esp_event.h>

extern "C" void app_main(void)
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

	conct::RuntimeHigh runtime;

	while( true )
	{
		runtime.processPort( nullptr );

		vTaskDelay( configTICK_RATE_HZ / 100 ); // 10ms
	}
}