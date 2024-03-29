//====================================
// インクルード
//====================================
#include "button.h"
//====================================
// グローバル変数の宣言
//====================================

//////////////////////////////////////////////////////////////////////////
// モジュール名 getTactsw
// 処理概要     5方向タクトスイッチの入力を取得
// 引数         なし
// 戻り値       方向、押し込みのマクロ定義
//////////////////////////////////////////////////////////////////////////
uint8_t getTactsw (void){
    uint8_t ret = 0;
    if (!PORT5.PIDR.BIT.B4) ret = UP;
    if (!PORT5.PIDR.BIT.B5) ret = DOWN;
    if (!PORT1.PIDR.BIT.B3) ret = LEFT;
    if (!PORT5.PIDR.BIT.B3) ret = RIGHT;
    if (!PORT1.PIDR.BIT.B2) ret = PUSH;
    return ret;
}
//////////////////////////////////////////////////////////////////////////
// モジュール名 getRotarysw
// 処理概要     ロータリスイッチの入力を取得
// 引数         なし
// 戻り値       4bit
//////////////////////////////////////////////////////////////////////////
uint8_t getRotarysw(void){    
    uint8_t ret = 0;
    if (PORT1.PIDR.BIT.B7) ret += 0x1;
    if (PORT2.PIDR.BIT.B2) ret += 0x2;
    if (PORT2.PIDR.BIT.B3) ret += 0x4;
    if (PORT2.PIDR.BIT.B7) ret += 0x8;
    return ret;
}