/***************************************************************************//**
 * @file
 * @brief Universal synchronous/asynchronous receiver/transmitter (USART/UART)
 *   peripheral API
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

#ifndef EM_USART_H
#define EM_USART_H

#include "em_device.h"
#if defined(USART_COUNT) && (USART_COUNT > 0)

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup usart USART - Synchronous/Asynchronous Serial
 * @brief Universal Synchronous/Asynchronous Receiver/Transmitter
 *   Peripheral API
 * @details
 * The Universal Synchronous/Asynchronous Receiver/Transmitter (USART)
 * is a very flexible serial I/O module. It supports full duplex asynchronous UART
 * communication as well as RS-485, SPI, MicroWire, and 3-wire. It can also interface
 * with ISO7816 Smart-Cards, and IrDA devices.
 *
 * The USART has a wide selection of operating modes, frame formats, and baud rates.
 * All features are supported through the API of this module.
 *
 * Triple buffering and DMA support makes high data-rates possible with minimal
 * CPU intervention. It is possible to transmit and receive large frames while
 * the MCU remains in EM1 Sleep.
 *
 * This module does not support DMA configuration. The UARTDRV and SPIDRV drivers
 * provide full support for DMA and more.
 *
 *  The following steps are necessary for basic operation:
 *
 *  Clock enable:
 *  @include em_usart_clock_enable.c
 *
 *  To initialize the USART for asynchronous operation (e.g., UART):
 *  @include em_usart_init_async.c
 *
 *  To initialize the USART for synchronous operation (e.g., SPI):
 *  @include em_usart_init_sync.c
 *
 *  After pins are assigned for the application/board, enable pins at the
 *  desired location. Available locations can be obtained from the Pin Definitions
 *  section in the data sheet.
 *  @if DOXYDOC_P1_DEVICE
 *  @include em_usart_route_p1.c
 *  @note UART hardware flow control is not directly supported in hardware on
 *        _SILICON_LABS_32B_SERIES_0 parts.
 *  @endif
 *  @if DOXYDOC_P2_DEVICE
 *  @include em_usart_route_p2.c
 *  @endif
 *  @note UARTDRV supports all types of UART flow control. Software assisted
 *        hardware flow control is available for parts without true UART hardware
 *        flow control.
 * @{
 ******************************************************************************/

/*******************************************************************************
 ********************************   ENUMS   ************************************
 ******************************************************************************/

/** Databit selection. */
typedef enum {
  usartDatabits4  = USART_FRAME_DATABITS_FOUR,     /**< 4 data bits (not available for UART). */
  usartDatabits5  = USART_FRAME_DATABITS_FIVE,     /**< 5 data bits (not available for UART). */
  usartDatabits6  = USART_FRAME_DATABITS_SIX,      /**< 6 data bits (not available for UART). */
  usartDatabits7  = USART_FRAME_DATABITS_SEVEN,    /**< 7 data bits (not available for UART). */
  usartDatabits8  = USART_FRAME_DATABITS_EIGHT,    /**< 8 data bits. */
  usartDatabits9  = USART_FRAME_DATABITS_NINE,     /**< 9 data bits. */
  usartDatabits10 = USART_FRAME_DATABITS_TEN,      /**< 10 data bits (not available for UART). */
  usartDatabits11 = USART_FRAME_DATABITS_ELEVEN,   /**< 11 data bits (not available for UART). */
  usartDatabits12 = USART_FRAME_DATABITS_TWELVE,   /**< 12 data bits (not available for UART). */
  usartDatabits13 = USART_FRAME_DATABITS_THIRTEEN, /**< 13 data bits (not available for UART). */
  usartDatabits14 = USART_FRAME_DATABITS_FOURTEEN, /**< 14 data bits (not available for UART). */
  usartDatabits15 = USART_FRAME_DATABITS_FIFTEEN,  /**< 15 data bits (not available for UART). */
  usartDatabits16 = USART_FRAME_DATABITS_SIXTEEN   /**< 16 data bits (not available for UART). */
} USART_Databits_TypeDef;

/** Enable selection. */
typedef enum {
  /** Disable both receiver and transmitter. */
  usartDisable  = 0x0,

  /** Enable receiver only, transmitter disabled. */
  usartEnableRx = USART_CMD_RXEN,

  /** Enable transmitter only, receiver disabled. */
  usartEnableTx = USART_CMD_TXEN,

  /** Enable both receiver and transmitter. */
  usartEnable   = (USART_CMD_RXEN | USART_CMD_TXEN)
} USART_Enable_TypeDef;

/** Oversampling selection, used for asynchronous operation. */
typedef enum {
  usartOVS16 = USART_CTRL_OVS_X16,     /**< 16x oversampling (normal). */
  usartOVS8  = USART_CTRL_OVS_X8,      /**< 8x oversampling. */
  usartOVS6  = USART_CTRL_OVS_X6,      /**< 6x oversampling. */
  usartOVS4  = USART_CTRL_OVS_X4       /**< 4x oversampling. */
} USART_OVS_TypeDef;

/** Parity selection, mainly used for asynchronous operation. */
typedef enum {
  usartNoParity   = USART_FRAME_PARITY_NONE,    /**< No parity. */
  usartEvenParity = USART_FRAME_PARITY_EVEN,    /**< Even parity. */
  usartOddParity  = USART_FRAME_PARITY_ODD      /**< Odd parity. */
} USART_Parity_TypeDef;

/** Stop bits selection, used for asynchronous operation. */
typedef enum {
  usartStopbits0p5 = USART_FRAME_STOPBITS_HALF,        /**< 0.5 stop bits. */
  usartStopbits1   = USART_FRAME_STOPBITS_ONE,         /**< 1 stop bits. */
  usartStopbits1p5 = USART_FRAME_STOPBITS_ONEANDAHALF, /**< 1.5 stop bits. */
  usartStopbits2   = USART_FRAME_STOPBITS_TWO          /**< 2 stop bits. */
} USART_Stopbits_TypeDef;

