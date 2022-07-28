/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include "codec2.h"

void main(void)
{
	codec2_create(1300);
	printk("Hello World! %s\n", CONFIG_BOARD);
}
