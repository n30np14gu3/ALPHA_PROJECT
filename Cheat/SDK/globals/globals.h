#pragma once
#include <string>
#include "../license/license_manager.h"

namespace globals
{
	using namespace  std;

	extern string server_url;
	extern string lib_user_agent;

	extern string crypto_key;
	extern string crypto_iv;
	extern string access_token;
	extern uint32_t user_id;
	extern license_manager::LICENSE_DATA user_modules;

	void initGlobals();
}