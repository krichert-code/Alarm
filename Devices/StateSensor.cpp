/*
 * StateSensor.cpp
 *
 *  Created on: 30 cze 2021
 *      Author: krichert
 */



/*
 * MoveSensor.cpp
 *
 *  Created on: 1 paź 2019
 *      Author: dom
 */

#include "StateSensor.h"



StateSensor::StateSensor(string address, shared_ptr<spdlog::logger> logger)
{
    deviceAddress = stoi(address);
    readingType = READING_STATE;

    wiringPiSetup();
    pinMode(deviceAddress, OUTPUT);
    digitalWrite(deviceAddress,0);
}

SensorReading StateSensor::getDeviceReading()
{
	SensorReading reading;


	reading.readingType = this->readingType;
	reading.lastReadingValue = digitalRead(deviceAddress);
	reading.status = STATUS_OK;

	return reading;
}


void StateSensor::updateDeviceReading(int reading)
{
	digitalWrite(deviceAddress, reading);
}

