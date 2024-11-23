// Compass.h

#ifndef _Compass_h
#define _Compass_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <Wire.h>
#include <Adafruit_BNO055.h>
#include <math.h>

#define BNO055_SAMPLERATE_DELAY_MS (100)

typedef struct
{
	uint8_t system;
	uint8_t gyro;
	uint8_t accel;
	uint8_t mag;
} CalibrationStatus;

class Compass
{
	public:
		Compass();
		void begin();
		bool calibrate();
		void getCalibrationState(CalibrationStatus *calibrationStatus);
		float getPitch();
		float getRoll();
		float getSlope();
		double getAzimuth();
	
	private:
		void calculate();
		Adafruit_BNO055 myIMU = Adafruit_BNO055();
		float theta;
		float thetaM;
		float thetaFold = 0;
		float thetaFnew;
		float thetaG = 0;
		float thetaRad;

		float phi;
		float phiM;
		float phiFold = 0;
		float phiFnew;
		float phiG = 0;
		float phiRad;

		float Xm;
		float Ym;
		float psi;

		float dt;
		unsigned long millisOld;
		uint8_t system = 0;
		uint8_t gyro = 0;
		uint8_t accel = 0;
		uint8_t mag = 0;
};

#endif

