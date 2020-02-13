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
#include "Motor.h"
#include "Timer.h"
#include "MicroSD.h"
#include "I2C_MPU9250.h"

void main(void)
{
	// I2C通信開始
	R_Config_SCI12_Start();
	R_Config_SCI6_Start();
	inti_lcd();
	init_IMU();
	// SPI通信開始
	R_Config_SCI5_Start();
	init_msd();
	//タイマ割り込み開始
	R_Config_CMT0_Start();
	// 位相計数モード計測開始
	R_Config_MTU2_Start();
	// PWM出力開始
	R_Config_MTU0_Start();
	// ブレーキモード
	motor_r_mode(BRAKE,BRAKE);
	// モータ停止
	motor_r(0,0);
	// センサ駆動
	PORTA.PODR.BIT.B3 = 0;
	// A/D変換開始
	R_Config_S12AD0_Start();
	R_Config_S12AD1_Start();
	
	while(1){
		
		if ( cnt0 >= 100 ) {
			cnt0 = 0;
			lcdPosition( 0, 0 );
			lcdPrintf("zg      ");
			lcdPosition( 0, 1 );
			lcdPrintf("%5d   ",rawZg);
		}
		
		// モータ動作確認
		/*
		motor_r(0,0);
		wait_lcd(2000U);
		motor_r_mode(BRAKE,BRAKE);
		motor_r(30,30);
		wait_lcd(2000U);
		motor_r(0,0);
		wait_lcd(2000U);
		motor_r_mode(BRAKE,BRAKE);
		motor_r(60,60);
		wait_lcd(2000U);
		motor_r(0,0);
		wait_lcd(2000U);
		motor_r_mode(FREE,FREE);
		motor_r(100,100);
		wait_lcd(2000U);
		*/
	}
}