/***************************************************************************//**
 * @file
 * @brief Energy Management Unit (EMU) peripheral API
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifndef EM_EMU_H
#define EM_EMU_H

#include "em_device.h"
#include "sl_status.h"
#if defined(EMU_PRESENT)

#include <stdbool.h>
#include "em_bus.h"
#include "sl_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup emu EMU - Energy Management Unit
 * @{
 ******************************************************************************/

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

#if (defined(_EMU_STATUS_VSCALE_MASK) || defined(_EMU_CTRL_EM23VSCALE_MASK)) \
  && !defined(_SILICON_LABS_GECKO_INTERNAL_SDID_200)
/** Voltage scaling present */
#define EMU_VSCALE_PRESENT
#if !defined(_SILICON_LABS_32B_SERIES_2_CONFIG_5)
/** Voltage scaling for EM01 present */
#define EMU_VSCALE_EM01_PRESENT
#endif
#endif

#if defined(_EMU_DCDCCTRL_MASK)
/** DC-DC buck converter present */
#define EMU_SERIES1_DCDC_BUCK_PRESENT
#endif

#if defined(_SILICON_LABS_DCDC_FEATURE)                                    \
  && ((_SILICON_LABS_DCDC_FEATURE == _SILICON_LABS_DCDC_FEATURE_DCDC_BUCK) \
  || (_SILICON_LABS_DCDC_FEATURE == _SILICON_LABS_DCDC_FEATURE_DCDC_BOB))
/** DC-DC buck converter present */
#define EMU_SERIES2_DCDC_BUCK_PRESENT
#endif

#if defined(_SILICON_LABS_DCDC_FEATURE)                                     \
  && ((_SILICON_LABS_DCDC_FEATURE == _SILICON_LABS_DCDC_FEATURE_DCDC_BOOST) \
  || (_SILICON_LABS_DCDC_FEATURE == _SILICON_LABS_DCDC_FEATURE_DCDC_BOB))
/** DC-DC boost converter present */
#define EMU_SERIES2_DCDC_BOOST_PRESENT
#endif

/*******************************************************************************
 ********************************   ENUMS   ************************************
 ******************************************************************************/

#if defined(_EMU_EM4CONF_OSC_MASK)
/** EM4 duty oscillator. */
typedef enum {
  /** Select ULFRCO as duty oscillator in EM4. */
  emuEM4Osc_ULFRCO = EMU_EM4CONF_OSC_ULFRCO,
  /** Select LFXO as duty oscillator in EM4. */
  emuEM4Osc_LFXO = EMU_EM4CONF_OSC_LFXO,
  /** Select LFRCO as duty oscillator in EM4. */
  emuEM4Osc_LFRCO = EMU_EM4CONF_OSC_LFRCO
} EMU_EM4Osc_TypeDef;
#endif

#if defined(_EMU_BUCTRL_PROBE_MASK)
/** Backup Power Voltage Probe types. */
typedef enum {
  /** Disable voltage probe. */
  emuProbe_Disable = EMU_BUCTRL_PROBE_DISABLE,
  /** Connect probe to VDD_DREG. */
  emuProbe_VDDDReg = EMU_BUCTRL_PROBE_VDDDREG,
  /** Connect probe to BU_IN. */
  emuProbe_BUIN    = EMU_BUCTRL_PROBE_BUIN,
  /** Connect probe to BU_OUT. */
  emuProbe_BUOUT   = EMU_BUCTRL_PROBE_BUOUT
} EMU_Probe_TypeDef;
#endif

#if defined(_EMU_PWRCONF_PWRRES_MASK)
/** Backup Power Domain resistor selection. */
typedef enum {
  /** Main power and backup power connected with RES0 series resistance. */
  emuRes_Res0 = EMU_PWRCONF_PWRRES_RES0,
  /** Main power and backup power connected with RES1 series resistance. */
  emuRes_Res1 = EMU_PWRCONF_PWRRES_RES1,
  /** Main power and backup power connected with RES2 series resistance. */
  emuRes_Res2 = EMU_PWRCONF_PWRRES_RES2,
  /** Main power and backup power connected with RES3 series resistance. */
  emuRes_Res3 = EMU_PWRCONF_PWRRES_RES3,
} EMU_Resistor_TypeDef;
#endif

#if defined(BU_PRESENT) && defined(_SILICON_LABS_32B_SERIES_0)
/** Backup Power Domain power connection. */
typedef enum {
  /** No connection between main and backup power. */
  emuPower_None = EMU_BUINACT_PWRCON_NONE,
  /** Main power and backup power connected through diode,
      allowing current from backup to main only. */
  emuPower_BUMain = EMU_BUINACT_PWRCON_BUMAIN,
  /** Main power and backup power connected through diode,
      allowing current from main to backup only. */
  emuPower_MainBU = EMU_BUINACT_PWRCON_MAINBU,
  /** Main power and backup power connected without diode. */
  emuPower_NoDiode = EMU_BUINACT_PWRCON_NODIODE,
} EMU_Power_TypeDef;
#endif

#if defined(_EMU_BUCTRL_BUINACTPWRCON_MASK)
/** Backup Power Domain power connection configuration when not in backup mode selection. */
typedef enum {
  /** No connection. */
  emuBuBuInactPwrCon_None    = EMU_BUCTRL_BUINACTPWRCON_NONE,
  /** Allow power from main to backup. */
  emuBuBuInactPwrCon_MainBu  = EMU_BUCTRL_BUINACTPWRCON_MAINBU,
  /** Allow power from backup to main. */
  emuBuBuInactPwrCon_BuMain  = EMU_BUCTRL_BUINACTPWRCON_BUMAIN,
  /** Backup and main power are connected. */
  emuBuBuInactPwrCon_NoDiode = EMU_BUCTRL_BUINACTPWRCON_NODIODE,
} EMU_BUBuInactPwrCon_TypeDef;
#endif

#if defined(_EMU_BUCTRL_BUACTPWRCON_MASK)
/** Backup Power Domain power connection configuration in backup mode selection. */
typedef enum {
  /** No connection. */
  emuBuBuActPwrCon_None    = EMU_BUCTRL_BUACTPWRCON_NONE,
  /** Allow power from main to backup. */
  emuBuBuActPwrCon_MainBu  = EMU_BUCTRL_BUACTPWRCON_MAINBU,
  /** Allow power from backup to main. */
  emuBuBuActPwrCon_BuMain  = EMU_BUCTRL_BUACTPWRCON_BUMAIN,
  /** Backup and main power are connected. */
  emuBuBuActPwrCon_NoDiode = EMU_BUCTRL_BUACTPWRCON_NODIODE,
} EMU_BUBuActPwrCon_TypeDef;
#endif

#if defined(_EMU_BUCTRL_PWRRES_MASK)
/** Backup Power Domain resistor selection. */
typedef enum {
  /** Main power and backup power connected with RES0 series resistance. */
  emuBuPwrRes_Res0 = EMU_BUCTRL_PWRRES_RES0,
  /** Main power and backup power connected with RES1 series resistance. */
  emuBuPwrRes_Res1 = EMU_BUCTRL_PWRRES_RES1,
  /** Main power and backup power connected with RES2 series resistance. */
  emuBuPwrRes_Res2 = EMU_BUCTRL_PWRRES_RES2,
  /** Main power and backup power connected with RES3 series resistance. */
  emuBuPwrRes_Res3 = EMU_BUCTRL_PWRRES_RES3,
} EMU_BUPwrRes_TypeDef;
#endif

#if defined(_EMU_BUCTRL_VOUTRES_MASK)
/** Resistance between backup domain power supply and BU_VOUT. */
typedef enum {
  /** BU_VOUT is not connected. */
  emuBuVoutRes_Dis = EMU_BUCTRL_VOUTRES_DIS,
  /** Enable weak switch. */
  emuBuVoutRes_Weak = EMU_BUCTRL_VOUTRES_WEAK,
  /** Enable medium switch. */
  emuBuVoutRes_Med = EMU_BUCTRL_VOUTRES_MED,
  /** Enable strong switch. */
  emuBuVoutRes_Strong = EMU_BUCTRL_VOUTRES_STRONG,
} EMU_BUVoutRes_TypeDef;
#endif

/** BOD threshold setting selector, active or inactive mode. */
typedef enum {
  /** Configure BOD threshold for active mode. */
  emuBODMode_Active,
  /** Configure BOD threshold for inactive mode. */
  emuBODMode_Inactive,
} EMU_BODMode_TypeDef;

/** EM4 modes. */
typedef enum {
  /** EM4 Shutoff. */
  emuEM4Shutoff   = 0,
  /** EM4 Hibernate. */
  emuEM4Hibernate = 1,
} EMU_EM4State_TypeDef;

#if defined(_EMU_EM4CTRL_EM4IORETMODE_MASK)
/** EM4 Pin Retention Type. */
typedef enum {
  /** No Retention: Pads enter reset state when entering EM4. */
  emuPinRetentionDisable = EMU_EM4CTRL_EM4IORETMODE_DISABLE,
  /** Retention through EM4: Pads enter reset state when exiting EM4. */
  emuPinRetentionEm4Exit = EMU_EM4CTRL_EM4IORETMODE_EM4EXIT,
  /** Retention through EM4 and wakeup: call @ref EMU_UnlatchPinRetention() to
      release pins from retention after EM4 wakeup. */
  emuPinRetentionLatch   = EMU_EM4CTRL_EM4IORETMODE_SWUNLATCH,
} EMU_EM4PinRetention_TypeDef;
#endif

#if defined(_EMU_CTRL_HDREGSTOPGEAR_MASK)
/** HDREG Stop Gear Max Current Type. */
typedef enum {
  /** HDREG current limit is 4mA. */
  emuHdregStopGearILmt4mA = _EMU_CTRL_HDREGSTOPGEAR_ILMT_4MA,
  /** HDREG current limit is 8mA. */
  emuHdregStopGearILmt8mA = _EMU_CTRL_HDREGSTOPGEAR_ILMT_8MA,
  /** HDREG current limit is 12mA. */
  emuHdregStopGearILmt12mA = _EMU_CTRL_HDREGSTOPGEAR_ILMT_12MA,
  /** HDREG current limit is 16mA. */
  emuHdregStopGearILmt16mA = _EMU_CTRL_HDREGSTOPGEAR_ILMT_16MA,
  /** HDREG current limit is 24mA. */
  emuHdregStopGearILmt24mA = _EMU_CTRL_HDREGSTOPGEAR_ILMT_24MA,
  /** HDREG current limit is 48mA. */
  emuHdregStopGearILmt48mA = _EMU_CTRL_HDREGSTOPGEAR_ILMT_48MA,
  /** HDREG current limit is 64mA. */
  emuHdregStopGearILmt64mA = _EMU_CTRL_HDREGSTOPGEAR_ILMT_64MA,
  /** HDREG current limit is 64mA. */
  emuHdregStopGearILmtMax = _EMU_CTRL_HDREGSTOPGEAR_ILMT_MAX,
} EMU_HdregStopGearILmt_TypeDef;
#endif

/** Power configurations. DCDC-to-DVDD is currently the only supported mode. */
typedef enum {
  /** DCDC is connected to DVDD. */
  emuPowerConfig_DcdcToDvdd,
} EMU_PowerConfig_TypeDef;

#if defined(EMU_SERIES1_DCDC_BUCK_PRESENT)
/** DCDC operating modes. */
typedef enum {
  /** DCDC regulator bypass. */
  emuDcdcMode_Bypass = EMU_DCDCCTRL_DCDCMODE_BYPASS,
  /** DCDC low-noise mode. */
  emuDcdcMode_LowNoise = EMU_DCDCCTRL_DCDCMODE_LOWNOISE,
#if defined(_EMU_DCDCLPEM01CFG_MASK)
  /** DCDC low-power mode. */
  emuDcdcMode_LowPower = EMU_DCDCCTRL_DCDCMODE_LOWPOWER,
#endif
} EMU_DcdcMode_TypeDef;

/** DCDC operating modes in EM2 or EM3. */
typedef enum {
  /** DCDC mode is low power. */
  emuDcdcModeEM23_LowPower = EMU_DCDCCTRL_DCDCMODEEM23_EM23LOWPOWER,
  /** DCDC mode is according to DCDCMODE field. */
  emuDcdcModeEM23_Sw = EMU_DCDCCTRL_DCDCMODEEM23_EM23SW,
} EMU_DcdcModeEM23_TypeDef;

#endif