#if defined(_USART_ROUTEPEN_RTSPEN_MASK) && defined(_USART_ROUTEPEN_CTSPEN_MASK)
/** Hardware Flow Control Selection. */
typedef enum {
  /** No hardware flow control. */
  usartHwFlowControlNone = 0,
  /** CTS signal is enabled for TX flow control. */
  usartHwFlowControlCts = USART_ROUTEPEN_CTSPEN,
  /** RTS signal is enabled for RX flow control. */
  usartHwFlowControlRts = USART_ROUTEPEN_RTSPEN,
  /** CTS and RTS signals are enabled for TX and RX flow control. */
  usartHwFlowControlCtsAndRts = USART_ROUTEPEN_CTSPEN | USART_ROUTEPEN_RTSPEN,
} USART_HwFlowControl_TypeDef;

#elif defined(USART_CTRLX_CTSEN)
/** Hardware Flow Control Selection. */
typedef enum {
  /** No hardware flow control. */
  usartHwFlowControlNone = 0,
  /** CTS signal is enabled for TX flow control. */
  usartHwFlowControlCts,
  /** RTS signal is enabled for RX flow control. */
  usartHwFlowControlRts,
  /** CTS and RTS signals are enabled for TX and RX flow control. */
  usartHwFlowControlCtsAndRts
} USART_HwFlowControl_TypeDef;
#endif

/** Clock polarity/phase mode. */
typedef enum {
  /** Clock idle low, sample on rising edge. */
  usartClockMode0 = USART_CTRL_CLKPOL_IDLELOW | USART_CTRL_CLKPHA_SAMPLELEADING,

  /** Clock idle low, sample on falling edge. */
  usartClockMode1 = USART_CTRL_CLKPOL_IDLELOW | USART_CTRL_CLKPHA_SAMPLETRAILING,

  /** Clock idle high, sample on falling edge. */
  usartClockMode2 = USART_CTRL_CLKPOL_IDLEHIGH | USART_CTRL_CLKPHA_SAMPLELEADING,

  /** Clock idle high, sample on rising edge. */
  usartClockMode3 = USART_CTRL_CLKPOL_IDLEHIGH | USART_CTRL_CLKPHA_SAMPLETRAILING
} USART_ClockMode_TypeDef;

/** Pulse width selection for IrDA mode. */
typedef enum {
  /** IrDA pulse width is 1/16 for OVS=0 and 1/8 for OVS=1 */
  usartIrDAPwONE   = USART_IRCTRL_IRPW_ONE,

  /** IrDA pulse width is 2/16 for OVS=0 and 2/8 for OVS=1 */
  usartIrDAPwTWO   = USART_IRCTRL_IRPW_TWO,

  /** IrDA pulse width is 3/16 for OVS=0 and 3/8 for OVS=1 */
  usartIrDAPwTHREE = USART_IRCTRL_IRPW_THREE,

  /** IrDA pulse width is 4/16 for OVS=0 and 4/8 for OVS=1 */
  usartIrDAPwFOUR  = USART_IRCTRL_IRPW_FOUR
} USART_IrDAPw_Typedef;

/** PRS Channel type */
typedef uint8_t USART_PRS_Channel_t;

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
/** Deprecated PRS channel selector value.
 *  New code should use an integer instead. */
#define usartIrDAPrsCh0       0U
#define usartIrDAPrsCh1       1U
#define usartIrDAPrsCh2       2U
#define usartIrDAPrsCh3       3U
#define usartIrDAPrsCh4       4U
#define usartIrDAPrsCh5       5U
#define usartIrDAPrsCh6       6U
#define usartIrDAPrsCh7       7U
#define usartPrsRxCh0         0U
#define usartPrsRxCh1         1U
#define usartPrsRxCh2         2U
#define usartPrsRxCh3         3U
#define usartPrsRxCh4         4U
#define usartPrsRxCh5         5U
#define usartPrsRxCh6         6U
#define usartPrsRxCh7         7U
#define usartPrsRxCh8         8U
#define usartPrsRxCh9         9U
#define usartPrsRxCh10       10U
#define usartPrsRxCh11       11U
#define usartPrsTriggerCh0    0U
#define usartPrsTriggerCh1    1U
#define usartPrsTriggerCh2    2U
#define usartPrsTriggerCh3    3U
#define usartPrsTriggerCh4    4U
#define usartPrsTriggerCh5    5U
#define usartPrsTriggerCh6    6U
#define usartPrsTriggerCh7    7U
/** @endcond */

#if defined(_USART_I2SCTRL_MASK) && defined(USART_I2SCTRL_I2SEN)
/** I2S format selection. */
typedef enum {
  usartI2sFormatW32D32  = USART_I2SCTRL_I2SFORMAT_W32D32,   /**< 32-bit word, 32-bit data */
  usartI2sFormatW32D24M = USART_I2SCTRL_I2SFORMAT_W32D24M,  /**< 32-bit word, 32-bit data with 8 lsb masked */
  usartI2sFormatW32D24  = USART_I2SCTRL_I2SFORMAT_W32D24,   /**< 32-bit word, 24-bit data */
  usartI2sFormatW32D16  = USART_I2SCTRL_I2SFORMAT_W32D16,   /**< 32-bit word, 16-bit data */
  usartI2sFormatW32D8   = USART_I2SCTRL_I2SFORMAT_W32D8,    /**< 32-bit word, 8-bit data  */
  usartI2sFormatW16D16  = USART_I2SCTRL_I2SFORMAT_W16D16,   /**< 16-bit word, 16-bit data */
  usartI2sFormatW16D8   = USART_I2SCTRL_I2SFORMAT_W16D8,    /**< 16-bit word, 8-bit data  */
  usartI2sFormatW8D8    = USART_I2SCTRL_I2SFORMAT_W8D8      /**<  8-bit word, 8-bit data  */
} USART_I2sFormat_TypeDef;

