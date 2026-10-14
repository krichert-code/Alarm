/*
 * XmlParser.cpp
 *
 *  Created on: 28 paź 2019
 *      Author: dom
 */


#include <XmlParser.h>


XmlParser::XmlParser(string fileName)
{
	rapidxml::file<> xmlFile(fileName.c_str());
	this->xmlDocument.parse<rapidxml::parse_full>(xmlFile.data());
	sensorsNode = xmlDocument.first_node("sensors");
	settingsNode = xmlDocument.first_node("settings");
}

SensorParameters XmlParser::readSensorParameters()
{
	SensorParameters sensorData;
	if (sensorsNode != nullptr)
	{
		try
		{
			sensorData.sensorName = sensorsNode->first_attribute("name")->value();
			sensorData.sensorType = static_cast<SensorType>(stoi(sensorsNode->first_attribute("sensorType")->value()));
			sensorData.scanningPeriod = stoi(sensorsNode->first_attribute("scanningPeriod")->value());

			if (sensorData.scanningPeriod != ASYNC_SENSOR)
				sensorData.sensorAddress = sensorsNode->first_attribute("address")->value();
			else sensorData.sensorAddress = "";

			sensorData.enabled = static_cast<bool>(stoi(sensorsNode->first_attribute("enabled")->value()));
			sensorData.enableThresholdValue = stoi(sensorsNode->first_attribute("threshold")->value());
			sensorData.status = STATUS_OK;
		}
		catch (invalid_argument& e)
		{
			//todo: logg it - wrong configuration
			std::cerr << e.what();
			sensorData.status = STATUS_XML_PARSE_ERROR;
		}
	}
	else
	{
		sensorData.status = STATUS_XML_NO_MORE_SENSORS;
	}

	return sensorData;
}

SettingParameters XmlParser::readSettingParameters()
{
	SettingParameters settingData;
	if (settingsNode != nullptr)
	{
		try
		{
			settingData.settingName = settingsNode->first_attribute("name")->value();
			settingData.value = settingsNode->first_attribute("value")->value();
			settingData.status = STATUS_OK;
		}
		catch (invalid_argument& e)
		{
			//todo: logg it - wrong configuration
			settingData.status = STATUS_XML_PARSE_ERROR;
		}
	}
	else
	{
		settingData.status = STATUS_XML_NO_MORE_SETTINGS;
	}

	return settingData;
}

SensorParameters XmlParser::getFirstSensorEntry()
{
	sensorsNode = xmlDocument.first_node("sensors");
	sensorsNode = sensorsNode->first_node("sensor");
	return readSensorParameters();
}

SensorParameters XmlParser::getNextSensorEntry()
{
	sensorsNode = sensorsNode->next_sibling("sensor");
	return readSensorParameters();
}


SettingParameters XmlParser::getFirstSettingEntry()
{
	settingsNode = xmlDocument.first_node("settings");
	settingsNode = settingsNode->first_node("setting");
	return readSettingParameters();
}

SettingParameters XmlParser::getNextSettingEntry()
{
	settingsNode = settingsNode->next_sibling("setting");
	return readSettingParameters();
}
