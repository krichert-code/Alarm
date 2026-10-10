/*
 * XmlParser.h
 *
 *  Created on: 28 paź 2019
 *      Author: dom
 */

#ifndef DEVICECONFIGURATION_XMLPARSER_H_
#define DEVICECONFIGURATION_XMLPARSER_H_

#include <ParserInterface.h>
#include <iostream>
#include <rapidxml.hpp>
#include "rapidxml_utils.hpp"

class XmlParser: public ParserInterface
{
	private:
		rapidxml::xml_document<> xmlDocument;
		rapidxml::xml_node<> *sensorsNode;
		rapidxml::xml_node<> *settingsNode;
		SensorParameters readSensorParameters();
		SettingParameters readSettingParameters();

	public:
		XmlParser(string fileName);
		SensorParameters getFirstSensorEntry() override;
		SensorParameters getNextSensorEntry() override;
		SettingParameters getFirstSettingEntry() override;
		SettingParameters getNextSettingEntry() override;

		virtual ~XmlParser() {}
};


#endif /* DEVICECONFIGURATION_XMLPARSER_H_ */

/*
 *
     rapidxml::file<> xmlFile("Alarm.xml"); // Default template is char
    rapidxml::xml_document<> doc;
    //cout << xmlFile.data();
    doc.parse<rapidxml::parse_full>(xmlFile.data());

    rapidxml::xml_node<> *root = doc.first_node("sensors",7);
    cout << root->first_node("sensor", 6)->first_attribute("name")->value() << endl;
 *
 */
