#ifndef MOTOR_H
#define MOTOR_H
//====================================//
// �C���N���[�h                        						//
//====================================//
#include "r_smc_entry.h"
//====================================//
// �V���{����`									//
//====================================//
#define SERVO_LIMIT		430		// �T�[�{���~�b�gAD�l�}
#define TGR_MOTOR			5998		// �W�F�l�������W�X�^�����l(�쓮���[�^)
#define TGR_SERVO			5998		// �W�F�l�������W�X�^�����l(�T�[�{)

#define FORWARD			0		// ���]
#define REVERSE			1		// �t�]
#define BRAKE				0		// �u���[�L���[�h
#define FREE				1		// �t���[���[�h

// �����(pwm3,MTIOC0A)
#define DIR_RL			PORTC.PODR.BIT.B4		// ���[�^��]����(0:���] 1:�t�])
#define SR_RL			PORTC.PODR.BIT.B5		// 0:�t���[���[�h  1:�u���[�L���[�h
#define PWM_RL_OUT	MTU0.TGRB = pwmrl		// PWM�o��
// �E���(pwm4,MTIOC0C)
#define DIR_RR			PORTC.PODR.BIT.B2
#define SR_RR			PORTC.PODR.BIT.B3
#define PWM_RR_OUT	MTU0.TGRD = pwmrr
//====================================//
// �O���[�o���ϐ��̐錾								//
//====================================//
// ���[�^�[�֘A
extern signed char	accele_fR;		// �E�O���[�^�[PWM�l
extern signed char	accele_fL;		// ���O���[�^�[PWM�l
extern signed char	accele_rR;		// �E�ヂ�[�^�[PWM�l
extern signed char	accele_rL;		// ���ヂ�[�^�[PWM�l
extern signed char	sPwm;		// �T�[�{���[�^�[PWM�l
//====================================//
// �v���g�^�C�v�錾									//
//====================================//
// ���[�^�[�֘A
void motor_r_mode(char rl, char rr );
void motor_f( signed char accelefL, signed char accelefR );
void motor_r( signed char accelerL, signed char accelerR );

// �T�[�{�֘A
void servoPwmOut( signed char pwm );
#endif