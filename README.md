# Codec 2 on Zephyr Proof of Concept Project

## How to use this code

The project is tested on Windows 10.
### Hardware
- Two [nRF9160 Thing Plus](https://www.sparkfun.com/products/17354) boards
- J-Link, for flashing application to the board
- Two UART <-> USB adapters

### Software
- [串口调试助手 (Serial Debug Assistant)](https://www.microsoft.com/store/productId/9NBLGGH43HDM). Other serial port tools on PC are possible, but this tutorial uses the aforementioned tool.
- [HxD](https://mh-nexus.de/en/hxd/), a hex editor.
- [Audacity](https://www.audacityteam.org/), an audio process software.
- [nRF Connect for VS Code](https://nrfconnect.github.io/vscode-nrf-connect/index.html)

### Tutorial
Install the nRF Connect tools by following the [guide](https://nrfconnect.github.io/vscode-nrf-connect/connect/install.html). Import this project by the nRF Connect VS Code extension (https://nrfconnect.github.io/vscode-nrf-connect/connect/ui.html)

#### Flash the Application
In `main.c` file, comment out one of ```TX_BOARD``` and ```RX_BOARD``` defines to make the board either a transmitter or a receiver (but of course, one board need to be tx and another one to be rx). Build and flash.  
![picture 1](images/1659435623057.png)  

#### Wiring
Each pin name printed on the nRF9160 board corresponds to a pin number, listed in the following figures. Pin number is used in the wiring tables.

![picture 3](images/1659447370342.png) 
![picture 2](images/1659447352381.png)  


|       |     Board    | UART <-> USB adapter |
|-------|--------------|----------------------|
| UART2 | TX: P0.24    | RX                   |
|       | RX: P0.23    | TX                   |


|       |     Board 1    |     Board 2    |
|-------|----------------|----------------|
| UART1 | TX: P0.00      | RX: P0.01      |
|       | RX: P0.01      | TX: P0.00      |

#### Transfer Raw Audio from PC to the TX Board


## Reference
- [A slightly modified old version of Codec 2 for STM32F4](https://github.com/x893/codec2)
- [nRF9160 Schematic](https://cdn.sparkfun.com/assets/5/7/c/a/c/nRF9160_Thing_Plus.pdf)
