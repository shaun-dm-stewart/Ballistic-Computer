/*
 Name:		Ballistic_Computer.ino
 Created:	11/9/2024 2:50:29 PM
 Author:	Shaun Stewart
*/

#include <Wire.h>
#include "Structs.h"
#include "Rifle.h"
#include <BME280I2C.h>

RifleInfo rifleInfo;
Cartridge cartridge;
ShotLocationInfo shotLocationInfo;
WeatherCondition weatherCondition;
ShotSolution shotSolution;
BME280I2C bme;

#define I2C_SDA 21
#define I2C_SCL 22 
#define SERIAL_BAUD 9600

#define DEBUG

#ifdef DEBUG 
#define debugln(x) Serial.println(x)
#define debug(x) Serial.print(x)
#define debugSerial(x) Serial.begin(x)
#else
#define debugln(x)
#define debug(x)
#define debugSerial(x)
#endif

void getEnvironment()
{
	float temp(NAN), hum(NAN), pres(NAN);

	BME280::TempUnit tempUnit(BME280::TempUnit_Fahrenheit);
	BME280::PresUnit presUnit(BME280::PresUnit_inHg);

	bme.read(pres, temp, hum, tempUnit, presUnit);
	weatherCondition.Barometer = pres;
	weatherCondition.Temperature = temp;
	weatherCondition.RelativeHumidity = (hum / 100.0);
	debug("Temp: ");
	debug(weatherCondition.Temperature);
	debug("�" + String(tempUnit == BME280::TempUnit_Celsius ? 'C' : 'F'));
	debug("\t\tHumidity: ");
	debug(weatherCondition.RelativeHumidity);
	debug("% RH");
	debug("\t\tPressure: ");
	debug(weatherCondition.Barometer);
	debugln(" in Hg");
}

void setup() 
{
	// Remove for live
	rifleInfo.TwistRate = 11.25;
	rifleInfo.ZeroingConditions.Altitude = 0;
	rifleInfo.ZeroingConditions.Barometer = 29.929494;
	rifleInfo.ZeroingConditions.Temperature = 85.999999999999;
	rifleInfo.ZeroingConditions.RelativeHumidity = 0.5;
	rifleInfo.ScopeHeight = 1.5748;
	rifleInfo.ElevationClicksPerMOA = 1;
	rifleInfo.WindageClicksPerMOA = 0.5;
	rifleInfo.ZeroDistance = 109.361;

	cartridge.MuzzleVelocity = 2591.86;
	cartridge.BC = 0.505;
	cartridge.DragFunc = G1;
	cartridge.WeightGrains = 175;
	cartridge.BulletLength = 1.24;
	cartridge.Caliber = 0.308;

	shotLocationInfo.Latitude = 45;
	shotLocationInfo.ShotAzimuth = 270;

	weatherCondition.Altitude = 0;
	weatherCondition.Barometer = 29.929494;
	weatherCondition.RelativeHumidity = 0.5;
	weatherCondition.Temperature = 85.999999999999;

	Rifle rifle(&rifleInfo, &cartridge, &shotSolution);
	rifle.Solve(
		0.0, //shooting angle
		6.2137119223733395,
		90, //wind direction angle (degrees)
		546.807,
		&weatherCondition,
		&shotLocationInfo);
	Serial.printf("Vertical MOA correction %f\n", shotSolution.VerticalMOA);
	// End of remove for live

#ifdef DEBUG
	Serial.begin(SERIAL_BAUD);
	while (!Serial) {} // Wait
#endif
	Wire.begin(I2C_SDA, I2C_SCL);
	while (!bme.begin())
	{
		delay(1000);
	}
}

void loop() {
	getEnvironment();
	delay(1000);
}
