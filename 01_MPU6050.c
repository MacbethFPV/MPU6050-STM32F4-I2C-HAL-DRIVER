/*
 * MPU6050.c
 *
 *  Created on: Oct 6, 2022
 *      Author: mac
 */
#include "MPU6050.h"

#define MPU6050_ADDR 0xD0

#define SMPLRT_DIV_REG 0x19
#define GYRO_CONFIG_REG 0x1B
#define ACCEL_CONFIG_REG 0x1C
#define ACCEL_XOUT_H_REG 0x3B
#define TEMP_OUT_H_REG 0x41
#define GYRO_XOUT_H_REG 0x43
#define PWR_MGMT_1_REG 0x6B
#define WHO_AM_I_REG 0x75

int16_t Accel_X_RAW, Accel_Y_RAW, Accel_Z_RAW = 0;
int16_t Gyro_X_RAW, Gyro_Y_RAW, Gyro_Z_RAW = 0;
float Ax, Ay, Az = 0;
float Gx, Gy, Gz = 0;


void MPU6050_Init (void)
{
  uint8_t check, Data;

  //check device ID WHO_AM_I
  HAL_I2C_Mem_Read (&hi2c1, MPU6050_ADDR, WHO_AM_I_REG, 1, &check, 1, 1000);

  uint8_t buf[64] = { 0 };
  sprintf(buf, "%d, Got check \r\n", check);
  HAL_UART_Transmit(&huart1, buf, sizeof(buf), 10);

  //wake sensor up
  if (check == 104) //if the device is present
  {
    // power management register (0x6B) we should write all 0's to wake the sensor up
    Data = 0;
    HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, PWR_MGMT_1_REG, 1, &Data, 1, 1000);

    // Set the DATA RATE of 1KHz by writing SMPLRT_DIV register
    Data = 0x07;
    HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, SMPLRT_DIV_REG, 1, &Data, 1, 1000);

    // Set accelerometer configuration in ACCEL_CONFIG Register
    // XA_ST=0, YG_ST=0, ZG_ST=0, FS_SEL=0 -> +- 250 deg/s
    Data = 0x00;
    HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, ACCEL_CONFIG_REG, 1, &Data, 1, 1000);

    // Set Gyroscope configuration in GYRO_CONFIG Register
    // XG_ST=0, YG_ST=0, ZG_ST=0, FS_SEL=0 -> +- 250deg/s
    Data = 0x00;
    HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, GYRO_CONFIG_REG, 1, &Data, 1, 1000);

  }
}

void MPU6050_Read_Accel (void)
{
  uint8_t Rec_Data[6];

  // Read 6 BYTES of data starting from ACCEL_XOUT_H register
  HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR, ACCEL_XOUT_H_REG, 1, Rec_Data, 6, 1000);

  Accel_X_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data[1]);
  Accel_Y_RAW = (int16_t)(Rec_Data[2] << 8 | Rec_Data[3]);
  Accel_Z_RAW = (int16_t)(Rec_Data[4] << 8 | Rec_Data[5]);

  /*** To convert the RAW values into acceleration in 'g'
   * we have to divide according to the Full scale value set in FS_SEL
   * I have configured FS_SEL = 0, so I am dividing by 16384.0
   * for more details check ACCEL_CONFIG Register              ****/

  Ax = Accel_X_RAW/16384.0;
  Ay = Accel_Y_RAW/16384.0;
  Az = Accel_Z_RAW/16384.0;
}

void MPU6050_Read_Gyro (void)
{
  uint8_t Rec_Data[6];

  // Read 6 BYTES of data starting from GYRO_XOUT_H register
  HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR, GYRO_XOUT_H_REG, 1, Rec_Data, 6, 1000);

  Gyro_X_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data [1]);
  Gyro_Y_RAW = (int16_t)(Rec_Data[2] << 8 | Rec_Data [3]);
  Gyro_Z_RAW = (int16_t)(Rec_Data[4] << 8 | Rec_Data [5]);

  /*** To convert the RAW values into dps (°/s)
   * we have to divide according to the Full Scale value set in FS_SEL.
   * I have configured FS_SEL = 0, so I am dividing by 131.0
   * for more details check GYRO_CONFIG Register             ****/

  Gx = Gyro_X_RAW / 131.0;
  Gy = Gyro_Y_RAW / 131.0;
  Gz = Gyro_Z_RAW / 131.0;
}


