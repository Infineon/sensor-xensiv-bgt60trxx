/***********************************************************************************************//**
 * \file xensiv_bgt60trxx.h
 *
 * \brief
 * This file contains the functions for interacting with the
 * XENSIV(TM) BGT60TRxx 60GHz FMCW radar sensors.
 *
 ***************************************************************************************************
 * \copyright
 * Copyright 2022 Infineon Technologies AG
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
 */

#include <stdint.h>
#include <stdbool.h>

#include "xensiv_bgt60trxx_regs.h"

/************************************** Macros *******************************************/

/** Result code indicating successful operation. */
#define XENSIV_BGT60TRXX_STATUS_OK                      (0)
/** Result code indicating a communication error. */
#define XENSIV_BGT60TRXX_STATUS_COM_ERROR               (1)
/** Result code indicating an unsupported device error. */
#define XENSIV_BGT60TRXX_STATUS_DEV_ERROR               (2)
/** Result code indicating an operation timeout error. */
#define XENSIV_BGT60TRXX_STATUS_TIMEOUT_ERROR           (3)
/** Result code indicating that an error occurred while reading from FIFO. */
#define XENSIV_BGT60TRXX_STATUS_GSR0_ERROR              (4)

/** Initial value of the LFSR test sequence generator. */
#define XENSIV_BGT60TRXX_INITIAL_TEST_WORD              (0x0001U)

/** Number of samples stored in a FIFO word (24 bits). */
#define XENSIV_BGT60TRXX_NUM_SAMPLES_FIFO_WORD          (2U)

/** Size of the radar device FIFO word in bytes. */
#define XENSIV_BGT60TRXX_FIFO_WORD_SIZE_BYTES           (3U)   /* two ADC samples of 12bits */

/** Size of the header in the SPI burst transfer. */
#define XENSIV_BGT60TRXX_SPI_BURST_HEADER_SIZE_BYTES    (4U)

/** Timeout for wait on software reset done. */
#ifndef XENSIV_BGT60TRXX_RESET_WAIT_TIMEOUT
#define XENSIV_BGT60TRXX_RESET_WAIT_TIMEOUT             (0xFFFFFFFFU)
#endif

/********************************* Type definitions **************************************/

/** enum defining the different reset commands passed to \ref xensiv_bgt60trxx_soft_reset() */
typedef enum
{
    /*! Software reset.
        Resets all registers to default state.
        Resets all internal counters (e.g. shape, frame).
        Perform FIFO reset. Perform FSM reset */
    XENSIV_BGT60TRXX_RESET_SW = 0x1 << XENSIV_BGT60TRXX_REG_MAIN_RESET_POS,
    /*! FSM reset.
        Resets FSM to deep sleep mode.
        Resets FSM internal counters for channel/shape set and timers */
    XENSIV_BGT60TRXX_RESET_FSM = 0x2 << XENSIV_BGT60TRXX_REG_MAIN_RESET_POS,
    /*! FIFO reset.
        Reset the read and write pointers of the FIFO.
        Perform an implicit FSM reset */
    XENSIV_BGT60TRXX_RESET_FIFO = 0x4 << XENSIV_BGT60TRXX_REG_MAIN_RESET_POS
} xensiv_bgt60trxx_reset_t;

/** enum with supported devices */
typedef enum
{
    XENSIV_DEVICE_BGT60TR13C = 0,   /**< BGT60TR13C */
    XENSIV_DEVICE_BGT60UTR13D = 1,  /**< BGT60UTR13D */
    XENSIV_DEVICE_BGT60UTR11 = 2,   /**< BGT60UTR11 */
    XENSIV_DEVICE_UNKNOWN = -1      /**< Unknown not supported device */
} xensiv_bgt60trxx_device_t;

/** \cond INTERNAL */
/* Forward declaration of structure holding device specific type info */
struct xensiv_bgt60trxx_type;
/** \endcond */

/** XENSIV(TM) BGT60TRxx sensor device object.
 *
 * Application code should not rely on the specific content of this struct.
 * They are considered an implementation detail which is subject to change
 * between platforms and/or library releases.
 */
