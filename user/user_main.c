#include "stdio.h"
#include "user_interface.h"
#include "osapi.h"
#include "user_config.h"
#include "mem.h"

#include "appdef.h"
#include "sdcard.h"
#include "ff.h"

#if ((SPI_FLASH_SIZE_MAP == 0) || (SPI_FLASH_SIZE_MAP == 1))
#error "The flash map is not supported"
#elif (SPI_FLASH_SIZE_MAP == 2)
#define SYSTEM_PARTITION_OTA_SIZE                           0x6A000
#define SYSTEM_PARTITION_OTA_2_ADDR                         0x81000
#define SYSTEM_PARTITION_RF_CAL_ADDR                        0xfb000
#define SYSTEM_PARTITION_PHY_DATA_ADDR                      0xfc000
#define SYSTEM_PARTITION_SYSTEM_PARAMETER_ADDR              0xfd000
#define SYSTEM_PARTITION_CUSTOMER_PRIV_PARAM_ADDR           0x7c000
#elif (SPI_FLASH_SIZE_MAP == 3)
#define SYSTEM_PARTITION_OTA_SIZE                           0x6A000
#define SYSTEM_PARTITION_OTA_2_ADDR                         0x81000
#define SYSTEM_PARTITION_RF_CAL_ADDR                        0x1fb000
#define SYSTEM_PARTITION_PHY_DATA_ADDR                      0x1fc000
#define SYSTEM_PARTITION_SYSTEM_PARAMETER_ADDR              0x1fd000
#define SYSTEM_PARTITION_CUSTOMER_PRIV_PARAM_ADDR           0x7c000
#elif (SPI_FLASH_SIZE_MAP == 4)
#define SYSTEM_PARTITION_OTA_SIZE                           0x6A000
#define SYSTEM_PARTITION_OTA_2_ADDR                         0x81000
#define SYSTEM_PARTITION_RF_CAL_ADDR                        0x3fb000
#define SYSTEM_PARTITION_PHY_DATA_ADDR                      0x3fc000
#define SYSTEM_PARTITION_SYSTEM_PARAMETER_ADDR              0x3fd000
#define SYSTEM_PARTITION_CUSTOMER_PRIV_PARAM_ADDR           0x7c000
#elif (SPI_FLASH_SIZE_MAP == 5)
#define SYSTEM_PARTITION_OTA_SIZE                           0x6A000
#define SYSTEM_PARTITION_OTA_2_ADDR                         0x101000
#define SYSTEM_PARTITION_RF_CAL_ADDR                        0x1fb000
#define SYSTEM_PARTITION_PHY_DATA_ADDR                      0x1fc000
#define SYSTEM_PARTITION_SYSTEM_PARAMETER_ADDR              0x1fd000
#define SYSTEM_PARTITION_CUSTOMER_PRIV_PARAM_ADDR           0xfc000
#elif (SPI_FLASH_SIZE_MAP == 6)
#define SYSTEM_PARTITION_OTA_SIZE                           0x6A000
#define SYSTEM_PARTITION_OTA_2_ADDR                         0x101000
#define SYSTEM_PARTITION_RF_CAL_ADDR                        0x3fb000
#define SYSTEM_PARTITION_PHY_DATA_ADDR                      0x3fc000
#define SYSTEM_PARTITION_SYSTEM_PARAMETER_ADDR              0x3fd000
#define SYSTEM_PARTITION_CUSTOMER_PRIV_PARAM_ADDR           0xfc000
#else
#error "The flash map is not supported"
#endif

#define SYSTEM_PARTITION_CUSTOMER_PRIV_PARAM                SYSTEM_PARTITION_CUSTOMER_BEGIN

uint32 priv_param_start_sec;


static const partition_item_t at_partition_table[] = {
    { SYSTEM_PARTITION_BOOTLOADER,          0x0,                                        0x1000},
    { SYSTEM_PARTITION_OTA_1,               0x1000,                                     SYSTEM_PARTITION_OTA_SIZE},
    { SYSTEM_PARTITION_OTA_2,               SYSTEM_PARTITION_OTA_2_ADDR,                SYSTEM_PARTITION_OTA_SIZE},
    { SYSTEM_PARTITION_RF_CAL,              SYSTEM_PARTITION_RF_CAL_ADDR,               0x1000},
    { SYSTEM_PARTITION_PHY_DATA,            SYSTEM_PARTITION_PHY_DATA_ADDR,             0x1000},
    { SYSTEM_PARTITION_SYSTEM_PARAMETER,    SYSTEM_PARTITION_SYSTEM_PARAMETER_ADDR,     0x3000},
    { SYSTEM_PARTITION_CUSTOMER_PRIV_PARAM, SYSTEM_PARTITION_CUSTOMER_PRIV_PARAM_ADDR,  0x1000},
};

void ICACHE_FLASH_ATTR user_pre_init(void)
{
    if(!system_partition_table_regist(at_partition_table, sizeof(at_partition_table)/sizeof(at_partition_table[0]),SPI_FLASH_SIZE_MAP)) {
        os_printf("system_partition_table_regist fail\r\n");
        while(1);
    }
}

void ICACHE_FLASH_ATTR print_directory(const char *path) {
    DIR dir;
    FILINFO Finfo;
    char buff[512];
    FIL fp;
//    size_t len;

    if (f_opendir(&dir, path) == FR_OK) {
        os_sprintf(buff, "Directory: %s\n\n", path);
        os_printf(buff);

        while (f_readdir(&dir, &Finfo) == FR_OK && Finfo.fname[0]) {
            if (!(Finfo.fattrib & AM_DIR)) {
                os_printf("main: %s", Finfo.fname);
                os_sprintf(buff, "%s/%s", path, Finfo.fname);
                if (f_open(&fp, buff, FA_READ) != FR_OK) {
                    os_printf("\tOpen file FAILED\n");
                } else {
                    os_printf("\tOpen file OK\n");
//                    if (strcmp(Finfo.fname, "scripts.js") == 0) {
//                        for (;;) {
//                            f_read(&fp, buff, sizeof(buff), &len);
//                            if (len == 0) break;
//                            for (int i = 0; i < len; i++) {
//                                os_printf("%c", buff[i]);
//                            }
//                        }
//                    }
                    f_close(&fp);
                }
            }
        }
        f_closedir(&dir);
    }

}


void ICACHE_FLASH_ATTR user_init(void) {

    FATFS FatFs;

	wifi_set_opmode(NULL_MODE);

	if (sd_init() == SD_CARD_INIT_OK) {
	    os_printf("sd card init ok\n");
	    f_mount(&FatFs, "", 0);

	    print_directory("/html");

	}


}

