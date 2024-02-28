#include "esphome.h"

/* 
 The protocol according to the docs:
  - 0x20  = default address of  Chirp
  - reg. 0x00 =  read capacitance register
  - reg. 0x03 =  request light measurement 
  - reg. 0x04 =  read light register
  - reg. 0x05 =  read temperature register
  - reg. 0x10 =  read humidity in % format, custom code, needs a calibration function implemented in Chirp SW

 */


class Chirp : public PollingComponent, public Sensor {
 public:

  #define cirp_addr 0x20
  #define humidityRAW_reg 0x00
  #define humidity_reg 0x10
  #define calibStatus_reg 0x11
  #define _16bit 65536
  #define _10bit 1024
  #define _8bit 256
  #define _boolean 2
  #define _percent 100

  Sensor *humidity_RAW = new Sensor();
  Sensor *humidity = new Sensor();
  Sensor *calibStatus = new Sensor();

  Chirp() : PollingComponent(500) {}   // update each 500ms

  void setup() override {
    //Wire.begin();   // not needed because the core is automatically launching this one with pin definition.
  }


  void update() override {

    int _humidityRaw = readI2CRegister(cirp_addr, humidityRAW_reg, _16bit);
    int _humidity = readI2CRegister(cirp_addr, humidity_reg, _16bit);
    byte _calibStatus = readI2CRegister(cirp_addr, calibStatus_reg, _8bit);
      
    if(_humidityRaw < _10bit) humidity_RAW -> publish_state(_humidityRaw); 
    else{
      _humidityRaw = readI2CRegister(cirp_addr, humidityRAW_reg, _16bit);
      if(_humidityRaw < _10bit) humidity_RAW -> publish_state(_humidityRaw);
    }

    if(_humidity < _percent + 10) humidity -> publish_state(_humidity); 
    else{
      _humidity = readI2CRegister(cirp_addr, humidity_reg, _16bit);
      if(_humidityRaw < _percent + 10) humidity -> publish_state(_humidity);
    }

    if(_calibStatus < _boolean) calibStatus -> publish_state(_calibStatus); 
    else{
      _humidity = readI2CRegister(cirp_addr, calibStatus_reg, _8bit);
      if(_humidityRaw < _boolean) calibStatus -> publish_state(_calibStatus);
    }
  }

  unsigned int readI2CRegister(byte addr, byte reg, unsigned int dataLenght) {  // dataLenght = 8 or 16bit
    Wire.beginTransmission(addr);
    Wire.write(reg);
    Wire.endTransmission();
    delay(1);
    unsigned int t = 0;

    switch(dataLenght){
      case _8bit:
        Wire.requestFrom(addr, 1);
        t = Wire.read();
      break;

      case _16bit:
        Wire.requestFrom(addr, 2);
        t = Wire.read() << 8;
        t = t | Wire.read();
      break;
    }
  return t;
  }

};
