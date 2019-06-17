#pragma once

namespace license_manager
{
	struct LICENSE_DATA
	{
		unsigned int  modules_count = 0;
		unsigned int* modules_ids = nullptr;
		unsigned int* modules_end_date = nullptr;
	};

	enum ServerCodes
	{
		API_CODE_UNKNOWN_ERROR = 300,

		API_CODE_USER_NOT_FOUND,
		API_CODE_SUBSCRIPTION_DUPLICATE,
		API_CODE_SUBSCRIPTION_EXPIRY,
		API_CODE_USER_BLOCKED,
		API_CODE_HWID_ERROR,

		API_CODE_GAME_NOT_FOUND,
		API_CODE_GAME_DISABLED,
		API_CODE_OK = 100,
	};

	bool getModulesInfo();

	bool checkModuleActive(unsigned int _end_date);
}