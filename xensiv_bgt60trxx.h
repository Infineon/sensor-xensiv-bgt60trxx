/***********************************************************************************************//**
 * \file xensiv_bgt60trxx.h
 *
 * Description: This file contains the functions for interacting with the
 *              XENSIV(TM) BGT60TRxx 60GHz FMCW radar sensors.
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

#ifndef XENSIV_BGT60TRXX_H_
#define XENSIV_BGT60TRXX_H_

/**
 * \addtogroup group_board_libs XENSIV(TM) BGT60TRxx Radar Sensor
 * \{
 * Basic set of APIs for interacting with the XENSIV(TM) BGT60TRxx 60GHz FMCW radar sensors.
 * The library allows:
 * - sensor initialization
 * - set/get individual registers of the sensor
 * - start/stop the frame generation
 * - read the digitized IF signal from the sensor FIFO
 * - set the FIFO level-filling threshold
 * - enable/disable the data test mode
 * - software reset the sensor
 *
 * More information about the XENSIV(TM) BGT60TRxx 60GHz FMCW radar sensors is available at:
 * https://www.infineon.com/cms/de/product/sensor/radar-sensors/radar-sensors-for-iot/60ghz-radar/
 *
 * \note This library uses delays while waiting for the sensor. If the RTOS_AWARE component is set
 * or CY_RTOS_AWARE is defined, the driver will defer to the RTOS for delays. Because of this, it is
 * not safe to call any functions other than \ref xensiv_bgt60trxx_init until after the RTOS
 * scheduler has started.
 */

#include <stdint.h>
#include <stdbool.h>

#include "xensiv_bgt60trxx_regs.h"
#include "xensiv_bgt60trxx_platform.h"

/************************************** Macros *******************************************/

/** Result code indicating successful operation. */
#define XENSIV_BGT60TRXX_STATUS_OK                    (0)
/** Result code indicating a communication error. */
#define XENSIV_BGT60TRXX_STATUS_SPI_ERROR             (1)
/** Result code indicating an unsupported device error. */
#define XENSIV_BGT60TRXX_STATUS_CHIPID_ERROR          (2)
/** Result code indicating a timeout waiting for reset completion. */
#define XENSIV_BGT60TRXX_STATUS_TIMEOUT_ERROR         (3)

/** Initial value of the LFSR test sequence generator. */
#define XENSIV_BGT60TRXX_INITIAL_TEST_WORD            (0x0001U)

/** Number of samples stored in a FIFO word (24 bits). */
#define XENSIV_BGT60TRXX_NUM_SAMPLES_FIFO_WORD        (2U)

/** Size of the radar device FIFO word in bytes. */
#define XENSIV_BGT60TRXX_FIFO_WORD_SIZE_BYTES         (3U)   /* two ADC samples of 12bits */

/** Size of the header in the SPI burst transfer. */
#define XENSIV_BGT60TRXX_SPI_BURST_HEADER_SIZE_BYTES  (4U)

/** Timeout for wait on software reset done. */
#ifndef XENSIV_BGT60TRXX_RESET_WAIT_TIMEOUT
#define XENSIV_BGT60TRXX_RESET_WAIT_TIMEOUT           (0xFFFFFFFFU)
#endif

/** FIFO size. */
#define XENSIV_BGT60TRXX_FIFO_SIZE_WORDS              (8192U)

/********************************* Type definitions **************************************/

/** enum defining the different reset commands */
typedef enum
{
    /*! Software reset. */
    XENSIV_BGT60TRXX_RESET_SW = 0x1 << XENSIV_BGT60TRXX_REG_MAIN_RESET_POS,
    /*! FSM reset. */
    XENSIV_BGT60TRXX_RESET_FSM = 0x2 << XENSIV_BGT60TRXX_REG_MAIN_RESET_POS,
    /*! FIFO reset. */
    XENSIV_BGT60TRXX_RESET_FIFO = 0x4 << XENSIV_BGT60TRXX_REG_MAIN_RESET_POS
} xensiv_bgt60trxx_reset_t;

