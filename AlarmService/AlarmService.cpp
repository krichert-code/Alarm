/*
 * AlarmService.cpp
 *
 *  Created on: 9 gru 2019
 *      Author: dom
 *
 */

#include <iostream>

#include "AlarmService.h"
#include "AlarmSensor.h"


using namespace std;


Status AlarmService::callbackFunction()
{
	Status status = STATUS_OK;

	synch.lock();

	if (isAlarmArmed)
	{
		for (auto &sensor : alarmSensors)
		{
			sensor->triggerAlarm();
		}
	}

	synch.unlock();

	return status;
}

Status AlarmService::enableAlarm()
{
	this->connect();

	return STATUS_OK;
}



Status AlarmService::disableAlarm()
{
	Status status = STATUS_OK;

	synch.lock();

	for (auto &sensor : alarmSensors)
		if (STATUS_OK != sensor->disableAlarm())
			status = STATUS_GENERIC_ERR;

	synch.unlock();

	return status;
}

Status AlarmService::enableService()
{
	synch.lock();

	//todo: implement timer to enable/generate alarm - test it

	isAlarmArmed = true;

	synch.unlock();

	return STATUS_OK;
}

Status AlarmService::disableService(string code)
{
	synch.lock();
	Status status = STATUS_OK;
	SettingParameters configCode = dynamic_pointer_cast<DevicesConfiguration>(deviceConfiguration)->getSettingValue("code");

	if ((configCode.status == STATUS_OK) && ( 0 == code.compare(configCode.value)))
	{
		status = disableAlarm();
		if (STATUS_OK == status)
			isAlarmArmed = false;
	}
	else
	{
		status = STATUS_AUTH_ERR;
	}

	synch.unlock();

	return status;
}

DeviceInfoData AlarmService::getData(const int deviceId)
{
	AlarmReading reading;

	synch.lock();

	if (this->alerts.find(deviceId) == this->alerts.end())
	{
		if (deviceId > this->maxSensorIdx)
		{
			reading.status = STATUS_XML_NO_MORE_SENSORS;
		}
		else
		{
			reading.status = STATUS_SENSOR_NOT_EXIST;
		}
	}
	else
	{
		reading = this->alerts[deviceId];
	}

	synch.unlock();

	return reading;
}

Status AlarmService::setData(const int deviceId, DeviceInfoData data)
{
	Status status = STATUS_OK;
	AlarmReading reading;

	synch.lock();

	try
	{
		SensorParameters configData = any_cast<SensorParameters>(deviceConfiguration->getData(deviceId));
		SensorReading readingData = any_cast<SensorReading>(data);
		ReadingConverter converter;

		reading.sensorName = configData.sensorName;

		if((readingData.status == STATUS_OK) && (converter.ConvertReadingToAlarm(readingData, configData.enableThresholdValue)))		
		{
			// todo : enableAlarm doesn't work correctly - separat thread is not good idea
			//cout << "[AlarmService] SensorId(" << configData.sensorName << ") = " << deviceId << " Value = " <<  readingData.lastReadingValue << " threshold = " <<  configData.enableThresholdValue << endl;

			reading.isActivate = true;
			reading.status = STATUS_OK;
			this->alerts[deviceId] = reading;

			// enableAlarm();
		}
		else if (readingData.status == STATUS_OK)
		{
			reading.isActivate = false;
			reading.status = STATUS_OK;
			this->alerts[deviceId] = reading;
		}
		else if (readingData.status != STATUS_OK)
		{
			reading.status = STATUS_READING_EXCEPTION;
			//todo: log critical event - sensor issue
		}
	}
	catch (bad_any_cast &e)
	{
		//todo: log this unexpected case
	}

	synch.unlock();

	return status;
}

void AlarmService::prepareDeviceInfoSetup(const int deviceId, const shared_ptr<DeviceInterface> device)
{
	SensorParameters sensorConfiguration;

	synch.lock();

	auto sensor = dynamic_pointer_cast<AlarmSensor>(device);

	if (sensor != nullptr)
	{
		alarmSensors.push_back(sensor);
	}

	synch.unlock();
}

AlarmService::AlarmService()
{
	SettingParameters params;
	deviceConfiguration = DevicesConfiguration::getInstance();
	isAlarmArmed = false;
	maxSensorIdx = -1;
	//set default alarm settings
	waitingTimeInSec = 30;
	countOfAlarmTiggerInSec = 1;

	try
	{
		params = dynamic_pointer_cast<DevicesConfiguration>(deviceConfiguration)->getSettingValue("waitingTimeInSec");
		if(STATUS_OK == params.status)
		{
			waitingTimeInSec = stoi(params.value);
		}
	}
	catch (invalid_argument& e)
	{
		//set default value
		waitingTimeInSec = 30;
		//todo : log exception
	}

	try
	{
		params = dynamic_pointer_cast<DevicesConfiguration>(deviceConfiguration)->getSettingValue("retriesInSec");
		countOfAlarmTiggerInSec = stoi(params.value);
	}
	catch (invalid_argument& e)
	{
		//set default value
		countOfAlarmTiggerInSec = 1;
		//todo : log exception
	}

}

shared_ptr<AlarmService> AlarmService::getInstance()
{
	synch.lock();

	if (nullptr == instance)
	{
		instance = static_cast<shared_ptr<AlarmService> >(new AlarmService());
	}

	synch.unlock();

	return instance;
}

shared_ptr<AlarmService> AlarmService::instance = nullptr;
recursive_mutex AlarmService::synch;
