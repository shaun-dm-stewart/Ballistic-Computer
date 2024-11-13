// NextionComms.h

#ifndef _NextionComms_h
#define _NextionComms_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif
#include <HardwareSerial.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include "Constants.h"
#include "Structs.h"
//#define STX 0x02;
//#define ETX 0x03;

class NextionComms
{
public:
	NextionComms();
	bool begin(HardwareSerial* serial);
	bool isMessageInBuffer();
	bool getData(UIData* data);
	bool sendStringToNextion(char variable[], char value[]);
	bool sendIntToNextion(char variable[], int value);
	bool sendFloatToNextion(char variable[], double value);
	bool sendPageToNextion(int page);
private:
	char  _msgBuffer[101];
	HardwareSerial* _serial;
	JsonDocument _doc;
};

#endif
