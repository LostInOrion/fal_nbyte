/*
 * flash_free.c
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-12        hgl          first version
 *
 */

#include "fal_nbyte.h"


#define FLASH_CACHE        4096

// #define LOG_TAG     "fal_nbyte"
// #include "ulog.h"

static rt_mutex_t fal_nbyte_mtx;


int fal_partition_write_nbyte(const struct fal_partition *part, 
                              uint32_t addr, uint8_t *buf, size_t size)
{
    size_t i, remain, offset, _size;
    const struct fal_flash_dev * fal_dev = RT_NULL;
    uint8_t *fal_cache;
    
    fal_dev = fal_flash_device_find(part->flash_name);
    RT_ASSERT(fal_dev != RT_NULL);
    RT_ASSERT(FLASH_CACHE >= fal_dev->blk_size);
    RT_ASSERT(fal_dev != RT_NULL);

    rt_mutex_take(fal_nbyte_mtx, RT_WAITING_FOREVER);
    
    offset = addr%fal_dev->blk_size;
    remain = fal_dev->blk_size - offset;
    
    fal_cache = (uint8_t *)rt_malloc(fal_dev->blk_size);
    if (NULL == fal_cache)
        goto ERR;
        
    if (remain > size)
        remain = size;
     
    _size = size;
    while (1) {
        if (fal_partition_read(part, addr - offset, fal_cache, fal_dev->blk_size) != fal_dev->blk_size) {
            rt_kprintf("fal_partition_read err!\r\n");
            goto ERR1;
        }
		
        for (i = 0; i < remain; i++) {
            if (fal_cache[offset+i] != 0xff)
                break;
		}
        if (i != remain)
            fal_partition_erase(part, addr - offset, remain);
        
		rt_memcpy(&fal_cache[offset], buf, remain);
        if (fal_partition_write(part, addr - offset, fal_cache, fal_dev->blk_size) != fal_dev->blk_size) {
			rt_kprintf("fal_partition_write err!\r\n");
			goto ERR1;
		}

        if (remain == size) {
            break;
        } else {
            buf += remain;
            size -= remain;
			addr += remain;
            offset = 0;
            if (size > fal_dev->blk_size)
                remain = fal_dev->blk_size;
            else
                remain = size;
        }
    }
    rt_free(fal_cache);
    rt_mutex_release(fal_nbyte_mtx);
    return _size;
ERR1:
    rt_free(fal_cache);
ERR:
    rt_mutex_release(fal_nbyte_mtx);
    return -1;
}

int fal_partition_read_nbyte(const struct fal_partition *part, uint32_t addr, uint8_t *buf, size_t size)
{
    size_t remain, offset, index, _size;
    const struct fal_flash_dev * fal_dev = RT_NULL;
    uint8_t *fal_cache;
    
    fal_dev = fal_flash_device_find(part->flash_name);
    RT_ASSERT(fal_dev != RT_NULL);
    RT_ASSERT(FLASH_CACHE >= fal_dev->blk_size);
    RT_ASSERT(fal_dev != RT_NULL);

    rt_mutex_take(fal_nbyte_mtx, RT_WAITING_FOREVER);
    
    offset = addr%fal_dev->blk_size;
    remain = fal_dev->blk_size - offset;
    
    fal_cache = (uint8_t *)rt_malloc(fal_dev->blk_size);
    if (NULL == fal_cache)
        goto ERR;
        
    if (remain > size)
        remain = size;
     
    _size = size;
    index = 0;
    while (1) {
        if (fal_partition_read(part, addr - offset, fal_cache, fal_dev->blk_size) != fal_dev->blk_size) {
            rt_kprintf("fal_partition_read err!\r\n");
            goto ERR1;
        }
        
        rt_memcpy(buf, &fal_cache[offset], remain);
        if (remain == size) {
            break;
        } else {
			addr += remain;
            buf += remain;
            index += remain;
            size -= remain;
            offset = 0;
            if (size > fal_dev->blk_size)
                remain = fal_dev->blk_size;
            else
                remain = size;
        }
    }
    rt_free(fal_cache);
    rt_mutex_release(fal_nbyte_mtx);
    return _size;
ERR1:
    rt_free(fal_cache);
ERR:
    rt_mutex_release(fal_nbyte_mtx);
    return -1;
}

#include <finsh.h>
#include <stdio.h>
int fal_nbyte_wtest(int argc, char *argv[])
{
    int i;
    uint8_t buff[128];
	uint32_t addr, size;
	const struct fal_partition * cfg_fal = RT_NULL;
	
	if (argc != 3) {
		rt_kprintf("usage fal_nbyte_wtest addr size\r\n");
		rt_kprintf("eg: fal_nbyte_wtest 4080 128\r\n");
		
	} else {
		addr = atoi(argv[1]);	//Use MicroLib
		size = atoi(argv[2]);
		if (size > 128)
			size = 128;
		
		for (i = 0; i < size; ++i)
			buff[i] = i;
		
		cfg_fal = fal_partition_find("cfg");
		RT_ASSERT(cfg_fal != RT_NULL);
		
		rt_kprintf("fal cfg write! addr: 0x%x, size: %d\r\n", addr, size);
		 for (i = 0; i < size; i++) {
            if ((i != 0 && i % 16 == 0))
                rt_kprintf("\r\n");
			rt_kprintf("%02X ", buff[i]);
        }
		rt_kprintf("\r\n");
		fal_partition_write_nbyte(cfg_fal, addr, buff, size);
	}

    return 0;
}
MSH_CMD_EXPORT(fal_nbyte_wtest, fal nbyte write test.);

int fal_nbyte_rtest(int argc, char *argv[])
{
    int i;
    uint8_t buff[128];
	uint32_t addr, size;
	const struct fal_partition * cfg_fal = RT_NULL;
	
	if (argc != 3) {
		rt_kprintf("usage fal_nbyte_rtest addr size\r\n");
		rt_kprintf("eg: fal_nbyte_rtest 4080 128\r\n");
		
	} else {
		addr = atoi(argv[1]);	//Use MicroLib
		size = atoi(argv[2]);
		if (size > 128)
			size = 128;
		
		for (i = 0; i < size; ++i)
			buff[i] = i;
		
		cfg_fal = fal_partition_find("cfg");
		RT_ASSERT(cfg_fal != RT_NULL);
		

        fal_partition_read_nbyte(cfg_fal, addr, buff, size);
        rt_kprintf("fal cfg read. addr: 0x%x, size: %d\r\n", addr, size);
        for (i = 0; i < size; i++) {
            if ((i != 0 && i % 16 == 0))
                rt_kprintf("\r\n");
			rt_kprintf("%02X ", buff[i]);
        }
		rt_kprintf("\r\n");
	}
	
	

    return 0;
}
MSH_CMD_EXPORT(fal_nbyte_rtest, fal nbyte read test.);


int fal_nbyte_init(void)
{
	fal_nbyte_mtx = rt_mutex_create("fal_nbyte_mtx", RT_IPC_FLAG_FIFO);
	RT_ASSERT(fal_nbyte_mtx!=RT_NULL);
	return 0;
}
INIT_ENV_EXPORT(fal_nbyte_init);


int fal_nbyte_deinit(void)
{
	rt_mutex_delete(fal_nbyte_mtx);
	return 0;
}

