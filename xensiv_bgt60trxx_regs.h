/***********************************************************************************************//**
 * \file xensiv_bgt60trxx_regs.h
 *
 * \brief
 * This file contains the register definitions
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

#ifndef XENSIV_BGT60TRXX_REGS_H_
#define XENSIV_BGT60TRXX_REGS_H_

/**
 * \addtogroup group_board_libs XENSIV(TM) BGT60TRxx Radar Sensor
 * \{
 */

#define XENSIV_BGT60TRXX_REG_MAIN                       (0x00U)     /*!< MAIN: addr */
#define XENSIV_BGT60TRXX_REG_ADC0                       (0x01U)     /*!< ADC0: addr */
#define XENSIV_BGT60TRXX_REG_CHIP_ID                    (0x02U)     /*!< CHIP_ID: addr */
#define XENSIV_BGT60TRXX_REG_STAT1                      (0x03U)     /*!< STAT1: addr */
#define XENSIV_BGT60TRXX_REG_PACR1                      (0x04U)     /*!< PACR1: addr */
#define XENSIV_BGT60TRXX_REG_PACR2                      (0x05U)     /*!< PACR2: addr */
#define XENSIV_BGT60TRXX_REG_SFCTL                      (0x06U)     /*!< SFCTL: addr */
#define XENSIV_BGT60TRXX_REG_SADC_CTRL                  (0x07U)     /*!< SADC_CTRL: addr */
#define XENSIV_BGT60TRXX_REG_CSI_0                      (0x08U)     /*!< CSI_0: addr */
#define XENSIV_BGT60TRXX_REG_CSI_1                      (0x09U)     /*!< CSI_1: addr */
#define XENSIV_BGT60TRXX_REG_CSI_2                      (0x0aU)     /*!< CSI_2: addr */
#define XENSIV_BGT60TRXX_REG_CSCI                       (0x0bU)     /*!< CSCI: addr */
#define XENSIV_BGT60TRXX_REG_CSDS_0                     (0x0cU)     /*!< CSDS_0: addr */
#define XENSIV_BGT60TRXX_REG_CSDS_1                     (0x0dU)     /*!< CSDS_1: addr */
#define XENSIV_BGT60TRXX_REG_CSDS_2                     (0x0eU)     /*!< REG_CSDS_2: addr */
#define XENSIV_BGT60TRXX_REG_CSCDS                      (0x0fU)     /*!< REG_CSCDS: addr */
#define XENSIV_BGT60TRXX_REG_CSU1_0                     (0x10U)     /*!< REG_CS1_U_0: addr */
#define XENSIV_BGT60TRXX_REG_CSU1_1                     (0x11U)     /*!< REG_CS1_U_1: addr */
#define XENSIV_BGT60TRXX_REG_CSU1_2                     (0x12U)     /*!< REG_CS1_U_2: addr */
#define XENSIV_BGT60TRXX_REG_CSD1_0                     (0x13U)     /*!< REG_CS1_D_0: addr */
#define XENSIV_BGT60TRXX_REG_CSD1_1                     (0x14U)     /*!< REG_CS1_D_1: addr */
#define XENSIV_BGT60TRXX_REG_CSD1_2                     (0x15U)     /*!< REG_CS1_D_2: addr */
#define XENSIV_BGT60TRXX_REG_CSC1                       (0x16U)     /*!< REG_CSC1: addr */
#define XENSIV_BGT60TRXX_REG_CSU2_0                     (0x17U)     /*!< REG_CS2_U_0: addr */
#define XENSIV_BGT60TRXX_REG_CSU2_1                     (0x18U)     /*!< REG_CS2_U_1: addr */
#define XENSIV_BGT60TRXX_REG_CSU2_2                     (0x19U)     /*!< REG_CS2_U_2: addr */
#define XENSIV_BGT60TRXX_REG_CSD2_0                     (0x1aU)     /*!< REG_CS2_D_0: addr */
#define XENSIV_BGT60TRXX_REG_CSD2_1                     (0x1bU)     /*!< REG_CS2_D_1: addr */
#define XENSIV_BGT60TRXX_REG_CSD2_2                     (0x1cU)     /*!< REG_CS2_D_2: addr */
#define XENSIV_BGT60TRXX_REG_CSC2                       (0x1dU)     /*!< REG_CSC2: addr */
#define XENSIV_BGT60TRXX_REG_CSU3_0                     (0x1eU)     /*!< REG_CS3_U_0: addr */
#define XENSIV_BGT60TRXX_REG_CSU3_1                     (0x1fU)     /*!< REG_CS3_U_1: addr */
#define XENSIV_BGT60TRXX_REG_CSU3_2                     (0x20U)     /*!< REG_CS3_U_2: addr */
#define XENSIV_BGT60TRXX_REG_CSD3_0                     (0x21U)     /*!< REG_CS3_D_0: addr */
#define XENSIV_BGT60TRXX_REG_CSD3_1                     (0x22U)     /*!< REG_CS3_D_1: addr */
#define XENSIV_BGT60TRXX_REG_CSD3_2                     (0x23U)     /*!< REG_CS3_D_2: addr */
#define XENSIV_BGT60TRXX_REG_CSC3                       (0x24U)     /*!< REG_CSC3: addr */
#define XENSIV_BGT60TRXX_REG_CSU4_0                     (0x25U)     /*!< REG_CS4_U_0: addr */
#define XENSIV_BGT60TRXX_REG_CSU4_1                     (0x26U)     /*!< REG_CS4_U_1: addr */
#define XENSIV_BGT60TRXX_REG_CSU4_2                     (0x27U)     /*!< REG_CS4_U_2: addr */
#define XENSIV_BGT60TRXX_REG_CSD4_0                     (0x28U)     /*!< REG_CS4_D_0: addr */
#define XENSIV_BGT60TRXX_REG_CSD4_1                     (0x29U)     /*!< REG_CS4_D_1: addr */
#define XENSIV_BGT60TRXX_REG_CSD4_2                     (0x2aU)     /*!< REG_CS4_D_2: addr */
#define XENSIV_BGT60TRXX_REG_CSC4                       (0x2bU)     /*!< REG_CSC4: addr */
#define XENSIV_BGT60TRXX_REG_CCR0                       (0x2cU)     /*!< REG_CCR0: addr */
#define XENSIV_BGT60TRXX_REG_CCR1                       (0x2dU)     /*!< REG_CCR1: addr */
#define XENSIV_BGT60TRXX_REG_CCR2                       (0x2eU)     /*!< REG_CCR2: addr */
#define XENSIV_BGT60TRXX_REG_CCR3                       (0x2fU)     /*!< REG_CCR3: addr */
#define XENSIV_BGT60TRXX_REG_PLL1_0                     (0x30U)     /*!< REG_PLL1_0: addr */
#define XENSIV_BGT60TRXX_REG_PLL1_1                     (0x31U)     /*!< REG_PLL1_1: addr */
#define XENSIV_BGT60TRXX_REG_PLL1_2                     (0x32U)     /*!< REG_PLL1_2: addr */
#define XENSIV_BGT60TRXX_REG_PLL1_3                     (0x33U)     /*!< REG_PLL1_3: addr */
#define XENSIV_BGT60TRXX_REG_PLL1_4                     (0x34U)     /*!< REG_PLL1_4: addr */
#define XENSIV_BGT60TRXX_REG_PLL1_5                     (0x35U)     /*!< REG_PLL1_5: addr */
#define XENSIV_BGT60TRXX_REG_PLL1_6                     (0x36U)     /*!< REG_PLL1_6: addr */
#define XENSIV_BGT60TRXX_REG_PLL1_7                     (0x37U)     /*!< REG_PLL1_7: addr */
#define XENSIV_BGT60TRXX_REG_PLL2_0                     (0x38U)     /*!< REG_PLL2_0: addr */
#define XENSIV_BGT60TRXX_REG_PLL2_1                     (0x39U)     /*!< REG_PLL2_1: addr */
#define XENSIV_BGT60TRXX_REG_PLL2_2                     (0x3aU)     /*!< REG_PLL2_2: addr */
#define XENSIV_BGT60TRXX_REG_PLL2_3                     (0x3bU)     /*!< REG_PLL2_3: addr */
#define XENSIV_BGT60TRXX_REG_PLL2_4                     (0x3cU)     /*!< REG_PLL2_4: addr */
#define XENSIV_BGT60TRXX_REG_PLL2_5                     (0x3dU)     /*!< REG_PLL2_5: addr */
#define XENSIV_BGT60TRXX_REG_PLL2_6                     (0x3eU)     /*!< REG_PLL2_6: addr */
#define XENSIV_BGT60TRXX_REG_PLL2_7                     (0x3fU)     /*!< REG_PLL2_7: addr */
#define XENSIV_BGT60TRXX_REG_PLL3_0                     (0x40U)     /*!< REG_PLL3_0: addr */
#define XENSIV_BGT60TRXX_REG_PLL3_1                     (0x41U)     /*!< REG_PLL3_1: addr */
#define XENSIV_BGT60TRXX_REG_PLL3_2                     (0x42U)     /*!< REG_PLL3_2: addr */
#define XENSIV_BGT60TRXX_REG_PLL3_3                     (0x43U)     /*!< REG_PLL3_3: addr */
#define XENSIV_BGT60TRXX_REG_PLL3_4                     (0x44U)     /*!< REG_PLL3_4: addr */
#define XENSIV_BGT60TRXX_REG_PLL3_5                     (0x45U)     /*!< REG_PLL3_5: addr */
#define XENSIV_BGT60TRXX_REG_PLL3_6                     (0x46U)     /*!< REG_PLL3_6: addr */
#define XENSIV_BGT60TRXX_REG_PLL3_7                     (0x47U)     /*!< REG_PLL3_7: addr */
#define XENSIV_BGT60TRXX_REG_PLL4_0                     (0x48U)     /*!< REG_PLL4_0: addr */
#define XENSIV_BGT60TRXX_REG_PLL4_1                     (0x49U)     /*!< REG_PLL4_1: addr */
#define XENSIV_BGT60TRXX_REG_PLL4_2                     (0x4aU)     /*!< REG_PLL4_2: addr */
#define XENSIV_BGT60TRXX_REG_PLL4_3                     (0x4bU)     /*!< REG_PLL4_3: addr */
#define XENSIV_BGT60TRXX_REG_PLL4_4                     (0x4cU)     /*!< REG_PLL4_4: addr */
#define XENSIV_BGT60TRXX_REG_PLL4_5                     (0x4dU)     /*!< REG_PLL4_5: addr */
#define XENSIV_BGT60TRXX_REG_PLL4_6                     (0x4eU)     /*!< REG_PLL4_6: addr */
#define XENSIV_BGT60TRXX_REG_PLL4_7                     (0x4fU)     /*!< REG_PLL4_7: addr */
#define XENSIV_BGT60TRXX_REG_RFT0                       (0x55U)     /*!< REG_RFT0: addr */
#define XENSIV_BGT60TRXX_REG_RFT1                       (0x56U)     /*!< REG_RFT1: addr */
#define XENSIV_BGT60TRXX_REG_PLL_DFT0                   (0x59U)     /*!< REG_PDFT0: addr */
#define XENSIV_BGT60TRXX_REG_STAT0                      (0x5dU)     /*!< REG_STAT0: addr */
#define XENSIV_BGT60TRXX_REG_SDAC_RESULT                (0x5eU)     /*!< REG_SADC_RESULT: addr */
#define XENSIV_BGT60TRXX_REG_FSTAT_TR13C                (0x5fU)     /*!< TR13C REG_FSTAT: addr */
#define XENSIV_BGT60TRXX_REG_FIFO_TR13C                 (0x60U)     /*!< TR13C REG_FIFO: addr */
#define XENSIV_BGT60TRXX_REG_FSTAT_UTR13D               (0x5fU)     /*!< UTR13D REG_FSTAT: addr */
#define XENSIV_BGT60TRXX_REG_FIFO_UTR13D                (0x60U)     /*!< UTR13D REG_FIFO: addr */
#define XENSIV_BGT60TRXX_REG_FSTAT_UTR11                (0x63U)     /*!< UTR11 REG_FSTAT: addr */
#define XENSIV_BGT60TRXX_REG_FIFO_UTR11                 (0x64U)     /*!< UTR11: REG_FIFO: addr */

