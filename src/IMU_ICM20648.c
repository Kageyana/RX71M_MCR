//=====================================
// �C���N���[�h
//=====================================
#include "IMU_ICM20648.h"
//====================================
// �O���[�o���ϐ��̐錾
//====================================
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
char	busIMU = BUS_IMU_FREE;

///////////////////////////////////////////////////////////////////////////
// ���W���[���� wait_IMU
// �����T�v   �x������
// ����         �x������(ms)
// �߂�l       �Ȃ�
///////////////////////////////////////////////////////////////////////////
void wait_IMU ( short waitTime )
{
	volatile int time, i = 0;
	
	time = (int)waitTime * ( CLOCK * 1000 ) / 16;
	for ( i = 0; i < time; i++) __nop();
}
///////////////////////////////////////////////////////////////
// ���W���[���� IMUWriteByte
// �����T�v   �w�肵�����W�X�^�Ƀf�[�^����������
// ����         reg:���W�X�^�̃A�h���X data:�������݃f�[�^
// �߂�l       �Ȃ�
///////////////////////////////////////////////////////////////
void IMUWriteByte( char reg, char data )
{
	uint8_t data_tr[2] = { reg, data }, data_re[ 1 ], numS = 2, numR = 2;
	#if USECOM == 0
		I2C_IMU_COMMAND;		// �R�}���h���M
		busIMU = BUS_IMU_BUSY;
		while(busIMU)__nop();
	#else
		SPI_IMU_CS = 0;
		SPI_IMU_SEND;
		busIMU = BUS_IMU_BUSY;
		while(busIMU)__nop();
		SPI_IMU_CS = 1;
	#endif
}
/////////////////////////////////////////////////////////
// ���W���[���� IMUReadByte
// �����T�v   �w�肵�����W�X�^�̃f�[�^��ǂ�
// ����         reg:���W�X�^�̃A�h���X
// �߂�l       ���W�X�^�̒l
/////////////////////////////////////////////////////////
char IMUReadByte( char reg )
{
	uint8_t data_tr[2] = { reg | 0x80, 0xff }, data_re[2], numS = 2, numR = 2;
  	
	#if USECOM == 0
		I2C_IMU_COMMAND;		// �R�}���h���M
		busIMU = BUS_IMU_BUSY;SPI_IMU_CS = 0;
		while(busIMU)__nop();
		I2C_IMU_RECIVE;		// �f�[�^��M
		busIMU = BUS_IMU_BUSY;
		while(busIMU)__nop();
	# else
		// �A�h���X���M
		SPI_IMU_CS = 0;
		SPI_IMU_SEND;
		busIMU = BUS_IMU_BUSY;
		while(busIMU)__nop();
		SPI_IMU_CS = 1;
	#endif
	
	return data_re[1];
}
///////////////////////////////////////////////////////////////////////////////////////////////////
// ���W���[���� IMUReadArry
// �����T�v   �w�肵�����W�X�^����w��̐������f�[�^��ǂ�
// ����         reg:���W�X�^�̃A�h���X num2 �󂯎��f�[�^�̐� reciveData �擾�f�[�^���i�[����z��
// �߂�l       �Ȃ�
///////////////////////////////////////////////////////////////////////////////////////////////////
void IMUReadArry( char reg, char num2, char* data_re )
{
	uint8_t data_tr[1] = { reg | 0x80 }, numS = 1, numR = num2;
	
	#if USECOM == 0
		I2C_IMU_COMMAND;
		busIMU = BUS_IMU_BUSY;
		while(busIMU)__nop();
		I2C_IMU_ARRY;
		busIMU = BUS_IMU_BUSY;
		while(busIMU)__nop();
	# else
		SPI_IMU_CS = 0;
		SPI_IMU_SEND;
		SPI_IMU_CS = 1;
	#endif
}
///////////////////////////////////////////////////
// ���W���[���� init_IMU
// �����T�v   IMU�̏�����
// ����         �Ȃ�
// �߂�l       �Ȃ�
///////////////////////////////////////////////////
void init_IMU (void)
{
	#if USECOM == 1
		SPI_IMU_CS = 1;
	#endif
	printf("0x%x\n",IMUReadByte(WHO_AM_I));
	if (IMUReadByte(0x0) == 0xE0) {
		IMUWriteByte( PWR_MGMT_1, 0x01);	// �X���[�v���[�h����
		IMUWriteByte( USER_CTRL, 0x10);		// ���X�@�\�����@SPIonly
		IMUWriteByte( REG_BANK_SEL, 0x20);	// USER_BANK2��L����
		IMUWriteByte( GYRO_CONFIG_1, 0x17);	// �p���x�����W�}2000dps DLPF enable DLPFCFG = 2
		IMUWriteByte( ACCEL_CONFIG, 0x06);	// �����x�����W�}16g
		IMUWriteByte( REG_BANK_SEL, 0x00);	// USER_BANK0��L����
	}
}
///////////////////////////////////////////////////
// ���W���[���� IMUProcess
// �����T�v   �Z���Z�f�[�^�̎擾
// ����         �Ȃ�
// �߂�l       �Ȃ�
///////////////////////////////////////////////////
void IMUProcess (void)
{
	char 	axisData[14];	// �p�����x�A���x��8bit�����f�[�^�i�[��
	
	IMUReadArry( GYRO_XOUT_H, 6, axisData);
	rawXg = (short)((axisData[0] << 8 & 0xff00 ) | axisData[1]);
	rawYg = (short)((axisData[2] << 8 & 0xff00 ) | axisData[3]);
	rawZg = (short)((axisData[4] << 8 & 0xff00 ) | axisData[5]);
	
	
}