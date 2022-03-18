/***********************************************************************************************//**
 * \file xensiv_bgt60trxx_mtb.c
 *
 * Description: This file contains the MTB platform functions implementation
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

#if defined(CY_USING_HAL)

#include "cyhal_system.h"

#include "xensiv_bgt60trxx_mtb.h"

#define XENSIV_BGT60TRXX_ERROR(x)           (((x) == XENSIV_BGT60TRXX_STATUS_OK) ? CY_RSLT_SUCCESS :\
                                             CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_BOARD_HARDWARE_XENSIV_BGT60TRXX, (x)))

static cy_rslt_t xensiv_bgt60trxx_mtb_hard_reset(const xensiv_bgt60trxx_mtb_t* obj);

cy_rslt_t xensiv_bgt60trxx_mtb_init(xensiv_bgt60trxx_mtb_t* obj,
                                    cyhal_spi_t* spi,
                                    cyhal_gpio_t selpin,
                                    cyhal_gpio_t rstpin,
                                    const uint32_t* regs,
                                    size_t len)
{
    CY_ASSERT(obj != NULL);
    CY_ASSERT(spi != NULL);
    CY_ASSERT(selpin != NC);
    CY_ASSERT(rstpin != NC);

    cy_rslt_t rslt;

    obj->selpin = NC;
    obj->rstpin = NC;

    rslt = cyhal_gpio_init(selpin,
                           CYHAL_GPIO_DIR_OUTPUT,
                           CYHAL_GPIO_DRIVE_STRONG,
                           true);

    if (CY_RSLT_SUCCESS == rslt)
    {
        obj->selpin = selpin;
        obj->rstpin = NC;
        rslt = cyhal_gpio_init(rstpin,
                               CYHAL_GPIO_DIR_OUTPUT,
                               CYHAL_GPIO_DRIVE_STRONG,
                               true);
    }

    if (CY_RSLT_SUCCESS == rslt)
    {
        obj->rstpin = rstpin;
        rslt = xensiv_bgt60trxx_mtb_hard_reset(obj);
    }

    if (CY_RSLT_SUCCESS == rslt)
    {
        obj->spi = spi;
        int32_t res = xensiv_bgt60trxx_init((xensiv_bgt60trxx_t)obj, regs, len);
        if (res != XENSIV_BGT60TRXX_STATUS_OK)
        {
            obj->spi = NULL;
        }
        rslt = XENSIV_BGT60TRXX_ERROR(res);
    }

    return rslt;
}


cy_rslt_t xensiv_bgt60trxx_mtb_interrupt_init(xensiv_bgt60trxx_mtb_t* obj,
                                              uint16_t fifo_limit,
                                              cyhal_gpio_t intpin,
                                              uint8_t intr_priority,
                                              xensiv_bgt60trxx_mtb_interrupt_cb_t* interrupt_cb)
{
    CY_ASSERT(obj != NULL);
    CY_ASSERT(obj->spi != NULL);
    CY_ASSERT(interrupt_cb != NULL);

    obj->intpin = intpin;

    int32_t res = xensiv_bgt60trxx_set_fifo_limit((xensiv_bgt60trxx_t)obj, fifo_limit);
    cy_rslt_t rslt = XENSIV_BGT60TRXX_ERROR(res);

    if (CY_RSLT_SUCCESS == rslt)
    {
        rslt = cyhal_gpio_init(intpin,
                               CYHAL_GPIO_DIR_INPUT,
                               CYHAL_GPIO_DRIVE_PULLDOWN,
                               false);

        if (rslt == CY_RSLT_SUCCESS)
        {
            #if defined(CYHAL_API_VERSION) && (CYHAL_API_VERSION >= 2)
            cyhal_gpio_register_callback(intpin, (cyhal_gpio_callback_data_t*)interrupt_cb);
            #else
            cyhal_gpio_register_callback(intpin, interrupt_cb->callback,
                                         interrupt_cb->callback_arg);
            #endif
            cyhal_gpio_enable_event(intpin,
                                    CYHAL_GPIO_IRQ_RISE,
                                    intr_priority,
                                    true);
        }
    }

    return rslt;
}


void xensiv_bgt60trxx_mtb_free(const xensiv_bgt60trxx_mtb_t* obj)
{
    CY_ASSERT(obj != NULL);

    if (obj->selpin != NC)
    {
        cyhal_gpio_free(obj->selpin);
    }

    if (obj->rstpin != NC)
    {
        cyhal_gpio_free(obj->rstpin);
    }

    if (obj->intpin != NC)
    {
        cyhal_gpio_free(obj->intpin);
    }
}


static cy_rslt_t xensiv_bgt60trxx_mtb_hard_reset(const xensiv_bgt60trxx_mtb_t* obj)
{
    CY_ASSERT(obj != NULL);
    CY_ASSERT(obj->rstpin != NC);
    CY_ASSERT(obj->selpin != NC);

    cyhal_gpio_t rstpin = obj->rstpin;
    cyhal_gpio_t selpin = obj->selpin;

    cyhal_gpio_write(selpin, 1);
    cyhal_gpio_write(rstpin, 1);
    cy_rslt_t rslt = cyhal_system_delay_ms(1);

    cyhal_gpio_write(rstpin, 0);
    rslt |= cyhal_system_delay_ms(1);

    cyhal_gpio_write(rstpin, 1);
    rslt |= cyhal_system_delay_ms(1);

    return rslt;
}


/********************** driver platform specific implementation ****************************/

#if !defined(XENSIV_BGT60TRXX_MTB_USE_DMA)
int32_t xensiv_bgt60trxx_platform_spi_transfer(void* dev,
                                               const uint8_t* tx_data,
                                               uint32_t tx_len,
                                               uint8_t* rx_data,
                                               uint32_t rx_len)
{
    CY_ASSERT(dev != NULL);
    CY_ASSERT((tx_data != NULL) || (rx_data != NULL));

    xensiv_bgt60trxx_mtb_t* obj = (xensiv_bgt60trxx_mtb_t*)dev;

    cyhal_gpio_write(obj->selpin, 0);
    cy_rslt_t result = cyhal_spi_transfer(obj->spi,
                                          tx_data,
                                          (tx_data != NULL) ? tx_len : 0U,
                                          rx_data,
                                          (rx_data != NULL) ? rx_len : 0U,
                                          0xFFU);
    cyhal_gpio_write(obj->selpin, 1);

    return ((CY_RSLT_SUCCESS == result) ?
            XENSIV_BGT60TRXX_STATUS_OK :
            XENSIV_BGT60TRXX_STATUS_SPI_ERROR);
}


#endif // if !defined(XENSIV_BGT60TRXX_MTB_USE_DMA)

void xensiv_bgt60trxx_platform_delay(uint32_t ms)
{
    (void)cyhal_system_delay_ms(ms);
}


uint32_t xensiv_bgt60trxx_platform_word_reverse(uint32_t x)
{
    return __REV(x);
}


void xensiv_bgt60trxx_platform_assert(bool expr)
{
    CY_ASSERT(expr);
    (void)expr; /* make release build */
}


#endif // defined(CY_USING_HAL)
