#ifndef ROTARYENCODER_H
#define ROTARYENCODER_H
//====================================//
// �C���N���[�h                        						//
//====================================//
#include "r_smc_entry.h"
//====================================//
// �V���{����`									//
//====================================//

//====================================//
// �O���[�o���ϐ��̐錾								//
//====================================//
extern unsigned int	EncoderTotal;	// �����s����
extern signed short	Encoder;		// 1ms���Ƃ̃p���X
extern unsigned int	enc1;		// ���s�p�����J�E���g
extern unsigned int	enc_slope;		// ��㋗���J�E���g
//====================================//
// �v���g�^�C�v�錾									//
//====================================//
void getEncoder (void);

#endif