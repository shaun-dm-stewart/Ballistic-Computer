#pragma once

#ifndef CONSTANTS_H
#define CONSTANTS_H

// Data record lengths
const int16_t RIFLERECORDLENGTH = 151;
const int16_t CARTRECORDLENGTH = 101;
const uint16_t MAXRIFLECOUNT = 10;
const uint16_t MAXCARTRIDGECOUNT = 20;
const char RIFLEFILENAME[] = "rifle.dbb";
const char CARTRIDGEFILEFORMAT[] = "rifle%d.dbb";

// Alternative pins for Serial1
const int8_t GPS_RXD1 = 32;
const int8_t GPS_TXD1 = 33;

// Alternative pins for Serial2
const int8_t NEX_RXD1 = 16;
const int8_t NEX_TXD1 = 17;

// I2C pins
const int16_t SDAPIN = 21;
const int16_t SCLPIN = 22;
const int16_t SERIAL_BAUD = 9600;

// SPI pins
const uint8_t SD_CHIPSELECTPIN = 5;
const uint8_t MOSIPIN = 23;
const uint8_t MISOPIN = 19;
const uint8_t SCKPIN = 18;

// Confirm button pint
const uint8_t BUTTON_PIN = 13;

#endif