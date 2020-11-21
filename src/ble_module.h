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
#ifndef BLE_MODULE_H
#define BLE_MODULE_H
#include <string.h>
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "config_module.h"

/**< https://www.uuidgenerator.net/ */
#define BLE_SERVICE_UUID        "fd3e6318-a9c8-4bf2-b1ee-0e68e2e1b097"
#define BLE_CHARACTERISTIC_WSSID_UUID "7b7e359a-f9b9-4c67-9a21-84830895d538"
#define BLE_CHARACTERISTIC_WPASW_UUID "179a0fd2-5cda-430f-afaa-f73acc8f96ad"
#define BLE_CHARACTERISTIC_REBOOT_UUID "9b7b7d45-0e57-475b-98ee-1fdd64dfb36a" /**< Writing to this characteristic will reboot the device */

class bleCallback: public BLECharacteristicCallbacks 
{
    void onWrite(BLECharacteristic *pCharacteristic) 
    {      
      if(pCharacteristic->getUUID().equals(BLEUUID(BLE_CHARACTERISTIC_WSSID_UUID)) == true)
      {
          config_module_set_parameter("wifi.ssid", (void*)pCharacteristic->getValue().c_str());
      }
    
      if(pCharacteristic->getUUID().equals(BLEUUID(BLE_CHARACTERISTIC_WPASW_UUID))  == true)
      {
          config_module_set_parameter("wifi.password", (void*)pCharacteristic->getValue().c_str());
      }

      if(pCharacteristic->getUUID().equals(BLEUUID(BLE_CHARACTERISTIC_REBOOT_UUID))  == true)
      {
          ESP.restart();
      }
    }

    void onRead(BLECharacteristic* pCharacteristic)
    {
      if(pCharacteristic->getUUID().equals(BLEUUID(BLE_CHARACTERISTIC_WSSID_UUID)) == true)
      {
          pCharacteristic->setValue((uint8_t*)&config_module_get_preferences()->wifi.ssid[0], strlen(&config_module_get_preferences()->wifi.ssid[0]));
      }
    
      if(pCharacteristic->getUUID().equals(BLEUUID(BLE_CHARACTERISTIC_WPASW_UUID))  == true)
      {
          pCharacteristic->setValue((uint8_t*)&config_module_get_preferences()->wifi.password[0], strlen(&config_module_get_preferences()->wifi.password[0]));
      }
    }
};
void ble_module_init();
#endif
