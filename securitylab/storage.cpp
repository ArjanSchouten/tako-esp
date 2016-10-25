#include<EEPROM.h>
#include "Arduino.h"

class Storage {
  public:
    static const int DEVICEID = 2;
    static const int WIFI_SETTINGS_ID = 3;

    struct DeviceId {
      char* deviceId;
    };

    struct WifiSettings {
      char* ssid;
      char* password;
    };

    void init() {
      EEPROM.begin(maxMemorySize);
    }

    template<typename T>
    bool save(int id, T storage) {
      int addr;
      int size;
      switch (id) {
        case DEVICEID:
          addr = DEVICE_ID_START;
          size = DEVICE_ID_SIZE;
          break;
        case WIFI_SETTINGS_ID:
          addr = WIFI_SETTINGS_START;
          size = WIFI_SETTINGS_SIZE;
          break;
        default:
          return false;
          break;
      }

      Serial.print("Addr ");
      Serial.println(addr);

      Serial.print("Size ");
      Serial.println(size);
      
      EEPROM.put(addr, storage);
      EEPROM.commit();

      return true;
    }

    template<typename T>
    bool read(int id, T *storage) {
      int addr;
      int size;
      switch (id) {
        case DEVICEID:
          addr = DEVICE_ID_START;
          size = DEVICE_ID_SIZE;
          break;
        case WIFI_SETTINGS_ID:
          addr = WIFI_SETTINGS_START;
          size = WIFI_SETTINGS_SIZE;
          break;
        default:
          size = 0;
          addr = -1;
      }
      
      storage = EEPROM.get(addr, storage);

      return true;
    }

  private:
    const int maxMemorySize = 1024;
    static const int DEVICE_ID_START = 0;
    static const int DEVICE_ID_SIZE = 64;
    static const int WIFI_SETTINGS_START = 65;
    static const int WIFI_SETTINGS_SIZE = 128;
};