/** I2S frame data justify. */
typedef enum {
  usartI2sJustifyLeft  = USART_I2SCTRL_I2SJUSTIFY_LEFT,  /**< Data is left-justified within the frame  */
  usartI2sJustifyRight = USART_I2SCTRL_I2SJUSTIFY_RIGHT  /**< Data is right-justified within the frame */
} USART_I2sJustify_TypeDef;

#elif defined(_USART_I2SCTRL_MASK)
/** I2S format selection. */
typedef enum {
  usartI2sFormatW32D32  = USART_I2SCTRL_FORMAT_W32D32,   /**< 32-bit word, 32-bit data. */
  usartI2sFormatW32D24M = USART_I2SCTRL_FORMAT_W32D24M,  /**< 32-bit word, 32-bit data with 8 lsb masked. */
  usartI2sFormatW32D24  = USART_I2SCTRL_FORMAT_W32D24,   /**< 32-bit word, 24-bit data. */
  usartI2sFormatW32D16  = USART_I2SCTRL_FORMAT_W32D16,   /**< 32-bit word, 16-bit data. */
  usartI2sFormatW32D8   = USART_I2SCTRL_FORMAT_W32D8,    /**< 32-bit word, 8-bit data.  */
  usartI2sFormatW16D16  = USART_I2SCTRL_FORMAT_W16D16,   /**< 16-bit word, 16-bit data. */
  usartI2sFormatW16D8   = USART_I2SCTRL_FORMAT_W16D8,    /**< 16-bit word, 8-bit data.  */
  usartI2sFormatW8D8    = USART_I2SCTRL_FORMAT_W8D8      /**<  8-bit word, 8-bit data.  */
} USART_I2sFormat_TypeDef;

/** I2S frame data justify. */
typedef enum {
  usartI2sJustifyLeft  = USART_I2SCTRL_JUSTIFY_LEFT,  /**< Data is left-justified within the frame.  */
  usartI2sJustifyRight = USART_I2SCTRL_JUSTIFY_RIGHT  /**< Data is right-justified within the frame. */
} USART_I2sJustify_TypeDef;
#endif

/*******************************************************************************
 *******************************   STRUCTS   ***********************************
 ******************************************************************************/

/** Asynchronous mode initialization structure. */
typedef struct {
  /** Specifies whether TX and/or RX is enabled when initialization is completed. */
  USART_Enable_TypeDef   enable;

  /**
   * USART/UART reference clock assumed when configuring baud rate setup.
   * Set to 0 to use the currently configured reference clock.
   */
  uint32_t               refFreq;

  /** Desired baud rate. */
  uint32_t               baudrate;

  /** Oversampling used. */
  USART_OVS_TypeDef      oversampling;

  /** Number of data bits in frame. Notice that UART modules only support 8 or
   * 9 data bits. */
  USART_Databits_TypeDef databits;

  /** Parity mode to use. */
  USART_Parity_TypeDef   parity;

  /** Number of stop bits to use. */
  USART_Stopbits_TypeDef stopbits;

#if !defined(_EFM32_GECKO_FAMILY)
  /** Majority Vote Disable for 16x, 8x and 6x oversampling modes. */
  bool                   mvdis;

  /** Enable USART Rx via PRS. */
  bool                   prsRxEnable;

  /** Select PRS channel for USART Rx. (Only valid if prsRxEnable is true). */
  USART_PRS_Channel_t    prsRxCh;
#endif

  /** Auto CS enabling. */
  bool                  autoCsEnable;

  /** Enable CS invert. By default, chip select is active low.
   * Set to true to make chip select active high. */
  bool                  csInv;

#if (_SILICON_LABS_32B_SERIES > 0)
  /** Auto CS hold time in baud cycles. */
  uint8_t               autoCsHold;

  /** Auto CS setup time in baud cycles. */
  uint8_t               autoCsSetup;

  /** Hardware flow control mode. */
  USART_HwFlowControl_TypeDef hwFlowControl;
#endif
} USART_InitAsync_TypeDef;

/** USART PRS trigger enable. */
typedef struct {
#if defined(USART_TRIGCTRL_AUTOTXTEN)
  /** Enable AUTOTX. */
  bool autoTxTriggerEnable;
#endif
  /** Trigger receive via PRS channel. */
  bool rxTriggerEnable;
  /** Trigger transmit via PRS channel. */
  bool txTriggerEnable;
  /** PRS channel to be used to trigger auto transmission. */
  USART_PRS_Channel_t prsTriggerChannel;
} USART_PrsTriggerInit_TypeDef;

/** Default configuration for USART asynchronous initialization structure. */
#if defined(_EFM32_GECKO_FAMILY)
/* Default USART Async struct for the EFM32G device */
#define USART_INITASYNC_DEFAULT                                                                    \
  {                                                                                                \
    usartEnable,           /* Enable RX/TX when initialization is complete. */                     \
    0,                     /* Use current configured reference clock for configuring baud rate. */ \
    115200,                /* 115200 bits/s. */                                                    \
    usartOVS16,            /* 16x oversampling. */                                                 \
    usartDatabits8,        /* 8 data bits. */                                                      \
    usartNoParity,         /* No parity. */                                                        \
    usartStopbits1,        /* 1 stop bit. */                                                       \
    false,                 /* Auto CS functionality enable/disable switch. */                      \
    false,                 /* No CS invert. */                                                     \
  }
#elif defined(_SILICON_LABS_32B_SERIES_0)
/* Default USART Async struct for Series 0 devices */
#define USART_INITASYNC_DEFAULT                                                                    \
  {                                                                                                \
    usartEnable,           /* Enable RX/TX when initialization is complete. */                     \
    0,                     /* Use current configured reference clock for configuring baud rate. */ \
    115200,                /* 115200 bits/s. */                                                    \
    usartOVS16,            /* 16x oversampling. */                                                 \
    usartDatabits8,        /* 8 data bits. */                                                      \
    usartNoParity,         /* No parity. */                                                        \
    usartStopbits1,        /* 1 stop bit. */                                                       \
    false,                 /* Do not disable majority vote. */                                     \
    false,                 /* Not USART PRS input mode. */                                         \
    0,                     /* PRS channel 0. */                                                    \
    false,                 /* Auto CS functionality enable/disable switch. */                      \
    false,                 /* No CS invert. */                                                     \
  }
