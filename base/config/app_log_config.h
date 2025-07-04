/***************************************************************************//**
 * @file
 * @brief Application logging configuration
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef APP_LOG_CONFIG_H
#define APP_LOG_CONFIG_H

#include "sl_iostream.h"

// <<< Use Configuration Wizard in Context Menu >>>

#define APP_LOG_NEW_LINE_N                    "\n"
#define APP_LOG_NEW_LINE_R                    "\r"
#define APP_LOG_NEW_LINE_RN                   "\r\n"

#define APP_LOG_HEXDUMP_PREFIX_NONE           ""
#define APP_LOG_HEXDUMP_PREFIX_0X_U           "0X"
#define APP_LOG_HEXDUMP_PREFIX_0X_L           "0x"
#define APP_LOG_HEXDUMP_PREFIX_X_U            "X"
#define APP_LOG_HEXDUMP_PREFIX_X_L            "x"

#define APP_LOG_HEXDUMP_FORMAT_PAD0_U         "%02X"
#define APP_LOG_HEXDUMP_FORMAT_PAD0_L         "%02x"
#define APP_LOG_HEXDUMP_FORMAT_PAD_U          "%2X"
#define APP_LOG_HEXDUMP_FORMAT_PAD_L          "%2x"
#define APP_LOG_HEXDUMP_FORMAT_U              "%X"
#define APP_LOG_HEXDUMP_FORMAT_L              "%x"

#define APP_LOG_SEPARATOR_NONE        ""
#define APP_LOG_SEPARATOR_SPACE       " "
#define APP_LOG_SEPARATOR_COMMA       ","
#define APP_LOG_SEPARATOR_TAB         "\t"
#define APP_LOG_SEPARATOR_DASH        "-"
#define APP_LOG_SEPARATOR_UNDER       "_"
#define APP_LOG_SEPARATOR_COLON       ":"
#define APP_LOG_SEPARATOR_SEMI        ";"

// APP_LOG_HEXDUMP_SEPARATOR_* macros are deprecated, use APP_LOG_SEPARATOR_* macros instead.
#define APP_LOG_HEXDUMP_SEPARATOR_NONE        APP_LOG_SEPARATOR_NONE
#define APP_LOG_HEXDUMP_SEPARATOR_SPACE       APP_LOG_SEPARATOR_SPACE
#define APP_LOG_HEXDUMP_SEPARATOR_COMMA       APP_LOG_SEPARATOR_COMMA
#define APP_LOG_HEXDUMP_SEPARATOR_TAB         APP_LOG_SEPARATOR_TAB
#define APP_LOG_HEXDUMP_SEPARATOR_DASH        APP_LOG_SEPARATOR_DASH
#define APP_LOG_HEXDUMP_SEPARATOR_UNDER       APP_LOG_SEPARATOR_UNDER
#define APP_LOG_HEXDUMP_SEPARATOR_COLON       APP_LOG_SEPARATOR_COLON
#define APP_LOG_HEXDUMP_SEPARATOR_SEMI        APP_LOG_SEPARATOR_SEMI

// <e APP_LOG_ENABLE> Application Logging
// <i> Enables Logging.
#define APP_LOG_ENABLE            1

// <h> General

// <q APP_LOG_TRACE_ENABLE> Trace
// <i> Enables printing file, line and function information.
#define APP_LOG_TRACE_ENABLE      0

// <q APP_LOG_TIME_ENABLE> Timestamp
// <i> Enables printing timestamp
#define APP_LOG_TIME_ENABLE                     0

// <q APP_LOG_COUNTER_ENABLE> Counter
// <i> Enables printing counter
#define APP_LOG_COUNTER_ENABLE                  0

// <o APP_LOG_NEW_LINE> Line ending
// <APP_LOG_NEW_LINE_N=> "\n"
// <APP_LOG_NEW_LINE_R=> "\r"
// <APP_LOG_NEW_LINE_RN=> "\r\n"
// <i> Default: APP_LOG_NEW_LINE_N
#define APP_LOG_NEW_LINE                        APP_LOG_NEW_LINE_RN

#define APP_LOG_AUTO_NL                         0

// </h>

// <h> Log level filtering

// <e APP_LOG_LEVEL_FILTER_ENABLE> Threshold filter
// <i> Enable simple filter for log levels
#define APP_LOG_LEVEL_FILTER_ENABLE            1

// <o APP_LOG_LEVEL_FILTER_THRESHOLD> Threshold
// <APP_LOG_LEVEL_DEBUG=> DEBUG
// <APP_LOG_LEVEL_INFO=> INFO
// <APP_LOG_LEVEL_WARNING=> WARNING
// <APP_LOG_LEVEL_ERROR=> ERROR
// <APP_LOG_LEVEL_CRITICAL=> CRITICAL
// <i> Default: DEBUG
#define APP_LOG_LEVEL_FILTER_THRESHOLD              APP_LOG_LEVEL_DEBUG

// </e>

// <e APP_LOG_LEVEL_MASK_ENABLE> Mask filter
// <i> Enable mask for log levels
#define APP_LOG_LEVEL_MASK_ENABLE            0

// <q APP_LOG_LEVEL_MASK_DEBUG> DEBUG
// <i> Enable mask entry for log DEBUG
#define APP_LOG_LEVEL_MASK_DEBUG             1

// <q APP_LOG_LEVEL_MASK_INFO> INFO
// <i> Enable mask entry for log INFO
#define APP_LOG_LEVEL_MASK_INFO             1

// <q APP_LOG_LEVEL_MASK_WARNING> WARNING
// <i> Enable mask entry for log WARNING
#define APP_LOG_LEVEL_MASK_WARNING             1

// <q APP_LOG_LEVEL_MASK_ERROR> ERROR
// <i> Enable mask entry for log ERROR
#define APP_LOG_LEVEL_MASK_ERROR             1

// <q APP_LOG_LEVEL_MASK_CRITICAL> CRITICAL
// <i> Enable mask entry for log CRITICAL
#define APP_LOG_LEVEL_MASK_CRITICAL             1

// </e>

// </h>

// <e APP_LOG_OVERRIDE_DEFAULT_STREAM> Override default stream
// <i> Enable overriding the system level default stream to use for logging.
#define APP_LOG_OVERRIDE_DEFAULT_STREAM            0

// <o APP_LOG_STREAM_TYPE> Stream type
// <SL_IOSTREAM_TYPE_SWO=> SWO
// <SL_IOSTREAM_TYPE_RTT=> RTT
// <SL_IOSTREAM_TYPE_UART=> UART
// <SL_IOSTREAM_TYPE_VUART=> VUART
// <i> Default: UART
#define APP_LOG_STREAM_TYPE        SL_IOSTREAM_TYPE_UART

// <s APP_LOG_STREAM_INSTANCE> Preferred instance
// <i> Preferred IOStream instance name
#define APP_LOG_STREAM_INSTANCE    "vcom"

// </e>

// <h> Dump settings

// <o APP_LOG_HEXDUMP_PREFIX> Prefix
// <APP_LOG_HEXDUMP_PREFIX_NONE=> None
// <APP_LOG_HEXDUMP_PREFIX_0X_U=> 0X
// <APP_LOG_HEXDUMP_PREFIX_0X_L=> 0x
// <APP_LOG_HEXDUMP_PREFIX_X_U=> X
// <APP_LOG_HEXDUMP_PREFIX_X_L=> x
// <i> Default: None
// <i> Format string for hexdump
#define APP_LOG_HEXDUMP_PREFIX                 APP_LOG_HEXDUMP_PREFIX_NONE

// <o APP_LOG_HEXDUMP_FORMAT> Format
// <APP_LOG_HEXDUMP_FORMAT_PAD0_U=> Uppercase with 0 padding (0F)
// <APP_LOG_HEXDUMP_FORMAT_PAD0_L=> Lowercase with 0 padding (0f)
// <APP_LOG_HEXDUMP_FORMAT_PAD_U=> Uppercase with padding ( F)
// <APP_LOG_HEXDUMP_FORMAT_PAD_L=> Lowercase with padding ( f)
// <APP_LOG_HEXDUMP_FORMAT_U=> Lowercase without padding (F)
// <APP_LOG_HEXDUMP_FORMAT_L=> Lowercase without padding (f)
// <i> Default: %02X
// <i> Format string for hexdump
#define APP_LOG_HEXDUMP_FORMAT                 APP_LOG_HEXDUMP_FORMAT_PAD0_U

// <o APP_LOG_HEXDUMP_SEPARATOR>Hexdump separator
// <APP_LOG_SEPARATOR_NONE=> None
// <APP_LOG_SEPARATOR_SPACE=> Space ( )
// <APP_LOG_SEPARATOR_COMMA=> Comma(,)
// <APP_LOG_SEPARATOR_TAB=> Tab (\t)
// <APP_LOG_SEPARATOR_DASH=> Dash (-)
// <APP_LOG_SEPARATOR_UNDER=> Underscore (_)
// <APP_LOG_SEPARATOR_COLON=> Colon(:)
// <APP_LOG_SEPARATOR_SEMI=> Semicolon(;)
// <i> Default: " "
// <i> Separator for hexdump
#define APP_LOG_HEXDUMP_SEPARATOR            APP_LOG_SEPARATOR_SPACE

// <o APP_LOG_ARRAY_DUMP_SEPARATOR>Array Dump Separator
// <APP_LOG_SEPARATOR_NONE=> None
// <APP_LOG_SEPARATOR_SPACE=> Space ( )
// <APP_LOG_SEPARATOR_COMMA=> Comma(,)
// <APP_LOG_SEPARATOR_TAB=> Tab (\t)
// <APP_LOG_SEPARATOR_DASH=> Dash (-)
// <APP_LOG_SEPARATOR_UNDER=> Underscore (_)
// <APP_LOG_SEPARATOR_COLON=> Colon(:)
// <APP_LOG_SEPARATOR_SEMI=> Semicolon(;)
// <i> Default: " "
// <i> Separator for array dump
#define APP_LOG_ARRAY_DUMP_SEPARATOR         APP_LOG_SEPARATOR_SPACE

// <o APP_LOG_CUSTOM_ARRAY_DUMP_SEPARATOR>Custom Array Dump Separator
// <APP_LOG_SEPARATOR_NONE=> None
// <APP_LOG_SEPARATOR_SPACE=> Space ( )
// <APP_LOG_SEPARATOR_COMMA=> Comma(,)
// <APP_LOG_SEPARATOR_TAB=> Tab (\t)
// <APP_LOG_SEPARATOR_DASH=> Dash (-)
// <APP_LOG_SEPARATOR_UNDER=> Underscore (_)
// <APP_LOG_SEPARATOR_COLON=> Colon(:)
// <APP_LOG_SEPARATOR_SEMI=> Semicolon(;)
// <i> Default: " "
// <i> Separator for custom array dump
#define APP_LOG_CUSTOM_ARRAY_DUMP_SEPARATOR   APP_LOG_SEPARATOR_SPACE
// </h>

// <e APP_LOG_PREFIX_ENABLE> Log level prefixes
// <i> Enables for logging.
#define APP_LOG_PREFIX_ENABLE                    1

// <s APP_LOG_LEVEL_DEBUG_PREFIX> Prefix for DEBUG
// <i> Log prefix for DEBUG
#define APP_LOG_LEVEL_DEBUG_PREFIX              "[D]"

// <s APP_LOG_LEVEL_INFO_PREFIX> Prefix for INFO
// <i> Log prefix for INFO
#define APP_LOG_LEVEL_INFO_PREFIX               "[I]"

// <s APP_LOG_LEVEL_WARNING_PREFIX> Prefix for WARNING
// <i> Log prefix for WARNING
#define APP_LOG_LEVEL_WARNING_PREFIX            "[W]"

// <s APP_LOG_LEVEL_ERROR_PREFIX> Prefix for ERROR
// <i> Log prefix for ERROR
#define APP_LOG_LEVEL_ERROR_PREFIX              "[E]"

// <s APP_LOG_LEVEL_CRITICAL_PREFIX> Prefix for CRITICAL
// <i> Log prefix for CRITICAL
#define APP_LOG_LEVEL_CRITICAL_PREFIX           "[C]"

// </e>

// <e APP_LOG_COLOR_ENABLE> Colors for terminal logging
// <i> Enables color prefixing for logging to terminal.
#define APP_LOG_COLOR_ENABLE                    0

// <h> Text color

// <o APP_LOG_LEVEL_DEBUG_COLOR> Color for DEBUG
// <APP_LOG_COLOR_BLACK=> BLACK
// <APP_LOG_COLOR_RED=> RED
// <APP_LOG_COLOR_GREEN=> GREEN
// <APP_LOG_COLOR_BLUE=> BLUE
// <APP_LOG_COLOR_YELLOW=> YELLOW
// <APP_LOG_COLOR_MAGENTA=> MAGENTA
// <APP_LOG_COLOR_CYAN=> CYAN
// <APP_LOG_COLOR_WHITE=> WHITE
// <APP_LOG_COLOR_BRIGHT_RED=> BRIGHT_RED
// <APP_LOG_COLOR_BRIGHT_GREEN=> BRIGHT_GREEN
// <APP_LOG_COLOR_BRIGHT_BLUE=> BRIGHT_BLUE
// <APP_LOG_COLOR_BRIGHT_YELLOW=> BRIGHT_YELLOW
// <APP_LOG_COLOR_MAGENTA=> BRIGHT_MAGENTA
// <APP_LOG_COLOR_BRIGHT_CYAN=> BRIGHT_CYAN
// <APP_LOG_COLOR_BRIGHT_WHITE=> BRIGHT_WHITE
// <i> Default: BRIGHT_WHITE
#define APP_LOG_LEVEL_DEBUG_COLOR              APP_LOG_COLOR_BRIGHT_WHITE

// <o APP_LOG_LEVEL_INFO_COLOR> Color for INFO
// <APP_LOG_COLOR_BLACK=> BLACK
// <APP_LOG_COLOR_RED=> RED
// <APP_LOG_COLOR_GREEN=> GREEN
// <APP_LOG_COLOR_BLUE=> BLUE
// <APP_LOG_COLOR_YELLOW=> YELLOW
// <APP_LOG_COLOR_MAGENTA=> MAGENTA
// <APP_LOG_COLOR_CYAN=> CYAN
// <APP_LOG_COLOR_WHITE=> WHITE
// <APP_LOG_COLOR_BRIGHT_RED=> BRIGHT_RED
// <APP_LOG_COLOR_BRIGHT_GREEN=> BRIGHT_GREEN
// <APP_LOG_COLOR_BRIGHT_BLUE=> BRIGHT_BLUE
// <APP_LOG_COLOR_BRIGHT_YELLOW=> BRIGHT_YELLOW
// <APP_LOG_COLOR_MAGENTA=> BRIGHT_MAGENTA
// <APP_LOG_COLOR_BRIGHT_CYAN=> BRIGHT_CYAN
// <APP_LOG_COLOR_BRIGHT_WHITE=> BRIGHT_WHITE
// <i> Default: BRIGHT_CYAN
#define APP_LOG_LEVEL_INFO_COLOR               APP_LOG_COLOR_BRIGHT_CYAN

// <o APP_LOG_LEVEL_WARNING_COLOR> Color for WARNING
// <APP_LOG_COLOR_BLACK=> BLACK
// <APP_LOG_COLOR_RED=> RED
// <APP_LOG_COLOR_GREEN=> GREEN
// <APP_LOG_COLOR_BLUE=> BLUE
// <APP_LOG_COLOR_YELLOW=> YELLOW
// <APP_LOG_COLOR_MAGENTA=> MAGENTA
// <APP_LOG_COLOR_CYAN=> CYAN
// <APP_LOG_COLOR_WHITE=> WHITE
// <APP_LOG_COLOR_BRIGHT_RED=> BRIGHT_RED
// <APP_LOG_COLOR_BRIGHT_GREEN=> BRIGHT_GREEN
// <APP_LOG_COLOR_BRIGHT_BLUE=> BRIGHT_BLUE
// <APP_LOG_COLOR_BRIGHT_YELLOW=> BRIGHT_YELLOW
// <APP_LOG_COLOR_MAGENTA=> BRIGHT_MAGENTA
// <APP_LOG_COLOR_BRIGHT_CYAN=> BRIGHT_CYAN
// <APP_LOG_COLOR_BRIGHT_WHITE=> BRIGHT_WHITE
// <i> Default: BRIGHT_YELLOW
#define APP_LOG_LEVEL_WARNING_COLOR            APP_LOG_COLOR_BRIGHT_YELLOW

// <o APP_LOG_LEVEL_ERROR_COLOR> Color for ERROR
// <APP_LOG_COLOR_BLACK=> BLACK
// <APP_LOG_COLOR_RED=> RED
// <APP_LOG_COLOR_GREEN=> GREEN
// <APP_LOG_COLOR_BLUE=> BLUE
// <APP_LOG_COLOR_YELLOW=> YELLOW
// <APP_LOG_COLOR_MAGENTA=> MAGENTA
// <APP_LOG_COLOR_CYAN=> CYAN
// <APP_LOG_COLOR_WHITE=> WHITE
// <APP_LOG_COLOR_BRIGHT_RED=> BRIGHT_RED
// <APP_LOG_COLOR_BRIGHT_GREEN=> BRIGHT_GREEN
// <APP_LOG_COLOR_BRIGHT_BLUE=> BRIGHT_BLUE
// <APP_LOG_COLOR_BRIGHT_YELLOW=> BRIGHT_YELLOW
// <APP_LOG_COLOR_MAGENTA=> BRIGHT_MAGENTA
// <APP_LOG_COLOR_BRIGHT_CYAN=> BRIGHT_CYAN
// <APP_LOG_COLOR_BRIGHT_WHITE=> BRIGHT_WHITE
// <i> Default: BRIGHT_RED
#define APP_LOG_LEVEL_ERROR_COLOR              APP_LOG_COLOR_BRIGHT_RED

// <o APP_LOG_LEVEL_CRITICAL_COLOR> Color for CRITICAL
// <APP_LOG_COLOR_BLACK=> BLACK
// <APP_LOG_COLOR_RED=> RED
// <APP_LOG_COLOR_GREEN=> GREEN
// <APP_LOG_COLOR_BLUE=> BLUE
// <APP_LOG_COLOR_YELLOW=> YELLOW
// <APP_LOG_COLOR_MAGENTA=> MAGENTA
// <APP_LOG_COLOR_CYAN=> CYAN
// <APP_LOG_COLOR_WHITE=> WHITE
// <APP_LOG_COLOR_BRIGHT_RED=> BRIGHT_RED
// <APP_LOG_COLOR_BRIGHT_GREEN=> BRIGHT_GREEN
// <APP_LOG_COLOR_BRIGHT_BLUE=> BRIGHT_BLUE
// <APP_LOG_COLOR_BRIGHT_YELLOW=> BRIGHT_YELLOW
// <APP_LOG_COLOR_MAGENTA=> BRIGHT_MAGENTA
// <APP_LOG_COLOR_BRIGHT_CYAN=> BRIGHT_CYAN
// <APP_LOG_COLOR_BRIGHT_WHITE=> BRIGHT_WHITE
// <i> Default: BRIGHT_WHITE
#define APP_LOG_LEVEL_CRITICAL_COLOR           APP_LOG_COLOR_BRIGHT_WHITE

// </h>

// <h> Background color

// <o APP_LOG_LEVEL_DEBUG_BACKGROUND_COLOR> Background color for DEBUG
// <APP_LOG_BACKGROUND_COLOR_NONE=> NONE
// <APP_LOG_BACKGROUND_COLOR_BLACK=> BLACK
// <APP_LOG_BACKGROUND_COLOR_RED=> RED
// <APP_LOG_BACKGROUND_COLOR_GREEN=> GREEN
// <APP_LOG_BACKGROUND_COLOR_BLUE=> BLUE
// <APP_LOG_BACKGROUND_COLOR_YELLOW=> YELLOW
// <APP_LOG_BACKGROUND_COLOR_MAGENTA=> MAGENTA
// <APP_LOG_BACKGROUND_COLOR_CYAN=> CYAN
// <APP_LOG_BACKGROUND_COLOR_WHITE=> WHITE
// <APP_LOG_BACKGROUND_COLOR_BRIGHT_RED=> BRIGHT_RED
// <APP_LOG_BACKGROUND_COLOR_BRIGHT_GREEN=> BRIGHT_GREEN
// <APP_LOG_BACKGROUND_COLOR_BRIGHT_BLUE=> BRIGHT_BLUE
// <APP_LOG_BACKGROUND_COLOR_BRIGHT_YELLOW=> BRIGHT_YELLOW
// <APP_LOG_BACKGROUND_COLOR_BRIGHT_MAGENTA=> BRIGHT_MAGENTA
// <APP_LOG_BACKGROUND_COLOR_BRIGHT_CYAN=> BRIGHT_CYAN
// <APP_LOG_BACKGROUND_COLOR_BRIGHT_WHITE=> BRIGHT_WHITE
// <i> Default: NONE
#define APP_LOG_LEVEL_DEBUG_BACKGROUND_COLOR              APP_LOG_BACKGROUND_COLOR_NONE

// <o APP_LOG_LEVEL_INFO_BACKGROUND_COLOR> Background color for INFO
// <APP_LOG_BACKGROUND_COLOR_NONE=> NONE
// <APP_LOG_BACKGROUND_COLOR_BLACK=> BLACK
// <APP_LOG_BACKGROUND_COLOR_RED=> RED
// <APP_LOG_BACKGROUND_COLOR_GREEN=> GREEN
// <APP_LOG_BACKGROUND_COLOR_BLUE=> BLUE
// <APP_LOG_BACKGROUND_COLOR_YELLOW=> YELLOW
// <APP_LOG_BACKGROUND_COLOR_MAGENTA=> MAGENTA
// <APP_LOG_BACKGROUND_COLOR_CYAN=> CYAN
// <APP_LOG_BACKGROUND_COLOR_WHITE=> WHITE
// <APP_LOG_BACKGROUND_COLOR_BRIGHT_RED=> BRIGHT_RED
// <APP_LOG_BACKGROUND_COLOR_BRIGHT_GREEN=> BRIGHT_GREEN
// <APP_LOG_BACKGROUND_COLOR_BRIGHT_BLUE=> BRIGHT_BLUE
// <APP_LOG_BACKGROUND_COLOR_BRIGHT_YELLOW=> BRIGHT_YELLOW
// <APP_LOG_BACKGROUND_COLOR_BRIGHT_MAGENTA=> BRIGHT_MAGENTA
// <APP_LOG_BACKGROUND_COLOR_BRIGHT_CYAN=> BRIGHT_CYAN
// <APP_LOG_BACKGROUND_COLOR_BRIGHT_WHITE=> BRIGHT_WHITE
// <i> Default: NONE
#define APP_LOG_LEVEL_INFO_BACKGROUND_COLOR               APP_LOG_BACKGROUND_COLOR_NONE

// <o APP_LOG_LEVEL_WARNING_BACKGROUND_COLOR> Background color for WARNING
// <APP_LOG_BACKGROUND_COLOR_NONE=> NONE
// <APP_LOG_BACKGROUND_COLOR_BLACK=> BLACK
// <APP_LOG_BACKGROUND_COLOR_RED=> RED
// <APP_LOG_BACKGROUND_COLOR_GREEN=> GREEN
// <APP_LOG_BACKGROUND_COLOR_BLUE=> BLUE
// <APP_LOG_BACKGROUND_COLOR_YELLOW=> YELLOW
// <APP_LOG_BACKGROUND_COLOR_MAGENTA=> MAGENTA
// <APP_LOG_BACKGROUND_COLOR_CYAN=> CYAN
// <APP_LOG_BACKGROUND_COLOR_WHITE=> WHITE
// <APP_LOG_BACKGROUND_COLOR_BRIGHT_RED=> BRIGHT_RED
// <APP_LOG_BACKGROUND_COLOR_BRIGHT_GREEN=> BRIGHT_GREEN
// <APP_LOG_BACKGROUND_COLOR_BRIGHT_BLUE=> BRIGHT_BLUE
// <APP_LOG_BACKGROUND_COLOR_BRIGHT_YELLOW=> BRIGHT_YELLOW
// <APP_LOG_BACKGROUND_COLOR_BRIGHT_MAGENTA=> BRIGHT_MAGENTA
// <APP_LOG_BACKGROUND_COLOR_BRIGHT_CYAN=> BRIGHT_CYAN
// <APP_LOG_BACKGROUND_COLOR_BRIGHT_WHITE=> BRIGHT_WHITE
// <i> Default: NONE
#define APP_LOG_LEVEL_WARNING_BACKGROUND_COLOR            APP_LOG_BACKGROUND_COLOR_NONE

// <o APP_LOG_LEVEL_ERROR_BACKGROUND_COLOR> Background color for ERROR
// <APP_LOG_BACKGROUND_COLOR_NONE=> NONE
// <APP_LOG_BACKGROUND_COLOR_BLACK=> BLACK
// <APP_LOG_BACKGROUND_COLOR_RED=> RED
// <APP_LOG_BACKGROUND_COLOR_GREEN=> GREEN
// <APP_LOG_BACKGROUND_COLOR_BLUE=> BLUE
// <APP_LOG_BACKGROUND_COLOR_YELLOW=> YELLOW
// <APP_LOG_BACKGROUND_COLOR_MAGENTA=> MAGENTA
// <APP_LOG_BACKGROUND_COLOR_CYAN=> CYAN
// <APP_LOG_BACKGROUND_COLOR_WHITE=> WHITE
// <APP_LOG_BACKGROUND_COLOR_BRIGHT_RED=> BRIGHT_RED
// <APP_LOG_BACKGROUND_COLOR_BRIGHT_GREEN=> BRIGHT_GREEN
// <APP_LOG_BACKGROUND_COLOR_BRIGHT_BLUE=> BRIGHT_BLUE
// <APP_LOG_BACKGROUND_COLOR_BRIGHT_YELLOW=> BRIGHT_YELLOW
// <APP_LOG_BACKGROUND_COLOR_BRIGHT_MAGENTA=> BRIGHT_MAGENTA
// <APP_LOG_BACKGROUND_COLOR_BRIGHT_CYAN=> BRIGHT_CYAN
// <APP_LOG_BACKGROUND_COLOR_BRIGHT_WHITE=> BRIGHT_WHITE
// <i> Default: NONE
#define APP_LOG_LEVEL_ERROR_BACKGROUND_COLOR              APP_LOG_BACKGROUND_COLOR_NONE

// <o APP_LOG_LEVEL_CRITICAL_BACKGROUND_COLOR> Background color for CRITICAL
// <APP_LOG_BACKGROUND_COLOR_NONE=> NONE
// <APP_LOG_BACKGROUND_COLOR_BLACK=> BLACK
// <APP_LOG_BACKGROUND_COLOR_RED=> RED
// <APP_LOG_BACKGROUND_COLOR_GREEN=> GREEN
// <APP_LOG_BACKGROUND_COLOR_BLUE=> BLUE
// <APP_LOG_BACKGROUND_COLOR_YELLOW=> YELLOW
// <APP_LOG_BACKGROUND_COLOR_MAGENTA=> MAGENTA
// <APP_LOG_BACKGROUND_COLOR_CYAN=> CYAN
// <APP_LOG_BACKGROUND_COLOR_WHITE=> WHITE
// <APP_LOG_BACKGROUND_COLOR_BRIGHT_RED=> BRIGHT_RED
// <APP_LOG_BACKGROUND_COLOR_BRIGHT_GREEN=> BRIGHT_GREEN
// <APP_LOG_BACKGROUND_COLOR_BRIGHT_BLUE=> BRIGHT_BLUE
// <APP_LOG_BACKGROUND_COLOR_BRIGHT_YELLOW=> BRIGHT_YELLOW
// <APP_LOG_BACKGROUND_COLOR_BRIGHT_MAGENTA=> BRIGHT_MAGENTA
// <APP_LOG_BACKGROUND_COLOR_BRIGHT_CYAN=> BRIGHT_CYAN
// <APP_LOG_BACKGROUND_COLOR_BRIGHT_WHITE=> BRIGHT_WHITE
// <i> Default: BRIGHT_RED
#define APP_LOG_LEVEL_CRITICAL_BACKGROUND_COLOR           APP_LOG_BACKGROUND_COLOR_BRIGHT_RED

// </h>

// </e>

// </e>

// <<< end of configuration section >>>

#endif // APP_LOG_CONFIG_H
