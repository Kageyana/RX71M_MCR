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
* File Name        : r_smc_cgc.c
* Version          : 1.8.0
* Device(s)        : R5F571MFCxFP
* Description      : This file implements CGC setting.
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
#include "r_smc_cgc.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_CGC_Create
* Description  : This function initializes the clock generator
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_CGC_Create(void)
{
    uint16_t w_count;

    /* Set sub-clock oscillation wait time */
    SYSTEM.SOSCWTCR.BYTE = _21_CGC_SOSCWTCR_VALUE;

    RTC.RCR4.BIT.RCKSEL = 0U;

    /* Stop sub-clock */
    while (0U != RTC.RCR3.BIT.RTCEN)
    {
        RTC.RCR3.BIT.RTCEN = 0U;
    }

    /* Stop sub-clock */
    while (1U != SYSTEM.SOSCCR.BIT.SOSTP)
    {
        SYSTEM.SOSCCR.BIT.SOSTP = 1U;
    }

    while (0U != SYSTEM.OSCOVFSR.BIT.SOOVF)
    {
        /* Wait for sub-clock to be stable */
    }

    /* Set sub-clock drive capacity */
    while (0x06U != RTC.RCR3.BIT.RTCDV)
    {
        RTC.RCR3.BIT.RTCDV = 0x06U;
    }

    /* Wait for the 5 sub-clock cycles */
    for (w_count = 0U; w_count < _21_CGC_SOSCWTCR_VALUE; w_count++)
    {
        nop();
    }

    /* Set sub-clock */
    SYSTEM.SOSCCR.BIT.SOSTP = 0U;

    while (0U != SYSTEM.SOSCCR.BIT.SOSTP)
    {
        /* Wait for the register modification to complete */
    }

    while (1U != SYSTEM.OSCOVFSR.BIT.SOOVF)
    {
        /* Wait for sub-clock to be stable */
    }

    SYSTEM.SOSCWTCR.BYTE = 0x00U;

    R_CGC_Create_UserInit();
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
