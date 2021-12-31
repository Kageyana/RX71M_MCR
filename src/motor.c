//=====================================
// �C���N���[�h
//=====================================
#include "Motor.h"
//=====================================
// �O���[�o���ϐ��̐錾
//=====================================//
int8_t	accele_fR;		// �E�O���[�^�[PWM�l
int8_t	accele_fL;		// ���O���[�^�[PWM�l
int8_t	accele_rR;		// �E�ヂ�[�^�[PWM�l
int8_t	accele_rL;		// ���ヂ�[�^�[PWM�l
int8_t	sPwm;		// �T�[�{���[�^�[PWM�l

///////////////////////////////////////////////////////////////////////////
// ���W���[���� motor_mode
// �����T�v     �u���[�L��t���[���[�h�̐؂�ւ�
// ����         acelerL, accelerR(PWM��1�`100%�Ŏw��)
// �߂�l       �Ȃ�
///////////////////////////////////////////////////////////////////////////
void motor_r_mode(uint8_t rl, uint8_t rr )
{
	SR_RL = rl;
	SR_RR = rr;
}
///////////////////////////////////////////////////////////////////////////
// ���W���[���� motor_r
// �����T�v     ���[�^�[��PWM�̕ύX
// ����         acelerL, accelerR(PWM��1�`100%�Ŏw��)
// �߂�l       �Ȃ�
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
	
	// �����
	if( accelerL >= 0 ) {					
		// ���]
		DIR_RL = FORWARD;
	} else {						
		// �t�]
		pwmrl = -pwmrl;
		DIR_RL = REVERSE;
	}
	PWM_RL_OUT;
	
	// �E���
	if( accelerR >= 0 ) {					
		// ���]
		DIR_RR = FORWARD;
	} else {						
		// �t�]
		pwmrr = -pwmrr;
		DIR_RR = REVERSE;
	}
	PWM_RR_OUT;
}