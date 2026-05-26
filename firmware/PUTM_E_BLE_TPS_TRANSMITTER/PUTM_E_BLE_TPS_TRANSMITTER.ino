#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define S_UUID "c1e7cc92-bcfd-4657-b932-2464557514ae"
#define C_UUID "450c020a-39b5-4512-a1a7-2f203e11c5ce"

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
BLEAdvertising *pAdvertising = NULL;
uint8_t connected = 0;
uint32_t value = 0;

class serverCallbacks: public BLEServerCallbacks { // połączyło z jakimś urządzeniem: daje connected
    void onConnect(BLEServer* pServer) {
      connected++;
      BLEDevice::startAdvertising();
    };

    void onDisconnect(BLEServer* pServer) { // tak samo jak wyżej ale jak rozłączy, od razu nasłuchuje nowych
      if (connected > 0) {
        connected--;
      }
      BLEDevice::startAdvertising();
    }
};

void setup() {
  Serial.begin(115200);
  BLEDevice::init("Jesus"); //Nazwa do zmiany
  BLEServer *pServer = BLEDevice::createServer(); //Tworzenie serwera
  pServer->setCallbacks(new serverCallbacks());
  BLEService *pService = pServer->createService(S_UUID);
  pCharacteristic = pService->createCharacteristic(C_UUID,BLECharacteristic::PROPERTY_NOTIFY); //ustalenia co można robić 
  pService->start();

  pAdvertising = BLEDevice::getAdvertising(); //włączenie serwisu
  pAdvertising->addServiceUUID(S_UUID); //konfig
  pAdvertising->setScanResponse(true);  
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising(); //zaczyna nadawanie połączenia
}

void loop() {
  if (connected > 0) {
    String msg = String(value);
    pCharacteristic->setValue(msg.c_str());
    pCharacteristic->notify();
    value++;
  }
  delay(200);
}