#if defined(EMU_SERIES1_DCDC_BUCK_PRESENT)
/** DCDC conduction modes. */
typedef enum {
  /** DCDC Low-Noise Continuous Conduction Mode (CCM). EFR32 interference minimization
      features are available in this mode. */
  emuDcdcConductionMode_ContinuousLN,
  /** DCDC Low-Noise Discontinuous Conduction Mode (DCM). This mode should be used for EFM32 or
      for EFR32 when its radio is not enabled. */
  emuDcdcConductionMode_DiscontinuousLN,
} EMU_DcdcConductionMode_TypeDef;
#endif

#if defined(_EMU_PWRCTRL_MASK)
/** DCDC to DVDD mode analog peripheral power supply select. */
typedef enum {
  /** Select AVDD as analog power supply. Typically lower noise, but less energy efficient. */
  emuDcdcAnaPeripheralPower_AVDD = EMU_PWRCTRL_ANASW_AVDD,
  /** Select DCDC (DVDD) as analog power supply. Typically more energy efficient, but more noise. */
  emuDcdcAnaPeripheralPower_DCDC = EMU_PWRCTRL_ANASW_DVDD
} EMU_DcdcAnaPeripheralPower_TypeDef;
#endif

#if defined(_EMU_DCDCMISCCTRL_MASK)
/** DCDC Forced CCM and reverse current limiter control. Positive values have unit mA. */
typedef int16_t EMU_DcdcLnReverseCurrentControl_TypeDef;

/** High efficiency mode. EMU_DCDCZDETCTRL_ZDETILIMSEL is "don't care". */
#define emuDcdcLnHighEfficiency       -1

/** Default reverse current for fast transient response mode (low noise).  */
#define emuDcdcLnFastTransient         160
#endif

#if defined(EMU_SERIES1_DCDC_BUCK_PRESENT)
/** DCDC Low-noise RCO band select. */
typedef enum {
  /** Set RCO to 3MHz. */
  emuDcdcLnRcoBand_3MHz = 0,
  /** Set RCO to 4MHz. */
  emuDcdcLnRcoBand_4MHz = 1,
  /** Set RCO to 5MHz. */
  emuDcdcLnRcoBand_5MHz = 2,
  /** Set RCO to 6MHz. */
  emuDcdcLnRcoBand_6MHz = 3,
  /** Set RCO to 7MHz. */
  emuDcdcLnRcoBand_7MHz = 4,
  /** Set RCO to 8MHz. */
  emuDcdcLnRcoBand_8MHz = 5,
  /** Set RCO to 9MHz. */
  emuDcdcLnRcoBand_9MHz = 6,
  /** Set RCO to 10MHz. */
  emuDcdcLnRcoBand_10MHz = 7,
} EMU_DcdcLnRcoBand_TypeDef;

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
/* Deprecated. */
#define EMU_DcdcLnRcoBand_3MHz          emuDcdcLnRcoBand_3MHz
#define EMU_DcdcLnRcoBand_4MHz          emuDcdcLnRcoBand_4MHz
#define EMU_DcdcLnRcoBand_5MHz          emuDcdcLnRcoBand_5MHz
#define EMU_DcdcLnRcoBand_6MHz          emuDcdcLnRcoBand_6MHz
#define EMU_DcdcLnRcoBand_7MHz          emuDcdcLnRcoBand_7MHz
#define EMU_DcdcLnRcoBand_8MHz          emuDcdcLnRcoBand_8MHz
#define EMU_DcdcLnRcoBand_9MHz          emuDcdcLnRcoBand_9MHz
#define EMU_DcdcLnRcoBand_10MHz         emuDcdcLnRcoBand_10MHz
/** @endcond */

/** DCDC Low Noise Compensator Control register. */
typedef enum {
  /** DCDC capacitor is 1uF. */
  emuDcdcLnCompCtrl_1u0F,
  /** DCDC capacitor is 4.7uF. */
  emuDcdcLnCompCtrl_4u7F,
} EMU_DcdcLnCompCtrl_TypeDef;
#endif /* EMU_SERIES1_DCDC_BUCK_PRESENT */

#if defined(EMU_SERIES2_DCDC_BUCK_PRESENT) \
  || defined(EMU_SERIES2_DCDC_BOOST_PRESENT)

/** DCDC mode. */
typedef enum {
  emuDcdcMode_Bypass     = _DCDC_CTRL_MODE_BYPASS,            /**< DCDC regulator bypass. */
  emuDcdcMode_Regulation = _DCDC_CTRL_MODE_DCDCREGULATION     /**< DCDC regulator on.     */
} EMU_DcdcMode_TypeDef;
#endif

#if defined(EMU_SERIES2_DCDC_BUCK_PRESENT)
/** VREGIN comparator threshold. */
typedef enum {
  emuVreginCmpThreshold_2v0 = 0,          /**< Comparator threshold is 2.0V. */
  emuVreginCmpThreshold_2v1 = 1,          /**< Comparator threshold is 2.1V. */
  emuVreginCmpThreshold_2v2 = 2,          /**< Comparator threshold is 2.2V. */
  emuVreginCmpThreshold_2v3 = 3           /**< Comparator threshold is 2.3V. */
} EMU_VreginCmpThreshold_TypeDef;

/** DCDC Buck Ton max timeout. */
typedef enum {
#if (defined(_SILICON_LABS_32B_SERIES_2_CONFIG_2) \
  || defined(_SILICON_LABS_32B_SERIES_2_CONFIG_7) \
  || defined(_SILICON_LABS_32B_SERIES_2_CONFIG_9))
  emuDcdcTonMaxTimeout_Off    = _DCDC_CTRL_IPKTMAXCTRL_OFF,         /**< Ton max off.       */
  emuDcdcTonMaxTimeout_0P35us = _DCDC_CTRL_IPKTMAXCTRL_TMAX_0P35us, /**< Ton max is 0.35us. */
  emuDcdcTonMaxTimeout_0P63us = _DCDC_CTRL_IPKTMAXCTRL_TMAX_0P63us, /**< Ton max is 0.63us. */
  emuDcdcTonMaxTimeout_0P91us = _DCDC_CTRL_IPKTMAXCTRL_TMAX_0P91us, /**< Ton max is 0.91us. */
  emuDcdcTonMaxTimeout_1P19us = _DCDC_CTRL_IPKTMAXCTRL_TMAX_1P19us, /**< Ton max is 1.19us. */
  emuDcdcTonMaxTimeout_1P47us = _DCDC_CTRL_IPKTMAXCTRL_TMAX_1P47us, /**< Ton max is 1.47us. */
  emuDcdcTonMaxTimeout_1P75us = _DCDC_CTRL_IPKTMAXCTRL_TMAX_1P75us, /**< Ton max is 1.75us. */
  emuDcdcTonMaxTimeout_2P03us = _DCDC_CTRL_IPKTMAXCTRL_TMAX_2P03us  /**< Ton max is 2.03us. */
#else
  emuDcdcTonMaxTimeout_Off    = 0, /**< Ton max off.       */
  emuDcdcTonMaxTimeout_0P14us = 1, /**< Ton max is 0.14us. */
  emuDcdcTonMaxTimeout_0P21us = 2, /**< Ton max is 0.21us. */
  emuDcdcTonMaxTimeout_0P28us = 3, /**< Ton max is 0.28us. */
  emuDcdcTonMaxTimeout_0P35us = 4, /**< Ton max is 0.35us. */
  emuDcdcTonMaxTimeout_0P42us = 5, /**< Ton max is 0.42us. */
  emuDcdcTonMaxTimeout_0P49us = 6, /**< Ton max is 0.49us. */
  emuDcdcTonMaxTimeout_0P56us = 7, /**< Ton max is 0.56us. */
  emuDcdcTonMaxTimeout_0P63us = 8, /**< Ton max is 0.63us. */
  emuDcdcTonMaxTimeout_0P70us = 9, /**< Ton max is 0.70us. */
  emuDcdcTonMaxTimeout_0P77us = 10, /**< Ton max is 0.77us. */
  emuDcdcTonMaxTimeout_0P84us = 11, /**< Ton max is 0.84us. */
  emuDcdcTonMaxTimeout_0P91us = 12, /**< Ton max is 0.91us. */
  emuDcdcTonMaxTimeout_0P98us = 13, /**< Ton max is 0.98us. */
  emuDcdcTonMaxTimeout_1P05us = 14, /**< Ton max is 1.05us. */
  emuDcdcTonMaxTimeout_1P12us = 15, /**< Ton max is 1.12us. */
  emuDcdcTonMaxTimeout_1P19us = 16, /**< Ton max is 1.19us. */
  emuDcdcTonMaxTimeout_1P26us = 17, /**< Ton max is 1.26us. */
  emuDcdcTonMaxTimeout_1P33us = 18, /**< Ton max is 1.33us. */
  emuDcdcTonMaxTimeout_1P40us = 19, /**< Ton max is 1.40us. */
  emuDcdcTonMaxTimeout_1P47us = 20, /**< Ton max is 1.47us. */
  emuDcdcTonMaxTimeout_1P54us = 21, /**< Ton max is 1.54us. */
  emuDcdcTonMaxTimeout_1P61us = 22, /**< Ton max is 1.61us. */
  emuDcdcTonMaxTimeout_1P68us = 23, /**< Ton max is 1.68us. */
  emuDcdcTonMaxTimeout_1P75us = 24, /**< Ton max is 1.75us. */
  emuDcdcTonMaxTimeout_1P82us = 25, /**< Ton max is 1.82us. */
  emuDcdcTonMaxTimeout_1P89us = 26, /**< Ton max is 1.89us. */
  emuDcdcTonMaxTimeout_1P96us = 27, /**< Ton max is 1.96us. */
  emuDcdcTonMaxTimeout_2P03us = 28, /**< Ton max is 2.03us. */
  emuDcdcTonMaxTimeout_2P10us = 29, /**< Ton max is 2.10us. */
  emuDcdcTonMaxTimeout_2P17us = 30, /**< Ton max is 2.17us. */
  emuDcdcTonMaxTimeout_2P24us = 31  /**< Ton max is 2.24us. */
#endif
} EMU_DcdcTonMaxTimeout_TypeDef;

/** DCDC Buck drive speed. */
typedef enum {
  emuDcdcDriveSpeed_BestEmi        = _DCDC_EM01CTRL0_DRVSPEED_DEFAULT_SETTING,        /**< Recommend no options other than DEFAULT be used here, as there is no benefit. */
  emuDcdcDriveSpeed_Default        = _DCDC_EM01CTRL0_DRVSPEED_DEFAULT_SETTING,        /**< Recommend no options other than DEFAULT be used here, as there is no benefit. */
  emuDcdcDriveSpeed_Intermediate   = _DCDC_EM01CTRL0_DRVSPEED_DEFAULT_SETTING,        /**< Recommend no options other than DEFAULT be used here, as there is no benefit. */
  emuDcdcDriveSpeed_BestEfficiency = _DCDC_EM01CTRL0_DRVSPEED_DEFAULT_SETTING         /**< Recommend no options other than DEFAULT be used here, as there is no benefit. */
} EMU_DcdcDriveSpeed_TypeDef;

