// GPS.h

#ifndef _GPS_h
#define _GPS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include<TinyGPSPlus.h>
#include<HardwareSerial.h>

typedef struct
{
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
} DateAndTime;

class GPS
{
public:
	//typedef struct
	//{
	//	int year;
	//	int month;
	//	int day;
	//	int hour;
	//	int minute;
	//	int second;
	//} dateAndTime;

	GPS();
	bool   getFix();
	void   begin(HardwareSerial* serial);
	double getLongitude();
	double getLatitude();
	double getAltitude();
	DateAndTime getDateTime();
private:
	HardwareSerial* _serial;
	double _latitude;
	double _longitude;
	double _altitude;
	DateAndTime _dateTime;
	bool _gotFix;
	TinyGPSPlus gps;
	int decodeSentence();
};
#endif

