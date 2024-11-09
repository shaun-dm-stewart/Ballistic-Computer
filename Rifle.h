// Rifle.h

#ifndef _Rifle_h
#define _Rifle_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include "Structs.h"
#include "Ballistics.h"

class Rifle
{
public:
	Cartridge Ammo;
	WeatherCondition ZeroingConditions;
	double BarrelTwist;
	char Name[20];
	Rifle(RifleInfo* rifleInfo, Cartridge* ammoInfo, ShotSolution* sln);
	Rifle();
	void Initialize(RifleInfo* rifleInfo, Cartridge* ammoInfo, ShotSolution* sln);
	int Solve(double shootingAngle, double windSpeed, double windAngle, double range, WeatherCondition* atmInfo, ShotLocationInfo* shotLocationInfo);
private:
	Ballistics _ballistics;
	ShotSolution* _shotSln;
	BallisticSolution _sln;
	RifleInfo* _rifleInfo;
	Cartridge* _ammoInfo;
	double _zeroAngle;
};
#endif
