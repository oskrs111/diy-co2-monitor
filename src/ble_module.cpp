/*
Copyright 2020 Oscar Sanz Llopis
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), 
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "ble_module.h"
static BLEServer *pServer = 0x00;
static BLEService *pService = 0x00;
static BLECharacteristic *pCharacteristic_ssid = 0x00;
static BLECharacteristic *pCharacteristic_pasw = 0x00;
static BLECharacteristic *pCharacteristic_reboot = 0x00;
static BLEAdvertising *pAdvertising = 0x00;

void ble_module_init()
{
  BLEDevice::init("diy-co2-monitor");
  pServer = BLEDevice::createServer();
  pService = pServer->createService(BLE_SERVICE_UUID);
  pCharacteristic_ssid = pService->createCharacteristic(BLE_CHARACTERISTIC_WSSID_UUID, BLECharacteristic::PROPERTY_READ |BLECharacteristic::PROPERTY_WRITE);
  pCharacteristic_ssid->setCallbacks(new bleCallback);
  pCharacteristic_ssid->setValue((uint8_t*)&config_module_get_preferences()->wifi.ssid[0], strlen(&config_module_get_preferences()->wifi.ssid[0]));

  pCharacteristic_pasw = pService->createCharacteristic(BLE_CHARACTERISTIC_WPASW_UUID, BLECharacteristic::PROPERTY_READ |BLECharacteristic::PROPERTY_WRITE);
  pCharacteristic_pasw->setCallbacks(new bleCallback);
  pCharacteristic_pasw->setValue((uint8_t*)&config_module_get_preferences()->wifi.password[0], strlen(&config_module_get_preferences()->wifi.password[0]));

  pCharacteristic_reboot = pService->createCharacteristic(BLE_CHARACTERISTIC_REBOOT_UUID, BLECharacteristic::PROPERTY_READ |BLECharacteristic::PROPERTY_WRITE);
  pCharacteristic_reboot->setCallbacks(new bleCallback);
  pCharacteristic_reboot->setValue("REBOOT");

  pCharacteristic_reboot = pService->createCharacteristic(BLE_CHARACTERISTIC_CALSEN_UUID, BLECharacteristic::PROPERTY_READ |BLECharacteristic::PROPERTY_WRITE);
  pCharacteristic_reboot->setCallbacks(new bleCallback);
  pCharacteristic_reboot->setValue("CALSEN");

  pService->start();  
  pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(BLE_SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("> ble_module_init() => Done!");
}
