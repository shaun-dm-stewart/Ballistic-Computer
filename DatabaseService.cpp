// 
// 
// 

#include "DatabaseService.h"

DatabaseService::DatabaseService()
{

}

DatabaseService::DatabaseService(HardwareSerial *serial)
{
	_serial = serial;

	pinMode(CHIPSELECTPIN, OUTPUT);

	// Change these values in the constants file
	if (!SD.begin(CHIPSELECTPIN, MOSIPIN, MISOPIN, SCKPIN)) {
		debugln("initialization failed!");
		return;
	}
	debugln("initialization done.");
}

#ifdef DEBUG
void DatabaseService::testSD()
{
	// open the file. note that only one file can be open at a time,
	// so you have to close this one before opening another.
	_data = SD.open("test.txt", FILE_WRITE);

	// if the file opened okay, write to it:
	if (_data) {
		debugln("Writing to test.txt...");
		_data.println("testing 1, 2, 3.");
		// close the file:
		_data.close();
		Serial.println("done.");
	}
	else {
		// if the file didn't open, print an error:
		Serial.println("error opening test.txt");
	}

	// re-open the file for reading:
	_data = SD.open("test.txt");
	if (_data) {
		debugln("test.txt:");

		// read from the file until there's nothing else in it:
		while (_data.available()) {
			debugWrite(_data.read());
		}
		// close the file:
		_data.close();
	}
	else {
		// if the file didn't open, print an error:
		debug("error opening test.txt");
	}
}
#endif



