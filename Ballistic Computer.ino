/*
 Name:		Ballistic_Computer.ino
 Created:	11/9/2024 2:50:29 PM
 Author:	Shaun Stewart
*/

#include <Adafruit_BNO055.h>
#include <HardwareSerial.h>
#include <TinyGPS++.h>
#include <TinyGPSPlus.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include <Wire.h>
#include <BME280I2C.h>
#include <BME280.h>
#include <math.h>
#include "Constants.h"
#include "Structs.h"
#include "Rifle.h"
#include "DatabaseService.h"
#include "Climate.h"
#include "GPS.h"
#include "NextionComms.h"
#include "Compass.h"

#define DEBUG

Rifle rifle;
RifleData rifleData;
IndexItem rifleIndex[MAXRIFLECOUNT];
IndexItem cartIndex[MAXCARTRIDGECOUNT];
RifleInfo rifleInfo;
int rifleCount = 0;
int rifleSelected = 0;
CartridgeData cartridgeData;
int cartridgeCount = 0;
int cartridgeSelected = 0;
CartridgeInfo cartridgeInfo;
ShotLocationInfo shotLocationInfo;
WeatherCondition weatherCondition;
ShotSolution shotSolution;
DatabaseService dbService;
GPS gps;
DateAndTime dateAndTime;
ComputerState computerState = IDLE;
NextionComms nextionComms;
Compass compass;
CalibrationStatus calState;

#ifdef DEBUG 
#define debugln(x) Serial.println(x)
#define debug(x) Serial.print(x)
#define debugSerial(x) Serial.begin(x)
#else
#define debugln(x)
#define debug(x)
#define debugSerial(x)
#endif

void setup()
{
#ifdef DEBUG
	rifleInfo.TwistRate = 11.25;
	rifleInfo.ZeroingConditions.Altitude = 0;
	rifleInfo.ZeroingConditions.Barometer = 29.929494;
	rifleInfo.ZeroingConditions.Temperature = 85.999999999999;
	rifleInfo.ZeroingConditions.RelativeHumidity = 0.5;
	rifleInfo.ScopeHeight = 1.5748;
	rifleInfo.ElevationClicksPerMOA = 1;
	rifleInfo.WindageClicksPerMOA = 0.5;
	rifleInfo.ZeroDistance = 109.361;

	cartridgeInfo.MuzzleVelocity = 2591.86;
	cartridgeInfo.BC = 0.505;
	cartridgeInfo.DragFunc = G1;
	cartridgeInfo.WeightGrains = 175;
	cartridgeInfo.BulletLength = 1.24;
	cartridgeInfo.Caliber = 0.308;

	shotLocationInfo.Latitude = 45;
	shotLocationInfo.ShotAzimuth = 270;

	weatherCondition.Altitude = 0;
	weatherCondition.Barometer = 29.929494;
	weatherCondition.RelativeHumidity = 0.5;
	weatherCondition.Temperature = 85.999999999999;
#endif

#ifdef DEBUG
	Serial.begin(SERIAL_BAUD);
	while (!Serial) {} // Wait
#endif
    // Button pin
    pinMode(BUTTON_PIN, INPUT_PULLDOWN);

	// GPS connection
    Serial1.begin(SERIAL_BAUD, SERIAL_8N1, GPS_RXD1, GPS_TXD1);
	while (!Serial1) {} // Wait

	// Nextion connection
	Serial2.begin(SERIAL_BAUD, SERIAL_8N1, NEX_RXD1, NEX_TXD1);
	while (!Serial2) {} // Wait

	Wire.begin(SDAPIN, SCLPIN);

    if (!dbService.begin())
    {
        debugln("Failed to start the database service");
    }

	while (!bme.begin())
	{
		delay(1000);
	}

	gps.begin(&Serial1);
	
	nextionComms.begin(&Serial2);

    compass.begin();

    debugln("");
}

