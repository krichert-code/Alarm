/*
 * DeviceInfoRegister.cpp
 *
 *  Created on: 14 lis 2019
 *      Author: krichert
 */


#include <DeviceInfoRegister.h>
#include "DevicesConfiguration.h"
#include "SensorEventsDatabase.h"
#include "ReadingConverter.h"
#include <chrono>

using namespace std;

DeviceInfoRegister::DeviceInfoRegister()
{
	deviceInfoSource.push_back(DevicesConfiguration::getInstance());
	//todo: provide deconstructor
	deviceInfoSource.push_back(SensorEventsDatabase::getInstance());
}

xmlResponse DeviceInfoRegister::changeSensorReadingValue(string sensorName, const int reading)
{
	xmlResponse result;
	int deviceId = 0;
	SensorParameters config;

	//find sensorId for provided sensorName
	while (config.status != STATUS_SENSOR_NOT_EXIST)
	{
		config = any_cast<SensorParameters>(deviceInfoSource[0]->getData(deviceId));
		if (config.sensorName.compare(sensorName) == 0)
		{
			break;
		}
		deviceId++;
	}

	if (config.status != STATUS_SENSOR_NOT_EXIST)
	{
		dynamic_pointer_cast<DeviceSetupInterface>(deviceInfoSource[1])->setupNewReadingValue(deviceId, reading);
		result[deviceId].insert((pair<string, string>("sensorName", sensorName)));
		result[deviceId].insert((pair<string, string>("status", "OK")));
	}
	else
	{
		result[deviceId].insert((pair<string, string>("sensorName", sensorName)));
		result[deviceId].insert((pair<string, string>("status", "ERROR")));
	}

	return result;
}

xmlResponse DeviceInfoRegister::getConfigurationDeviceInfo(int deviceId)
{
	xmlResponse result;
	SensorParameters config = any_cast<SensorParameters>(deviceInfoSource[0]->getData(deviceId));

	result[deviceId].insert((pair<string, string>("sensorName", config.sensorName)));
	result[deviceId].insert((pair<string, string>("enable", to_string(static_cast<int>(config.enabled)))));
	result[deviceId].insert((pair<string, string>("address", config.sensorAddress)));
	result[deviceId].insert((pair<string, string>("sensorType", to_string(config.sensorType))));
	result[deviceId].insert((pair<string, string>("scanningPeriod", to_string(config.scanningPeriod))));
	result[deviceId].insert((pair<string, string>("status", to_string(config.status))));

	return result;
}

xmlResponse DeviceInfoRegister::getReadingDeviceInfo(int deviceId)
{
	xmlResponse result;
	ReadingConverter converter;
	chrono::time_point<chrono::system_clock> timestamp = any_cast<DatabaseReadingEntry>(deviceInfoSource[1]->getData(deviceId)).readingTimestamp;

	SensorReading reading = any_cast<DatabaseReadingEntry>(deviceInfoSource[1]->getData(deviceId)).reading;

	result[deviceId].insert((pair<string, string>("value", converter.ConvertReadingToString(reading))));
	result[deviceId].insert((pair<string, string>("status", converter.ConvertStatusToString(reading))));

	result[deviceId].insert( (pair<string, string>("time", to_string(std::chrono::system_clock::to_time_t(timestamp)) )) );

	return result;
}

xmlResponse DeviceInfoRegister::getPresenceDeviceInfo()
{
	xmlResponse result;
	ReadingConverter converter;
	int sensorIdx = 0;
	DatabaseReadingEntry readingEntry;
	SensorParameters sensorParam;

	do
	{
		readingEntry = any_cast<DatabaseReadingEntry>(deviceInfoSource[1]->getData(sensorIdx));
		sensorParam = any_cast<SensorParameters>(deviceInfoSource[0]->getData(sensorIdx));

		if((STATUS_OK == readingEntry.reading.status) && ((sensorParam.sensorType == MOVE_SENSOR) || (sensorParam.sensorType == CAMERA_SENSOR)))
		{
		    result[sensorIdx].insert(pair<string, string>("sensorName", sensorParam.sensorName));
		    result[sensorIdx].insert(pair<string, string>("sensorId", to_string(sensorIdx)));
		    result[sensorIdx].insert(pair<string, string>("presenceState", to_string(readingEntry.isSensorGeneratingAlarm)));
		}
		sensorIdx++;
	}
	while (readingEntry.reading.status != STATUS_XML_NO_MORE_SENSORS);

	return result;
}