/** DCDC Buck peak current setting. */
typedef enum {
#if defined(_DCDC_EM23CTRL0_IPKVAL_Load5mA)
  emuDcdcPeakCurrent_Load5mA  = _DCDC_EM23CTRL0_IPKVAL_Load5mA,  /**< Load 5mA, peak current 90mA. */
#endif
#if defined(_DCDC_EM23CTRL0_IPKVAL_LOAD5MA)
  emuDcdcPeakCurrent_Load5mA  = _DCDC_EM23CTRL0_IPKVAL_LOAD5MA,  /**< Load 5mA, peak current 90mA. */
#endif
#if defined(_DCDC_EM23CTRL0_IPKVAL_Load10mA)
  emuDcdcPeakCurrent_Load10mA = _DCDC_EM23CTRL0_IPKVAL_Load10mA, /**< Load 10mA, peak current 150mA. */
#endif
#if defined(_DCDC_EM23CTRL0_IPKVAL_LOAD10MA)
  emuDcdcPeakCurrent_Load10mA = _DCDC_EM23CTRL0_IPKVAL_LOAD10MA, /**< Load 10mA, peak current 150mA. */
#endif
#if defined(_DCDC_EM01CTRL0_IPKVAL_Load28mA)
  emuDcdcPeakCurrent_Load28mA = _DCDC_EM01CTRL0_IPKVAL_Load28mA, /**< Load 28mA, peak current 70mA. */
#endif
#if defined(_DCDC_EM01CTRL0_IPKVAL_Load32mA)
  emuDcdcPeakCurrent_Load32mA = _DCDC_EM01CTRL0_IPKVAL_Load32mA, /**< Load 32mA, peak current 80mA. */
#endif
#if defined(_DCDC_EM01CTRL0_IPKVAL_Load36mA)
  emuDcdcPeakCurrent_Load36mA = _DCDC_EM01CTRL0_IPKVAL_Load36mA, /**< Load 36mA, peak current 90mA. */
#endif
  emuDcdcPeakCurrent_Load40mA = _DCDC_EM01CTRL0_IPKVAL_Load40mA, /**< Load 40mA, peak current 100mA. */
  emuDcdcPeakCurrent_Load44mA = _DCDC_EM01CTRL0_IPKVAL_Load44mA, /**< Load 44mA, peak current 110mA. */
  emuDcdcPeakCurrent_Load48mA = _DCDC_EM01CTRL0_IPKVAL_Load48mA, /**< Load 48mA, peak current 120mA. */
  emuDcdcPeakCurrent_Load52mA = _DCDC_EM01CTRL0_IPKVAL_Load52mA, /**< Load 52mA, peak current 130mA. */
  emuDcdcPeakCurrent_Load56mA = _DCDC_EM01CTRL0_IPKVAL_Load56mA, /**< Load 56mA, peak current 140mA. */
  emuDcdcPeakCurrent_Load60mA = _DCDC_EM01CTRL0_IPKVAL_Load60mA, /**< Load 60mA, peak current 150mA. */
#if defined(_DCDC_EM01CTRL0_IPKVAL_Load64mA)
  emuDcdcPeakCurrent_Load64mA = _DCDC_EM01CTRL0_IPKVAL_Load64mA, /**< Load 64mA, peak current 160mA. */
#endif
#if defined(_DCDC_EM01CTRL0_IPKVAL_Load68mA)
  emuDcdcPeakCurrent_Load68mA = _DCDC_EM01CTRL0_IPKVAL_Load68mA, /**< Load 68mA, peak current 170mA. */
#endif
#if defined(_DCDC_EM01CTRL0_IPKVAL_Load72mA)
  emuDcdcPeakCurrent_Load72mA = _DCDC_EM01CTRL0_IPKVAL_Load72mA, /**< Load 72mA, peak current 180mA. */
#endif
#if defined(_DCDC_EM01CTRL0_IPKVAL_Load76mA)
  emuDcdcPeakCurrent_Load76mA = _DCDC_EM01CTRL0_IPKVAL_Load76mA, /**< Load 76mA, peak current 190mA. */
#endif
#if defined(_DCDC_EM01CTRL0_IPKVAL_Load80mA)
  emuDcdcPeakCurrent_Load80mA = _DCDC_EM01CTRL0_IPKVAL_Load80mA  /**< Load 80mA, peak current 200mA. */
#endif
} EMU_DcdcPeakCurrent_TypeDef;
#endif /* EMU_SERIES2_DCDC_BUCK_PRESENT */

#if defined(EMU_SERIES2_DCDC_BOOST_PRESENT)
/** DCDC Boost drive speed. */
typedef enum {
  emuDcdcBoostDriveSpeed_BestEmi        = _DCDC_BSTEM01CTRL_DRVSPEED_DEFAULT_SETTING,        /**< Recommend no options other than DEFAULT be used here, as there is no benefit. */
  emuDcdcBoostDriveSpeed_Default        = _DCDC_BSTEM01CTRL_DRVSPEED_DEFAULT_SETTING,        /**< Recommend no options other than DEFAULT be used here, as there is no benefit. */
  emuDcdcBoostDriveSpeed_Intermediate   = _DCDC_BSTEM01CTRL_DRVSPEED_DEFAULT_SETTING,        /**< Recommend no options other than DEFAULT be used here, as there is no benefit. */
  emuDcdcBoostDriveSpeed_BestEfficiency = _DCDC_BSTEM01CTRL_DRVSPEED_DEFAULT_SETTING         /**< Recommend no options other than DEFAULT be used here, as there is no benefit. */
} EMU_DcdcBoostDriveSpeed_TypeDef;

/** DCDC Boost EM01 peak current setting. */
typedef enum {
  emuDcdcBoostEM01PeakCurrent_Load10mA = _DCDC_BSTEM01CTRL_IPKVAL_Load10mA, /**< Load 10mA */
  emuDcdcBoostEM01PeakCurrent_Load11mA = _DCDC_BSTEM01CTRL_IPKVAL_Load11mA, /**< Load 11mA */
  emuDcdcBoostEM01PeakCurrent_Load13mA = _DCDC_BSTEM01CTRL_IPKVAL_Load13mA, /**< Load 13mA */
  emuDcdcBoostEM01PeakCurrent_Load15mA = _DCDC_BSTEM01CTRL_IPKVAL_Load15mA, /**< Load 15mA */
  emuDcdcBoostEM01PeakCurrent_Load16mA = _DCDC_BSTEM01CTRL_IPKVAL_Load16mA, /**< Load 16mA */
  emuDcdcBoostEM01PeakCurrent_Load18mA = _DCDC_BSTEM01CTRL_IPKVAL_Load18mA, /**< Load 18mA */
  emuDcdcBoostEM01PeakCurrent_Load20mA = _DCDC_BSTEM01CTRL_IPKVAL_Load20mA, /**< Load 20mA */
  emuDcdcBoostEM01PeakCurrent_Load21mA = _DCDC_BSTEM01CTRL_IPKVAL_Load21mA, /**< Load 21mA */
  emuDcdcBoostEM01PeakCurrent_Load23mA = _DCDC_BSTEM01CTRL_IPKVAL_Load23mA, /**< Load 23mA */
  emuDcdcBoostEM01PeakCurrent_Load25mA = _DCDC_BSTEM01CTRL_IPKVAL_Load25mA, /**< Load 25mA */
} EMU_DcdcBoostEM01PeakCurrent_TypeDef;

/** DCDC Boost Toff max timeout */
typedef enum {
  emuDcdcBoostToffMaxTimeout_Off    = _DCDC_BSTCTRL_BSTTOFFMAX_OFF,         /**< Toff max off.       */
  emuDcdcBoostToffMaxTimeout_0P35us = _DCDC_BSTCTRL_BSTTOFFMAX_TMAX_0P35us, /**< Toff max is 0.35us. */
  emuDcdcBoostToffMaxTimeout_0P63us = _DCDC_BSTCTRL_BSTTOFFMAX_TMAX_0P63us, /**< Toff max is 0.63us. */
  emuDcdcBoostToffMaxTimeout_0P91us = _DCDC_BSTCTRL_BSTTOFFMAX_TMAX_0P91us, /**< Toff max is 0.91us. */
  emuDcdcBoostToffMaxTimeout_1P19us = _DCDC_BSTCTRL_BSTTOFFMAX_TMAX_1P19us, /**< Toff max is 1.19us. */
  emuDcdcBoostToffMaxTimeout_1P47us = _DCDC_BSTCTRL_BSTTOFFMAX_TMAX_1P47us, /**< Toff max is 1.47us. */
  emuDcdcBoostToffMaxTimeout_1P75us = _DCDC_BSTCTRL_BSTTOFFMAX_TMAX_1P75us, /**< Toff max is 1.75us. */
  emuDcdcBoostToffMaxTimeout_2P03us = _DCDC_BSTCTRL_BSTTOFFMAX_TMAX_2P03us, /**< Toff max is 2.03us. */
} EMU_DcdcBoostToffMaxTimeout_TypeDef;

/** DCDC Boost peak current detection maximum timeout */
typedef enum {
  emuDcdcBoostTonMaxTimeout_Off    = _DCDC_BSTCTRL_IPKTMAXCTRL_OFF,         /**< Ton max off.       */
  emuDcdcBoostTonMaxTimeout_0P35us = _DCDC_BSTCTRL_IPKTMAXCTRL_TMAX_0P35us, /**< Ton max is 0.35us. */
  emuDcdcBoostTonMaxTimeout_0P63us = _DCDC_BSTCTRL_IPKTMAXCTRL_TMAX_0P63us, /**< Ton max is 0.63us. */
  emuDcdcBoostTonMaxTimeout_0P91us = _DCDC_BSTCTRL_IPKTMAXCTRL_TMAX_0P91us, /**< Ton max is 0.91us. */
  emuDcdcBoostTonMaxTimeout_1P19us = _DCDC_BSTCTRL_IPKTMAXCTRL_TMAX_1P19us, /**< Ton max is 1.19us. */
  emuDcdcBoostTonMaxTimeout_1P47us = _DCDC_BSTCTRL_IPKTMAXCTRL_TMAX_1P47us, /**< Ton max is 1.47us. */
  emuDcdcBoostTonMaxTimeout_1P75us = _DCDC_BSTCTRL_IPKTMAXCTRL_TMAX_1P75us, /**< Ton max is 1.75us. */
  emuDcdcBoostTonMaxTimeout_2P03us = _DCDC_BSTCTRL_IPKTMAXCTRL_TMAX_2P03us, /**< Ton max is 2.03us. */
} EMU_DcdcBoostTonMaxTimeout_TypeDef;

/** DCDC Boost EM23 peak current setting. */
typedef enum {
  emuDcdcBoostEM23PeakCurrent_Load10mA = _DCDC_BSTEM23CTRL_IPKVAL_Load10mA, /**< Load 10mA */
} EMU_DcdcBoostEM23PeakCurrent_TypeDef;

#if defined(_DCDC_CTRL_DVDDBSTPRG_MASK)
/** DCDC Boost output voltage */
typedef enum {
  emuDcdcBoostOutputVoltage_1v8 = _DCDC_CTRL_DVDDBSTPRG_BOOST_1V8,    /**< Output voltage is 1.8V. */
  emuDcdcBoostOutputVoltage_1v9 = _DCDC_CTRL_DVDDBSTPRG_BOOST_1V9,    /**< Output voltage is 1.9V. */
  emuDcdcBoostOutputVoltage_2v0 = _DCDC_CTRL_DVDDBSTPRG_BOOST_2V,     /**< Output voltage is 2.0V. */
  emuDcdcBoostOutputVoltage_2v1 = _DCDC_CTRL_DVDDBSTPRG_BOOST_2V1,    /**< Output voltage is 2.1V. */
  emuDcdcBoostOutputVoltage_2v2 = _DCDC_CTRL_DVDDBSTPRG_BOOST_2V2,    /**< Output voltage is 2.2V. */
  emuDcdcBoostOutputVoltage_2v3 = _DCDC_CTRL_DVDDBSTPRG_BOOST_2V3,    /**< Output voltage is 2.3V. */
  emuDcdcBoostOutputVoltage_2v4 = _DCDC_CTRL_DVDDBSTPRG_BOOST_2V4,    /**< Output voltage is 2.4V. */
} EMU_DcdcBoostOutputVoltage_TypeDef;
#endif

#endif /* EMU_SERIES2_DCDC_BOOST_PRESENT) */

#if defined(EMU_STATUS_VMONRDY)
/** VMON channels. */
typedef enum {
  emuVmonChannel_AVDD,
  emuVmonChannel_ALTAVDD,
  emuVmonChannel_DVDD,
  emuVmonChannel_IOVDD0,
#if defined(_EMU_VMONIO1CTRL_EN_MASK)
  emuVmonChannel_IOVDD1,
#endif
#if defined(_EMU_VMONBUVDDCTRL_EN_MASK)
  emuVmonChannel_BUVDD,
#endif
} EMU_VmonChannel_TypeDef;
#endif /* EMU_STATUS_VMONRDY */

#if defined(_SILICON_LABS_GECKO_INTERNAL_SDID_80)
/** Bias mode configurations. */
typedef enum {
  emuBiasMode_1KHz,
  emuBiasMode_4KHz,
  emuBiasMode_Continuous
} EMU_BiasMode_TypeDef;
#endif

#if defined(EMU_VSCALE_EM01_PRESENT)
/** Supported EM0/1 Voltage Scaling Levels. */
typedef enum {
  /** High-performance voltage level. HF clock can be set to any frequency. */
  emuVScaleEM01_HighPerformance = _EMU_STATUS_VSCALE_VSCALE2,
  /** Low-power optimized voltage level. HF clock must be limited
      to CMU_VSCALEEM01_LOWPOWER_VOLTAGE_CLOCK_MAX Hz at this voltage.
      EM0/1 voltage scaling is applied when core clock frequency is
      changed from @ref cmu or when calling @ref EMU_EM01Init() when HF
      clock is already below the limit. */
#if defined(_SILICON_LABS_32B_SERIES_2)
  /** Minimum VSCALE level in EM0/1 is VSCALE1. */
  emuVScaleEM01_LowPower        = _EMU_STATUS_VSCALE_VSCALE1,
#else
  emuVScaleEM01_LowPower        = _EMU_STATUS_VSCALE_VSCALE0,
#endif
} EMU_VScaleEM01_TypeDef;
#endif

