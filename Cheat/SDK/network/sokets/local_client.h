#pragma once
#include <Windows.h>
#include <string>
#include <openssl/rsa.h>

class local_client
{
public:
	int NoError = 0;

	local_client(const char* ip, u_short port);
	~local_client();

	bool data_exchange();
	bool generate_key(int bits);
	byte* recivepacket(DWORD len, DWORD* lpRecived);
	bool sendpacket(byte* data, DWORD data_size);
private:

	//bool generate_key(int bits);
	struct PROTO_HEADER
	{
		unsigned char hash[65];
		unsigned int key_length;
	};

	struct SERVER_RESPONSE
	{
		DWORD user_id;
		byte access_token[64];
		byte key[32];
		byte iv[16];
	};

	RSA* m_rsa;

	std::string m_sSessionKey;

	PROTO_HEADER m_pHeader{};

	WSADATA m_wsaData{};
	SOCKET m_sClient;
	sockaddr_in m_sockAddr{};
	int m_iErrorCode;

};