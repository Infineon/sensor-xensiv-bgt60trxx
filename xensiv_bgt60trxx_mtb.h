/***********************************************************************************************//**
 * \file xensiv_bgt60trxx_mtb.h
 *
 * Description: This file contains the MTB platform functions declarations
 *              for interacting with the XENSIV(TM) BGT60TRxx 60GHz FMCW radar sensors.
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

#ifndef XENSIV_BGT60TRXX_MTB_H_
#define XENSIV_BGT60TRXX_MTB_H_

#include "cy_result.h"

#include "xensiv_bgt60trxx.h"

/**
 * \addtogroup group_board_libs_mtb XENSIV(TM) BGT60TRxx Radar Sensor ModusToolBox(TM) Interface
 * \{
 * Provides the ModusToolbox(TM) interface to the XENSIV(TM) BGT60TRxx 60GHz FMCW radar sensors
 * library and the implementation of the platform functions using the PSoC(TM) 6 HAL.
 *
 * \note The library uses delays while waiting for the sensor. If the RTOS_AWARE component is set
 * or CY_RTOS_AWARE is defined, the driver will defer to the RTOS for delays. Because of this, it is
 * not safe to call any functions until after the RTOS scheduler has started.
 *
 * \section subsection_board_libs_snippets Code snippets
 * \subsection subsection_board_libs_snippet_1 Snippet 1: Initialization.
 * The following snippet initializes an SPI instance and the BGT60TRxx.
 * \snippet snippet/main.c snippet_xensiv_bgt60trxx_init
 *
 * \subsection subsection_board_libs_snippet_2 Snippet 2: Reading from the sensor.
 * The following snippet demonstrates how to read data from the sensor.
 * \snippet snippet/main.c snippet_xensiv_bgt60trxx_get_fifo_data
 * \image html example-terminal.png
 *
 */

#if defined(CY_USING_HAL)
#include "cyhal_gpio.h"
#include "cyhal_spi.h"

/************************************** Macros *******************************************/

#ifndef CY_RSLT_MODULE_BOARD_HARDWARE_XENSIV_BGT60TRXX
/** Module identifier for the XENSIV(TM) BGT60TRxx radar sensor library.
    Asset(s): (sensor-XENSIV(TM)-bgt60trxx) */
#define CY_RSLT_MODULE_BOARD_HARDWARE_XENSIV_BGT60TRXX 0x01CC
#endif

/** Result code indicating a communication error. */
#define XENSIV_BGT60TRXX_RSLT_ERR_COMM\
    (CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_BOARD_HARDWARE_XENSIV_PASCO2, XENSIV_BGT60TRXX_STATUS_SPI_ERROR))

/** Result code indicating an unsupported device error. */
#define XENSIV_BGT60TRXX_RSLT_ERR_CHIPID\
    (CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_BOARD_HARDWARE_XENSIV_PASCO2, XENSIV_BGT60TRXX_STATUS_CHIPID_ERROR))

/******************************** Type definitions ****************************************/

/**
 * Structure holding the XENSIV(TM) BGT60TRxx ModusToolbox(TM) object specific information.
 *
 * Application code should not rely on the specific content of this struct.
 * They are considered an implementation detail which is subject to change
 * between platforms and/or library releases.
 */
typedef struct
{
    cyhal_spi_t* spi;
    cyhal_gpio_t selpin;
    cyhal_gpio_t rstpin;
    cyhal_gpio_t intpin;
} xensiv_bgt60trxx_mtb_t;

/** Structure containing callback data for handling interrupts from sensor.
 * Instances of this object are expected to persist for the length of time the callback is
 * registered. As such, care must be given if declaring it on the stack to ensure the frame does
 * not go away while the callback is still registered.*/
#if defined(CYHAL_API_VERSION) && (CYHAL_API_VERSION >= 2)
typedef cyhal_gpio_callback_data_t xensiv_bgt60trxx_mtb_interrupt_cb_t;
#else
typedef struct
{
    cyhal_gpio_event_callback_t callback; /**< The callback function to run */
    void* callback_arg;                   /**< Optional argument for the callback */
} xensiv_bgt60trxx_mtb_interrupt_cb_t;
#endif

/******************************* Function prototypes *************************************/

#ifdef __cplusplus
extern "C" {
#endif

/** Initializes the XENSIV(TM) BGT60TRxx sensor.
 *
 * @param[inout] obj       Pointer to the BGT60TRxx ModusToolbox(TM) object. The caller must
 * allocate the memory for this object but the init function will initialize its contents.
 * @param[in]    spi       Pointer to an initialized SPI HAL object.
 * @param[in]    selpin    Pin connected to the SEL pin of the sensor.
 * @param[in]    rstpin    Pin connected to the RST pin of the sensor.
 * @param[in]    regs      Pointer to the configuration registers list.
 * @param[in]    len       Length of the configuration registers list.
 * @return CY_RSLT_SUCCESS if properly initialized; else an error indicating what went wrong.
 */
cy_rslt_t xensiv_bgt60trxx_mtb_init(xensiv_bgt60trxx_mtb_t* obj,
                                    cyhal_spi_t* spi,
                                    cyhal_gpio_t selpin,
                                    cyhal_gpio_t rstpin,
                                    const uint32_t* regs,
                                    size_t len);

/** Configures a GPIO pin as an interrupt for the XENSIV(TM) BGT60TRXX.
 * Configures the XENSIV(TM) BGT60TRxx to trigger an interrupt after the number of fifo_limit words
 * are stored in the BGT60TRxx FIFO. Each FIFO word corresponds to two ADC samples.
 * Initializes and configures the pin as an interrupt.
 * @note BGT60TRxx pointer must be initialized using \ref xensiv_bgt60trxx_mtb_init() before
 * calling this function.
 * @param[inout] obj               Pointer to the BGT60TRxx ModusToolbox(TM) object.
 * @param[in]    fifo_limit        Number of words stored in FIFO that will trigger an interrupt.
 * @param[in]    intpin            Pin connected to the IRQ pin of the sensor.
 * @param[in]    intr_priority     The priority for NVIC interrupt events.
 * @param[in]    interrupt_cb      Interrupt callback.
 * @return CY_RSLT_SUCCESS if interrupt was successfully enabled; else an error occurred while
 * initializing the pin.
 */
cy_rslt_t xensiv_bgt60trxx_mtb_interrupt_init(xensiv_bgt60trxx_mtb_t* obj,
                                              uint16_t fifo_limit,
                                              cyhal_gpio_t intpin,
                                              uint8_t intr_priority,
                                              xensiv_bgt60trxx_mtb_interrupt_cb_t* interrupt_cb);



/**
 * Frees up any resources allocated by the XENSIV(TM) BGT60TRxx as part of
 * \ref xensiv_bgt60trxx_mtb_init()
 * and \ref xensiv_bgt60trxx_mtb_interrupt_init().
 * @param[in] obj  Pointer to the BGT60TRxx ModusToolbox(TM) object.
 */
void xensiv_bgt60trxx_mtb_free(const xensiv_bgt60trxx_mtb_t* obj);

#ifdef __cplusplus
}
#endif

#endif // defined(CY_USING_HAL)

/** \} group_board_libs_mtb */

#endif // ifndef XENSIV_BGT60TRXX_MTB_H_