#if defined(EMU_VSCALE_PRESENT)
/** Supported EM2/3 Voltage Scaling Levels. */
typedef enum {
  /** Fast-wakeup voltage level. */
  emuVScaleEM23_FastWakeup      = _EMU_CTRL_EM23VSCALE_VSCALE2,
  /** Low-power optimized voltage level. Using this voltage level in EM2 and 3
      adds approximately 30 us to wakeup time if EM0 and 1 voltage must be scaled
      up to emuVScaleEM01_HighPerformance on EM2 or 3 exit. */
  emuVScaleEM23_LowPower        = _EMU_CTRL_EM23VSCALE_VSCALE0,
} EMU_VScaleEM23_TypeDef;
#endif

#if defined(_EMU_CTRL_EM4HVSCALE_MASK)
/** Supported EM4H Voltage Scaling Levels */
typedef enum {
  /** Fast-wakeup voltage level. */
  emuVScaleEM4H_FastWakeup      = _EMU_CTRL_EM4HVSCALE_VSCALE2,
  /** Low-power optimized voltage level. Using this voltage level in EM4H
      adds approximately 30 us to wakeup time if EM0 and 1 voltage must be scaled
      up to @ref emuVScaleEM01_HighPerformance on EM4H exit. */
  emuVScaleEM4H_LowPower        = _EMU_CTRL_EM4HVSCALE_VSCALE0,
} EMU_VScaleEM4H_TypeDef;
#endif

#if defined(_EMU_EM23PERNORETAINCTRL_MASK)
/** Peripheral EM2 and 3 retention control. */
typedef enum {
#if defined(_EMU_EM23PERNORETAINCTRL_USBDIS_MASK)
  emuPeripheralRetention_USB      = _EMU_EM23PERNORETAINCTRL_USBDIS_MASK,       /**< Select USB retention control.  */
#endif
#if defined(_EMU_EM23PERNORETAINCTRL_RTCDIS_MASK)
  emuPeripheralRetention_RTC      = _EMU_EM23PERNORETAINCTRL_RTCDIS_MASK,       /**< Select RTC retention control.  */
#endif
#if defined(_EMU_EM23PERNORETAINCTRL_ACMP3DIS_MASK)
  emuPeripheralRetention_ACMP3    = _EMU_EM23PERNORETAINCTRL_ACMP3DIS_MASK,     /**< Select ACMP3 retention control. */
#endif
#if defined(_EMU_EM23PERNORETAINCTRL_ACMP2DIS_MASK)
  emuPeripheralRetention_ACMP2    = _EMU_EM23PERNORETAINCTRL_ACMP2DIS_MASK,     /**< Select ACMP2 retention control.  */
#endif
#if defined(_EMU_EM23PERNORETAINCTRL_ADC1DIS_MASK)
  emuPeripheralRetention_ADC1     = _EMU_EM23PERNORETAINCTRL_ADC1DIS_MASK,      /**< Select ADC1 retention control.  */
#endif
#if defined(_EMU_EM23PERNORETAINCTRL_I2C2DIS_MASK)
  emuPeripheralRetention_I2C2     = _EMU_EM23PERNORETAINCTRL_I2C2DIS_MASK,      /**< Select I2C2 retention control.  */
#endif
#if defined(_EMU_EM23PERNORETAINCTRL_LETIMER1DIS_MASK)
  emuPeripheralRetention_LETIMER1 = _EMU_EM23PERNORETAINCTRL_LETIMER1DIS_MASK,  /**< Select LETIMER1 retention control.  */
#endif
#if defined(_EMU_EM23PERNORETAINCTRL_LCDDIS_MASK)
  emuPeripheralRetention_LCD      = _EMU_EM23PERNORETAINCTRL_LCDDIS_MASK,       /**< Select LCD retention control.  */
#endif
#if defined(_EMU_EM23PERNORETAINCTRL_LEUART1DIS_MASK)
  emuPeripheralRetention_LEUART1  = _EMU_EM23PERNORETAINCTRL_LEUART1DIS_MASK,   /**< Select LEUART1 retention control.  */
#endif
  emuPeripheralRetention_LEUART0  = _EMU_EM23PERNORETAINCTRL_LEUART0DIS_MASK,   /**< Select LEUART0 retention control.  */
#if defined(_EMU_EM23PERNORETAINCTRL_CSENDIS_MASK)
  emuPeripheralRetention_CSEN     = _EMU_EM23PERNORETAINCTRL_CSENDIS_MASK,      /**< Select CSEN retention control.  */
#endif
  emuPeripheralRetention_LESENSE0 = _EMU_EM23PERNORETAINCTRL_LESENSE0DIS_MASK,  /**< Select LESENSE0 retention control.  */
#if defined(_EMU_EM23PERNORETAINCTRL_WDOG1DIS_MASK)
  emuPeripheralRetention_WDOG1    = _EMU_EM23PERNORETAINCTRL_WDOG1DIS_MASK,     /**< Select WDOG1 retention control.  */
#endif
  emuPeripheralRetention_WDOG0    = _EMU_EM23PERNORETAINCTRL_WDOG0DIS_MASK,     /**< Select WDOG0 retention control.  */
  emuPeripheralRetention_LETIMER0 = _EMU_EM23PERNORETAINCTRL_LETIMER0DIS_MASK,  /**< Select LETIMER0 retention control.  */
  emuPeripheralRetention_ADC0     = _EMU_EM23PERNORETAINCTRL_ADC0DIS_MASK,      /**< Select ADC0 retention control.  */
#if defined(_EMU_EM23PERNORETAINCTRL_IDAC0DIS_MASK)
  emuPeripheralRetention_IDAC0    = _EMU_EM23PERNORETAINCTRL_IDAC0DIS_MASK,     /**< Select IDAC0 retention control.  */
#endif
#if defined(_EMU_EM23PERNORETAINCTRL_VDAC0DIS_MASK)
  emuPeripheralRetention_VDAC0    = _EMU_EM23PERNORETAINCTRL_VDAC0DIS_MASK,     /**< Select VDAC0 retention control.  */
#endif
#if defined(_EMU_EM23PERNORETAINCTRL_I2C1DIS_MASK)
  emuPeripheralRetention_I2C1     = _EMU_EM23PERNORETAINCTRL_I2C1DIS_MASK,      /**< Select I2C1 retention control.  */
#endif
  emuPeripheralRetention_I2C0     = _EMU_EM23PERNORETAINCTRL_I2C0DIS_MASK,      /**< Select I2C0 retention control.  */
  emuPeripheralRetention_ACMP1    = _EMU_EM23PERNORETAINCTRL_ACMP1DIS_MASK,     /**< Select ACMP1 retention control.  */
  emuPeripheralRetention_ACMP0    = _EMU_EM23PERNORETAINCTRL_ACMP0DIS_MASK,     /**< Select ACMP0 retention control.  */
#if defined(_EMU_EM23PERNORETAINCTRL_PCNT1DIS_MASK)
  emuPeripheralRetention_PCNT2    = _EMU_EM23PERNORETAINCTRL_PCNT2DIS_MASK,     /**< Select PCNT2 retention control.  */
  emuPeripheralRetention_PCNT1    = _EMU_EM23PERNORETAINCTRL_PCNT1DIS_MASK,     /**< Select PCNT1 retention control.  */
#endif
  emuPeripheralRetention_PCNT0    = _EMU_EM23PERNORETAINCTRL_PCNT0DIS_MASK,     /**< Select PCNT0 retention control.  */

  emuPeripheralRetention_D1       = _EMU_EM23PERNORETAINCTRL_LETIMER0DIS_MASK
                                    | _EMU_EM23PERNORETAINCTRL_PCNT0DIS_MASK
                                    | _EMU_EM23PERNORETAINCTRL_ADC0DIS_MASK
                                    | _EMU_EM23PERNORETAINCTRL_ACMP0DIS_MASK
                                    | _EMU_EM23PERNORETAINCTRL_LESENSE0DIS_MASK,/**< Select all peripherals in domain 1. */
  emuPeripheralRetention_D2       = _EMU_EM23PERNORETAINCTRL_ACMP1DIS_MASK
#if defined(_EMU_EM23PERNORETAINCTRL_IDAC0DIS_MASK)
                                    | _EMU_EM23PERNORETAINCTRL_IDAC0DIS_MASK
#endif
#if defined(_EMU_EM23PERNORETAINCTRL_VDAC0DIS_MASK)
                                    | _EMU_EM23PERNORETAINCTRL_VDAC0DIS_MASK
#endif
#if defined(_EMU_EM23PERNORETAINCTRL_CSENDIS_MASK)
                                    | _EMU_EM23PERNORETAINCTRL_CSENDIS_MASK
#endif
                                    | _EMU_EM23PERNORETAINCTRL_LEUART0DIS_MASK
#if defined(_EMU_EM23PERNORETAINCTRL_USBDIS_MASK)
                                    | _EMU_EM23PERNORETAINCTRL_USBDIS_MASK
#endif
#if defined(_EMU_EM23PERNORETAINCTRL_RTCDIS_MASK)
                                    | _EMU_EM23PERNORETAINCTRL_RTCDIS_MASK
#endif
#if defined(_EMU_EM23PERNORETAINCTRL_ACMP3DIS_MASK)
                                    | _EMU_EM23PERNORETAINCTRL_ACMP3DIS_MASK
#endif
#if defined(_EMU_EM23PERNORETAINCTRL_ACMP2DIS_MASK)
                                    | _EMU_EM23PERNORETAINCTRL_ACMP2DIS_MASK
#endif
#if defined(_EMU_EM23PERNORETAINCTRL_ADC1DIS_MASK)
                                    | _EMU_EM23PERNORETAINCTRL_ADC1DIS_MASK
#endif
#if defined(_EMU_EM23PERNORETAINCTRL_I2C2DIS_MASK)
                                    | _EMU_EM23PERNORETAINCTRL_I2C2DIS_MASK
#endif
#if defined(_EMU_EM23PERNORETAINCTRL_LETIMER1DIS_MASK)
                                    | _EMU_EM23PERNORETAINCTRL_LETIMER1DIS_MASK
#endif
#if defined(_EMU_EM23PERNORETAINCTRL_LCDDIS_MASK)
                                    | _EMU_EM23PERNORETAINCTRL_LCDDIS_MASK
#endif
#if defined(_EMU_EM23PERNORETAINCTRL_LEUART1DIS_MASK)
                                    | _EMU_EM23PERNORETAINCTRL_LEUART1DIS_MASK
#endif
#if defined(_EMU_EM23PERNORETAINCTRL_PCNT1DIS_MASK)
                                    | _EMU_EM23PERNORETAINCTRL_PCNT1DIS_MASK
                                    | _EMU_EM23PERNORETAINCTRL_PCNT2DIS_MASK
#endif
#if defined(_EMU_EM23PERNORETAINCTRL_I2C1DIS_MASK)
                                    | _EMU_EM23PERNORETAINCTRL_I2C1DIS_MASK    /**< Select all peripherals in domain 2. */
#endif
                                    | _EMU_EM23PERNORETAINCTRL_I2C0DIS_MASK,
  emuPeripheralRetention_ALL       = emuPeripheralRetention_D1
                                     | emuPeripheralRetention_D2
#if defined(_EMU_EM23PERNORETAINCTRL_WDOG1DIS_MASK)
                                     | emuPeripheralRetention_WDOG1
#endif
                                     | emuPeripheralRetention_WDOG0,            /**< Select all peripherals with retention control.  */
} EMU_PeripheralRetention_TypeDef;
#endif

#if defined(_EMU_TEMP_TEMPAVG_MASK)
/** Number of samples to use for temperature averaging. */
typedef enum {
  /** 16 samples used for temperature averaging. */
  emuTempAvgNum_16      = _EMU_CTRL_TEMPAVGNUM_N16,
  /** 64 samples used for temperature averaging. */
  emuTempAvgNum_64      = _EMU_CTRL_TEMPAVGNUM_N64,
} EMU_TempAvgNum_TypeDef;
#endif

/*******************************************************************************
 *******************************   STRUCTS   ***********************************
 ******************************************************************************/

#if defined(EMU_VSCALE_EM01_PRESENT)
/** EM0 and 1 initialization structure. Voltage scaling is applied when
    the core clock frequency is changed from @ref cmu. EM0 and 1 emuVScaleEM01_HighPerformance
    is always enabled. */