typedef struct
{
    void* iface; /**< Pointer to platform-specific SPI interface object provided to the
                      xensiv_bgt60trxx_platform_spi_transfer function */
    const struct xensiv_bgt60trxx_type* type; /**< Device type detected during initialization */
    bool high_speed; /**< SPI speed mode */
} xensiv_bgt60trxx_t;

/******************************* Function prototypes *************************************/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes the XENSIV(TM) BGT60TRxx radar sensor device.
 * It checks the integrity of the serial communication interface.
 * Detects and identify the connected radar sensor.
 *
 * @param[in] dev Pointer to the XENSIV(TM) BGT60TRxx sensor device object. The caller must allocate
 * the memory for this object but the init function will initialize its contents.
 * @param[in] iface Pointer to the interface object used to communicate with the sensor over SPI.
 * Typically, a pointer to the platform-specific SPI interface object.
 * @param[in] high_speed The XENSIV(TM) BGT60TRxx sensor offers an additional high speed mode which
 * increase the timing budget on SPI master side by sending out data via DO with the rising edge
 * instead of the falling edge of the CLK.
 * @return XENSIV_BGT60TRXX_STATUS_OK if the initialization was successful; else an error indicating
 * what went wrong.
 */
int32_t xensiv_bgt60trxx_init(xensiv_bgt60trxx_t* dev,
                              void* iface,
                              bool high_speed);

/**
 * @brief Obtains the detected sensor device name.
 *
 * @param[in] dev Pointer to the XENSIV(TM) BGT60TRxx sensor device object.
 * @return sensor device name.
 */
xensiv_bgt60trxx_device_t xensiv_bgt60trxx_get_device(const xensiv_bgt60trxx_t* dev);

/**
 * @brief Configures the XENSIV(TM) BGT60TRxx radar sensor device.
 * It performs a SW reset and applies the sensor configurator given in the regs array
 * The register configuration can be generated using the BGT60TRxx configurator tool.
 *
 * @param[in] dev Pointer to the XENSIV(TM) BGT60TRxx sensor device object.
 * @param[in] regs Pointer to the configuration registers list.
 * @param[in] len Length of the configuration registers list.
 * @return XENSIV_BGT60TRXX_STATUS_OK if the initialization was successful; else an error indicating
 * what went wrong.
 */
int32_t xensiv_bgt60trxx_config(xensiv_bgt60trxx_t* dev,
                                const uint32_t* regs,
                                uint32_t len);

/**
 * @brief Writes the given data buffer into the sensor device.
 * Writes the given data buffer to the sensor register map starting at the register address.
 *
 * @param[in] dev Pointer to the XENSIV(TM) BGT60TRxx sensor device object.
 * @param[in] reg_addr Register address.
 * @param[in] data Data to be written in the sensor register.
 * @return XENSIV_BGT60TRXX_STATUS_OK if writing to the sensor register wash successful; else an
 * error indicating what went wrong.
 */
int32_t xensiv_bgt60trxx_set_reg(const xensiv_bgt60trxx_t* dev,
                                 uint32_t reg_addr,
                                 uint32_t data);

/**
 * @brief Reads from the sensor device into the given data buffer.
 * Reads from the sensor register map sensor starting at register address into the given data
 * buffer.
 *
 * @param[in] dev Pointer to the XENSIV(TM) BGT60TRxx sensor device object.
 * @param[in] reg_addr Register address.
 * @param[out] data Data to be written in the sensor register.
 * @return XENSIV_BGT60TRXX_STATUS_OK if reading from the sensor register was successful; else an
 * error indicating what went wrong.
 */
int32_t xensiv_bgt60trxx_get_reg(const xensiv_bgt60trxx_t* dev,
                                 uint32_t reg_addr,
                                 uint32_t* data);

/**
 * @brief Obtains the sensor device FIFO size.
 *
 * @param[in] dev Pointer to the XENSIV(TM) BGT60TRxx sensor device object.
 * @return FIFO size.
 */
uint16_t xensiv_bgt60trxx_get_fifo_size(const xensiv_bgt60trxx_t* dev);

/**
 * @brief Obtains the sensor device FIFO status.
 *
 * @param[in] dev Pointer to the XENSIV(TM) BGT60TRxx sensor device object.
 * @param[out] status Pointer to populate with FIFO status. The status value
 * is used to get fifo error information when compared with bit masks from
 * xensiv_bgt60trxx_regs.h i.e. to check for fifo overflow error use
 * XENSIV_BGT60TRXX_REG_FSTAT_FOF_ERR_MSK & status
 * @return XENSIV_BGT60TRXX_STATUS_OK if reading the FIFO status was successful; else
 * an error indicating what went wrong.
 */
