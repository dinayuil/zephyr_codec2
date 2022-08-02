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

/* Choose role */
#define TX_BOARD
// #define RX_BOARD

/* Register Log */
LOG_MODULE_REGISTER(codec, LOG_LEVEL_DBG);

/* UART peripheral*/
/* UART2 TX 24 (TX), RX 23 (RX), used for audio transfer between board and PC*/
#define UART2_DEVICE_NODE DT_NODELABEL(uart2)
static const struct device *uart2_dev = DEVICE_DT_GET(UART2_DEVICE_NODE);

/* UART1 TX 0 (D4), RX 1 (D5), used for codec2 bits transfer between boards */
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
volatile bool uart1RxFinished = false;

/* TX control */
volatile bool uart2TxFinished = false;

void uart2_cb(const struct device *dev, struct uart_event *evt, void *user_data)
{
	switch(evt->type) {
	case UART_TX_DONE:
        LOG_INF("Tx sent %d bytes\n", evt->data.tx.len);
		uart2TxFinished = true;
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
        printk("UART1 sent %d bytes\n", evt->data.tx.len);
        break;

    case UART_TX_ABORTED:
        LOG_INF("Tx aborted\n");
        break;

    case UART_RX_RDY:
        printk("UART1 received data %d bytes\n", evt->data.rx.len);
		uart1RxFinished = true;
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
	
    /* Initialise Codec 2 */
	struct CODEC2 *c2;

	c2 = codec2_create(CODEC2_MODE_1200);
	int nsam = codec2_samples_per_frame(c2);
	int nbit = codec2_bits_per_frame(c2);
  	int nbyte = (nbit + 7) / 8;

    printk("==== Codec 2 config info ====\n");
	printk("number of PCM samples to encode for one frame (nsam): %d\n", nsam);
	printk("number of bits after encoding (nbit): %d\n", nbit);
	printk("number of bytes after encoding (nbyte) %d\n", nbyte);
    printk("=============================\n");

	int encodedSize = AUDIO_BUF_U16_SIZE/nsam * nbyte;
	uint8_t encodedAudio[encodedSize];

    /* Verify UART is ready*/
	if (!device_is_ready(uart2_dev)) {
		LOG_WRN("UART2 device not found!");
		return;
	}

	if (!device_is_ready(uart1_dev)) {
		LOG_WRN("UART1 device not found!");
		return;
	}

	/* Configure callback for UART tx and rx */
	err = uart_callback_set(uart2_dev, uart2_cb, NULL);
	if(err) {
		LOG_ERR("err from uart_callback_set for UART2, errno: %d.\n", err);
	}

	err = uart_callback_set(uart1_dev, uart1_cb, NULL);
	if(err) {
		LOG_ERR("err from uart_callback_set for UART1, errno: %d.\n", err);
	}

#ifdef TX_BOARD

	LOG_DBG("&rx_buf1: %p\n", &rx_buf1);
	LOG_DBG("&rx_buf2: %p\n", &rx_buf2);

    /* Receive original audio from PC */
	err = uart_rx_enable(uart2_dev, rx_buf1, RX_BUF_SIZE, SYS_FOREVER_MS);
	if(err) {
		LOG_ERR("err from uart_rx_enable for UART2, errno: %d.\n", err);
	}

    // receive complete, turn off UART2 rx
	while(1) {
		if(rxMsgCount == AUDIO_BUF_U16_SIZE/RX_BUF_SIZE * 2 + 1) {
			err = uart_rx_disable(uart2_dev);
			if(err) {
				LOG_ERR("err from uart_rx_disable for UART2, errno: %d.\n", err);
			}
			break;
		}
	}

	printk("Received.\n");

    /* Encode audio*/
	printk("Start encoding audio.\n");

    audioBufPos = 0;
	for(int i = 0; i < encodedSize; i+=nbyte) {
		codec2_encode(c2, &encodedAudio[i], &audio_buf[audioBufPos]);
        audioBufPos += nsam;
	}
    printk("Encoding finished.\n");

    /* Send codec2 bits to another board */
    uart_tx(uart1_dev, encodedAudio, encodedSize, SYS_FOREVER_MS);

#endif

#ifdef RX_BOARD

	/* Receive encoded audio bits */

	err = uart_rx_enable(uart1_dev, encodedAudio, encodedSize, SYS_FOREVER_MS);
    // wait until rx finished
	while(1) {
		if(uart1RxFinished) {
			break;
		}
	}
    
	/* Decode */
	audioBufPos = 0;
	for(int i = 0; i < encodedSize; i+=nbyte) {
		codec2_decode(c2, &audio_buf[audioBufPos], &encodedAudio[i]);
        audioBufPos += nsam;
	}

	k_sleep(K_MSEC(1000));

    /* Send decoded audio to PC */
	printk("Start tx.\n");
	for(int i = 0; i < AUDIO_BUF_U16_SIZE*2; i+=1000) {
		printk("i = %d\n", i);
		err = uart_tx(uart2_dev, (uint8_t *)audio_buf+i, 1000, SYS_FOREVER_MS);
		if(err) {
			LOG_ERR("err from uart_tx, errno: %d.\n", err);
		}
        // wait until this tx finished, then start next tx
		while(1) {
			if(uart2TxFinished) {
				uart2TxFinished = false;
				break;
			}
		}
	}
	printk("tx finished.\n");

#endif

}
