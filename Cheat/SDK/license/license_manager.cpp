#include<iostream>
#include<ctime>

#include "../globals/globals.h"
#include "../rapidjson/document.h"
#include "../network/http/http_request.h"
#include "../crypto/XorStr.h"
#include "../crypto/base64/base64.hpp"
#include "../openssl extensions/openssl_extensions.h"
#include "../static/modules_ids.h"

#include "license_manager.h"

#include <Windows.h>
using namespace std;
using namespace rapidjson;
namespace license_manager
{
	bool getModulesInfo()
	{
		string requestParams =
			string(XorStr("access_token=")) + globals::access_token + "&" +
			string(XorStr("game_id=")) + to_string(GAME_ID) + "&" +
			string(XorStr("user_id=")) + to_string(globals::user_id);

		string postData = http_request::post(globals::server_url, string(XorStr("/api/request_modules")), globals::lib_user_agent, requestParams, true);
#if !NDEBUG
		MessageBox(nullptr, "JSON GETTED", "", MB_OK);
#endif
		//postData = base64::base64_decode(postData);
		//postData = openssl_extensions::openssl_decrypt(
		//	postData,
		//	XorStr("aes-256-cbc"),
		//	reinterpret_cast<unsigned char const*>(globals::crypto_key.c_str()),
		//	reinterpret_cast<unsigned char const*>(globals::crypto_iv.c_str()));

		Document responcse;

		responcse.Parse(postData.c_str());

#if !NDEBUG
		MessageBox(nullptr, "parse codes", "", MB_OK);
#endif

		ServerCodes code = static_cast<ServerCodes>(responcse[XorStr("code")].GetInt());




		if(code == API_CODE_OK)
		{
#if !NDEBUG
			MessageBox(nullptr, "parse count", "", MB_OK);
#endif

			globals::user_modules.modules_count = responcse[XorStr("data")][XorStr("count")].GetInt();

			globals::user_modules.modules_ids = new unsigned int[globals::user_modules.modules_count];
			globals::user_modules.modules_end_date = new unsigned int[globals::user_modules.modules_count];

			memset(globals::user_modules.modules_ids, 0, sizeof(unsigned int) * globals::user_modules.modules_count);
			memset(globals::user_modules.modules_end_date, 0, sizeof(unsigned int) * globals::user_modules.modules_count);

#if !NDEBUG
			MessageBox(nullptr, "parse modules", "", MB_OK);
#endif
			Value& modules = responcse[XorStr("data")][XorStr("modules")];

#if !NDEBUG
			MessageBox(nullptr, "parse modules data", "", MB_OK);
#endif
			for (SizeType i = 0; i < modules.Size(); i++)
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
		for(unsigned i = 0; i < data.modules_count; i++)
		{
			if(data.modules_ids[i] == moduleId)
			{
				time_t t = time(nullptr);
				return  t < data.modules_end_date[i];
			}
		}

		return false;
	}


	bool allModulesExpired()
	{
		for(unsigned i = 0; i < globals::user_modules.modules_count; i++)
		{
			if(checkModuleActive(globals::user_modules, globals::user_modules.modules_ids[i]))
				return false;
		}

		return true;
	}
}