/* Fields of register MAIN */
/* -------------------------- */
#define XENSIV_BGT60TRXX_REG_MAIN_FRAME_START_POS       (0)          /*!< FRAME_START: pos */
#define XENSIV_BGT60TRXX_REG_MAIN_FRAME_START_MSK       (0x000001UL) /*!< FRAME_START: msk */
#define XENSIV_BGT60TRXX_REG_MAIN_RESET_POS             (1)          /*!< RESET: pos */
#define XENSIV_BGT60TRXX_REG_MAIN_RESET_MSK             (0x00000eUL) /*!< RESET: msk */

/* Fields of register CHIP_ID */
/* -------------------------- */
#define XENSIV_BGT60TRXX_REG_CHIP_ID_RF_ID_POS          (0)          /*!< RF_ID: pos */
#define XENSIV_BGT60TRXX_REG_CHIP_ID_RF_ID_MSK          (0x0000ffUL) /*!< RF_ID: msk */
#define XENSIV_BGT60TRXX_REG_CHIP_ID_DIGITAL_ID_POS     (8)          /*!< DIGITAL_ID: pos */
#define XENSIV_BGT60TRXX_REG_CHIP_ID_DIGITAL_ID_MSK     (0xffff00UL) /*!< DIGITAL_ID: msk */

