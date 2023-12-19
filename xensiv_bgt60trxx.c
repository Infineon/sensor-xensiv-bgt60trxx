/***********************************************************************************************//**
 * \file xensiv_bgt60trxx.c
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

#include <stddef.h>

#include "xensiv_bgt60trxx.h"
#include "xensiv_bgt60trxx_platform.h"

#define XENSIV_BGT60TRXX_SPI_REG_XFER_LEN_BYTES         (4U)
#define XENSIV_BGT60TRXX_SOFT_RESET_DELAY_MS            (10U)

#define XENSIV_BGT60TRXX_SPI_WR_OP_MSK                  (0x01000000UL)
#define XENSIV_BGT60TRXX_SPI_WR_OP_POS                  (24U)
#define XENSIV_BGT60TRXX_SPI_GSR0_MSK                   (0x0F000000UL)
#define XENSIV_BGT60TRXX_SPI_GSR0_POS                   (24U)
#define XENSIV_BGT60TRXX_SPI_REGADR_MSK                 (0xFE000000UL)
#define XENSIV_BGT60TRXX_SPI_REGADR_POS                 (25U)
#define XENSIV_BGT60TRXX_SPI_DATA_MSK                   (0x00FFFFFFUL)
#define XENSIV_BGT60TRXX_SPI_DATA_POS                   (0U)
#define XENSIV_BGT60TRXX_SPI_BURST_MODE_CMD             (0xFF000000UL)
#define XENSIV_BGT60TRXX_SPI_BURST_MODE_SADR_MSK        (0x00FE0000UL)
#define XENSIV_BGT60TRXX_SPI_BURST_MODE_SADR_POS        (17U)
#define XENSIV_BGT60TRXX_SPI_BURST_MODE_RWB_MSK         (0x00010000UL)
#define XENSIV_BGT60TRXX_SPI_BURST_MODE_RWB_POS         (16U)
#define XENSIV_BGT60TRXX_SPI_BURST_MODE_LEN_MSK         (0x0000FE00UL)
#define XENSIV_BGT60TRXX_SPI_BURST_MODE_LEN_POS         (9U)


struct xensiv_bgt60trxx_type
{
    uint32_t fifo_addr;
    uint16_t fifo_size;
    xensiv_bgt60trxx_device_t device;
};

static const struct xensiv_bgt60trxx_type bgt60trxx_types[] =
{
    {
        .fifo_addr = XENSIV_BGT60TRXX_REG_FIFO_TR13C,
        .fifo_size = 8192U,
        .device = XENSIV_DEVICE_BGT60TR13C
    },
    {
        .fifo_addr = XENSIV_BGT60TRXX_REG_FIFO_UTR13D,
        .fifo_size = 8192U,
        .device = XENSIV_DEVICE_BGT60UTR13D
    },
    {
        .fifo_addr = XENSIV_BGT60TRXX_REG_FIFO_UTR11,
        .fifo_size = 2048U,
        .device = XENSIV_DEVICE_BGT60UTR11
    }
};

static xensiv_bgt60trxx_device_t detect_device_type(uint32_t chipid)
{
    uint32_t chip_id_digital = (chipid & XENSIV_BGT60TRXX_REG_CHIP_ID_DIGITAL_ID_MSK) >>
                               XENSIV_BGT60TRXX_REG_CHIP_ID_DIGITAL_ID_POS;
    uint32_t chip_id_rf = (chipid & XENSIV_BGT60TRXX_REG_CHIP_ID_RF_ID_MSK) >>
                          XENSIV_BGT60TRXX_REG_CHIP_ID_RF_ID_POS;

    if ((chip_id_digital == 3U) && (chip_id_rf == 3U))
    {
        return XENSIV_DEVICE_BGT60TR13C;
    }
    else if ((chip_id_digital == 6U) &&
             ((chip_id_rf == 6U) || (chip_id_rf == 11U)))
    {
        return XENSIV_DEVICE_BGT60UTR13D;
    }
    else if ((chip_id_digital == 7U) &&
             ((chip_id_rf == 7U) || (chip_id_rf == 9U) || (chip_id_rf == 12U)))
    {
        return XENSIV_DEVICE_BGT60UTR11;
    }
    else if ((chip_id_digital == 8U) && (chip_id_rf == 12U))
    {
        return XENSIV_DEVICE_BGT60UTR11;
    }
    else
    {
        return XENSIV_DEVICE_UNKNOWN;
    }
}


int32_t xensiv_bgt60trxx_init(xensiv_bgt60trxx_t* dev,
                              void* iface,
                              bool high_speed)
{
    xensiv_bgt60trxx_platform_assert(dev != NULL);
    xensiv_bgt60trxx_platform_assert(iface != NULL);

    int32_t status;

    dev->iface = iface;
    dev->high_speed = high_speed;

    //xensiv_bgt60trxx_hard_reset(dev);

    if (high_speed)
    {
        status = xensiv_bgt60trxx_set_reg(dev,
                                          XENSIV_BGT60TRXX_REG_SFCTL,
                                          XENSIV_BGT60TRXX_REG_SFCTL_MISO_HS_READ_MSK);
    }
    else
    {
        status = xensiv_bgt60trxx_set_reg(dev, XENSIV_BGT60TRXX_REG_SFCTL, 0U);
    }

    /* Read chipid */
    uint32_t chipid;
    if (XENSIV_BGT60TRXX_STATUS_OK == status)
    {
        status = xensiv_bgt60trxx_get_reg(dev, XENSIV_BGT60TRXX_REG_CHIP_ID, &chipid);
    }

    if (XENSIV_BGT60TRXX_STATUS_OK == status)
    {
        xensiv_bgt60trxx_device_t device_type = detect_device_type(chipid);
        if (device_type != XENSIV_DEVICE_UNKNOWN)
        {
            dev->type = &bgt60trxx_types[device_type];
        }
        else
        {
            dev->type = NULL;
            status = XENSIV_BGT60TRXX_STATUS_DEV_ERROR;
        }
    }

    return status;
}


