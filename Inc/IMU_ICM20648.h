#ifndef I2C_MPU9250_H_
#define I2C_MPU9250_H_
//====================================//
// インクルード                  	   //
//====================================//
#include "r_smc_entry.h"
//====================================//
// シンボル定義						   //
//====================================//
// MPU-9255 Register Map
#define WHO_AM_I	 	0x00
#define USER_CTRL		0x03
#define PWR_MGMT_1		0x06
#define REG_BANK_SEL 	0x7F
#define GYRO_CONFIG_1 	0x01
#define ACCEL_CONFIG 	0x14
#define GYRO_XOUT_H 	0x33
#define ACCEL_XOUT_H 	0x2D

#define MPU9250_ADDRESS_W     	0xd0		// 書き込み時のスレーブアドレス
#define MPU9250_ADDRESS_R    	0xd1		// 読み込み時のスレーブアドレス

#define ACCELLSB			2048	// 16[g]
#define GYROLSB				16.4	// 1000[deg/s]
#define TEMP_LSB			333.87	// LSB/°C
#define ROOMTEMPOFFSET		0		// 21°Cのとき0

#define BUS_IMU_FREE 		0		// 通信可能
#define BUS_IMU_BUSY 		1		// 通信中

// データ処理関連
#define CLOCK				240		// 動作周波数[MHz]

#define MAXDATA_RANGE		32764	// 16bitデータの最大値
#define G_ACCELERATION		9.81	// 重力加速度
#define DELTATIMU			0.005	// 取得周期
#define SAMPLE				5000	// キャリブレーションのサンプル数

/*************************************** 自動生成関数 **********************************/
#define USECOM 			1		// 0:I2c 1:SPI
#if USECOM == 0
	//I2c
	#define I2C_IMU_COMMAND		R_Config_SCI2_IIC_Master_Send( MPU9250_ADDRESS_W, data_tr, num)
	#define I2C_IMU_RECIVE		R_Config_SCI2_IIC_Master_Receive( MPU9250_ADDRESS_R, data_re, num)
	#define I2C_IMU_ARRY		R_Config_SCI2_IIC_Master_Receive(MPU9250_ADDRESS_R, data_re, num)
#else
	//SPI
	#define SPI_IMU_CS			PORTC.PODR.BIT.B6	// CS端子
	#define SPI_IMU_SEND		R_Config_SCI2_SPI_Master_Send_Receive( data_tr, numS, data_re, numR)
#endif
/****************************************************************************************/
//====================================//
// グローバル変数の宣言					//
//====================================//
// IMUから取得したデータ
extern volatile int 	rawXa, rawYa, rawZa;		// 加速度(16bitデータ)
extern volatile int 	rawXg, rawYg, rawZg;	// 角加速度(16bitデータ)
extern volatile short 	rawTemp;				// 温度(16bitデータ)

extern short 	rawXa2, rawYa2, rawZa2;	// 加速度(16bitデータ)
extern short 	rawXg2, rawYg2, rawZg2;	// 角加速度(16bitデータ)

// データ処理
extern double 		TurningAngleIMU;	// IMUから求めた旋回角度
extern double		RollAngleIMU;		// IMUから求めたロール方向角度
extern double 		PichAngleIMU;		// IMUから求めたピッチ方向角度
extern double		TempIMU;			// IMUの温度

// モード関連
extern uint8_t	whoami;
extern uint8_t	cnt_imu;
extern uint8_t	busIMU;
//====================================//
// プロトタイプ宣言					   //
//====================================//
void wait_IMU ( uint16_t waitTime );
void IMUWriteByte( uint8_t reg, uint8_t data );
uint8_t IMUReadByte( uint8_t reg );
void IMUReadArry( uint8_t reg, uint8_t num2, uint8_t* data_re );
bool init_IMU (void);
void getIMUdata (void);
void caribrateIMU (void);
void getTurningAngleIMU(void);
void getRollAngleIMU(void);
void getPichAngleIMU( void );
#endif // I2C_MPU9250_H_