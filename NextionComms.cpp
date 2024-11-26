// 
// 
// 

#include "NextionComms.h"
NextionComms::NextionComms()
{

}

bool NextionComms::begin(HardwareSerial* serial)
{
    _serial = serial;
    return true;
}

bool NextionComms::sendStringToNextion(char variable[], char value[])
{
    bool result = true;
    _serial->printf("%s=\"%s\"\xFF\xFF\xFF", variable, value);
    return result;
}

bool NextionComms::sendIntToNextion(char variable[], int value)
{
    bool result = true;
    _serial->printf("%s=\"%d\"\xFF\xFF\xFF", variable, value);
    return result;
}

bool NextionComms::sendFloatToNextion(char variable[], double value)
{
    bool result = true;
    _serial->printf("%s=\"%f\"\xFF\xFF\xFF", variable, value);
    return result;
}

bool NextionComms::sendPageToNextion(int page)
{
    bool result = true;
    _serial->printf("page %d\xFF\xFF\xFF", page);
    return result;
}

bool NextionComms::isMessageInBuffer()
{
    static bool msgInFlight = false;
    static int bufferOffset = 0;

    char inChar;
    bool msgInBuffer = false;

    while (_serial->available())
    {
        inChar = _serial->read();
        switch (inChar)
        {
        case 0x02:  //STX
            if (!msgInFlight)
            {
                msgInFlight = true;
                bufferOffset = 0;
            }
            else
            {
                _msgBuffer[bufferOffset++] = inChar;
            }
            break;
        case 0x03:  //ETX
            if (msgInFlight)
            {
                msgInFlight = false;
                msgInBuffer = true;
                bufferOffset = 0;
            }
            break;
        default:
            if (msgInFlight)
            {
                _msgBuffer[bufferOffset++] = inChar;
                _msgBuffer[bufferOffset] = 0x00;
            }
            break;
        }
    }
    return msgInBuffer;
}

bool NextionComms::getData(UIData* data)
{
    bool result = false;
    DeserializationError error = deserializeJson(_doc, _msgBuffer);
    // Test if parsing succeeds.
    if (error) {
        return result;
    }

    // All messages contain this key-value pair so extract them here.
    data->page = _doc["page"];
    data->value = _doc["value"];

    switch (data->page)
    {
    case 0:     // We have the data we need so there is nothing to do
    case 1:
    case 2:
    case 3:
    case 4:
        data->range = _doc["range"];
        break;
    case 5:
        break;
    case 6:
        data->speed = _doc["speed"];
        data->direction = _doc["direction"];
        break;
    case 7:
    case 8:
    default:
        break;
    }

    result = true;

    return result;
}