/* Fields of register STAT1 */
/* ------------------------ */
#define XENSIV_BGT60TRXX_REG_STAT1_SHAPE_GRP_CNT_POS    (0)          /*!< SHAPE_GRP_CNT: pos */
#define XENSIV_BGT60TRXX_REG_STAT1_SHAPE_GRP_CNT_MSK    (0x000fffUL) /*!< SHAPE_GRP_CNT: msk */
#define XENSIV_BGT60TRXX_REG_STAT1_FRAME_CNT_POS        (12)         /*!< FRAME_CNT: pos */
#define XENSIV_BGT60TRXX_REG_STAT1_FRAME_CNT_MSK        (0xfff000UL) /*!< FRAME_CNT: msk */

/* Fields of register SFCTL */
/* ------------------------ */
#define XENSIV_BGT60TRXX_REG_SFCTL_FIFO_CREF_POS        (0)          /*!< FIFO_CREF: pos */
#define XENSIV_BGT60TRXX_REG_SFCTL_FIFO_CREF_MSK        (0x001fffUL) /*!< FIFO_CREF: msk */
#define XENSIV_BGT60TRXX_REG_SFCTL_FIFO_LP_MODE_POS     (13)         /*!< FIFO_LP_MODE: pos */
#define XENSIV_BGT60TRXX_REG_SFCTL_FIFO_LP_MODE_MSK     (0x002000UL) /*!< FIFO_LP_MODE: msk */
#define XENSIV_BGT60TRXX_REG_SFCTL_MISO_HS_READ_POS     (16)         /*!< MISO_HF_READ: pos */
#define XENSIV_BGT60TRXX_REG_SFCTL_MISO_HS_READ_MSK     (0x010000UL) /*!< MISO_HF_READ: msk */
#define XENSIV_BGT60TRXX_REG_SFCTL_LFSR_EN_POS          (17)         /*!< LFSR_EN: pos */
#define XENSIV_BGT60TRXX_REG_SFCTL_LFSR_EN_MSK          (0x020000UL) /*!< LFSR_EN: msk */
#define XENSIV_BGT60TRXX_REG_SFCTL_PREFIX_EN_POS        (18)         /*!< PREFIX_EN: pos */
#define XENSIV_BGT60TRXX_REG_SFCTL_PREFIX_EN_MSK        (0x040000UL) /*!< PREFIX_EN: msk */