#elif (_SILICON_LABS_32B_SERIES > 0)
/* Default USART Async struct for Series 1 and Series 2 devices */
#define USART_INITASYNC_DEFAULT                                                                    \
  {                                                                                                \
    usartEnable,           /* Enable RX/TX when initialization is complete. */                     \
    0,                     /* Use current configured reference clock for configuring baud rate. */ \
    115200,                /* 115200 bits/s. */                                                    \
    usartOVS16,            /* 16x oversampling. */                                                 \
    usartDatabits8,        /* 8 data bits. */                                                      \
    usartNoParity,         /* No parity. */                                                        \
    usartStopbits1,        /* 1 stop bit. */                                                       \
    false,                 /* Do not disable majority vote. */                                     \
    false,                 /* Not USART PRS input mode. */                                         \
    0,                     /* PRS channel 0. */                                                    \
    false,                 /* Auto CS functionality enable/disable switch */                       \
    false,                 /* No CS invert. */                                                     \
    0,                     /* Auto CS Hold cycles. */                                              \
    0,                     /* Auto CS Setup cycles. */                                             \
    usartHwFlowControlNone /* No HW flow control. */                                               \
  }
#endif

/** Default configuration for USART PRS triggering structure. */
#if defined(USART_TRIGCTRL_AUTOTXTEN)
#define USART_INITPRSTRIGGER_DEFAULT                            \
  {                                                             \
    false,             /* Do not enable autoTX triggering. */   \
    false,             /* Do not enable receive triggering. */  \
    false,             /* Do not enable transmit triggering. */ \
    0                  /* Set default channel to zero. */       \
  }
#else
#define USART_INITPRSTRIGGER_DEFAULT                            \
  {                                                             \
    false,             /* Do not enable receive triggering. */  \
    false,             /* Do not enable transmit triggering. */ \
    0                  /* Set default channel to zero. */       \
  }
#endif

/** Synchronous mode initialization structure. */
typedef struct {
  /** Specifies whether TX and/or RX shall be enabled when initialization is completed. */
  USART_Enable_TypeDef    enable;

  /**
   * USART/UART reference clock assumed when configuring baud rate setup.
   * Set to 0 to use the currently configured reference clock.
   */
  uint32_t                refFreq;

  /** Desired baud rate. */
  uint32_t                baudrate;

  /** Number of data bits in frame. */
  USART_Databits_TypeDef  databits;

  /** Select if to operate in master or slave mode. */
  bool                    master;

  /** Select if to send most or least significant bit first. */
  bool                    msbf;

  /** Clock polarity/phase mode. */
  USART_ClockMode_TypeDef clockMode;

#if !defined(_EFM32_GECKO_FAMILY)
  /** Enable USART Rx via PRS. */
  bool                    prsRxEnable;

  /** Select PRS channel for USART Rx. (Only valid if prsRxEnable is true). */
  USART_PRS_Channel_t     prsRxCh;
#endif

#if defined(USART_TRIGCTRL_AUTOTXTEN)
  /** Enable AUTOTX mode. Transmits as long as RX is not full.
   *  Generates underflows if TX is empty. */
  bool                    autoTx;
#endif

  /** Auto CS enabling */
  bool                    autoCsEnable;

  /** Enable CS invert. By default, chip select is active low.
   * Set to true to make chip select active high. */
  bool                    csInv;

#if defined(_USART_TIMING_CSHOLD_MASK)
  /** Auto CS hold time in baud cycles */
  uint8_t                 autoCsHold;

  /** Auto CS setup time in baud cycles */
  uint8_t                 autoCsSetup;
#endif
} USART_InitSync_TypeDef;

/** Default configuration for USART sync initialization structure. */
#if defined(_EFM32_GECKO_FAMILY)
/* Default USART Sync configuration for EFM32G devices. */
#define USART_INITSYNC_DEFAULT                                                               \
  {                                                                                          \
    usartEnable,     /* Enable RX/TX when initialization is complete. */                     \
    0,               /* Use current configured reference clock for configuring baud rate. */ \
    1000000,         /* 1 Mbits/s. */                                                        \
    usartDatabits8,  /* 8 data bits. */                                                      \
    true,            /* Master mode. */                                                      \
    false,           /* Send least significant bit first. */                                 \
    usartClockMode0, /* Clock idle low, sample on rising edge. */                            \
    false,           /* No AUTOCS mode. */                                                   \
    false,           /* No CS invert. */                                                     \
  }
#elif defined(_SILICON_LABS_32B_SERIES_0)
/* Default USART Sync configuration for series 0 devices. */
#define USART_INITSYNC_DEFAULT                                                               \
  {                                                                                          \
    usartEnable,     /* Enable RX/TX when initialization is complete. */                     \
    0,               /* Use current configured reference clock for configuring baud rate. */ \
    1000000,         /* 1 Mbits/s. */                                                        \
    usartDatabits8,  /* 8 data bits. */                                                      \
    true,            /* Master mode. */                                                      \
    false,           /* Send least significant bit first. */                                 \
    usartClockMode0, /* Clock idle low, sample on rising edge. */                            \
    false,           /* Not USART PRS input mode. */                                         \
    0,               /* PRS channel 0. */                                                    \
    false,           /* No AUTOTX mode. */                                                   \
    false,           /* No AUTOCS mode. */                                                   \
    false,           /* No CS invert. */                                                     \
  }
