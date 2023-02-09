/*
 * MPU9250.c
 *
 *  Created on: Mar 19, 2022
 *      Author: naokichi
 */

#include "MPU9250.h"

uint8_t MPU9250_Read_Byte(uint8_t reg)
{
    uint8_t rx_data[2];
    uint8_t tx_data[2];

    tx_data[0] = reg | 0x80;
    tx_data[1] = 0x00;  // dummy

    HAL_GPIO_WritePin(SPI3_CS_GPIO_Port, SPI3_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive(&hspi3, tx_data, rx_data, 2, 1);
    HAL_GPIO_WritePin(SPI3_CS_GPIO_Port, SPI3_CS_Pin, GPIO_PIN_SET);

    return rx_data[1];
}

void MPU9250_Write_Byte(uint8_t reg, uint8_t data)
{
  uint8_t rx_data[2];
  uint8_t tx_data[2];

  tx_data[0] = reg & 0x7F;
  tx_data[1] = data;  // write data

  HAL_GPIO_WritePin(SPI3_CS_GPIO_Port, SPI3_CS_Pin, GPIO_PIN_RESET);
  HAL_SPI_TransmitReceive(&hspi3, tx_data, rx_data, 2, 1);
  HAL_GPIO_WritePin(SPI3_CS_GPIO_Port, SPI3_CS_Pin, GPIO_PIN_SET);
}

void MPU9250_Init()
{
	MPU9250_Write_Byte(PWR_MGMT_1, 0x00);
	MPU9250_Write_Byte(PWR_MGMT_1, 0x01);
	MPU9250_Write_Byte(PWR_MGMT_2, 0x00);
	MPU9250_Write_Byte(CONFIG, 0x03);
	MPU9250_Write_Byte(GYRO_CONFIG, 0x18);
	MPU9250_Write_Byte(ACCEL_CONFIG, 0x18);
}

void MPU9250_Calibration()
{
	gz_offset = 0;
	for(int i=0; i<1000; i++){
		int16_t raw = (int16_t)((int16_t)(MPU9250_Read_Byte(GYRO_ZOUT_H) << 8) | MPU9250_Read_Byte(GYRO_ZOUT_L));
		float gz = (float)(raw) / 16.4f;
		gz_offset += gz;
		HAL_Delay(1);
	}
	gz_offset /= 1000.0f;
}

float MPU9250_Read_GyroZ()
{
	int16_t raw = (int16_t)((int16_t)(MPU9250_Read_Byte(GYRO_ZOUT_H) << 8) | MPU9250_Read_Byte(GYRO_ZOUT_L));
	float gz = ((float)(raw) / 16.4f) - gz_offset;
	return gz;
}
