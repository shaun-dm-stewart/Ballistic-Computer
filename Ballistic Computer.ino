/*
 Name:		Ballistic_Computer.ino
 Created:	11/9/2024 2:50:29 PM
 Author:	Shaun Stewart
*/

#include <HardwareSerial.h>
#include <TinyGPS++.h>
#include <TinyGPSPlus.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include <Wire.h>
#include <BME280I2C.h>
#include <BME280.h>
#include "Constants.h"
#include "Structs.h"
#include "Rifle.h"
#include "DatabaseService.h"
#include "Climate.h"
#include "GPS.h"
#include "NextionComms.h"

#define DEBUG

RifleData rifleData;
IndexItem rifleIndex[MAXRIFLECOUNT];
IndexItem cartIndex[MAXCARTRIDGECOUNT];
RifleInfo rifleInfo;
int rifleCount = 0;
int rifleSelected = 0;
CartridgeData cartridgeData;
int cartridgeCount = 0;
int cartridgeSelected = 0;
CartridgeInfo cartridge;
ShotLocationInfo shotLocationInfo;
WeatherCondition weatherCondition;
ShotSolution shotSolution;
DatabaseService dbService;
GPS gps;
DateAndTime dateAndTime;
ComputerState computerState = IDLE;
NextionComms nextionComms;

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
#endif

#ifdef DEBUG
	Serial.begin(SERIAL_BAUD);
	while (!Serial) {} // Wait
#endif
	// GPS connection
	Serial1.begin(SERIAL_BAUD, SERIAL_8N1, GPS_RXD1, GPS_TXD1);
	while (!Serial1) {} // Wait

	// Nextion connection
	Serial2.begin(SERIAL_BAUD, SERIAL_8N1, NEX_RXD1, NEX_TXD1);
	while (!Serial2) {} // Wait

	Wire.begin(SDAPIN, SCLPIN);

	dbService.begin();

	while (!bme.begin())
	{
		delay(1000);
	}

	gps.begin(&Serial1);
	nextionComms.begin(&Serial2);
}

void getFiringSolution()
{
	Rifle rifle(&rifleInfo, &cartridge, &shotSolution);
	rifle.Solve(
		0.0, //shooting angle
		6.2137119223733395,
		90, //wind direction angle (degrees)
		546.807,
		&weatherCondition,
		&shotLocationInfo);
	debug("Vertical MOA correction: ");
	debugln(shotSolution.VerticalMOA);
	getConditions(&weatherCondition);
	debug("Temp: ");
	debug(weatherCondition.Temperature);
	debug("°F");
	debug("\t\tHumidity: ");
	debug(weatherCondition.RelativeHumidity);
	debug("% RH");
	debug("\t\tPressure: ");
	debug(weatherCondition.Barometer);
	debugln(" in Hg");
}

void demoTheRest()
{
	getFiringSolution();
	int count = dbService.loadRifleIndex(rifleIndex);
	for (int i = 0; i < count; i++)
	{
		Serial.println(rifleIndex[i].desc);
	}

	count = dbService.loadCartridgeIndex(rifleIndex[0].id, cartIndex);
	for (int i = 0; i < count; i++)
	{
		Serial.println(cartIndex[i].desc);
	}

	dbService.loadRifleDetail(2, &rifleData);

	Serial.println(rifleData.desc);

	dbService.loadCartridgeDetail(1, 3, &cartridgeData);

	Serial.println(cartridgeData.desc);

	if (gps.getFix())
	{
		debug("Latitude: ");
		debug(gps.getLatitude());
		debug("\t");
		debug("Altitude: ");
		debug(gps.getAltitude());
		debug("\t");
		debug("year: ");
		debug(gps.getDateTime().year);
		debug("\n");
	}

	delay(2000);
}

