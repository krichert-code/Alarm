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
	SensorReading reading;

	try
	{
		std::ifstream t(this->devices_directory+deviceAddress+this->reading_file);
		std::string data((std::istreambuf_iterator<char>(t)),
							std::istreambuf_iterator<char>());

		if (data.find(crc_ok) != string::npos){
			reading.readingType = READING_FIXED_INT;
			int readingValue = stoi(data.substr(data.find(data_pattern) + data_pattern.length()));
			if (readingValue >= 0)
				reading.lastReadingValue = (readingValue / 1000) << 12 | (readingValue - ((static_cast<int>(readingValue / 1000)) * 1000));
			else
				reading.lastReadingValue = (readingValue / 1000) << 12 | (abs(readingValue) - ((static_cast<int>(abs(readingValue) / 1000)) * 1000));
			reading.status = STATUS_OK;
		}
		else{
			reading.status = STATUS_WRONG_READING;
		}
	}
	catch (...)
	{
		reading.status = STATUS_WRONG_READING;
	}

	//cout << "TEMP SENSOR with address = " << deviceAddress << " Value = " << reading.lastReadingValue << endl;
	
	return reading;
}

TempSensor::TempSensor(string address){
	deviceAddress = address;
	reading.status = STATUS_READING_NOT_READY;
}