/** Handle to the XENSIV(TM) BGT60TRxx sensor device object.
 * Used to communicate with the sensor over SPI. Typically, a pointer to the platform-specific
 * SPI object. See the example usage in xensiv_bgt60trxx_mtb.c.
 */
typedef void* xensiv_bgt60trxx_t;

/******************************* Function prototypes *************************************/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes the XENSIV(TM) BGT60TRxx radar sensor device.
 * It checks the integrity of the communication layer of the serial communication interface.
 * It applies the sensor configurator given in the regs array that can be generated using the
 * BGT60TRxx configurator tool.
 *
 * @param[in] dev Handle to the XENSIV(TM) BGT60TRxx sensor device object.
 * @param[in] regs Pointer to the configuration registers list.
 * @param[in] len Length of the configuration registers list.
 * @return XENSIV_BGT60TRXX_STATUS_OK if the initialization was successful; else an error indicating
 * what went wrong.
 */
int32_t xensiv_bgt60trxx_init(xensiv_bgt60trxx_t dev,
                              const uint32_t* regs,
                              uint32_t len);

/**
 * @brief Writes the given data buffer into the sensor device.
 * Writes the given data buffer to the sensor register map starting at the register address.
 *
 * @param[in] dev Handle to the XENSIV(TM) BGT60TRxx sensor device object.
 * @param[in] reg_addr Register address.
 * @param[in] data Data to be written in the sensor register.
 * @return XENSIV_BGT60TRXX_STATUS_OK if writing to the sensor register wash successful; else an
 * error indicating what went wrong.
 */
int32_t xensiv_bgt60trxx_set_reg(xensiv_bgt60trxx_t dev, uint32_t reg_addr, uint32_t data);

/**
 * @brief Reads from the sensor device into the given data buffer.
 * Reads from the sensor register map sensor starting at register address into the given data
 * buffer.
 *
 * @param[in] dev Handle to the XENSIV(TM) BGT60TRxx sensor device object.
 * @param[in] reg_addr Register address.
 * @param[out] data Data to be written in the sensor register.
 * @return XENSIV_BGT60TRXX_STATUS_OK if reading from the sensor register was successful; else an
 * error indicating what went wrong.
 */
int32_t xensiv_bgt60trxx_get_reg(xensiv_bgt60trxx_t dev, uint32_t reg_addr, uint32_t* data);

/**
 * @brief Obtains the sensor device FIFO status.
 *
 * @param[in] dev Handle to the XENSIV(TM) BGT60TRxx sensor device object.
 * @param[out] status Pointer to populate with FIFO status.
 * @return XENSIV_BGT60TRXX_STATUS_OK if reading the FIFO status was successful; else
 * an error indicating what went wrong.
 */
int32_t xensiv_bgt60trxx_get_fifo_status(xensiv_bgt60trxx_t dev, uint32_t* status);

/**
 * @brief Sets the FIFO compare reference value.
 * The beat signal signal is sampled, digitized, and stored into the sensor FIFO.
 * When the filling level is greater than the FIFO compare reference (CREF), an interrupt is issued,
 * indicating to the controlling processor the availability of at least CREF samples in
 * the FIFO.
 *
 * @param[inout] dev Handle to the XENSIV(TM) BGT60TRxx sensor device object.
 * @param[in] limit FIFO compare reference. @note Each FIFO word contains two samples,
 * each of 12 bits.
 * @return XENSIV_BGT60TRXX_STATUS_OK if setting the new FIFO limit was successful; else
 * an error indicating what went wrong.
 */
int32_t xensiv_bgt60trxx_set_fifo_limit(xensiv_bgt60trxx_t dev, uint16_t limit);

