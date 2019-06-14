#include <openssl/conf.h>
#include <openssl/evp.h>
#include "openssl_extensions.h"

#pragma comment(lib, "libeay32.lib")
#pragma comment(lib, "ssleay32.lib")

string openssl_encrypt(string data, const char* mode, unsigned const char* key, unsigned const char* iv)
{
	int buf_length, out_length;
	unsigned char *cipher_text = static_cast<unsigned char*>(malloc(data.length()));
	EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
	if (!ctx) { return "ctx is null"; }
	if (!EVP_EncryptInit_ex(ctx, (EVP_CIPHER*)(mode), NULL, key, iv)) { return "EVP_EncryptInit_ex error"; }
	if (!EVP_EncryptUpdate(ctx, cipher_text, &buf_length, (unsigned char*)data.c_str(), data.length())) { return "EVP_EncryptUpdate error"; }
	out_length = buf_length;
	if (!EVP_EncryptFinal_ex(ctx, cipher_text + buf_length, &buf_length)) { return "EVP_EncryptFinal_ex error"; }
	out_length += buf_length;
	EVP_CIPHER_CTX_free(ctx);
	std::string out(reinterpret_cast<char*>(cipher_text), out_length);
	free(cipher_text);
	return out;
}

string openssl_decrypt(string cipher, const char* mode, unsigned const char* key, unsigned const char* iv)
{
	int buf_length, out_length;
	unsigned char *data_buf = (unsigned char*)malloc(cipher.length());
	EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
	if (!ctx) { return "ctx is null"; }
	if (!EVP_DecryptInit_ex(ctx, (EVP_CIPHER*)(mode), NULL, key, iv)) { return "EVP_DecryptInit_ex error"; }
	if (!EVP_DecryptUpdate(ctx, data_buf, &buf_length, (unsigned char*)cipher.c_str(), cipher.length())) { return "EVP_DecryptUpdate error"; }
	out_length = buf_length;
	if (!EVP_DecryptFinal_ex(ctx, data_buf + buf_length, &buf_length)) { return "EVP_DecryptFinal_ex error"; }
	out_length += buf_length;
	EVP_CIPHER_CTX_free(ctx);
	std::string out(reinterpret_cast<char*>(data_buf), out_length);
	free(data_buf);
	return out;
}