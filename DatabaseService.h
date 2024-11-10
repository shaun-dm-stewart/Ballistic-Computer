// DatabaseService.h

#ifndef _DatabaseService_h
#define _DatabaseService_h

#include <SPI.h>
#include <mySD.h>
#include <HardwareSerial.h>

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
#define debugSerial(x)
#endif

#include "Constants.h"
#include "Structs.h"
class DatabaseService
{
	public:
		DatabaseService();
		DatabaseService(HardwareSerial* serial);
		void testSD();
	private:
		ext::File _data;
		HardwareSerial* _serial;
};

#endif

