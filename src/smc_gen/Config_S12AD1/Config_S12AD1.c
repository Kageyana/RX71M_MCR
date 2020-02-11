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
* File Name    : Config_S12AD1.c
* Version      : 1.8.0
* Device(s)    : R5F571MFCxFP
* Description  : This file implements device driver for Config_S12AD1.
* Creation Date: 2020-02-11
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
#include "Config_S12AD1.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_S12AD1_Create
* Description  : This function initializes the S12AD1 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_S12AD1_Create(void)
{
    /* Cancel S12AD1 module stop state */
    MSTP(S12AD1) = 0U;

    /* Disable and clear S12ADI1 S12CMPI1 interrupt flags */
    S12AD1.ADCSR.BIT.ADIE = 0U;
    S12AD1.ADCMPCR.BIT.CMPIE = 0U;
    IEN(PERIB, INTB192) = 0U;

    /* Set S12AD1 control registers */
    S12AD1.ADDISCR.BYTE = _00_AD_DISCONECT_UNUSED;
    S12AD1.ADCSR.WORD = _0000_AD_SYNCASYNCTRG_DISABLE | _4000_AD_CONTINUOUS_SCAN_MODE | 
                        _1000_AD_SCAN_END_INTERRUPT_ENABLE;
    S12AD1.ADCER.WORD = _0020_AD_AUTO_CLEARING_ENABLE | _0000_AD_RIGHT_ALIGNMENT | _0000_AD_SELFTDIAGST_DISABLE | 
                        _0000_AD_RESOLUTION_12BIT;
    S12AD1.ADADC.BYTE = _03_AD_4_TIME_CONVERSION | _80_AD_AVERAGE_MODE;

    /* Set channels and sampling time */
    S12AD1.ADANSA0.WORD = _0100_AD_AN108_USED | _0200_AD_AN109_USED | _0400_AD_AN110_USED | _0800_AD_AN111_USED | 
                          _1000_AD_AN112_USED | _2000_AD_AN113_USED;
    S12AD1.ADADS0.WORD = _0100_AD_AN108_ADD_USED | _0200_AD_AN109_ADD_USED | _0400_AD_AN110_ADD_USED | 
                         _0800_AD_AN111_ADD_USED | _1000_AD_AN112_ADD_USED | _2000_AD_AN113_ADD_USED;
    S12AD1.ADSSTRL = _FF_AD1_SAMPLING_STATE_L;

    /* Set compare control register */
    S12AD1.ADCMPCR.BYTE = _00_AD_COMPARISON_INTERRUPT_DISABLE | _00_AD_WINDOWFUNCTION_DISABLE;
    S12AD1.ADCMPDR0 = 0x0000U;

    /* Set interrupt and priority level */
    ICU.SLIBR192.BYTE = 0x44U;
    IPR(PERIB, INTB192) = _0F_AD_PRIORITY_LEVEL15;

    /* Set AN108 pin */
    PORTD.PMR.BYTE &= 0xFEU;
    PORTD.PDR.BYTE &= 0xFEU;
    MPC.PD0PFS.BYTE = 0x80U;

    /* Set AN109 pin */
    PORTD.PMR.BYTE &= 0xFDU;
    PORTD.PDR.BYTE &= 0xFDU;
    MPC.PD1PFS.BYTE = 0x80U;

    /* Set AN110 pin */
    PORTD.PMR.BYTE &= 0xFBU;
    PORTD.PDR.BYTE &= 0xFBU;
    MPC.PD2PFS.BYTE = 0x80U;

    /* Set AN111 pin */
    PORTD.PMR.BYTE &= 0xF7U;
    PORTD.PDR.BYTE &= 0xF7U;
    MPC.PD3PFS.BYTE = 0x80U;

    /* Set AN112 pin */
    PORTD.PMR.BYTE &= 0xEFU;
    PORTD.PDR.BYTE &= 0xEFU;
    MPC.PD4PFS.BYTE = 0x80U;

    /* Set AN113 pin */
    PORTD.PMR.BYTE &= 0xDFU;
    PORTD.PDR.BYTE &= 0xDFU;
    MPC.PD5PFS.BYTE = 0x80U;

    R_Config_S12AD1_Create_UserInit();
}

/***********************************************************************************************************************
* Function Name: R_Config_S12AD1_Start
* Description  : This function starts the AD1 converter
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_S12AD1_Start(void)
{
    IR(PERIB, INTB192) = 0U;
    IEN(PERIB, INTB192) = 1U;
    S12AD1.ADCSR.BIT.ADST = 1U;
}

/***********************************************************************************************************************
* Function Name: R_Config_S12AD1_Stop
* Description  : This function stop the AD1 converter
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_S12AD1_Stop(void)
{
    S12AD1.ADCSR.BIT.ADST = 0U;
    IEN(PERIB, INTB192) = 0U;
    IR(PERIB, INTB192) = 0U;
}

/***********************************************************************************************************************
* Function Name: R_Config_S12AD1_Get_ValueResult
* Description  : This function gets result from the AD1 converter
* Arguments    : channel -
*                    channel of data register to be read
*                buffer -
*                    buffer pointer
* Return Value : None
***********************************************************************************************************************/

