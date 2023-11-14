/*
 * SensorEventsDatabase.cpp
 *
 *  Created on: 16 lis 2019
 *      Author: dom
 */



#include "SensorEventsDatabase.h"
#include "ReadingConverter.h"
#include <chrono>
#include <ratio>

using namespace std;

DeviceInfoData SensorEventsDatabase::getData(const int deviceId)
{
	DatabaseReadingEntry readingEntry = {};

	synch.lock();

	if (lastSensorsEvents.find(deviceId) == lastSensorsEvents.end())
	{
		if (deviceId > lastDeviceIdx)
		{
			readingEntry.reading.status = STATUS_XML_NO_MORE_SENSORS;
		}
		else
		{
			readingEntry.reading.status = STATUS_SENSOR_NOT_EXIST;
		}
	}
	else
	{
		readingEntry = lastSensorsEvents[deviceId];
	}

	synch.unlock();

	return readingEntry;
}

void SensorEventsDatabase::prepareDeviceInfoSetup(const int deviceId, const shared_ptr<DeviceInterface> device)
{
	DatabaseReadingEntry data;

	synch.lock();

	//initialize sensors reading data
	data.reading.status = STATUS_READING_NOT_READY;
	data.isSensorGeneratingAlarm = false;
	data.setupDeviceWithNewReadingValue = false;
	lastSensorsEvents[deviceId] = data;

	synch.unlock();
}

void SensorEventsDatabase::setupNewReadingValue(const int deviceId, const int reading)
{
	synch.lock();

	lastSensorsEvents[deviceId].setupDeviceWithNewReadingValue = true;
	lastSensorsEvents[deviceId].newReadingValue = reading;

	synch.unlock();
}

void SensorEventsDatabase::clearNewReadingValue(const int deviceId)
{
	synch.lock();

	lastSensorsEvents[deviceId].setupDeviceWithNewReadingValue = false;

	synch.unlock();
}

Status SensorEventsDatabase::setData(const int deviceId, DeviceInfoData data)
{
	Status status = STATUS_OK;
	ReadingConverter reading;
	auto timeStamp = chrono::high_resolution_clock::now();
    //todo: deviceId is not checked against overflow
	auto duration_msec = chrono::duration_cast<chrono::milliseconds>(timeStamp  - lastSensorsEvents[deviceId].lastExecedThrReadingTimestamp);

	synch.lock();

	try
	{
		if (lastSensorsEvents[deviceId].reading.status == STATUS_OK)
		{
			auto thresholdExceded = reading.ConvertReadingToAlarm(any_cast<SensorReading>(data), any_cast<SensorParameters>(deviceConfiguration->getData(deviceId)).enableThresholdValue);
			if( true == thresholdExceded )
			{
				if (false == lastSensorsEvents[deviceId].isValidExecedTimeStamp)
				{
					lastSensorsEvents[deviceId].isValidExecedTimeStamp = true;
				}
				lastSensorsEvents[deviceId].isSensorGeneratingAlarm = true;
				lastSensorsEvents[deviceId].lastExecedThrReadingTimestamp = timeStamp;
			}
			else if ((true == lastSensorsEvents[deviceId].isValidExecedTimeStamp) &&
					(duration_msec.count() > timeOfPresenceSec))
			{
				lastSensorsEvents[deviceId].isValidExecedTimeStamp = false;
				lastSensorsEvents[deviceId].isSensorGeneratingAlarm = false;
			}

			lastSensorsEvents[deviceId].readingTimestamp = timeStamp;
			lastSensorsEvents[deviceId].reading = any_cast<SensorReading>(data);

			if(lastDeviceIdx < deviceId)
			{
				lastDeviceIdx = deviceId;
			}
		}
		else
		{
			lastSensorsEvents[deviceId].readingTimestamp = timeStamp;
			lastSensorsEvents[deviceId].reading = any_cast<SensorReading>(data);
		}
	}
	catch(bad_any_cast &e)
	{
		status = STATUS_READING_EXCEPTION;
	}

	synch.unlock();
	return status;
}

SensorEventsDatabase::SensorEventsDatabase()
{
	lastDeviceIdx = -1;
	deviceConfiguration = DevicesConfiguration::getInstance();
	try
	{		
		timeOfPresenceSec = stoi(deviceConfiguration->getSettingValue("timeOfPresenceSec").value);
	}
	catch (invalid_argument& e)
	{
		timeOfPresenceSec = 0;
		//todo: log error
	}
}

shared_ptr<SensorEventsDatabase> SensorEventsDatabase::getInstance()
{
	synch.lock();

	if (nullptr == instance)
	{
		instance = static_cast<shared_ptr<SensorEventsDatabase> >(new SensorEventsDatabase());
	}

	synch.unlock();

	return instance;
}


shared_ptr<SensorEventsDatabase> SensorEventsDatabase::instance = nullptr;
recursive_mutex SensorEventsDatabase::synch;