//void getFiringSolution()
//{
//	Rifle rifle(&rifleInfo, &cartridgeInfo, &shotSolution);
//	rifle.Solve(
//		0.0, //shooting angle
//		6.2137119223733395,  //Wind speed
//		90, //wind direction angle (degrees)
//		546.807,
//		&weatherCondition,
//		&shotLocationInfo);
//	debug("Vertical MOA correction: ");
//	debugln(shotSolution.VerticalMOA);
//	getConditions(&weatherCondition);
//	debug("Temp: ");
//	debug(weatherCondition.Temperature);
//	debug("�F");
//	debug("\t\tHumidity: ");
//	debug(weatherCondition.RelativeHumidity);
//	debug("% RH");
//	debug("\t\tPressure: ");
//	debug(weatherCondition.Barometer);
//	debugln(" in Hg");
//}

//void demoTheRest()
//{
//	getFiringSolution();
//	int count = dbService.loadRifleIndex(rifleIndex);
//	for (int i = 0; i < count; i++)
//	{
//		Serial.println(rifleIndex[i].desc);
//	}
//
//	count = dbService.loadCartridgeIndex(rifleIndex[0].id, cartIndex);
//	for (int i = 0; i < count; i++)
//	{
//		Serial.println(cartIndex[i].desc);
//	}
//
//	dbService.loadRifleDetail(2, &rifleData);
//
//    Serial.println(rifleData.desc); 0;
//
//	dbService.loadCartridgeDetail(1, 3, &cartridgeData);
//
//	Serial.println(cartridgeData.desc);
//
//	if (gps.getFix())
//	{
//		debug("Latitude: ");
//		debug(gps.getLatitude());
//		debug("\t");
//		debug("Altitude: ");
//		debug(gps.getAltitude());
//		debug("\t");
//		debug("year: ");
//		debug(gps.getDateTime().year);
//		debug("\n");
//	}
//
//	delay(2000);
//}

