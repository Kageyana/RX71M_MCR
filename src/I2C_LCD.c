//====================================
// インクルード
//====================================
#include "I2C_LCD.h"
//====================================
// グローバル変数の宣言
//====================================
// LCD関連
static volatile uint8_t		buffLcdData[ LCD_MAX_X / LCD_MAX_Y ];		// 表示バッファ
static uint8_t				buffLcdData2[ LCD_MAX_X / LCD_MAX_Y + 10 ]; 	// 表示バッファ一時作業エリア
static volatile uint32_t	lcdBuffPosition;				// バッファに書き込む位置
static volatile uint32_t	lcdMode2 = 1;					// 表示処理No管理
static volatile uint32_t	lcdNowLocate;					// 現在の表示している位置
static volatile uint32_t	lcdRefreshFlag;					// リフレッシュフラグ

uint8_t	busLCD = BUS_LCD_FREE;
/////////////////////////////////////////////////////////////////////////
// モジュール名 lcd_put
// 処理概要     データ送信
// 引数         data
// 戻り値       なし
/////////////////////////////////////////////////////////////////////////
void lcd_put( uint8_t data )
{
	uint8_t word[2] = { RSBIT1, data };
	I2C_LCD_SEND;
	busLCD = BUS_LCD_BUSY;
	while(busLCD)__nop();
}
//////////////////////////////////////////////////////////////////////////
// モジュール名 lcd_CMD
// 処理概要     コマンド送信
// 引数         cmd
// 戻り値       なし
//////////////////////////////////////////////////////////////////////////
void lcd_CMD( uint8_t cmd ) 
{
	uint8_t Command[2] = { RSBIT0, cmd };
 	I2C_LCD_CMD;
	busLCD = BUS_LCD_BUSY;
	while(busLCD)__nop();
}
//////////////////////////////////////////////////////////////////////////
// モジュール名 wait_lcd
// 処理概要     遅延処理
// 引数         遅延時間(ms)
// 戻り値       なし
//////////////////////////////////////////////////////////////////////////
void wait_lcd ( uint16_t waitTime )
{
	volatile uint32_t time, i = 0;
	
	time = (uint32_t)waitTime * ( CLOCK * 1000 )/ 10;
	for ( i = 0; i < time; i++) __nop();
}
//////////////////////////////////////////////////////////////////////////
// モジュール名 inti_lcd
// 処理概要     LCDの初期化
// 引数         なし
// 戻り値       なし
//////////////////////////////////////////////////////////////////////////
 void inti_lcd(void)
 {
	wait_lcd(4);
	lcd_CMD(0x38);	// function set				: データ線は8本・表示は２行・フォントは5x8ドット
	wait_lcd(1);
	lcd_CMD(0x39);	// function set           	: 拡張コマンドの設定を有効にする
	wait_lcd(1);
	lcd_CMD(0x14);	// Internal OSC frequency 	: バイアスの選択と内部OSC周波数の調整
	wait_lcd(1);
	lcd_CMD(0x70);	// Contrast set          	: コントラスト調整データ(下位4ビット)
	wait_lcd(1);
	lcd_CMD(0x56);	// Power/ICON/Contrast control	: 昇圧回路有効、コントラスト調整データ(上位2ビット)
	wait_lcd(1);
	lcd_CMD(0x6c);	// Follower control     	: フォロア回路をON、増幅率の調整を行う
	wait_lcd(200);
	lcd_CMD(0x38);	// function set         	: 拡張コマンドを設定を無効にする
	wait_lcd(1);
	lcd_CMD(0x0c);	// display ON/OFF control	: 画面表示はON・カーソル表示はOFF・カーソル点滅はOFF
	wait_lcd(1);
	lcd_CMD(0x01);	// Clear Display 			: 画面全体に20Hのｽﾍﾟｰｽで表示、ｶｰｿﾙはcol=0,row=0に移動
	wait_lcd(2);
}
//////////////////////////////////////////////////////////////////////////
// モジュール名 lcdLocate
// 処理概要     液晶カーソル移動
// 引数         x , y
// 戻り値       なし
//////////////////////////////////////////////////////////////////////////
static void lcdLocate( uint32_t x, uint32_t y )
{
    volatile uint8_t work = 0x80;

    // xの計算
    work += x;

    // yの計算
    if( y == 1 ) {
        work += 0x40;
    } else if( y == 2 ) {
        work += 0x14;
    } else if( y == 3 ) {
        work += 0x54;
    }

    // カーソル移動
    lcd_CMD(work);
}
//////////////////////////////////////////////////////////////////////////
// モジュール名 lcdShowProcess
// 処理概要     液晶表示処理
// 引数         なし
// 戻り値       なし
// メモ         この関数は割り込みで1msごとに実行してください
//////////////////////////////////////////////////////////////////////////
void lcdShowProcess( void )
{
    switch( lcdMode2 ) {
    case 1: // データ更新されたかチェック
    	if( lcdRefreshFlag ) {
    		lcdRefreshFlag = 0;
    		lcdMode2 = 2;
	}
        break;

    case 2: // 位置初期化
	lcdNowLocate = 0;
    	lcdLocate( 0, 0 );
	lcdMode2 = 3;
        break;

    case 3: // 改行位置の確認 
	if( lcdNowLocate % LCD_MAX_X == 0 ) {
    		lcdLocate( 0, lcdNowLocate / LCD_MAX_X );
	}
	lcdMode2 = 4;
        break;

    case 4: // データ表示処理
	lcd_put(buffLcdData[ lcdNowLocate++ ]);
	if( lcdNowLocate >= LCD_MAX_X * LCD_MAX_Y ) {
		lcdMode2 = 1;
	} else {
		lcdMode2 = 3;
	}
	break;

    default:
	lcdMode2 = 1;
   	break;
    }
}
//////////////////////////////////////////////////////////////////////////
// モジュール名 lcdPrintf
// 処理概要     液晶へ表示 表示位置は過去に表示した位置の次から
// 引数         printfと同じ
// 戻り値       正常時：出力した文字列 異常時：負の数
//////////////////////////////////////////////////////////////////////////
int32_t lcdPrintf(uint8_t *format, ...)
{
    volatile va_list	argptr;
    volatile uint8_t	*p;
    volatile uint16_t	ret = 0;

    va_start(argptr, format);
    ret = vsprintf( buffLcdData2, format, argptr );
    va_end(argptr);

    if( ret > 0 ) {
        // vsprintfが正常なら液晶バッファへ転送
        p = buffLcdData2;
        while( *p ) {
            buffLcdData[lcdBuffPosition++] = *p++;
            if( lcdBuffPosition >= LCD_MAX_X * LCD_MAX_Y ) {
                lcdBuffPosition = 0;
            }
        }
        lcdRefreshFlag = 1;
    }
    return ret;
}
//////////////////////////////////////////////////////////////////////////
// モジュール名 lcdPosition
// 処理概要     液晶の表示位置指定
// 引数         横位置 , 縦位置
// 戻り値       なし
//////////////////////////////////////////////////////////////////////////
void lcdPosition(uint8_t x ,uint8_t y)
{
    if( x >= LCD_MAX_X ) return;
    if( y >= LCD_MAX_Y ) return;

    lcdBuffPosition = x + y * LCD_MAX_X;
}