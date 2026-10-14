/*
 * TempSensor.cpp
 *
 *  Created on: 10 lis 2019
 *      Author: dom
 */

#include "TempSensor.h"
#include <string>
#include <fstream>
#include <streambuf>
#include <stdlib.h>

SensorReading TempSensor::getDeviceReading()
{
	const string data_pattern = "t=";
	const string crc_ok = "YES";
	int readingValue = 0;
	SensorReading reading;

	reading.status = STATUS_READING_NOT_READY;
	reading.readingType = readingType;

	try
	{
	    if (asyncSensor == false) {
		std::ifstream t(this->devices_directory+deviceAddress+this->reading_file);
		std::string data((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());
		if (data.find(crc_ok) != string::npos){
			readingValue = stoi(data.substr(data.find(data_pattern) + data_pattern.length()));
			reading.status = STATUS_OK;
		}
		else{
			reading.status = STATUS_WRONG_READING;
		}

	    }
	    else if (asyncSensor == true) {
                readingValue = asyncSensorReading;
		reading.status = STATUS_OK;
	    }


	    if (reading.status == STATUS_OK) {
		if (readingValue >= 0)
		    reading.lastReadingValue = (readingValue / 1000) << 12 | (readingValue - ((static_cast<int>(readingValue / 1000)) * 1000));
		else
		    reading.lastReadingValue = (readingValue / 1000) << 12 | (abs(readingValue) - ((static_cast<int>(abs(readingValue) / 1000)) * 1000));
	    }
	    else {
		reading.lastReadingValue = 0;
	    }
	}
	catch (...)
	{
		reading.status = STATUS_WRONG_READING;
	}

	// cout << "TEMP SENSOR with address = " << deviceAddress << " Value = " << reading.lastReadingValue << endl;
	
	return reading;
}

TempSensor::TempSensor(string address, shared_ptr<spdlog::logger> logger){
        readingType = READING_FIXED_INT;

        if (address.size() == ASYNC_SENSOR) {
		asyncSensor = true;
		asyncSensorReading = 0;
	}
	else {
	    deviceAddress = address;
	    asyncSensor = false;
        }
}


void TempSensor::updateDeviceReading(int reading)
{
        asyncSensorReading = reading;
}