/* Fields of register STAT0 */
/* ------------------------ */
#define XENSIV_BGT60TRXX_REG_STAT0_SADC_RDY_POS         (0)          /*!< SADC_RDY: pos */
#define XENSIV_BGT60TRXX_REG_STAT0_SADC_RDY_MSK         (0x000001UL) /*!< SADC_RDY: msk */
#define XENSIV_BGT60TRXX_REG_STAT0_MADC_RDY_POS         (1)          /*!< MADC_RDY: pos */
#define XENSIV_BGT60TRXX_REG_STAT0_MADC_RDY_MSK         (0x000002UL) /*!< MADC_RDY: msk */
#define XENSIV_BGT60TRXX_REG_STAT0_MADC_BGUP_POS        (2)          /*!< MADC_BGUP: pos */
#define XENSIV_BGT60TRXX_REG_STAT0_MADC_BGUP_MSK        (0x000004UL) /*!< MADC_BGUP: msk */
#define XENSIV_BGT60TRXX_REG_STAT0_LDO_RDY_POS          (3)          /*!< LDO_RDY: pos */
#define XENSIV_BGT60TRXX_REG_STAT0_LDO_RDY_MSK          (0x000008UL) /*!< LDO_RDY: msk */
#define XENSIV_BGT60TRXX_REG_STAT0_PM_POS               (5)          /*!< PM: pos */
#define XENSIV_BGT60TRXX_REG_STAT0_PM_MSK               (0x0000e0UL) /*!< PM: msk */
#define XENSIV_BGT60TRXX_REG_STAT0_CH_IDX_POS           (8)          /*!< CH_IDX: pos */
#define XENSIV_BGT60TRXX_REG_STAT0_CH_IDX_MSK           (0x000700UL) /*!< CH_IDX: msk */
#define XENSIV_BGT60TRXX_REG_STAT0_SH_IDX_POS           (11)         /*!< SH_IDX: pos */
#define XENSIV_BGT60TRXX_REG_STAT0_SH_IDX_MSK           (0x003800UL) /*!< SH_IDX: msk */

