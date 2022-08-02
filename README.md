# Codec 2 on Zephyr Proof of Concept Project

## How to use this code

The project is tested on Windows 10.
### Hardware
- Two [nRF9160 Thing Plus](https://www.sparkfun.com/products/17354) boards
- Two UART <-> USB adapters

### Software
- [串口调试助手 (Serial Debug Assistant)](https://www.microsoft.com/store/productId/9NBLGGH43HDM). Other serial port tools on PC are possible, but this tutorial uses the aforementioned tool.
- [HxD](https://mh-nexus.de/en/hxd/), a hex editor.
- [Audacity](https://www.audacityteam.org/), an audio process software.
- [nRF Connect for VS Code](https://nrfconnect.github.io/vscode-nrf-connect/index.html)

### Tutorial
Install the nRF Connect tools by following the [guide](https://nrfconnect.github.io/vscode-nrf-connect/connect/install.html). Import this project by the nRF Connect VS Code extension (https://nrfconnect.github.io/vscode-nrf-connect/connect/ui.html)

In `main.c` file, comment out one of ```TX_BOARD``` and ```RX_BOARD``` defines to make the board either a transmitter or a receiver (but of course, one board need to be tx and another one to be rx). Build and flash.  
![picture 1](images/1659435623057.png)  



## Reference
- [A slightly modified old version of Codec 2 for STM32F4](https://github.com/x893/codec2)
