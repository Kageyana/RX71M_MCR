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
* File Name    : Config_PORT.c
* Version      : 2.0.0
* Device(s)    : R5F571MFCxFP
* Description  : This file implements device driver for Config_PORT.
* Creation Date: 2020-02-09
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
#include "Config_PORT.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_PORT_Create
* Description  : This function initializes the PORT
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_PORT_Create(void)
{
    /* Set PORTE registers */
    PORTE.PODR.BYTE = _00_Pm3_OUTPUT_0 | _00_Pm4_OUTPUT_0 | _00_Pm5_OUTPUT_0 | _00_Pm6_OUTPUT_0 | _00_Pm7_OUTPUT_0;
    PORTE.ODR0.BYTE = _00_Pm3_CMOS_OUTPUT;
    PORTE.ODR1.BYTE = _00_Pm4_CMOS_OUTPUT | _00_Pm5_CMOS_OUTPUT | _00_Pm6_CMOS_OUTPUT | _00_Pm7_CMOS_OUTPUT;
    PORTE.DSCR.BYTE = _00_Pm3_HIDRV_OFF | _00_Pm4_HIDRV_OFF | _00_Pm5_HIDRV_OFF | _00_Pm6_HIDRV_OFF | _00_Pm7_HIDRV_OFF;
    PORTE.PMR.BYTE = _00_Pm3_PIN_GPIO | _00_Pm4_PIN_GPIO | _00_Pm5_PIN_GPIO | _00_Pm6_PIN_GPIO | _00_Pm7_PIN_GPIO;
    PORTE.PDR.BYTE = _08_Pm3_MODE_OUTPUT | _10_Pm4_MODE_OUTPUT | _20_Pm5_MODE_OUTPUT | _40_Pm6_MODE_OUTPUT | 
                     _80_Pm7_MODE_OUTPUT;

    R_Config_PORT_Create_UserInit();
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */   
