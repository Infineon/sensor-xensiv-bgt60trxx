/***********************************************************************************************//**
 * \file xensiv_bgt60trxx.c
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

#include <stddef.h>

#include "xensiv_bgt60trxx.h"
#include "xensiv_bgt60trxx_platform.h"

#define XENSIV_BGT60TRXX_SPI_REG_XFER_LEN_BYTES         (4U)
#define XENSIV_BGT60TRXX_SOFT_RESET_DELAY_MS            (10U)

#define XENSIV_BGT60TRXX_CHIPID_TR13C                   (0x000303UL)

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
#define XENSIV_BGT60TRXX_SPI_BURST_MODE_SADR_FIFO       (0x60UL)
#define XENSIV_BGT60TRXX_REG_MAIN_RESET_POS             (1)

int32_t xensiv_bgt60trxx_init(xensiv_bgt60trxx_t dev, const uint32_t* regs, uint32_t len)
{
    xensiv_bgt60trxx_platform_assert(dev != NULL);

    int32_t status = xensiv_bgt60trxx_set_reg(dev, XENSIV_BGT60TRXX_REG_SFCTL, 0x102000UL);
    if (XENSIV_BGT60TRXX_STATUS_OK == status)
    {
        /* Read chipid */
        uint32_t chipid;
        status = xensiv_bgt60trxx_get_reg(dev, XENSIV_BGT60TRXX_REG_CHIP_ID, &chipid);

        if (XENSIV_BGT60TRXX_STATUS_OK == status)
        {
            /* Check chipid */
            if (chipid != XENSIV_BGT60TRXX_CHIPID_TR13C)
            {
                status = XENSIV_BGT60TRXX_STATUS_CHIPID_ERROR;
            }
        }
    }

    if (XENSIV_BGT60TRXX_STATUS_OK == status)
    {
        /* Perform software reset if chipid is correct */
        status = xensiv_bgt60trxx_soft_reset(dev,
                                             XENSIV_BGT60TRXX_RESET_SW);
    }

    if (XENSIV_BGT60TRXX_STATUS_OK == status)
    {
        status = xensiv_bgt60trxx_set_reg(dev, XENSIV_BGT60TRXX_REG_SFCTL, 0x102000UL);
    }

    if ((XENSIV_BGT60TRXX_STATUS_OK == status) && (regs != NULL))
    {
        /* Apply register configuration */
        for (uint32_t reg_idx = 0; reg_idx < len; ++reg_idx)
        {
            uint32_t val = regs[reg_idx];
            status = xensiv_bgt60trxx_set_reg(dev,
                                              ((val & XENSIV_BGT60TRXX_SPI_REGADR_MSK) >>
                                               XENSIV_BGT60TRXX_SPI_REGADR_POS),
                                              ((val & XENSIV_BGT60TRXX_SPI_DATA_MSK) >>
                                               XENSIV_BGT60TRXX_SPI_DATA_POS));

            if (status != XENSIV_BGT60TRXX_STATUS_OK)
            {
                break;
            }
        }
    }

    return status;
}


int32_t xensiv_bgt60trxx_set_reg(xensiv_bgt60trxx_t dev, uint32_t reg_addr, uint32_t data)
{
    xensiv_bgt60trxx_platform_assert(dev != NULL);

    uint32_t temp;
    temp = (reg_addr << XENSIV_BGT60TRXX_SPI_REGADR_POS) & XENSIV_BGT60TRXX_SPI_REGADR_MSK;
    temp |= XENSIV_BGT60TRXX_SPI_WR_OP_MSK;
    temp |= (data << XENSIV_BGT60TRXX_SPI_DATA_POS) & XENSIV_BGT60TRXX_SPI_DATA_MSK;

    temp = xensiv_bgt60trxx_platform_word_reverse(temp);

    return xensiv_bgt60trxx_platform_spi_transfer(dev,
                                                  (uint8_t*)&temp,
                                                  4U,
                                                  NULL,
                                                  XENSIV_BGT60TRXX_SPI_REG_XFER_LEN_BYTES);
}


int32_t xensiv_bgt60trxx_get_reg(xensiv_bgt60trxx_t dev, uint32_t reg_addr, uint32_t* data)
{
    xensiv_bgt60trxx_platform_assert(dev != NULL);
    xensiv_bgt60trxx_platform_assert(data != NULL);

    uint32_t temp;
    temp = (reg_addr << XENSIV_BGT60TRXX_SPI_REGADR_POS) & XENSIV_BGT60TRXX_SPI_REGADR_MSK;

    temp = xensiv_bgt60trxx_platform_word_reverse(temp);

    int32_t status = xensiv_bgt60trxx_platform_spi_transfer(dev,
                                                            (uint8_t*)&temp,
                                                            4U,
                                                            (uint8_t*)data,
                                                            XENSIV_BGT60TRXX_SPI_REG_XFER_LEN_BYTES);

    if (XENSIV_BGT60TRXX_STATUS_OK == status)
    {
        *data = xensiv_bgt60trxx_platform_word_reverse(*data);
        *data &= XENSIV_BGT60TRXX_SPI_DATA_MSK;
    }

    return status;
}


