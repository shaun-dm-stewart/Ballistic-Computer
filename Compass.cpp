#include "Compass.h"

Compass::Compass()
{

}

void Compass::begin()
{
	myIMU.begin();
}

/// <summary>
/// The acceleromater takes the longest time to calibrate so when it goe to 3 we are good to go.
/// </summary>
/// <returns>true if the accelerometer is calibrated</returns>
bool Compass::calibrate()
{
	myIMU.getCalibration(&system, &gyro, &accel, &mag);
	return accel == 3 ? true : false;;
}

/// <summary>
/// Reads the current calibration status of the device
/// </summary>
/// <param name="calibrationStatus"></param>
void Compass::getCalibrationState(CalibrationStatus* calibrationStatus)
{
	calibrationStatus->system = system;
	calibrationStatus->accel = accel;
	calibrationStatus->gyro = gyro;
	calibrationStatus->mag = mag;
}

/// <summary>
/// Calculates pitch roll and heading
/// </summary>
void Compass::calculate()
{
	imu::Vector<3> acc = myIMU.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
	imu::Vector<3> gyr = myIMU.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
	imu::Vector<3> mag = myIMU.getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER);

	thetaM = -atan2(acc.x() / 9.8, acc.z() / 9.8) / 2 / M_PI * 360;
	thetaFnew = 0.95 * thetaFold + 0.05 * thetaM;

	phiM = -atan2(acc.y() / 9.8, acc.z() / 9.8) / 2 / M_PI * 360;
	phiFnew = 0.95 * phiFold + 0.05 * phiM;

	dt = (millis() - millisOld) / 1000.;
	millisOld = millis();

	theta = (theta + gyr.y() * dt) * 0.95 + thetaM * 0.05;
	thetaG = thetaG + gyr.y() * dt;

	phi = (phi - gyr.x() * dt) * 0.95 + phiM * 0.05;
	phiG = phiG - gyr.x() * dt;

	phiRad = phi / 360 * (2 * M_PI);
	thetaRad = theta / 360 * (2 * M_PI);

	Xm = mag.x() * cos(thetaRad) - mag.y() * sin(phiRad) * sin(thetaRad) + mag.z() * cos(phiRad) * sin(thetaRad);
	Ym = mag.y() * cos(phiRad) + mag.z() * sin(phiRad);

	psi = atan2(Ym, Xm) / (2 * M_PI) * 360;

	thetaFold = thetaFnew;
	phiFold = phiFnew;

	// Leave this out because there will be enough to be going on with elsewhere
	// delay(BNO055_SAMPLERATE_DELAY_MS);
}

/// <summary>
/// Gets the inclination of the target in degrees
/// </summary>
/// <returns> 0 to 359 degrees</returns>
float Compass::getSlope()
{
	//TODO work out if this is x or y.  It will depend upon the orientation of the device
	return 0.0;
}

/// <summary>
/// Gets the azimuth of the target
/// </summary>
/// <returns>returns 0 to 359 degrees</returns>
double Compass::getAzimuth()
{
	return psi;
}

/// <summary>
/// gets the pitch
/// </summary>
/// <returns></returns>
float Compass::getPitch()
{
	return theta;
}

float Compass::getRoll()
{
	return phi;
}
