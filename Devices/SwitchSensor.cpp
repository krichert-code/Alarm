/*
 * SwitchSensor.cpp
 *
 *  Created on: 1 lip 2021
 *      Author: krichert
 */


#include "SwitchSensor.h"

SwitchSensor::SwitchSensor(string address, shared_ptr<spdlog::logger> logger)
{
    deviceAddress = stoi(address);
    readingType = READING_STATE;
    beginTime = std::chrono::duration_cast< std::chrono::milliseconds >(std::chrono::system_clock::now().time_since_epoch());

    wiringPiSetup();
    pinMode(deviceAddress, OUTPUT);
    digitalWrite(deviceAddress,0);
}

SensorReading SwitchSensor::getDeviceReading()
{
	SensorReading reading;
	int value;
	auto endTime = std::chrono::duration_cast< std::chrono::milliseconds >(std::chrono::system_clock::now().time_since_epoch());


	value = digitalRead(deviceAddress);
	if (value !=0 && (endTime.count()-beginTime.count() > 200) )
	{
		digitalWrite(deviceAddress, 0);
		value = 0;
	}

	reading.readingType = this->readingType;
	reading.lastReadingValue = value;
	reading.status = STATUS_OK;

	return reading;
}


void SwitchSensor::updateDeviceReading(int reading)
{
	beginTime = std::chrono::duration_cast< std::chrono::milliseconds >(std::chrono::system_clock::now().time_since_epoch());
	digitalWrite(deviceAddress, reading);
}


