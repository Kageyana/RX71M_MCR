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
                            //これ呼んだら周辺機器ｽﾀｰﾄ関数のwarning消えた
char a = 0;
void main(void)
{   
    //周辺機器ｽﾀｰﾄ
    R_Config_CMT0_Start();
    //R_Config_S12AD0_Start();
    //R_Config_S12AD1_Start();
    
    //Config_なんとかかんとか_user.c内にUserInit関数あり、設定したいときとかは関数作ってそこで呼べばよさそう
    
    while(1){
        //PORTE.PODR.BYTE = 0x00;とかでも動くはず
	PORTE.PODR.BIT.B3 = 1;
	PORTE.PODR.BIT.B4 = 1;
	PORTE.PODR.BIT.B5 = 1;
	PORTE.PODR.BIT.B6 = 1;
	//PORTE.PODR.BIT.B7 = 1;
    }
}

void r_config_CMT0_cmi0_interrupt(void) {
	if (a==1) {
		PORTE.PODR.BIT.B7 = 1;
		a = 0;
	} else {
		PORTE.PODR.BIT.B7 = 0;
		a=1;
	}
}