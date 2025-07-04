/***************************************************************************//**
 * @file
 * @brief Hall sensor header
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_SENSOR_HALL_H
#define SL_SENSOR_HALL_H

/***********************************************************************************************//**
 * @addtogroup sensor_hall
 * @{
 **************************************************************************************************/

#include <stdbool.h>
#include "sl_status.h"

/**************************************************************************//**
 * Initialize hall sensor.
 *
 * \note With certain boards (e.g. 4184A, 4184B), using this initialization
 * function enables other sensors, because they're on the same enable pin.
 * Please take that into account when using this function.
 *
 * @return Status of the operation.
 *****************************************************************************/
sl_status_t sensor_hall_init(void);

/**************************************************************************//**
 * Deinitialize hall sensor.
 *
 * \warning With certain boards (e.g. 4184A, 4184B), using this deinitialization
 * function disables other sensors, because they're on the same enable pin.
 * Please use with caution.
 *****************************************************************************/
void sensor_hall_deinit(void);

/**************************************************************************//**
 * Getter for hall sensor measurement data.
 * @param[out] field_strength Field strength level (in mT).
 * @param[out] alert Field strength has reached the alert level.
 * @param[out] tamper Field strength has reached the tamper level.
 * @return Status of the operation.
 *****************************************************************************/
sl_status_t sensor_hall_get(float *field_strength, bool *alert, bool *tamper);

/** @} (end addtogroup sensor_hall) */
#endif // SL_SENSOR_HALL_H
