#pragma once
#include <cstdint>

#include "../../sdk/datatypes/color.h"
#include "../../sdk/datatypes/vector.h"



union ConfigItem
{
	ConfigItem() {};
	ConfigItem(bool b) { this->bl = b; };
	ConfigItem(int i) { this->it = i; };
	ConfigItem(float f) { this->fl = f; };
	ConfigItem(Color_t c) { this->clr = c; };
	ConfigItem(Vector v) { this->vec3 = v; };

	constexpr ConfigItem& operator=(const ConfigItem& n)
	{
		this->bl = n.bl;
		this->it = n.it;
		this->fl = n.fl;
		this->clr = n.clr;
		this->vec3 = n.vec3;

		return *this;
	}

	bool bl;
	int it;
	float fl;
	Color_t clr;
	Vector vec3;

};


class ConfigManager {
public:
	ConfigManager() {

	}

	void init() {
		configs["esp_box"] = ConfigItem(false);
	}

	std::unordered_map<std::string,ConfigItem> configs;

};


inline ConfigManager* g_ConfigManager = new ConfigManager;