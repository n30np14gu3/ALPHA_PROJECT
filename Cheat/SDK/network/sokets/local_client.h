#pragma once
#include <Windows.h>

class local_client
{
public:
	local_client(const char* ip, DWORD port);
private:
	struct PROTO_HEADER
	{
		unsigned char hash[64];
		unsigned int key_length;
		char* pub_key;
	};
};