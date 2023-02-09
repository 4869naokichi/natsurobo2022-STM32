/*
 * MPU9250.h
 *
 *  Created on: Mar 19, 2022
 *      Author: naokichi
 *
 *  HALを使用してMPU9250をSPI通信で動かすライブラリです。
 */

#ifndef MPU9250_H_
#define MPU9250_H_

#include "main.h"
#include "spi.h"

//Registers
#define CONFIG              0x1A
#define GYRO_CONFIG         0x1B
#define ACCEL_CONFIG        0x1C
#define I2C_MST_CTRL        0x24
#define I2C_SLV0_ADDR       0x25
#define I2C_SLV0_REG        0x26
#define I2C_SLV0_CTRL       0x27
#define ACCEL_XOUT_H        0x3B
#define ACCEL_XOUT_L        0x3C
#define ACCEL_YOUT_H        0x3D
#define ACCEL_YOUT_L        0x3E
#define ACCEL_ZOUT_H        0x3F
#define ACCEL_ZOUT_L        0x40
#define GYRO_XOUT_H         0x43
#define GYRO_XOUT_L         0x44
#define GYRO_YOUT_H         0x45
#define GYRO_YOUT_L         0x46
#define GYRO_ZOUT_H         0x47
#define GYRO_ZOUT_L         0x48
#define EXT_SENS_DATA_00    0x49
#define EXT_SENS_DATA_01    0x4A
#define EXT_SENS_DATA_02    0x4B
#define EXT_SENS_DATA_03    0x4C
#define EXT_SENS_DATA_04    0x4D
#define EXT_SENS_DATA_05    0x4E
#define I2C_SLV0_DO         0x63
#define USER_CTRL           0x6A
#define PWR_MGMT_1          0x6B
#define PWR_MGMT_2          0x6C

float gz_offset;

uint8_t MPU9250_Read_Byte(uint8_t reg);
void MPU9250_Write_Byte(uint8_t reg, uint8_t data);
void MPU9250_Init();
void MPU9250_Calibration();
float MPU9250_Read_GyroZ();

#endif /* MPU9250_H_ */
