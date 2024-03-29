//=====================================
// インクルード
//=====================================
#include "Motor.h"
//=====================================
// グローバル変数の宣言
//=====================================//
int8_t	accele_fR;		// 右前モーターPWM値
int8_t	accele_fL;		// 左前モーターPWM値
int8_t	accele_rR;		// 右後モーターPWM値
int8_t	accele_rL;		// 左後モーターPWM値
int8_t	sPwm;		// サーボモーターPWM値

///////////////////////////////////////////////////////////////////////////
// モジュール名 motor_mode
// 処理概要     ブレーキ､フリーモードの切り替え
// 引数         acelerL, accelerR(PWMを1〜100%で指定)
// 戻り値       なし
///////////////////////////////////////////////////////////////////////////
void motor_r_mode(uint8_t rl, uint8_t rr )
{
	SR_RL = rl;
	SR_RR = rr;
}
///////////////////////////////////////////////////////////////////////////
// モジュール名 motor_r
// 処理概要     モーターのPWMの変更
// 引数         acelerL, accelerR(PWMを1〜100%で指定)
// 戻り値       なし
///////////////////////////////////////////////////////////////////////////
void motor_r( int8_t accelerL, int8_t accelerR )
{
	uint16_t pwmrl, pwmrr;
	/*
	if ( pushcart_mode ) {
		accelerL = 0;
		accelerR = 0;
	}
	*/
	accele_rR = accelerR;
	accele_rL = accelerL;
	
	pwmrl = TGR_MOTOR * accelerL / 100;
	pwmrr = TGR_MOTOR * accelerR / 100;
	
	// 左後輪
	if( accelerL >= 0 ) {					
		// 正転
		DIR_RL = FORWARD;
	} else {						
		// 逆転
		pwmrl = -pwmrl;
		DIR_RL = REVERSE;
	}
	PWM_RL_OUT;
	
	// 右後輪
	if( accelerR >= 0 ) {					
		// 正転
		DIR_RR = FORWARD;
	} else {						
		// 逆転
		pwmrr = -pwmrr;
		DIR_RR = REVERSE;
	}
	PWM_RR_OUT;
}