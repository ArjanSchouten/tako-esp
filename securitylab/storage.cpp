#include<EEPROM.h>

class Storage {
  public:
    struct DeviceId {
      char* deviceId;
    };

    struct WifiSettings {
      char* ssid;
      char* username;
      char* password;
    };

    template <typename T>
    struct StorageSettings {
      int start;
      int len;
      T val;
    };

    struct DeviceIdStorage : StorageSettings<DeviceId> {
      const int start = 0;
      const int len = 32;
    };

    struct WifiStorage : StorageSettings<WifiSettings> {
      const int start = 32;
      const int len = 128;
    };

    void init() {
      EEPROM.begin(maxMemorySize);
    }

    template<typename T>
    bool save(struct StorageSettings<T> storage) {
      if (sizeof(storage.val) > storage.len) {
        return false;
      }

      EEPROM.put(storage.start, storage.val);
      EEPROM.commit();

      return true;
    }

    template<typename T>
    T read(struct StorageSettings<T> storage) {
      EEPROM.get(storage.start, storage.val);
      
      return storage.val;
    }

  private:
    const int maxMemorySize = 512;
};









