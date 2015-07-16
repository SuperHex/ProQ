#include <Wire.h>  // 调用I2C库
#include <Servo.h>
#include "PID_v1.h"
typedef unsigned char byte;
#include "Filter.h"
#include "Control.h"
//#include "libmpu.c"
//-------------------
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

// 加速度传感器 ADXL345
#define ACC          0x53    // 定义ADXL345地址
#define A_TO_READ    6       // 读取每次占用的字节数 (每个坐标轴占两个字节)
 
// 陀螺仪 ITG3200 
#define GYRO         0x68   // ITG3200地址,将AD0连接到GND口，传感器地址为二进制数11101000
#define G_SMPLRT_DIV 0x15
#define G_DLPF_FS    0x16
#define G_INT_CFG    0x17
#define G_PWR_MGM    0x3E
#define G_TO_READ    8   // x,y,z 每个轴2 bytes

// 地磁传感器HMC488l
#define MAGNET       0x1E
#define M_MODE       0x02
#define M_TO_READ    6
//--------------------------
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
//---------------------------

Servo motor[4];
//全局变量姿态角的导入
extern double angle[3];
extern const double dt;

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
//  Serial.print(acc[0]);
//  Serial.print(",");
//  Serial.print(acc[1]);
//  Serial.print(",");
//  Serial.print(acc[2]);
//  Serial.print(",");
//Serial.print("---------------------\n");
  Serial.print(angle[0]);
  Serial.print(",   ");
  Serial.print(angle[1]);
  Serial.print(",   ");
  Serial.print(angle[2]);
  Serial.print(",\n");

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
