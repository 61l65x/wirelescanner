# Wirelesscanner

**Wirelesscanner** is a multi-threaded C program that scans Bluetooth and WiFi devices. It collects data from all nearby wireless devices by utilizing **all Bluetooth and WiFi interfaces** connected to the scanning device. This means the more interfaces available (Bluetooth or WiFi) and the more processing power your device has, the more data can be gathered, resulting in more powerful scanning capabilities and use cases.

- **Multi-threaded**: Handles Bluetooth and WiFi scanning simultaneously.
- **Device Compatibility**: Runs on any C-capable device (e.g., Raspberry Pi Zero).
- **Data Collection**: Gathers data from all nearby Bluetooth and WiFi devices using every available interface connected to the scanning device.
- **Data Transmission**: Sends collected data to a server.
- **ESP Implementation**: BLE device scanning on supported ESP boards (`platformio.ini`).



## WiFi Scanning
+ Arguments `"wifi"` to turn WiFi scan on!
+ Default: WiFi scan is off; only Bluetooth devices are scanned.

---

## Bluetooth Scanning Capabilities
- **BLE Scanning (Ready)**: Efficiently captures data from constantly advertising BLE devices, allowing rapid collection without explicit inquiries.
- **Bluetooth Classic (Ready)**: Scans classic Bluetooth devices using inquiry requests, which may result in slightly slower data collection compared to BLE.

## Features

### Thread 1: Bluetooth Device Scanner
- Scans and collects data from nearby Bluetooth devices.
- Continuously updates device information.

### Thread 2: WiFi Device Scanner
- Monitors WiFi signal strength using a small C/C++ library.
- Collects data from:
  - Associated station only (rapidly, connected).
  - All networks around (slow passive scan).
- Continuously scans and updates collected data.

### Thread 3: Dynamic Linked List Maintenance
- Manages a dynamic linked list of devices.
- Removes devices not seen for a certain period to prevent data clutter.

### Thread 4: Data Transmission to Server
- Sends collected data to a remote server via HTTP POST requests.

## Prerequisites
Before running Wirelesscanner, ensure you have:
- C Compiler
- Libcurl (for HTTP communication)
- Bluetooth hardware support
- [Additional dependencies as needed]

## Building and Running

To build and run **Wirelesscanner**, follow these steps:

1. Build the program:

```shell
make all
```

2. Run the program:

```shell
./wirelesscanner
```

For more details on the build process and customization, check the **Makefile**.

---

## Usage

To clean or rebuild the project, use:

```shell
make clean    # Clean build files
make re       # Clean and rebuild
```


## License

This project is licensed under the MIT License. You are free to use, modify, and distribute it, but cannot hold the authors liable for any issues. See the [LICENSE](assets/LICENSE) file for more details.