int32_t xensiv_bgt60trxx_config(xensiv_bgt60trxx_t* dev,
                                const uint32_t* regs,
                                uint32_t len)
{
    xensiv_bgt60trxx_platform_assert(dev != NULL);
    xensiv_bgt60trxx_platform_assert(regs != NULL);

    int32_t status = xensiv_bgt60trxx_soft_reset(dev,
                                                 XENSIV_BGT60TRXX_RESET_SW);

    if (XENSIV_BGT60TRXX_STATUS_OK == status)
    {
        /* Apply register configuration */
        for (uint32_t reg_idx = 0; reg_idx < len; ++reg_idx)
        {
            uint32_t val = regs[reg_idx];
            uint32_t reg_addr = ((val & XENSIV_BGT60TRXX_SPI_REGADR_MSK) >>
                                 XENSIV_BGT60TRXX_SPI_REGADR_POS);
            uint32_t reg_data = ((val & XENSIV_BGT60TRXX_SPI_DATA_MSK) >>
                                 XENSIV_BGT60TRXX_SPI_DATA_POS);

            if (reg_addr == XENSIV_BGT60TRXX_REG_SFCTL)
            {
                /* FIFO limit set by user */
                reg_data &= (uint32_t) ~XENSIV_BGT60TRXX_REG_SFCTL_FIFO_CREF_MSK;
                if (dev->high_speed)
                {
                    reg_data |= (uint32_t)XENSIV_BGT60TRXX_REG_SFCTL_MISO_HS_READ_MSK;
                }
                else
                {
                    reg_data &= (uint32_t) ~XENSIV_BGT60TRXX_REG_SFCTL_MISO_HS_READ_MSK;
                }
            }

            status = xensiv_bgt60trxx_set_reg(dev, reg_addr, reg_data);

            if (status != XENSIV_BGT60TRXX_STATUS_OK)
            {
                break;
            }
        }
    }

    return status;
}


