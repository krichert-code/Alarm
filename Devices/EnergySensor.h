/*
 * EnergySensor.h
 *
 *  Created on: 2 czerw 2021
 *      Author: dom
 */

#include <DeviceInterface.h>
#include "CommonDefs.h"
#include <iostream>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"


using namespace std;


class EnergySensor : public DeviceInterface
{
    private:
		string deviceUrl;
		string deviceAddress;
		string user;
		string password;
		SensorReading reading;

    public:
		EnergySensor(string address, shared_ptr<spdlog::logger> logger);
		string getDeviceAddress();
		static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);

		SensorReading getDeviceReading() override;
		virtual ~EnergySensor() {}
};
