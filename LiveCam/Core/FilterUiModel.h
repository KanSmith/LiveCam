#pragma once

#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

class FilterUiModel {
public:
	void setId(int i);
	int getId(void);
	
	void setTitle(std::string value);
	std::string getTitle(void);

	void setIcon(std::string value);
	std::string getIcon(void);

	void setJSONsource(std::string value);
	std::string getJSONsource(void);

	void setWatermark(std::string value);
	std::string getWatermark(void);
	
	FilterUiModel();
	FilterUiModel(boost::property_tree::ptree &record);

private:
	std::string title;
	int id;
	std::string icon;
	std::string JSONsource;
	std::string watermark;
};
