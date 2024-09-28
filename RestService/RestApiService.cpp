/*
 * RestApiService.cpp
 *
 *  Created on: 10 lis 2019
 *      Author: dom
 */



#include <DeviceInterface.h>
#include <DeviceServiceInterface.h>
#include "RestApiService.h"
#include "CommonDefs.h"
#include "AlarmService.h"


using namespace std;

void RestApiService::onRequest(const Pistache::Http::Request& request, Pistache::Http::ResponseWriter response)
{
	string apiResponse;
	map<string,string> apiParameters;
	string cmd;

	cmd = this->getCommand(request.resource());
	apiParameters = this->getParameters(request.resource());

	if (apiCommands.find(cmd) != apiCommands.end())
	{
		apiResponse = apiCommands[cmd](apiParameters);
	}
	else
	{
		apiResponse = generateErrorResponse("-1","Invalid command");
	}
	response.send(Pistache::Http::Code::Ok, apiResponse);
}

string RestApiService::getCommand(const string inputQuery)
{
	const string commandPattern = "command=";
	string command;
	int cmdStringBegin = -1;
	int paramSeparatorIdx = -1;

	cmdStringBegin = inputQuery.find(commandPattern);
	cmdStringBegin+=commandPattern.length();
	paramSeparatorIdx = inputQuery.find("&");
	if (-1 == paramSeparatorIdx)
	{
		command = inputQuery.substr(cmdStringBegin);
	}
	else
	{
		command = inputQuery.substr(cmdStringBegin, paramSeparatorIdx - cmdStringBegin);
	}

	return command;
}

map<string,string> RestApiService::getParameters(const string inputQuery)
{
	map<string,string> apiParams;
	const string paramSeparatorPattern = "&";
	const string paramValuePattern     = "=";
	string paramName;
	string paramValue;
	int paramNameBeginIdx = 0;
	int paramValueBeginIdx = -1;
	int paramSeparatorIdx = -1;

	paramNameBeginIdx  = inputQuery.find("/") + 1;

	while ( paramNameBeginIdx != 0 )
	{
		paramValueBeginIdx = inputQuery.find(paramValuePattern,paramNameBeginIdx) + 1;
		paramSeparatorIdx = inputQuery.find(paramSeparatorPattern, paramValueBeginIdx);

		paramName  = inputQuery.substr(paramNameBeginIdx, inputQuery.find(paramValuePattern, paramNameBeginIdx)-paramNameBeginIdx);
		if (-1 == paramSeparatorIdx)
		{
			paramValue = inputQuery.substr(paramValueBeginIdx);
		}
		else
		{
			paramValue = inputQuery.substr(paramValueBeginIdx, inputQuery.find(paramSeparatorPattern, paramNameBeginIdx)-paramValueBeginIdx);
		}
		apiParams[paramName] = paramValue;

		paramNameBeginIdx  = inputQuery.find(paramSeparatorPattern, paramNameBeginIdx) + 1;
	}

	return apiParams;
}

string RestApiService::generateSingleElementResponse(xmlResponse responseElements)
{
	string response;

	response = "<sensor>";
	for (auto &element : responseElements)
	{
		response += "<sensorId>" + to_string(element.first) + "</sensorId>";
		for (auto &params : element.second)
			response += "<" + params.first +">" + params.second + "</" + params.first +">";
	}

	response += "</sensor>";

	return response;
}

string RestApiService::generateResponse(xmlResponse responseElements)
{
	string response;

	response = "<sensors>";

	//prepare response
	for (auto &element : responseElements)
	{
		response += "<sensor>";

		for (auto &xmlElement : element.second)
			response += "<" + xmlElement.first +">" + xmlElement.second + "</" + xmlElement.first +">";

		response += "</sensor>";
	}

	response += "</sensors>";
	return response;
}

string RestApiService::generateErrorResponse(string sensorId, string message)
{
	string response;

	//prepare response
	response = "<sensor>" + sensorId + "</sensor>";
	response += "<error>" +message + "</error>";
        response += "<available_commands>";
	response += "<command>command=GetVersion</command>";
	response += "<command>command=UpdateSensorState&sensorName=BramaGaraz&reading=0</command>";
        response += "</available_commands>";
	return response;
}

string RestApiService::getVersion(map<string,string> parameters)
{
	return "<version>1.0</version>";
}

