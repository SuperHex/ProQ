#include "libmpu.h"

void initAcc() {
  //调用 ADXL345
  writeTo(ACC, 0x2D, 0);
  writeTo(ACC, 0x2D, 16);
  writeTo(ACC, 0x2D, 8);
  //设定在 +-2g 时的默认读数
}

void getAccelerometerData(double * result) {
  int regAddress = 0x32;    //加速度传感器ADXL345第一轴的数据的设定
  byte buff[A_TO_READ];

  readFrom(ACC, regAddress, A_TO_READ, buff); //读取加速度传感器ADXL345的数据

  //每个轴的读数有10位分辨率，即2个字节.
  //我们要转换两个bytes为一个int变量
  result[0] = ((((int)buff[1]) << 8) | buff[0] + a_offx) / 256.0;
  result[1] = ((((int)buff[3]) << 8) | buff[2] + a_offy) / 256.0;
  result[2] = ((((int)buff[5]) << 8) | buff[4] + a_offz) / 256.0;
}

//初始化陀螺仪
void initGyro()
{
  /*****************************************
   * ITG 3200
   * 电源管理设定：
   * 时钟选择 =内部振荡器
   * 无复位, 无睡眠模式
   * 无待机模式
   * 采样率 = 125Hz
   * 参数为+ / - 2000度/秒
   * 低通滤波器=5HZ
   * 没有中断
   ******************************************/
  writeTo(GYRO, G_PWR_MGM, 0x00);
  writeTo(GYRO, G_SMPLRT_DIV, 0x07); // EB, 50, 80, 7F, DE, 23, 20, FF
  writeTo(GYRO, G_DLPF_FS, 0x1E); // +/- 2000 dgrs/sec, 1KHz, 1E, 19
  writeTo(GYRO, G_INT_CFG, 0x00);
}


void getGyroscopeData(double * result)
{
  /**************************************
   * 陀螺仪ITG- 3200的I2C
   * 寄存器：
   * temp MSB = 1B, temp LSB = 1C
   * x axis MSB = 1D, x axis LSB = 1E
   * y axis MSB = 1F, y axis LSB = 20
   * z axis MSB = 21, z axis LSB = 22
   *************************************/

  int regAddress = 0x1B;
  int temp, x, y, z;
  byte buff[G_TO_READ];

  readFrom(GYRO, regAddress, G_TO_READ, buff); //读取陀螺仪ITG3200的数据

  result[0] = (((buff[2] << 8) | buff[3]) + g_offx) / 14.375;
  result[1] = (((buff[4] << 8) | buff[5]) + g_offy) / 14.375;
  result[2] = (((buff[6] << 8) | buff[7]) + g_offz) / 14.375;
  result[3] = (buff[0] << 8) | buff[1]; // 温度

}

void initMagnet(){
  writeTo(MAGNET, M_MODE, 0x00);
}

void getMagnetData(double *result){
  int regAddress = 0x03;
  byte buff[M_TO_READ];

  readFrom(MAGNET, regAddress, M_TO_READ, buff);

  result[0] = (((buff[0] << 8) | buff[1]) + m_offx);
  result[2] = (((buff[2] << 8) | buff[3]) + m_offy);
  result[1] = (((buff[4] << 8) | buff[5]) + m_offz);

}
