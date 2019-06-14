#pragma once
#include <iostream>
#include <string>

using namespace std;

string openssl_encrypt(string data, const char* mode, unsigned const char* key, unsigned const char* iv);
string openssl_decrypt(string cipher, const char* mode, unsigned const char* key, unsigned const char* iv);