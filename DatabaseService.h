// DatabaseService.h

#ifndef _DatabaseService_h
#define _DatabaseService_h

#include <SPI.h>
#include <mySD.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include"Structs.h"
#include "Constants.h"

class DatabaseService
{
	public:
		DatabaseService();
		bool begin();
		int16_t loadRifleIndex(IndexItem rifles[]);
		int16_t loadCartridgeIndex(int rifleId, IndexItem cartridges[]);
		bool loadRifleDetail(int rifleId, RifleData* rifleData);
		bool loadCartridgeDetail(int rifleId, int cartridgeId, CartridgeData* cartridgeData);
	private:
		JsonDocument doc;
		ext::File _data;
};

#endif