int32_t xensiv_bgt60trxx_get_fifo_status(const xensiv_bgt60trxx_t* dev,
                                         uint32_t* status);

/**
 * @brief Sets the FIFO compare reference value.
 * The beat signal signal is sampled, digitized, and stored into the sensor FIFO.
 * When the filling level is greater than the FIFO compare reference (CREF), an interrupt is issued,
 * indicating to the controlling processor the availability of at least CREF samples in
 * the FIFO.
 *
 * @param[inout] dev Pointer to the XENSIV(TM) BGT60TRxx sensor device object.
 * @param[in] num_samples Number of samples stored in FIFO to trigger interrupt.
 * @note Should be an even number
 * @return XENSIV_BGT60TRXX_STATUS_OK if setting the new FIFO limit was successful; else
 * an error indicating what went wrong.
 */
int32_t xensiv_bgt60trxx_set_fifo_limit(const xensiv_bgt60trxx_t* dev,
                                        uint32_t num_samples);

/**
 * @brief Reads from the sensor device FIFO into the given data buffer.
 * The beat signal is sampled, digitized, and stored into the sensor FIFO.
 * This function reads out the sensor FIFO contents and places it in the given buffer.
 *
 * @param[in] dev Pointer to the XENSIV(TM) BGT60TRxx sensor device object.
 * @param[out] data Pointer to a data buffer.
 * @param[in] num_samples Number of samples to read from the sensor.
 * @return XENSIV_BGT60TRXX_STATUS_OK if reading from the FIFO was successful; else
 * an error indicating what went wrong.
 */
int32_t xensiv_bgt60trxx_get_fifo_data(const xensiv_bgt60trxx_t* dev,
                                       uint16_t* data,
                                       uint32_t num_samples);

/**
 * @brief Starts/stops radar frame generation.
 *
 * @param[in] dev Pointer to the XENSIV(TM) BGT60TRxx sensor device object.
 * @param[in] start Starts/stops control of frames.
 * @return XENSIV_BGT60TRXX_STATUS_OK if the starting the frame generation was successful,
 * else an error indicating what went wrong.
 */
int32_t xensiv_bgt60trxx_start_frame(const xensiv_bgt60trxx_t* dev,
                                     bool start);

/**
 * @brief Triggers a soft reset of the sensor device.
 * Triggers reset and waits for reset done.
 * @param[in] dev Pointer to the XENSIV(TM) BGT60TRxx sensor device object.
 * @param[in] reset_type Reset type, combination of .
 * @return XENSIV_BGT60TRXX_STATUS_OK if the soft reset was successful,
 * XENSIV_BGT60TRXX_STATUS_TIMEOUT_ERROR if a timeout occurs while waiting reset to finish;
 * else an error indicating what went wrong.
 */
int32_t xensiv_bgt60trxx_soft_reset(const xensiv_bgt60trxx_t* dev,
                                    xensiv_bgt60trxx_reset_t reset_type);

/**
 * @brief Enables/disables generation of a test sequence out of FIFO.
 * Enables/disables the output of test sequence data instead of the ADC data for the first ADC
 * channel. This can be used to check data from the radar sensor FIFO against a defined bit
 * sequence.
 * @note ADC Channel 1 need to be enabled in the BGT60TRxx configuration.
 *
 * @param[in] dev Pointer to the XENSIV(TM) BGT60TRxx sensor device object.
 * @param[in] enable Enable/disable the output of the test sequence data.
 * @return XENSIV_BGT60TRXX_STATUS_OK if enabling the data testomode was successful; else
 * an error indicating what went wrong.
 */
int32_t xensiv_bgt60trxx_enable_data_test_mode(const xensiv_bgt60trxx_t* dev,
                                               bool enable);

/**
 * @brief Performs a hard reset of the sensor device.
 *
 * @param[in] dev Pointer to the XENSIV(TM) BGT60TRxx sensor device object.
 */
void xensiv_bgt60trxx_hard_reset(const xensiv_bgt60trxx_t* dev);

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
