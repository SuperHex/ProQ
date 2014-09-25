#ifndef __Filter__
#define __Filter__

#include <math.h>

#define pi (57.3) // = 180 / 3.1415926

const double wGyro = 2.5;
const double dt    = 0.004;

static double angle[3];

void angle_init(double acc[3])
{
	double R_acc = sqrt(acc[0] * acc[0] + acc[1] * acc[1] + acc[2] * acc[2]);

	acc[0]  /=  R_acc;
	acc[1]  /=  R_acc;
	acc[2]  /=  R_acc;

	angle[0] = atan2(acc[1],acc[2]) * pi;
	angle[1] = atan2(acc[0],acc[2]) * pi;
	angle[2] = atan2(acc[0],acc[1]) * pi;
}


void filter(double acc[3], double gyro[3], double mag[3])
{
	double R_acc = sqrt(acc[0] * acc[0] + acc[1] * acc[1] + acc[2] * acc[2]);

	acc[0]  /=  R_acc;
	acc[1]  /=  R_acc;
	acc[2]  /=  R_acc;
	
	angle[0] = (atan2(acc[1], acc[2]) * pi  +  (angle[0] + gyro[0] * dt) * wGyro) / (1 + wGyro);
	angle[1] = (atan2(acc[0], acc[2]) * pi  +  (angle[1] + gyro[1] * dt) * wGyro) / (1 + wGyro);
	angle[2] = (atan2(mag[1], mag[0]) * pi  +  (angle[2] + gyro[2] * dt) * wGyro) / (1 + wGyro);
}

#endif