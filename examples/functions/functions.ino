#include <bluemicro_nrf52.h>
#include <Adafruit_TinyUSB.h> // for Serial

void setup() {
  Serial.begin(115200);
  while ( !Serial ) delay(10);   // for nrf52840 with native usb
  setupGpio();
  setupWDT();
}

void loop() {
  updateWDT();
  uint32_t vdd_mv =  analogReadVDD()*3600/1024; // returns a uint32_t value of the mV. 0.6V*6/10bits
  Serial.println(vdd_mv);
  //reboot();
}