int32_t xensiv_bgt60trxx_set_reg(const xensiv_bgt60trxx_t* dev, uint32_t reg_addr, uint32_t data)
{
    xensiv_bgt60trxx_platform_assert(dev != NULL);

    uint32_t temp;
    temp = (reg_addr << XENSIV_BGT60TRXX_SPI_REGADR_POS) & XENSIV_BGT60TRXX_SPI_REGADR_MSK;
    temp |= XENSIV_BGT60TRXX_SPI_WR_OP_MSK;
    temp |= (data << XENSIV_BGT60TRXX_SPI_DATA_POS) & XENSIV_BGT60TRXX_SPI_DATA_MSK;

    temp = xensiv_bgt60trxx_platform_word_reverse(temp);

    xensiv_bgt60trxx_platform_spi_cs_set(dev->iface, 0);
    int32_t status = xensiv_bgt60trxx_platform_spi_transfer(dev->iface,
                                                            (uint8_t*)&temp,
                                                            NULL,
                                                            XENSIV_BGT60TRXX_SPI_REG_XFER_LEN_BYTES);
    xensiv_bgt60trxx_platform_spi_cs_set(dev->iface, 1);

    return status;
}


int32_t xensiv_bgt60trxx_get_reg(const xensiv_bgt60trxx_t* dev, uint32_t reg_addr, uint32_t* data)
{
    xensiv_bgt60trxx_platform_assert(dev != NULL);
    xensiv_bgt60trxx_platform_assert(data != NULL);

    uint32_t temp;
    temp = (reg_addr << XENSIV_BGT60TRXX_SPI_REGADR_POS) & XENSIV_BGT60TRXX_SPI_REGADR_MSK;

    temp = xensiv_bgt60trxx_platform_word_reverse(temp);

    xensiv_bgt60trxx_platform_spi_cs_set(dev->iface, 0);
    int32_t status = xensiv_bgt60trxx_platform_spi_transfer(dev->iface,
                                                            (uint8_t*)&temp,
                                                            (uint8_t*)data,
                                                            XENSIV_BGT60TRXX_SPI_REG_XFER_LEN_BYTES);
    xensiv_bgt60trxx_platform_spi_cs_set(dev->iface, 1);

    if (XENSIV_BGT60TRXX_STATUS_OK == status)
    {
        *data = xensiv_bgt60trxx_platform_word_reverse(*data);
        *data &= XENSIV_BGT60TRXX_SPI_DATA_MSK;
    }

    return status;
}


uint16_t xensiv_bgt60trxx_get_fifo_size(const xensiv_bgt60trxx_t* dev)
{
    return (dev->type->fifo_size);
}


xensiv_bgt60trxx_device_t xensiv_bgt60trxx_get_device(const xensiv_bgt60trxx_t* dev)
{
    return (dev->type->device);
}


int32_t xensiv_bgt60trxx_set_fifo_limit(const xensiv_bgt60trxx_t* dev, uint32_t num_samples)
{
    xensiv_bgt60trxx_platform_assert(dev != NULL);
    xensiv_bgt60trxx_platform_assert((num_samples % 2U) == 0U);
    xensiv_bgt60trxx_platform_assert((num_samples / 2U) <= dev->type->fifo_size);

    uint32_t tmp;
    int32_t retval = xensiv_bgt60trxx_get_reg(dev, XENSIV_BGT60TRXX_REG_SFCTL, &tmp);

    if (XENSIV_BGT60TRXX_STATUS_OK == retval)
    {
        tmp &= (uint32_t) ~XENSIV_BGT60TRXX_REG_SFCTL_FIFO_CREF_MSK;
        tmp |= (((num_samples / 2U) - 1U) << XENSIV_BGT60TRXX_REG_SFCTL_FIFO_CREF_POS) &
               XENSIV_BGT60TRXX_REG_SFCTL_FIFO_CREF_MSK;
        retval = xensiv_bgt60trxx_set_reg(dev, XENSIV_BGT60TRXX_REG_SFCTL, tmp);
    }

    return retval;
}


