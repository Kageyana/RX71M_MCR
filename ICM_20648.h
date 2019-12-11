//ICM_20648.h Ver.1.0
#ifndef ICM_20648_H
#define ICM_20648_H

#include "main.h"

extern SPI_HandleTypeDef hspi1;
// IMUから取得したデータ
extern volatile uint16_t 	xa, ya, za; // 加速度(16bitデータ)
extern volatile uint16_t 	xg, yg, zg;	// 角加速度(16bitデータ)

#define CS_RESET 		R_PG_IO_PORT_Write_P05( 0x00 )
#define CS_SET   		R_PG_IO_PORT_Write_P05( 0x0 )
#define SPITRANSFER 	R_PG_SCI_SPIMode_Transfer_C0(ret, val, 1)

uint8_t read_byte( uint8_t );
void write_byte( uint8_t, uint8_t);
uint8_t IMU_init(void);
void read_gyro_data(void);
void read_accel_data(void);

/*
Gyro		レンジ(dps)	スケールファクター(LSB/dps)　
					±250				131
					±500				65.5
					±1000				32.8
					±2000				16.4
Accel		レンジ(g)		スケールファクター(LSB/g)
					±2					16384
					±4					8192
					±8					4096
					±16					2048
*/

#endif