xmlResponse DeviceInfoRegister::getTemperatureDeviceInfo()
{
	xmlResponse result;
	ReadingConverter converter;
	int sensorIdx = 0;
	DatabaseReadingEntry readingEntry;
	SensorParameters sensorParam;

	do
	{
		readingEntry = any_cast<DatabaseReadingEntry>(deviceInfoSource[1]->getData(sensorIdx));
		sensorParam = any_cast<SensorParameters>(deviceInfoSource[0]->getData(sensorIdx));

		if(TEMP_SENSOR == sensorParam.sensorType)
		{
			result[sensorIdx].insert(pair<string, string>("sensorId", to_string(sensorIdx)));
			result[sensorIdx].insert(pair<string, string>("temperature", converter.ConvertReadingToString(readingEntry.reading)));
			result[sensorIdx].insert(pair<string, string>("thresholdExceeded", converter.ConvertReadingToString(readingEntry.isSensorGeneratingAlarm)));
			result[sensorIdx].insert(pair<string, string>("sensorName", sensorParam.sensorName));
			result[sensorIdx].insert(pair<string, string>("status", converter.ConvertStatusToString(readingEntry.reading)));
		}
		sensorIdx++;
	}
	while (readingEntry.reading.status != STATUS_XML_NO_MORE_SENSORS);


	return result;
}

xmlResponse DeviceInfoRegister::getEnergyDeviceInfo()
{
	xmlResponse result;
	ReadingConverter converter;
	int sensorIdx = 0;
	DatabaseReadingEntry readingEntry;
	SensorParameters sensorParam;

	do
	{
		readingEntry = any_cast<DatabaseReadingEntry>(deviceInfoSource[1]->getData(sensorIdx));
		sensorParam = any_cast<SensorParameters>(deviceInfoSource[0]->getData(sensorIdx));

		if(ENERGY_SENSOR == sensorParam.sensorType)
		{
			result[sensorIdx].insert(pair<string, string>("sensorId", to_string(sensorIdx)));
			result[sensorIdx].insert(pair<string, string>("power", converter.ConvertReadingToString(readingEntry.reading)));
			result[sensorIdx].insert(pair<string, string>("sensorName", sensorParam.sensorName));
			result[sensorIdx].insert(pair<string, string>("type", sensorParam.sensorAddress));
			result[sensorIdx].insert(pair<string, string>("status", converter.ConvertStatusToString(readingEntry.reading)));
		}
		sensorIdx++;
	}
	while (readingEntry.reading.status != STATUS_XML_NO_MORE_SENSORS);


	return result;
}

xmlResponse DeviceInfoRegister::getStateDeviceInfo()
{
	xmlResponse result;
	ReadingConverter converter;
	int sensorIdx = 0;
	DatabaseReadingEntry readingEntry;
	SensorParameters sensorParam;

	do
	{
		readingEntry = any_cast<DatabaseReadingEntry>(deviceInfoSource[1]->getData(sensorIdx));
		sensorParam = any_cast<SensorParameters>(deviceInfoSource[0]->getData(sensorIdx));

		if(sensorParam.sensorType == STATE_SENSOR)
		{
		    result[sensorIdx].insert(pair<string, string>("sensorName", sensorParam.sensorName));
		    result[sensorIdx].insert(pair<string, string>("sensorId", to_string(sensorIdx)));
		    result[sensorIdx].insert(pair<string, string>("outputState", to_string(readingEntry.reading.lastReadingValue)));
		    result[sensorIdx].insert(pair<string, string>("status", converter.ConvertStatusToString(readingEntry.reading)));
		}
		sensorIdx++;
	}
	while (readingEntry.reading.status != STATUS_XML_NO_MORE_SENSORS);


	return result;
}
