/*
 * EnergySensor.cpp
 *
 *  Created on: 2 czerw 2021
 *      Author: dom
 */

#include "EnergySensor.h"

#include <iostream>
#include <string>
#include <curl/curl.h>


SensorReading EnergySensor::getDeviceReading()
{
	SensorReading reading;
	CURL *curl;
	CURLcode res;
	std::string readBuffer;
	std::size_t pos = 0;
	std::string val = "";

	try
	{
		curl = curl_easy_init();
		if(curl) 
		{			
			curl_easy_setopt(curl, CURLOPT_URL, deviceUrl.c_str());
			curl_easy_setopt(curl, CURLOPT_USERNAME, user.c_str());
			curl_easy_setopt(curl, CURLOPT_PASSWORD, password.c_str());

			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, EnergySensor::WriteCallback);
			curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
			readBuffer.clear();
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
			res = curl_easy_perform(curl);			
			curl_easy_cleanup(curl);
			if (res == 0)
			{
				pos = readBuffer.find(deviceAddress);
				if (pos != -1)
				{
					val = readBuffer.substr (pos);
					pos = val.find("\"")+1;
					val = val.substr (pos);
					pos = val.find("\"");
					val = val.substr (0,pos);


					reading.readingType = READING_INT;
					reading.lastReadingValue = stoi(val);
					reading.status = STATUS_OK;
				}
				else
				{
					reading.readingType = READING_INT;
					reading.lastReadingValue = 0;
					reading.status = STATUS_READING_NOT_READY;
				}
			}
			else
			{
				reading.readingType = READING_INT;
				reading.lastReadingValue = 0;
				reading.status = STATUS_READING_NOT_READY;
			}
		}
		else
		{			
			reading.status = STATUS_GENERIC_ERR;
		}		
	}
	catch (...)
	{
		reading.status = STATUS_GENERIC_ERR;		
	}

	// cout << "ENERGY SENSOR with address = " << deviceAddress << " Value = " << reading.lastReadingValue << " Status = " << reading.status << endl;

	return reading;
}

EnergySensor::EnergySensor(string address, shared_ptr<spdlog::logger> logger)
{
	// address template = " user:password:parameter@url"
	user = address.substr(0, address.find(":"));
	address = address.substr(address.find(":") + 1);
	password = address.substr(0, address.find(":"));
	address = address.substr(address.find(":") + 1);
	if (address.find("@") != -1)
	{
		deviceAddress = address.substr(0, address.find("@"));
		deviceUrl = address.substr(address.find("@")+1);
	}
	else
	{
		deviceAddress = address;
		deviceUrl = "None";
	}
	reading.lastReadingValue = 0;
	reading.readingType = READING_INT;
	reading.status = STATUS_READING_NOT_READY;
}

string EnergySensor::getDeviceAddress(){
	return user + ":" + password + ":" +deviceAddress + "@" + deviceUrl;
}

size_t EnergySensor::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}







