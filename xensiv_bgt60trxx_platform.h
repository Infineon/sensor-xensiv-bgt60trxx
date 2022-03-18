/***********************************************************************************************//**
 * \file xensiv_bgt60trxx_platform.h
 *
 * Description: XENSIV(TM) BGT60TRxx 60GHz FMCW radar sensors library platform dependencies
 *
 ***************************************************************************************************
 * \copyright
 * Copyright 2021 Infineon Technologies AG
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 **************************************************************************************************/

#ifndef XENSIV_BGT60TRXX_PLATFORM_H_
#define XENSIV_BGT60TRXX_PLATFORM_H_

/**
 * \addtogroup group_board_libs_platform XENSIV BGT60TRxx radar sensor platform functions
 * \{
 * XENSIV&trade; BGT60TRxx radar sensor platform functions.
 *
 * To adapt the driver to your platform, you need to provide an implementation for the functions
 * declared in this file. See the example implementation in xensiv_bgt60trxx_mtb.c using the
 * PSoC&trade; 6 HAL.
 *
 */

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Platform-specific function that performs an SPI transfer.
 *
 * @param[in] dev SPI object.
 * @param[in] tx_data Transmit buffer. @note: Set to NULL to fill data.
 * @param[in] tx_len Number of bytes to write.
 * @param[in] rx_data Receive buffer. @note: Set to NULL if not interested in read data.
 * @param[in] rx_len Number of bytes to read.
 * @return XENSIV_BGT60TRXX_STATUS_OK if the transfer is complete without errors, otherwise returns
 * XENSIV_BGT60TRXX_STATUS_SPI_ERROR.
 */
int32_t xensiv_bgt60trxx_platform_spi_transfer(void* dev,
                                               const uint8_t* tx_data,
                                               uint32_t tx_len,
                                               uint8_t* rx_data,
                                               uint32_t rx_len);

/**
 * @brief Platform-specific function that waits for a specified time period in milliseconds.
 *
 * @param[in] ms Number of milliseconds to wait for.
 */
void xensiv_bgt60trxx_platform_delay(uint32_t ms);

/**
 * @brief Platform-specific function to reverse the byte order (32 bits).
 * A sample implementation would look like
 * \code
 *  return (((x & 0x000000ffUL) << 24) |
 *          ((x & 0x0000ff00UL) <<  8) |
 *          ((x & 0x00ff0000UL) >>  8) |
 *          ((x & 0xff000000UL) >> 24));
 * \endcode
 *
 * @param[in] x Value to reverse.
 * @return Reversed value.
 */
uint32_t xensiv_bgt60trxx_platform_word_reverse(uint32_t x);

/**
 * @brief Platform-specific function that implements a runtime assertion; used to verify the
 * assumptions made by the program and take appropriate actions if the assumption is false.
 *
 * @param[in] expr Expression to be verified.
 */
void xensiv_bgt60trxx_platform_assert(bool expr);

#ifdef __cplusplus
}
#endif

/** \} group_board_libs_platform */

#endif // ifndef XENSIV_BGT60TRXX_PLATFORM_H_
