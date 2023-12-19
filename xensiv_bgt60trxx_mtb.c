/***********************************************************************************************//**
 * \file xensiv_bgt60trxx_mtb.c
 *
 * \brief
 * This file contains the MTB platform functions implementation
 * for interacting with the XENSIV(TM) BGT60TRxx 60GHz FMCW radar sensors.
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

#if defined(CY_USING_HAL)

#include "cyhal_system.h"
#include "xensiv_bgt60trxx_mtb.h"
#include "xensiv_bgt60trxx_platform.h"

/*******************************************************************************
* Macros
*******************************************************************************/
#define XENSIV_BGT60TRXX_ERROR(x)           (((x) == XENSIV_BGT60TRXX_STATUS_OK) ? CY_RSLT_SUCCESS :\
                                             CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_BOARD_HARDWARE_XENSIV_BGT60TRXX, (x)))


/*******************************************************************************
 * Function Prototypes
 ********************************************************************************/
static inline bool pins_equal(xensiv_bgt60trxx_mtb_interrupt_pin_t ref_pin, cyhal_gpio_t pin);

static inline void set_pin(xensiv_bgt60trxx_mtb_interrupt_pin_t* ref_pin, cyhal_gpio_t pin);

static inline void free_pin(xensiv_bgt60trxx_mtb_interrupt_pin_t ref_pin);

static cy_rslt_t config_int(xensiv_bgt60trxx_mtb_interrupt_pin_t* intpin,
                            cyhal_gpio_t pin,
                            bool init,
                            uint8_t intr_priority,
                            cyhal_gpio_event_callback_t callback,
                            void* callback_arg);

/*******************************************************************************
 * Public interface implementation
 ********************************************************************************/
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
    CY_ASSERT(regs != NULL);

    xensiv_bgt60trxx_mtb_iface_t* iface = &obj->iface;

    iface->spi = spi;
    iface->selpin = selpin;
    iface->rstpin = rstpin;
    set_pin(&(iface->irqpin), NC);

    cy_rslt_t rslt = cyhal_gpio_init(selpin,
                                     CYHAL_GPIO_DIR_OUTPUT,
                                     CYHAL_GPIO_DRIVE_STRONG,
                                     true);

    if (CY_RSLT_SUCCESS == rslt)
    {
        rslt = cyhal_gpio_init(rstpin,
                               CYHAL_GPIO_DIR_OUTPUT,
                               CYHAL_GPIO_DRIVE_STRONG,
                               true);
    }


    xensiv_bgt60trxx_t* dev = &obj->dev;

    /* perform device hard reset before beginning init via SPI */
    xensiv_bgt60trxx_platform_rst_set(iface, true);
    xensiv_bgt60trxx_platform_spi_cs_set(iface, true);
    xensiv_bgt60trxx_platform_delay(1U);
    xensiv_bgt60trxx_platform_rst_set(iface, false);
    xensiv_bgt60trxx_platform_delay(1U);
    xensiv_bgt60trxx_platform_rst_set(iface, true);
    xensiv_bgt60trxx_platform_delay(1U);

    if (CY_RSLT_SUCCESS == rslt)
    {
        int32_t res = xensiv_bgt60trxx_init(dev, iface, false);
        rslt = XENSIV_BGT60TRXX_ERROR(res);
    }

    if (CY_RSLT_SUCCESS == rslt)
    {
        int32_t res = xensiv_bgt60trxx_config(dev, regs, len);
        rslt = XENSIV_BGT60TRXX_ERROR(res);
    }

    return rslt;
}


