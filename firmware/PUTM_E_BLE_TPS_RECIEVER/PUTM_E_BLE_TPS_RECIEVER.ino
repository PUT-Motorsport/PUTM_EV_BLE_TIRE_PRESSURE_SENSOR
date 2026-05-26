#include <BLEDevice.h>

#define S_UUID "c1e7cc92-bcfd-4657-b932-2464557514ae"
#define C_UUID "450c020a-39b5-4512-a1a7-2f203e11c5ce"

static boolean doConnect = false; 
static boolean connected = false;
static BLEAdvertisedDevice* myDevice;
BLERemoteCharacteristic* aRemoteCharacteristic;
BLEClient* aClient = NULL;
BLERemoteService* aRemoteService = NULL;
BLEScan* aBLEScan;

static void notifyCallback( //callbacki v2
  BLERemoteCharacteristic* aBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
    
    Serial.print("[");
    for (int i = 0; i < length; i++) {
      Serial.print((char)pData[i]);
    }
    Serial.print("]");
    Serial.println();
}

class clientCallback : public BLEClientCallbacks {  //po prostu sprawdza czy lapie
  void onConnect(BLEClient* client) {}
  void onDisconnect(BLEClient* client) {
    connected = false;
    Serial.println("connection lost");
  }
};

bool connectToServer() { //nie da sie bardziej obv nazwy dać
  aClient = BLEDevice::createClient();
  aClient->setClientCallbacks(new clientCallback());
  aClient->connect(myDevice);
  
  aRemoteService = aClient->getService(S_UUID);
  if (aRemoteService == nullptr) {
    aClient->disconnect();
    return false;
  }
  
  aRemoteCharacteristic = aRemoteService->getCharacteristic(C_UUID);
  if (aRemoteCharacteristic == nullptr) {
    aClient->disconnect();
    return false;
  }

  if(aRemoteCharacteristic->canNotify()) {
    aRemoteCharacteristic->registerForNotify(notifyCallback);
  }

  connected = true;
  return true;
}

class advertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(BLEUUID(S_UUID))) {
      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
    }
  }
};

void setup() {
  Serial.begin(115200);
  BLEDevice::init("apostle");
  
  aBLEScan = BLEDevice::getScan();
  aBLEScan->setAdvertisedDeviceCallbacks(new advertisedDeviceCallbacks());
  aBLEScan->setInterval(1349);
  aBLEScan->setWindow(449);
  aBLEScan->setActiveScan(true);
  aBLEScan->start(5, false);
}

void loop() {
  if (doConnect == true) {
    if (connectToServer()) {
      Serial.println("we hear");
    } else {
      Serial.println("");
    }
    doConnect = false;
  }
  delay(200);
}