/**
 * @brief Reads from the sensor device FIFO into the given data buffer.
 * The beat signal is sampled, digitized, and stored into the sensor FIFO.
 * This function reads out the sensor FIFO contents and places it in the given buffer.
 * The SPI burst read consists of a 4-byte header + (len * XENSIV_BGT60TRXX_FIFO_WORD_SIZE_BYTES).
 * The 4-byte header consists of the status register GSR0 (four 1B bits + four status bits) followed
 * by 24 padding bits set to 0B.
 *
 * @param[in] dev Handle to the XENSIV(TM) BGT60TRxx sensor device object.
 * @param[out] data Pointer to a data buffer of size 4 + (len *
 * XENSIV_BGT60TRXX_FIFO_WORD_SIZE_BYTES) to store the FIFO values from the sensor.
 * @param[in] len Number of FIFO words to read from the sensor.
 * @note Each FIFO word contains two samples, each of 12 bits.
 * @return XENSIV_BGT60TRXX_STATUS_OK if reading from the FIFO was successful; else
 * an error indicating what went wrong.
 */
int32_t xensiv_bgt60trxx_get_fifo_data(xensiv_bgt60trxx_t dev, uint8_t* data, uint32_t len);

/**
 * @brief Starts/stops radar frame generation.
 *
 * @param[in] dev Handle to the XENSIV(TM) BGT60TRxx sensor device object.
 * @param[in] start Starts/stops control of frames.
 * @return XENSIV_BGT60TRXX_STATUS_OK if the starting the frame generation was successful,
 * else an error indicating what went wrong.
 */
int32_t xensiv_bgt60trxx_start_frame(xensiv_bgt60trxx_t dev, bool start);

/**
 * @brief Triggers a soft reset of the sensor device.
 * Triggers reset and waits for reset done.
 * @param[in] dev Handle to the XENSIV(TM) BGT60TRxx sensor device object.
 * @param[in] reset_type Reset type.
 * @return XENSIV_BGT60TRXX_STATUS_OK if the soft reset was successful,
 * XENSIV_BGT60TRXX_STATUS_TIMEOUT_ERROR if a timeout occurs while waiting reset to finish;
 * else an error indicating what went wrong.
 */
int32_t xensiv_bgt60trxx_soft_reset(xensiv_bgt60trxx_t dev,
                                    xensiv_bgt60trxx_reset_t reset_type);

/**
 * @brief Enables/disables generation of a test sequence out of FIFO.
 * Enables/disables the output of test sequence data instead of the ADC data for the first ADC
 * channel. This can be used to check data from the radar sensor FIFO against a defined bit
 * sequence.
 * @note ADC Channel 1 need to be enabled in the BGT60TRxx configuration.
 *
 * @param[in] dev Handle to the XENSIV(TM) BGT60TRxx sensor device object.
 * @param[in] enable Enable/disable the output of the test sequence data.
 * @return XENSIV_BGT60TRXX_STATUS_OK if enabling the data testomode was successful; else
 * an error indicating what went wrong.
 */
int32_t xensiv_bgt60trxx_enable_data_test_mode(xensiv_bgt60trxx_t dev, bool enable);

/**
 * @brief Utility function that generates test sequence data that can be used to compare against
 * the FIFO data obtained from device sensor when data test mode is enabled.
 * Implements the LFSR sequence generator based on the polynomial x^12+x^11+x^10+x^4+1.
 * Argument variable cur_test_word keeps the current state of the LFSR generator and should be
 * initialized using XENSIV_BGT60TRXX_INITIAL_TEST_WORD.
 *
 * @code
 * uint16_t test_word = XENSIV_BGT60TRXX_INITIAL_TEST_WORD;
 * test_word = xensiv_bgt60trxx_get_next_test_word(test_word);
 * @endcode
 *
 * @param[in] cur_test_word Keeps the current state of the LFSR generator.
 * @return Next word in the LFSR sequence.
 */
uint16_t xensiv_bgt60trxx_get_next_test_word(uint16_t cur_test_word);

#ifdef __cplusplus
}
#endif

/** \} group_board_libs */

#endif // ifndef XENSIV_BGT60TRXX_H_