void R_Config_S12AD1_Get_ValueResult(ad_channel_t channel, uint16_t * const buffer)
{
    if (channel == ADSELFDIAGNOSIS)
    {
        *buffer = (uint16_t)(S12AD1.ADRD.WORD);
    }
    else if (channel == ADCHANNEL0)
    {
        *buffer = (uint16_t)(S12AD1.ADDR0);
    }
    else if (channel == ADCHANNEL1)
    {
        *buffer = (uint16_t)(S12AD1.ADDR1);
    }
    else if (channel == ADCHANNEL2)
    {
        *buffer = (uint16_t)(S12AD1.ADDR2);
    }
    else if (channel == ADCHANNEL3)
    {
        *buffer = (uint16_t)(S12AD1.ADDR3);
    }
    else if (channel == ADCHANNEL4)
    {
        *buffer = (uint16_t)(S12AD1.ADDR4);
    }
    else if (channel == ADCHANNEL5)
    {
        *buffer = (uint16_t)(S12AD1.ADDR5);
    }
    else if (channel == ADCHANNEL6)
    {
        *buffer = (uint16_t)(S12AD1.ADDR6);
    }
    else if (channel == ADCHANNEL7)
    {
        *buffer = (uint16_t)(S12AD1.ADDR7);
    }
    else if (channel == ADCHANNEL8)
    {
        *buffer = (uint16_t)(S12AD1.ADDR8);
    }
    else if (channel == ADCHANNEL9)
    {
        *buffer = (uint16_t)(S12AD1.ADDR9);
    }
    else if (channel == ADCHANNEL10)
    {
        *buffer = (uint16_t)(S12AD1.ADDR10);
    }
    else if (channel == ADCHANNEL11)
    {
        *buffer = (uint16_t)(S12AD1.ADDR11);
    }
    else if (channel == ADCHANNEL12)
    {
        *buffer = (uint16_t)(S12AD1.ADDR12);
    }
    else if (channel == ADCHANNEL13)
    {
        *buffer = (uint16_t)(S12AD1.ADDR13);
    }
    else if (channel == ADCHANNEL14)
    {
        *buffer = (uint16_t)(S12AD1.ADDR14);
    }
    else if (channel == ADCHANNEL15)
    {
        *buffer = (uint16_t)(S12AD1.ADDR15);
    }
    else if (channel == ADCHANNEL16)
    {
        *buffer = (uint16_t)(S12AD1.ADDR16);
    }
    else if (channel == ADCHANNEL17)
    {
        *buffer = (uint16_t)(S12AD1.ADDR17);
    }
    else if (channel == ADCHANNEL18)
    {
        *buffer = (uint16_t)(S12AD1.ADDR18);
    }
    else if (channel == ADCHANNEL19)
    {
        *buffer = (uint16_t)(S12AD1.ADDR19);
    }
    else if (channel == ADCHANNEL20)
    {
        *buffer = (uint16_t)(S12AD1.ADDR20);
    }
    else if (channel == ADTEMPSENSOR)
    {
        *buffer = (uint16_t)(S12AD1.ADTSDR);
    }
    else if (channel == ADINTERREFVOLT)
    {
        *buffer = (uint16_t)(S12AD1.ADOCDR);
    }
    else if (channel == ADDATADUPLICATION)
    {
        *buffer = (uint16_t)(S12AD1.ADDBLDR);
    }
    else if (channel == ADDATADUPLICATIONA)
    {
        *buffer = (uint16_t)(S12AD1.ADDBLDRA);
    }
    else if (channel == ADDATADUPLICATIONB)
    {
        *buffer = (uint16_t)(S12AD1.ADDBLDRB);
    }
    else
    {
        /* Do Nothing */ 
    }
}

/***********************************************************************************************************************
* Function Name: R_Config_S12AD1_Set_CompareValue
* Description  : This function sets reference data for AD1 comparison
* Arguments    : reg_value0 -
*                    reference data 0 for comparison
*                reg_value1 -
*                    reference data 1 for comparison
* Return Value : None
***********************************************************************************************************************/

void R_Config_S12AD1_Set_CompareValue(uint16_t reg_value0, uint16_t reg_value1)
{
    S12AD1.ADCMPDR0 = reg_value0;
    S12AD1.ADCMPDR1 = reg_value1;
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */   

