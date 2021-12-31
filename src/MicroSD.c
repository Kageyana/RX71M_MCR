//====================================
// インクルード
//====================================
#include "MicroSD.h"
//====================================
// グローバル変数の宣言
//====================================
// タイマ関連
uint16_t 			cnt_log;				// ログ漏れ確認用カウント

static uint8_t				msdlibBuff[512 + 128];	// 一時保管バッファ
static volatile int16_t		msdlibMode;				// 状態
volatile int16_t			msdlibCnt;				// 書き込み数
static volatile uint8_t*	msdlibWrite;			// 書き込みデータのアドレス
volatile uint8_t			msdlibError;			// エラー番号
static volatile uint32_t	msdSize;				// microSDのサイズ(kB単位)
static volatile int16_t		modeSector;				// 1:セクタ指定モード 0:アドレス
volatile uint8_t			interrupt_msd_send_data = 0;	// 送信フラグ

// microSD関連
int8_t			msdBuff[ 512 ];		// 一時保存バッファ
int16_t			msdBuffaddress;		// 一時記録バッファ書込アドレス
int16_t			msdFlag = 0;		// 1:データ記録 0:記録しない
int16_t			msdTimer;			// 取得間隔計算用
uint32_t		msdStartaddress;	// 記録開始アドレス
uint32_t		msdEndaddress;		// 記録終了アドレス
uint32_t		msdWorkaddress;		// 作業用アドレス
uint32_t		msdWorkaddress2;	// 作業用アドレス2
int8_t 			*msdBuffPointa;		// RAM保存バッファ用ポインタ
uint32_t 		msdaddrBuff[25];	// MicroSDカードの最終書き込みアドレス保存用

uint8_t			busMSD = BUS_MSD_FREE;
                                        
