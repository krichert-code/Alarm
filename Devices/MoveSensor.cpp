/*
 * MoveSensor.cpp
 *
 *  Created on: 1 paź 2019
 *      Author: dom
 */

#include "MoveSensor.h"



MoveSensor::MoveSensor(string address, shared_ptr<spdlog::logger> logger)
{
    deviceAddress = stoi(address);
    readingType = READING_STATE;

    wiringPiSetup();
    pinMode(deviceAddress, INPUT);
    pullUpDnControl (deviceAddress, PUD_UP);
}

SensorReading MoveSensor::getDeviceReading()
{
	SensorReading reading;

	reading.readingType = this->readingType;
	reading.lastReadingValue = digitalRead(deviceAddress);
	reading.status = STATUS_OK;

	// cout << "MOVE SENSOR with address = " << deviceAddress << " Value = " << reading.lastReadingValue << endl;	

	return reading;
}