/* Fields of register FSTAT */
/* ------------------------ */
#define XENSIV_BGT60TRXX_REG_FSTAT_FILL_STATUS_POS      (0)          /*!< FILL_STATUS: pos */
#define XENSIV_BGT60TRXX_REG_FSTAT_FILL_STATUS_MSK      (0x003fffUL) /*!< FILL_STATUS: msk */
#define XENSIV_BGT60TRXX_REG_FSTAT_CLK_NUM_ERR_POS      (17)         /*!< CLK_NUM_ERR: pos */
#define XENSIV_BGT60TRXX_REG_FSTAT_CLK_NUM_ERR_MSK      (0x020000UL) /*!< CLK_NUM_ERR: msk */
#define XENSIV_BGT60TRXX_REG_FSTAT_SPI_BURST_ERR_POS    (18)         /*!< SPI_BURST_ERR: pos */
#define XENSIV_BGT60TRXX_REG_FSTAT_SPI_BURST_ERR_MSK    (0x040000UL) /*!< SPI_BURST_ERR: msk */
#define XENSIV_BGT60TRXX_REG_FSTAT_FUF_ERR_POS          (19)         /*!< FUF_ERR: pos */
#define XENSIV_BGT60TRXX_REG_FSTAT_FUF_ERR_MSK          (0x080000UL) /*!< FUF_ERR: msk */
#define XENSIV_BGT60TRXX_REG_FSTAT_EMPTY_POS            (20)         /*!< EMPTY: pos */
#define XENSIV_BGT60TRXX_REG_FSTAT_EMPTY_MSK            (0x100000UL) /*!< EMPTY: msk */
#define XENSIV_BGT60TRXX_REG_FSTAT_CREF_POS             (21)         /*!< CREF: pos */
#define XENSIV_BGT60TRXX_REG_FSTAT_CREF_MSK             (0x200000UL) /*!< CREF: msk */
#define XENSIV_BGT60TRXX_REG_FSTAT_FULL_POS             (22)         /*!< FULL: pos */
#define XENSIV_BGT60TRXX_REG_FSTAT_FULL_MSK             (0x400000UL) /*!< FULL: msk */
#define XENSIV_BGT60TRXX_REG_FSTAT_FOF_ERR_POS          (23)         /*!< FOF_ERR: pos */
#define XENSIV_BGT60TRXX_REG_FSTAT_FOF_ERR_MSK          (0x800000UL) /*!< FOF_ERR: msk */

/* Fields of register GSR0 */
/* ------------------------ */
#define XENSIV_BGT60TRXX_REG_GSR0_FOU_ERR_MSK           (0x01UL)     /*!< FOU_ERR: msk */
#define XENSIV_BGT60TRXX_REG_GSR0_MISO_HS_READ_MSK      (0x02UL)     /*!< MISO_HS_READ: msk */
#define XENSIV_BGT60TRXX_REG_GSR0_SPI_BURST_ERR_MSK     (0x04UL)     /*!< SPI_BURST_ERR: msk */
#define XENSIV_BGT60TRXX_REG_GSR0_CLK_NUM_ERR_MSK       (0x08UL)     /*!< CLK_NUM_ERR: msk */

/** \} group_board_libs */

#endif // ifndef XENSIV_BGT60TRXX_REGS_H_
