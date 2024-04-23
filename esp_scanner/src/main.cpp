#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>  // Include FreeRTOS semaphore support

SemaphoreHandle_t bleMutex;  // Declare a mutex
String lastBLEDeviceFound = "";

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    if (xSemaphoreTake(bleMutex, portMAX_DELAY)) {
      Serial.print("Device found: ");
      Serial.println(advertisedDevice.toString().c_str());
      lastBLEDeviceFound = advertisedDevice.toString().c_str();
      xSemaphoreGive(bleMutex);
    }
  }
};

void scanBLETask(void * parameter);

void setup() {
  Serial.begin(115200);
  bleMutex = xSemaphoreCreateMutex();
  Serial.println("Starting BLE scan...");
  BLEDevice::init(""); // Initialize BLE
  Serial.println("Scanning for BLE devices...");

  // Create a task for BLE scanning on core 0
  xTaskCreatePinnedToCore(scanBLETask, "BLEScan", 10000, NULL, 1, NULL, 0);
}

void loop() {
  if (xSemaphoreTake(bleMutex, portMAX_DELAY)) {  // Take the mutex
    // Perform other tasks here
    // Access and use the shared BLE data
    if (lastBLEDeviceFound != "") {
      Serial.println("Last BLE Device Found: " + lastBLEDeviceFound);
      lastBLEDeviceFound = "";  // Clear the variable
    }
    xSemaphoreGive(bleMutex);  // Give the mutex back
  }

  delay(1000); // Loop delay
}

void scanBLETask(void * parameter) {
  for (;;) {
    BLEScan* pBLEScan = BLEDevice::getScan();
    pBLEScan->setActiveScan(true);
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks(), true);
    pBLEScan->start(5, false); // Scan for 5 seconds
    pBLEScan->clearResults(); // Clear results to free memory
    vTaskDelay(500 / portTICK_PERIOD_MS); // Reduced delay between scans
  }
}
