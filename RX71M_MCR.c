/***********************************************************************/
/*                                                                     */
/*  FILE        :Main.c or Main.cpp                                    */
/*  DATE        :Tue, Oct 31, 2006                                     */
/*  DESCRIPTION :Main Program                                          */
/*  CPU TYPE    :                                                      */
/*                                                                     */
/*  NOTE:THIS IS A TYPICAL EXAMPLE.                                    */
/*                                                                     */
/***********************************************************************/
//#include "typedefine.h"
#include "r_smc_entry.h"  //スマートコンフィグレータのマニュアルの使用例で呼んでたのでinclude
#include "I2c_LCD.h"
#include "timer.h"

void main(void)
{   
	//R_Config_CMT1_Start();
	
	// I2C通信開始
	R_Config_SCI1_Start();
	R_Config_SCI12_Start();
	inti_lcd();
	//タイマ割り込み開始
	R_Config_CMT0_Start();
	
	
	
	//R_Config_SCI1_Serial_Send(data, 2U);
	//R_Config_SCI1_Serial_Send(data, 2U);
	
	while(1){
		lcdPosition( 0, 0 );
		lcdPrintf("Hello");
		lcdPosition( 0, 1 );
		lcdPrintf("RXworld");
		
		PORTE.PODR.BIT.B3 = 1;
	}
}