#elif (_SILICON_LABS_32B_SERIES > 0)
/* Default USART Sync configuration for series 2 devices */
#define USART_INITSYNC_DEFAULT                                                               \
  {                                                                                          \
    usartEnable,     /* Enable RX/TX when initialization is complete. */                     \
    0,               /* Use current configured reference clock for configuring baud rate. */ \
    1000000,         /* 1 Mbits/s. */                                                        \
    usartDatabits8,  /* 8 databits. */                                                       \
    true,            /* Master mode. */                                                      \
    false,           /* Send least significant bit first. */                                 \
    usartClockMode0, /* Clock idle low, sample on rising edge. */                            \
    false,           /* Not USART PRS input mode. */                                         \
    0,               /* PRS channel 0. */                                                    \
    false,           /* No AUTOTX mode. */                                                   \
    false,           /* No AUTOCS mode. */                                                   \
    false,           /* No CS invert. */                                                     \
    0,               /* Auto CS Hold cycles. */                                              \
    0                /* Auto CS Setup cycles. */                                             \
  }
#endif

/** IrDA mode initialization structure. Inherited from asynchronous mode initialization structure. */
typedef struct {
  /** General Asynchronous initialization structure. */
  USART_InitAsync_TypeDef  async;

  /** Set to invert Rx signal before IrDA demodulator. */
  bool                     irRxInv;

  /** Set to enable filter on IrDA demodulator. */
  bool                     irFilt;

  /** Configure the pulse width generated by the IrDA modulator as a fraction
   *  of the configured USART bit period. */
  USART_IrDAPw_Typedef     irPw;

#if defined(USART_IRCTRL_IRPRSEN)
  /** Enable the PRS channel selected by irPrsSel as input to IrDA module
   *  instead of TX. */
  bool                     irPrsEn;

  /** PRS can be used as input to the pulse modulator instead of TX.
   *  This value selects the channel to use. */
  USART_PRS_Channel_t      irPrsSel;
#endif
} USART_InitIrDA_TypeDef;

/** Default configuration for IrDA mode initialization structure. */
#if defined(_EFM32_GECKO_FAMILY)
/* Default USART IrDA struct for the EFM32G device */
#define USART_INITIRDA_DEFAULT                                                                       \
  {                                                                                                  \
    {                                                                                                \
      usartEnable,           /* Enable RX/TX when initialization is complete. */                     \
      0,                     /* Use current configured reference clock for configuring baud rate. */ \
      115200,                /* 115200 bits/s. */                                                    \
      usartOVS16,            /* 16x oversampling. */                                                 \
      usartDatabits8,        /* 8 data bits. */                                                      \
      usartEvenParity,       /* Even parity. */                                                      \
      usartStopbits1,        /* 1 stop bit. */                                                       \
      false,                 /* Auto CS functionality enable/disable switch */                       \
      false,                 /* No CS invert. */                                                     \
    },                                                                                               \
    false,            /* Rx invert disabled. */                                                      \
    false,            /* Filtering disabled. */                                                      \
    usartIrDAPwTHREE, /* Pulse width is set to ONE. */                                               \
    false,            /* Routing to PRS is disabled. */                                              \
    0                 /* PRS channel 0. */                                                           \
  }
#elif defined(_SILICON_LABS_32B_SERIES_0)
/* Default USART IrDA struct for Series 0 devices */
#define USART_INITIRDA_DEFAULT                                                                       \
  {                                                                                                  \
    {                                                                                                \
      usartEnable,           /* Enable RX/TX when initialization is complete. */                     \
      0,                     /* Use current configured reference clock for configuring baud rate. */ \
      115200,                /* 115200 bits/s. */                                                    \
      usartOVS16,            /* 16x oversampling. */                                                 \
      usartDatabits8,        /* 8 data bits. */                                                      \
      usartEvenParity,       /* Even parity. */                                                      \
      usartStopbits1,        /* 1 stop bit. */                                                       \
      false,                 /* Do not disable majority vote. */                                     \
      false,                 /* Not USART PRS input mode. */                                         \
      0,                     /* PRS channel 0. */                                                    \
      false,                 /* Auto CS functionality enable/disable switch */                       \
      false,                 /* No CS invert. */                                                     \
    },                                                                                               \
    false,            /* Rx invert disabled. */                                                      \
    false,            /* Filtering disabled. */                                                      \
    usartIrDAPwTHREE, /* Pulse width is set to ONE. */                                               \
    false,            /* Routing to PRS is disabled. */                                              \
    0                 /* PRS channel 0. */                                                           \
  }
#elif (_SILICON_LABS_32B_SERIES > 0)
/* Default USART IrDA struct for Series 1 and Series 2 devices */
#if defined(USART_IRCTRL_IRPRSEN)
#define USART_INITIRDA_DEFAULT                                                                       \
  {                                                                                                  \
    {                                                                                                \
      usartEnable,           /* Enable RX/TX when initialization is complete. */                     \
      0,                     /* Use current configured reference clock for configuring baud rate. */ \
      115200,                /* 115200 bits/s. */                                                    \
      usartOVS16,            /* 16x oversampling. */                                                 \
      usartDatabits8,        /* 8 data bits. */                                                      \
      usartEvenParity,       /* Even parity. */                                                      \
      usartStopbits1,        /* 1 stop bit. */                                                       \
      false,                 /* Do not disable majority vote. */                                     \
      false,                 /* Not USART PRS input mode. */                                         \
      0,                     /* PRS channel 0. */                                                    \
      false,                 /* Auto CS functionality enable/disable switch */                       \
      false,                 /* No CS invert. */                                                     \
      0,                     /* Auto CS Hold cycles */                                               \
      0,                     /* Auto CS Setup cycles */                                              \
      usartHwFlowControlNone /* No HW flow control */                                                \
    },                                                                                               \
    false,            /* Rx invert disabled. */                                                      \
    false,            /* Filtering disabled. */                                                      \
    usartIrDAPwTHREE, /* Pulse width is set to ONE. */                                               \
    false,            /* Routing to PRS is disabled. */                                              \
    0                 /* PRS channel 0. */                                                           \
  }
