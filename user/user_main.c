#include "osapi.h"
#include "user_interface.h"
#include "driver/uart.h"
#include "user_config.h"
#include "mem.h"

#include "appdef.h"
#include "sdcard.h"
#include "fs.h"

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
    DIR *dir;
    struct dirent *de;
    char buff[512];
    FILE *fp;
    size_t len;

    dir = opendir(path);
    if (dir) {
        os_sprintf(buff, "Directory: %s\n\n", path);
        os_printf(buff);

        while ((de = readdir(dir))) {
            if (!(de->finfo.fattrib & AM_DIR)) {
                os_printf("main: %s", de->d_name);
                os_sprintf(buff, "%s/%s", path, de->d_name);
                fp = fopen(buff, "r");
                if (fp == NULL) {
                    os_printf("\tOpen file FAILED\n");
                } else {
                    os_printf("\tOpen file OK\n");
//                    if (strcmp(de->d_name, "scripts.js") == 0) {
//                        for (;;) {
//                            len = fread(buff, sizeof(uint8_t), sizeof(buff), fp);
//                            if (len == 0) break;
//                            for (int i = 0; i < len; i++) {
//                                os_printf("%c", buff[i]);
//                            }
//                        }
//                    }
                    fclose(fp);
                }
            }
        }
        closedir(dir);
    }

}

void ICACHE_FLASH_ATTR copy_file() {

    char *name1 = "/html/scripts.js";
    char *name2 = "/html/scripts.js.copy";
    char buff[1024];
    size_t r_len, w_len;

    FILE *file1, *file2;

    file1 = fopen(name1, "r");
    if (file1 == NULL) {
        os_printf("Failed to open file: %s\n", name1);
        return;
    }

    file2 = fopen(name2, "w");
    if (file2 == NULL) {
        os_printf("Failed to create file: %s\n", name2);
        goto fail1;
    }

    do {
        r_len = fread(buff, sizeof(uint8_t), sizeof(buff), file1);
        if (r_len == 0) {
            os_printf("Failed to read from file: %s\n", name1);
            goto fail;
        }

        w_len = fwrite(buff, sizeof(uint8_t), r_len, file2);
        if (r_len != w_len) {
            os_printf("Failed to write into file: %s\n", name2);
            goto fail;
        }
    } while(r_len == sizeof(buff));
fail:
    fclose(file2);
fail1:
    fclose(file1);

    return;
}

void  user_init(void) {

    FATFS FatFs;

    uart_init(BIT_RATE_115200, BIT_RATE_115200);
    os_delay_us(65535);

	wifi_set_opmode(NULL_MODE);

	if (sd_init() == SD_CARD_OK) {
	    os_printf("sd card init ok\n");
	    f_mount(&FatFs, "", 0);
	    os_delay_us(65535);

	    copy_file();

	    print_directory("/html");

	    FATFS *fs;
        DWORD fre_clust, fre_sect, tot_sect;

        /* Get volume information and free clusters of drive 1 */
        if (f_getfree("", &fre_clust, &fs) != FR_OK) {
            os_printf("f_getfree return error\n");
            return;
        }

        /* Get total sectors and free sectors */
        tot_sect = (fs->n_fatent - 2) * fs->csize;
        fre_sect = fre_clust * fs->csize;

        /* Print the free space (assuming 512 bytes/sector) */
        os_printf("%10u KiB total drive space.\n%10u KiB available.\n", tot_sect / 2, fre_sect / 2);

	}


}

