#ifndef _H_FAL_NBYTE_H
#define _H_FAL_NBYTE_H


#include "fal.h"

int fal_nbyte_init(void);

int fal_nbyte_deinit(void);


int fal_partition_write_nbyte(const struct fal_partition *part, 
                              uint32_t addr, uint8_t *buf, size_t size);

int fal_partition_read_nbyte(const struct fal_partition *part, uint32_t addr, uint8_t *buf, size_t size);

#endif /*_H_FAL_NBYTE_H*/
