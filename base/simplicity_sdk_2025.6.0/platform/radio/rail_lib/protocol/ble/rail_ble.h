/***************************************************************************//**
 * @file
 * @brief The BLE specific header file for the RAIL library.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef __RAIL_BLE_H__
#define __RAIL_BLE_H__

// Get the standard include types
#include <stdint.h>
#include <stdbool.h>

// Get the RAIL specific structures and types
#include "rail_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/// @addtogroup BLE
/// @ingroup Protocol_Specific
/// Accelerator routines for Bluetooth Low Energy (BLE).
///
/// The APIs in this module configure the radio for BLE
/// operation and provide additional helper routines necessary for
/// normal BLE send/receive that aren't available directly in RAIL.
/// RAIL APIs should be used to set up the application. However,
/// \ref RAIL_ConfigChannels() should not be called to set up
/// the PHY. Instead, RAIL_BLE_Config* APIs should be used to set up the
/// 1 Mbps, 2 Mbps, or Coded PHY configurations needed by the application. These
/// APIs will configure the hardware and also configure the set of valid BLE
/// channels.
///
/// To implement a standard BLE link layer, you will also need to handle tight
/// turnaround times and send packets at specific instants. This can all be
/// managed through general RAIL functions, such as \ref RAIL_StartScheduledTx(),
/// \ref RAIL_ScheduleRx(), and \ref RAIL_SetStateTiming(). See RAIL APIs for more
/// useful functions.
///
/// A simple example to set up the application to be in BLE mode is shown
/// below. Note that this will put the radio on the first advertising channel
/// with the advertising Access Address. In any full-featured BLE application you
/// will need to use the \ref RAIL_BLE_ConfigChannelRadioParams() function to change
/// the sync word and other parameters as needed based on your connection.
/// @code{.c}
/// // RAIL handle set at initialization time.
/// static RAIL_Handle_t gRailHandle = NULL;
///
/// static void radioEventHandler(RAIL_Handle_t railHandle,
///                               RAIL_Events_t events)
/// {
///   // ... handle RAIL events, e.g., receive and transmit completion
/// }
///
/// // Set the radio to receive on the first BLE advertising channel.
/// void bleAdvertiseEnable(void)
/// {
///   RAIL_Config_t railCfg = {
///     .eventsCallback = &radioEventHandler,
///   };
///
///   // Initializes the RAIL library and any internal state it requires.
///   gRailHandle = RAIL_Init(&railCfg, NULL);
///
///   // Calls the BLE initialization function to load the right radio configuration.
///   RAIL_BLE_Init(gRailHandle);
///
///   // Always choose the Viterbi PHY configuration if available on your chip
///   // for performance reasons.
///   RAIL_BLE_ConfigPhy1MbpsViterbi(gRailHandle);
///
///   // Configures us for the first advertising channel (Physical: 0, Logical: 37).
///   // The CRC init value and Access Address come from the BLE specification.
///   RAIL_BLE_ConfigChannelRadioParams(gRailHandle,
///                                     0x555555,
///                                     0x8E89BED6,
///                                     37,
///                                     false);
///
///   // Starts receiving on physical channel 0 (logical channel 37).
///   RAIL_StartRx(gRailHandle, 0, NULL);
///  }
/// @endcode
///
/// @{

/**
 * @enum RAIL_BLE_Coding_t
 * @brief The variant of the BLE Coded PHY.
 */
