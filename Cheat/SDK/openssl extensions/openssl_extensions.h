#pragma once
#include <string>

namespace openssl_extensions 
{
	using namespace std;

	string openssl_encrypt(string data, const char* mode, unsigned const char* key, unsigned const char* iv);
	string openssl_decrypt(string cipher, const char* mode, unsigned const char* key, unsigned const char* iv);
}