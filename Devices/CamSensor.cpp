/*
 * CamSensor.cpp
 *
 *  Created on: 11 lis 2023
 *      Author: dom
 */

#include "CamSensor.h"
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>

CamSensor::CamSensor(string address)
{
    deviceAddress = address;
    readingType = READING_STATE;
}

SensorReading CamSensor::getDeviceReading()
{
	SensorReading reading;
    string cmd = "ffprobe -hide_banner  -stimeout 8000 " + deviceAddress + " 2>&1";    
    array<char, 128> buffer;
    string result;

    
    unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    reading.readingType = this->readingType;

    if (result.find("404 Stream Not Found") == string::npos) reading.lastReadingValue = 1;
        else reading.lastReadingValue = 0;
	
	reading.status = STATUS_OK;
    
	// cout << "CAM SENSOR with address = " << deviceAddress << " Value = " << reading.lastReadingValue << endl;
	return reading;
}