cy_rslt_t xensiv_bgt60trxx_mtb_interrupt_init(xensiv_bgt60trxx_mtb_t* obj,
                                              uint16_t fifo_limit,
                                              cyhal_gpio_t irqpin,
                                              uint8_t intr_priority,
                                              cyhal_gpio_event_callback_t callback,
                                              void* callback_arg)
{
    CY_ASSERT(obj != NULL);

    xensiv_bgt60trxx_t* dev = &obj->dev;
    xensiv_bgt60trxx_mtb_iface_t* iface = &obj->iface;

    cy_rslt_t result;

    if (pins_equal(iface->irqpin, irqpin))
    {
        result = config_int(&(iface->irqpin), irqpin, false, intr_priority, callback, callback_arg);
    }
    else if (pins_equal(iface->irqpin, NC))
    {
        result = config_int(&(iface->irqpin), irqpin, true, intr_priority, callback, callback_arg);
    }
    else
    {
        result = XENSIV_BGT60TRXX_RSLT_ERR_INTPIN_INUSE;
    }

    if (result == CY_RSLT_SUCCESS)
    {
        int32_t res = xensiv_bgt60trxx_set_fifo_limit(dev, fifo_limit);
        result = XENSIV_BGT60TRXX_ERROR(res);
    }

    return result;
}


void xensiv_bgt60trxx_mtb_free(xensiv_bgt60trxx_mtb_t* obj)
{
    CY_ASSERT(obj != NULL);

    xensiv_bgt60trxx_mtb_iface_t* iface = &obj->iface;

    if (iface->selpin != NC)
    {
        cyhal_gpio_free(iface->selpin);
    }

    if (iface->rstpin != NC)
    {
        cyhal_gpio_free(iface->rstpin);
    }

    if (!pins_equal(iface->irqpin, NC))
    {
        free_pin(iface->irqpin);
    }
}


/*******************************************************************************
 * Platform functions implementation
 ********************************************************************************/
__STATIC_INLINE void spi_set_data_width(CySCB_Type* base, uint32_t data_width)
{
    CY_ASSERT(CY_SCB_SPI_IS_DATA_WIDTH_VALID(data_width));

    CY_REG32_CLR_SET(SCB_TX_CTRL(base),
                     SCB_TX_CTRL_DATA_WIDTH,
                     (uint32_t)data_width - 1U);
    CY_REG32_CLR_SET(SCB_RX_CTRL(base),
                     SCB_RX_CTRL_DATA_WIDTH,
                     (uint32_t)data_width - 1U);
}


int32_t xensiv_bgt60trxx_platform_spi_transfer(void* iface,
                                               uint8_t* tx_data,
                                               uint8_t* rx_data,
                                               uint32_t len)
{
    CY_ASSERT(iface != NULL);
    CY_ASSERT((tx_data != NULL) || (rx_data != NULL));

    const xensiv_bgt60trxx_mtb_iface_t* mtb_iface = iface;

    spi_set_data_width(mtb_iface->spi->base, 8U);
    Cy_SCB_SetByteMode(mtb_iface->spi->base, true);
    cy_en_scb_spi_status_t status = Cy_SCB_SPI_Transfer(mtb_iface->spi->base, tx_data, rx_data, len,
                                                        &(mtb_iface->spi->context));
    if (CY_SCB_SPI_SUCCESS == status)
    {
        while (0UL !=
               (CY_SCB_SPI_TRANSFER_ACTIVE &
                Cy_SCB_SPI_GetTransferStatus(mtb_iface->spi->base, &(mtb_iface->spi->context))))
        {
        }
    }

    return ((CY_SCB_SPI_SUCCESS == status) ?
            XENSIV_BGT60TRXX_STATUS_OK :
            XENSIV_BGT60TRXX_STATUS_COM_ERROR);
}


