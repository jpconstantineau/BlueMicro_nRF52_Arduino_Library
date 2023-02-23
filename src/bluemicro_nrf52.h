// SPDX-FileCopyrightText: 2022-2023 BlueMicro_nRF52_Arduino_Library contributors (https://github.com/jpconstantineau/BlueMicro_nRF52_Arduino_Library/graphs/contributors)
//
// SPDX-License-Identifier: BSD-3-Clause

#ifndef BLUEMICRO_NRF52_H
#define BLUEMICRO_NRF52_H
#include <Arduino.h>
#include <cstdint>
#include <vector>
#include <bluemicro_hid.h>


template < size_t N1,  size_t N2>
void sleep_C2R( const byte (&rows)[N1], const byte (&columns)[N2] ) {
  for(int j = 0; j < N1; ++j) {                             
    //set the current row as OUPUT and LOW
    pinMode(rows[j], OUTPUT);                                      
    digitalWrite(rows[j], LOW);                                       // 'enables' a specific row to be "low" 
  }
  //loops thru all of the columns
  for (int i = 0; i < N2; ++i) {                                       
        pinMode(columns[i], INPUT_PULLUP_SENSE);              // 'enables' the column High Value on the diode; becomes "LOW" when pressed - Sense makes it wake up when sleeping
  }
  sd_power_system_off();
}

template < size_t N1,  size_t N2>
void sleep_R2C( const byte (&rows)[N1], const byte (&columns)[N2] ) {
  for(int j = 0; j < N1; ++j) {                             
    //set the current row as OUPUT and LOW
    pinMode(rows[j], OUTPUT);
    digitalWrite(rows[j], HIGH);                                       // 'enables' a specific row to be "HIGH"
  }
  //loops thru all of the columns
  for (int i = 0; i < N2; ++i) {
        pinMode(columns[i], INPUT_PULLDOWN_SENSE);            // 'enables' the column High Value on the diode; becomes "LOW" when pressed - Sense makes it wake up when sleeping
  }
  sd_power_system_off();
}

/**************************************************************************************************************************/
/* These are compile time replacements for the scanMatrix functions and depends on DIODE_DIRECTION and nRF52832/nRF52840  */
/**************************************************************************************************************************/

  #define writeRow_C2R(r) digitalWrite(r,LOW)
  #define modeCol_C2R(c) pinMode(c, INPUT_PULLUP)
  #define writeRow_R2C(r) digitalWrite(r,HIGH)
  #define modeCol_R2C(c) pinMode(c, INPUT_PULLDOWN)
  #ifdef NRF52840_XXAA
    #define gpioIn_C2R (((uint64_t)(NRF_P1->IN)^0xffffffff)<<32)|(NRF_P0->IN)^0xffffffff
    #define gpioIn_R2C (((uint64_t)NRF_P1->IN)<<32)|(NRF_P0->IN)
    #define PINDATATYPE uint64_t
  #else
    #define gpioIn_C2R (NRF_GPIO->IN)^0xffffffff
    #define gpioIn_R2C NRF_GPIO->IN
    #define PINDATATYPE uint32_t
  #endif





