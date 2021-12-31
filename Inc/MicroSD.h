#ifndef MICROSD_H_
#define MICROSD_H_
//======================================
// �C���N���[�h
//======================================
#include "r_smc_entry.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
//=======================================
// �V���{����`	
//=======================================
#define WRITINGTIME			2		// ���O�擾����(ms)
#define DATA_BYTE			32		// ���ɕۑ�����f�[�^��(byte)
#define RECODTIME			30000	// �L�^����(ms)
#define MSD_STARTADDRESS	0		// �J�n�A�h���X
#define MSD_ENDADDRESS		960000	// �I���A�h���X�@RECODTIME * DATA_BYTE / WRITINGTIME

#define BUS_MSD_FREE 		0		// �ʐM�\
#define BUS_MSD_BUSY 		1		// �ʐM��

/******************************** ���������֐� ************************************/
// CS�[�q
//#define MSD_CS_TERMINAL_HIGH	R_PG_IO_PORT_Write_PA0( 1 );	// HIGHT
//#define MSD_CS_TERMINAL_LOW	R_PG_IO_PORT_Write_PA0( 0 );	// LOW

#define MSD_CS			PORTA.PODR.BIT.B3	// CS�[�q
#define CS_HIGH			1	// HIGH
#define CS_LOW			0	// LOW

// SPI
#define SPI_SEND		R_Config_SCI5_SPI_Master_Send_Receive( data_tr, 1, data_re, 1)

// SD�X�C�b�`
#define GET_SDSWITCH	PORTA.PIDR.BIT.B5
#define SLOTIN			0	// SD�J�[�h����
#define SLOTOUT			1	// SD�J�[�h����
/***********************************************************************************/
//========================================
// �O���[�o���ϐ��̐錾
//========================================
// �^�C�}�֘A
extern unsigned short		cnt_log;		// ���O�R��m�F�p�J�E���g

// microSD�֘A
extern int8_t			msdBuff[ 512 ];			// �ꎞ�ۑ��o�b�t�@
extern int16_t			msdBuffaddress;			// �ꎞ�L�^�o�b�t�@�����A�h���X
extern int16_t			msdFlag;				// 1:�f�[�^�L�^ 0:�L�^���Ȃ�
extern int16_t			msdTimer;				// �擾�Ԋu�v�Z�p
extern uint32_t			msdStartaddress;		// �L�^�J�n�A�h���X
extern uint32_t			msdEndaddress;			// �L�^�I���A�h���X
extern uint32_t			msdWorkaddress;			// ��Ɨp�A�h���X
extern uint32_t			msdWorkaddress2;		// ��Ɨp�A�h���X2
extern volatile uint8_t	msdlibError;			// �G���[�ԍ�
extern int8_t 			*msdBuffPointa;			// RAM�ۑ��o�b�t�@�p�|�C���^

extern volatile int16_t	msdlibCnt;
extern volatile uint8_t	interrupt_msd_send_data;	// ���M�t���O

extern uint32_t			msdaddrBuff[25];	// MicroSD�J�[�h�̍ŏI�������݃A�h���X�ۑ��p

extern uint8_t			busMSD;
//========================================
// �v���g�^�C�v�錾
//========================================
void msd_write( uint8_t data );
uint8_t msd_read( void );
uint8_t msd_CMD ( uint8_t cmd, uint8_t arg1, uint8_t arg2,
			uint8_t arg3, uint8_t arg4, uint8_t crc );
uint8_t init_msd ( void );
void init_log ( void );
uint8_t getMicroSD_CSD( volatile uint8_t *p );
uint8_t readMicroSD ( uint32_t address, int8_t *read );
uint8_t writeMicroSD ( uint32_t address, int8_t *write );
uint8_t eraseMicroSD( uint32_t st_address, uint32_t ed_address );
uint8_t setMicroSDdata( int8_t *p );
uint8_t microSDProcessStart( uint32_t address );
uint8_t microSDProcessEnd( void );
void microSDProcess( void );
uint8_t checkMicroSDProcess( void );
void sendLog (uint8_t c, uint8_t s, uint8_t i, ...);
uint8_t msdEndLog ( void );
void msd_send_data (void);
void msdgetData ( void ) ;
void send_Char ( uint8_t data );
void send_ShortToChar ( int16_t data );
void send_uIntToChar ( uint32_t data );
int16_t CharToShort( uint8_t offsetaddress );
uint32_t CharTouInt( uint8_t offsetaddress );
void msd_sendToPC ( void );

#endif /* MICROSD_H_ */