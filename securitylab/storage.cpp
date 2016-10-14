#include<EEPROM.h>
#include<Arduino.h>

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

    struct Configured {
      bool isDeviceConfigured;
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

    struct WifiStorage : StorageSettings<DeviceId> {
      const int start = 32;
      const int len = 128;
    };
    
    struct ConfiguredStorage : StorageSettings<Configured> {
      const int start = 32;
      const int len = 128;
    };

    void init() {
#ifdef STORAGEDEBUG
      Serial.print("[IFNO][storage] Max memory size for storage: ");
      Serial.println(MAX_MEMORY_SIZE);
#endif
      EEPROM.begin(MAX_MEMORY_SIZE);
    }

    template<typename T>
    bool save(struct StorageSettings<T> storage) {
#ifdef STORAGEDEBUG
      Serial.print("[INFO][storage] Saving at memory location: ");
      Serial.print(storage.start);
      Serial.print(" with max-length: ");
      Serial.println(storage.len);
#endif
      if (sizeof(storage.val) > storage.len) {
        Serial.println("[ERROR][storage] Size of value to save too large");
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
    const int MAX_MEMORY_SIZE = 512;
};

