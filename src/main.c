/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include "codec2.h"

void main(void)
{
	struct CODEC2 *c2;

	c2 = codec2_create(CODEC2_MODE_1200);
	// int nsam = codec2_samples_per_frame(c2);
	// int nbit = codec2_bits_per_frame(c2);
  	// int nbyte = (nbit + 7) / 8;

	printk("Hello World! %s\n", CONFIG_BOARD);
	// printk("nsam %d\n", nsam);
	// printk("nbit %d\n", nbit);
	// printk("nbyte %d\n", nbyte);
}
