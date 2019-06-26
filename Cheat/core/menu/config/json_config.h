#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "../../../SDK/rapidjson/rapidjson.h"
#include "../../../SDK/rapidjson/document.h"
#include "../../../SDK/rapidjson/stringbuffer.h"
#include "../../../SDK/rapidjson/writer.h"

class alpha_config
{
public:
	alpha_config();

	template<class T>
	rapidjson::Value const& operator[](T access) const;


private:
	rapidjson::Document m_dJson;
};