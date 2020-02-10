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
* File Name    : Config_MTU0.c
* Version      : 1.8.0
* Device(s)    : R5F571MFCxFP
* Description  : This file implements device driver for Config_MTU0.
* Creation Date: 2020-02-10
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
#include "Config_MTU0.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_MTU0_Create
* Description  : This function initializes the MTU0 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_MTU0_Create(void)
{
    /* Release MTU channel 0 from stop state */
    MSTP(MTU0) = 0U;

    /* Stop MTU channel 0 counter */
    MTU.TSTRA.BIT.CST0 = 0U;

    /* MTU channel 0 is used as PWM mode 1 */
    MTU.TSYRA.BIT.SYNC0 = 0U;
    MTU0.TCR.BYTE = _00_MTU_PCLK_1 | _20_MTU_CKCL_A;
    MTU0.TCR2.BYTE = _00_MTU_PCLK_1;
    MTU0.TIER.BYTE = _00_MTU_TGIEA_DISABLE | _00_MTU_TGIEB_DISABLE | _00_MTU_TGIEC_DISABLE | _00_MTU_TGIED_DISABLE | 
                     _00_MTU_TCIEV_DISABLE | _00_MTU_TTGE_DISABLE;
    MTU0.TIER2.BYTE = _00_MTU_TGIEE_DISABLE | _00_MTU_TGIEF_DISABLE | _00_MTU_TTGE2_DISABLE;
    MTU0.TMDR1.BYTE = _02_MTU_PWM1;
    MTU0.TIORH.BYTE = _02_MTU_IOA_LH | _60_MTU_IOB_HH;
    MTU0.TIORL.BYTE = _02_MTU_IOC_LH | _60_MTU_IOD_HH;
    MTU0.TGRA = _176F_TGRA0_VALUE;
    MTU0.TGRB = _0000_TGRB0_VALUE;
    MTU0.TGRC = _176F_TGRC0_VALUE;
    MTU0.TGRD = _0000_TGRD0_VALUE;
    MTU0.TGRE = _0064_TGRE0_VALUE;
    MTU0.TGRF = _0064_TGRF0_VALUE;

    /* Set MTIOC0A pin */
    MPC.PB3PFS.BYTE = 0x01U;
    PORTB.PMR.BYTE |= 0x08U;

    /* Set MTIOC0C pin */
    MPC.PB1PFS.BYTE = 0x01U;
    PORTB.PMR.BYTE |= 0x02U;

    R_Config_MTU0_Create_UserInit();
}

/***********************************************************************************************************************
* Function Name: R_Config_MTU0_Start
* Description  : This function starts the MTU0 channel counter
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_MTU0_Start(void)
{
    /* Start MTU channel 0 counter */
    MTU.TSTRA.BIT.CST0 = 1U;
}

/***********************************************************************************************************************
* Function Name: R_Config_MTU0_Stop
* Description  : This function stops the MTU0 channel counter
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_MTU0_Stop(void)
{
    /* Stop MTU channel 0 counter */
    MTU.TSTRA.BIT.CST0 = 0U;
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */   