typedef struct {
  bool  vScaleEM01LowPowerVoltageEnable; /**< EM0/1 low power voltage status. */
#if defined(_EMU_CTRL_HDREGSTOPGEAR_MASK)
  EMU_HdregStopGearILmt_TypeDef current; /**<  limit HDREG max current capability. */
#endif
} EMU_EM01Init_TypeDef;

/** Default initialization of EM0 and 1 configuration. */
#if defined(_EMU_CTRL_HDREGSTOPGEAR_MASK)
#define EMU_EM01INIT_DEFAULT                                                                 \
  {                                                                                          \
    false,                                                /* Do not scale down in EM0/1.*/   \
    emuHdregStopGearILmt64mA                              /* HDREG current limit is 64mA. */ \
  }
#else
#define EMU_EM01INIT_DEFAULT                                                               \
  {                                                                                        \
    false                                                 /* Do not scale down in EM0/1.*/ \
  }
#endif
#endif
/** EM2 and 3 initialization structure.  */
typedef struct {
  bool                          em23VregFullEn;         /**< Enable full VREG drive strength in EM2/3. */
#if defined(EMU_VSCALE_PRESENT)
  EMU_VScaleEM23_TypeDef        vScaleEM23Voltage;      /**< EM2/3 voltage scaling level. */
#endif
} EMU_EM23Init_TypeDef;

/** Default initialization of EM2 and 3 configuration. */
#if defined(EMU_VSCALE_PRESENT)
#define EMU_EM23INIT_DEFAULT                                                                                    \
  {                                                                                                             \
    false,                                              /* Reduced voltage regulator drive strength in EM2/3.*/ \
    emuVScaleEM23_FastWakeup,                           /* Do not scale down in EM2/3. */                       \
  }
#else
#define EMU_EM23INIT_DEFAULT                                                                                    \
  {                                                                                                             \
    false,                                              /* Reduced voltage regulator drive strength in EM2/3.*/ \
  }
#endif
#if defined(_EMU_EM4CONF_MASK) || defined(_EMU_EM4CTRL_MASK)
/** EM4 initialization structure.  */
typedef struct {
#if defined(_EMU_EM4CONF_MASK)
  /* Initialization parameters for platforms with EMU->EM4CONF register (Series 0). */
  bool                        lockConfig;       /**< Lock configuration of regulator, BOD and oscillator. */
  bool                        buBodRstDis;      /**< When set, no reset will be asserted due to Brownout when in EM4. */
  EMU_EM4Osc_TypeDef          osc;              /**< EM4 duty oscillator. */
  bool                        buRtcWakeup;      /**< Wake up on EM4 BURTC interrupt. */
  bool                        vreg;             /**< Enable EM4 voltage regulator. */
#elif defined(_EMU_EM4CTRL_MASK)
  /* Initialization parameters for platforms with EMU->EM4CTRL register (Series 1). */
  bool                        retainLfxo;       /**< Disable LFXO upon EM4 entry. */
  bool                        retainLfrco;      /**< Disable LFRCO upon EM4 entry. */
  bool                        retainUlfrco;     /**< Disable ULFRCO upon EM4 entry. */
  EMU_EM4State_TypeDef        em4State;         /**< Hibernate or shutoff EM4 state. */
  EMU_EM4PinRetention_TypeDef pinRetentionMode; /**< EM4 pin retention mode. */
#endif
#if defined(_EMU_CTRL_EM4HVSCALE_MASK)
  EMU_VScaleEM4H_TypeDef      vScaleEM4HVoltage;/**< EM4H voltage scaling level. */
#endif
} EMU_EM4Init_TypeDef;
#endif

#if defined(_EMU_EM4CONF_MASK)
/** Default initialization of EM4 configuration (Series 0). */
#define EMU_EM4INIT_DEFAULT                                                                \
  {                                                                                        \
    false,                            /* Do not lock configuration after it's been set. */ \
    false,                            /* No reset will be asserted due to BOD in EM4. */   \
    emuEM4Osc_ULFRCO,                 /* Use default ULFRCO oscillator.  */                \
    true,                             /* Wake up on EM4 BURTC interrupt. */                \
    true,                             /* Enable VREG. */                                   \
  }

#elif defined(_EMU_CTRL_EM4HVSCALE_MASK)
/** Default initialization of EM4 configuration (Series 1 with VSCALE). */
#define EMU_EM4INIT_DEFAULT                                                            \
  {                                                                                    \
    false,                           /* Retain LFXO configuration upon EM4 entry. */   \
    false,                           /* Retain LFRCO configuration upon EM4 entry. */  \
    false,                           /* Retain ULFRCO configuration upon EM4 entry. */ \
    emuEM4Shutoff,                   /* Use EM4 shutoff state. */                      \
    emuPinRetentionDisable,          /* Do not retain pins in EM4. */                  \
    emuVScaleEM4H_FastWakeup,        /* Do not scale down in EM4H. */                  \
  }

#elif defined(_EMU_EM4CTRL_MASK)
/** Default initialization of EM4 configuration (Series 1 without VSCALE). */
#define EMU_EM4INIT_DEFAULT                                                            \
  {                                                                                    \
    false,                           /* Retain LFXO configuration upon EM4 entry. */   \
    false,                           /* Retain LFRCO configuration upon EM4 entry. */  \
    false,                           /* Retain ULFRCO configuration upon EM4 entry. */ \
    emuEM4Shutoff,                   /* Use EM4 shutoff state. */                      \
    emuPinRetentionDisable,          /* Do not retain pins in EM4. */                  \
  }
#endif

#if defined(BU_PRESENT) && defined(_SILICON_LABS_32B_SERIES_0)
/** Backup Power Domain Initialization structure. */
typedef struct {
  /* Backup Power Domain power configuration. */

  /** Voltage probe select, selects ADC voltage. */
  EMU_Probe_TypeDef     probe;
  /** Enable BOD calibration mode. */
  bool                  bodCal;
  /** Enable BU_STAT status pin for active BU mode. */
  bool                  statusPinEnable;

  /* Backup Power Domain connection configuration. */
  /** Power domain resistor. */
  EMU_Resistor_TypeDef  resistor;
  /** BU_VOUT strong enable. */
  bool                  voutStrong;
  /** BU_VOUT medium enable. */
  bool                  voutMed;
  /** BU_VOUT weak enable. */
  bool                  voutWeak;
  /** Power connection, when not in Backup Mode. */
  EMU_Power_TypeDef  inactivePower;
  /** Power connection, when in Backup Mode. */
  EMU_Power_TypeDef     activePower;
  /** Enable backup power domain, and release reset, enable BU_VIN pin.  */
  bool                  enable;
} EMU_BUPDInit_TypeDef;

/** Default Backup Power Domain configuration. */
#define EMU_BUPDINIT_DEFAULT                                                              \
  {                                                                                       \
    emuProbe_Disable, /* Do not enable voltage probe. */                                  \
    false,          /* Disable BOD calibration mode. */                                   \
    false,          /* Disable BU_STAT pin for backup mode indication. */                 \
                                                                                          \
    emuRes_Res0,    /* RES0 series resistance between main and backup power. */           \
    false,          /* Do not enable strong switch. */                                    \
    false,          /* Do not enable medium switch. */                                    \
    false,          /* Do not enable weak switch. */                                      \
                                                                                          \
    emuPower_None,  /* No connection between main and backup power. (inactive mode) */    \
    emuPower_None,  /* No connection between main and backup power. (active mode) */      \
    true            /* Enable BUPD enter on BOD, enable BU_VIN pin, release BU reset.  */ \
  }
#endif

#if defined(BU_PRESENT) && defined(_SILICON_LABS_32B_SERIES_1)
/** Backup Power Domain Initialization structure. */
typedef struct {
  bool disMaxComp;                            /**< Disable MAIN-BU Comparator. */
  EMU_BUBuInactPwrCon_TypeDef inactivePwrCon; /**< Power connection configuration when not in backup mode. */
  EMU_BUBuActPwrCon_TypeDef activePwrCon;     /**< Power connection configuration when in backup mode. */
  EMU_BUPwrRes_TypeDef pwrRes;                /**< Power domain resistor. */
  EMU_BUVoutRes_TypeDef voutRes;              /**< BU_VOUT resistor select. */
  bool buVinProbeEn;                          /**< Enable BU_VIN probing. */
  bool staEn;                                 /**< Enable backup mode status export. */
  bool enable;                                /**< Enable backup power domain. */
} EMU_BUInit_TypeDef;

/** Default Backup Power Domain configuration. */
#define EMU_BUINIT_DEFAULT                                                               \
  {                                                                                      \
    false,                   /* MAIN-BU Comparator is not disabled */                    \
    emuBuBuInactPwrCon_None, /* No power connection wen not in backup mode */            \
    emuBuBuActPwrCon_None,   /* No power connection when in backup mode */               \
    emuBuPwrRes_Res0,        /* RES0 series resistance between main and backup power. */ \
    emuBuVoutRes_Dis,        /* Vout resistor is set to not connected */                 \
    false,                   /* BU_VIN probe is disabled */                              \
    false,                   /* Status export is disabled */                             \
    true                     /* Enable backup mode */                                    \
  }
#endif

#if defined(EMU_SERIES2_DCDC_BOOST_PRESENT)
/** DCDC Boost regulator initialization structure. */
typedef struct {
  EMU_DcdcBoostTonMaxTimeout_TypeDef    tonMax;                 /**< Ton max timeout control.                       */
  bool                                  externalShutdownEn;     /**< true = disable DCDC boost mode with BOOST_EN=0 */
  EMU_DcdcBoostDriveSpeed_TypeDef       driveSpeedEM01;         /**< DCDC drive speed in EM0/1.                     */
  EMU_DcdcBoostDriveSpeed_TypeDef       driveSpeedEM23;         /**< DCDC drive speed in EM2/3.                     */
  EMU_DcdcBoostEM01PeakCurrent_TypeDef  peakCurrentEM01;        /**< EM0/1 peak current setting.                    */
  EMU_DcdcBoostEM23PeakCurrent_TypeDef  peakCurrentEM23;        /**< EM2/3 peak current setting.                    */
#if defined(_DCDC_CTRL_DVDDBSTPRG_MASK)
  EMU_DcdcBoostOutputVoltage_TypeDef    outputVoltage;          /**< DCDC Boost output voltage.                     */
#endif
} EMU_DCDCBoostInit_TypeDef;

/** Default DCDC Boost initialization. */
#if defined(_DCDC_CTRL_DVDDBSTPRG_MASK)
#define EMU_DCDCBOOSTINIT_DEFAULT                                                         \
  {                                                                                       \
    emuDcdcBoostTonMaxTimeout_1P19us,     /**< Ton max is 1.19us. */                      \
    true,                                 /**< disable DCDC boost mode with BOOST_EN=0 */ \
    emuDcdcBoostDriveSpeed_Default,       /**< Default efficiency in EM0/1. */            \
    emuDcdcBoostDriveSpeed_Default,       /**< Default efficiency in EM2/3. */            \
    emuDcdcBoostEM01PeakCurrent_Load23mA, /**< Default peak current in EM0/1. */          \
    emuDcdcBoostEM23PeakCurrent_Load10mA, /**< Default peak current in EM2/3. */          \
    emuDcdcBoostOutputVoltage_1v8         /**< DCDC Boost output voltage. */              \
  }
#else
#define EMU_DCDCBOOSTINIT_DEFAULT                                                         \
  {                                                                                       \
    emuDcdcBoostTonMaxTimeout_1P19us,     /**< Ton max is 1.19us. */                      \
    true,                                 /**< disable DCDC boost mode with BOOST_EN=0 */ \
    emuDcdcBoostDriveSpeed_Default,       /**< Default efficiency in EM0/1. */            \
    emuDcdcBoostDriveSpeed_Default,       /**< Default efficiency in EM2/3. */            \
    emuDcdcBoostEM01PeakCurrent_Load23mA, /**< Default peak current in EM0/1. */          \
    emuDcdcBoostEM23PeakCurrent_Load10mA  /**< Default peak current in EM2/3. */          \
  }
#endif
#endif /* EMU_SERIES2_DCDC_BOOST_PRESENT */

