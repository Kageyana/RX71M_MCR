/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name        : Config_SCI12.c
* Component Version: 1.11.0
* Device(s)        : R5F571MFCxFP
* Description      : This file implements device driver for Config_SCI12.
***********************************************************************************************************************/

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "Config_SCI12.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
volatile uint8_t   g_sci12_iic_transmit_receive_flag; /* SCI12 transmit receive flag for I2C */
volatile uint8_t   g_sci12_iic_cycle_flag;            /* SCI12 start stop flag for I2C */
volatile uint8_t   g_sci12_slave_address;             /* SCI12 target slave address */
volatile uint8_t * gp_sci12_tx_address;               /* SCI12 transmit buffer address */
volatile uint16_t  g_sci12_tx_count;                  /* SCI12 transmit data number */
volatile uint8_t * gp_sci12_rx_address;               /* SCI12 receive buffer address */
volatile uint16_t  g_sci12_rx_count;                  /* SCI12 receive data number */
volatile uint16_t  g_sci12_rx_length;                 /* SCI12 receive data length */
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_SCI12_Create
* Description  : This function initializes the SCI12 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_SCI12_Create(void)
{
    /* Cancel SCI stop state */
    MSTP(SCI12) = 0U;

    /* Set interrupt priority */
    IPR(SCI12, RXI12) = _07_SCI_PRIORITY_LEVEL7;
    IPR(SCI12, TXI12) = _08_SCI_PRIORITY_LEVEL8;

    /* Clear the control register */
    SCI12.SCR.BYTE = 0x00U;

    /* Initialize SSCL and SSDA pins to high impedance */
    SCI12.SIMR3.BYTE = _C0_SCI_SSCL_HIGH_IMPEDANCE | _30_SCI_SSDA_HIGH_IMPEDANCE;

    /* Set up transfer or reception format in SMR and SCMR */
    SCI12.SMR.BYTE = _00_SCI_CLOCK_PCLK | _00_SCI_ASYNCHRONOUS_OR_I2C_MODE;
    SCI12.SCMR.BIT.SMIF = 0U;
    SCI12.SCMR.BIT.SINV = 0U;
    SCI12.SCMR.BIT.SDIR = 1U;

    /* Set bit rate */
    SCI12.BRR = 0x04U;
    SCI12.SEMR.BYTE = _00_SCI_NOISE_FILTER_DISABLE | _00_SCI_BIT_MODULATION_DISABLE;
    SCI12.SIMR1.BYTE |= (_01_SCI_IIC_MODE | _08_SCI_0_TO_1_CYCLE);
    SCI12.SIMR2.BYTE |= (_01_SCI_RX_TX_INTERRUPTS | _02_SCI_SYNCHRONIZATION | _20_SCI_NACK_TRANSMISSION);
    SCI12.SPMR.BYTE = _00_SCI_CLOCK_NOT_INVERTED | _00_SCI_CLOCK_NOT_DELAYED;
    SCI12.SCR.BYTE = _10_SCI_RECEIVE_ENABLE | _20_SCI_TRANSMIT_ENABLE | _40_SCI_RXI_ERI_ENABLE | _80_SCI_TXI_ENABLE | 
                     _04_SCI_TEI_INTERRUPT_ENABLE;

    /* Set SSCL12 pin */
    MPC.PE2PFS.BYTE = 0x0CU;
    PORTE.ODR0.BYTE |= 0x10U;
    PORTE.PMR.BYTE |= 0x04U;

    /* Set SSDA12 pin */
    MPC.PE1PFS.BYTE = 0x0CU;
    PORTE.ODR0.BYTE |= 0x04U;
    PORTE.PMR.BYTE |= 0x02U;

    R_Config_SCI12_Create_UserInit();
}

/***********************************************************************************************************************
* Function Name: R_Config_SCI12_Start
* Description  : This function starts the SCI12 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_SCI12_Start(void)
{
    /* Clear interrupt flag */
    IR(SCI12,TXI12) = 0U;
    IR(SCI12,RXI12) = 0U;

    /* Enable SCI interrupt */
    IEN(SCI12,TXI12) = 1U;
    ICU.GENBL0.BIT.EN16 = 1U;
    IEN(SCI12,RXI12) = 1U;
    ICU.GENBL0.BIT.EN17 = 1U;
}