#else
#define USART_INITIRDA_DEFAULT                                                                       \
  {                                                                                                  \
    {                                                                                                \
      usartEnable,           /* Enable RX/TX when initialization is complete. */                     \
      0,                     /* Use current configured reference clock for configuring baud rate. */ \
      115200,                /* 115200 bits/s. */                                                    \
      usartOVS16,            /* 16x oversampling. */                                                 \
      usartDatabits8,        /* 8 data bits. */                                                      \
      usartEvenParity,       /* Even parity. */                                                      \
      usartStopbits1,        /* 1 stop bit. */                                                       \
      false,                 /* Do not disable majority vote. */                                     \
      false,                 /* Not USART PRS input mode. */                                         \
      0,                     /* PRS channel 0. */                                                    \
      false,                 /* Auto CS functionality enable/disable switch */                       \
      false,                 /* No CS invert. */                                                     \
      0,                     /* Auto CS Hold cycles */                                               \
      0,                     /* Auto CS Setup cycles */                                              \
      usartHwFlowControlNone /* No HW flow control */                                                \
    },                                                                                               \
    false,            /* Rx invert disabled. */                                                      \
    false,            /* Filtering disabled. */                                                      \
    usartIrDAPwTHREE  /* Pulse width is set to ONE. */                                               \
  }
#endif
#endif

#if defined(_USART_I2SCTRL_MASK)
/** I2S mode initialization structure. Inherited from synchronous mode initialization structure. */
typedef struct {
  /** General Synchronous initialization structure. */
  USART_InitSync_TypeDef   sync;

  /** I2S mode. */
  USART_I2sFormat_TypeDef  format;

  /** Delay on I2S data. Set to add a one-cycle delay between a transition
   *  on the word-clock and the start of the I2S word.
   *  Should be set for standard I2S format. */
  bool                     delay;

  /** Separate DMA Request For Left/Right Data. */
  bool                     dmaSplit;

  /** Justification of I2S data within the frame. */
  USART_I2sJustify_TypeDef justify;

  /** Stereo or Mono, set to true for mono. */
  bool                     mono;
} USART_InitI2s_TypeDef;

/** Default configuration for I2S mode initialization structure. */
#if defined(_EFM32_GECKO_FAMILY)
/* Default USART Sync configuration for EFM32G devices. */
#define USART_INITI2S_DEFAULT                                                                  \
  {                                                                                            \
    {                                                                                          \
      usartEnable,     /* Enable RX/TX when initialization is complete. */                     \
      0,               /* Use current configured reference clock for configuring baud rate. */ \
      1000000,         /* 1 Mbits/s. */                                                        \
      usartDatabits16, /* 16 databits. */                                                      \
      true,            /* Master mode. */                                                      \
      true,            /* Most significant bit first. */                                       \
      usartClockMode0, /* Clock idle low, sample on rising edge. */                            \
      false,           /* No AUTOCS mode */                                                    \
      false,           /* No CS invert. */                                                     \
    },                                                                                         \
    usartI2sFormatW16D16, /* 16-bit word, 16-bit data */                                       \
    true,               /* Delay on I2S data. */                                               \
    false,              /* No DMA split. */                                                    \
    usartI2sJustifyLeft,/* Data is left-justified within the frame */                          \
    false               /* Stereo mode. */                                                     \
  }
#elif defined(_SILICON_LABS_32B_SERIES_0)
/* Default USART Sync configuration for series 0 devices. */
#define USART_INITI2S_DEFAULT                                                                  \
  {                                                                                            \
    {                                                                                          \
      usartEnable,     /* Enable RX/TX when initialization is complete. */                     \
      0,               /* Use current configured reference clock for configuring baud rate. */ \
      1000000,         /* 1 Mbits/s. */                                                        \
      usartDatabits16, /* 16 databits. */                                                      \
      true,            /* Master mode. */                                                      \
      true,            /* Most significant bit first. */                                       \
      usartClockMode0, /* Clock idle low, sample on rising edge. */                            \
      false,           /* Not USART PRS input mode. */                                         \
      0,               /* PRS channel 0. */                                                    \
      false,           /* No AUTOTX mode. */                                                   \
      false,           /* No AUTOCS mode */                                                    \
      false,           /* No CS invert. */                                                     \
    },                                                                                         \
    usartI2sFormatW16D16, /* 16-bit word, 16-bit data */                                       \
    true,               /* Delay on I2S data. */                                               \
    false,              /* No DMA split. */                                                    \
    usartI2sJustifyLeft,/* Data is left-justified within the frame */                          \
    false               /* Stereo mode. */                                                     \
  }
#elif (_SILICON_LABS_32B_SERIES > 0)
/* Default USART Sync configuration for series 2 devices */
#define USART_INITI2S_DEFAULT                                                                  \
  {                                                                                            \
    {                                                                                          \
      usartEnableTx,    /* Enable TX when init completed. */                                   \
      0,                /* Use current configured reference clock for configuring baudrate. */ \
      1000000,          /* Baudrate 1M bits/s. */                                              \
      usartDatabits16,  /* 16 databits. */                                                     \
      true,             /* Operate as I2S master. */                                           \
      true,             /* Most significant bit first. */                                      \
      usartClockMode0,  /* Clock idle low, sample on rising edge. */                           \
      false,            /* Don't enable USARTRx via PRS. */                                    \
      usartPrsRxCh0,    /* PRS channel selection (dummy). */                                   \
      false,            /* Disable AUTOTX mode. */                                             \
      false,            /* No AUTOCS mode */                                                   \
      false,            /* No CS invert. */                                                    \
      0,                /* Auto CS Hold cycles */                                              \
      0                 /* Auto CS Setup cycles */                                             \
    },                                                                                         \
    usartI2sFormatW16D16, /* 16-bit word, 16-bit data */                                       \
    true,               /* Delay on I2S data. */                                               \
    false,              /* No DMA split. */                                                    \
    usartI2sJustifyLeft,/* Data is left-justified within the frame */                          \
    false               /* Stereo mode. */                                                     \
  }
#endif
#endif

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

void USART_BaudrateAsyncSet(USART_TypeDef *usart,
                            uint32_t refFreq,
                            uint32_t baudrate,
                            USART_OVS_TypeDef ovs);
uint32_t USART_BaudrateCalc(uint32_t refFreq,
                            uint32_t clkdiv,
                            bool syncmode,
                            USART_OVS_TypeDef ovs);