RAIL_ENUM(RAIL_BLE_Coding_t) {
  /** Enables the 125 kbps variant of the BLE Coded PHY. */
  RAIL_BLE_Coding_125kbps = 0,
  /** @deprecated Will be removed in a future version of RAIL. */
  RAIL_BLE_Coding_125kbps_DSA = 1,
  /** Enables the 500 kbps variant of the BLE Coded PHY. */
  RAIL_BLE_Coding_500kbps = 2,
  /** @deprecated Will be removed in a future version of RAIL. */
  RAIL_BLE_Coding_500kbps_DSA = 3,
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// Self-referencing defines minimize compiler complaints when using RAIL_ENUM
#define RAIL_BLE_Coding_125kbps     ((RAIL_BLE_Coding_t) RAIL_BLE_Coding_125kbps)
#define RAIL_BLE_Coding_125kbps_DSA ((RAIL_BLE_Coding_t) RAIL_BLE_Coding_125kbps_DSA)
#define RAIL_BLE_Coding_500kbps     ((RAIL_BLE_Coding_t) RAIL_BLE_Coding_500kbps)
#define RAIL_BLE_Coding_500kbps_DSA ((RAIL_BLE_Coding_t) RAIL_BLE_Coding_500kbps_DSA)
#endif //DOXYGEN_SHOULD_SKIP_THIS

/**
 * @enum RAIL_BLE_Phy_t
 * @brief The variant of the BLE PHY.
 */
RAIL_ENUM(RAIL_BLE_Phy_t) {
  /** BLE 1 Mbps PHY. */
  RAIL_BLE_1Mbps = 0U,
  /** BLE 2 Mbps PHY. */
  RAIL_BLE_2Mbps = 1U,
  /** BLE 125 kbps coded PHY. */
  RAIL_BLE_Coded125kbps = 2U,
  /** BLE 500 kbps coded PHY. */
  RAIL_BLE_Coded500kbps = 3U,
  /** BLE Simulscan PHY. */
  RAIL_BLE_Simulscan = 4U,
  /** BLE 2 Mbps PHY with AoX functionality. */
  RAIL_BLE_AOX2mbps = 5U,
  /** BLE 1 Mbps Quuppa PHY. */
  RAIL_BLE_Quuppa1Mbps = 6U,
  /** BLE 1 Mbps PHY with CS. */
  RAIL_BLE_CS1Mbps = 7U,
  /** BLE 2 Mbps PHY with CS. */
  RAIL_BLE_CS2Mbps = 8U,
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// Self-referencing defines minimize compiler complaints when using RAIL_ENUM
#define RAIL_BLE_1Mbps        ((RAIL_BLE_Phy_t) RAIL_BLE_1Mbps)
#define RAIL_BLE_2Mbps        ((RAIL_BLE_Phy_t) RAIL_BLE_2Mbps)
#define RAIL_BLE_Coded125kbps ((RAIL_BLE_Phy_t) RAIL_BLE_Coded125kbps)
#define RAIL_BLE_Coded500kbps ((RAIL_BLE_Phy_t) RAIL_BLE_Coded500kbps)
#define RAIL_BLE_Simulscan    ((RAIL_BLE_Phy_t) RAIL_BLE_Simulscan)
#define RAIL_BLE_AOX2Mbps     ((RAIL_BLE_Phy_t) RAIL_BLE_AOX2Mbps)
#define RAIL_BLE_Quuppa1Mbps  ((RAIL_BLE_Phy_t) RAIL_BLE_Quuppa1Mbps)
#define RAIL_BLE_CS1Mbps      ((RAIL_BLE_Phy_t) RAIL_BLE_CS1Mbps)
#define RAIL_BLE_CS2Mbps      ((RAIL_BLE_Phy_t) RAIL_BLE_CS2Mbps)
#endif //DOXYGEN_SHOULD_SKIP_THIS

/// @addtogroup BLE_PHY BLE Radio Configurations
/// Radio configurations for the RAIL BLE Accelerator
///
/// These radio configurations are used to configure BLE when a function such
/// as \ref RAIL_BLE_ConfigPhy1MbpsViterbi() is called. Each radio
/// configuration listed below is compiled into the RAIL library as a weak
/// symbol that will take into account per-die defaults. If the board
/// configuration in use has different settings than the default, such as a
/// different radio subsystem clock frequency, these radio configurations can
/// be overriden to account for those settings.
/// @{

/**
 * Default PHY to use for BLE 1 Mbps non-Viterbi. Will be NULL if
 * \ref RAIL_BLE_SUPPORTS_1MBPS_NON_VITERBI is 0.
 */
extern const RAIL_ChannelConfig_t *const RAIL_BLE_Phy1Mbps;

/**
 * Default PHY to use for BLE 2 Mbps non-Viterbi. Will be NULL if
 * \ref RAIL_BLE_SUPPORTS_2MBPS_NON_VITERBI is 0.
 */
extern const RAIL_ChannelConfig_t *const RAIL_BLE_Phy2Mbps;

/**
 * Default PHY to use for BLE 1 Mbps Viterbi. Will be NULL if
 * \ref RAIL_BLE_SUPPORTS_1MBPS_VITERBI is 0.
 */
extern const RAIL_ChannelConfig_t *const RAIL_BLE_Phy1MbpsViterbi;

/**
 * Default PHY to use for BLE 2 Mbps Viterbi. Will be NULL if
 * \ref RAIL_BLE_SUPPORTS_2MBPS_VITERBI is 0.
 */
extern const RAIL_ChannelConfig_t *const RAIL_BLE_Phy2MbpsViterbi;

#ifndef DOXYGEN_UNDOCUMENTED
/**
 * Default PHY to use for BLE 1M Viterbi CS. Will be NULL if
 * \ref RAIL_BLE_SUPPORTS_CS is 0. On EFR32xG24, this will also
 * be NULL for non 40MHz HFXO frequencies.
 */
extern const RAIL_ChannelConfig_t *const RAIL_BLE_Phy1MbpsViterbiCs;

/**
 * Default PHY to use for BLE 2M Viterbi CS. Will be NULL if
 * \ref RAIL_BLE_SUPPORTS_CS is 0. On EFR32xG24, this will also
 * be NULL for non 40MHz HFXO frequencies.
 */
extern const RAIL_ChannelConfig_t *const RAIL_BLE_Phy2MbpsViterbiCs;
#endif//DOXYGEN_UNDOCUMENTED

/**
 * PHY to use for BLE 2 Mbps with AoX functionality. Will be NULL if either
 * \ref RAIL_BLE_SUPPORTS_2MBPS_VITERBI or \ref RAIL_BLE_SUPPORTS_AOX is 0.
 */
extern const RAIL_ChannelConfig_t *const RAIL_BLE_Phy2MbpsAox;

/**
 * Default PHY to use for BLE Coded 125 kbps. Will be NULL if
 * \ref RAIL_BLE_SUPPORTS_CODED_PHY is 0. This PHY can receive on both
 * 125 kbps and 500 kbps BLE Coded, but will only transmit at 125 kbps.
 */
extern const RAIL_ChannelConfig_t *const RAIL_BLE_Phy125kbps;

/**
 * Default PHY to use for BLE Coded 500 kbps. Will be NULL if
 * \ref RAIL_BLE_SUPPORTS_CODED_PHY is 0. This PHY can receive on both
 * 125 kbps and 500 kbps BLE Coded, but will only transmit at 125 kbps.
 */
extern const RAIL_ChannelConfig_t *const RAIL_BLE_Phy500kbps;

/**
 * Default PHY to use for BLE Simulscan. Will be NULL if
 * \ref RAIL_BLE_SUPPORTS_SIMULSCAN_PHY is 0. This PHY can receive on 1 Mbps
 * as well as 125 kbps and 500 kbps BLE Coded, but will only transmit at 1 Mbps.
 */
extern const RAIL_ChannelConfig_t *const RAIL_BLE_PhySimulscan;

/**
 * Default 1 Mbps Quuppa PHY. Will be NULL if
 * \ref RAIL_BLE_SUPPORTS_QUUPPA is 0.
 */
extern const RAIL_ChannelConfig_t *const RAIL_BLE_PhyQuuppa;

/// @} // End of group BLE_PHY

/** \ref RAIL_RxPacketDetails_t::subPhyId indicating a 500 kbps packet. */
#define RAIL_BLE_RX_SUBPHY_ID_500K     (0U)
/** \ref RAIL_RxPacketDetails_t::subPhyId indicating a 125 kbps packet. */
#define RAIL_BLE_RX_SUBPHY_ID_125K     (1U)
/** \ref RAIL_RxPacketDetails_t::subPhyId value indicating a 1 Mbps packet. */
#define RAIL_BLE_RX_SUBPHY_ID_1M       (2U)
/** \ref RAIL_RxPacketDetails_t::subPhyId invalid value. */
#define RAIL_BLE_RX_SUBPHY_ID_INVALID  (3U)
/** The total count of BLE subPhyId's. Must be last. */
#define RAIL_BLE_RX_SUBPHY_COUNT       (4U)

/**
 * @enum RAIL_BLE_SignalIdentifierMode_t
 * @brief Available Signal Identifier modes.
 */
RAIL_ENUM(RAIL_BLE_SignalIdentifierMode_t) {
  /** Disable signal detection mode. */
  RAIL_BLE_SIGNAL_IDENTIFIER_MODE_DISABLE = 0,
  /** BLE 1 Mbps (GFSK) detection mode. */
  RAIL_BLE_SIGNAL_IDENTIFIER_MODE_1MBPS = 1,
  /** BLE 2 Mbps (GFSK) detection mode. */
  RAIL_BLE_SIGNAL_IDENTIFIER_MODE_2MBPS = 2,
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// Self-referencing defines minimize compiler complaints when using RAIL_ENUM
#define RAIL_BLE_SIGNAL_IDENTIFIER_MODE_DISABLE ((RAIL_BLE_SignalIdentifierMode_t) RAIL_BLE_SIGNAL_IDENTIFIER_MODE_DISABLE)
#define RAIL_BLE_SIGNAL_IDENTIFIER_MODE_1MBPS   ((RAIL_BLE_SignalIdentifierMode_t) RAIL_BLE_SIGNAL_IDENTIFIER_MODE_1MBPS)
#define RAIL_BLE_SIGNAL_IDENTIFIER_MODE_2MBPS   ((RAIL_BLE_SignalIdentifierMode_t) RAIL_BLE_SIGNAL_IDENTIFIER_MODE_2MBPS)
#endif//DOXYGEN_SHOULD_SKIP_THIS

/**
 * @struct RAIL_BLE_State_t
 * @brief A structure for BLE radio state parameters.
 */
typedef struct RAIL_BLE_State {
  /** The value used to initialize the CRC algorithm. */
  uint32_t crcInit;
  /**
   * The access address used for the connection.
   * It is transmitted or received least-significant bit first.
   */
  uint32_t accessAddress;
  /** The logical channel used. */
  uint16_t channel;
  /** Indicates whether the whitening engine should be off (generally used for testing only). */
  bool disableWhitening;
  /** Reserved for future use; specify 0. */
  uint16_t whiteInit;
} RAIL_BLE_State_t;

#ifndef SLI_LIBRAIL_ALIAS

/**
 * Configure RAIL to run in BLE mode.
 *
 * @param[in] railHandle A handle for RAIL instance.
 * @return Status code indicating success of the function call.
 *
 * This function changes your radio, channel configuration, and other
 * parameters to match what is needed for BLE, initially establishing
 * the BLE 1 Mbps PHY. To switch back to a
 * default RAIL mode, call \ref RAIL_BLE_Deinit() first. This function
 * will configure the protocol output on PTI to \ref RAIL_PTI_PROTOCOL_BLE.
 *
 * @note BLE may not be enabled while Auto-ACKing is enabled.
 */
RAIL_Status_t RAIL_BLE_Init(RAIL_Handle_t railHandle);

/**
 * Take RAIL out of BLE mode.
 *
 * @param[in] railHandle A handle for RAIL instance.
 * @return Status code indicating success of the function call.
 *
 * This function will undo some of the configuration that happens when you call
 * \ref RAIL_BLE_Init(). After this you can safely run your normal radio
 * initialization code to use a non-BLE configuration. This function does \b
 * not change back your radio or channel configurations so you must do this by
 * manually reinitializing. This also resets the protocol output on PTI to \ref
 * RAIL_PTI_PROTOCOL_CUSTOM.
 */
RAIL_Status_t RAIL_BLE_Deinit(RAIL_Handle_t railHandle);

/**
 * Determine whether BLE mode is enabled or not.
 *
 * @param[in] railHandle A handle for RAIL instance.
 * @return true if BLE mode is enabled and false otherwise.
 *
 * This function returns the current status of RAIL's BLE mode. It is enabled by
 * a call to \ref RAIL_BLE_Init() and disabled by a call to \ref RAIL_BLE_Deinit().
 */
bool RAIL_BLE_IsEnabled(RAIL_Handle_t railHandle);

/**
 * Switch to the 1 Mbps Quuppa PHY.
 *
 * @param[in] railHandle A handle for RAIL instance.
 * @return Status code indicating success of the function call.
 *
 * You can use this function to switch to the Quuppa PHY.
 *
 * @note Not all chips support the 1 Mbps Quuppa PHY. This API should return \ref RAIL_STATUS_INVALID_CALL if
 *   unsupported by the hardware we're building for.
 */
RAIL_Status_t RAIL_BLE_ConfigPhyQuuppa(RAIL_Handle_t railHandle);

/**
 * Switch to the Viterbi 1 Mbps BLE PHY.
 *
 * @param[in] railHandle A handle for RAIL instance.
 * @return Status code indicating success of the function call.
 *
 * Use this function to switch back to the default BLE 1 Mbps PHY if you
 * have switched to the 2 Mbps or another configuration. You may only call this
 * function after initializing BLE and while the radio is idle.
 */
RAIL_Status_t RAIL_BLE_ConfigPhy1MbpsViterbi(RAIL_Handle_t railHandle);

#endif//SLI_LIBRAIL_ALIAS

/**
 * Switch to the legacy non-Viterbi 1 Mbps BLE PHY.
 *
 * @param[in] railHandle A handle for RAIL instance.
 * @return Status code indicating success of the function call.
 *
 * Use this function to switch back to the legacy BLE 1 Mbps PHY if you
 * have switched to the 2 Mbps or another configuration. You may only call this
 * function after initializing BLE and while the radio is idle.
 *
 * @deprecated BLE non-Viterbi PHYs are no longer supported.
 */
RAIL_Status_t RAIL_BLE_ConfigPhy1Mbps(RAIL_Handle_t railHandle);

#ifndef SLI_LIBRAIL_ALIAS

/**
 * Switch to the Viterbi 2 Mbps BLE PHY.
 *
 * @param[in] railHandle A handle for RAIL instance.
 * @return Status code indicating success of the function call.
 *
 * Use this function to switch back to the BLE 2 Mbps PHY from the
 * default 1 Mbps option. You may only call this function after initializing BLE
 * and while the radio is idle.
 */
RAIL_Status_t RAIL_BLE_ConfigPhy2MbpsViterbi(RAIL_Handle_t railHandle);

#endif//SLI_LIBRAIL_ALIAS

/**
 * Switch to the legacy non-Viterbi 2 Mbps BLE PHY.
 *
 * @param[in] railHandle A handle for RAIL instance.
 * @return Status code indicating success of the function call.
 *
 * Use this function to switch back to legacy BLE 2 Mbps PHY from the
 * default 1 Mbps option. You may only call this function after initializing BLE
 * and while the radio is idle.
 *
 * @deprecated BLE non-Viterbi PHYs are no longer supported.
 */
RAIL_Status_t RAIL_BLE_ConfigPhy2Mbps(RAIL_Handle_t railHandle);

#ifndef SLI_LIBRAIL_ALIAS

/**
 * Switch to the BLE Coded PHY.
 *
 * @param[in] railHandle A handle for RAIL instance.
 * @param[in] bleCoding The \ref RAIL_BLE_Coding_t to use
 * @return Status code indicating success of the function call.
 *
 * Use this function to switch back to BLE Coded PHY from the default
 * 1 Mbps option. You may only call this function after initializing BLE and
 * while the radio is idle. When using a BLE Coded PHY, the \ref
 * RAIL_RxPacketDetails_t::subPhyId marks the coding of the received packet.
 * A subPhyId of 0 marks a 500 kbps packet, and a subPhyId of 1 marks a 125
 * kbps packet.
 */
RAIL_Status_t RAIL_BLE_ConfigPhyCoded(RAIL_Handle_t railHandle,
                                      RAIL_BLE_Coding_t bleCoding);

/**
 * Switch to the Simulscan PHY.
 *
 * @param[in] railHandle A handle for RAIL instance.
 * @return Status code indicating success of the function call.
 *
 * Use this function to switch to the BLE Simulscan PHY. You may only
 * call this function after initializing BLE and while the radio is idle.
 * When using Simulscan PHY, the \ref RAIL_RxPacketDetails_t::subPhyId
 * marks the coding of the received packet. A subPhyId of 0 marks a
 * 500 kbps packet, a subPhyId of 1 marks a 125 kbps packet, and a
 * subPhyId of 2 marks a 1 Mbps packet.
 *
 * @note: The Simulscan PHY is supported only on some parts.
 *   The preprocessor symbol \ref RAIL_BLE_SUPPORTS_SIMULSCAN_PHY and the
 *   runtime function \ref RAIL_BLE_SupportsSimulscanPhy() may be used to
 *   test for support of the Simulscan PHY.
 */
RAIL_Status_t RAIL_BLE_ConfigPhySimulscan(RAIL_Handle_t railHandle);

#ifndef DOXYGEN_UNDOCUMENTED
/**
 * Switch to the 1 Mbps BLE PHY for CS.
 *
 * @param[in] railHandle A handle for RAIL instance.
 * @return Status code indicating success of the function call.
 *
 * Use this function to switch back to the BLE 1 Mbps CS PHY from
 * another configuration. You may only call this
 * function after initializing BLE and while the radio is idle.
 *
 * @note This PHY is only supported when \ref RAIL_BLE_SUPPORTS_CS is not 0.
 */
RAIL_Status_t RAIL_BLE_ConfigPhy1MbpsCs(RAIL_Handle_t railHandle);

/**
 * Switch to the 2 Mbps BLE PHY for CS.
 *
 * @param[in] railHandle A handle for RAIL instance.
 * @return Status code indicating success of the function call.
 *
 * Use this function to switch back to the BLE 2 Mbps CS PHY from
 * another configuration. You may only call this
 * function after initializing BLE and while the radio is idle.
 *
 * @note This PHY is only supported when \ref RAIL_BLE_SUPPORTS_CS is not 0.
 */
RAIL_Status_t RAIL_BLE_ConfigPhy2MbpsCs(RAIL_Handle_t railHandle);
#endif //DOXYGEN_UNDOCUMENTED

#endif//SLI_LIBRAIL_ALIAS

/**
 * Change BLE radio parameters.
 *
 * @param[in] railHandle A handle for RAIL instance.
 * @param[in] crcInit The value to use for CRC initialization.
 * @param[in] accessAddress The access address to use for the connection. The
 *   bits of this parameter are transmitted or received LSB first.
 * @param[in] channel The logical channel that you're changing to, which
 *   initializes the whitener if used.
 * @param[in] disableWhitening This can turn off the whitening engine and is useful
 *   for sending BLE test mode packets that don't have this turned on.
 * @return Status code indicating success of the function call.
 *
 * This function can be used to switch radio parameters on every connection
 * and/or channel change. It is BLE-aware and will set the access address,
 * preamble, CRC initialization value, and whitening configuration without
 * requiring you to load a new radio configuration. This function should be
 * called after switching to a particular BLE phy (1 Mbps, 2 Mbps, etc.) and
 * not while the radio is active.
 */
RAIL_Status_t RAIL_BLE_ConfigChannelRadioParams(RAIL_Handle_t railHandle,
                                                uint32_t crcInit,
                                                uint32_t accessAddress,
                                                uint16_t channel,
                                                bool disableWhitening);

/**
 * Change the current BLE PHY and go into receive.
 *
 * @param[in] railHandle A handle for RAIL instance.
 * @param[in] phy Indicates which PHY to receive on.
 * @param[in] railChannel Which channel of the given PHY to receive on.
 * @param[in] startRxTime Absolute near-future RAIL time to enter RX.
 * @param[in] crcInit The value to use for CRC initialization.
 * @param[in] accessAddress The access address to use for the connection. The
 *   bits of this parameter are transmitted or received LSB first.
 * @param[in] logicalChannel The logical channel that you're changing to, which
 *   initializes the whitener if used.
 * @param[in] disableWhitening This can turn off the whitening engine and is useful
 *   for sending BLE test mode packets that don't have this turned on.
 * @return Status code indicating success of the function call.
 *
 * This function is used to implement auxiliary packet reception, as defined in
 * the BLE specification. The radio will be put into IDLE, the PHY and channel
 * will be changed, and then receive will be entered at the start time given.
 * The new receive will have a timeout of 30 us, which means that this function
 * should only be called if the offset unit is 30 us.
 *
 * This function is extremely time-sensitive, and may only be called within the
 * interrupt context of a \ref RAIL_EVENT_RX_PACKET_RECEIVED event.
 */
RAIL_Status_t RAIL_BLE_PhySwitchToRx(RAIL_Handle_t railHandle,
                                     RAIL_BLE_Phy_t phy,
                                     uint16_t railChannel,
                                     RAIL_Time_t startRxTime,
                                     uint32_t crcInit,
                                     uint32_t accessAddress,
                                     uint16_t logicalChannel,
                                     bool disableWhitening);

#ifndef SLI_LIBRAIL_ALIAS

/**
 * Configure and enable signal identifier for BLE signal detection.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] signalIdentifierMode Mode of signal identifier operation.
 * @return Status code indicating success of the function call.
 *
 * This features allows detection of BLE signal on air based on the mode.
 * This function must be called once before \ref RAIL_BLE_EnableSignalDetection
 * to configure and enable signal identifier.
 *
 * To enable event for signal detection \ref RAIL_ConfigEvents() must be called
 * for enabling \ref RAIL_EVENT_SIGNAL_DETECTED.
 *
 * This function is only supported by chips where
 * \ref RAIL_BLE_SUPPORTS_SIGNAL_IDENTIFIER and
 * \ref RAIL_BLE_SupportsSignalIdentifier() are true.
 */
RAIL_Status_t RAIL_BLE_ConfigSignalIdentifier(RAIL_Handle_t railHandle,
                                              RAIL_BLE_SignalIdentifierMode_t signalIdentifierMode);

/**
 * Enable or disable signal identifier interrupt for BLE signal detection.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] enable Signal detection is enabled if true, disabled if false.
 * @return Status code indicating success of the function call.
 *
 * \ref RAIL_BLE_ConfigSignalIdentifier() must be called once before calling this
 * function to configure and enable signal identifier.
 * Once a signal is detected signal detection will be turned off and this
 * function should be called to re-enable the signal detection without needing
 * to call \ref RAIL_BLE_ConfigSignalIdentifier() if the signal identifier
 * is already configured and enabled.
 *
 * This function is only supported by chips where
 * \ref RAIL_BLE_SUPPORTS_SIGNAL_IDENTIFIER and
 * \ref RAIL_BLE_SupportsSignalIdentifier() are true.
 */
RAIL_Status_t RAIL_BLE_EnableSignalDetection(RAIL_Handle_t railHandle,
                                             bool enable);

/**
 * @brief Backward compatible name for the \ref
 *   RAIL_BLE_EnableSignalDetection API.
 */
#define RAIL_BLE_EnableSignalIdentifier RAIL_BLE_EnableSignalDetection

#endif//SLI_LIBRAIL_ALIAS

/******************************************************************************
 * Angle of Arrival/Departure (AoX)
 *****************************************************************************/
/**
 * @addtogroup AoX Angle of Arrival/Departure
 * @{
 * @brief These APIs are to a stack implementing BLE's angle of arrival and
 *   angle of departure functionality.
 *
 * They are designed for use by the Silicon Labs BLE stack only at this time and
 * may cause problems if accessed directly.
 */

/**
 * The maximum number of GPIO pins used for AoX Antenna switching.
 *
 * If the user configures more pins using
 * \ref RAIL_BLE_ConfigAoxAntenna() than allowed
 * \ref RAIL_BLE_AOX_ANTENNA_PIN_COUNT, then
 * \ref RAIL_STATUS_INVALID_PARAMETER status will be returned.
 *
 * \ref RAIL_STATUS_INVALID_CALL is returned if :
 * \ref RAIL_BLE_AOX_ANTENNA_PIN_COUNT is set to 0 or
 * the user configures no pins.
 *
 * The maximum value \ref RAIL_BLE_AOX_ANTENNA_PIN_COUNT can take depends on
 * number of Antenna route pins , a chip provides.
 * For EFR32xG22, the maximum value of \ref RAIL_BLE_AOX_ANTENNA_PIN_COUNT is 6.
 * If the user configures fewer pins than \ref RAIL_BLE_AOX_ANTENNA_PIN_COUNT,
 * then only number of pins asked by user will be configured with
 * \ref RAIL_STATUS_NO_ERROR.
 */
#define RAIL_BLE_AOX_ANTENNA_PIN_COUNT (6U)

/**
 * @enum RAIL_BLE_AoxOptions_t
 * @brief Angle of Arrival/Departure options bit fields
 */
RAIL_ENUM_GENERIC(RAIL_BLE_AoxOptions_t, uint16_t) {
  /** Shift position of \ref RAIL_BLE_AOX_OPTIONS_SAMPLE_MODE bit. */
  RAIL_BLE_AOX_OPTIONS_SAMPLE_MODE_SHIFT = 0,
  /** Shift position of \ref RAIL_BLE_AOX_OPTIONS_CONNLESS bit. */
  RAIL_BLE_AOX_OPTIONS_CONNLESS_SHIFT = 1,
  /** Shift position of \ref RAIL_BLE_AOX_OPTIONS_CONN bit. */
  RAIL_BLE_AOX_OPTIONS_CONN_SHIFT = 2,
  /** Shift position of \ref RAIL_BLE_AOX_OPTIONS_DISABLE_BUFFER_LOCK bit. */
  RAIL_BLE_AOX_OPTIONS_DISABLE_BUFFER_LOCK_SHIFT = 3,
};

/**
 * @deprecated Obsolete AOX option
 */
#define RAIL_BLE_AOX_OPTIONS_DO_SWITCH           (0U)
/**
 * @deprecated Obsolete AOX option
 */
#define RAIL_BLE_AOX_OPTIONS_TX_ENABLED          (0U)
/**
 * @deprecated Obsolete AOX option
 */
#define RAIL_BLE_AOX_OPTIONS_RX_ENABLED          (0U)
/**
 * @deprecated Please use \ref RAIL_BLE_AOX_OPTIONS_DISABLE_BUFFER_LOCK_SHIFT instead.
 */
#define RAIL_BLE_AOX_OPTIONS_LOCK_CTE_BUFFER_SHIFT RAIL_BLE_AOX_OPTIONS_DISABLE_BUFFER_LOCK_SHIFT

/**
 * Disable the AoX feature.
 */
#define RAIL_BLE_AOX_OPTIONS_DISABLED            (0U)
/**
 * Sets one of the two AoX sampling/switching modes: 1 us or 2 us window.
 */
#define RAIL_BLE_AOX_OPTIONS_SAMPLE_MODE         (1U << RAIL_BLE_AOX_OPTIONS_SAMPLE_MODE_SHIFT)
/**
 * Enables connectionless AoX Rx packets.
 */
#define RAIL_BLE_AOX_OPTIONS_CONNLESS            (1U << RAIL_BLE_AOX_OPTIONS_CONNLESS_SHIFT)
/**
 * Enables connection based AoX Rx packets.
 */
#define RAIL_BLE_AOX_OPTIONS_CONN                (1U << RAIL_BLE_AOX_OPTIONS_CONN_SHIFT)
/**
 * Disables CTE buffer lock.
 */
#define RAIL_BLE_AOX_OPTIONS_DISABLE_BUFFER_LOCK (1U << RAIL_BLE_AOX_OPTIONS_DISABLE_BUFFER_LOCK_SHIFT)
/**
 * Enables connection based or connectionless AoX Rx packets.
 */
#define RAIL_BLE_AOX_OPTIONS_ENABLED             (RAIL_BLE_AOX_OPTIONS_CONN | RAIL_BLE_AOX_OPTIONS_CONNLESS)

/**
 * @struct RAIL_BLE_AoxConfig_t
 * @brief Contains arguments for \ref RAIL_BLE_ConfigAox() function.
 */
typedef struct RAIL_BLE_AoxConfig {
  /** AoX options. */
  RAIL_BLE_AoxOptions_t aoxOptions;
  /**
   * Size of the raw AoX CTE (continuous tone extension) data capture buffer in
   * bytes. Note this value should be a multiple of 4 as each IQ sample
   * requires 4 bytes.
   */
  uint16_t cteBuffSize;
  /**
   * Address to where the received CTE is written. Buffer must be 32-bit
   * aligned.
   */
  uint32_t * cteBuffAddr;
  /**
   * Address to first element of antenna pattern array. Array must be in RAM.
   * Each element of the array contains an antenna number. The switching pattern
   * is defined by the order of antennas in this array.
   */
  uint8_t * antArrayAddr;
  /**
   * Number of entries in the antenna pattern array.
   */
  uint8_t antArraySize;
} RAIL_BLE_AoxConfig_t;

/**
 * @struct RAIL_BLE_AoxAntennaPortPins_t
 * @brief Contains elements of \ref RAIL_BLE_AoxAntennaConfig_t struct.
 */
typedef struct RAIL_BLE_AoxAntennaPortPins {
  /** The port which is used for AoX antenna switching. */
  uint8_t antPort;
  /** The pin which is used for AoX antenna switching. */
  uint8_t antPin;
} RAIL_BLE_AoxAntennaPortPins_t;

/**
 * @struct RAIL_BLE_AoxAntennaConfig_t
 * @brief Contains arguments for \ref RAIL_BLE_ConfigAoxAntenna() function.
 */
typedef struct RAIL_BLE_AoxAntennaConfig {
  /**
   * A pointer to an array containing struct of port and pin used for
   * AoX antenna switching.
   */
  RAIL_BLE_AoxAntennaPortPins_t *antPortPin;
  /**
   * Number of antenna pins to be configured.
   */
  uint8_t antCount;
} RAIL_BLE_AoxAntennaConfig_t;

#ifndef SLI_LIBRAIL_ALIAS

/**
 * Lock/unlock the CTE buffer from the application's perspective. The radio
 * will write to the buffer only if the bit is NOT set at the beginning of the
 * sampling period. The radio will set the bit once the sampling period starts
 * to indicate that some CTE data has been collected, which will not be
 * overwritten during the next sampling period, unless the buffer is unlocked by
 * the application.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] lock Lock the CTE buffer if true and unlock it if false.
 * @return true if the CTE buffer is locked after the call, otherwise false.
 */
bool RAIL_BLE_LockCteBuffer(RAIL_Handle_t railHandle, bool lock);

/**
 * Determine whether the CTE buffer is currently locked or not.
 *
 * @param[in] railHandle A handle for RAIL instance.
 * @return true if CTE buffer is locked and false otherwise.
 */
bool RAIL_BLE_CteBufferIsLocked(RAIL_Handle_t railHandle);

/**
 * Get the offset into CTE sample of CTE data.
 *
 * @param[in] railHandle A handle for RAIL instance.
 * @return The offset of CTE data in a CTE sample in bytes.
 *   On unsupported platforms this returns 0.
 */
uint8_t RAIL_BLE_GetCteSampleOffset(RAIL_Handle_t railHandle);

/**
 * Get the effective sample rate used by the ADC to capture the CTE samples.
 *
 * @param[in] railHandle A handle for RAIL instance.
 * @return The actual sample rate used to capture the CTE in samples per second.
 *   On unsupported platforms this returns 0.
 */
uint32_t RAIL_BLE_GetCteSampleRate(RAIL_Handle_t railHandle);

/**
 * Configure Angle of Arrival/Departure (AoX) functionality.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] aoxConfig Configuration options for AoX
 * @return Status code indicating success of the function call.
 *
 * AoX is a method
 * of radio localization which infers angle of arrival/departure of the signal
 * based on different phases of the raw I/Q signal from different antennas by
 * controlling external RF switch during the continuous tone extension (CTE).
 * Connection based AoX packets are different than normal BLE packets in that
 * they have 3 header bytes instead of 2 and they have CTE appended after the
 * payload's CRC. 3rd byte or CTE info contains CTE length. Connectionless AoX
 * packets have 2 header bytes and CTE info is part of the payload.
 *
 * @note Calling \ref RAIL_GetRadioEntropy() during AoX reception may break
 *   packet reception.
 */
RAIL_Status_t RAIL_BLE_ConfigAox(RAIL_Handle_t railHandle,
                                 const RAIL_BLE_AoxConfig_t *aoxConfig);

/**
 * Perform one time initialization of AoX registers.
 * This function must be called before \ref RAIL_BLE_ConfigAox()
 * and before configuring the BLE PHY.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return Status code indicating success of the function call.
 */
RAIL_Status_t RAIL_BLE_InitCte(RAIL_Handle_t railHandle);

/**
 * Perform initialization of AoX antenna GPIO pins.
 * This function must be called before calls to \ref RAIL_BLE_InitCte()
 * and \ref RAIL_BLE_ConfigAox(), and before configuring the BLE PHY,
 * else a \ref RAIL_STATUS_INVALID_CALL is returned.
 *
 * If user configures more pins in \ref RAIL_BLE_AoxAntennaConfig_t::antCount
 * than allowed by \ref RAIL_BLE_AOX_ANTENNA_PIN_COUNT, then the API returns
 * \ref RAIL_STATUS_INVALID_PARAMETER.
 *
 * If user configures lesser than or equal to number of pins allowed by
 * \ref RAIL_BLE_AOX_ANTENNA_PIN_COUNT, then the requested number of pins
 * are configured and \ref RAIL_STATUS_NO_ERROR is returned.
 *
 * If AoX antenna switching is inactive, non-AoX transmits and receives
 * will occur on the first antenna specified by the antenna pattern or
 * on the default antenna if no antenna pattern is provided.
 *
 * @note This function must only be called while channel sounding is disabled.
 *   If channel sounding has been enabled \ref RAIL_STATUS_INVALID_STATE will
 *   be returned.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] antennaConfig A pointer to the antenna configuration
 *   structure to hold the set of GPIO ports and pins for AoX antenna
 *   switching.
 * @return Status code indicating success of the function call.
 *
 * @note Use the compile time symbol \ref RAIL_BLE_SUPPORTS_ANTENNA_SWITCHING
 *   or the runtime call \ref RAIL_BLE_SupportsAntennaSwitching() to check
 *   whether the platform supports this feature.
 *
 * @warning As this function relies on GPIO access and RAIL is meant to run in
 *   TrustZone non-secure world, it is not supported if GPIO is configured as
 *   secure peripheral and it will return \ref RAIL_STATUS_INVALID_CALL.
 */
RAIL_Status_t RAIL_BLE_ConfigAoxAntenna(RAIL_Handle_t railHandle,
                                        RAIL_BLE_AoxAntennaConfig_t *antennaConfig);

/** @} */  // end of group AoX

#endif//SLI_LIBRAIL_ALIAS

#ifndef DOXYGEN_UNDOCUMENTED
/******************************************************************************
 * Channel Sounding (CS)
 *****************************************************************************/
/**
 * @addtogroup CS Channel Sounding
 * @{
 * @brief These APIs are to a stack implementing BLE's channel sounding
 *   functionality.
 *
 * They are designed for use by the Silicon Labs BLE stack only at this time and
 * may cause problems if accessed directly.
 */

/** Total number of CS Channels. */
#define RAIL_BLE_CS_NUM_CHANNELS 79

/** Total number of allowed CS Channels. */
#define RAIL_BLE_CS_NUM_ALLOWED_CHANNELS 72

/**
 * @enum RAIL_BLE_CsRole_t
 * @brief The device role during CS events.
 */
RAIL_ENUM(RAIL_BLE_CsRole_t) {
  /** Device cannot perform CS events. */
  RAIL_BLE_CS_ROLE_UNASSIGNED = 0,
  /** Device is an initiator during CS events. */
  RAIL_BLE_CS_ROLE_INITIATOR = 1,
  /** Device is a reflector during CS events. */
  RAIL_BLE_CS_ROLE_REFLECTOR = 2,
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// Self-referencing defines minimize compiler complaints when using RAIL_ENUM
#define RAIL_BLE_CS_ROLE_UNASSIGNED ((RAIL_BLE_CsRole_t) RAIL_BLE_CS_ROLE_UNASSIGNED)
#define RAIL_BLE_CS_ROLE_INITIATOR  ((RAIL_BLE_CsRole_t) RAIL_BLE_CS_ROLE_INITIATOR)
#define RAIL_BLE_CS_ROLE_REFLECTOR  ((RAIL_BLE_CsRole_t) RAIL_BLE_CS_ROLE_REFLECTOR)
#endif//DOXYGEN_SHOULD_SKIP_THIS

/**
 * @struct RAIL_BLE_CsResults_t
 * @brief Contains measurement results from CS step.
 */
typedef struct {
  /** CS measurement data for a particular step. */
  uint32_t result[7];
} RAIL_BLE_CsResults_t;

/**
 * @enum RAIL_BLE_CsRttType_t
 * @brief CS RTT Types.
 */
RAIL_ENUM(RAIL_BLE_CsRttType_t) {
  /** Coarse cost function engine method RTT. */
  RAIL_BLE_CS_RTT_AA_ONLY = 0U,
  /** 32 bit sounding sequence method RTT. */
  RAIL_BLE_CS_RTT_32B_SS = 1U,
  /** 96 bit sounding sequence method RTT. */
  RAIL_BLE_CS_RTT_96B_SS = 2U,
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// Self-referencing defines minimize compiler complaints when using RAIL_ENUM
#define RAIL_BLE_CS_RTT_AA_ONLY ((RAIL_BLE_CsRttType_t) RAIL_BLE_CS_RTT_AA_ONLY)
#define RAIL_BLE_CS_RTT_32B_SS  ((RAIL_BLE_CsRttType_t) RAIL_BLE_CS_RTT_32B_SS)
#define RAIL_BLE_CS_RTT_96B_SS  ((RAIL_BLE_CsRttType_t) RAIL_BLE_CS_RTT_96B_SS)
#endif//DOXYGEN_SHOULD_SKIP_THIS

/**
 *  The minimum size in 32 bit words for the IQ buffer. This value guarantees
 *  all IQ samples for a single 1 Mbps CS step can be stored.
 */
#define RAIL_BLE_CS_1MBPS_MINIMUM_IQ_BUFFER_SIZE  1500U

/**
 * @struct RAIL_BLE_CsConfig_t
 * @brief Contains arguments for \ref RAIL_BLE_ConfigCs function.
 */
typedef struct RAIL_BLE_CsConfig {
  /** The device role during CS event. */
  RAIL_BLE_CsRole_t role;
  /**
   * Number of mode 2 phase measurement slots, including the
   * tone extension slot. This value should be between
   * \ref RAIL_BLE_CS_MIN_ANTENNA_SLOTS and
   * \ref RAIL_BLE_CS_MAX_ANTENNA_SLOTS, inclusive.
   * A provided value below or above this range will be pegged
   * to the appropriate minimum or maximum value.
   */
  uint8_t slotCount;
  /** Number of steps in CS event. */
  uint16_t csSqteSteps;
  /** Pointer to CS measurements. Set to NULL if unused. */
  RAIL_BLE_CsResults_t *pCsDataOutput;
  /** Frequency change spacing (in us). */
  uint16_t t_fcs;
  /** Interlude period for mode 0 & 1 steps (in us). */
  uint16_t t_ip1;
  /** Interlude period for mode 2 steps (in us). */
  uint16_t t_ip2;
  /** Phase measurement time (in us). */
  uint16_t t_pm;
  /** Antenna switching time (in us). */
  uint16_t t_sw;
  /**
   * Pointer to buffer where IQ data will be written. Buffer must be 32-bit
   * aligned.
   */
  uint32_t *pIqBuffer;
  /**
   * Size of IQ buffer in 32 bit words. Must be at least \ref
   * RAIL_BLE_CS_1MBPS_MINIMUM_IQ_BUFFER_SIZE or else an error will be
   * returned by \ref RAIL_BLE_ConfigCs().
   */
  uint16_t iqBufferSize;
  /**
   * Step Index to perform the event calibration. This index must correspond
   * to a mode 0 step or else the event calibration won't occur.
   */
  uint8_t eventCalStepIndex;
  /** RTT type returned during mode 1 step. */
  RAIL_BLE_CsRttType_t rttType;
  /**
   * A pointer to the selected CS event gain index. This field will be
   * populated after \ref eventCalStepIndex has been reached.
   */
  uint8_t *pEventGainIndex;
  /**
   * A pointer to the selected CS event Fractional Frequency Offset
   * (FFO) * pp100m (parts-per-100-million). This field will be populated
   * after \ref eventCalStepIndex has been reached.
   */
  int16_t *pEventFfoPp100m;
  /** Debug flag to disable RTT GD compensation. */
  bool disableRttGdComp;
  /** Debug flag to disable PBR DC compensation. */
  bool disablePbrDcComp;
  /** Debug flag to disable PBR GD compensation. */
  bool disablePbrGdComp;
  /** Debug flag to force event gain for calibration. */
  bool forceAgcGain;
  /**
   * Pointer to an FAE table of size \ref RAIL_BLE_CS_NUM_ALLOWED_CHANNELS
   * that holds the FAE value for each allowed CS channel in units of
   * parts-per-32-million (pp32m). In units of parts-per-million (ppm),
   * the FAE ranges from [-4, +3.96875] ppm with a resolution of 0.03125 ppm.
   * Set to NULL if unused.
   */
  int8_t(*pFaeTable)[RAIL_BLE_CS_NUM_ALLOWED_CHANNELS];
  /** Equivalent AGC STATUS0 register to force. */
  uint32_t forcedAgcStatus0;
} RAIL_BLE_CsConfig_t;

/** The maximum number of CS steps allowed during a CS event. */
#define RAIL_BLE_CS_MAX_SQTE_STEPS 512U

/**
 * @enum RAIL_BLE_CsStepState_t
 * @brief The current CS step state.
 */
RAIL_ENUM(RAIL_BLE_CsStepState_t) {
  /** CS step state idle. */
  RAIL_BLE_CS_STATE_IDLE = 0,
  /** CS step state initiator transmit mode 0. */
  RAIL_BLE_CS_STATE_I_TX_MODE0 = 1,
  /** CS step state reflector transmit mode 0. */
  RAIL_BLE_CS_STATE_R_TX_MODE0 = 2,
  /** CS step state initiator transmit mode 1. */
  RAIL_BLE_CS_STATE_I_TX_MODE1 = 3,
  /** CS step state reflector transmit mode 1. */
  RAIL_BLE_CS_STATE_R_TX_MODE1 = 4,
  /** CS step state reflector transmit mode 2. */
  RAIL_BLE_CS_STATE_R_TX_MODE2 = 6,
  /** CS step state initiator transmit mode 2. */
  RAIL_BLE_CS_STATE_I_TX_MODE2 = 7,
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// Self-referencing defines minimize compiler complaints when using RAIL_ENUM
#define RAIL_BLE_CS_STATE_IDLE       ((RAIL_BLE_CsStepState_t) RAIL_BLE_CS_STATE_IDLE)
#define RAIL_BLE_CS_STATE_I_TX_MODE0 ((RAIL_BLE_CsStepState_t) RAIL_BLE_CS_STATE_I_TX_MODE0)
#define RAIL_BLE_CS_STATE_R_TX_MODE0 ((RAIL_BLE_CsStepState_t) RAIL_BLE_CS_STATE_R_TX_MODE0)
#define RAIL_BLE_CS_STATE_I_TX_MODE1 ((RAIL_BLE_CsStepState_t) RAIL_BLE_CS_STATE_I_TX_MODE1)
#define RAIL_BLE_CS_STATE_R_TX_MODE1 ((RAIL_BLE_CsStepState_t) RAIL_BLE_CS_STATE_R_TX_MODE1)
#define RAIL_BLE_CS_STATE_R_TX_MODE2 ((RAIL_BLE_CsStepState_t) RAIL_BLE_CS_STATE_R_TX_MODE2)
#define RAIL_BLE_CS_STATE_I_TX_MODE2 ((RAIL_BLE_CsStepState_t) RAIL_BLE_CS_STATE_I_TX_MODE2)
#endif//DOXYGEN_SHOULD_SKIP_THIS

/**
 * First step state for CS mode 0.
 */
#define RAIL_BLE_CS_STEP_MODE0             RAIL_BLE_CS_STATE_I_TX_MODE0

/**
 * First step state for CS mode 1.
 */
#define RAIL_BLE_CS_STEP_MODE1             RAIL_BLE_CS_STATE_I_TX_MODE1

/**
 * First step state for CS mode 2.
 */
#define RAIL_BLE_CS_STEP_MODE2             RAIL_BLE_CS_STATE_I_TX_MODE2

/**
 * @enum RAIL_BLE_CsStepMode_t
 * @brief The CS step mode.
 */
RAIL_ENUM(RAIL_BLE_CsStepMode_t) {
  /** CS step mode 0. */
  RAIL_BLE_CS_MODE_0 = 0,
  /** CS step mode 1. */
  RAIL_BLE_CS_MODE_1 = 1,
  /** CS step mode 2. */
  RAIL_BLE_CS_MODE_2 = 2,
  /** CS step mode 3. */
  RAIL_BLE_CS_MODE_3 = 3,
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// Self-referencing defines minimize compiler complaints when using RAIL_ENUM
#define RAIL_BLE_CS_MODE_0 ((RAIL_BLE_CsStepMode_t) RAIL_BLE_CS_MODE_0)
#define RAIL_BLE_CS_MODE_1 ((RAIL_BLE_CsStepMode_t) RAIL_BLE_CS_MODE_1)
#define RAIL_BLE_CS_MODE_2 ((RAIL_BLE_CsStepMode_t) RAIL_BLE_CS_MODE_2)
#define RAIL_BLE_CS_MODE_3 ((RAIL_BLE_CsStepMode_t) RAIL_BLE_CS_MODE_3)
#endif//DOXYGEN_SHOULD_SKIP_THIS

/** The maximum number of antennas supported. */
#define RAIL_BLE_CS_MAX_ANTENNAS 4U

/**
 * @enum RAIL_BLE_CsAntennaId_t
 * @brief The CS antenna ID. Valid values according to the CS spec are within
 *   the range 1 and 4 inclusive.
 */
RAIL_ENUM(RAIL_BLE_CsAntennaId_t) {
  /** Antenna ID of the first supported antenna. */
  RAIL_BLE_CS_ANTENNA_ID_1 = 1U,
  /** Antenna ID of the second supported antenna. */
  RAIL_BLE_CS_ANTENNA_ID_2 = 2U,
  /** Antenna ID of the third supported antenna. */
  RAIL_BLE_CS_ANTENNA_ID_3 = 3U,
  /** Antenna ID of the fourth supported antenna. */
  RAIL_BLE_CS_ANTENNA_ID_4 = 4U,
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// Self-referencing defines minimize compiler complaints when using RAIL_ENUM
#define RAIL_BLE_CS_ANTENNA_ID_1 ((RAIL_BLE_CsAntennaId_t) RAIL_BLE_CS_ANTENNA_ID_1)
#define RAIL_BLE_CS_ANTENNA_ID_2 ((RAIL_BLE_CsAntennaId_t) RAIL_BLE_CS_ANTENNA_ID_2)
#define RAIL_BLE_CS_ANTENNA_ID_3 ((RAIL_BLE_CsAntennaId_t) RAIL_BLE_CS_ANTENNA_ID_3)
#define RAIL_BLE_CS_ANTENNA_ID_4 ((RAIL_BLE_CsAntennaId_t) RAIL_BLE_CS_ANTENNA_ID_4)
#endif//DOXYGEN_SHOULD_SKIP_THIS

/** The value returned by RAIL for an invalid CS antenna count. */
#define RAIL_BLE_CS_INVALID_ANTENNA_COUNT 0U

/**
 * @enum RAIL_BLE_CsRttPacketQuality_t
 * @brief CS RTT packet quality.
 */
RAIL_ENUM(RAIL_BLE_CsRttPacketQuality_t) {
  /** Access address check succeeded. */
  RAIL_BLE_CS_RTT_AA_SUCCESS = 0U,
  /** Access address had one or more bit errors. */
  RAIL_BLE_CS_RTT_AA_BIT_ERRORS = 1U,
  /** Access address not found. */
  RAIL_BLE_CS_RTT_AA_NOT_FOUND = 2U,
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// Self-referencing defines minimize compiler complaints when using RAIL_ENUM
#define RAIL_BLE_CS_RTT_AA_SUCCESS    ((RAIL_BLE_CsRttPacketQuality_t) RAIL_BLE_CS_RTT_AA_SUCCESS)
#define RAIL_BLE_CS_RTT_AA_BIT_ERRORS ((RAIL_BLE_CsRttPacketQuality_t) RAIL_BLE_CS_RTT_AA_BIT_ERRORS)
#define RAIL_BLE_CS_RTT_AA_NOT_FOUND  ((RAIL_BLE_CsRttPacketQuality_t) RAIL_BLE_CS_RTT_AA_NOT_FOUND)
#endif//DOXYGEN_SHOULD_SKIP_THIS

/**
 * @struct RAIL_BLE_CsMode0Results_t
 * @brief Contains CS mode 0 step measurement results.
 */
typedef struct RAIL_BLE_CsMode0Results {
  /** Mode of CS step. */
  uint8_t mode;
  /** Antenna ID. */
  RAIL_BLE_CsAntennaId_t antenna;
  /** RSSI during step in integer dBm. */
  int8_t rssi;
  /** Packet quality. */
  uint8_t packetQuality;
  /** Reserved. */
  uint16_t reserved;
  /** Fractional Frequency Offset (FFO) in units of parts per 100 million. */
  int16_t csFfoPp100m;
  /** The gain setting. */
  uint32_t stepGainSetting;
  /** Reserved. */
  uint32_t reserved1[4];
} RAIL_BLE_CsMode0Results_t;

/**
 *  A sentinel value to indicate an invalid rtt time value in
 *  \ref RAIL_BLE_CsMode1Results_t::rttHalfNs
 */
#define RAIL_BLE_CS_INVALID_RTT_VALUE ((int16_t)0x8000)

/**
 * @struct RAIL_BLE_CsMode1Results_t
 * @brief Contains CS mode 1 step measurement results.
 */
typedef struct RAIL_BLE_CsMode1Results {
  /** Mode of CS step. */
  uint8_t mode;
  /** Antenna ID. */
  RAIL_BLE_CsAntennaId_t antenna;
  /** RSSI during step in integer dBm. */
  int8_t rssi;
  /** Packet quality. */
  uint8_t packetQuality;
  /**
   * For the initiator, this is the time (in 0.5 ns units) between time of
   * departure and time of arrival excluding known offsets such as interlude
   * period and packet length.
   * For the reflector, this is the time (in 0.5 ns units) between time of
   * arrival and time of departure excluding known offsets such as interlude
   * period and packet length.
   */
  int16_t rttHalfNs;
  /** Flag used to indicate whether we have missed frequency calibration. */
  uint8_t missedFcal;
  /** Reserved. */
  uint8_t reserved;
  /** Reserved. */
  uint32_t reserved1[5];
} RAIL_BLE_CsMode1Results_t;

/**
 * @enum RAIL_BLE_CsToneQuality_t
 * @brief CS tone quality.
 */
RAIL_ENUM(RAIL_BLE_CsToneQuality_t) {
  /** Good quality CS mode 2 tone. */
  RAIL_BLE_CS_TONE_QUALITY_GOOD = 0U,
  /** Medium quality CS mode 2 tone. */
  RAIL_BLE_CS_TONE_QUALITY_MEDIUM = 1U,
  /** Low quality CS mode 2 tone. */
  RAIL_BLE_CS_TONE_QUALITY_LOW = 2U,
  /** CS mode 2 tone quality indication unavailable. */
  RAIL_BLE_CS_TONE_QUALITY_UNAVAILABLE = 3U,
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// Self-referencing defines minimize compiler complaints when using RAIL_ENUM
#define RAIL_BLE_CS_TONE_QUALITY_GOOD        ((RAIL_BLE_CsToneQuality_t) RAIL_BLE_CS_TONE_QUALITY_GOOD)
#define RAIL_BLE_CS_TONE_QUALITY_MEDIUM      ((RAIL_BLE_CsToneQuality_t) RAIL_BLE_CS_TONE_QUALITY_MEDIUM)
#define RAIL_BLE_CS_TONE_QUALITY_LOW         ((RAIL_BLE_CsToneQuality_t) RAIL_BLE_CS_TONE_QUALITY_LOW)
#define RAIL_BLE_CS_TONE_QUALITY_UNAVAILABLE ((RAIL_BLE_CsToneQuality_t) RAIL_BLE_CS_TONE_QUALITY_UNAVAILABLE)
#endif//DOXYGEN_SHOULD_SKIP_THIS

/** The minimum number of antenna slots supported during a CS event. */
#define RAIL_BLE_CS_MIN_ANTENNA_SLOTS 2U

/** The maximum number of antenna slots supported during a CS event. */
#define RAIL_BLE_CS_MAX_ANTENNA_SLOTS 5U

/**
 * @struct RAIL_BLE_CsMode2Results_t
 * @brief Contains CS mode 2 step measurement results.
 */
typedef struct RAIL_BLE_CsMode2Results {
  /** Mode of CS step. */
  uint8_t mode;
  /** Flag used to indicate whether we have missed frequency calibration. */
  uint8_t missedFcal;
  /** PCT i value. */
  int16_t pctI[RAIL_BLE_CS_MAX_ANTENNA_SLOTS];
  /** PCT q value. */
  int16_t pctQ[RAIL_BLE_CS_MAX_ANTENNA_SLOTS];
  /** Tone quality indicator. */
  RAIL_BLE_CsToneQuality_t tqi[RAIL_BLE_CS_MAX_ANTENNA_SLOTS];
  /** Reserved. */
  uint8_t reserved[3];
} RAIL_BLE_CsMode2Results_t;

/**
 * @struct RAIL_BLE_CsStepResults_t
 * @brief Generic CS step mode result structure. Based on the value of the
 *   mode field, this structure can be type cast to the appropriate mode
 *   specific structure \ref RAIL_BLE_CsMode0Results_t,
 *   \ref RAIL_BLE_CsMode1Results_t, or \ref RAIL_BLE_CsMode2Results_t.
 */
typedef struct RAIL_BLE_CsStepResults {
  /** Mode of CS step. */
  uint8_t mode;
  /** Reserved. */
  uint8_t reserved;
  /** Reserved. */
  uint16_t reserved1;
  /** Reserved. */
  uint32_t reserved2[6];
} RAIL_BLE_CsStepResults_t;

/**
 * @struct RAIL_BLE_CsMode0DebugResults_t
 * @brief Contains CS mode 0 step measurement debug results.
 */
typedef struct RAIL_BLE_CsMode0DebugResults {
  /**
   * AGC gain value of the Mode 0 step with the highest recorded
   * RSSI up to and including the current Mode 0 step.
   */
  uint32_t agcStatus0;
  /**
   * For devices configured as an initiator, the measured frequency offset
   * in Hz between the two devices during a CS mode 0 step. For devices
   * configured as a reflector, this value will always be 0.
   */
  int32_t freqOffHz;
  /** Estimated coarse frequency offset in internal units. */
  int32_t hwFreqOffEst;
  /** Starting index IQ sample index of unmodulated carrier. */
  uint16_t ucStartIndex;
  /** End index IQ sample index of unmodulated carrier. */
  uint16_t ucEndIndex;
  /**
   * FFO of the Mode 0 step with the highest recorded RSSI
   * up to and including the current Mode 0 step.
   */
  int16_t csFfoPp100m;
  /** Highest recorded RSSI up to and including the current mode 0 step, in dBm. */
  int8_t highestRssiDbm;
  /** Tx timestamp. */
  uint8_t txTimeStampPre;
  /** Tx timestamp. */
  uint32_t txTimeStamp;
  /** Reserved. */
  uint32_t reserved[4];
} RAIL_BLE_CsMode0DebugResults_t;

/**
 * @struct RAIL_BLE_CsMode1DebugResults_t
 * @brief Contains CS mode 1 step measurement debug results.
 */
typedef struct RAIL_BLE_CsMode1DebugResults {
  /** Coarse time of flight in units of HFXO clock cycles. */
  uint16_t toxClks;
  /** Fractional component of time of flight in units of half nanoseconds. */
  int16_t fracRttHalfNs;
  /** Coarse component of time of flight in units of half nanoseconds. */
  uint32_t coarseRttHalfNs;
  /** Group delay compensation in units of half nanoseconds. */
  int32_t gdCompRttHalfNs;
  /** Time of flight without T_SY_CENTER_DELTA compensation in units of half nanoseconds. */
  int32_t toxWithOffsetsRttHalfNs;
  /** Internal CS status register. */
  uint32_t csstatus3;
  /** Internal CS status register. */
  uint32_t csstatus4;
  /** Internal CS status register. */
  uint32_t csstatus5;
  /** Tx timestamp. */
  uint32_t txTimeStamp;
  /** Tx timestamp. */
  uint8_t txTimeStampPre;
  /** Reserved. */
  uint8_t reserved;
  /** Reserved. */
  uint16_t reserved1;
  /** Reserved. */
  uint32_t reserved2;
} RAIL_BLE_CsMode1DebugResults_t;

/**
 * @struct RAIL_BLE_CsMode2DebugResults_t
 * @brief Contains CS mode 2 step measurement debug results.
 */
typedef struct RAIL_BLE_CsMode2DebugResults {
  /** Hardware PCT I value. */
  int16_t hardwarePctI;
  /** Hardware PCT Q value. */
  int16_t hardwarePctQ;
  /** DCCOMP i value. */
  int16_t dcCompI;
  /** DCCOMP q value. */
  int16_t dcCompQ;
  /** GDCOMP i value. */
  int16_t gdCompI[RAIL_BLE_CS_MAX_ANTENNAS];
  /** GDCOMP q value. */
  int16_t gdCompQ[RAIL_BLE_CS_MAX_ANTENNAS];
  /** Raw tone quality value. */
  uint16_t tqiRaw;
  /** Raw tone quality tone extension value. */
  uint16_t tqiToneExtRaw;
  /**
   * Pointer to the starting index of each antenna slot for
   * reading IQ samples.
   */
  uint16_t *ucStartIndex;
  /**
   * Pointer to the end index of each antenna slot for
   * reading IQ samples.
   */
  uint16_t *ucEndIndex;
  /** Frequency calibration value in internal units. */
  uint16_t fcal;
  /** Reserved. */
  uint16_t reserved;
} RAIL_BLE_CsMode2DebugResults_t;

/**
 * @struct RAIL_BLE_CsStepDebugResults_t
 * @brief Generic CS step mode debug result structure. Based on the value of
 *   the mode field, this structure can be type cast to the appropriate mode
 *   specific structure \ref RAIL_BLE_CsMode0DebugResults_t,
 *   \ref RAIL_BLE_CsMode1DebugResults_t, or \ref RAIL_BLE_CsMode2DebugResults_t.
 */
typedef struct RAIL_BLE_CsStepDebugResults {
  /** Reserved. */
  uint32_t reserved[10];
} RAIL_BLE_CsStepDebugResults_t;

/**
 * @struct RAIL_BLE_CsStepConfig_t
 * @brief Contains arguments for \ref RAIL_BLE_SetNextCsStep().
 */
typedef struct RAIL_BLE_CsStepConfig {
  /** Sets the CS step state. */
  RAIL_BLE_CsStepState_t stepState;
  /** Indicates whether this is final step in CS event. */
  bool lastStep;
  /**
   * Transmit tone during tone extension slot in mode 2 packet.
   * This field is ignored during RX and for all non mode 2 packets.
   */
  bool transmitToneExtension;
  /**
   * Length of packet payload in bytes. Should not include trailer, guard,
   * or UC bits. Only used for mode 1 steps, ignored otherwise.
   */
  uint8_t packetLength;
  /** Sets the CS step logical channel. */
  uint16_t channel;
  /** RTT marker bit positions. Ignored for mode 0 and 2 steps. */
  uint8_t rttMarkerBitPosition[2];
  /** The initiator (first) access address during step. */
  uint32_t initAccessAddress;
  /** The reflector (second) access address during step. */
  uint32_t reflAccessAddress;
  /** A pointer to TX data to be transmitted. Ignored for mode 0 and 2 steps. */
  uint8_t *pTxData;
  /**
   * A pointer to an array of CS step results. These results will be
   * populated after the completion of the CS step. This array can be cast to
   * \ref RAIL_BLE_CsMode0Results_t, \ref RAIL_BLE_CsMode1Results_t, or
   * \ref RAIL_BLE_CsMode2Results_t as appropriate to read mode specific
   * results.
   */
  RAIL_BLE_CsStepResults_t *pResults;
  /**
   * A pointer to an array of CS step debug results. These results will be
   * populated after the completion of the CS step. This array can be cast to
   * \ref RAIL_BLE_CsMode0DebugResults_t, \ref
   * RAIL_BLE_CsMode1DebugResults_t, or \ref RAIL_BLE_CsMode2DebugResults_t
   * as appropriate to read mode specific debug results.
   *
   * Setting this pointer to NULL means no debug data will be collected.
   */
  RAIL_BLE_CsStepDebugResults_t *pDebugResults;
  /**
   * A pointer to the start of captured IQ data for this step. This pointer
   * will be populated after the completion of the CS step.
   */
  uint32_t **pIqBuffer;
  /**
   * A pointer to captured IQ data size in 32 bit words. This pointer will be
   * populated after the completion of the CS step.
   */
  uint16_t *pIqBufferSize;
  /**
   * A pointer to a boolean to indicate whether to preserve IQ data for this
   * step. If this is the final step of the event, IQ data will automatically
   * be preserved regardless of how this boolean is set. For other steps, if
   * this boolean is set true, and there are at least \ref
   * RAIL_BLE_CS_1MBPS_MINIMUM_IQ_BUFFER_SIZE unused 32 bit words still
   * available in the event IQ buffer, this step's IQ data will be preserved
   * and not be overwritten by IQ data from a subsequent step. Otherwise, this
   * step's IQ data will not be preserved and may be overwritten. This boolean
   * will be updated after completion of the CS step to indicate whether the
   * IQ data from that step was actually preserved.
   */
  bool *pSaveIqData;
  /**
   * Array containing antenna settings for this step. This field has two uses
   * depending on the mode of the current step.
   *
   * On mode 0 and mode 1 steps, only the first element will be used to
   * indicate the antenna to be utilized during a mode 0 and
   * mode 1 step.
   *
   * On mode 2 steps, as many elements as
   * \ref RAIL_BLE_CS_MAX_ANTENNA_SLOTS - 1 that were configured for the
   * CS event will be applied.
   *
   * @note \ref RAIL_BLE_ConfigCsAntenna must be called prior to setting
   *   this field in order to set the antenna count as well as configure
   *   each antenna. Each element must be a valid antenna between 1 and
   *   the set antenna count.
   */
  RAIL_BLE_CsAntennaId_t antennaSelectBuffer[RAIL_BLE_CS_MAX_ANTENNAS];
} RAIL_BLE_CsStepConfig_t;

/**
 * @struct RAIL_BLE_CsAntennaConfig_t
 * @brief Contains arguments for \ref RAIL_BLE_ConfigCsAntenna() function.
 */
typedef struct RAIL_BLE_CsAntennaConfig {
  /** Total number of antenna elements. */
  uint8_t antennaCount;
  /** A pointer to antenna offsets in cm units. */
  const int16_t *pAntennaOffsetCm;
} RAIL_BLE_CsAntennaConfig_t;

/**
 * @struct RAIL_BLE_CsGdCompTables_t
 * @brief Contains pointers to CS group delay compensation tables.
 */
typedef struct RAIL_BLE_CsGdCompTables {
  /** A pointer to PBR phase LSB group delay compensation table. */
  const int16_t *pPbrPhaseLsb;
  /** A pointer to RTT slope group delay compensation table. */
  const int16_t *pRttSlope;
  /** A pointer to RTT offset group delay compensation table. */
  const int16_t *pRttOffset;
  /** Common length for each table in units of int16_t. */
  uint8_t length;
} RAIL_BLE_CsGdCompTables_t;

#ifndef SLI_LIBRAIL_ALIAS

/**
 * Configure Channel Sounding (CS) functionality.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] csConfig A non-NULL pointer to configuration options for CS.
 * @return Status code indicating success of the function call.
 *
 * @warning This API is not safe to use in a multiprotocol app.
 *
 * @warning As this function relies on GPIO access when more than one antenna
 *   is configured by \ref RAIL_BLE_ConfigCsAntenna and RAIL is meant to run in
 *   TrustZone non-secure world, it is not supported if GPIO is configured as
 *   secure peripheral and it will return \ref RAIL_STATUS_INVALID_CALL.
 */
RAIL_Status_t RAIL_BLE_ConfigCs(RAIL_Handle_t railHandle,
                                const RAIL_BLE_CsConfig_t *csConfig);

/**
 * Enable Channel Sounding (CS) functionality.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] enable Enable or disable CS functionality.
 * @return Status code indicating success of the function call.
 *
 * @warning This API is not safe to use in a multiprotocol app.
 *
 * @note While CS is enabled, the PA ramp time is cached and overridden to 5us.
 *   Upon disabling CS, the original PA ramp time is restored. While CS is
 *   enabled, calls to \ref RAIL_ConfigTxPower() should be avoided as they may
 *   corrupt the ramp time and result in incorrect CS event timing.
 */
RAIL_Status_t RAIL_BLE_EnableCs(RAIL_Handle_t railHandle,
                                bool enable);

/**
 * Set up the next CS step.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in,out] csStepConfig A pointer to configuration options for next CS step.
 * @param[in] pend If true, apply configuration at next appropriate radio
 *   transition (i.e., at Rx-to-Tx for an initiator, or Tx-to-Rx for a reflector).
 *   Otherwise, apply configuration immediately.
 * @return Status code indicating success of the function call.
 *
 * @note When the next CS step is to be pended, the specified step in
 *   csStepConfig must be the initial step state for a particular mode (e.g.
 *   \ref RAIL_BLE_CS_STEP_MODE0, \ref RAIL_BLE_CS_STEP_MODE1, or \ref
 *   RAIL_BLE_CS_STEP_MODE2). Otherwise this API will return \ref
 *   RAIL_STATUS_INVALID_PARAMETER.
 *
 * @warning This API is not safe to use in a multiprotocol app.
 */
RAIL_Status_t RAIL_BLE_SetNextCsStep(RAIL_Handle_t railHandle,
                                     const RAIL_BLE_CsStepConfig_t *csStepConfig,
                                     bool pend);

/**
 * Configure antennas for CS event.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] pAntennaConfig A pointer to the antenna config.
 *   The \ref RAIL_BLE_CsAntennaConfig_t::antennaCount of this
 *   parameter must be a value greater than 0
 *   and less than or equal to \ref RAIL_BLE_CS_MAX_ANTENNAS.
 * @return Status code indicating success of the function call.
 */
RAIL_Status_t RAIL_BLE_ConfigCsAntenna(RAIL_Handle_t railHandle,
                                       const RAIL_BLE_CsAntennaConfig_t *pAntennaConfig);

/**
 * Returns the number of antennas configured for a CS event.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return The number of antennas configured for a CS event.
 *
 * @note If \ref RAIL_BLE_ConfigCsAntenna has not been called, this
 *   API will return \ref RAIL_BLE_CS_INVALID_ANTENNA_COUNT as
 *   no antennas have been configured for the CS event. The CS event
 *   will still run with an antenna count of 1 and 0 cm antenna offset.
 */
uint8_t RAIL_BLE_GetCsAntennaCount(RAIL_Handle_t railHandle);

/**
 * Loads the CS RTT and PBR group delay compensation tables for a
 * particular PA mode.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] pTables A pointer to group delay compensation lookup tables.
 * @param[in] powerMode The PA mode for which to load compensation tables.
 * @return Status code indicating success of the function call.
 */
RAIL_Status_t RAIL_BLE_LoadCsCompTables(RAIL_Handle_t railHandle,
                                        const RAIL_BLE_CsGdCompTables_t *pTables,
                                        RAIL_TxPowerMode_t powerMode);

/**
 * Callback used to load CS group delay compensation tables for all PA modes
 * supported by device during \ref RAIL_BLE_EnableCs() when enable is true.
 * This function is optional to implement.
 *
 * @return Status code indicating success of the function call.
 *
 * @note If this callback function is not implemented, unneeded tables may not
 *   be dead stripped, resulting in larger overall code size. The API \ref
 *   RAIL_BLE_LoadCsCompTables() should be used within this callback to load the
 *   appropriate tables for each supported PA mode.
 */
RAIL_Status_t RAILCb_BLE_CsGdCompTableLoad(void);

#endif//SLI_LIBRAIL_ALIAS

/** @} */  // end of group CS
#endif//DOXYGEN_UNDOCUMENTED

/// @addtogroup BLETX2TX BLE TX Channel Hopping
/// @{
/// @code{.c}
/// // Configuration to send one additional packet
/// static RAIL_BLE_TxChannelHoppingConfigEntry_t entry[1];
/// static uint32_t buffer[BUFFER_SIZE];
/// static RAIL_BLE_TxRepeatConfig_t repeat = {
///   .iterations = 1,
///   .repeatOptions = RAIL_TX_REPEAT_OPTION_HOP,
///   .delayOrHop.channelHopping = {
///     .buffer = buffer,
///     .bufferLength = BUFFER_SIZE,
///     .numberOfChannels = 1,
///     .entries = &entry[0],
///   },
/// };
///
/// // Send a normal packet on the current channel, then a packet on a new channel
/// void bleSendThenAdvertise(uint8_t *firstPacket, uint8_t *secondPacket)
/// {
///   // Load both packets into the FIFO
///   RAIL_WriteTxFifo(railHandle, firstPacket, FIRST_PACKET_LEN, true);
///   RAIL_WriteTxFifo(railHandle, secondPacket, SECOND_PACKET_LEN, false);
///
///   // Configure a 300 us turnaround between transmits
///   entry[0].delayMode = RAIL_CHANNEL_HOPPING_DELAY_MODE_STATIC;
///   entry[0].delay = 300; // microseconds
///
///   // Use default advertising parameters
///   entry[0].disableWhitening = false;
///   entry[0].crcInit = 0x00555555;
///   entry[0].accessAddress = 0x8E89BED6;
///
///   // Transmit the repeated packet on the first advertising channel
///   entry[0].phy = RAIL_BLE_1Mbps;
///   entry[0].railChannel = 0;
///   entry[0].logicalChannel = 37;
///
///  // Configure repeated transmit in RAIL, then transmit, sending both packets
///  RAIL_BLE_SetNextTxRepeat(railHandle, &repeat);
///  RAIL_StartTx(railHandle, currentChannel, RAIL_TX_OPTIONS_DEFAULT, NULL);
/// }
/// @endcode

/**
 * @struct RAIL_BLE_TxChannelHoppingConfigEntry_t
 * @brief Structure that represents one of the channels that is part of a
 *   \ref RAIL_BLE_TxChannelHoppingConfig_t sequence of channels used in
 *   channel hopping.
 */
typedef struct RAIL_BLE_TxChannelHoppingConfigEntry {
  /**
   * Idle time in microseconds to wait before hopping into the
   * channel indicated by this entry.
   */
  uint32_t delay;
  /**
   * The BLE PHY to use for this hop's transmit.
   */
  RAIL_BLE_Phy_t phy;
  /**
   * The logical channel to use for this hop's transmit. The whitener will
   * be reinitialized if used.
   */
  uint8_t logicalChannel;
  /**
   * The channel number to be used for this hop's transmit. If this is an
   * invalid channel for the chosen PHY, the call to \ref RAIL_SetNextTxRepeat()
   * will fail.
   */
  uint8_t railChannel;
  /**
   * This can turn off the whitening engine and is useful for sending BLE test
   * mode packets that don't have this turned on.
   */
  bool disableWhitening;
  /**
   * The value to use for CRC initialization.
   */
  uint32_t crcInit;
  /**
   * The access address to use for the connection.
   */
  uint32_t accessAddress;
} RAIL_BLE_TxChannelHoppingConfigEntry_t;

/**
 * @struct RAIL_BLE_TxChannelHoppingConfig_t
 * @brief Wrapper struct that will contain the sequence of
 *   \ref RAIL_BLE_TxChannelHoppingConfigEntry_t that represents the channel
 *   sequence to use during TX Channel Hopping.
 */
typedef struct RAIL_BLE_TxChannelHoppingConfig {
  /**
   * Pointer to contiguous global read-write memory that will be used
   * by RAIL to store channel hopping information throughout its operation.
   * It need not be initialized and applications should never write
   * data anywhere in this buffer.
   */
  uint32_t *buffer;
  /**
   * This parameter must be set to the length of the buffer array. This way,
   * during configuration, the software can confirm it's writing within the
   * range of the buffer. The configuration API will return an error
   * if bufferLength is insufficient.
   */
  uint16_t bufferLength;
  /** The number of channels that is in the channel hopping sequence. */
  uint8_t numberOfChannels;
  /**
   * Pad bytes reserved for future use and currently ignored.
   */
  uint8_t reserved;
  /**
   * A pointer to the first element of an array of \ref
   * RAIL_BLE_TxChannelHoppingConfigEntry_t that represents the channels
   * used during channel hopping. The number of entries in this array must be
   * numberOfChannels.
   */
  RAIL_BLE_TxChannelHoppingConfigEntry_t *entries;
} RAIL_BLE_TxChannelHoppingConfig_t;

/// @struct RAIL_BLE_TxRepeatConfig_t
/// @brief A configuration structure for repeated transmits
///
typedef struct RAIL_BLE_TxRepeatConfig {
  /**
   * The number of repeated transmits to run. A total of (iterations + 1)
   * transmits will go on-air in the absence of errors.
   */
  uint16_t iterations;
  /**
   * Repeat option(s) to apply.
   */
  RAIL_TxRepeatOptions_t repeatOptions;
  /**
   * Per-repeat delay or hopping configuration, depending on repeatOptions.
   */
  union {
    /**
     * When \ref RAIL_TX_REPEAT_OPTION_HOP is not set, this specifies
     * the delay time between each repeated transmit. Specify \ref
     * RAIL_TRANSITION_TIME_KEEP to use the current \ref
     * RAIL_StateTiming_t::txToTx transition time setting.
     */
    RAIL_TransitionTime_t delay;
    /**
     * When \ref RAIL_TX_REPEAT_OPTION_HOP is set, this specifies
     * the channel hopping configuration to use when hopping between
     * repeated transmits. Per-hop delays are configured within each
     * \ref RAIL_BLE_TxChannelHoppingConfigEntry_t::delay rather than
     * this union's delay field.
     */
    RAIL_BLE_TxChannelHoppingConfig_t channelHopping;
  } delayOrHop;
} RAIL_BLE_TxRepeatConfig_t;

#ifndef SLI_LIBRAIL_ALIAS

/**
 * Set up automatic repeated transmits after the next transmit.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] repeatConfig A non-NULL pointer to the configuration structure for repeated transmits.
 * @return Status code indicating a success of the function call.
 *
 * Repeated transmits will occur after an application-initiated transmit caused
 * by calling one of the \ref Packet_TX APIs. The repetition will only occur
 * after the first application-initiated transmit after this function is
 * called. Future repeated transmits must be requested by calling this function
 * again.
 *
 * Each repeated transmit that occurs will have full \ref PTI information and
 * will receive events such as \ref RAIL_EVENT_TX_PACKET_SENT as normal.
 *
 * If a TX error occurs during the repetition, the process will abort and the
 * TX error transition from \ref RAIL_SetTxTransitions() will be used. If the
 * repetition completes successfully, the TX success transition from
 * \ref RAIL_SetTxTransitions() will be used.
 *
 * Any call to \ref RAIL_Idle(), \ref RAIL_StopTx(), or \ref
 * RAIL_SetTxTransitions() will clear the pending
 * repeated transmits. The state will also be cleared by another call to this
 * function. To clear the repeated transmits before they've started without
 * stopping other radio actions, call this function with a \ref
 * RAIL_BLE_TxRepeatConfig_t::iterations count of 0. A DMP switch will clear this
 * state only if the initial transmit triggering the repeated transmits has
 * started.
 *
 * The application is responsible for populating the transmit data to be used
 * by the repeated transmits via \ref RAIL_SetTxFifo() or \ref RAIL_WriteTxFifo().
 * Data will be transmitted from the TX FIFO. If the TX FIFO does not have
 * sufficient data to transmit, a TX error and a \ref
 * RAIL_EVENT_TX_UNDERFLOW will occur. To avoid an underflow, the
 * application should queue data to be transmitted as early as possible.
 *
 * This function will fail to configure the repetition if a transmit of any
 * kind is ongoing, including during the time between an initial transmit and
 * the end of a previously-configured repetition.
 *
 * @note Use the compile time symbol \ref RAIL_SUPPORTS_TX_TO_TX or the runtime
 *   call \ref RAIL_SupportsTxToTx() to check whether the platform supports
 *   this feature.
 */
RAIL_Status_t RAIL_BLE_SetNextTxRepeat(RAIL_Handle_t railHandle,
                                       const RAIL_BLE_TxRepeatConfig_t *repeatConfig);

/** @} */  // end of group BLETX2TX

/** @} */ // end of BLE

/// @addtogroup Calibration
/// @brief Bluetooth protocol-specific APIs for calibrating the radio.
/// @{

/**
 * Calibrate image rejection for Bluetooth Low Energy.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[out] imageRejection A pointer to where the result of the image
 *   rejection calibration will be stored.
 * @return Status code indicating success of the function call.
 *
 * Some chips have protocol-specific image rejection calibrations programmed
 * into their flash. This function will either get the value from flash and
 * apply it, or run the image rejection algorithm to find the value.
 */
RAIL_Status_t RAIL_BLE_CalibrateIr(RAIL_Handle_t railHandle,
                                   uint32_t *imageRejection);

/// @} // End of group Calibration

#endif//SLI_LIBRAIL_ALIAS

#ifdef __cplusplus
}
#endif

#endif // __RAIL_BLE_H__