///////////////////////////////////////////////////////////////////////////
// モジュール名 msd_write
// 処理概要     microSD 1バイト書き込み
// 引数         char データ
// 戻り値       なし
///////////////////////////////////////////////////////////////////////////
void msd_write( uint8_t data )
{
	uint8_t data_tr[] = { data }, data_re[ 1 ];
	
	SPI_SEND;
	busMSD = BUS_MSD_BUSY;
	while(busMSD)__nop();
}
///////////////////////////////////////////////////////////////////////////
// モジュール名 msd_read
// 処理概要     microSD 1バイト読み込み
// 引数         なし
// 戻り値       char データ
///////////////////////////////////////////////////////////////////////////
uint8_t msd_read( void )
{
	uint8_t data_tr[] = { 0xff }, data_re[ 1 ] = { 0x00 }, ret;
	volatile int16_t data;
	
	SPI_SEND;
	busMSD = BUS_MSD_BUSY;
	while(busMSD)__nop();
	
	data = data_re[0];
	ret = data & 0x00ff;
	
	return  ret;
}
///////////////////////////////////////////////////////////////////////////
// モジュール名 msd_CMD
// 処理概要     コマンド送信
// 引数         コマンド、引数1,引数2,引数3,引数4,CRC
// 戻り値       microSDからの戻り値
///////////////////////////////////////////////////////////////////////////
uint8_t msd_CMD ( uint8_t cmd, uint8_t arg1, uint8_t arg2,
			uint8_t arg3, uint8_t arg4, uint8_t crc )
{
	volatile uint8_t ret;
	volatile uint16_t cnt_retry;
	
	msd_write ( 0xff );
	
	msd_write ( cmd );
	msd_write ( arg1 );
	msd_write ( arg2 );
	msd_write ( arg3 );
	msd_write ( arg4 );
	msd_write ( crc );
	
	// レスポンスが返ってくるまでまつ
	cnt_retry = 1500;
	while ( --cnt_retry ) {
		MSD_CS = CS_HIGH;		// CS端子をHIGHにする
		MSD_CS = CS_LOW;		// CS端子をLOWにする
		
		ret = msd_read();
		if ( ret != 0xff ) break;	// 0xff以外で終了
	}
	return ret;
}
///////////////////////////////////////////////////////////////////////////
// モジュール名 init_msd
// 処理概要     MicroSDの初期化
// 引数         なし
// 戻り値       0:初期化成功	1以上:初期化失敗
///////////////////////////////////////////////////////////////////////////
uint8_t init_msd ( void )
{
	uint8_t sd_sw;
	volatile uint8_t pattern_intimsd = 1, ret = 0, response_register[ 16 ];
	volatile int16_t receive, i;
	
	while ( ret == 0 && pattern_intimsd <= 8 ) {
		sd_sw = GET_SDSWITCH;
		if ( sd_sw == SLOTIN ) {
			switch ( pattern_intimsd ) {
				case 1:
					MSD_CS = CS_HIGH;			// CS端子をHIGHにする
					
					for ( i = 0; i < 10; i++) {
						receive = msd_read();		// 80クロック(ダミーデータ10バイト)を送る
					}
					
					printf("Dumydata send\n");
					pattern_intimsd = 2;
					break;
					
				case 2:
					MSD_CS = CS_LOW;			// CS端子をLOWにする
					
					i = 0;
					do {
						// CMD0送信
						receive = msd_CMD( 0x40, 0x00, 0x00, 0x00, 0x00, 0x95 );
						
						if ( i >= 10 ) {
							ret = 3;
							printf("CMD0 error\n");
							break;
						}
						i++;
					} while ( ( receive & 0xfe ) != 0x0 );	// 0x01を受信すればSDカードはアイドル状態
					
					pattern_intimsd = 3;
					break;
					
				case 3:
					printf("CMD0 send\n");
					i = 0;
					do {
						// CMD8送信
						receive = msd_CMD( 0x48, 0x00, 0x00, 0x01, 0xaa, 0x87 );
						
						i++;
						if ( i >= 20 ) {
							ret = 4;
							printf("CMD8 error\n");
							break;
						}
					} while ( receive == 0xff );
						
					// R7レスポンス残りを取得
					for ( i = 0; i < 5; i++ ) {
						response_register[ i ] = msd_read();
					}
					
					if ( response_register[ 2 ] != 0x01 || response_register[ 3 ] != 0xaa ) {
						ret = 4;
						printf("CMD8 error\n");
						break;
					}
					
					pattern_intimsd = 4;
					break;
						
				case 4:
					printf("CMD8 send\n");
					i = 0;
					do {
						// CMD58送信
						receive = msd_CMD( 0x7a, 0x40, 0x00, 0x00, 0x00, 0xaf );
						
						i++;
						if ( i > 50 ) {
							ret = 5;
							printf("CMD58 error\n");
							break;
						}
					} while ( receive != 0x01 );
						
					// R3レスポンス残りを取得
					for ( i = 0; i < 6; i++ ) {
						response_register[ i ] = msd_read();
					}
					
					pattern_intimsd = 5;
					break;
						
				case 5:
					printf("CMD58 send\n");
					i = 0;
					do {
						// CMD55送信
						receive = msd_CMD( 0x77, 0x00, 0x00, 0x00, 0x00, 0x65 );
						// ACMD41送信
						receive = msd_CMD( 0x69, 0x40, 0x00, 0x00, 0x00, 0x77 );
						
						i++;
						if ( i > 1000 ) {
							ret = 6;
							printf("ACMD41 error\n");
							break;
						}
					} while ( receive != 0x00 );
					
					pattern_intimsd = 6;
					break;
						
				case 6:
					printf("ACMD41 send\n");
					i = 0;
					do {
						// CMD58送信
						receive = msd_CMD( 0x7a, 0x40, 0x00, 0x00, 0x00, 0xaf );
						
						if ( i >= 10 ) {
							ret = 7;
							printf("CMD58 error\n");
							break;
						}
						i++;
					} while ( receive != 0x00 );
						
					// R3レスポンス残りを取得
					for ( i = 0; i < 6; i++ ) {
						response_register[ i ] = msd_read();
					}
						
					if ( response_register[ 0 ] & 0x40 ) {
						printf("SDHCcard\n");
					} else {
						printf("SDcard\n");
					}
					pattern_intimsd = 7;
					break;
						
				case 7:
					printf("CMD58 send\n");
					// CSD取得
					receive = getMicroSD_CSD( response_register );
					if ( receive == 1 ) {
						ret = 8;
						printf("CSD error\n");
						break;
					}
					// 容量計算
					// SDHCの場合
					msdSize   = ( ( unsigned short )response_register[7]&0x3f) << 16;
					msdSize  |=  ( unsigned short )response_register[8] << 8;
					msdSize  |=  ( unsigned short )response_register[9];
					msdSize  += 1;
					msdSize <<= 9;
					printf("%dkB\n", msdSize);
					pattern_intimsd = 8;
					break;
						
				case 8:
					MSD_CS = CS_HIGH;		// CS端子をHIGHにする
					pattern_intimsd = 9;
					break;
			}
		} else {
			ret = 1;
			break;
		}
	}
	
	if ( ret == 0 ) {
		printf("SDcard Initialized\n");
	} else if ( ret == 1 ) {
		printf("No slot card\n");
	}
	
	return ret;
}
///////////////////////////////////////////////////////////////////////////
// モジュール名 getMicroSD_CSD
// 処理概要     microSD Card Specific Data取得
// 引数         *p *読み込み配列(16バイト以上)
// 戻り値       0:正常 1以上:エラー
///////////////////////////////////////////////////////////////////////////
uint8_t getMicroSD_CSD( volatile uint8_t *p )
{
	volatile uint8_t ret = 0;
	volatile int16_t receive, i;
	
	ret = 0;
	MSD_CS = CS_LOW;			// CS端子をLOWにする
	
	// CMD9送信
	receive = msd_CMD( 0x49, 0x00, 0x00, 0x00, 0x00, 0xaf );
	
	i = 0;
	do {
		receive = msd_read();
		
		if ( i > 1000 ) {
			ret = 1;
			break;
		}
		i++;
	} while ( receive & 0x80 );
	
	*p++ = receive;
	
	// CSD読み込み
	for ( i = 1; i < 16; i++ ) {
		*p++ = msd_read();
	}
	
	// ダミーリード
	msd_read();
	msd_read();
	
	// ダミークロック送信
	msd_write( 0xff );
	MSD_CS = CS_HIGH;			// CS端子をHIGHにする
	return ret;
}
///////////////////////////////////////////////////////////////////////////
// モジュール名 readMicroSD
// 処理概要     microSDからデータ読み込み(512バイト)
// 引数         address: MicroSDのアドレス	*read: *読み込んだデータを格納する配列のアドレス
// 戻り値       0:正常 1以上:エラー
///////////////////////////////////////////////////////////////////////////
uint8_t readMicroSD ( uint32_t address, int8_t *read )
{
	volatile uint16_t	i, receive;
	volatile uint8_t	a1, a2, a3, a4, pattern_msd_read = 1, ret = 0;
	
	if( msdlibMode != 0 ) {             // 別な作業中ならエラー
		return 1;
	}
	msdlibMode = 1;
	
	// SDHCならアドレス→セクタ番号に変換
	address >>= 9;
	
	a1 = ( unsigned char )( ( address&0xff000000 ) >> 24 );
	a2 = ( unsigned char )( ( address&0x00ff0000 ) >> 16 );
	a3 = ( unsigned char )( ( address&0x0000ff00 ) >>  8 );
	a4 = ( unsigned char )(  address&0x000000ff       );
	
	while ( ret < 1 && pattern_msd_read <= 3 ) {
		switch ( pattern_msd_read ) {
			case 1:
				MSD_CS = CS_HIGH;		// CS端子をHIGHにする
				msd_write( 0xff );		// ダミークロック送信
				MSD_CS = CS_LOW;		// CS端子をLOWにする
				msd_write( 0xff );		// ダミークロック送信
				pattern_msd_read = 2;
				break;
			
			case 2:
				// CMD17送信
				receive = msd_CMD( 0x51, a1, a2, a3, a4, 0xff );
				
				// レスポンス確認
				if ( receive != 0x00 ) {
					ret = 1;
					break;
				}
				// 0xfeか確認
				i = 0;
				do {
					receive = msd_read();
					i++;
					if ( i >= 1000 ) {
						ret = 2;
						break;
					}
				} while( receive != 0xfe );
				
				pattern_msd_read = 3;
				break;
				
			case 3:
				// データ読み込み
				for ( i = 0; i < 512; i++ ) {
					*read++ = msd_read();
				}
				
				// CRC 2バイト読み込み
				msd_read();
				msd_read();
				
				pattern_msd_read = 4;
				break;
		}
	}
	
	msd_write( 0xff );			// ダミークロック送信
	MSD_CS = CS_HIGH;			// CS端子をHIGHにする
	msdlibMode = ( ret != 0 ) ? 99 : 0;	// 初期化エラーなら99をセット
	
	if ( ret == 0 ) {
		//printf("読み込み完了\n");
	} else if ( ret == 1 ) {
		printf("CMD17 error\n");
	} else if ( ret == 2 ) {
		printf("response error\n");
	}
				
	return ret;
}
///////////////////////////////////////////////////////////////////////////
// モジュール名 writeMicroSD
// 処理概要     microSD書き込み(512バイト)
// 引数         address: MicroSDのアドレス	write:	*書き込む配列のアドレス
// 戻り値       0:正常 1以上:エラー
///////////////////////////////////////////////////////////////////////////
uint8_t writeMicroSD ( uint32_t address, int8_t *write )
{
	volatile int16_t	i, receive;
	volatile uint8_t	a1, a2, a3, a4, pattern_msd_write = 1, ret = 0;
	
	if( msdlibMode != 0 ) {             // 別な作業中ならエラー
		return 1;
	}
	msdlibMode = 1;
	
	while ( ret < 1 && pattern_msd_write <= 4 ) {
		switch ( pattern_msd_write ) {
			case 1:
				// SDHCならアドレス→セクタ番号に変換
				address >>= 9;
				
				MSD_CS = CS_LOW;		// CS端子をLOWにする
				
				pattern_msd_write = 2;
				break;
			
			case 2:
				// CMD24送信
				a1 = ( unsigned char )( ( address&0xff000000 ) >> 24 );
				a2 = ( unsigned char )( ( address&0x00ff0000 ) >> 16 );
				a3 = ( unsigned char )( ( address&0x0000ff00 ) >>  8 );
				a4 = ( unsigned char )(  address&0x000000ff       );
				
				receive = msd_CMD( 0x58, a1, a2, a3, a4, 0x00 );
				
				// レスポンス確認
				if ( ( receive & 0x1f ) != 0x00 ) {
					ret = 1;
					break;
				}
				// 1バイト時間を空ける
				receive = msd_read();
				
				pattern_msd_write = 3;
				break;
				
			case 3:
				// 開始合図
				msd_write( 0xfe );
				// データ送信
				/*for ( i=0; i<512; i++ ) {
					msd_write( *write++ );
				}*/
				i = 0;
				while ( i < 512 ) {
					msd_write( *write++ );
					i++;
				}
				// CRC送信
				msd_write( 0xff );
				msd_write( 0xff );
				
				pattern_msd_write = 4;
				break;
				
			case 4:
				// レスポンス読み込み
				receive = msd_read();
				receive &= 0x1f;
				
				if ( receive == 0x05 ) {
					ret = 0;        // 書き込み正常終了
				} else if ( receive == 0x0d ) {
					ret = 2;       // 書き込みエラー
				} else {
					ret = 3;       // それ以外のエラー
				}
				// busy状態が終わるまで待つ
				/*for ( i=0; i<10000; i++ ) {
					if( msd_read() != 0x00 ) break;
				}*/
				i = 0;
				while ( i < 10000 ) {
					if( msd_read() != 0x00 ) break;
					i++;
				}
				pattern_msd_write = 5;
				break;
		}
	}
	
	// ダミークロック送信
	msd_write( 0xff );
	
	MSD_CS = CS_HIGH;			// CS端子をHIGHにする
	msdlibMode = (ret != 0) ? 99 : 0;	// 初期化エラーなら99をセット
	
	if ( ret == 0 ) {
		//printf("書き込み正常終了\n");
	} else if ( ret == 1 ) {
		printf("CMD24 error\n");
	} else if ( ret == 2 ) {
		printf("Write error\n");
	} else if ( ret == 3 ) {
		printf("Other error\n");
	}
				
	return ret;
}
///////////////////////////////////////////////////////////////////////////
// モジュール名 eraseMicroSD
// 処理概要     microSDのデータイレース(0x00書き込み)
// 引数         st_address: 開始アドレス ed_address; 終了アドレス
// 戻り値       0:正常 1以上:エラー
///////////////////////////////////////////////////////////////////////////
uint8_t eraseMicroSD( uint32_t st_address, uint32_t ed_address )
{
	volatile int16_t	i, j, receive;
	volatile uint8_t	a1, a2, a3, a4, pattern_msd_erase = 1, ret = 0;
	
	if( msdlibMode != 0 ) {             // 別な作業中ならエラー
		return 1;
	}
	msdlibMode = 1;
	
	while ( ret < 1 && pattern_msd_erase <= 4 ) {
		switch ( pattern_msd_erase ) {
			case 1:
				// SDHCならアドレス→セクタ番号に変換
				st_address >>= 9;
				ed_address >>= 9;
				
				MSD_CS = CS_LOW;	// CS端子をLOWにする
				
				pattern_msd_erase = 2;
				break;
			
			case 2:
				// CMD32送信(イレース開始アドレスのセット)
				a1 = ( unsigned char )( ( st_address&0xff000000 ) >> 24 );
				a2 = ( unsigned char )( ( st_address&0x00ff0000 ) >> 16 );
				a3 = ( unsigned char )( ( st_address&0x0000ff00 ) >>  8 );
				a4 = ( unsigned char )(  st_address&0x000000ff       );
				
				receive = msd_CMD( 0x60, a1, a2, a3, a4, 0xff );
				
				// レスポンス確認
				if ( receive != 0x00 ) {
					ret = 1;
					break;
				}
				
				pattern_msd_erase = 3;
				break;
				
			case 3:
				// ウェイト
				for( j=0; j<10000; j++ );
				
				// CMD33送信(イレース終了アドレスのセット)
				a1 = (unsigned char)( (ed_address&0xff000000) >> 24);
				a2 = (unsigned char)( (ed_address&0x00ff0000) >> 16);
				a3 = (unsigned char)( (ed_address&0x0000ff00) >>  8);
				a4 = (unsigned char)(  ed_address&0x000000ff       );
				
				receive = msd_CMD( 0x61, a1, a2, a3, a4, 0xff );
				
				// レスポンス確認
				if( receive != 0x00 ) {
					ret = 2;
					break;
				}
				
				pattern_msd_erase = 4;
				break;
				
			case 4:
				// ウェイト
				for( j=0; j<10000; j++ );
				
				// CMD38送信(イレース)
				receive = msd_CMD( 0x66, 0, 0, 0, 0, 0xff );
				
				// レスポンス確認
				if( receive != 0x00 ) {
					ret = 3;
					break;
				}
				
				// busy状態が終わるまで待つ
				for( i=0; i<10000; i++ ) {
					if( msd_read() != 0x00 ) break;
				}
				
				pattern_msd_erase = 5;
				break;
		}
	}
	
	// ウェイト
	for( j = 0; j < 10000; j++ );
	
	msd_write( 0xff );		// ダミークロック送信
	MSD_CS = CS_HIGH;		// CS端子をHIGHにする
	
	// イレース後、１回ダミーライト
	if ( ret == 0 ) {
		for ( i=0; i<512; i++ ) msdlibBuff[i] = 0;   // バッファクリア
		msdlibMode = 0;
		receive = writeMicroSD( st_address, ( signed char* )msdlibBuff );
		if ( receive != 0x00 ) {
			// 書き込みできず
			// たまにエラーを返すmicroSDがあるが、正常なようなので
			// エラーは無視しておく
			//ret = 44;
		}
	}
	msdlibMode = (ret != 0) ? 99 : 0;   // 初期化エラーなら99をセット
	
	if ( ret == 0 ) {
		//printf("イレーズ正常終了\n");
	} else if ( ret == 1 ) {
		printf("CMD32 error\n");
	} else if ( ret == 2 ) {
		printf("CMD33 error\n");
	} else if ( ret == 3 ) {
		printf("CMD38 errorn");
	}
	
	return ret;
}
/////////////////////////////////////////////////////////////////////////////////
// モジュール名 setMicroSDdata
// 処理概要     microSDにデータセット
// 引数         microSDに書き込むデータのある配列
// 戻り値       12:正常に終了 それ以外:書き込み中で今回のセットは無効
/////////////////////////////////////////////////////////////////////////////////
uint8_t setMicroSDdata( int8_t *p )
{	
	volatile int16_t i = 0;
	
	if( msdlibMode != 11) {				// 現在書き込み処理中か?
		return msdlibMode;
	} else {
		// 512バイト msdlibBuffへ転送
		memcpy( msdlibBuff, p, 512 );
		
		msdlibBuff[512] = 0xff;
		msdlibBuff[513] = 0xff;
		
		msdlibCnt = 514;			// バッファ書き込み数
		msdlibWrite = msdlibBuff;
		
		msd_write( 0xfc );			// 1個だけ送信(残りは割込みで)
		interrupt_msd_send_data = 1;
		//R_PG_Timer_StartCount_CMT_U1_C2(); 	// カウントスタート
		
		
		msdlibMode = 12;			// microSDProcess 処理開始
		return msdlibMode;
	}
}
///////////////////////////////////////////////////////////////////////////
// モジュール名 microSDProcessStart
// 処理概要     microSDProcess開始処理
// 引数         microSDの書き込みアドレス
// 戻り値       0:正常に終了 それ以外:既に書き込み中
///////////////////////////////////////////////////////////////////////////
uint8_t microSDProcessStart( uint32_t address )
{
	volatile int16_t	receive;
	volatile uint8_t	ret = 0;
	volatile uint8_t	a1, a2, a3, a4;
	
	if( msdlibMode != 0 ) {
		// 既に処理中
		ret = 1;
		return ret;
	} else {
		// SDHCならアドレス→セクタ番号に変換
		address >>= 9;
		
		// microSDに書き込みアドレスセット
		a1 = (unsigned char)( (address&0xff000000) >> 24);
		a2 = (unsigned char)( (address&0x00ff0000) >> 16);
		a3 = (unsigned char)( (address&0x0000ff00) >>  8);
		a4 = (unsigned char)(  address&0x000000ff       );
		
		MSD_CS = CS_LOW;	// CS端子をLOWにする
		
		// CMD25送信
		receive = msd_CMD( 0x59, a1, a2, a3, a4, 0xff );
		
		// レスポンス確認
		if( ( receive & 0x80 ) != 0x00 ) {
			ret = 2;                        // セットできず 
			MSD_CS = CS_HIGH;		// CS端子をHIGHにする
		} else {
			printf("CMD25 send\n");
			msdlibMode = 11;                // セット完了
		}
	}
	return ret;
}
///////////////////////////////////////////////////////////////////////////
// モジュール名 microSDProcessEnd
// 処理概要     microSDProcess終了処理
// 引数         microSDの書き込みアドレス
// 戻り値       0:正常に終了 それ以外:既に書き込み中
///////////////////////////////////////////////////////////////////////////
uint8_t microSDProcessEnd( void )
{
	volatile uint8_t ret = 1;
	
	if( msdlibMode == 11 ) {
		msdlibMode = 21;                // 終了処理セット
	} else if( msdlibMode == 0 ) {
		ret = 0;                        // 終了
	}
	
	return ret;
}
///////////////////////////////////////////////////////////////////////////
// モジュール名 microSDProcess
// 処理概要     microSD　間欠書き込み処理
// 引数         なし
// 戻り値       なし
///////////////////////////////////////////////////////////////////////////
void microSDProcess( void )
{
	volatile int16_t receive;
	
	// microSD msdprintf関数処理(1msごとに実行)
	//microSDprintfProcess();
	
	switch( msdlibMode ) {
		case 0:
			// 開始前の待機
			break;
			
		case 1:
			// 別な作業中
			break;
		
		case 11:
			// 開始後の待機　何もせず
			break;
		
		case 12:
			// 送信割り込みでmicroSDにデータ書き込み中
			if( msdlibCnt <= 0 ) {
				msdlibMode = 14;
			}
			break;
		
		case 13:
			// 最後のデータ送るまで待つ
			/*if( ri_u1c1 == 1 ) {
				i = u1rb;                   // ダミーリード
				msdlibMode = 14;
			}*/
			break;
		
		case 14:
			// 終了処理
			// レスポンス読み込み
			receive = msd_read();
			receive &= 0x1f;
		
			if( receive == 0x05 ) {
				msdlibError = 2;            // 書き込み正常終了
				msdlibMode = 15;
			} else if( receive == 0xc ) {
				msdlibError = 3;            // 書き込みエラー
				msdlibMode = 31;
			} else {
				msdlibError = 4;            // それ以外のエラー
				msdlibMode = 31;
			}
			break;
		
		case 15:
			// busy状態が終わるまで待つ
			if( msd_read() != 0x00 ) {
				msdlibMode = 11;
			}
			break;
	
		case 21:
			// 終了処理
			msd_write( 0xfd );
			msdlibMode = 22;
			break;
	
		case 22:
			// busy状態が終わるまで待つ
			if( msd_read() != 0x00 ) {
				msdlibMode = 31;
			}
			break;
		
		case 31:
			// CS="1"
			msd_write( 0xff );		// ダミークロック送信
			MSD_CS = CS_LOW;		// CS端子をLOWにする
			msdlibMode = 32;		// 処理終了
			msdlibCnt = 10;
			break;

		case 32:
			msdlibCnt--;
			if( msdlibCnt == 0 ) {
				msdlibMode = 0;             // 処理終了
			}
			break;
		
		case 99:
			// 初期化エラー
			break;
	}
}
///////////////////////////////////////////////////////////////////////////
// モジュール名 checkMicroSDProcess
// 処理概要     microSD　間欠書き込み処理の終了チェック
// 引数         なし
// 戻り値       0:処理無し 11:開始後の待機 それ以外:処理中
///////////////////////////////////////////////////////////////////////////
uint8_t checkMicroSDProcess( void )
{
	return msdlibMode;
}
///////////////////////////////////////////////////////////////////////////
// モジュール名 msd_send_data
// 処理概要     書き込みデータをMicroSDに送信
// 引数         なし
// 戻り値       なし
// 備考			10usごとに実行させる
///////////////////////////////////////////////////////////////////////////
void msd_send_data (void)
{	
	__setpsw_i();
	if ( interrupt_msd_send_data == 1 ) {
		msd_write( *msdlibWrite++ );
		msdlibCnt--;
		if ( msdlibCnt <= 0) {
			interrupt_msd_send_data = 0;
			//R_PG_Timer_HaltCount_CMT_U1_C2();
		}
	}
}
///////////////////////////////////////////////////////////////////////////
// モジュール名 init_log
// 処理概要     書き込み準備
// 引数         なし
// 戻り値       なし
///////////////////////////////////////////////////////////////////////////
void init_log ( void )
{
	volatile uint8_t ret, pattern_inti_log = 0;
	
	// microSD 書き込み開始アドレス
	// 512の倍数に設定する
	msdStartaddress = msdaddrBuff[0] + 1;
	// 終了アドレスが初期値の場合開始アドレスを0にする
	if ( msdStartaddress == MSD_ENDADDRESS + 1 || msdStartaddress >= 4000000000 ) msdStartaddress = 0;

	// microSD 書き込み終了アドレス
	// 書き込みしたい時間[ms] : x = 10[ms] : 64バイト(保存バイト数)
	// 5000msなら、x = 5000 * 64 / 10 = 32000
	// 結果は512の倍数になるように繰り上げする。よって、32256にする。
	// msdEndaddressBuff = RecodeTime * Data_Size / WriteTime
	// msdEndaddressBuff = ( 512 - ( msdEndaddressBuff % 512 ) ) + msdEndaddressBuff;
	
	msdEndaddress = MSD_ENDADDRESS;
	msdEndaddress += msdStartaddress;   // スタート分足す
	printf("msdStartaddress = %d\n", msdStartaddress);
	printf("msdEndaddress = %d\n", msdEndaddress);
	
	while ( pattern_inti_log < 2 ) {
		switch ( pattern_inti_log ) {
			case 0:
				// microSDクリア
				ret = eraseMicroSD( msdStartaddress, msdEndaddress - 1 );
				if( ret != 0x00 ) {
					//setBeepPatternS( 0xaa00 );
					printf( "\nmicroSD Erase Error!!\n" );  // エラー
					break;
				}
				pattern_inti_log = 1;
				break;
				
			case 1:
				// microSDProcess開始処理
				ret = microSDProcessStart( msdStartaddress );
			        if( ret != 0x00 ) {
					//setBeepPatternS( 0xaa00 );
					printf( "\nmicroSD microSDProcess Error!!\n" );  // エラー
					break;
				}
				msdBuffaddress = 0;
				msdWorkaddress = msdStartaddress;
				
				//flashDataBuff[ 0 ] = msdStartaddress >> 16;
				//flashDataBuff[ 1 ] = msdStartaddress & 0xffff;	// 開始アドレス
				//writeFlashData( MSD_STARTAREA, MSD_ENDAREA, MSD_DATA, 2 );
				
				pattern_inti_log = 2;
				break;
		}
	} 
}
///////////////////////////////////////////////////////////////////////////
// モジュール名 msdEndLog
// 処理概要     ログの終了処理
// 引数         なし
// 戻り値       0:正常に終了 1:異常終了
///////////////////////////////////////////////////////////////////////////
uint8_t msdEndLog ( void )
{
	uint8_t pattern_msdend = 0;
	
	while ( pattern_msdend < 2 ) {
	switch( pattern_msdend ) {
		case 0:
				// 最後のデータが書き込まれるまで待つ
				if ( checkMicroSDProcess() == 11 ) {
					msdFlag = 0;			// ログ記録終了
					microSDProcessEnd();        // microSDProcess終了処理
					pattern_msdend = 1;
					break;
				} else if ( checkMicroSDProcess() == 0 ) {
					//setBeepPatternS( 0xf0f0 );
					pattern_msdend = 3;
					break;
				}
				break;
				
			case 1:
				// 終了処理が終わるまで待つ
				if ( checkMicroSDProcess() == 0 ) {
					// MicroSD最終書き込みアドレス保存
					//flashDataBuff[ 0 ] = msdWorkaddress >> 16;
					//flashDataBuff[ 1 ] = msdWorkaddress & 0xffff;	// 終了アドレス
					//writeFlashData( MSD_STARTAREA, MSD_ENDAREA, MSD_DATA, 2 );
					pattern_msdend = 2;
					//setBeepPatternS( 0xa8a8 );
					break;
				}
				break;
	}
	}
	
	return pattern_msdend - 2;
}
///////////////////////////////////////////////////////////////////////////
// モジュール名 send_Char
// 処理概要     char型変数をmsdBuffに送る
// 引数         変換するchar型変数
// 戻り値       なし
///////////////////////////////////////////////////////////////////////////
void send_Char ( uint8_t data )
{
	*msdBuffPointa++ = data;
}
///////////////////////////////////////////////////////////////////////////
// モジュール名 ShortToChar
// 処理概要     short型変数をchar型に変換してmsdBuffに送る
// 引数         変換するshort型変数
// 戻り値       なし
///////////////////////////////////////////////////////////////////////////
void send_ShortToChar ( int16_t data )
{
	*msdBuffPointa++ = data >> 8;
	*msdBuffPointa++ = data & 0xff;
}
///////////////////////////////////////////////////////////////////////////
// モジュール名 uintToChar
// 処理概要     unsigned int型変数をchar型に変換してmsdBuffに送る
// 引数         変換するunsigned int型変数
// 戻り値       なし
///////////////////////////////////////////////////////////////////////////
void send_uIntToChar ( uint32_t data )
{
	*msdBuffPointa++ = data >> 24;
	*msdBuffPointa++ = ( data & 0x00ff0000 ) >> 16;
	*msdBuffPointa++ = ( data & 0x0000ff00 ) >> 8;
	*msdBuffPointa++ = data & 0x000000ff;
}
///////////////////////////////////////////////////////////////////////////////////////
// モジュール名 CharToShort
// 処理概要     unsigned char型変数をshort型に変換する
// 引数         data:変換するsigned char型変数	offsetaddress: MicroSD内の順番
// 戻り値       変換したshort型
///////////////////////////////////////////////////////////////////////////////////////
int16_t CharToShort( uint8_t offsetaddress )
{
	volatile short s;
	
	s = (short)( (unsigned char)msdBuff[msdBuffaddress + offsetaddress] * 0x100 + 
			(unsigned char)msdBuff[msdBuffaddress + offsetaddress + 1] );
					
	return s;				
}
///////////////////////////////////////////////////////////////////////////////////////
// モジュール名 CharTouInt
// 処理概要     unsigned char型変数をunsigned int型に変換する
// 引数         data:変換するsigned char型変数	offsetaddress: MicroSD内の順番
// 戻り値       変換したunsigned int型
///////////////////////////////////////////////////////////////////////////////////////
uint32_t CharTouInt( uint8_t offsetaddress )
{
	volatile unsigned int i;
	
	i = (unsigned int)(unsigned char)msdBuff[msdBuffaddress + offsetaddress] * 0x1000000;
	i += (unsigned int)(unsigned char)msdBuff[msdBuffaddress + offsetaddress + 1] * 0x10000;
	i += (unsigned int)(unsigned char)msdBuff[msdBuffaddress + offsetaddress + 2] * 0x100;
	i += (unsigned int)(unsigned char)msdBuff[msdBuffaddress + offsetaddress + 3];
					
	return i;				
}