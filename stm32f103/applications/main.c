/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-06     SummerGift   first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include "fal.h"


int main(void)
{
#ifdef PKG_USING_FAL
    fal_init();
#endif /*BSK_USING_FAL_INIT*/
	
    return RT_EOK;
}
