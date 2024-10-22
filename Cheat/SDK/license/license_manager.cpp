#include<iostream>
#include<ctime>
#include <rapidjson/document.h>

#include "../globals/globals.h"

#include "../network/http/http_request.h"
#include "../crypto/XorStr.h"
#include "../static/modules_ids.h"
#include "license_manager.h"
#include <Windows.h>

using namespace std;
namespace license_manager
{
	bool getModulesInfo()
	{
		string requestParams =
			string(XorStr("access_token=")) + globals::access_token + "&" +
			string(XorStr("game_id=")) + to_string(GAME_ID);

		string postData = http_request::post(globals::server_url, string(XorStr("/api/request_modules")), globals::lib_user_agent, requestParams, true);
		
		rapidjson::Document responcse;
		responcse.Parse(postData.c_str());
		
		ServerCodes code = static_cast<ServerCodes>(responcse[XorStr("code")].GetInt());

		if (code == API_CODE_OK)
		{
			globals::user_modules.modules_count = responcse[XorStr("data")][XorStr("count")].GetInt();

			globals::user_modules.modules_ids = new unsigned int[globals::user_modules.modules_count];
			globals::user_modules.modules_end_date = new unsigned int[globals::user_modules.modules_count];

			memset(globals::user_modules.modules_ids, 0, sizeof(unsigned int) * globals::user_modules.modules_count);
			memset(globals::user_modules.modules_end_date, 0, sizeof(unsigned int) * globals::user_modules.modules_count);

			rapidjson::Value& modules = responcse[XorStr("data")][XorStr("modules")];
			globals::is_lifetime = responcse[XorStr("data")][XorStr("is_lifetime")].GetBool();

			for (rapidjson::SizeType i = 0; i < modules.Size(); i++)
			{
				globals::user_modules.modules_ids[i] = modules[i][XorStr("id")].GetInt();
				globals::user_modules.modules_end_date[i] = modules[i][XorStr("end_date")].GetInt();
			}


			return true;
		}

		return false;
	}

	bool checkModuleActive(LICENSE_DATA data, int moduleId)
	{
#if !NDEBUG
		return true;
#endif
		if (globals::is_lifetime)
			return true;

		for (unsigned i = 0; i < data.modules_count; i++)
		{
			if (data.modules_ids[i] == moduleId)
			{
				time_t t = time(nullptr);
				return  t < data.modules_end_date[i];
			}
		}

		return false;
	}


	bool allModulesExpired()
	{
		for (unsigned i = 0; i < globals::user_modules.modules_count; i++)
		{
			if (checkModuleActive(globals::user_modules, globals::user_modules.modules_ids[i]))
				return false;
		}

		return true;
	}
}
