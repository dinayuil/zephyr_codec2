/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/logging/log.h>

#include <errno.h>

#include "codec2.h"

#define AUDIO_BUF_U16_SIZE 24000
#define RX_BUF_SIZE 1000

/* Register Log */
LOG_MODULE_REGISTER(codec, LOG_LEVEL_DBG);

/* UART peripheral*/
/* UART2 TX 24, RX 23 */
#define UART2_DEVICE_NODE DT_NODELABEL(uart2)
static const struct device *uart2_dev = DEVICE_DT_GET(UART2_DEVICE_NODE);

/* UART1 TX 0, RX 1 */
#define UART1_DEVICE_NODE DT_NODELABEL(uart1)
static const struct device *uart1_dev = DEVICE_DT_GET(UART1_DEVICE_NODE);

/* receive buffer used in UART callback */
static uint8_t rx_buf1[RX_BUF_SIZE];
static uint8_t rx_buf2[RX_BUF_SIZE];
uint8_t inUseBuffer = 1;

/* Temporary audio buffer */
static uint16_t audio_buf[AUDIO_BUF_U16_SIZE];
static int audioBufPos = 0;

/* RX control */
volatile int rxMsgCount = 0;

/* TX control */
volatile bool txFinished = false;

void uart2_cb(const struct device *dev, struct uart_event *evt, void *user_data)
{
	switch(evt->type) {
	case UART_TX_DONE:
        LOG_INF("Tx sent %d bytes\n", evt->data.tx.len);
		txFinished = true;
        break;

    case UART_TX_ABORTED:
        LOG_INF("Tx aborted\n");
        break;

    case UART_RX_RDY:
        LOG_INF("Received data %d bytes\n", evt->data.rx.len);
		LOG_INF("buf address: %p\n", &(evt->data.rx.buf[0]));

		memcpy((uint8_t *)audio_buf + audioBufPos, evt->data.rx.buf, RX_BUF_SIZE);
		audioBufPos += RX_BUF_SIZE;

        break;
    case UART_RX_BUF_REQUEST:
		LOG_INF("Buffer request\n");
		if(inUseBuffer == 1) {
			uart_rx_buf_rsp(uart2_dev, rx_buf2, RX_BUF_SIZE);
			inUseBuffer = 2;
		} else {
			uart_rx_buf_rsp(uart2_dev, rx_buf1, RX_BUF_SIZE);
			inUseBuffer = 1;
		}
		rxMsgCount++;
		break;
    case UART_RX_BUF_RELEASED:
		LOG_INF("Buffer released\n");
		break;
    case UART_RX_DISABLED:
		LOG_INF("Buffer disabled\n");
		break;
    case UART_RX_STOPPED:
		LOG_INF("Buffer stopped\n");
        break;
	}
}

void uart1_cb(const struct device *dev, struct uart_event *evt, void *user_data)
{
	switch(evt->type) {
	case UART_TX_DONE:
        LOG_INF("UART1 sent %d bytes\n", evt->data.tx.len);
        break;

    case UART_TX_ABORTED:
        LOG_INF("Tx aborted\n");
        break;

    case UART_RX_RDY:
        LOG_INF("UART1 received data %d bytes\n", evt->data.rx.len);
		for(int i = 0; i < evt->data.rx.len; i++) {
			printk("%c", evt->data.rx.buf[i]);
		}
		printk("\n");
        break;
    case UART_RX_BUF_REQUEST:
		LOG_INF("Buffer request\n");

		break;
    case UART_RX_BUF_RELEASED:
		LOG_INF("Buffer released\n");
		break;
    case UART_RX_DISABLED:
		LOG_INF("Buffer disabled\n");
		break;
    case UART_RX_STOPPED:
		LOG_INF("Buffer stopped\n");
        break;
	}
}


void main(void)
{
	int err = 0;
	
	struct CODEC2 *c2;

	c2 = codec2_create(CODEC2_MODE_1200);
	int nsam = codec2_samples_per_frame(c2);
	int nbit = codec2_bits_per_frame(c2);
  	int nbyte = (nbit + 7) / 8;

	uint16_t audio_frame_buf[320];
	uint8_t encoded_audio_bits[nbyte];

	printk("nsam %d\n", nsam);
	printk("nbit %d\n", nbit);
	printk("nbyte %d\n", nbyte);

	if (!device_is_ready(uart2_dev)) {
		LOG_WRN("UART2 device not found!");
		return;
	}

	if (!device_is_ready(uart1_dev)) {
		LOG_WRN("UART1 device not found!");
		return;
	}

	LOG_DBG("&rx_buf1: %p\n", &rx_buf1);
	LOG_DBG("&rx_buf2: %p\n", &rx_buf2);

	/* configure interrupt and callback to receive data */
	err = uart_callback_set(uart2_dev, uart2_cb, NULL);
	if(err) {
		LOG_ERR("err from uart_callback_set for UART2, errno: %d.\n", err);
	}

	err = uart_callback_set(uart1_dev, uart1_cb, NULL);
	if(err) {
		LOG_ERR("err from uart_callback_set for UART1, errno: %d.\n", err);
	}

	// tx test
	char tx_buf[15];
	snprintk(tx_buf, 15, "Hello UART1!\n");
	err = uart_tx(uart1_dev, tx_buf, strlen(tx_buf), SYS_FOREVER_MS);
	if(err) {
		LOG_ERR("err from uart_tx, errno: %d.\n", err);
	}

	// rx test for UART1
	char rx_buf_uart1[10];
	err = uart_rx_enable(uart1_dev, rx_buf_uart1, 10, SYS_FOREVER_MS);
	if(err) {
		LOG_ERR("err from uart_rx_enable_u16, errno: %d.\n", err);
	}

	err = uart_rx_enable(uart2_dev, rx_buf1, RX_BUF_SIZE, SYS_FOREVER_MS);
	if(err) {
		LOG_ERR("err from uart_rx_enable_u16, errno: %d.\n", err);
	}

	while(1) {
		if(rxMsgCount == AUDIO_BUF_U16_SIZE/RX_BUF_SIZE * 2 + 1) {
			err = uart_rx_disable(uart2_dev);
			if(err) {
				LOG_ERR("err from uart_rx_disable, errno: %d.\n", err);
			}
			break;
		}
	}

	printk("Received.\n");
	printk("Start processing audio. \n");

	for(int i = 0; i < AUDIO_BUF_U16_SIZE; i+=320) {
		memcpy(audio_frame_buf, &audio_buf[i], 640);
		codec2_encode(c2, encoded_audio_bits, audio_frame_buf);
		// memset(audio_frame_buf, 0, 640);
		codec2_decode(c2, audio_frame_buf, encoded_audio_bits);
		memcpy(&audio_buf[i], audio_frame_buf, 640);
	}

	k_sleep(K_MSEC(1000));

	printk("Start tx.\n");
	for(int i = 0; i < 48000; i+=1000) {
		printk("i = %d\n", i);
		err = uart_tx(uart2_dev, (uint8_t *)audio_buf+i, 1000, SYS_FOREVER_MS);
		if(err) {
			LOG_ERR("err from uart_tx, errno: %d.\n", err);
		}
		while(1) {
			if(txFinished) {
				txFinished = false;
				break;
			}
		}
	}
	printk("tx finished.\n");
}