void loop()
{
    static bool idleFirstRun = true;
    static bool calibrationFirstRun = true;
    static bool rifleFirstRun = true;
    static bool cartridgeFirstRun = true;
    static bool rangeFirstRun = true;
    static bool environmentFirstRun = true;
    static bool isCalibrated = false;
    static bool windageFirstRun = true;
    static bool anglesFirstRun = true;
    static bool solutionFirstRun = true;

    static UIData uiData;

    // Let's get the finite state machine going

    switch (computerState) 
    {
        case IDLE:
            if (idleFirstRun) 
            {
                debugln("Idle");
                idleFirstRun = false;
            }

            if (nextionComms.isMessageInBuffer()) 
            {
                // Run this until the state changes
                nextionComms.getData(&uiData);
                debug("value: ");
                debugln(uiData.value);
                if (uiData.page == 0) {
                    if (uiData.value == 1) {
                        computerState = CALIBRATION;
                        idleFirstRun = true;
                    }
                }
            }

            break;
        case CALIBRATION:
            if (calibrationFirstRun)
            { 
                debugln("Calibrating");
                calibrationFirstRun = false;
                isCalibrated = false;
                nextionComms.sendPageToNextion(1);
                nextionComms.sendStringToNextion("calib.txt", "");
            }

            if (isCalibrated == false) 
            {
                compass.getCalibrationState(&calState);
                Serial.printf("System: %d\tAccel: %d\tGyro: %d\tMag: %d\n", calState.system, calState.accel, calState.gyro, calState.mag);
                if (compass.calibrate())
                {
                    nextionComms.sendStringToNextion("calib.txt", "Calibrated");
                    isCalibrated = true;
                }
            }

            if (isCalibrated)
            {
                if (nextionComms.isMessageInBuffer()) 
                {
                    nextionComms.getData(&uiData);
                    if (uiData.page == 1) {
                        switch (uiData.value) {
                            case 1:
                                computerState = RIFLE;
                                calibrationFirstRun = true;
                                isCalibrated = false;
                                break;
                        }
                    }
                }
            }

            break;
        case RIFLE:
            if (rifleFirstRun) 
            {
                // One shots in here on entry to this state
                if (rifleCount == 0) 
                {
                    debugln("Rifle");
                    rifleCount = dbService.loadRifleIndex(rifleIndex);
                    rifleSelected = 0;
                    debugln(rifleCount);
                }
                if (rifleCount > 0) 
                {
                    rifleData.id = rifleIndex[rifleSelected].id;
                    strcpy(rifleData.desc, rifleIndex[rifleSelected].desc);
                    nextionComms.sendStringToNextion("globals.riflename.txt", rifleData.desc);
                    nextionComms.sendPageToNextion(2);
                    rifleFirstRun = false;
                }
            }

            if (nextionComms.isMessageInBuffer()) 
            {
                nextionComms.getData(&uiData);
                if (uiData.page == 2) 
                {
                    switch (uiData.value)
                    {
                        case 1:
                            //Scroll left
                            if (rifleSelected > 0) {
                                rifleSelected--;
                            }
                            else {
                                rifleSelected = rifleCount - 1;
                            }
                            rifleData.id = rifleIndex[rifleSelected].id;
                            strcpy(rifleData.desc, rifleIndex[rifleSelected].desc);
                            nextionComms.sendStringToNextion("rifle.t2.txt", rifleData.desc);
                            break;
                        case 2:
                            // scroll right
                            if (rifleSelected < (rifleCount - 1)) {
                                rifleSelected++;
                            }
                            else {
                                rifleSelected = 0;
                            }
                            rifleData.id = rifleIndex[rifleSelected].id;
                            strcpy(rifleData.desc, rifleIndex[rifleSelected].desc);
                            nextionComms.sendStringToNextion("rifle.t2.txt", rifleData.desc);
                            break;
                        case 3:
                            // Back to previous page
                            computerState = CALIBRATION;
                            rifleFirstRun = true;
                            break;
                        case 4:
                            dbService.loadRifleDetail(rifleIndex[rifleSelected].id, &rifleData);    // Load up the details for the selected rifle
                            rifleInfo.TwistRate = rifleData.tr;
                            rifleInfo.ZeroingConditions.Altitude = rifleData.al;
                            rifleInfo.ZeroingConditions.Barometer = rifleData.ap;
                            rifleInfo.ZeroingConditions.Temperature = rifleData.te;
                            rifleInfo.ZeroingConditions.RelativeHumidity = rifleData.rh;
                            rifleInfo.ScopeHeight = rifleData.sh;
                            rifleInfo.ElevationClicksPerMOA = rifleData.ec;
                            rifleInfo.WindageClicksPerMOA = rifleData.wc;
                            rifleInfo.ZeroDistance = rifleData.zd;
                            computerState = CARTRIDGE;
                            rifleFirstRun = true;
                            break;
                        default:
                            break;
                    }
                }
            }

            break;
        case CARTRIDGE:
            if (cartridgeFirstRun == true) 
            {
                debugln("Cartridge");
                if (cartridgeCount == 0) 
                {
                    cartridgeCount = dbService.loadCartridgeIndex(rifleData.id, cartIndex);
                    cartridgeSelected = 0;
                    debugln(cartridgeCount);
                }
            
                if (cartridgeCount > 0) 
                {
                    cartridgeData.id = cartIndex[cartridgeSelected].id;
                    strcpy(cartridgeData.desc, cartIndex[cartridgeSelected].desc);
                    nextionComms.sendStringToNextion("globals.cartname.txt", cartridgeData.desc);
                    nextionComms.sendPageToNextion(3);
                    cartridgeFirstRun = false;
                }
            }

            if (nextionComms.isMessageInBuffer()) 
            {
                nextionComms.getData(&uiData);
                if (uiData.page == 3) 
                {
                    switch (uiData.value) 
                    {
                        case 1:
                            //Scroll left
                            if (cartridgeSelected > 0) {
                                cartridgeSelected--;
                            }
                            else {
                                cartridgeSelected = cartridgeCount - 1;
                            }
                            cartridgeData.id = cartIndex[cartridgeSelected].id;
                            strcpy(cartridgeData.desc, cartIndex[cartridgeSelected].desc);
                            nextionComms.sendStringToNextion("cartridge.t2.txt", cartridgeData.desc);
                            break;
                        case 2:
                            // scroll right
                            if (cartridgeSelected < (cartridgeCount - 1)) {
                                cartridgeSelected++;
                            }
                            else {
                                cartridgeSelected = 0;
                            }
                            cartridgeData.id = cartIndex[cartridgeSelected].id;
                            strcpy(cartridgeData.desc, cartIndex[cartridgeSelected].desc);
                            nextionComms.sendStringToNextion("cartridge.t2.txt", cartridgeData.desc);
                            break;
                        case 3:
                            // Back to previous page
                            computerState = RIFLE;
                            cartridgeFirstRun = true;
                            break;
                        case 4:
                            // navigate to range page
                            dbService.loadCartridgeDetail(rifleIndex[rifleSelected].id, cartIndex[cartridgeSelected].id, &cartridgeData);
                            cartridgeInfo.MuzzleVelocity = cartridgeData.mv;
                            cartridgeInfo.BC = cartridgeData.bc;
                            cartridgeInfo.DragFunc = G1;    // Should have thought of that before.  Hard coded for now.  Maybe select from display  G1 is the biggest anyway (Must be the best ;)
                            cartridgeInfo.WeightGrains = cartridgeData.wt;
                            cartridgeInfo.BulletLength = cartridgeData.bl;
                            cartridgeInfo.Caliber = cartridgeData.clbr;
                            computerState = RANGE;
                            cartridgeFirstRun = true;
                            break;
                        default:
                            break;
                    }
                }
            }

            break;
        case RANGE:
            if (rangeFirstRun) {
                debugln("Range");
                nextionComms.sendIntToNextion("globals.range.val", (int)shotSolution.Range);
                nextionComms.sendPageToNextion(4);
                rangeFirstRun = false;
            }

            if (nextionComms.isMessageInBuffer()) {
                nextionComms.getData(&uiData);
                if (uiData.page == 4) {
                    switch (uiData.value) 
                    {
                        case 1:
                            computerState = CARTRIDGE;
                            rangeFirstRun = true;
                            break;
                        case 2:
                            shotSolution.Range = uiData.range;
                            debugln(shotSolution.Range);
                            computerState = ENVIRONMENT;
                            rangeFirstRun = true;
                            break;
                        default:
                            break;
                    }
                }
            }

            break;
        case ENVIRONMENT:
            if (environmentFirstRun)
            {
                if (gps.getFix())
                {
                    getConditions(&weatherCondition);
                    shotLocationInfo.Latitude = gps.getLatitude();
                    weatherCondition.Altitude = gps.getAltitude();
                    nextionComms.sendPageToNextion(5);
                    nextionComms.sendFloatToNextion("lat.txt", shotLocationInfo.Latitude);
                    nextionComms.sendFloatToNextion("al.txt", weatherCondition.Altitude);
                    nextionComms.sendFloatToNextion("te.txt", weatherCondition.Temperature);
                    nextionComms.sendFloatToNextion("rh.txt", weatherCondition.RelativeHumidity * 100);
                    nextionComms.sendFloatToNextion("ap.txt", weatherCondition.Barometer);
                    environmentFirstRun = false;
                    debugln(shotLocationInfo.Latitude);
                }
            }

            if (nextionComms.isMessageInBuffer())
            {
                nextionComms.getData(&uiData);
                debug("Value: ");
                debugln(uiData.value);
                if (uiData.page == 5)
                {
                    switch (uiData.value)
                    {
                        case 1:
                            computerState = RANGE;
                            environmentFirstRun = true;
                            break;
                        case 2:
                            computerState = WINDAGE;
                            environmentFirstRun = true;
                            break;
                        default:
                            break;
                    }
                }
            }

            break;
        case WINDAGE:
            if (windageFirstRun) 
            {
                debugln("Windage");
                nextionComms.sendIntToNextion("globals.speed.val", (int)shotLocationInfo.WindSpeed);
                nextionComms.sendIntToNextion("globals.direction.val", (int)shotLocationInfo.WindAngle);
                nextionComms.sendPageToNextion(6);
                rangeFirstRun = false;
            }

            if (digitalRead(BUTTON_PIN) == 1)
            {
                int dir = (int)compass.getAzimuth();
                nextionComms.sendIntToNextion("direction.val", dir);
                shotLocationInfo.WindAngle = dir;
            }

            if (nextionComms.isMessageInBuffer())
            {
                nextionComms.getData(&uiData);
                debug("Value: ");
                debugln(uiData.value);
                if (uiData.page == 6)
                {
                    switch (uiData.value)
                    {
                        case 1:
                            computerState = ENVIRONMENT;
                            rangeFirstRun = true;
                            break;
                        case 2:
                            shotLocationInfo.WindSpeed = uiData.speed;
                            computerState = ANGLES;
                            rangeFirstRun = true;
                            break;
                        default:
                            break;
                    }
                }
            }

            break;
        case ANGLES:
            if (anglesFirstRun)
            {
                debugln("Angles");
                nextionComms.sendIntToNextion("globals.azimuth.val", (int)shotLocationInfo.ShotAzimuth);
                nextionComms.sendIntToNextion("globals.direction.val", (int)shotLocationInfo.ShootingAngle);
                nextionComms.sendPageToNextion(7);
                rangeFirstRun = false;
            }


            if (digitalRead(BUTTON_PIN) == 1)
            {
                int inc = (int)compass.getPitch();
                int az = (int)compass.getAzimuth();
                nextionComms.sendIntToNextion("inclination.val", inc);
                nextionComms.sendIntToNextion("azimuth.val", az);
                shotLocationInfo.ShootingAngle = inc;
                shotLocationInfo.ShotAzimuth = az;
            }

            if (nextionComms.isMessageInBuffer())
            {
                nextionComms.getData(&uiData);
                debug("Value: ");
                debugln(uiData.value);
                if (uiData.page == 7)
                {
                    switch (uiData.value)
                    {
                    case 1:
                        computerState = WINDAGE;
                        rangeFirstRun = true;
                        break;
                    case 2:
                        computerState = SOLUTION;
                        rangeFirstRun = true;
                        break;
                    default:
                        break;
                    }
                }
            }

            break;
        case SOLUTION:
            if (anglesFirstRun)
            {
                debugln("Solution");
                rifle.Initialize(&rifleInfo, &cartridgeInfo, &shotSolution);
                rifle.Solve(&weatherCondition, &shotLocationInfo);
                nextionComms.sendIntToNextion("globals.vertClicks.val", (int)shotSolution.VerticalClicks);
                nextionComms.sendIntToNextion("globals.horizClicks.val", (int)shotSolution.HorizontalClicks);
                nextionComms.sendPageToNextion(8);
                rangeFirstRun = false;
            }

            if (nextionComms.isMessageInBuffer())
            {
                nextionComms.getData(&uiData);
                debug("Value: ");
                debugln(uiData.value);
                if (uiData.page == 8)
                {
                    switch (uiData.value)
                    {
                    case 1:
                        computerState = ANGLES;
                        rangeFirstRun = true;
                        break;
                    case 2:
                        computerState = CARTRIDGE;
                        shotLocationInfo.Latitude = 0.0;
                        shotLocationInfo.Range = 0.0;
                        shotLocationInfo.ShootingAngle = 0.0;
                        shotLocationInfo.ShotAzimuth = 0.0;
                        shotLocationInfo.WindAngle = 0.0;
                        shotLocationInfo.WindSpeed = 0.0;
                        rangeFirstRun = true;
                        break;
                    default:
                        break;
                    }
                }
            }

            break;
        default:
            break;
    }
}
