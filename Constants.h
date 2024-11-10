#pragma once

#ifndef CONSTANTS_H
#define CONSTANTS_H

// Data record lengths
const int16_t RIFLERECORDLENGTH = 151;
const int16_t CARTRECORDLENGTH = 101;
const uint16_t MAXRIFLECOUNT = 10;
const uint16_t MACCARTRIDGECOUNT = 20;
const char RIFLEFILENAME[] = "rifle.dbb";

// Alternative pins for Serial1
const int8_t RXD1 = 32;
const int8_t TXD1 = 33;

// I2C pins
const int16_t SDAPIN = 21;
const int16_t SCLPIN = 22;
const int16_t SERIAL_BAUD = 9600;

// SPI pins
const uint8_t CHIPSELECTPIN = 5;
const uint8_t MOSIPIN = 23;
const uint8_t MISOPIN = 19;
const uint8_t SCKPIN = 18;

#endif