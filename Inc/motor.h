#ifndef MOTOR_H
#define MOTOR_H
//====================================//
// インクルード                        						//
//====================================//
#include "r_smc_entry.h"
//====================================//
// シンボル定義									//
//====================================//
#define SERVO_LIMIT		430		// サーボリミットAD値±
#define TGR_MOTOR			5998		// ジェネラルレジスタ初期値(駆動モータ)
#define TGR_SERVO			5998		// ジェネラルレジスタ初期値(サーボ)

#define FORWARD			0		// 正転
#define REVERSE			1		// 逆転
#define BRAKE				0		// ブレーキモード
#define FREE				1		// フリーモード

// 左後輪(pwm3,MTIOC0A)
#define DIR_RL			PORTC.PODR.BIT.B4		// モータ回転方向(0:正転 1:逆転)
#define SR_RL			PORTC.PODR.BIT.B5		// 0:フリーモード  1:ブレーキモード
#define PWM_RL_OUT	MTU0.TGRB = pwmrl		// PWM出力
// 右後輪(pwm4,MTIOC0C)
#define DIR_RR			PORTC.PODR.BIT.B2
#define SR_RR			PORTC.PODR.BIT.B3
#define PWM_RR_OUT	MTU0.TGRD = pwmrr
//====================================//
// グローバル変数の宣言								//
//====================================//
// モーター関連
extern signed char	accele_fR;		// 右前モーターPWM値
extern signed char	accele_fL;		// 左前モーターPWM値
extern signed char	accele_rR;		// 右後モーターPWM値
extern signed char	accele_rL;		// 左後モーターPWM値
extern signed char	sPwm;		// サーボモーターPWM値
//====================================//
// プロトタイプ宣言									//
//====================================//
// モーター関連
void motor_r_mode(char rl, char rr );
void motor_f( signed char accelefL, signed char accelefR );
void motor_r( signed char accelerL, signed char accelerR );

// サーボ関連
void servoPwmOut( signed char pwm );
#endif