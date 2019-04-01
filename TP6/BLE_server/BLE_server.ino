/*
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleServer.cpp
    Ported to Arduino ESP32 by Evandro Copercini
    updates by chegewara
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

/* Target Service ID */
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"

/* Target Char ID */
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");

  // set device name
  BLEDevice::init("kaid-zhuang");

  // init fonction : server
  BLEServer *pServer = BLEDevice::createServer();

  // init service with UUID
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // init characteristic with UUID and Property
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  /* Value of characteristic when a client read it 
   * The value can be changed by recall this function 
   */
  pCharacteristic->setValue("Hello World !");

  /* Start the service 
   * A device can have multiple service
   */
  pService->start();

  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  
  /* print adver setting*/
  /* Advertising allows devices to broadcast information defining their intentions. */
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}

void loop() {
  // put your main code here, to run repeatedly:
  
  delay(2000);
}
