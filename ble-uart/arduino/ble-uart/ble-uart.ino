#include <bluefruit.h>

#define SERIAL_TX_BUFFER_SIZE 512
#define SERIAL_RX_BUFFER_SIZE 512
#define BAUDRATE 115200 //921600

// Device Name: Maximum 30 bytes
#define DEVICE_NAME "Takahiro BLE Serial Port"

// User service UUID: Change this to your generated service UUID
#define USER_SERVICE_UUID "9d019f5b-f367-4bcb-83aa-e8cae9854312"
// User service characteristics
#define WRITE_CHARACTERISTIC_UUID "c143b70a-3f7f-49f8-84c1-90b6c01eb6bb"
#define NOTIFY_CHARACTERISTIC_UUID "c0e78027-e4aa-4a17-9e3e-3f252a2e0856"

uint8_t userServiceUUID[16];
uint8_t writeCharacteristicUUID[16];
uint8_t notifyCharacteristicUUID[16];

BLEService userService;
BLECharacteristic notifyCharacteristic;
BLECharacteristic writeCharacteristic;

void setup()
{
  Serial.begin(BAUDRATE);
  while (!Serial)
    delay(10);
  Bluefruit.begin();
  Bluefruit.setName(DEVICE_NAME);

  setupServices();
  startAdvertising();
}

void loop()
{
  if (Serial.available())
  {
    char str[256] = {};
    for (int i = 0; i < 256; i++)
    {
      str[i] = '\0';
    }
    Serial.readBytes(str, 256);
    notifyCharacteristic.notify(&str, 256);
  }
}

void setupServices(void)
{
  // Convert String UUID to raw UUID bytes
  strUUID2Bytes(USER_SERVICE_UUID, userServiceUUID);
  strUUID2Bytes(WRITE_CHARACTERISTIC_UUID, writeCharacteristicUUID);
  strUUID2Bytes(NOTIFY_CHARACTERISTIC_UUID, notifyCharacteristicUUID);

  // Setup User Service
  userService = BLEService(userServiceUUID);
  userService.begin();

  writeCharacteristic = BLECharacteristic(writeCharacteristicUUID);
  writeCharacteristic.setProperties(CHR_PROPS_WRITE);
  writeCharacteristic.setWriteCallback(bleCallback);
  writeCharacteristic.setPermission(SECMODE_ENC_NO_MITM, SECMODE_ENC_NO_MITM);
  writeCharacteristic.setFixedLen(256);
  writeCharacteristic.begin();

  notifyCharacteristic = BLECharacteristic(notifyCharacteristicUUID);
  notifyCharacteristic.setProperties(CHR_PROPS_NOTIFY);
  notifyCharacteristic.setPermission(SECMODE_ENC_NO_MITM, SECMODE_NO_ACCESS);
  notifyCharacteristic.setFixedLen(256);
  notifyCharacteristic.begin();
}

void startAdvertising(void)
{
  // Start Advertising
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addService(userService);
  Bluefruit.ScanResponse.addName();
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.start(0);
}

void bleCallback(uint16_t conn_hdl, BLECharacteristic *chr, uint8_t *data, uint16_t len)
{
  return;
}

// UUID Converter
void strUUID2Bytes(String strUUID, uint8_t binUUID[])
{
  String hexString = String(strUUID);
  hexString.replace("-", "");

  for (int i = 16; i != 0; i--)
  {
    binUUID[i - 1] = hex2c(hexString[(16 - i) * 2], hexString[((16 - i) * 2) + 1]);
  }
}

char hex2c(char c1, char c2)
{
  return (nibble2c(c1) << 4) + nibble2c(c2);
}

char nibble2c(char c)
{
  if ((c >= '0') && (c <= '9'))
    return c - '0';
  if ((c >= 'A') && (c <= 'F'))
    return c + 10 - 'A';
  if ((c >= 'a') && (c <= 'f'))
    return c + 10 - 'a';
  return 0;
}
