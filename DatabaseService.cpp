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

int DatabaseService::LoadRifleIndex(IndexItem rifles[])
{
	int result = 0;
	uint32_t indexOffset = 0;
	uint32_t fileSize = 0;
	uint16_t recordCount = 0;
	byte buffer[RIFLERECORDLENGTH];
	IndexItem rifleIndex[MAXRIFLECOUNT];

	_data = SD.open(RIFLEFILENAME);
	recordCount = _data.size() / RIFLERECORDLENGTH;
	if (_data) {
		while (indexOffset < recordCount && recordCount < MAXRIFLECOUNT)
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
			rifles[indexOffset].id = doc["id"];
			strcpy(rifles[indexOffset].desc, desc);
			indexOffset++;
		}
		result = indexOffset;
	}
	_data.close();

	return result;
}

int DatabaseService::LoadCartridgeIndex(int rifleId, IndexItem cartridges[])
{
	int result = 0;
	uint32_t indexOffset = 0;
	uint32_t fileSize = 0;
	uint16_t recordCount = 0;
	char fileName[13];            // Max file name length = 8.3
	byte buffer[CARTRECORDLENGTH];

	sprintf(fileName, CARTRIDGEFILEFORMAT, rifleId);
	_data = SD.open(fileName);
	recordCount = _data.size() / CARTRECORDLENGTH;
	if (_data) {
		while (indexOffset < recordCount && recordCount < MAXCARTRIDGECOUNT)
		{
			for (int i = 0; i < CARTRECORDLENGTH; i++)
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
			cartridges[indexOffset].id = doc["id"];
			strcpy(cartridges[indexOffset].desc, desc);
			indexOffset++;
		}
		result = indexOffset;
	}
	_data.close();

	return result;
}

bool DatabaseService::LoadRifleDetail(int rifleId, RifleData* rifleData)
{
	uint32_t indexOffset = (rifleId - 1) * RIFLERECORDLENGTH;
	uint32_t fileSize = 0;
	byte buffer[RIFLERECORDLENGTH];
	bool result = false;

	_data = SD.open(RIFLEFILENAME);
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

bool DatabaseService::LoadCartridgeDetail(int rifleId, int cartridgeId, CartridgeData* cartridgeData)
{
	bool result = false;
	uint32_t indexOffset = (cartridgeId - 1) * CARTRECORDLENGTH;
	uint32_t fileSize = 0;
	byte buffer[CARTRECORDLENGTH];
	char fileName[13];            // Max file name length = 8.3

	sprintf(fileName, CARTRIDGEFILEFORMAT, rifleId);
	_data = SD.open(fileName);

	fileSize = _data.size();
	if (_data && indexOffset <= (fileSize - CARTRECORDLENGTH))
	{
		if (_data.seek(indexOffset))
		{
			for (int i = 0; i < CARTRECORDLENGTH; i++)
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
				cartridgeData->id = doc["id"];
				cartridgeData->rifleid = doc["rifleid"];
				const char* desc;
				desc = doc["desc"];
				strcpy(cartridgeData->desc, desc);
				cartridgeData->wt = doc["wt"];
				cartridgeData->mv = doc["mv"];
				cartridgeData->bc = doc["bc"];
				cartridgeData->bl = doc["bl"];
				cartridgeData->clbr = doc["clbr"];
				result = true;
			}
		}
	}
	_data.close();

	return result;
}



