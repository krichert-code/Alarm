/*
 * MoveSensor.cpp
 *
 *  Created on: 1 paź 2019
 *      Author: dom
 */

#include "MoveSensor.h"



MoveSensor::MoveSensor(string address, shared_ptr<spdlog::logger> logger)
{

	try {
		asyncSensor = false;
		readingType = READING_STATE;
		deviceAddress = stoi(address);
		if (deviceAddress != ASYNC_SENSOR) {
			wiringPiSetup();
			pinMode(deviceAddress, INPUT);
			pullUpDnControl (deviceAddress, PUD_UP);
		}
		else 
		{
			// -1 means not address provided (common for async sensor - reading is push by sensor, no need to poll data from device)
			asyncSensor = true;
			asyncSensorReading = 0;
		}
	}
	catch (invalid_argument& e)
	{
		//todo: logg it - wrong configuration
		std::cerr << e.what();
	}

}

SensorReading MoveSensor::getDeviceReading()
{
	SensorReading reading;

	reading.readingType = this->readingType;
	if (asyncSensor == true)
		reading.lastReadingValue = asyncSensorReading;
	else
		reading.lastReadingValue = digitalRead(deviceAddress);
	reading.status = STATUS_OK;

	// cout << "MOVE SENSOR with address = " << deviceAddress << " Value = " << reading.lastReadingValue << endl;

	return reading;
}

void MoveSensor::updateDeviceReading(int reading)
{
	asyncSensorReading = reading;
}