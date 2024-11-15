// 
// 
// 

#include "GPS.h"
GPS::GPS()
{

}

void GPS::begin(HardwareSerial* serial)
{
	_serial = serial;
}

int GPS::decodeSentence()
{
    int result = 0;
    if (gps.location.isValid())
    {
        _latitude = gps.location.lat();
        _longitude = gps.location.lng();
        result += 1;
    }

    if (gps.location.isValid())
    {
        _altitude = gps.altitude.feet();
        Serial.print("Altitude: ");
        Serial.println(_altitude);
        result += 2;
    }

    if (gps.date.isValid())
    {
        _dateTime.month = gps.date.month();
        _dateTime.day = gps.date.day();
        _dateTime.year = gps.date.year();
        result += 4;
    }

    Serial.print(F(" "));
    if (gps.time.isValid())
    {
        _dateTime.hour = gps.time.hour();
        _dateTime.minute = gps.time.minute();
        _dateTime.second = gps.time.second();
        result += 8;
    }

    return result;
}

bool GPS::getFix()
{
	_gotFix = false;
	while (_gotFix == false)
	{
		while (_serial->available() > 0)
		{
			if (gps.encode(Serial1.read()))
			{
                if (decodeSentence() >= 15)
                {
                    _gotFix = true;
                    break;
                }
			}
		}
	}
    return _gotFix;
}

double GPS::getLongitude()
{
    return _longitude;
}

double GPS::getLatitude()
{
    return _latitude;
}

double GPS::getAltitude()
{
    return _altitude;
}

DateAndTime GPS::getDateTime()
{
    return _dateTime;
}