#if defined(EMU_SERIES2_DCDC_BUCK_PRESENT)
/** DCDC regulator initialization structure. */
typedef struct {
  EMU_DcdcMode_TypeDef            mode;             /**< DCDC mode. */
  EMU_VreginCmpThreshold_TypeDef  cmpThreshold;     /**< VREGIN comparator threshold. */
  EMU_DcdcTonMaxTimeout_TypeDef   tonMax;           /**< Ton max timeout control. */
#if defined(_DCDC_CTRL_DCMONLYEN_MASK)
  bool                            dcmOnlyEn;        /**< DCM only mode enable. */
#endif
  EMU_DcdcDriveSpeed_TypeDef      driveSpeedEM01;   /**< DCDC drive speed in EM0/1. */
  EMU_DcdcDriveSpeed_TypeDef      driveSpeedEM23;   /**< DCDC drive speed in EM2/3. */
  EMU_DcdcPeakCurrent_TypeDef     peakCurrentEM01;  /**< EM0/1 peak current setting. */
  EMU_DcdcPeakCurrent_TypeDef     peakCurrentEM23;  /**< EM2/3 peak current setting. */
} EMU_DCDCInit_TypeDef;

/** Default DCDC Buck initialization. */
#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_2)
#define EMU_DCDCINIT_DEFAULT                                                 \
  {                                                                          \
    emuDcdcMode_Regulation,        /**< DCDC regulator on. */                \
    emuVreginCmpThreshold_2v3,     /**< 2.3V VREGIN comparator threshold. */ \
    emuDcdcTonMaxTimeout_1P19us,   /**< Ton max is 1.19us. */                \
    true,                          /**< Enable DCM only mode. */             \
    emuDcdcDriveSpeed_Default,     /**< Default efficiency in EM0/1. */      \
    emuDcdcDriveSpeed_Default,     /**< Default efficiency in EM2/3. */      \
    emuDcdcPeakCurrent_Load60mA,   /**< Default peak current in EM0/1. */    \
    emuDcdcPeakCurrent_Load5mA     /**< Default peak current in EM2/3. */    \
  }
#elif defined(_SILICON_LABS_32B_SERIES_2_CONFIG_3) \
  || defined(_SILICON_LABS_32B_SERIES_2_CONFIG_4)  \
  || defined(_SILICON_LABS_32B_SERIES_2_CONFIG_5)  \
  || defined(_SILICON_LABS_32B_SERIES_2_CONFIG_6)  \
  || defined(_SILICON_LABS_32B_SERIES_2_CONFIG_7)  \
  || defined(_SILICON_LABS_32B_SERIES_2_CONFIG_8)  \
  || defined(_SILICON_LABS_32B_SERIES_2_CONFIG_9)
#define EMU_DCDCINIT_DEFAULT                                                 \
  {                                                                          \
    emuDcdcMode_Regulation,        /**< DCDC regulator on. */                \
    emuVreginCmpThreshold_2v3,     /**< 2.3V VREGIN comparator threshold. */ \
    emuDcdcTonMaxTimeout_1P19us,   /**< Ton max is 1.19us. */                \
    emuDcdcDriveSpeed_Default,     /**< Default efficiency in EM0/1. */      \
    emuDcdcDriveSpeed_Default,     /**< Default efficiency in EM2/3. */      \
    emuDcdcPeakCurrent_Load60mA,   /**< Default peak current in EM0/1. */    \
    emuDcdcPeakCurrent_Load5mA     /**< Default peak current in EM2/3. */    \
  }
#endif
#endif /* SERIES2_DCDC_BUCK_PRESENT */

#if defined(EMU_SERIES1_DCDC_BUCK_PRESENT)
/** DCDC initialization structure. */
typedef struct {
  EMU_PowerConfig_TypeDef powerConfig;                  /**< Device external power configuration.
                                                             @ref emuPowerConfig_DcdcToDvdd is currently the only supported mode. */
  EMU_DcdcMode_TypeDef dcdcMode;                        /**< DCDC regulator operating mode in EM0/1. */
  uint16_t mVout;                                       /**< Target output voltage (mV). */
  uint16_t em01LoadCurrent_mA;                          /**< Estimated average load current in EM0/1. (mA).
                                                             This estimate is also used for EM1 optimization;
                                                             if EM1 current is expected to be higher than EM0,
                                                             then this parameter should hold the higher EM1 current. */
  uint16_t em234LoadCurrent_uA;                         /**< Estimated average load current in EM2 (uA).
                                                             This estimate is also used for EM3 and 4 optimization;
                                                             if EM3 or 4 current is expected to be higher than EM2,
                                                             then this parameter should hold the higher EM3 or 4 current. */
  uint16_t maxCurrent_mA;                               /**< Maximum average DCDC output current (mA).
                                                             This can be set to the maximum for the power source,
                                                             for example the maximum for a battery. */
  EMU_DcdcAnaPeripheralPower_TypeDef
    anaPeripheralPower;                                 /**< Select analog peripheral power in DCDC-to-DVDD mode. */
  EMU_DcdcLnReverseCurrentControl_TypeDef
    reverseCurrentControl;                              /**< Low-noise reverse current control.
                                                             NOTE: this parameter uses special encoding:
                                                             >= 0 is forced CCM mode where the parameter is used as the
                                                                  reverse current threshold in mA.
                                                             -1   is encoded as emuDcdcLnHighEfficiencyMode (EFM32 only). */
  EMU_DcdcLnCompCtrl_TypeDef dcdcLnCompCtrl;            /**< DCDC Low-noise mode compensator control. */
} EMU_DCDCInit_TypeDef;

/** Default DCDC initialization. */
#if defined(_EFM_DEVICE)
#if defined(_SILICON_LABS_GECKO_INTERNAL_SDID_80)
#define EMU_DCDCINIT_DEFAULT                                                         \
  {                                                                                  \
    emuPowerConfig_DcdcToDvdd,   /* DCDC to DVDD. */                                 \
    emuDcdcMode_LowNoise,        /* Low-noise mode in EM0. */                        \
    1800,                        /* Nominal output voltage for DVDD mode, 1.8V.  */  \
    5,                           /* Nominal EM0/1 load current of less than 5mA. */  \
    10,                          /* Nominal EM2/3/4 load current less than 10uA.  */ \
    200,                         /* Maximum average current of 200mA
                                    (assume strong battery or other power source). */      \
    emuDcdcAnaPeripheralPower_DCDC,/* Select DCDC as analog power supply (lower power). */ \
    emuDcdcLnHighEfficiency,     /* Use high-efficiency mode. */                           \
    emuDcdcLnCompCtrl_1u0F,      /* 1uF DCDC capacitor. */                                 \
  }
#elif defined(WGM160PX22KGA2)
#define EMU_DCDCINIT_DEFAULT                                                         \
  {                                                                                  \
    emuPowerConfig_DcdcToDvdd,   /* DCDC to DVDD. */                                 \
    emuDcdcMode_LowNoise,        /* Low-noise mode in EM0. */                        \
    1800,                        /* Nominal output voltage for DVDD mode, 1.8V.  */  \
    5,                           /* Nominal EM0/1 load current of less than 5mA. */  \
    10,                          /* Nominal EM2/3/4 load current less than 10uA.  */ \
    200, /* Maximum average current of 200mA
            (assume strong battery or other power source). */                 \
    emuDcdcAnaPeripheralPower_AVDD,/* Select AVDD as analog power supply). */ \
    emuDcdcLnHighEfficiency,     /* Use high-efficiency mode. */              \
    emuDcdcLnCompCtrl_4u7F,      /* 4.7uF DCDC capacitor. */                  \
  }
#else
#define EMU_DCDCINIT_DEFAULT                                                         \
  {                                                                                  \
    emuPowerConfig_DcdcToDvdd,   /* DCDC to DVDD. */                                 \
    emuDcdcMode_LowPower,        /* Low-power mode in EM0. */                        \
    1800,                        /* Nominal output voltage for DVDD mode, 1.8V.  */  \
    5,                           /* Nominal EM0/1 load current of less than 5mA. */  \
    10,                          /* Nominal EM2/3/4 load current less than 10uA.  */ \
    200, /* Maximum average current of 200mA
            (assume strong battery or other power source). */                 \
    emuDcdcAnaPeripheralPower_AVDD,/* Select AVDD as analog power supply). */ \
    emuDcdcLnHighEfficiency,     /* Use high-efficiency mode. */              \
    emuDcdcLnCompCtrl_4u7F,      /* 4.7uF DCDC capacitor. */                  \
  }
#endif

#else /* EFR32 device. */
#if defined(_SILICON_LABS_GECKO_INTERNAL_SDID_80)
#define EMU_DCDCINIT_DEFAULT                                                         \
  {                                                                                  \
    emuPowerConfig_DcdcToDvdd,   /* DCDC to DVDD. */                                 \
    emuDcdcMode_LowNoise,        /* Low-noise mode in EM0. */                        \
    1800,                        /* Nominal output voltage for DVDD mode, 1.8V.  */  \
    15,                          /* Nominal EM0/1 load current of less than 15mA. */ \
    10,                          /* Nominal EM2/3/4 load current less than 10uA.  */ \
    200,                         /* Maximum average current of 200mA
                                    (assume strong battery or other power source). */      \
    emuDcdcAnaPeripheralPower_DCDC,/* Select DCDC as analog power supply (lower power). */ \
    160,                         /* Maximum reverse current of 160mA. */                   \
    emuDcdcLnCompCtrl_1u0F,      /* 1uF DCDC capacitor. */                                 \
  }
#else
#define EMU_DCDCINIT_DEFAULT                                                         \
  {                                                                                  \
    emuPowerConfig_DcdcToDvdd,   /* DCDC to DVDD. */                                 \
    emuDcdcMode_LowNoise,        /* Low-noise mode in EM0. */                        \
    1800,                        /* Nominal output voltage for DVDD mode, 1.8V.  */  \
    15,                          /* Nominal EM0/1 load current of less than 15mA. */ \
    10,                          /* Nominal EM2/3/4 load current less than 10uA.  */ \
    200,                         /* Maximum average current of 200mA
                                    (assume strong battery or other power source). */      \
    emuDcdcAnaPeripheralPower_DCDC,/* Select DCDC as analog power supply (lower power). */ \
    160,                         /* Maximum reverse current of 160mA. */                   \
    emuDcdcLnCompCtrl_4u7F,      /* 4.7uF DCDC capacitor. */                               \
  }
#endif
#endif
#endif

#if defined(EMU_STATUS_VMONRDY)
/** VMON initialization structure. */
typedef struct {
  EMU_VmonChannel_TypeDef channel;      /**< VMON channel to configure. */
  int threshold;                        /**< Trigger threshold (mV). Supported range is 1620 mV to 3400 mV. */
  bool riseWakeup;                      /**< Wake up from EM4H on rising edge. */
  bool fallWakeup;                      /**< Wake up from EM4H on falling edge. */
  bool enable;                          /**< Enable VMON channel. */
  bool retDisable;                      /**< Disable IO0 retention when voltage drops below threshold (IOVDD only). */
} EMU_VmonInit_TypeDef;

/** Default VMON initialization structure. */
#define EMU_VMONINIT_DEFAULT                                                         \
  {                                                                                  \
    emuVmonChannel_AVDD,                /* AVDD VMON channel. */                     \
    3200,                               /* 3.2 V threshold. */                       \
    false,                              /* Do not wake from EM4H on rising edge. */  \
    false,                              /* Do not wake from EM4H on falling edge. */ \
    true,                               /* Enable VMON channel. */                   \
    false                               /* Do not disable IO0 retention  */          \
  }

/** VMON Hysteresis initialization structure. */
typedef struct {
  EMU_VmonChannel_TypeDef channel;      /**< VMON channel to configure. */
  int riseThreshold;                    /**< Rising threshold (mV). */
  int fallThreshold;                    /**< Falling threshold (mV). */
  bool riseWakeup;                      /**< Wake up from EM4H on rising edge. */
  bool fallWakeup;                      /**< Wake up from EM4H on falling edge. */
  bool enable;                          /**< Enable VMON channel. */
} EMU_VmonHystInit_TypeDef;

/** Default VMON Hysteresis initialization structure. */
#define EMU_VMONHYSTINIT_DEFAULT                                                     \
  {                                                                                  \
    emuVmonChannel_AVDD,                /* AVDD VMON channel. */                     \
    3200,                               /* 3.2 V rise threshold. */                  \
    3200,                               /* 3.2 V fall threshold. */                  \
    false,                              /* Do not wake from EM4H on rising edge. */  \
    false,                              /* Do not wake from EM4H on falling edge. */ \
    true                                /* Enable VMON channel. */                   \
  }
#endif /* EMU_STATUS_VMONRDY */

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