int32_t xensiv_bgt60trxx_get_fifo_data(const xensiv_bgt60trxx_t* dev, uint16_t* data,
                                       uint32_t num_samples)
{
    xensiv_bgt60trxx_platform_assert(dev != NULL);
    xensiv_bgt60trxx_platform_assert(data != NULL);
    xensiv_bgt60trxx_platform_assert((num_samples % 2U) == 0U);
    xensiv_bgt60trxx_platform_assert((num_samples / 2U) <= dev->type->fifo_size);

    uint32_t gsr0;
    uint32_t reg_addr = XENSIV_BGT60TRXX_SPI_BURST_MODE_CMD |
                        (dev->type->fifo_addr << XENSIV_BGT60TRXX_SPI_BURST_MODE_SADR_POS);

    reg_addr = xensiv_bgt60trxx_platform_word_reverse(reg_addr);

    /* SPI read burst mode command */
    xensiv_bgt60trxx_platform_spi_cs_set(dev->iface, false);

    int32_t retval = xensiv_bgt60trxx_platform_spi_transfer(dev->iface,
                                                            (uint8_t*)&reg_addr,
                                                            (uint8_t*)&gsr0,
                                                            XENSIV_BGT60TRXX_SPI_REG_XFER_LEN_BYTES);

    if (XENSIV_BGT60TRXX_STATUS_OK == retval)
    {
        if ((gsr0 & (XENSIV_BGT60TRXX_REG_GSR0_FOU_ERR_MSK |
                     XENSIV_BGT60TRXX_REG_GSR0_SPI_BURST_ERR_MSK |
                     XENSIV_BGT60TRXX_REG_GSR0_CLK_NUM_ERR_MSK)) == 0U)
        {
            retval = xensiv_bgt60trxx_platform_spi_fifo_read(dev->iface,
                                                             data,
                                                             num_samples);
        }
        else
        {
            retval = XENSIV_BGT60TRXX_STATUS_GSR0_ERROR;
        }
    }

    xensiv_bgt60trxx_platform_spi_cs_set(dev->iface, true);

    return retval;
}


int32_t xensiv_bgt60trxx_get_fifo_status(const xensiv_bgt60trxx_t* dev, uint32_t* status)
{
    xensiv_bgt60trxx_platform_assert(dev != NULL);
    xensiv_bgt60trxx_platform_assert(status != NULL);

    int32_t retval = xensiv_bgt60trxx_get_reg(dev, dev->type->fifo_addr - 1U, status);
    *status &= (uint32_t) ~XENSIV_BGT60TRXX_REG_FSTAT_FILL_STATUS_MSK;
    return retval;
}


int32_t xensiv_bgt60trxx_start_frame(const xensiv_bgt60trxx_t* dev, bool start)
{
    xensiv_bgt60trxx_platform_assert(dev != NULL);

    uint32_t tmp;
    int32_t status;

    if (start)
    {
        status = xensiv_bgt60trxx_get_reg(dev, XENSIV_BGT60TRXX_REG_MAIN, &tmp);
        if (status == XENSIV_BGT60TRXX_STATUS_OK)
        {
            tmp |= XENSIV_BGT60TRXX_REG_MAIN_FRAME_START_MSK;
            status = xensiv_bgt60trxx_set_reg(dev, XENSIV_BGT60TRXX_REG_MAIN, tmp);
        }
    }
    else
    {
        /* Stop chirp generation */
        status = xensiv_bgt60trxx_soft_reset(dev,
                                             XENSIV_BGT60TRXX_RESET_FSM);
    }

    return status;
}


