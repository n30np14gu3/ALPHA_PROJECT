#include "json_config.h"
#include "../../../SDK/crypto/XorStr.h"
template <class T>
rapidjson::Value const& alpha_config::operator[](T access) const
{
	return m_dJson[access];
}

alpha_config::alpha_config()
{
	m_dJson.SetObject();
	rapidjson::Document::AllocatorType& allocator = m_dJson.GetAllocator();

	rapidjson::Value aim_bot_config;{
		rapidjson::Value aim_bot_globals;{
			rapidjson::GenericValue<rapidjson::UTF8<char>> a(static_cast<const char&>(XorStr("1337")[4]));
			/*aim_bot_globals.AddMember(, false, allocator);
			aim_bot_globals.AddMember("", false, allocator);
			aim_bot_globals.AddMember()*/
		}

		aim_bot_config.AddMember("globals", aim_bot_globals, allocator);
	}
}
