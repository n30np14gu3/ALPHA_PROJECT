#include<iostream>
#include<ctime>

#include "../globals/globals.h"
#include "../rapidjson/document.h"
#include "../network/http/http_request.h"
#include "../crypto/XorStr.h"
#include "../crypto/base64/base64.hpp"
#include "../openssl extensions/openssl_extensions.h"

#include "license_manager.h"

using namespace std;
using namespace rapidjson;
namespace license_manager
{
	bool getModulesInfo()
	{
		string requestParams =
			string(XorStr("access_token=")) + globals::access_token + "&" +
			string(XorStr("user_id = ")) + to_string(globals::user_id);

		string postData = http_request::post(globals::server_url, string(XorStr("/api/request_modules")), globals::lib_user_agent, requestParams, false);
		postData = base64::base64_decode(postData);
		postData = openssl_extensions::openssl_decrypt(
			postData,
			XorStr("aes-256-cbc"),
			reinterpret_cast<unsigned char const*>(globals::crypto_key.c_str()),
			reinterpret_cast<unsigned char const*>(globals::crypto_iv.c_str()));

		Document responcse;

		responcse.Parse(postData.c_str());

		ServerCodes code = static_cast<ServerCodes>(responcse[XorStr("code")].GetInt());
		switch(code)
		{
		case API_CODE_OK:
			globals::user_modules.modules_count = responcse[XorStr("data")][XorStr("count")].GetInt();
			globals::user_modules.modules_ids = new unsigned int[globals::user_modules.modules_count];
			globals::user_modules.modules_end_date = new unsigned int[globals::user_modules.modules_count];
			memset(globals::user_modules.modules_ids, 0, sizeof(unsigned int) * globals::user_modules.modules_count);
			memset(globals::user_modules.modules_end_date, 0, sizeof(unsigned int) * globals::user_modules.modules_count);
			responcse.Clear();
			return true;
		default:
			return false;
		}
	}
	bool checkModuleActive(unsigned int _end_date)
	{
		time_t t = time(nullptr);
		return  t < _end_date;
	}
}