string RestApiService::getReading(map<string,string> parameters)
{
	int sensorId;
	string response;

	try
	{
		sensorId = stoi(parameters["id"]);
		response = generateSingleElementResponse(deviceRegister.getReadingDeviceInfo(sensorId));
	}
	catch (invalid_argument& e)
	{
		response = generateErrorResponse(parameters["id"], "Invalid parameter");
	}

	return response;
}

string RestApiService::updateSensorState(map<string,string> parameters)
{
	string sensorName;
	string response;
	int reading = 0;

	try
	{
		sensorName = parameters["sensorName"];
		reading = stoi(parameters["reading"]);
		response = generateSingleElementResponse(deviceRegister.changeSensorReadingValue(sensorName, reading));
	}
	catch (invalid_argument& e)
	{
		response = generateErrorResponse(sensorName, "Invalid parameter");
	}

	return response;
}


string RestApiService::getPresence(map<string,string> parameters)
{
	string response;

	response = generateResponse(deviceRegister.getPresenceDeviceInfo());

	return response;
}

string RestApiService::getTemperature(map<string,string> parameters)
{
	string response;

	response = generateResponse(deviceRegister.getTemperatureDeviceInfo());

	return response;
}

string RestApiService::getConfig(map<string,string> parameters)
{
	int sensorId;
	string response;

	try
	{
		sensorId = stoi(parameters["id"]);
		response = generateSingleElementResponse(deviceRegister.getConfigurationDeviceInfo(sensorId));
	}
	catch (invalid_argument& e)
	{
		response = generateErrorResponse(parameters["id"], "Invalid parameter");
	}

	return response;
}

string RestApiService::enableAlarm(map<string,string> parameters)
{
	shared_ptr<DeviceServiceInterface> alarmService = AlarmService::getInstance();

	alarmService->enableService();

	return "<enabled>1</enabled>";
}

string RestApiService::disableAlarm(map<string,string> parameters)
{
	shared_ptr<DeviceServiceInterface> alarmService = AlarmService::getInstance();
	string code = parameters["code"];
	Status status = STATUS_OK;
	string response = "<enabled>0</enabled>";

	status = alarmService->disableService(code);

	if (status != STATUS_OK)
	{
		response = "<enabled>1</enabled>";
	}

	return response;
}

string RestApiService::getAlerts(map<string,string> parameters)
{
	shared_ptr<DeviceInfoInterface> alarmService = AlarmService::getInstance();
	xmlResponse xmlResponse;
	AlarmReading reading;
	int sensorIdx = 0;
	string response;

	do
	{
		reading = any_cast<AlarmReading>(alarmService->getData(sensorIdx));
		if (STATUS_OK == reading.status)
		{
			xmlResponse[sensorIdx].insert(pair<string, string>("sensorName",reading.sensorName));
			xmlResponse[sensorIdx].insert(pair<string, string>("alert",to_string(reading.isActivate)));
			xmlResponse[sensorIdx].insert(pair<string, string>("SensorId", to_string(sensorIdx)));
		}
		sensorIdx++;
	}
	while (reading.status != STATUS_XML_NO_MORE_SENSORS);

	response = generateResponse(xmlResponse);

	return response;
}


string RestApiService::getEnergy(map<string,string> parameters)
{
	string response;

	response = generateResponse(deviceRegister.getEnergyDeviceInfo());

	return response;
}

string RestApiService::getStates(map<string,string> parameters)
{
	string response;

	response = generateResponse(deviceRegister.getStateDeviceInfo());

	return response;
}

map<string,function<string(map<string,string>)>> RestApiService::apiCommands =
{
		{"GetVersion", getVersion},                //command=GetVersion
		{"GetReading", getReading},
		{"GetConfig" , getConfig },
		{"EnableAlarm" , enableAlarm },
		{"DisableAlarm" , disableAlarm },
		{"UpdateSensorState" , updateSensorState }, //command=UpdateSensorState&sensorName=Zraszacze1&reading=0

		//Below methods return data for all available sensors
		{"GetTemperature" , getTemperature },       //command=GetTemperature
		{"GetPresence" , getPresence },
		{"GetEnergy" , getEnergy },                 //command=GetEnergy
		{"GetStates" , getStates },                 //command=GetStates
		{"GetAlerts" , getAlerts }
};

DeviceInfoRegister RestApiService::deviceRegister;