/***********************************************************************************************************************
* Function Name: R_Config_SCI12_Stop
* Description  : This function stops the SCI12 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_SCI12_Stop(void)
{
    IR(SCI12,TXI12) = 0U;
    IEN(SCI12,TXI12) = 0U;
    ICU.GENBL0.BIT.EN16 = 0U;
    IR(SCI12,RXI12) = 0U;
    IEN(SCI12,RXI12) = 0U;
    ICU.GENBL0.BIT.EN17 = 0U;
}

/***********************************************************************************************************************
* Function Name: R_Config_SCI12_IIC_StartCondition
* Description  : This function generates IIC start condition
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_SCI12_IIC_StartCondition(void)
{
    SCI12.SIMR3.BYTE = _01_SCI_START_CONDITION_ON | _10_SCI_SSDA_START_RESTART_STOP_CONDITION | 
                       _40_SCI_SSCL_START_RESTART_STOP_CONDITION;
}

/***********************************************************************************************************************
* Function Name: R_Config_SCI12_IIC_StopCondition
* Description  : This function generates IIC stop condition
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_SCI12_IIC_StopCondition(void)
{
    SCI12.SIMR3.BYTE = _04_SCI_STOP_CONDITION_ON | _10_SCI_SSDA_START_RESTART_STOP_CONDITION | 
                       _40_SCI_SSCL_START_RESTART_STOP_CONDITION;
}

/***********************************************************************************************************************
* Function Name: R_Config_SCI12_IIC_Master_Send
* Description  : This function sends simple IIC(SCI12) data to slave device
* Arguments    : adr -
*                    slave device address
*                tx_buf -
*                    transfer buffer pointer (Not used when receive data handled by DTC or DMAC)
*                tx_num -
*                    buffer size (Not used when receive data handled by DTC or DMAC)
* Return Value : None
***********************************************************************************************************************/

void R_Config_SCI12_IIC_Master_Send(uint8_t adr, uint8_t * const tx_buf, uint16_t tx_num)
{
    if (tx_num < 1U)
    {
        return;
    }

    gp_sci12_tx_address = tx_buf;
    g_sci12_tx_count = tx_num;
    g_sci12_slave_address = adr;
    g_sci12_iic_transmit_receive_flag = _80_SCI_IIC_TRANSMISSION;
    g_sci12_iic_cycle_flag = _80_SCI_IIC_START_CYCLE;

    /* Disable RXI and ERI interrupt requests */
    SCI12.SCR.BIT.RIE = 0U;

    /* Generate start condition */
    R_Config_SCI12_IIC_StartCondition();
}

/***********************************************************************************************************************
* Function Name: R_Config_SCI12_IIC_Master_Receive
* Description  : This function receives simple IIC(SCI12) data from slave device
* Arguments    : adr -
*                    slave device address
*                rx_buf -
*                    receive buffer pointer (Not used when receive data handled by DTC or DMAC)
*                rx_num -
*                    buffer size (Not used when receive data handled by DTC or DMAC)
* Return Value : None
***********************************************************************************************************************/

void R_Config_SCI12_IIC_Master_Receive(uint8_t adr, uint8_t * const rx_buf, uint16_t rx_num)
{
    if (rx_num < 1U)
    {
        return;
    }

    g_sci12_rx_length = rx_num;
    g_sci12_rx_count = 0;
    gp_sci12_rx_address = rx_buf;
    g_sci12_slave_address = adr;
    g_sci12_iic_transmit_receive_flag = _00_SCI_IIC_RECEPTION;
    g_sci12_iic_cycle_flag = _80_SCI_IIC_START_CYCLE;

    /* Disable RXI and ERI interrupt requests */
    SCI12.SCR.BIT.RIE = 0U;

    /* Generate start condition */
    R_Config_SCI12_IIC_StartCondition();
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

