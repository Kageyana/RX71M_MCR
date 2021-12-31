#ifndef MICROSD_H_
#define MICROSD_H_
//======================================
// インクルード
//======================================
#include "r_smc_entry.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
//=======================================
// シンボル定義	
//=======================================
#define WRITINGTIME			2		// ログ取得周期(ms)
#define DATA_BYTE			32		// 一回に保存するデータ数(byte)
#define RECODTIME			30000	// 記録時間(ms)
#define MSD_STARTADDRESS	0		// 開始アドレス
#define MSD_ENDADDRESS		960000	// 終了アドレス　RECODTIME * DATA_BYTE / WRITINGTIME

#define BUS_MSD_FREE 		0		// 通信可能
#define BUS_MSD_BUSY 		1		// 通信中

/******************************** 自動生成関数 ************************************/
// CS端子
//#define MSD_CS_TERMINAL_HIGH	R_PG_IO_PORT_Write_PA0( 1 );	// HIGHT
//#define MSD_CS_TERMINAL_LOW	R_PG_IO_PORT_Write_PA0( 0 );	// LOW

#define MSD_CS			PORTA.PODR.BIT.B3	// CS端子
#define CS_HIGH			1	// HIGH
#define CS_LOW			0	// LOW

// SPI
#define SPI_SEND		R_Config_SCI5_SPI_Master_Send_Receive( data_tr, 1, data_re, 1)

// SDスイッチ
#define GET_SDSWITCH	PORTA.PIDR.BIT.B5
#define SLOTIN			0	// SDカードあり
#define SLOTOUT			1	// SDカード無し
/***********************************************************************************/
//========================================
// グローバル変数の宣言
//========================================
// タイマ関連
extern unsigned short		cnt_log;		// ログ漏れ確認用カウント

// microSD関連
extern int8_t			msdBuff[ 512 ];			// 一時保存バッファ
extern int16_t			msdBuffaddress;			// 一時記録バッファ書込アドレス
extern int16_t			msdFlag;				// 1:データ記録 0:記録しない
extern int16_t			msdTimer;				// 取得間隔計算用
extern uint32_t			msdStartaddress;		// 記録開始アドレス
extern uint32_t			msdEndaddress;			// 記録終了アドレス
extern uint32_t			msdWorkaddress;			// 作業用アドレス
extern uint32_t			msdWorkaddress2;		// 作業用アドレス2
extern volatile uint8_t	msdlibError;			// エラー番号
extern int8_t 			*msdBuffPointa;			// RAM保存バッファ用ポインタ

extern volatile int16_t	msdlibCnt;
extern volatile uint8_t	interrupt_msd_send_data;	// 送信フラグ

extern uint32_t			msdaddrBuff[25];	// MicroSDカードの最終書き込みアドレス保存用

extern uint8_t			busMSD;
//========================================
// プロトタイプ宣言
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