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
* File Name    : Config_SCI1_user.c
* Version      : 1.9.2
* Device(s)    : R5F571MFCxFP
* Description  : This file implements device driver for Config_SCI1.
* Creation Date: 2021-09-01
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
#include "Config_SCI1.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
extern volatile uint8_t * gp_sci1_tx_address;                /* SCI1 transmit buffer address */
extern volatile uint16_t  g_sci1_tx_count;                   /* SCI1 transmit data number */
extern volatile uint8_t * gp_sci1_rx_address;                /* SCI1 receive buffer address */
extern volatile uint16_t  g_sci1_rx_count;                   /* SCI1 receive data number */
extern volatile uint16_t  g_sci1_rx_length;                  /* SCI1 receive data length */
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_SCI1_Create_UserInit
* Description  : This function adds user code after initializing the SCI1 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_SCI1_Create_UserInit(void)
{
    /* Start user code for user init. Do not edit comment generated here */
    PORT2.PMR.BYTE |= 0x40U;
    SCI1.SCR.BIT.TE = 1U;
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_Config_SCI1_transmit_interrupt
* Description  : This function is TXI1 interrupt service routine
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

#if FAST_INTERRUPT_VECTOR == VECT_SCI1_TXI1
#pragma interrupt r_Config_SCI1_transmit_interrupt(vect=VECT(SCI1,TXI1),fint)
#else
#pragma interrupt r_Config_SCI1_transmit_interrupt(vect=VECT(SCI1,TXI1))
#endif
static void r_Config_SCI1_transmit_interrupt(void)
{
    if (0U < g_sci1_tx_count)
    {
        SCI1.TDR = *gp_sci1_tx_address;
        gp_sci1_tx_address++;
        g_sci1_tx_count--;
    }
    else
    {
        SCI1.SCR.BIT.TIE = 0U;
        SCI1.SCR.BIT.TEIE = 1U;
    }
}

/***********************************************************************************************************************
* Function Name: r_Config_SCI1_transmitend_interrupt
* Description  : This function is TEI1 interrupt service routine
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void r_Config_SCI1_transmitend_interrupt(void)
{
    /* Set TXD1 pin */
    PORT2.PMR.BYTE &= 0xBFU;

    SCI1.SCR.BIT.TIE = 0U;
    SCI1.SCR.BIT.TE = 0U;
    SCI1.SCR.BIT.TEIE = 0U;
}

/***********************************************************************************************************************
* Function Name: r_Config_SCI1_receive_interrupt
* Description  : This function is RXI1 interrupt service routine
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

#if FAST_INTERRUPT_VECTOR == VECT_SCI1_RXI1
#pragma interrupt r_Config_SCI1_receive_interrupt(vect=VECT(SCI1,RXI1),fint)
#else
#pragma interrupt r_Config_SCI1_receive_interrupt(vect=VECT(SCI1,RXI1))
#endif
static void r_Config_SCI1_receive_interrupt(void)
{
    if (g_sci1_rx_length > g_sci1_rx_count)
    {
        *gp_sci1_rx_address = SCI1.RDR;
        gp_sci1_rx_address++;
        g_sci1_rx_count++;
    }

    if (g_sci1_rx_length <= g_sci1_rx_count)
    {
        /* All data received */
        SCI1.SCR.BIT.RIE = 0U;
        SCI1.SCR.BIT.RE = 0U;
    }
}

/***********************************************************************************************************************
* Function Name: r_Config_SCI1_receiveerror_interrupt
* Description  : This function is ERI1 interrupt service routine
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void r_Config_SCI1_receiveerror_interrupt(void)
{
    uint8_t err_type;

    /* Clear overrun, framing and parity error flags */
    err_type = SCI1.SSR.BYTE;
    err_type &= 0xC7U;
    err_type |= 0xC0U;
    SCI1.SSR.BYTE = err_type;
}

/* Start user code for adding. Do not edit comment generated here */

/* End user code. Do not edit comment generated here */
