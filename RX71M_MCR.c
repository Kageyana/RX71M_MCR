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
#include "r_smc_entry.h"  //�X�}�[�g�R���t�B�O���[�^�̃}�j���A���̎g�p��ŌĂ�ł��̂�include
#include "I2c_LCD.h"
#include "Motor.h"
//#include "timer.h"

void main(void)
{
	// I2C�ʐM�J�n
	R_Config_SCI12_Start();
	inti_lcd();
	//�^�C�}���荞�݊J�n
	R_Config_CMT0_Start();
	// �u���[�L���[�h
	motor_r_mode(BRAKE,BRAKE);
	motor_r(0,0);
	
	while(1){
		//lcdPosition( 0, 0 );
		//lcdPrintf("Hello");
		//lcdPosition( 0, 1 );
		//lcdPrintf("RXworld");
		
		PORTE.PODR.BIT.B3 = 1;
		
		// ���[�^����m�F
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