#ifndef __Control__
#define __Control__

#include <PID_v1.h>
#include <Servo.h>
#include <Arduino.h>

#define X 0
#define Y 1
#define Z 2

#define up_left    3
#define up_right   4
#define down_left  5
#define down_right 6

#define LF 0
#define RF 1
#define LB 2
#define RB 3

#define kp 2
#define ki 5
#define kd 1

/***************************************
  ---            ---
 | 0 |          | 1 |           ^ y
  ---            ---            |
     \         /                |
      ---------               + | -
     |pin3 pin4|                |           -
     |         |         z <--- |
     |         |                \--------------> x
     |pin5 pin6|                |     
	  ---------               + | -         +
	 /         \
  ---            ---
 | 2 |          | 3 |
  ---            ---
 ******************************************/

extern double angle[3];
extern Servo motor[4];

void initMotor(){
	motor[0].attach(up_left);
	motor[1].attach(up_right);
	motor[2].attach(down_left);
	motor[3].attach(down_right);
}

void TrueControl(double Roll, double Pitch, double Yaw){
	unsigned int motorControl[4];
	double PID_output[3];

	double throttle[4] = { 900, 900,
						   900, 900 };

	double f_roll[4]     = { 1, 1,
							 1, 1 };

	double f_pitch[4]    = { 1, 1,
							 1, 1};

	double f_yaw[4]      = { 1, 1,
							 1, 1};

	double X_setpoint = map(Roll,  -180, 180, -30, 30);
	double Y_setpoint = map(Pitch, -180, 180, -30, 30);
	double Z_setpoint = map(Yaw,   -180, 180, -180,180);

	PID PIDroll  (&angle[X], &PID_output[X], &X_setpoint, kp, ki, kd, DIRECT);
	PID PIDpitch (&angle[Y], &PID_output[Y], &Y_setpoint, kp, ki, kd, DIRECT);
	PID PIDyaw   (&angle[Z], &PID_output[Z], &Z_setpoint, kp, ki, kd, DIRECT);

	PIDroll.SetMode(AUTOMATIC);
	PIDpitch.SetMode(AUTOMATIC);
	PIDyaw.SetMode(AUTOMATIC);

	PIDroll.Compute();
	PIDpitch.Compute();
	PIDyaw.Compute();

	motorControl[LF] = (int) (throttle[LF]                 
							 + 		f_roll[LF]  * PID_output[X]
							 + 		f_pitch[LF] * PID_output[Y]
							 + 		f_yaw[LF]   * PID_output[Z]);
	motorControl[RF] = (int) (throttle[RF]
							 +		f_roll[RF]  * PID_output[X]
							 - 		f_pitch[RF] * PID_output[Y]
							 - 		f_yaw[RF]   * PID_output[Z]);
	motorControl[LB] = (int) (throttle[LB]
							 - 		f_roll[LB]  * PID_output[X] 
							 + 		f_pitch[LB] * PID_output[Y]
							 - 		f_yaw[LB]   * PID_output[Z]);
	motorControl[RB] = (int) (throttle[RB]
							 - 		f_roll[RB]  * PID_output[X]
							 - 		f_pitch[RB] * PID_output[Y]
							 +		f_yaw[RB]   * PID_output[Z]);

	for(int i = 0; i < 4; i++)
	{
		motor[i].writeMicroseconds(motorControl[i]);
	}
	
	delay(20);
}

#endif