uint32_t USART_BaudrateGet(USART_TypeDef *usart);
void USART_BaudrateSyncSet(USART_TypeDef *usart,
                           uint32_t refFreq,
                           uint32_t baudrate);
void USART_Enable(USART_TypeDef *usart, USART_Enable_TypeDef enable);

void USART_InitAsync(USART_TypeDef *usart, const USART_InitAsync_TypeDef *init);
void USART_InitSync(USART_TypeDef *usart, const USART_InitSync_TypeDef *init);
void USARTn_InitIrDA(USART_TypeDef *usart, const USART_InitIrDA_TypeDef *init);

#if defined(_USART_I2SCTRL_MASK)
void USART_InitI2s(USART_TypeDef *usart, USART_InitI2s_TypeDef *init);
#endif
void USART_InitPrsTrigger(USART_TypeDef *usart, const USART_PrsTriggerInit_TypeDef *init);

/***************************************************************************//**
 * @brief
 *   Clear one or more pending USART interrupts.
 *
 * @param[in] usart
 *   Pointer to the USART/UART peripheral register block.
 *
 * @param[in] flags
 *   Pending USART/UART interrupt source(s) to clear. Use one or more valid
 *   interrupt flags for the USART module (USART_IF_nnn) OR'ed together.
 ******************************************************************************/
__STATIC_INLINE void USART_IntClear(USART_TypeDef *usart, uint32_t flags)
{
#if defined (USART_HAS_SET_CLEAR)
  usart->IF_CLR = flags;
#else
  usart->IFC = flags;
#endif
}

/***************************************************************************//**
 * @brief
 *   Disable one or more USART interrupts.
 *
 * @param[in] usart
 *   Pointer to the USART/UART peripheral register block.
 *
 * @param[in] flags
 *   USART/UART interrupt source(s) to disable. Use one or more valid
 *   interrupt flags for the USART module (USART_IF_nnn) OR'ed together.
 ******************************************************************************/
__STATIC_INLINE void USART_IntDisable(USART_TypeDef *usart, uint32_t flags)
{
#if defined (USART_HAS_SET_CLEAR)
  usart->IEN_CLR = flags;
#else
  usart->IEN &= ~flags;
#endif
}

/***************************************************************************//**
 * @brief
 *   Enable one or more USART interrupts.
 *
 * @note
 *   Depending on the use, a pending interrupt may already be set prior to
 *   enabling the interrupt. To ignore a pending interrupt, consider using
 *   USART_IntClear() prior to enabling the interrupt.
 *
 * @param[in] usart
 *   Pointer to the USART/UART peripheral register block.
 *
 * @param[in] flags
 *   USART/UART interrupt source(s) to enable. Use one or more valid
 *   interrupt flags for the USART module (USART_IF_nnn) OR'ed together.
 ******************************************************************************/
__STATIC_INLINE void USART_IntEnable(USART_TypeDef *usart, uint32_t flags)
{
#if defined (USART_HAS_SET_CLEAR)
  usart->IEN_SET = flags;
#else
  usart->IEN |= flags;
#endif
}

/***************************************************************************//**
 * @brief
 *   Get pending USART interrupt flags.
 *
 * @note
 *   The event bits are not cleared by the use of this function.
 *
 * @param[in] usart
 *   Pointer to the USART/UART peripheral register block.
 *
 * @return
 *   USART/UART interrupt source(s) pending. Returns one or more valid
 *   interrupt flags for the USART module (USART_IF_nnn) OR'ed together.
 ******************************************************************************/
__STATIC_INLINE uint32_t USART_IntGet(USART_TypeDef *usart)
{
  return usart->IF;
}

/***************************************************************************//**
 * @brief
 *   Get enabled and pending USART interrupt flags.
 *   Useful for handling more interrupt sources in the same interrupt handler.
 *
 * @param[in] usart
 *   Pointer to the USART/UART peripheral register block.
 *
 * @note
 *   Interrupt flags are not cleared by the use of this function.
 *
 * @return
 *   Pending and enabled USART interrupt sources.
 *   The return value is the bitwise AND combination of
 *   - the OR combination of enabled interrupt sources in USARTx_IEN_nnn
 *     register (USARTx_IEN_nnn) and
 *   - the OR combination of valid interrupt flags of the USART module
 *     (USARTx_IF_nnn).
 ******************************************************************************/
__STATIC_INLINE uint32_t USART_IntGetEnabled(USART_TypeDef *usart)
{
  uint32_t ien;

  /* Store USARTx->IEN in temporary variable in order to define explicit order
   * of volatile accesses. */
  ien = usart->IEN;

  /* Bitwise AND of pending and enabled interrupts. */
  return usart->IF & ien;
}

/***************************************************************************//**
 * @brief
 *   Set one or more pending USART interrupts from SW.
 *
 * @param[in] usart
 *   Pointer to the USART/UART peripheral register block.
 *
 * @param[in] flags
 *   USART/UART interrupt source(s) to set to pending. Use one or more valid
 *   interrupt flags for the USART module (USART_IF_nnn) OR'ed together.
 ******************************************************************************/
__STATIC_INLINE void USART_IntSet(USART_TypeDef *usart, uint32_t flags)
{
#if defined (USART_HAS_SET_CLEAR)
  usart->IF_SET = flags;
#else
  usart->IFS = flags;
#endif
}

/***************************************************************************//**
 * @brief
 *   Get USART STATUS register.
 *
 * @param[in] usart
 *   Pointer to the USART/UART peripheral register block.
 *
 * @return
 *  STATUS register value.
 *
 ******************************************************************************/
__STATIC_INLINE uint32_t USART_StatusGet(USART_TypeDef *usart)
{
  return usart->STATUS;
}

void USART_Reset(USART_TypeDef *usart);
uint8_t USART_Rx(USART_TypeDef *usart);
uint16_t USART_RxDouble(USART_TypeDef *usart);
uint32_t USART_RxDoubleExt(USART_TypeDef *usart);
uint16_t USART_RxExt(USART_TypeDef *usart);

