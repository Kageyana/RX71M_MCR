//=====================================
// �C���N���[�h
//=====================================
#include "Motor.h"
//=====================================
// �O���[�o���ϐ��̐錾
//=====================================//
signed char	accele_fR;		// �E�O���[�^�[PWM�l
signed char	accele_fL;		// ���O���[�^�[PWM�l
signed char	accele_rR;		// �E�ヂ�[�^�[PWM�l
signed char	accele_rL;		// ���ヂ�[�^�[PWM�l
signed char	sPwm;		// �T�[�{���[�^�[PWM�l

///////////////////////////////////////////////////////////////////////////
// ���W���[���� motor_mode
// �����T�v     �u���[�L��t���[���[�h�̐؂�ւ�
// ����         acelerL, accelerR(PWM��1�`100%�Ŏw��)
// �߂�l       �Ȃ�
///////////////////////////////////////////////////////////////////////////
void motor_r_mode(char rl, char rr )
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
void motor_r( signed char accelerL, signed char accelerR )
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