/***************************************************************************//**
 * @file
 * @brief Hall Effect GATT Service header
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

#ifndef SL_GATT_SERVICE_HALL_H
#define SL_GATT_SERVICE_HALL_H

/***********************************************************************************************//**
 * @addtogroup gatt_service_hall
 * @{
 **************************************************************************************************/

#include "sl_bt_api.h"

/**************************************************************************//**
 * Bluetooth stack event handler.
 * @param[in] evt Event coming from the Bluetooth stack.
 *****************************************************************************/
void sl_gatt_service_hall_on_event(sl_bt_msg_t *evt);

/**************************************************************************//**
 * Getter for Field Strength and State characteristic values.
 * @param[out] field_strength Field strength level (in mT).
 * @param[out] alert Field strength has reached the alert level.
 * @param[out] tamper Field strength has reached the tamper level.
 * @return Status of the operation.
 * @note To be implemented in user code.
 *****************************************************************************/
sl_status_t sl_gatt_service_hall_get(float *field_strength, bool *alert, bool *tamper);

/** @} (end addtogroup gatt_service_hall) */
#endif // SL_GATT_SERVICE_HALL_H