#if defined(EMU_VSCALE_EM01_PRESENT)
void EMU_EM01Init(const EMU_EM01Init_TypeDef *em01Init);
#endif
void EMU_EM23Init(const EMU_EM23Init_TypeDef *em23Init);
void EMU_EM23PresleepHook(void);
void EMU_EM23PostsleepHook(void);
void EMU_EFPEM23PresleepHook(void);
void EMU_EFPEM23PostsleepHook(void);
void EMU_EnterEM2(bool restore);
void EMU_EnterEM3(bool restore);
void EMU_Save(void);
void EMU_Restore(void);
#if defined(_EMU_EM4CONF_MASK) || defined(_EMU_EM4CTRL_MASK)
void EMU_EM4Init(const EMU_EM4Init_TypeDef *em4Init);
#endif
void EMU_EM4PresleepHook(void);
void EMU_EFPEM4PresleepHook(void);
void EMU_EnterEM4(void);
void EMU_EnterEM4Wait(void);
#if defined(_EMU_EM4CTRL_MASK)
void EMU_EnterEM4H(void);
void EMU_EnterEM4S(void);
#endif
void EMU_RamPowerDown(uint32_t start, uint32_t end);
void EMU_RamPowerUp(void);
#if defined(_EMU_EM23PERNORETAINCTRL_MASK)
void EMU_PeripheralRetention(EMU_PeripheralRetention_TypeDef periMask, bool enable);
#endif
#if defined(EMU_VSCALE_EM01_PRESENT)
#if defined(_SILICON_LABS_32B_SERIES_2)
void EMU_EFPEM01VScale(EMU_VScaleEM01_TypeDef voltage);
#endif
void EMU_VScaleEM01ByClock(uint32_t clockFrequency, bool wait);
void EMU_VScaleEM01(EMU_VScaleEM01_TypeDef voltage, bool wait);
#endif
#if defined(BU_PRESENT) && defined(_SILICON_LABS_32B_SERIES_0)
void EMU_BUPDInit(const EMU_BUPDInit_TypeDef *bupdInit);
void EMU_BUThresholdSet(EMU_BODMode_TypeDef mode, uint32_t value);
void EMU_BUThresRangeSet(EMU_BODMode_TypeDef mode, uint32_t value);
#endif
#if defined(BU_PRESENT) && defined(_SILICON_LABS_32B_SERIES_1)
void EMU_BUInit(const EMU_BUInit_TypeDef *buInit);
#endif
#if defined(_EMU_BUCTRL_DISMAXCOMP_MASK)
void EMU_BUDisMaxCompSet(bool disableMainBuComparator);
#endif
#if defined(_EMU_BUCTRL_BUINACTPWRCON_MASK)
void EMU_BUBuInactPwrConSet(EMU_BUBuInactPwrCon_TypeDef inactPwrCon);
#endif
#if defined(_EMU_BUCTRL_BUACTPWRCON_MASK)
void EMU_BUBuActPwrConSet(EMU_BUBuActPwrCon_TypeDef actPwrCon);
#endif
#if defined(_EMU_BUCTRL_PWRRES_MASK)
void EMU_BUPwrResSet(EMU_BUPwrRes_TypeDef pwrRes);
#endif
#if defined(_EMU_BUCTRL_VOUTRES_MASK)
void EMU_BUVoutResSet(EMU_BUVoutRes_TypeDef resistorSel);
#endif
#if defined(_EMU_BUCTRL_BUVINPROBEEN_MASK)
void EMU_BUBuVinProbeEnSet(bool enable);
#endif
#if defined(_EMU_BUCTRL_STATEN_MASK)
void EMU_BUStatEnSet(bool enable);
#endif
#if defined(_EMU_BUCTRL_EN_MASK)
void EMU_BUEnableSet(bool enable);
#endif

#if defined(EMU_SERIES1_DCDC_BUCK_PRESENT)
void EMU_DCDCModeSet(EMU_DcdcMode_TypeDef dcdcMode);
#endif

#if (defined(EMU_SERIES2_DCDC_BUCK_PRESENT) \
  || defined(EMU_SERIES2_DCDC_BOOST_PRESENT))
sl_status_t EMU_DCDCModeSet(EMU_DcdcMode_TypeDef dcdcMode);
#endif

#if defined(EMU_SERIES2_DCDC_BUCK_PRESENT) \
  || defined(EMU_SERIES2_DCDC_BOOST_PRESENT)
void EMU_DCDCUpdatedHook(void);
#endif

#if defined(EMU_SERIES2_DCDC_BOOST_PRESENT)
bool EMU_DCDCBoostInit(const EMU_DCDCBoostInit_TypeDef *dcdcBoostInit);
void EMU_EM01BoostPeakCurrentSet(const EMU_DcdcBoostEM01PeakCurrent_TypeDef boostPeakCurrentEM01);
void EMU_BoostExternalShutdownEnable(bool enable);
#if defined(_DCDC_CTRL_DVDDBSTPRG_MASK)
void EMU_DCDCBoostOutputVoltageSet(const EMU_DcdcBoostOutputVoltage_TypeDef boostOutputVoltage);
#endif
#endif

#if defined(EMU_SERIES1_DCDC_BUCK_PRESENT) \
  || defined(EMU_SERIES2_DCDC_BUCK_PRESENT)
bool EMU_DCDCInit(const EMU_DCDCInit_TypeDef *dcdcInit);
bool EMU_DCDCPowerOff(void);
#endif

#if defined(EMU_SERIES2_DCDC_BUCK_PRESENT)
void EMU_EM01PeakCurrentSet(const EMU_DcdcPeakCurrent_TypeDef peakCurrentEM01);
#if defined(_DCDC_PFMXCTRL_IPKVAL_MASK)
void EMU_DCDCSetPFMXModePeakCurrent(uint32_t value);
#endif
#if defined(_DCDC_PFMXCTRL_IPKTMAXCTRL_MASK)
void EMU_DCDCSetPFMXTimeoutMaxCtrl(EMU_DcdcTonMaxTimeout_TypeDef value);
#endif
#endif /* EMU_SERIES2_DCDC_BUCK_PRESENT */

#if defined(EMU_SERIES1_DCDC_BUCK_PRESENT)
#if defined(EMU_DCDCCTRL_DCDCMODEEM23)
void EMU_DCDCModeEM23Set(EMU_DcdcModeEM23_TypeDef dcdcModeEM23);
#endif
void EMU_DCDCConductionModeSet(EMU_DcdcConductionMode_TypeDef conductionMode, bool rcoDefaultSet);
bool EMU_DCDCOutputVoltageSet(uint32_t mV, bool setLpVoltage, bool setLnVoltage);
void EMU_DCDCOptimizeSlice(uint32_t em0LoadCurrentmA);
void EMU_DCDCLnRcoBandSet(EMU_DcdcLnRcoBand_TypeDef band);
#endif /* EMU_SERIES1_DCDC_BUCK_PRESENT */

#if defined(EMU_STATUS_VMONRDY)
void EMU_VmonInit(const EMU_VmonInit_TypeDef *vmonInit);
void EMU_VmonHystInit(const EMU_VmonHystInit_TypeDef *vmonInit);
void EMU_VmonEnable(EMU_VmonChannel_TypeDef channel, bool enable);
bool EMU_VmonChannelStatusGet(EMU_VmonChannel_TypeDef channel);
#endif
#if defined(_SILICON_LABS_GECKO_INTERNAL_SDID_80)
void EMU_SetBiasMode(EMU_BiasMode_TypeDef mode);
#endif
#if defined(_EMU_TEMP_TEMP_MASK)
float EMU_TemperatureGet(void);
#endif
#if defined(EMU_CTRL_EFPDIRECTMODEEN)
void EMU_EFPDirectModeEnable(bool enable);
#endif
#if defined(EMU_CTRL_EFPDRVDECOUPLE)
void EMU_EFPDriveDecoupleSet(bool enable);
#endif
#if defined(EMU_CTRL_EFPDRVDVDD)
void EMU_EFPDriveDvddSet(bool enable);
#endif
#if defined(_EMU_CTRL_HDREGEM2EXITCLIM_MASK)
void EMU_HDRegEM2ExitCurrentLimitEnable(bool enable);
#endif
#if defined(_EMU_CTRL_HDREGSTOPGEAR_MASK)
void EMU_HDRegStopGearSet(EMU_HdregStopGearILmt_TypeDef current);
#endif
#if defined(_DCDC_CTRL_MASK)
/***************************************************************************//**
 * @brief
 *   Lock DCDC registers in order to protect them against unintended
 *   modification.
 ******************************************************************************/
__STATIC_INLINE void EMU_DCDCLock(void)
{
  DCDC->LOCK = ~DCDC_LOCK_LOCKKEY_UNLOCKKEY;
}
#endif

#if defined(_DCDC_CTRL_MASK)
/***************************************************************************//**
 * @brief
 *   Unlock the DCDC so that writing to locked registers again is possible.
 ******************************************************************************/
__STATIC_INLINE void EMU_DCDCUnlock(void)
{
  DCDC->LOCK = DCDC_LOCK_LOCKKEY_UNLOCKKEY;
}
#endif

#if defined(_DCDC_SYNCBUSY_MASK)
/***************************************************************************//**
 * @brief
 *   Wait for the DCDC to complete all synchronization of register changes.
 *
 * @param[in] mask
 *   A bitmask corresponding to SYNCBUSY register defined bits indicating
 *   registers that must complete any ongoing synchronization.
 ******************************************************************************/
__STATIC_INLINE void EMU_DCDCSync(uint32_t mask)
{
  while (0UL != (DCDC->SYNCBUSY & mask)) {
    /* Wait for previous synchronization to finish */
  }
}
#endif

#if defined(_SILICON_LABS_32B_SERIES_1)
/***************************************************************************//**
 * @brief
 *   Check status of the internal LDO regulator.
 *
 * @return
 *   Return true if the regulator is on, false if regulator is off.
 ******************************************************************************/
__STATIC_INLINE bool EMU_LDOStatusGet(void)
{
  if ((*(volatile uint32_t*)0x400E303C & 0x00000040UL) == 0UL) {
    return true;
  } else {
    return false;
  }
}
#endif

/***************************************************************************//**
 * @brief
 *   Enter energy mode 1 (EM1).
 *
 * @note
 *   This function is incompatible with the Power Manager module. When the
 *   Power Manager module is present, it must be the one deciding at which
 *   EM level the device sleeps to ensure the application properly works. Using
 *   both at the same time could lead to undefined behavior in the application.
 ******************************************************************************/
__STATIC_INLINE void EMU_EnterEM1(void)
{
  /* Enter sleep mode. */
  SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
  __WFI();
}

#if defined(EMU_VSCALE_EM01_PRESENT)
/***************************************************************************//**
 * @brief
 *   Wait for voltage scaling to complete.
 ******************************************************************************/
__STATIC_INLINE void EMU_VScaleWait(void)
{
#if defined(_SILICON_LABS_32B_SERIES_1)
  if (EMU_LDOStatusGet() == false) {
    /* Skip waiting if the LDO regulator is turned off. */
    return;
  }
#endif

  while (BUS_RegBitRead(&EMU->STATUS, _EMU_STATUS_VSCALEBUSY_SHIFT) != 0U) {
  }
}
#endif

#if defined(EMU_VSCALE_EM01_PRESENT)
/***************************************************************************//**
 * @brief
 *   Get current voltage scaling level.
 *
 * @return
 *    Current voltage scaling level.
 ******************************************************************************/
__STATIC_INLINE EMU_VScaleEM01_TypeDef EMU_VScaleGet(void)
{
  EMU_VScaleWait();
  return (EMU_VScaleEM01_TypeDef)((uint32_t)
                                  ((EMU->STATUS & _EMU_STATUS_VSCALE_MASK)
                                   >> _EMU_STATUS_VSCALE_SHIFT));
}
#endif

#if defined(_EMU_STATUS_VMONRDY_MASK)
/***************************************************************************//**
 * @brief
 *   Get the status of the voltage monitor (VMON).
 *
 * @return
 *   Status of the VMON. True if all the enabled channels are ready, false if
 *   one or more of the enabled channels are not ready.
 ******************************************************************************/
__STATIC_INLINE bool EMU_VmonStatusGet(void)
{
  return BUS_RegBitRead(&EMU->STATUS, _EMU_STATUS_VMONRDY_SHIFT) != 0U;
}
#endif /* _EMU_STATUS_VMONRDY_MASK */

#if defined(_EMU_IF_MASK)
/***************************************************************************//**
 * @brief
 *   Clear one or more pending EMU interrupts.
 *
 * @param[in] flags
 *   Pending EMU interrupt sources to clear. Use one or more valid
 *   interrupt flags for the EMU module (EMU_IFC_nnn or EMU_IF_nnn).
 ******************************************************************************/
