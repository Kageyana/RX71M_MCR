//======================================//
// インクルード                         //
//======================================//
#include "I2C_MPU9250.h"
//====================================//
// グローバル変数の宣言							//
//====================================//
// IMUから取得したデータ
volatile int 	rawXa = 0, rawYa = 0, rawZa = 0;	// 加速度(16bitデータ)
volatile int 	rawXg = 0, rawYg = 0, rawZg = 0;	// 角加速度(16bitデータ)

volatile short 	rawTemp;			// 温度(16bitデータ)

// データ処理
double 	TurningAngleIMU;	// IMUから求めたヨー角度
double	RollAngleIMU;		// IMUから求めたロール方向角度
double 	PichAngleIMU;		// IMUから求めたピッチ方向角度
double	TempIMU;			// IMUの温度
short		offset[3];			// オフセット値(16bit)

char	whoami;
char cnt_imu = 0;
char	busIMU = BUS_IMU_FREE;

///////////////////////////////////////////////////////////////////////////
// モジュール名 wait_IMU								//
// 処理概要   遅延処理									//
// 引数         遅延時間(ms)								//
// 戻り値       なし										//
///////////////////////////////////////////////////////////////////////////
void wait_IMU ( short waitTime )
{
	volatile int time, i = 0;
	
	time = (int)waitTime * ( CLOCK * 1000 ) / 16;
	for ( i = 0; i < time; i++) __nop();
}
///////////////////////////////////////////////////////////////
// モジュール名 IMUWriteByte						//
// 処理概要   指定したレジスタにデータを書き込む			//
// 引数         reg:レジスタのアドレス data:書き込みデータ		//
// 戻り値       なし								//
///////////////////////////////////////////////////////////////
void IMUWriteByte( char reg, char data )
{
	uint8_t sendData[2] = { reg, data }, num = 2;
	
	I2C_IMU_COMMAND;		// コマンド送信
	busIMU = BUS_IMU_BUSY;
	while(busIMU)__nop();
}
/////////////////////////////////////////////////////////
// モジュール名 IMUReadByte					//
// 処理概要   指定したレジスタのデータを読む			//
// 引数         reg:レジスタのアドレス				//
// 戻り値       レジスタの値					//
/////////////////////////////////////////////////////////
char IMUReadByte( char reg )
{
	uint8_t sendData[1] = { 0x75U }, num = 1, reciveData[1] = {0};
  	
	I2C_IMU_COMMAND;		// コマンド送信
	busIMU = BUS_IMU_BUSY;
	while(busIMU)__nop();
	I2C_IMU_RECIVE;		// データ送信
	busIMU = BUS_IMU_BUSY;
	while(busIMU)__nop();
	
	return reciveData[0];
}
///////////////////////////////////////////////////////////////////////////////////////////////////
// モジュール名 IMUReadArry												//
// 処理概要   指定したレジスタから指定の数だけデータを読む							//
// 引数         reg:レジスタのアドレス num2 受け取るデータの数 reciveData 取得データを格納する配列	//
// 戻り値       なし														//
///////////////////////////////////////////////////////////////////////////////////////////////////
void IMUReadArry( char reg, char num2, char* reciveData )
{
	uint8_t sendData[1] = { reg }, num = 1;
	
	
	I2C_IMU_COMMAND;
	busIMU = BUS_IMU_BUSY;
	while(busIMU)__nop();
	num = num2;
	I2C_IMU_ARRY;
	busIMU = BUS_IMU_BUSY;
	while(busIMU)__nop();
}
///////////////////////////////////////////////////
// モジュール名 init_IMU					//
// 処理概要   IMUの初期化				//
// 引数         なし						//
// 戻り値       なし						//
//////////////////////////////////////////////////
void init_IMU (void)
{
	IMUWriteByte( PWR_MGMT_1, 0x00);	// スリープモード解除
	IMUWriteByte( INT_PIN_CFG, 0x02);	// 内蔵プルアップ無効化
	IMUWriteByte( CONFIG, 0x00);		// ローパスフィルタを使用しない
	IMUWriteByte( ACCEL_CONFIG, 0x18);	// レンジ±16gに変更
	IMUWriteByte( GYRO_CONFIG, 0x10);	// レンジ±1000deg/sに変更
}
///////////////////////////////////////////////////
// モジュール名 IMUProcess				//
// 処理概要   センセデータの取得			//
// 引数         なし						//
// 戻り値       なし						//
//////////////////////////////////////////////////
void IMUProcess (void)
{
	char 	axisData[14];	// 角加速度、温度の8bit分割データ格納先
	
	IMUReadArry( GYRO_XOUT_H, 6, axisData);
	rawXg = (short)((axisData[0] << 8 & 0xff00 ) | axisData[1]);
	rawYg = (short)((axisData[2] << 8 & 0xff00 ) | axisData[3]);
	rawZg = (short)((axisData[4] << 8 & 0xff00 ) | axisData[5]);
	
	
}