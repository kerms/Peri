
# TP6: Communication radio

```
Yasma KADI
Kailun ZHUANG
```

### BLE
Le BLE est composé de 

  - plusieurs Service
  - plusieurs Charactéristique par service
  - plusieurs Description par caractéristique

Chacun possède un UUID qui le permet de lui identifier et accéder aux différents services.
L'`Advertising` permet aux devices de diffuser l'information qui définit leurs intentions.

[source](https://randomnerdtutorials.com/esp32-bluetooth-low-energy-ble-arduino-ide/ "BLE arduino")

### BLE SCAN

La demo se lance sans problème, et affiche les périphériques à proximité.

Pour faire un scan : 

    - Init BLEDevice
    - Créer le SCAN -> ::getScan()
    - Associer avec un callback qui serait appelé si un périphérique est découvert
    - Lancer le Scan

```cpp
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

int scanTime = 5; //In seconds
BLEScan* pBLEScan;

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {

    /* If found device */
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
    }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Scanning...");

  /* Init and set device name */
  BLEDevice::init("Hello");

  /* create new scan */
  pBLEScan = BLEDevice::getScan(); 

  /* Connect Advertising Callback*/
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());

  /* active scan uses more power, but get results faster */
  pBLEScan->setActiveScan(true);
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value
}

void loop() {
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  Serial.print("Devices found: ");
  Serial.println(foundDevices.getCount());
  Serial.println("Scan done!");
  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
  delay(2000);
}
```

### BLE Serveur

On a d'abord modifier le nom de notre ESP32 pour se reconnaître.
Puis après le téléversement, on peut le voir apparaître sur notre application `nRF connect`.
En envoyant les données on voie la donnée apparaître sur le Serial.

Pour lancer un Serveur : 

    - Préparer les UUIDs des Services et des Charactères
    - Init BLEDevice
    - Créer un serveur
    - Créer un service depuis un serveur (on peut créer plusieurs service) avec un UUID
    - Créer un characteristic depuis le service (on peut créer plusieurs char) avec un UUID
    - Maintenant on peut écrire des valeurs dans cette caracteristic
    - Lancer le service
    - Lancer un advertiser : pour se rendre visible par les autres

```cpp
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

/* Service ID */
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"

/* Char ID */
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
```

### BLE Client

On a modifié les UUIDs pour adapter au UUIDs de notre appli `nRF connect`.
Puis il faut s'assurer sur `nRF connect` que:

    - L'UUID du service correspond à celui entré sur la carte, i.e. characteristic
    - Advertising est open, sinon l'esp32 ne peut pas détecter notre portable.

Puis après le téléversement, l'esp32 se connecte bien sur `nRF connect`, on peut voir la donnée envoyée par l'esp32 dans notre caractéristique.

Pour Créer un Client : 

    - Préparer les deux UUID service et charactéristique de la cible.
    - Créer un Scan, et asscier à un callback pour ce scan
    - Dans le callback du scan, tester si le périphérique découvert est bien notre serveur
    - Sauvergarder le pointeur de cet périphérique
    - On est prêt à connecter au serveur
    - Créer le client
    - Associer un callback de BLEClient
    - Lancer la connection avec le pointeur sauvegardé
    - Vérifier si le service est disponible
    - Vérifier si le charactéristique est disponible
    - Vérifier les permissions du caractéristique
    - On peut à présent écrire / lire sur le caractéristique

###### Remarque
Sur un `HONOR v9`, si le téléphone est en veille, l'esp32 ne peut pas récupérer/envoyer des informations ou les données.
Les fonctions read/write sont bloquants, à un certain temps l'esp32 se déconnecte.
En rebootant on a quelques chances d'avoir des erreurs de type `assertion "heap != NULL && "free() target pointer is outside heap areas"" failed:`



```cpp
#include "BLEDevice.h"
//#include "BLEScan.h"

// The remote service we wish to connect to.
static BLEUUID serviceUUID("0000180A-0000-1000-8000-00805f9b34fb");
// The characteristic of the remote service we are interested in.
static BLEUUID    charUUID("00002A29-0000-1000-8000-00805f9b34fb");

/* Try to connect */
static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;

/* host characteristics */
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice; /* host device */

static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
    Serial.print("Notify callback for characteristic ");
    Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    Serial.print(" of data length ");
    Serial.println(length);
    Serial.print("data: ");
    Serial.println((char*)pData);
}

class MyClientCallback : public BLEClientCallbacks {

  /* Called when try to connect */
  void onConnect(BLEClient* pclient) {
    Serial.println("onConnect");
  }

  /* Called when disconnect */
  void onDisconnect(BLEClient* pclient) {
    connected = false;
    Serial.println("onDisconnect");
  }
};

bool connectToServer() {
    Serial.print("Forming a connection to ");
    
    /* Print host MAC addresse */
    Serial.println(myDevice->getAddress().toString().c_str());
    
    BLEClient*  pClient  = BLEDevice::createClient();
    Serial.println(" - Created client");

    /* link to our callback */
    pClient->setClientCallbacks(new MyClientCallback());

    // Connect to the remove BLE Server.
    pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
    Serial.println(" - Connected to server");

    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr) {
      Serial.print("Failed to find our service UUID: ");
      Serial.println(serviceUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our service");


    // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteCharacteristic == nullptr) {
      Serial.print("Failed to find our characteristic UUID: ");
      Serial.println(charUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our characteristic");

    // Read the value of the characteristic.
    if(pRemoteCharacteristic->canRead()) {
      std::string value = pRemoteCharacteristic->readValue();
      Serial.print("The characteristic value was: ");
      Serial.println(value.c_str());
    }

    if(pRemoteCharacteristic->canNotify())
      pRemoteCharacteristic->registerForNotify(notifyCallback);

    connected = true;
    return true;
}
/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
 /**
   * Called for each advertising BLE server.
   */
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());

    /* If target service UUID match, we can also check the name or addresse */
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {
      advertisedDevice.getScan()->stop(); //Scan can be stopped, we found what we are looking for
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;
    }
  } // onResult
}; // MyAdvertisedDeviceCallbacks


void setup() {
  Serial.begin(115200);
  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");

  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 5 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();

  /* Add callback */
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());

  /* active scan uses more power, but get results faster */
  pBLEScan->setActiveScan(true);
  pBLEScan->start(30);
} // End of setup.


// This is the Arduino main loop function.
void loop() {

  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are 
  // connected we set the connected flag to be true.
  if (doConnect == true) {
    if (connectToServer()) {
      Serial.println("We are now connected to the BLE Server.");
    } else {
      Serial.println("We have failed to connect to the server; there is nothin more we will do.");
    }
    doConnect = false;
  }

  // If we are connected to a peer BLE Server, update the characteristic each time we are reached
  // with the current time since boot.
  if (connected) {
    String newValue = "Time since boot: " + String(millis()/1000);
    Serial.println("Setting new characteristic value to \"" + newValue + "\"");
    
    // Set the characteristic's value to be the array of bytes that is actually a string.
    pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());
  }

  
  delay(1000); // Delay a second between loops.
} // End of loop
```