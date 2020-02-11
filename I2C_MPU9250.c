//======================================//
// �C���N���[�h                         //
//======================================//
#include "I2C_MPU9250.h"
//====================================//
// �O���[�o���ϐ��̐錾							//
//====================================//
// IMU����擾�����f�[�^
volatile int 	rawXa = 0, rawYa = 0, rawZa = 0;	// �����x(16bit�f�[�^)
volatile int 	rawXg = 0, rawYg = 0, rawZg = 0;	// �p�����x(16bit�f�[�^)

volatile short 	rawTemp;			// ���x(16bit�f�[�^)

// �f�[�^����
double 	TurningAngleIMU;	// IMU���狁�߂����[�p�x
double	RollAngleIMU;		// IMU���狁�߂����[�������p�x
double 	PichAngleIMU;		// IMU���狁�߂��s�b�`�����p�x
double	TempIMU;			// IMU�̉��x
short		offset[3];			// �I�t�Z�b�g�l(16bit)

char	whoami;
char cnt_imu = 0;

///////////////////////////////////////////////////////////////////////////
// ���W���[���� wait_IMU								//
// �����T�v     �x������								//
// ����         �x������(ms)								//
// �߂�l       �Ȃ�									//
///////////////////////////////////////////////////////////////////////////
void wait_IMU ( short waitTime )
{
	volatile int time, i = 0;
	
	time = (int)waitTime * ( CLOCK * 1000 ) / 16;
	for ( i = 0; i < time; i++) __nop();
}
///////////////////////////////////////////////////////////////////////////////////////////////////
// ���W���[���� IMUWriteByte											//
// �����T�v     �w�肵�����W�X�^�Ƀf�[�^����������								//
// ����         slaveaddr:�X���[�u�A�h���X reg:���W�X�^�̃A�h���X data:�������݃f�[�^	//
// �߂�l       �Ȃ�													//
///////////////////////////////////////////////////////////////////////////////////////////////////
void IMUWriteByte( char reg, char data )
{
	uint8_t sendData[2] = { reg, data }, num = 2;
    
	I2C_IMU_COMMAND;		// �R�}���h���M
}
///////////////////////////////////////////////////////////////////////////
// ���W���[���� IMUReadByte							//
// �����T�v     �w�肵�����W�X�^�̃f�[�^��ǂ�				//
// ����         slaveaddr:�X���[�u�A�h���X reg:���W�X�^�̃A�h���X	//
// �߂�l       0:����Ɏ�M�@1:��M�s��						//
///////////////////////////////////////////////////////////////////////////
char IMUReadByte( char reg )
{
	uint8_t sendData[1] = { reg }, num = 1, reciveData[1];
  	
	I2C_IMU_COMMAND;		// �R�}���h���M
	I2C_IMU_RECIVE;		// �f�[�^���M
	
	return reciveData[0];
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���W���[���� IMUReadArry																//
// �����T�v     �w�肵�����W�X�^����w��̐������f�[�^��ǂ�											//
// ����         slaveaddr:�X���[�u�A�h���X addr:���W�X�^�̃A�h���X num:�ǂݎ��f�[�^�� dataArry:�f�[�^�̊i�[��	//
// �߂�l       0:����ɒʐM	1:�ʐM�G���[																		//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void IMUReadArry( char reg, char num2, char* reciveData )
{
	uint8_t sendData[1] = { reg }, num = 1;
	
	
	I2C_IMU_COMMAND;
	num = num2;
	I2C_IMU_ARRY;
}