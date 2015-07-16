#include <Wire.h>  // 调用I2C库
#include <Servo.h>
#include <PID_v1.h>
#include "Filter.h"
#include "Control.h"
#include "libmpu.h"

Servo motor[4];
//全局变量姿态角的导入
extern double angle[3];
extern const double dt;

// 陀螺仪误差修正的偏移量
int g_offx = 0;
int g_offy = 0;
int g_offz = 0;

// 加速度传感器误差修正的偏移量
int a_offx = 0;
int a_offy = 0;
int a_offz = 0;

// 地磁仪误差修正偏移量
int m_offx = 0;
int m_offy = 0;
int m_offz = 0;


void setup()
{
  Serial.begin(9600);
  Wire.begin();
  initAcc();
  initGyro();
  initMagnet();
  initMotor();
}


void loop()
{
  double acc[3];
  double gyro[4];
  double magnet[3];
  int val;

  getAccelerometerData(acc);
  getGyroscopeData(gyro);
  getMagnetData(magnet);


  // state_update(gyro[1]);
  // kalman_update(acc[0],acc[2]);
  // Serial.print(angle);
  // Serial.write(byte(10));
  //sprintf(str, "%f,%f,%f,%f,%f,%f,%f", acc[0], acc[1], acc[2], gyro[0], gyro[1], gyro[2], gyro[3]);
  angle_init(acc);
  filter(acc,gyro,magnet);

  TrueControl(0,0,0);
  // Serial.print(acc[0]);
  // Serial.print(",");
  // Serial.print(acc[1]);
  // Serial.print(",");
  // Serial.print(acc[2]);
  // Serial.print(",");
//  Serial.print(angle[0]);
//  Serial.print(",   ");
//  Serial.print(angle[1]);
//  Serial.print(",   ");
//  Serial.print(angle[2]);
//  Serial.print(",\n");

  //延时50毫秒
  delay(dt);
}


//---------------- 功能
//将val写入到加速度传感器的地址寄存器中
void writeTo(int DEVICE, byte address, byte val) {
  Wire.beginTransmission(DEVICE); //传送到加速度传感器
  Wire.write(address);        // 发送寄存器地址
  Wire.write(val);        // 发送要写入的值
  Wire.endTransmission(); //结束传输
}


//加速度传感器在地址寄存器的缓冲区阵列中读取读数
void readFrom(int DEVICE, byte address, int num, byte buff[]) {
  Wire.beginTransmission(DEVICE); //开始传送至加速度传感器
  Wire.write(address);        //发送读取的地址
  Wire.endTransmission(); //结束传输

  Wire.beginTransmission(DEVICE); //开始传送到ACC
  Wire.requestFrom(DEVICE, num);    // 要求从加速度传感器中发送6个字节的数据

  int i = 0;
  while(Wire.available())    //当加速度传感器返回的数据小于要求值时（异常情况）
  {
    buff[i] = Wire.read(); // 接收数据
    i++;
  }
  Wire.endTransmission(); //结束传输
}
