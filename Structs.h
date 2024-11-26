#pragma once

#ifndef STRUCTS_H
#define STRUCTS_H

enum DragFunction
{
	G1 = 1,
	G2,
	G3,
	G4,
	G5,
	G6,
	G7,
	G8
};

// Somewhere in here we will need to do declination corrections
enum ComputerState
{
	IDLE,
	CALIBRATION,
	RIFLE,
	CARTRIDGE,
	RANGE,
	ENVIRONMENT,
	WINDAGE,
	ANGLES,
	SOLUTION
};

// initial load of rifle and cartridgeInfo data will be ids and descriptions
typedef struct
{
	int id;
	char desc[21];
} IndexItem;

typedef struct
{
	int id;
	char desc[21];
	double sh;
	double tr;
	double zd;
	double ec;
	double wc;
	double al;
	double ap;
	double te;
	double rh;
} RifleData;

typedef struct
{
	int id;
	int rifleid;
	char desc[21];
	double wt;
	double mv;
	double bc;
	double bl;
	double clbr;
} CartridgeData;

typedef struct
{
	double Altitude;
	double Barometer;
	double Temperature;
	double RelativeHumidity;
} WeatherCondition;

typedef struct
{
	char Name[20];
	double TwistRate;
	double ScopeHeight;
	double ZeroDistance;
	double ElevationClicksPerMOA;
	double WindageClicksPerMOA;
	WeatherCondition ZeroingConditions;
}RifleInfo;

typedef struct
{
	DragFunction DragFunc;
	double BC;
	char Name[21];
	double MuzzleVelocity;
	double Caliber;
	double BulletLength;
	double WeightGrains;
}CartridgeInfo;

typedef struct
{
	double BulletDrop;
	double WindDrift;
	double SpinDrift;
	double VerticalMOA;
	double VerticalMils;// = > VerticalMOA / 0.290888;
	double VerticalClicks;
	double HorizontalMOA;
	double HorizontalMils;// = > HorizontalMOA / 0.290888;
	double HorizontalClicks;
	double TimeToTargetSec;
	double ImpactVelocity;
	double Range;
} ShotSolution;

typedef struct
{
	double Path;
	double Correction;
	double Range;
	double TimeInSecs;
	double Windage;
	double WindageMOA;
	double VelocityCombined;
	double VelocityX;
	double VelocityY;
	double SpinDrift;

} BallisticSolution;

typedef struct
{
	double Latitude;
	double ShotAzimuth;
	double WindAngle;
	double WindSpeed;
	double ShootingAngle;
	double Range;
} ShotLocationInfo;

typedef struct UIData
{
	int page = 0;
	int value = 0;
	int range = 0;
	int speed = 0;
	int direction = 0;
};
#endif
