# Wirelescanner
## Works on the raspberrys to !
Bluai is a C multi-threaded program designed to perform various Bluetooth-related tasks concurrently. It's divided into three main threads:

+ Arguments "wifi" to turn wifi scan on!
+ Default the wifi scan is off only scans bluetoothdevices

## Bluetooth Scanning Capabilities

- **BLE Scanning (Ready)**: Our scanner efficiently captures data from constantly advertising BLE devices, allowing for rapid collection of information without needing explicit inquiries.
- **Bluetooth Classic (Not Included yet)**: Scanning for classic Bluetooth devices involves sending inquiry requests, which may result in a slightly slower data collection process compared to BLE.


## Features

### Thread 1: Bluetooth Device Scanner

- This thread is responsible for scanning and collecting data from all nearby Bluetooth devices.
- It continuously scans for devices and updates the collected information.

### Thread 2: Wifi Device Scanner

wifi-scan small C/C++ library for monitoring signal strength of WiFi networks. It can collect data from:
- associated station only (rapidly) (Connected)
- all networks around (slow passive scan)

- This thread is resbonsible for scanning and collecting data from all nearby networks
- it continuosly passive scans nearby networks for networks & updates collected data

### Thread 3: Dynamic Linked List of Devices Maintenance

- This thread ensures that the dynamic linked list containing device information is kept clean.
- Devices that have not been seen for a certain period are removed from the list to prevent data clutter.

### Thread 4: Data Transmission to Server

- The third thread is responsible for sending collected data to a remote server.
- It constructs POST requests and sends data to the specified server endpoint.

## Prerequisites

Before running Bluai, make sure you have the following prerequisites:

- C Compiler
- Libcurl (for HTTP communication)
- Bluetooth hardware support
- [Additional dependencies as needed]

## Building and Running

To build and run Bluai, follow these steps:

1. [Instructions for building your program]

2. [Instructions for running your program]

## Configuration

Bluai can be configured via a configuration file or command-line arguments. Here are some common configuration options:

- [Option 1]
- [Option 2]
- [Option 3]

## Usage

Provide usage examples and command-line options for your program here.

```shell
$ make all      # Builds the executable 
$ make clean    # Cleans the project 
