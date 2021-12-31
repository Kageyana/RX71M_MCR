//=====================================
// インクルード
//=====================================
#include "IMU_ICM20648.h"
//====================================
// グローバル変数の宣言
//====================================
// IMUから取得したデータ
volatile int32_t 	rawXa = 0, rawYa = 0, rawZa = 0;	// 加速度(16bitデータ)
volatile int32_t 	rawXg = 0, rawYg = 0, rawZg = 0;	// 角加速度(16bitデータ)

volatile int16_t 	rawTemp;			// 温度(16bitデータ)

// データ処理
double 	TurningAngleIMU;	// IMUから求めたヨー角度
double	RollAngleIMU;		// IMUから求めたロール方向角度
double 	PichAngleIMU;		// IMUから求めたピッチ方向角度
double	TempIMU;			// IMUの温度
int16_t	offsetX, offsetY, offsetZ;		// オフセット値(16bit)

uint8_t	whoami;
uint8_t cnt_imu = 0;
uint8_t	busIMU = BUS_IMU_FREE;

///////////////////////////////////////////////////////////////////////////
// モジュール名 wait_IMU
// 処理概要   	遅延処理
// 引数         遅延時間(ms)
// 戻り値       なし
///////////////////////////////////////////////////////////////////////////
void wait_IMU ( uint16_t waitTime )
{
	volatile int32_t time, i = 0;
	
	time = (int32_t)waitTime * ( CLOCK * 1000 ) / 10;
	for ( i = 0; i < time; i++) __nop();
}
///////////////////////////////////////////////////////////////
// モジュール名 IMUWriteByte
// 処理概要   	指定したレジスタにデータを書き込む
// 引数         reg:レジスタのアドレス data:書き込みデータ
// 戻り値       なし
///////////////////////////////////////////////////////////////
void IMUWriteByte( uint8_t reg, uint8_t data )
{
	uint8_t data_tr[2] = { reg, data }, data_re[2], numS = 2, numR = 2;
	
	#if USECOM == 0
		I2C_IMU_COMMAND;		// コマンド送信
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
// モジュール名 IMUReadByte
// 処理概要   	指定したレジスタのデータを読む
// 引数         reg:レジスタのアドレス
// 戻り値       レジスタの値
/////////////////////////////////////////////////////////
uint8_t IMUReadByte( uint8_t reg )
{
	uint8_t data_tr[2] = { reg | 0x80 }, data_re[2], numS = 2, numR = 2;
  	
	#if USECOM == 0
		I2C_IMU_COMMAND;		// コマンド送信
		busIMU = BUS_IMU_BUSY;SPI_IMU_CS = 0;
		while(busIMU)__nop();
		I2C_IMU_RECIVE;		// データ受信
		busIMU = BUS_IMU_BUSY;
		while(busIMU)__nop();
	# else
		// アドレス送信
		SPI_IMU_CS = 0;
		SPI_IMU_SEND;
		busIMU = BUS_IMU_BUSY;
		while(busIMU)__nop();
		SPI_IMU_CS = 1;
	#endif
	
	return data_re[1];
}
///////////////////////////////////////////////////////////////////////////////////////////////////
// モジュール名 IMUReadArry
// 処理概要   	指定したレジスタから指定の数だけデータを読む
// 引数         reg:レジスタのアドレス num2 受け取るデータの数 reciveData 取得データを格納する配列
// 戻り値       なし
///////////////////////////////////////////////////////////////////////////////////////////////////
void IMUReadArry( uint8_t reg, uint8_t num2, uint8_t* data_re )
{
	uint8_t data_tr[20] = { reg | 0x80 }, numS = num2+1, numR = num2+1;
	
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
		busIMU = BUS_IMU_BUSY;
		while(busIMU)__nop();
		SPI_IMU_CS = 1;
	#endif
}
///////////////////////////////////////////////////
// モジュール名 init_IMU
// 処理概要   	IMUの初期化
// 引数         なし
// 戻り値       0: 初期化失敗 1: 初期化成功
///////////////////////////////////////////////////
bool init_IMU (void)
{
	bool ret=false;
	#if USECOM == 1
		SPI_IMU_CS = 1;
	#endif
	if (IMUReadByte(0x0) == 0xE0) {
		ret = true;
		IMUWriteByte( PWR_MGMT_1, 0x01);	// スリープモード解除
		IMUWriteByte( USER_CTRL, 0x10);		// 諸々機能無効　SPIonly
		IMUWriteByte( REG_BANK_SEL, 0x20);	// USER_BANK2を有効化
		IMUWriteByte( GYRO_CONFIG_1, 0x17);	// 角速度レンジ±2000dps DLPF enable DLPFCFG = 2
		IMUWriteByte( ACCEL_CONFIG, 0x06);	// 加速度レンジ±16g
		IMUWriteByte( REG_BANK_SEL, 0x00);	// USER_BANK0を有効化
	}

	return ret;
}
///////////////////////////////////////////////////
// モジュール名 caribrateIMU
// 処理概要   	各軸のキャリブレーション
// 引数         なし
// 戻り値       なし
///////////////////////////////////////////////////
void caribrateIMU (void) {
	int32_t sumXg=0, sumYg=0,sumZg=0, test = -10000;
	int16_t test2;
	uint8_t axisData[14];
	// X,Y軸は基板に合わせて入れ替えている

	for (uint16_t i=0;i<SAMPLE;i++) {
		IMUReadArry( GYRO_XOUT_H, 6, axisData);
		sumYg += (int16_t)( (axisData[1] << 8 & 0xff00 ) | axisData[2] );
		sumXg += (int16_t)( (axisData[3] << 8 & 0xff00 ) | axisData[4] );
		sumZg += (int16_t)( (axisData[5] << 8 & 0xff00 ) | axisData[6] );
	}
	
	offsetX = sumXg/SAMPLE;
	offsetY = sumYg/SAMPLE;
	offsetZ = sumZg/SAMPLE;

	printf("x: %d y: %d z: %d\n",offsetX,offsetY,offsetZ);

	TurningAngleIMU = 0;
	RollAngleIMU = 0;
	PichAngleIMU = 0;
}
///////////////////////////////////////////////////
// モジュール名 IMUProcess
// 処理概要   	センセデータの取得
// 引数         なし
// 戻り値       なし
///////////////////////////////////////////////////
void getIMUdata (void)
{
	uint8_t 	axisData[14];	// 角加速度、温度の8bit分割データ格納先
	// X,Y軸は基板に合わせて入れ替えている

	IMUReadArry( ACCEL_XOUT_H, 12, axisData);
	rawYa = (short)( (axisData[1] << 8 & 0xff00 ) | axisData[2] );
	rawXa = (short)( (axisData[3] << 8 & 0xff00 ) | axisData[4] );
	rawZa = (short)( (axisData[5] << 8 & 0xff00 ) | axisData[6] );

 	rawYg = (short)( (axisData[7] << 8 & 0xff00 ) | axisData[8] ) - offsetY;
	rawXg = (short)( (axisData[9] << 8 & 0xff00 ) | axisData[10] ) - offsetX;
	rawZg = (short)( (axisData[11] << 8 & 0xff00 ) | axisData[12] ) - offsetZ;

	// rawXa = (short)( (IMUReadByte(0x2d) << 8 & 0xff00 ) | IMUReadByte(0x2e) );
	// rawYa = (short)( (IMUReadByte(0x2f) << 8 & 0xff00 ) | IMUReadByte(0x30) );
	// rawZa = (short)( (IMUReadByte(0x31) << 8 & 0xff00 ) | IMUReadByte(0x32) );
}
///////////////////////////////////////////////////
// モジュール名 getTurningAngleIMU
// 処理概要   	yaw軸の角度を算出
// 引数         なし
// 戻り値       なし
///////////////////////////////////////////////////
void getTurningAngleIMU(void) {
	double angularVelocity_zg;

	angularVelocity_zg = (double)(rawZg) / GYROLSB;	// IMUのデータを角速度[deg/s]に変換
	TurningAngleIMU += (double)( angularVelocity_zg) * DELTATIMU;
}
///////////////////////////////////////////////////
// モジュール名 getRollAngleIMU
// 処理概要   	roll軸の角度を算出
// 引数         なし
// 戻り値       なし
///////////////////////////////////////////////////
void getRollAngleIMU(void)
{
	double angularVelocity_yg;

	angularVelocity_yg = (double)(rawXg) / GYROLSB;	// IMUのデータを角速度[deg/s]に変換
	RollAngleIMU -= (double)( angularVelocity_yg) * DELTATIMU;
}
///////////////////////////////////////////////////
// モジュール名 getPichAngleIMU
// 処理概要   	pich軸の角度を算出
// 引数         なし
// 戻り値       なし
///////////////////////////////////////////////////
void getPichAngleIMU( void )
{
	double angularVelocity_xg;
	
	angularVelocity_xg = (double)(rawYg) / GYROLSB;	// IMUのデータを角速度[deg/s]に変換
	PichAngleIMU -= (double)( angularVelocity_xg) * DELTATIMU;
}