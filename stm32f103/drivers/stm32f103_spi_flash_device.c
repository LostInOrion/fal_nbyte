#include <rtthread.h>
#include "drv_spi.h"
#include "spi_flash_sfud.h"


int spi_device_init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();

    rt_hw_spi_device_attach("spi1", "spi10", GPIOA, GPIO_PIN_4);
    rt_sfud_flash_probe("norflash0", "spi10");
    return 0;
}
INIT_DEVICE_EXPORT(spi_device_init);