/**************************************************************************************************************************/
template < size_t N1,  size_t N2>
trigger_keys_t scanMatrix_C2R(trigger_keys_t activeKeys,  const byte (&rows)[N1], const byte (&columns)[N2] )
{
    //bool has_key = false;
    PINDATATYPE pinreg = 0;
    activeKeys.clear();
    //Setting up Scanning, Enabling all columns
    for (int i = 0; i < N2; ++i){
        modeCol_C2R(columns[i]);
    } 

    // FIRST ROW
    pinMode(rows[0], OUTPUT);
    writeRow_C2R(rows[0]);
    nrfx_coredep_delay_us(1);   // need for the GPIO lines to settle down electrically before reading first row;
    // READ FIRST ROW
    pinreg = gpioIn_C2R;   // press is active high regardless of diode dir
    pinMode(rows[0], INPUT);                                        // 'disables' the row that was just scanned  

    /*************/

     for (int j = 1; j < N1; ++j){       
      // NEXT ROW
        pinMode(rows[j], OUTPUT);
        writeRow_C2R(rows[j]);
        // PROCESS PREVIOUS ROW - need for the GPIO lines to settle down electrically before reading NEXT row;
        for (int i = 0; i < N2; ++i){
            int ulPin = g_ADigitalPinMap[columns[i]]; 
            if((pinreg>>ulPin)&1)  
              {
                uint8_t keynumber = (j-1)*N2 + i;
                activeKeys.push_back(keynumber);
                //has_key = true;
              }
        }
        // READ NEXT ROW
        pinreg = gpioIn_C2R;  // press is active high regardless of diode dir
        pinMode(rows[j], INPUT); 
     }

    // PROCESS LAST ROW - Process 4th row right away...
    for (int i = 0; i < N2; ++i){
        int ulPin = g_ADigitalPinMap[columns[i]]; 
        if((pinreg>>ulPin)&1)  
          {
            uint8_t keynumber = (N1-1)*N2 + i;
            //Serial.println(keynumber);
            activeKeys.push_back(keynumber);
            //has_key = true;
          }
    }
    /*************/ 
    //Scanning done, disabling all columns - needed to save power
    for (int i = 0; i < N2; ++i) {                             
        pinMode(columns[i], INPUT);                                     
    }
    return activeKeys;
}

/**************************************************************************************************************************/
template < size_t N1,  size_t N2>
trigger_keys_t scanMatrix_R2C(trigger_keys_t activeKeys,  const byte (&rows)[N1], const byte (&columns)[N2] )
{
    //bool has_key = false;
    PINDATATYPE pinreg = 0;
    activeKeys.clear();
    //Setting up Scanning, Enabling all columns
    for (int i = 0; i < N2; ++i){
        modeCol_R2C(columns[i]);
    } 

    // FIRST ROW
    pinMode(rows[0], OUTPUT);
    writeRow_R2C(rows[0]);
    nrfx_coredep_delay_us(1);   // need for the GPIO lines to settle down electrically before reading first row;
    // READ FIRST ROW
    pinreg = gpioIn_R2C;   // press is active high regardless of diode dir
    pinMode(rows[0], INPUT);                                        // 'disables' the row that was just scanned  

    /*************/

     for (int j = 1; j < N1; ++j){       
      // NEXT ROW
        pinMode(rows[j], OUTPUT);
        writeRow_R2C(rows[j]);
        // PROCESS PREVIOUS ROW - need for the GPIO lines to settle down electrically before reading NEXT row;
        for (int i = 0; i < N2; ++i){
            int ulPin = g_ADigitalPinMap[columns[i]]; 
            if((pinreg>>ulPin)&1)  
              {
                uint8_t keynumber = (j-1)*N2 + i;
                Serial.println(keynumber);
                activeKeys.push_back(keynumber);
                //has_key = true;
              }
        }
        // READ NEXT ROW
        pinreg = gpioIn_R2C;  // press is active high regardless of diode dir
        pinMode(rows[j], INPUT); 
     }

    // PROCESS LAST ROW - Process 4th row right away...
    for (int i = 0; i < N2; ++i){
        int ulPin = g_ADigitalPinMap[columns[i]]; 
        if((pinreg>>ulPin)&1)  
          {
            uint8_t keynumber = (N1-1)*N2 + i;
            Serial.println(keynumber);
            activeKeys.push_back(keynumber);
            //has_key = true;
          }
    }
    /*************/ 
    //Scanning done, disabling all columns - needed to save power
    for (int i = 0; i < N2; ++i) {                             
        pinMode(columns[i], INPUT);                                     
    }
    return activeKeys;
}








void setupGpio(void);

void setupWDT(void);
void updateWDT(void);

void reboot(void);

uint32_t analogReadVDD(void);


#endif