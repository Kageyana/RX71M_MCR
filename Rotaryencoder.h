#ifndef ROTARYENCODER_H
#define ROTARYENCODER_H
//====================================//
// インクルード                        						//
//====================================//
#include "r_smc_entry.h"
//====================================//
// シンボル定義									//
//====================================//

//====================================//
// グローバル変数の宣言								//
//====================================//
extern unsigned int	EncoderTotal;	// 総走行距離
extern signed short	Encoder;		// 1msごとのパルス
extern unsigned int	enc1;		// 走行用距離カウント
extern unsigned int	enc_slope;		// 坂上距離カウント
//====================================//
// プロトタイプ宣言									//
//====================================//
void getEncoder (void);

#endif