int32_t xensiv_bgt60trxx_platform_spi_fifo_read(void* iface,
                                                uint16_t* rx_data,
                                                uint32_t len)
{
    CY_ASSERT(iface != NULL);
    CY_ASSERT(rx_data != NULL);

    const xensiv_bgt60trxx_mtb_iface_t* mtb_iface = iface;

    spi_set_data_width(mtb_iface->spi->base, 12U);
    Cy_SCB_SetByteMode(mtb_iface->spi->base, false);
    cy_en_scb_spi_status_t status = Cy_SCB_SPI_Transfer(mtb_iface->spi->base, NULL, rx_data, len,
                                                        &(mtb_iface->spi->context));
    if (CY_SCB_SPI_SUCCESS == status)
    {
        while (0UL !=
               (CY_SCB_SPI_TRANSFER_ACTIVE &
                Cy_SCB_SPI_GetTransferStatus(mtb_iface->spi->base, &(mtb_iface->spi->context))))
        {
        }
    }

    return ((CY_SCB_SPI_SUCCESS == status) ?
            XENSIV_BGT60TRXX_STATUS_OK :
            XENSIV_BGT60TRXX_STATUS_COM_ERROR);
}


void xensiv_bgt60trxx_platform_rst_set(const void* iface, bool val)
{
    CY_ASSERT(iface != NULL);

    const xensiv_bgt60trxx_mtb_iface_t* mtb_iface = iface;

    CY_ASSERT(mtb_iface->rstpin != NC);

    cyhal_gpio_write(mtb_iface->rstpin, val);
}


void xensiv_bgt60trxx_platform_spi_cs_set(const void* iface, bool val)
{
    CY_ASSERT(iface != NULL);

    const xensiv_bgt60trxx_mtb_iface_t* mtb_iface = iface;

    CY_ASSERT(mtb_iface->selpin != NC);

    cyhal_gpio_write(mtb_iface->selpin, val);
}


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


/*******************************************************************************
 * Static functions implementation
 ********************************************************************************/
static inline bool pins_equal(xensiv_bgt60trxx_mtb_interrupt_pin_t ref_pin, cyhal_gpio_t pin)
{
    #if defined(CYHAL_API_VERSION) && (CYHAL_API_VERSION >= 2)
    return (ref_pin.pin == pin);
    #else
    return (ref_pin == pin);
    #endif
}


static inline void set_pin(xensiv_bgt60trxx_mtb_interrupt_pin_t* ref_pin, cyhal_gpio_t pin)
{
    #if defined(CYHAL_API_VERSION) && (CYHAL_API_VERSION >= 2)
    ref_pin->pin = pin;
    #else
    *ref_pin = pin;
    #endif
}


static inline void free_pin(xensiv_bgt60trxx_mtb_interrupt_pin_t ref_pin)
{
    #if defined(CYHAL_API_VERSION) && (CYHAL_API_VERSION >= 2)
    cyhal_gpio_free(ref_pin.pin);
    #else
    cyhal_gpio_free(ref_pin);
    #endif
}


static cy_rslt_t config_int(xensiv_bgt60trxx_mtb_interrupt_pin_t* intpin,
                            cyhal_gpio_t pin,
                            bool init,
                            uint8_t intr_priority,
                            cyhal_gpio_event_callback_t callback,
                            void* callback_arg)
{
    cy_rslt_t result = CY_RSLT_SUCCESS;

    if (NULL == callback)
    {
        set_pin(intpin, NC);
        cyhal_gpio_free(pin);
    }
    else
    {
        if (init)
        {
            result = cyhal_gpio_init(pin,
                                     CYHAL_GPIO_DIR_INPUT,
                                     CYHAL_GPIO_DRIVE_PULLDOWN,
                                     false);
        }

        if (CY_RSLT_SUCCESS == result)
        {
            set_pin(intpin, pin);
            #if defined(CYHAL_API_VERSION) && (CYHAL_API_VERSION >= 2)
            intpin->callback = callback;
            intpin->callback_arg = callback_arg;
            cyhal_gpio_register_callback(pin, intpin);
            #else
            cyhal_gpio_register_callback(pin, callback, callback_arg);
            #endif
            cyhal_gpio_enable_event(pin, CYHAL_GPIO_IRQ_RISE, intr_priority, true);
        }
    }

    return result;
}


#endif // defined(CY_USING_HAL)
