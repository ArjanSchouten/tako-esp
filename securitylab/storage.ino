 #include <EEPROM.h>

const int deviceID = 0;
void store(int addr, int val) {
    EEPROM.write(addr, val); 
 }