__STATIC_INLINE void EMU_IntClear(uint32_t flags)
{
#if defined(EMU_HAS_SET_CLEAR)
  EMU->IF_CLR = flags;
#else
  EMU->IFC = flags;
#endif
}

/***************************************************************************//**
 * @brief
 *   Disable one or more EMU interrupts.
 *
 * @param[in] flags
 *   EMU interrupt sources to disable. Use one or more valid
 *   interrupt flags for the EMU module (EMU_IEN_nnn).
 ******************************************************************************/
__STATIC_INLINE void EMU_IntDisable(uint32_t flags)
{
#if defined(EMU_HAS_SET_CLEAR)
  EMU->IEN_CLR = flags;
#else
  EMU->IEN &= ~flags;
#endif
}

/***************************************************************************//**
 * @brief
 *   Enable one or more EMU interrupts.
 *
 * @note
 *   Depending on the use, a pending interrupt may already be set prior to
 *   enabling the interrupt. To ignore a pending interrupt, consider using
 *   @ref EMU_IntClear() prior to enabling the interrupt.
 *
 * @param[in] flags
 *   EMU interrupt sources to enable. Use one or more valid
 *   interrupt flags for the EMU module (EMU_IEN_nnn).
 ******************************************************************************/
__STATIC_INLINE void EMU_IntEnable(uint32_t flags)
{
#if defined(EMU_HAS_SET_CLEAR)
  EMU->IEN_SET = flags;
#else
  EMU->IEN |= flags;
#endif
}

#if defined(EMU_CTRL_EFPDRVDVDD)
/***************************************************************************//**
 * @brief
 *   Disable one or more EFP interrupts.
 *
 * @param[in] flags
 *   EFP interrupt sources to disable. Use one or more valid
 *   interrupt flags for the EFP module (EFPIENnnn).
 ******************************************************************************/
__STATIC_INLINE void EMU_EFPIntDisable(uint32_t flags)
{
  EMU->EFPIEN_CLR = flags;
}

/***************************************************************************//**
 * @brief
 *   Enable one or more EFP interrupts.
 *
 * @param[in] flags
 *   EFP interrupt sources to enable. Use one or more valid
 *   interrupt flags for the EFP module (EFPIENnnn).
 ******************************************************************************/
__STATIC_INLINE void EMU_EFPIntEnable(uint32_t flags)
{
  EMU->EFPIEN_SET = flags;
}

/***************************************************************************//**
 * @brief
 *   Get pending EMU EFP interrupt flags.
 *
 * @note
 *   Event bits are not cleared by the use of this function.
 *
 * @return
 *   EMU EFP interrupt sources pending. .
 ******************************************************************************/
__STATIC_INLINE uint32_t EMU_EFPIntGet(void)
{
  return EMU->EFPIF;
}

/***************************************************************************//**
 * @brief
 *   Get enabled and pending EMU EFP interrupt flags.
 *   Useful for handling more interrupt sources in the same interrupt handler.
 *
 * @note
 *   Interrupt flags are not cleared by the use of this function.
 *
 * @return
 *   Pending and enabled EMU EFP interrupt sources
 *   Return value is the bitwise AND of
 *   - the enabled interrupt sources in EMU_EFPIEN and
 *   - the pending interrupt flags EMU_EFPIF.
 ******************************************************************************/
__STATIC_INLINE uint32_t EMU_EFPIntGetEnabled(void)
{
  uint32_t ien;

  ien = EMU->EFPIEN;
  return EMU->EFPIF & ien;
}

/***************************************************************************//**
 * @brief
 *   Set one or more pending EMU EFP interrupts.
 *
 * @param[in] flags
 *   EMU EFP interrupt sources to set to pending. Use one or more valid
 *   interrupt flags for the EMU EFP module (EMU_EFPIFSnnn).
 ******************************************************************************/
__STATIC_INLINE void EMU_EFPIntSet(uint32_t flags)
{
  EMU->EFPIF_SET = flags;
}

/***************************************************************************//**
 * @brief
 *   Clear one or more pending EMU EFP interrupts.
 *
 * @param[in] flags
 *   Pending EMU EFP interrupt sources to clear. Use one or more valid
 *   interrupt flags for the EMU EFP module.
 ******************************************************************************/
__STATIC_INLINE void EMU_EFPIntClear(uint32_t flags)
{
  EMU->EFPIF_CLR = flags;
}
#endif

/***************************************************************************//**
 * @brief
 *   Get pending EMU interrupt flags.
 *
 * @note
 *   Event bits are not cleared by the use of this function.
 *
 * @return
 *   EMU interrupt sources pending. Returns one or more valid
 *   interrupt flags for the EMU module (EMU_IF_nnn).
 ******************************************************************************/
__STATIC_INLINE uint32_t EMU_IntGet(void)
{
  return EMU->IF;
}

/***************************************************************************//**
 * @brief
 *   Get enabled and pending EMU interrupt flags.
 *   Useful for handling more interrupt sources in the same interrupt handler.
 *
 * @note
 *   Interrupt flags are not cleared by the use of this function.
 *
 * @return
 *   Pending and enabled EMU interrupt sources
 *   Return value is the bitwise AND of
 *   - the enabled interrupt sources in EMU_IEN and
 *   - the pending interrupt flags EMU_IF.
 ******************************************************************************/
__STATIC_INLINE uint32_t EMU_IntGetEnabled(void)
{
  uint32_t ien;

  ien = EMU->IEN;
  return EMU->IF & ien;
}

/***************************************************************************//**
 * @brief
 *   Set one or more pending EMU interrupts.
 *
 * @param[in] flags
 *   EMU interrupt sources to set to pending. Use one or more valid
 *   interrupt flags for the EMU module (EMU_IFS_nnn).
 ******************************************************************************/
__STATIC_INLINE void EMU_IntSet(uint32_t flags)
{
#if defined(EMU_HAS_SET_CLEAR)
  EMU->IF_SET = flags;
#else
  EMU->IFS = flags;
#endif
}
#endif /* _EMU_IF_MASK */

#if defined(_EMU_EM4CONF_LOCKCONF_MASK)
/***************************************************************************//**
 * @brief
 *   Enable or disable EM4 lock configuration.
 * @param[in] enable
 *   If true, locks down EM4 configuration.
 ******************************************************************************/
__STATIC_INLINE void EMU_EM4Lock(bool enable)
{
  BUS_RegBitWrite(&(EMU->EM4CONF), _EMU_EM4CONF_LOCKCONF_SHIFT, enable);
}
#endif

#if defined(_EMU_STATUS_BURDY_MASK)
/***************************************************************************//**
 * @brief
 *   Halts until backup power functionality is ready.
 ******************************************************************************/
__STATIC_INLINE void EMU_BUReady(void)
{
  while (!(EMU->STATUS & EMU_STATUS_BURDY))
    ;
}
#endif

#if defined(_EMU_ROUTE_BUVINPEN_MASK)
/***************************************************************************//**
 * @brief
 *   Disable BU_VIN support.
 * @param[in] enable
 *   If true, enables BU_VIN input pin support, if false disables it.
 ******************************************************************************/
__STATIC_INLINE void EMU_BUPinEnable(bool enable)
{
  BUS_RegBitWrite(&(EMU->ROUTE), _EMU_ROUTE_BUVINPEN_SHIFT, enable);
}
#endif

/***************************************************************************//**
 * @brief
 *   Lock EMU registers in order to protect them against unintended
 *   modification.
 *
 * @note
 *   If locking EMU registers, they must be unlocked prior to using any
 *   EMU API functions modifying EMU registers, excluding interrupt control
 *   and regulator control if the architecture has a EMU_PWRCTRL register.
 *   An exception to this is the energy mode entering API (EMU_EnterEMn()),
 *   which can be used when the EMU registers are locked.
 ******************************************************************************/
__STATIC_INLINE void EMU_Lock(void)
{
  EMU->LOCK = 0x0;
}

/***************************************************************************//**
 * @brief
 *   Unlock the EMU so that writing to locked registers again is possible.
 ******************************************************************************/
__STATIC_INLINE void EMU_Unlock(void)
{
  EMU->LOCK = EMU_LOCK_LOCKKEY_UNLOCK;
}

#if defined(_EMU_PWRLOCK_MASK)
/***************************************************************************//**
 * @brief
 *   Lock the EMU regulator control registers in order to protect against
 *   unintended modification.
 ******************************************************************************/
__STATIC_INLINE void EMU_PowerLock(void)
{
  EMU->PWRLOCK = EMU_PWRLOCK_LOCKKEY_LOCK;
}

/***************************************************************************//**
 * @brief
 *   Unlock the EMU power control registers so that writing to
 *   locked registers again is possible.
 ******************************************************************************/
__STATIC_INLINE void EMU_PowerUnlock(void)
{
  EMU->PWRLOCK = EMU_PWRLOCK_LOCKKEY_UNLOCK;
}
#endif

#if defined(_EMU_CTRL_EM2BLOCK_MASK)
/***************************************************************************//**
 * @brief
 *   Block entering EM2 or higher number energy modes.
 ******************************************************************************/
__STATIC_INLINE void EMU_EM2Block(void)
{
  BUS_RegBitWrite(&EMU->CTRL, _EMU_CTRL_EM2BLOCK_SHIFT, 1U);
}

/***************************************************************************//**
 * @brief
 *   Unblock entering EM2 or higher number energy modes.
 ******************************************************************************/
__STATIC_INLINE void EMU_EM2UnBlock(void)
{
  BUS_RegBitWrite(&EMU->CTRL, _EMU_CTRL_EM2BLOCK_SHIFT, 0U);
}
#endif

#if defined(_EMU_EM4CTRL_EM4IORETMODE_MASK)
/***************************************************************************//**
 * @brief
 *   When EM4 pin retention is set to emuPinRetentionLatch, then pins are retained
 *   through EM4 entry and wakeup. The pin state is released by calling this function.
 *   The feature allows peripherals or GPIO to be re-initialized after EM4 exit (reset),
 *   and when initialization is done, this function can release pins and return control
 *   to the peripherals or GPIO.
 ******************************************************************************/
__STATIC_INLINE void EMU_UnlatchPinRetention(void)
{
  EMU->CMD = EMU_CMD_EM4UNLATCH;
}
#endif

#if defined(_EMU_TEMP_TEMP_MASK)
/** Zero degrees Celcius in Kelvin */
#define EMU_TEMP_ZERO_C_IN_KELVIN (273.15f)
/***************************************************************************//**
 * @brief
 *   Temperature measurement ready status
 *
 * @return
 *   True if temperature measurement is ready
 ******************************************************************************/
__STATIC_INLINE bool EMU_TemperatureReady(void)
{
#if defined(EMU_STATUS_FIRSTTEMPDONE)
  return (0UL != (EMU->STATUS & EMU_STATUS_FIRSTTEMPDONE));
#else
  return !((EMU->TEMP & _EMU_TEMP_TEMP_MASK) == 0u);
#endif
}

#if defined(_EMU_TEMP_TEMPAVG_MASK)
/***************************************************************************//**
 * @brief
 *   Get averaged temperature in degrees Celsius.
 *
 * @note
 *   An averaged temperature measurement must first be requested by calling
 *   @ref EMU_TemperatureAvgRequest() and waiting for the TEMPAVG interrupt flag
 *   to go high.
 *
 * @return
 *   Averaged temperature
 ******************************************************************************/
__STATIC_INLINE float EMU_TemperatureAvgGet(void)
{
  uint32_t tmp = ((EMU->TEMP & _EMU_TEMP_TEMPAVG_MASK)
                  >> _EMU_TEMP_TEMPAVG_SHIFT);
  return (float)tmp / 4.0f - EMU_TEMP_ZERO_C_IN_KELVIN;
}

/***************************************************************************//**
 * @brief
 *   Request averaged temperature.
 *
 * @note
 *   EMU must be unlocked by calling @ref EMU_Unlock() before this function
 *   can be called.
 *
 * @param[in] numSamples
 *   Number of temperature samples to average
 ******************************************************************************/
__STATIC_INLINE void EMU_TemperatureAvgRequest(EMU_TempAvgNum_TypeDef numSamples)
{
  BUS_RegBitWrite(&EMU->CTRL, _EMU_CTRL_TEMPAVGNUM_SHIFT, (unsigned int)numSamples);
  EMU->CMD = 1u << _EMU_CMD_TEMPAVGREQ_SHIFT;
}

#endif //defined(_EMU_TEMP_TEMPAVG_MASK)
#endif //defined(_EMU_TEMP_TEMP_MASK)

/** @} (end addtogroup emu) */

#ifdef __cplusplus
}
#endif

#endif /* defined( EMU_PRESENT ) */
#endif /* EM_EMU_H */
