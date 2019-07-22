#pragma once
#include <string>

namespace json_ex
{
	struct local_proto_request
	{
		std::string access_token;
	};

	struct end_response
	{
		int completed;
	};

	local_proto_request* parse_from_bytes(unsigned const char* json);
	std::string to_json(const end_response& obj);
}