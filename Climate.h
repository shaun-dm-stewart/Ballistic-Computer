// Climate.h

#ifndef _Climate_h
#define _Climate_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Structs.h"

BME280I2C bme;

bool getConditions(WeatherCondition* weather)
{
	bool result = true;
	float temp, hum, pres;

	BME280::TempUnit tempUnit(BME280::TempUnit_Fahrenheit);
	BME280::PresUnit presUnit(BME280::PresUnit_inHg);

	bme.read(pres, temp, hum, tempUnit, presUnit);
	weather->Barometer = pres;
	weather->Temperature = temp;
	weather->RelativeHumidity = (hum / 100.0);

	return result;
}

#endif

