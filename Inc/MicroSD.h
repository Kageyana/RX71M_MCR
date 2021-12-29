#ifndef MICROSD_H_
#define MICROSD_H_
//======================================//
// インクルード                         //
//======================================//
#include "r_smc_entry.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
//========================================//
// シンボル定義										//
//=======================================//
#define WRITINGTIME		2		// ログ取得周期(ms)
#define DATA_BYTE			32		// 一回に保存するデータ数(byte)
#define RECODTIME			30000	// 記録時間(ms)
#define MSD_STARTADDRESS	0		// 開始アドレス
#define MSD_ENDADDRESS	960000	// 終了アドレス　RECODTIME * DATA_BYTE / WRITINGTIME

#define BUS_MSD_FREE 			0		// 通信可能
#define BUS_MSD_BUSY 			1		// 通信中

/******************************** 自動生成関数 ************************************/
// CS端子
//#define MSD_CS_TERMINAL_HIGH	R_PG_IO_PORT_Write_PA0( 1 );	// HIGHT
//#define MSD_CS_TERMINAL_LOW	R_PG_IO_PORT_Write_PA0( 0 );	// LOW

#define MSD_CS			PORTA.PODR.BIT.B3	// CS端子
#define CS_HIGH			1	// HIGH
#define CS_LOW			0	// LOW

// SPI
#define SPI_SEND			R_Config_SCI5_SPI_Master_Send_Receive( data_tr, 1, data_re, 1)

// SDスイッチ
#define GET_SDSWITCH		PORTA.PIDR.BIT.B5
#define SLOTIN				0	// SDカードあり
#define SLOTOUT			1	// SDカード無し
/*******************************************************************************/
//========================================//
// グローバル変数の宣言								//
//=======================================//
// タイマ関連
extern unsigned short		cnt_log;		// ログ漏れ確認用カウント

// microSD関連
extern signed char		msdBuff[ 512 ];        // 一時保存バッファ
extern short			msdBuffaddress;       // 一時記録バッファ書込アドレス
extern short			msdFlag;                	// 1:データ記録 0:記録しない
extern short			msdTimer;               	// 取得間隔計算用
extern unsigned int		msdStartaddress;      // 記録開始アドレス
extern unsigned int		msdEndaddress;        // 記録終了アドレス
extern unsigned int		msdWorkaddress;      // 作業用アドレス
extern unsigned int		msdWorkaddress2;	// 作業用アドレス2
extern volatile char		msdlibError;		// エラー番号
extern signed char 		*msdBuffPointa;		// RAM保存バッファ用ポインタ

extern volatile short			msdlibCnt;
extern volatile unsigned char	interrupt_msd_send_data;	// 送信フラグ

extern unsigned int 			msdaddrBuff[25];	// MicroSDカードの最終書き込みアドレス保存用

extern char			busMSD;
//========================================//
// プロトタイプ宣言									//
//=======================================//
void msd_write( unsigned char data );
unsigned char msd_read( void );
unsigned char msd_CMD ( unsigned char cmd, unsigned char arg1, unsigned char arg2,
			unsigned char arg3, unsigned char arg4, unsigned char crc );
char init_msd ( void );
void init_log ( void );
char getMicroSD_CSD( volatile unsigned char *p );
char readMicroSD ( unsigned int address, signed char *read );
char writeMicroSD ( unsigned int address, signed char *write );
char eraseMicroSD( unsigned int st_address, unsigned int ed_address );
char setMicroSDdata( signed char *p );
char microSDProcessStart( unsigned int address );
char microSDProcessEnd( void );
void microSDProcess( void );
char checkMicroSDProcess( void );
void sendLog (char c, char s, char i, ...);
char msdEndLog ( void );
void msd_send_data (void);
void msdgetData ( void ) ;
void send_Char ( char data );
void send_ShortToChar ( short data );
void send_uIntToChar ( unsigned int data );
short CharToShort( unsigned char offsetaddress );
unsigned int CharTouInt( unsigned char offsetaddress );
void msd_sendToPC ( void );

#endif /* MICROSD_H_ */