int32_t xensiv_bgt60trxx_set_fifo_limit(xensiv_bgt60trxx_t dev, uint16_t limit)
{
    xensiv_bgt60trxx_platform_assert(dev != NULL);
    xensiv_bgt60trxx_platform_assert(limit <= XENSIV_BGT60TRXX_FIFO_SIZE_WORDS);

    uint32_t tmp;
    int32_t retval = xensiv_bgt60trxx_get_reg(dev, XENSIV_BGT60TRXX_REG_SFCTL, &tmp);

    if (XENSIV_BGT60TRXX_STATUS_OK == retval)
    {
        tmp &= (uint32_t) ~XENSIV_BGT60TRXX_REG_SFCTL_FIFO_CREF_MSK;
        tmp |= (uint32_t)limit - 1U;
        retval = xensiv_bgt60trxx_set_reg(dev, XENSIV_BGT60TRXX_REG_SFCTL, tmp);
    }

    return retval;
}


int32_t xensiv_bgt60trxx_get_fifo_data(xensiv_bgt60trxx_t dev, uint8_t* data, uint32_t len)
{
    xensiv_bgt60trxx_platform_assert(dev != NULL);
    xensiv_bgt60trxx_platform_assert(data != NULL);
    xensiv_bgt60trxx_platform_assert(len <= XENSIV_BGT60TRXX_FIFO_SIZE_WORDS);

    uint32_t reg_addr = XENSIV_BGT60TRXX_SPI_BURST_MODE_CMD |
                        (XENSIV_BGT60TRXX_SPI_BURST_MODE_SADR_FIFO <<
                         XENSIV_BGT60TRXX_SPI_BURST_MODE_SADR_POS);

    reg_addr = xensiv_bgt60trxx_platform_word_reverse(reg_addr);

    // IRQ FIFO interrupt is generated based on FIFO words.
    // One FIFO word of 24 bit captures two ADC samples of 12 bit.
    // For dual and quad ADC operation all samples result in 1 or more FIFO words.
    // In single ADC mode, if an odd number of samples are selected, the FIFO interrupt will be
    // generated after the following (even) sample.
    // For single channel mode an even number of samples is recommended.
    uint32_t xfer_len = XENSIV_BGT60TRXX_SPI_BURST_HEADER_SIZE_BYTES +
                        (len * XENSIV_BGT60TRXX_FIFO_WORD_SIZE_BYTES);

    return xensiv_bgt60trxx_platform_spi_transfer(dev,
                                                  (uint8_t*)&reg_addr,
                                                  4U,
                                                  data,
                                                  xfer_len);
}


int32_t xensiv_bgt60trxx_get_fifo_status(xensiv_bgt60trxx_t dev, uint32_t* status)
{
    xensiv_bgt60trxx_platform_assert(dev != NULL);
    xensiv_bgt60trxx_platform_assert(status != NULL);

    int32_t retval = xensiv_bgt60trxx_get_reg(dev, XENSIV_BGT60TRXX_REG_FSTAT, status);
    *status &= (uint32_t) ~XENSIV_BGT60TRXX_REG_FSTAT_FILL_STATUS_MSK;
    return retval;
}


int32_t xensiv_bgt60trxx_start_frame(xensiv_bgt60trxx_t dev, bool start)
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


int32_t xensiv_bgt60trxx_soft_reset(xensiv_bgt60trxx_t dev,
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
            if ((status == XENSIV_BGT60TRXX_STATUS_OK) && ((tmp & (uint32_t)reset_type) == 0U))
            {
                break;
            }
            --timeout;
        }
    }

    if (timeout == 0U)
    {
        status = XENSIV_BGT60TRXX_STATUS_TIMEOUT_ERROR;
    }
    else
    {
        xensiv_bgt60trxx_platform_delay(XENSIV_BGT60TRXX_SOFT_RESET_DELAY_MS);
    }

    return status;
}


int32_t xensiv_bgt60trxx_enable_data_test_mode(xensiv_bgt60trxx_t dev, bool enable)
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


uint16_t xensiv_bgt60trxx_get_next_test_word(uint16_t cur_test_word)
{
    uint16_t next_value = (cur_test_word >> 1) |
                          (((cur_test_word << 11) ^
                            (cur_test_word << 10) ^
                            (cur_test_word <<  9) ^
                            (cur_test_word <<  3)) & 0x0800U);

    return next_value;
}
