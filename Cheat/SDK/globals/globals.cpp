#include "globals.h"
#include "../crypto/XorStr.h"
namespace globals
{
	string server_url;
	string lib_user_agent;
	string crypto_key;
	string crypto_iv;
	string access_token;
	uint32_t user_id;
	license_manager::LICENSE_DATA user_modules = {0, nullptr, nullptr};

	void initGlobals()
	{
		server_url = XorStr("alphacheat.com");
		lib_user_agent = XorStr("alphacheat dll");
	}
}