void loop()
{
	static bool idleFirstRun = true;
	static bool rifleFirstRun = true;
	static bool cartridgeFirstRun = true;
	static bool rangeFirstRun = true;
	static UIData uiData;

	// Let's get the finite state machine going
	switch (computerState)
	{
		case IDLE:
			if (idleFirstRun)
			{
				idleFirstRun = false;
			}
			
			if (nextionComms.isMessageInBuffer())
			{
				// Run this until the state changes
				nextionComms.getData(&uiData);
				if (uiData.page == 0)
				{
					if (uiData.value == 1)
					{
						computerState = RIFLE;
						idleFirstRun = true;
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
					rifleCount = dbService.loadRifleIndex(rifleIndex);
					rifleSelected = 0;
				}
				if (rifleCount > 0)
				{
					rifleData.id = rifleIndex[rifleSelected].id;
					strcpy(rifleData.desc, rifleIndex[rifleSelected].desc);
					nextionComms.sendStringToNextion("globals.riflename.txt", rifleData.desc);
					nextionComms.sendPageToNextion(uiData.value);
					rifleFirstRun = false;
				}
			}

			if (nextionComms.isMessageInBuffer())
			{
				nextionComms.getData(&uiData);
				if (uiData.page == 1)
				{
					switch (uiData.value)
					{
					case 1:
						//Scroll left
						if (rifleSelected > 0)
						{
							rifleSelected--;
						}
						else
						{
							rifleSelected = rifleCount - 1;
						}
						nextionComms.sendStringToNextion("rifle.t2.txt", rifleIndex[rifleSelected].desc);
						rifleData.id = rifleIndex[rifleSelected].id;
						strcpy(rifleData.desc, rifleIndex[rifleSelected].desc);
						break;
					case 2:
						// scroll right
						if (rifleSelected < (rifleCount - 2) )
						{
							rifleSelected++;
						}
						else
						{
							rifleSelected = 0;
						}
						nextionComms.sendStringToNextion("rifle.t2.txt", rifleIndex[rifleSelected].desc);
						rifleData.id = rifleIndex[rifleSelected].id;
						strcpy(rifleData.desc, rifleIndex[rifleSelected].desc);
						break;
					case 3:
							// navigate to cartridge page
							computerState = CARTRIDGE;
							idleFirstRun = true;
						break;
					}
				}
			}

			break;
		case CARTRIDGE:
			if (cartridgeFirstRun = true)
			{
				// navigate to cartridge page
				if (cartridgeCount == 0)
				{
					cartridgeCount = dbService.loadCartridgeIndex(rifleData.id, cartIndex);
					cartridgeSelected = 0;
				}
				if (cartridgeCount > 0)
				{
					cartridgeData.id = cartIndex[cartridgeSelected].id;
					strcpy(cartridgeData.desc, cartIndex[cartridgeSelected].desc);
					nextionComms.sendStringToNextion("globals.cartname.val", cartridgeData.desc);
					nextionComms.sendPageToNextion(uiData.value);
					cartridgeFirstRun = false;
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
						if (cartridgeSelected > 0)
						{
							cartridgeSelected--;
						}
						else
						{
							cartridgeSelected = cartridgeCount - 1;
						}
						nextionComms.sendStringToNextion("cartridge.t2.txt", cartIndex[cartridgeSelected].desc);
						cartridgeData.id = cartIndex[cartridgeSelected].id;
						strcpy(cartridgeData.desc, cartIndex[cartridgeSelected].desc);
						break;
					case 2:
						// scroll right
						if (cartridgeSelected < (rifleCount - 2))
						{
							cartridgeSelected++;
						}
						else
						{
							cartridgeSelected = 0;
						}
						nextionComms.sendStringToNextion("cartridge.t2.txt", cartIndex[cartridgeSelected].desc);
						cartridgeData.id = cartIndex[cartridgeSelected].id;
						strcpy(cartridgeData.desc, cartIndex[cartridgeSelected].desc);
						break;
					case 3:
						// Back to previous page
						computerState = RIFLE;
						break;
					case 4:
						// navigate to range page
						computerState = CARTRIDGE;
						cartridgeFirstRun = true;
						break;
					}
				}
			}
			break;
		case RANGE:
			if (rangeFirstRun)
			{
				nextionComms.sendPageToNextion(uiData.value);
				rangeFirstRun = false;
			}

			if (nextionComms.isMessageInBuffer())
			{
				nextionComms.getData(&uiData);
				if (uiData.page == 3)
				{
					switch(uiData.value)
					{
						case 1:
							computerState = RIFLE;
							break;
						case 2:
							// Do something cool with the range
							break;
					}
				}
			}

			break;
		case ENVIRONMENT:	// We might not need this machine state
			break;
		case WINDDIRECTION:
		case LOCATION:
			break;
		case AZIMUTH:
			break;
		case GEOMETRY:
			break;
		case SOLUTION:
			break;
		case CALIBRATION:
			break;
	}
}
