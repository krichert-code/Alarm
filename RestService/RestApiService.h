/*
 * RestApiService.h
 *
 *  Created on: 10 lis 2019
 *      Author: dom
 */

#ifndef RESTSERVICE_RESTAPISERVICE_H_
#define RESTSERVICE_RESTAPISERVICE_H_

#include <DeviceInfoRegister.h>
#include <iostream>
#include "pistache/endpoint.h"
#include <functional>
#include <map>
#include <vector>

using namespace std;

class RestApiService: public Pistache::Http::Handler
{
	private:
		static DeviceInfoRegister deviceRegister;
		static map<string, function<string(map<string,string>)>> apiCommands;

		static string getVersion(map<string,string> parameters);
		static string getReading(map<string,string> parameters);
		static string getConfig(map<string,string> parameters);
		static string getConfigByName(map<string,string> parameters);
		static string enableAlarm(map<string,string> parameters);
		static string disableAlarm(map<string,string> parameters);
		static string getPresence(map<string,string> parameters);
		static string getTemperature(map<string,string> parameters);
		static string getAlerts(map<string,string> parameters);
		static string getEnergy(map<string,string> parameters);
		static string getStates(map<string,string> parameters);
		static string updateSensorState(map<string,string> parameters);


		static string generateSingleElementResponse(xmlResponse responseElements);
		static string generateResponse(xmlResponse responseElements);
		static string generateErrorResponse(string sensorId, string message);

		string getCommand(const string inputQuery);
		map<string,string> getParameters(const string inputQuery);
	public:

    	HTTP_PROTOTYPE(RestApiService)

		void onRequest(const Pistache::Http::Request& request, Pistache::Http::ResponseWriter response);
};




#endif /* RESTSERVICE_RESTAPISERVICE_H_ */
