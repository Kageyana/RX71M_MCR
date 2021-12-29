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
//#include "MicroSD.h"
#include "IMU_ICM20648.h"

void main(void)
{
	uint8_t flg = 0;
	// UART通信開始
	R_Config_SCI1_Start();
	// I2C通信開始
	R_Config_SCI12_Start();
	inti_lcd();
	// SPI通信開始
	R_Config_SCI2_Start();
	init_IMU();
	//init_msd();
	//タイマ割り込み開始
	R_Config_CMT0_Start();
	// 位相計数モード計測開始
	//R_Config_MTU2_Start();
	// PWM出力開始
	//R_Config_MTU0_Start();
	// ブレーキモード
	//motor_r_mode(BRAKE,BRAKE);
	// モータ停止
	//motor_r(0,0);
	// センサ駆動
	//PORTA.PODR.BIT.B3 = 0;
	// A/D変換開始
	//R_Config_S12AD0_Start();
	printf("Hello\n");
	
	while(1){
		uint8_t ret, test_buff[512];
		uint16_t l, i;
		
		if ( cnt0 >= 100 ) {
			cnt0 = 0;
			lcdPosition( 0, 0 );
			lcdPrintf("zg      ");
			lcdPosition( 0, 1 );
			//lcdPrintf("%d", PORTA.PIDR.BIT.B2);
			lcdPrintf("asfa");
		}
		
		/*if ( PORT1.PIDR.BIT.B3 == 0 && flg == 0) {
			flg = 1;
			
			// microSDイレース
			cnt1 = 0;
			ret = eraseMicroSD( 0x00000, 0x5dc00-1 );
			l = cnt1;
			if (ret == 1) while(1);
			else printf("microSD Erase Time %d[ms]\n",l);
			
			// バッファ作成
			for ( i=0; i<512; i++ ) {
				test_buff[i] = i % 0x100;
			}
			
			// microSD書き込み
			cnt1 = 0;
			ret = writeMicroSD( 0x00000, test_buff );
			l = cnt1;
			if (ret == 1) while(1);
			else printf("microSD Write Time %d[ms]\n",l);
			
			// バッファクリア
			for ( i=0; i<512; i++ ) {
				test_buff[i] =0x00;
			}
			
			// microSD読み込み
			cnt1 = 0;
			ret = readMicroSD( 0x00000, test_buff );
			l = cnt1;
			if (ret == 1) while(1);
			else printf("microSD Read Time %d[ms]\n",l);
			
		}*/
		
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