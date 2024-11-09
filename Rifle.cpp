// 
// 
// 

#include "Rifle.h"
Rifle::Rifle(RifleInfo* rifleInfo, Cartridge* ammoInfo, ShotSolution* sln)
{
	Initialize(rifleInfo, ammoInfo, sln);
}

Rifle::Rifle()
{

}

void Rifle::Initialize(RifleInfo* rifleInfo, Cartridge* ammoInfo, ShotSolution* sln)
{
	_rifleInfo = rifleInfo;
	_ammoInfo = ammoInfo;
	_shotSln = sln;
	double zeroDistance = rifleInfo->ZeroDistance;
	double height = rifleInfo->ScopeHeight;
	double muzzleVelocity = ammoInfo->MuzzleVelocity;
	double temperature = rifleInfo->ZeroingConditions.Temperature;
	double barometer = rifleInfo->ZeroingConditions.Barometer;
	double altitude = rifleInfo->ZeroingConditions.Altitude;
	_zeroAngle = _ballistics.ZeroAngle((int)_ammoInfo->DragFunc, _ballistics.AtmCorrect(ammoInfo->BC, altitude, \
		barometer, temperature, _rifleInfo->ZeroingConditions.RelativeHumidity), muzzleVelocity, height, zeroDistance, 0.0);
}

int Rifle::Solve(
	double shootingAngle,
	double windSpeed,
	double windAngle,
	double range,
	WeatherCondition* atmInfo,
	ShotLocationInfo* shotLocationInfo) {

	double bc = _ammoInfo->BC;
	if (atmInfo != nullptr)
	{
		bc = _ballistics.AtmCorrect(_ammoInfo->BC,
			_rifleInfo->ZeroingConditions.Altitude,
			_rifleInfo->ZeroingConditions.Barometer,
			_rifleInfo->ZeroingConditions.Temperature,
			_rifleInfo->ZeroingConditions.RelativeHumidity);
	}

	_ballistics.SolveAll((int)_ammoInfo->DragFunc,
		bc,
		_ammoInfo->MuzzleVelocity,
		_rifleInfo->ScopeHeight,
		shootingAngle,
		_zeroAngle,
		windSpeed,
		windAngle,
		_ammoInfo->BulletLength,
		_ammoInfo->Caliber,
		_ammoInfo->WeightGrains,
		_rifleInfo->TwistRate,
		shotLocationInfo->Latitude,
		shotLocationInfo->ShotAzimuth,
		range,
		&_sln);


	double verticalClickMultiplier = 1 / _rifleInfo->ElevationClicksPerMOA;
	double horizontalClickMultiplier = 1 / _rifleInfo->WindageClicksPerMOA;
	double spinDrift = _sln.SpinDrift;	// _ GetSpinDrift(solution, range);
	double windage = _sln.Windage;		// GetWindage(solution, range);
	_shotSln->Range = _sln.Range;
	_shotSln->BulletDrop = _sln.Path;
	_shotSln->SpinDrift = _sln.SpinDrift;
	_shotSln->WindDrift = windage - spinDrift;
	_shotSln->VerticalMOA = _sln.Correction;
	_shotSln->HorizontalMOA = _sln.WindageMOA;
	_shotSln->TimeToTargetSec = _sln.TimeInSecs;
	_shotSln->ImpactVelocity = _sln.VelocityCombined;
	_shotSln->VerticalClicks = _shotSln->VerticalMOA * verticalClickMultiplier;
	_shotSln->HorizontalClicks = _shotSln->HorizontalMOA * horizontalClickMultiplier;
	return 0;
}
