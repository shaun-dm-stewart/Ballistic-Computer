#pragma once

#ifndef CONSTANTS_H
#define CONSTANTS_H

// Data record lengths
const int RIFLERECORDLENGTH = 151;
const int CARTRECORDLENGTH = 101;

// Alternative pins for Serial1
const int8_t RXD1 = 32;
const int8_t TXD1 = 33;

// I2C pins
const int SDAPIN = 21;
const int SCLPIN = 22;
const int SERIAL_BAUD = 115200;

// SPI pins
const uint8_t CHIPSELECTPIN = 5;
const uint8_t MOSIPIN = 23;
const uint8_t MISOPIN = 19;
const uint8_t SCKPIN = 18;

#endif