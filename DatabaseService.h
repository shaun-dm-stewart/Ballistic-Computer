// DatabaseService.h

#ifndef _DatabaseService_h
#define _DatabaseService_h

#include <SPI.h>
#include <mySD.h>
#include <HardwareSerial.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define DEBUG

#ifdef DEBUG 
#define debugln(x) _serial->println(x)
#define debug(x) _serial->print(x)
#define debugWrite(x) _serial->write(x)
#else
#define debugln(x)
#define debug(x)
#define debugWrite(x)
#endif

#include"Structs.h"
#include "Constants.h"

class DatabaseService
{
	public:
		DatabaseService();
		DatabaseService(HardwareSerial* serial);
		int LoadRifleIndex(IndexItem rifles[]);
		int LoadCartridgeIndex(int rifleId, IndexItem cartridges[]);
		bool LoadRifleDetail(int rifleId, RifleData* rifleData);
		bool LoadCartridgeDetail(int rifleId, int cartridgeId, CartridgeData* cartridgeData);
	private:
		JsonDocument doc;
		ext::File _data;
		HardwareSerial* _serial;
};

#endif

