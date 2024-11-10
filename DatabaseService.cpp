// 
// 
// 

#include "DatabaseService.h"

DatabaseService::DatabaseService()
{

}

DatabaseService::DatabaseService(HardwareSerial* serial)
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
bool DatabaseService::LoadRifleIndex(IndexItem* rifles[])
{
	bool result = false;
	uint32_t indexOffset = 0;
	uint32_t fileSize = 0;
	uint16_t recordCount = 0;
	byte buffer[RIFLERECORDLENGTH];

	_data = SD.open("rifle.dbb");
	if (_data) {
		while (indexOffset < fileSize && recordCount < MAXRIFLECOUNT)
		{
			for (int i = 0; i < RIFLERECORDLENGTH; i++)
			{
				buffer[i] = _data.read();
			}
			const char* desc;
			DeserializationError error = deserializeJson(doc, buffer);
			if (error)
			{
				break;
			}
			desc = doc["desc"];
			rifles[indexOffset]->id = doc["id"];
			strcpy(rifles[indexOffset]->desc, desc);
			indexOffset++;
			recordCount++;
		}
		result = true;
	}
	_data.close();

	return result;
}
bool DatabaseService::LoadCartridgeIndex(IndexItem* cartridges[])
{
	return true;
}
bool DatabaseService::LoadRifleDetail(int index, RifleData* rifleData)
{
	uint32_t indexOffset = (index - 1) * RIFLERECORDLENGTH;
	uint32_t fileSize = 0;
	byte buffer[RIFLERECORDLENGTH];
	bool result = false;

	_data = SD.open("rifle.dbb");
	fileSize = _data.size();
	if (_data && indexOffset <= (fileSize - RIFLERECORDLENGTH) )
	{
		if (_data.seek(indexOffset))
		{
			for (int i = 0; i < RIFLERECORDLENGTH; i++)
			{
				buffer[i] = _data.read();
			}
			DeserializationError error = deserializeJson(doc, buffer);
			if (error)
			{
				; // Go no further
			}
			else
			{
				rifleData->id = doc["id"];
				const char* desc;
				desc = doc["desc"];
				strcpy(rifleData->desc, desc);
				rifleData->sh = doc["sh"];
				rifleData->tr = doc["tr"];
				rifleData->zd = doc["zd"];
				rifleData->ec = doc["ec"];
				rifleData->wc = doc["tr"];
				rifleData->al = doc["al"];
				rifleData->ap = doc["ap"];
				rifleData->te = doc["te"];
				rifleData->rh = doc["al"];
				result = true;
			}
		}
	}
	_data.close();

	return result;
}
bool DatabaseService::LoadCartridgeDetail(int RifleIndex, int CartridgeIndex, CartridgeData* cartridgeData)
{
	return true;
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



