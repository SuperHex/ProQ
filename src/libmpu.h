#ifndef __libmpu__
#define __libmpu__

/* ********************************************************
 * This module is part of the project "Utopia"
 *
 * libmpu defines some useful functions to get access to
 * micro process unit such as write or read the registor.
 * It also have defined the physical address of the mpu (
 * mpu6050 here for our use )
 * To use this module, simply include it in your main.c
 * *******************************************************/

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


#endif