int32_t xensiv_bgt60trxx_soft_reset(const xensiv_bgt60trxx_t* dev,
                                    xensiv_bgt60trxx_reset_t reset_type)
{
    xensiv_bgt60trxx_platform_assert(dev != NULL);

    uint32_t tmp;
    int32_t status;

    status = xensiv_bgt60trxx_get_reg(dev, XENSIV_BGT60TRXX_REG_MAIN, &tmp);
    if (XENSIV_BGT60TRXX_STATUS_OK == status)
    {
        tmp |= (uint32_t)reset_type;
        status = xensiv_bgt60trxx_set_reg(dev, XENSIV_BGT60TRXX_REG_MAIN, tmp);
    }

    uint32_t timeout = XENSIV_BGT60TRXX_RESET_WAIT_TIMEOUT;
    if (XENSIV_BGT60TRXX_STATUS_OK == status)
    {
        while (timeout > 0U)
        {
            status = xensiv_bgt60trxx_get_reg(dev, XENSIV_BGT60TRXX_REG_MAIN, &tmp);
            if ((XENSIV_BGT60TRXX_STATUS_OK == status) && ((tmp & (uint32_t)reset_type) == 0U))
            {
                break;
            }
            --timeout;
        }
    }

    if (XENSIV_BGT60TRXX_STATUS_OK == status)
    {
        if (timeout == 0U)
        {
            status = XENSIV_BGT60TRXX_STATUS_TIMEOUT_ERROR;
        }
        else
        {
            xensiv_bgt60trxx_platform_delay(XENSIV_BGT60TRXX_SOFT_RESET_DELAY_MS);
        }
    }

    return status;
}


int32_t xensiv_bgt60trxx_enable_data_test_mode(const xensiv_bgt60trxx_t* dev, bool enable)
{
    xensiv_bgt60trxx_platform_assert(dev != NULL);

    uint32_t tmp;
    int32_t status;

    status = xensiv_bgt60trxx_get_reg(dev, XENSIV_BGT60TRXX_REG_SFCTL, &tmp);
    if (XENSIV_BGT60TRXX_STATUS_OK == status)
    {
        if (enable)
        {
            tmp |= XENSIV_BGT60TRXX_REG_SFCTL_LFSR_EN_MSK;
        }
        else
        {
            tmp &= (uint32_t) ~XENSIV_BGT60TRXX_REG_SFCTL_LFSR_EN_MSK;
        }

        status = xensiv_bgt60trxx_set_reg(dev, XENSIV_BGT60TRXX_REG_SFCTL, tmp);
    }

    return status;
}


/* Because the reset input is multiplexed with the quad SPI data line
   DIO3 the SPI CS signal must be HIGH all time during a reset
   condition.

   The reset signal of the connected device must be driven low
   and kept low for at least 1000ns, before going HIGH again. */
void xensiv_bgt60trxx_hard_reset(const xensiv_bgt60trxx_t* dev)
{
    xensiv_bgt60trxx_platform_assert(dev != NULL);
    xensiv_bgt60trxx_platform_assert(dev->iface != NULL);

    xensiv_bgt60trxx_platform_rst_set(dev->iface, true);
    xensiv_bgt60trxx_platform_spi_cs_set(dev->iface, true);

    xensiv_bgt60trxx_platform_delay(1U);

    xensiv_bgt60trxx_platform_rst_set(dev->iface, false);

    xensiv_bgt60trxx_platform_delay(1U);

    xensiv_bgt60trxx_platform_rst_set(dev->iface, true);

    xensiv_bgt60trxx_platform_delay(1U);
}


uint16_t xensiv_bgt60trxx_get_next_test_word(uint16_t cur_test_word)
{
    uint16_t next_value = (cur_test_word >> 1) |
                          (((cur_test_word << 11) ^
                            (cur_test_word << 10) ^
                            (cur_test_word <<  9) ^
                            (cur_test_word <<  3)) & 0x0800U);

    return next_value;
}