/***************************************************************************//**
 * @brief
 *   Receive one 4-8 bit frame, (or part of 10-16 bit frame).
 *
 * @details
 *   This function is used to quickly receive one 4-8 bits frame by reading the
 *   RXDATA register directly, without checking the STATUS register for the
 *   RXDATAV flag. This can be useful from the RXDATAV interrupt handler,
 *   i.e., waiting is superfluous, in order to quickly read the received data.
 *   Please refer to @ref USART_RxDataXGet() for reception of 9 bit frames.
 *
 * @note
 *   Because this function does not check whether the RXDATA register actually
 *   holds valid data, it should only be used in situations when it is certain
 *   that there is valid data, ensured by some external program routine, e.g.,
 *   when handling an RXDATAV interrupt. The @ref USART_Rx() is normally a
 *   better choice if the validity of the RXDATA register is not certain.
 *
 * @note
 *   Notice that possible parity/stop bits in asynchronous mode are not
 *   considered part of specified frame bit length.
 *
 * @param[in] usart
 *   Pointer to USART/UART peripheral register block.
 *
 * @return
 *   Data received.
 ******************************************************************************/
__STATIC_INLINE uint8_t USART_RxDataGet(USART_TypeDef *usart)
{
  return (uint8_t)usart->RXDATA;
}

/***************************************************************************//**
 * @brief
 *   Receive two 4-8 bit frames, or one 10-16 bit frame.
 *
 * @details
 *   This function is used to quickly receive one 10-16 bits frame or two 4-8
 *   bit frames by reading the RXDOUBLE register directly, without checking
 *   the STATUS register for the RXDATAV flag. This can be useful from the
 *   RXDATAV interrupt handler, i.e., waiting is superfluous, in order to
 *   quickly read the received data.
 *   This function is normally used to receive one frame when operating with
 *   frame length 10-16 bits. Please refer to @ref USART_RxDoubleXGet()
 *   for reception of two 9 bit frames.
 *
 * @note
 *   Because this function does not check whether the RXDOUBLE register actually
 *   holds valid data, it should only be used in situations when it is certain
 *   that there is valid data, ensured by some external program routine, e.g.,
 *   when handling an RXDATAV interrupt. The @ref USART_RxDouble() is
 *   normally a better choice if the validity of the RXDOUBLE register is not
 *   certain.
 *
 * @note
 *   Notice that possible parity/stop bits in asynchronous mode are not
 *   considered part of specified frame bit length.
 *
 * @param[in] usart
 *   Pointer to USART/UART peripheral register block.
 *
 * @return
 *   Data received.
 ******************************************************************************/
__STATIC_INLINE uint16_t USART_RxDoubleGet(USART_TypeDef *usart)
{
  return (uint16_t)usart->RXDOUBLE;
}

/***************************************************************************//**
 * @brief
 *   Receive two 4-9 bit frames, or one 10-16 bit frame with extended
 *   information.
 *
 * @details
 *   This function is used to quickly receive one 10-16 bits frame or two 4-9
 *   bit frames by reading the RXDOUBLEX register directly, without checking
 *   the STATUS register for the RXDATAV flag. This can be useful from the
 *   RXDATAV interrupt handler, i.e., waiting is superfluous, in order to
 *   quickly read the received data.
 *
 * @note
 *   Because this function does not check whether the RXDOUBLEX register actually
 *   holds valid data, it should only be used in situations when it is certain
 *   that there is valid data, ensured by some external program routine, e.g.,
 *   when handling an RXDATAV interrupt. The @ref USART_RxDoubleExt() is
 *   normally a better choice if the validity of the RXDOUBLEX register is not
 *   certain.
 *
 * @note
 *   Notice that possible parity/stop bits in asynchronous mode are not
 *   considered part of specified frame bit length.
 *
 * @param[in] usart
 *   Pointer to USART/UART peripheral register block.
 *
 * @return
 *   Data received.
 ******************************************************************************/
__STATIC_INLINE uint32_t USART_RxDoubleXGet(USART_TypeDef *usart)
{
  return usart->RXDOUBLEX;
}

/***************************************************************************//**
 * @brief
 *   Receive one 4-9 bit frame, (or part of 10-16 bit frame) with extended
 *   information.
 *
 * @details
 *   This function is used to quickly receive one 4-9 bit frame, (or part of
 *   10-16 bit frame) with extended information by reading the RXDATAX register
 *   directly, without checking the STATUS register for the RXDATAV flag. This
 *   can be useful from the RXDATAV interrupt handler, i.e., waiting is
 *   superfluous, in order to quickly read the received data.
 *
 * @note
 *   Because this function does not check whether the RXDATAX register actually
 *   holds valid data, it should only be used in situations when it is certain
 *   that there is valid data, ensured by some external program routine, e.g.,
 *   when handling an RXDATAV interrupt. The @ref USART_RxExt() is normally
 *   a better choice if the validity of the RXDATAX register is not certain.
 *
 * @note
 *   Notice that possible parity/stop bits in asynchronous mode are not
 *   considered part of specified frame bit length.
 *
 * @param[in] usart
 *   Pointer to USART/UART peripheral register block.
 *
 * @return
 *   Data received.
 ******************************************************************************/
__STATIC_INLINE uint16_t USART_RxDataXGet(USART_TypeDef *usart)
{
  return (uint16_t)usart->RXDATAX;
}

uint8_t USART_SpiTransfer(USART_TypeDef *usart, uint8_t data);
void USART_Tx(USART_TypeDef *usart, uint8_t data);
void USART_TxDouble(USART_TypeDef *usart, uint16_t data);
void USART_TxDoubleExt(USART_TypeDef *usart, uint32_t data);
void USART_TxExt(USART_TypeDef *usart, uint16_t data);

/** @} (end addtogroup usart) */

#ifdef __cplusplus
}
#endif

#endif /* defined(USART_COUNT) && (USART_COUNT > 0) */
#endif /* EM_USART_H */
