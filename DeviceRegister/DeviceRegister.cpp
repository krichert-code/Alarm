/*
 * DeviceRegister.cpp
 *
 *  Created on: 20 paź 2019
 *      Author: dom
 */
#include <DeviceRegister.h>
#include "MoveSensor.h"
#include "TempSensor.h"
#include "AlarmSensor.h"
#include "EnergySensor.h"
#include "CamSensor.h"
#include "StateSensor.h"
#include "SwitchSensor.h"
#include "DevicesConfiguration.h"

DevicesRegister::DevicesRegister()
{
	deviceRegister.clear();
}


void DevicesRegister::registerDevices(SensorType sensorType, shared_ptr<spdlog::logger> logger)
{
	shared_ptr<DeviceInfoInterface> config = DevicesConfiguration::getInstance();
	SensorParameters sensorConfiguration;
	int deviceId = 0;

	try
	{
		do
		{
			sensorConfiguration = any_cast<SensorParameters>(config->getData(deviceId));
			if (true == sensorConfiguration.enabled && sensorConfiguration.sensorType == sensorType)
			{
				deviceRegister[deviceId] = deviceType[sensorConfiguration.sensorType](sensorConfiguration.sensorAddress, logger);
			}
			deviceId++;
		}
		while (sensorConfiguration.status == STATUS_OK);
	}
	catch (bad_any_cast &e)
	{
		//todo: log critical event
	}
}

const vector<int> DevicesRegister::getRegistredDevicesId()
{
	vector<int> devicesId;

	for (auto &deviceElement : deviceRegister)
	{
		devicesId.push_back(deviceElement.first);
	}

	return devicesId;
}

const shared_ptr<DeviceInterface> DevicesRegister::getRegisteredDevice(const int deviceId)
{
	//todo: check if it can throw exception if index is out of the range - catch it in scheduler
	return deviceRegister[deviceId];
}


constexpr int DevicesRegister::deviceTypes;
array<function<shared_ptr<DeviceInterface>(string, shared_ptr<spdlog::logger>)>, DevicesRegister::deviceTypes> DevicesRegister::deviceType =
{
		[](string address, shared_ptr<spdlog::logger> logger)->shared_ptr<DeviceInterface> { return make_shared<MoveSensor>(address, logger); },
		[](string address, shared_ptr<spdlog::logger> logger)->shared_ptr<DeviceInterface> { return make_shared<TempSensor>(address, logger); },
		[](string address, shared_ptr<spdlog::logger> logger)->shared_ptr<DeviceInterface> { return make_shared<EnergySensor>(address, logger); },
		[](string address, shared_ptr<spdlog::logger> logger)->shared_ptr<DeviceInterface> { return make_shared<StateSensor>(address, logger); },
		[](string address, shared_ptr<spdlog::logger> logger)->shared_ptr<DeviceInterface> { return make_shared<SwitchSensor>(address, logger); },
		[](string address, shared_ptr<spdlog::logger> logger)->shared_ptr<DeviceInterface> { return make_shared<CamSensor>(address, logger); },
		[](string address, shared_ptr<spdlog::logger> logger)->shared_ptr<DeviceInterface> { return make_shared<AlarmSensor>(address, logger); }
};
