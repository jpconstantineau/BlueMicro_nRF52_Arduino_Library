// SPDX-FileCopyrightText: 2022 BlueMicro_Engine_Arduino_Library contributors (https://github.com/jpconstantineau/BlueMicro_Engine_Arduino_Library/graphs/contributors)
//
// SPDX-License-Identifier: BSD-3-Clause

#include "bluemicro_nrf52.h"

void setupGpio() 
{
      // this code enables the NFC pins to be GPIO.
     if ((NRF_UICR->NFCPINS & UICR_NFCPINS_PROTECT_Msk) == (UICR_NFCPINS_PROTECT_NFC << UICR_NFCPINS_PROTECT_Pos)){
       // Serial.println("Fix NFC pins");
        NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos;
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy);
        NRF_UICR->NFCPINS &= ~UICR_NFCPINS_PROTECT_Msk;
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy);
        NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos;
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy);
        //Serial.println("Done");
        delay(500);
        NVIC_SystemReset();
      } // end of NFC switch code.
}

void setupWDT()
{
  // Configure WDT
  NRF_WDT->CONFIG         = 0x00;             // stop WDT when sleeping
  NRF_WDT->CRV            = 5 * 32768 + 1;    // set timeout (5 sec)
  NRF_WDT->RREN           = 0x01;             // enable the RR[0] reload register
  NRF_WDT->TASKS_START    = 1;                // start WDT
}

void updateWDT()
{
  NRF_WDT->RR[0] = WDT_RR_RR_Reload;          // pet watchdog
}

/**************************************************************************************************************************/
uint32_t analogReadVDD()
{
  // thanks to vladkozlov69 on github.
  // from https://gist.github.com/vladkozlov69/2500a27cd93245d71573164cda789539

   // uint32_t pin = SAADC_CH_PSELP_PSELP_VDD;
    //uint32_t resolution;
    int16_t value;

    //resolution = 10;


    NRF_SAADC->RESOLUTION = SAADC_RESOLUTION_VAL_10bit;

    NRF_SAADC->ENABLE = (SAADC_ENABLE_ENABLE_Enabled << SAADC_ENABLE_ENABLE_Pos);
    for (int i = 0; i < 8; i++) 
    {
        NRF_SAADC->CH[i].PSELN = SAADC_CH_PSELP_PSELP_NC;
        NRF_SAADC->CH[i].PSELP = SAADC_CH_PSELP_PSELP_NC;
    }
    NRF_SAADC->CH[0].CONFIG =   ((SAADC_CH_CONFIG_RESP_Bypass     << SAADC_CH_CONFIG_RESP_Pos)   & SAADC_CH_CONFIG_RESP_Msk)
                                | ((SAADC_CH_CONFIG_RESP_Bypass     << SAADC_CH_CONFIG_RESN_Pos)   & SAADC_CH_CONFIG_RESN_Msk)
                                | ((SAADC_CH_CONFIG_GAIN_Gain1_6    << SAADC_CH_CONFIG_GAIN_Pos)   & SAADC_CH_CONFIG_GAIN_Msk)
                                | ((SAADC_CH_CONFIG_REFSEL_Internal << SAADC_CH_CONFIG_REFSEL_Pos) & SAADC_CH_CONFIG_REFSEL_Msk)
                                | ((SAADC_CH_CONFIG_TACQ_3us        << SAADC_CH_CONFIG_TACQ_Pos)   & SAADC_CH_CONFIG_TACQ_Msk)
                                | ((SAADC_CH_CONFIG_MODE_SE         << SAADC_CH_CONFIG_MODE_Pos)   & SAADC_CH_CONFIG_MODE_Msk);
    NRF_SAADC->CH[0].PSELN = SAADC_CH_PSELP_PSELP_VDD;
    NRF_SAADC->CH[0].PSELP = SAADC_CH_PSELP_PSELP_VDD;


    NRF_SAADC->RESULT.PTR = (uint32_t)&value;
    NRF_SAADC->RESULT.MAXCNT = 1; // One sample

    NRF_SAADC->TASKS_START = 0x01UL;

    while (!NRF_SAADC->EVENTS_STARTED);
    NRF_SAADC->EVENTS_STARTED = 0x00UL;

    NRF_SAADC->TASKS_SAMPLE = 0x01UL;

    while (!NRF_SAADC->EVENTS_END);
    NRF_SAADC->EVENTS_END = 0x00UL;
    NRF_SAADC->TASKS_STOP = 0x01UL;

    while (!NRF_SAADC->EVENTS_STOPPED);
    NRF_SAADC->EVENTS_STOPPED = 0x00UL;

    if (value < 0) 
    {
        value = 0;
    }

    NRF_SAADC->ENABLE = (SAADC_ENABLE_ENABLE_Disabled << SAADC_ENABLE_ENABLE_Pos);

    return value; 
}


void reboot(void)
{
    NVIC_SystemReset();
}

extern "C" void vApplicationIdleHook(void) {
    sd_power_mode_set(NRF_POWER_MODE_LOWPWR); // 944uA
    //sd_power_mode_set(NRF_POWER_MODE_CONSTLAT); // 1.5mA
    sd_app_evt_wait();  // puts the nrf52 to sleep when there is nothing to do.  You need this to reduce power consumption. (removing this will increase current to 8mA)
};
