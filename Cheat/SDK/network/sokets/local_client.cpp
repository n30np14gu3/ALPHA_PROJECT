#include <WinSock.h>

#include <openssl/rsa.h>
#include <openssl/engine.h>
#include <openssl/pem.h>

#include "local_client.h"

#include "../../crypto/hash/sha256.hpp"
#include "../../crypto/XorStr.h"
#include "../../globals/globals.h"

#include <fstream>

#pragma comment(lib, "Ws2_32.lib")


std::ifstream::pos_type filesize(const char* filename)
{
	std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
	return in.tellg();
}

local_client::local_client(const char* ip, u_short port)
{
	m_rsa = nullptr;
	m_sClient = 0;
	m_iErrorCode = 0;

	m_sSessionKey = std::string(XorStr("5=N4t5SZH_!2Ugw4tctC*U-sMWw_QgT2^eP?nE9GL7JE3=qp?XRjF4ZS+XW$Bk8^^uPTj%AZv7SWmyDwS?2dC#3xJhb7venM&!N?"));

	ZeroMemory(&m_pHeader, sizeof(PROTO_HEADER));

	NoError = !WSAStartup(MAKEWORD(2, 2), &m_wsaData);
	if(!NoError)
		return;

	m_sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	ZeroMemory(&m_sockAddr, sizeof(sockaddr_in));
	m_sockAddr.sin_family = AF_INET;
	m_sockAddr.sin_addr.s_addr = inet_addr(ip);
	m_sockAddr.sin_port = htons(port);

	m_iErrorCode = connect(m_sClient, reinterpret_cast<sockaddr*>(&m_sockAddr), sizeof(m_sockAddr));

	NoError = m_iErrorCode != SOCKET_ERROR;
	if (!NoError)
	{
		closesocket(m_sClient);
		WSACleanup();
	}
}

local_client::~local_client()
{
	ZeroMemory(&m_pHeader, sizeof(PROTO_HEADER));


	m_sSessionKey.clear();

	NoError = 0;
	m_sClient = 0;
	m_iErrorCode = 0;
	ZeroMemory(&m_sockAddr, sizeof(sockaddr_in));
	closesocket(m_sClient);
	WSACleanup();

	RSA_free(m_rsa);
}



bool local_client::data_exchange()
{
	//IMPORT PUBLIC KEY
	BIO* bp = BIO_new_file(XorStr("public.pem"), XorStr("w+"));
	PEM_write_bio_RSAPublicKey(bp, m_rsa);
	BIO_free_all(bp);

	//READ PUBLIC KEY AND DELETE FILE
	DWORD keySize = static_cast<DWORD>(filesize(XorStr("public.pem")));
	FILE* public_key = nullptr;
	byte* key_data = new byte[keySize];
	memset(key_data, 0, keySize);
	
	byte* rnd_data = new byte[keySize];
	RAND_bytes(rnd_data, keySize);

	fopen_s(&public_key, XorStr("public.pem"), XorStr("rb"));
	fread_s(key_data, keySize, 1, keySize, public_key);
	fclose(public_key);

	fopen_s(&public_key, XorStr("public.pem"), XorStr("wb"));
	fwrite(rnd_data, 1, keySize, public_key);
	fclose(public_key);
	remove(XorStr("public.pem"));

	//SEND PROTOCOL HEADER
	std::string hash = sha256(std::string(reinterpret_cast<char*>(key_data)) + m_sSessionKey);
	
	PROTO_HEADER header = {};
	ZeroMemory(&header, sizeof(PROTO_HEADER));

	header.key_length = keySize;
	memcpy((void*)header.hash, hash.c_str(), hash.length());
	OPENSSL_cleanse(rnd_data, keySize);
	hash.clear();
	delete[] rnd_data;

	if(!sendpacket(reinterpret_cast<byte*>(&header), sizeof(PROTO_HEADER)))
		return false;

	if(!sendpacket(key_data, keySize))
		return false;

	//RECV KEYS & DECRYPT
	DWORD recived = 0;
	DWORD* encryptedSize = reinterpret_cast<DWORD*>(recivepacket(sizeof(DWORD), &recived));
	if(recived != 4)
		return false;

	DWORD encryptedRecv = 0;
	byte* encryptedData = recivepacket(*encryptedSize, &encryptedRecv);
	if(*encryptedSize != encryptedRecv)
		return false;

	SERVER_RESPONSE response = {};
	ZeroMemory(&response, sizeof(SERVER_RESPONSE));

	if(!RSA_private_decrypt(*encryptedSize, encryptedData, reinterpret_cast<byte*>(&response), m_rsa, RSA_PKCS1_PADDING))
		return false;

	//STORE DATA
	globals::access_token = std::string(reinterpret_cast<char*>(response.access_token));
	globals::crypto_key = std::string(reinterpret_cast<char*>(response.key));
	globals::crypto_iv = std::string(reinterpret_cast<char*>(response.iv));
	globals::user_id = response.user_id;

	ZeroMemory(&response, sizeof(SERVER_RESPONSE));

	return true;
}

bool local_client::generate_key(int bits)
{
	int ret;
	BIGNUM* bne;

	unsigned long e = RSA_F4;

	bne = BN_new();
	ret = BN_set_word(bne, e);
	if (ret == 1) 
	{
		m_rsa = RSA_new();
		ret = RSA_generate_key_ex(m_rsa, bits, bne, nullptr);
	}
	BN_free(bne);
	return ret == 1;
}

byte* local_client::recivepacket(DWORD len, DWORD* lpRecived)
{
	byte* buff = new byte[len];
	memset(buff, 0, len);
	*lpRecived = recv(m_sClient, reinterpret_cast<char*>(buff), len, 0);
	return buff;
}

bool local_client::sendpacket(byte* data, DWORD data_size)
{
	return  static_cast<DWORD>(send(m_sClient, reinterpret_cast<const char*>(data), data_size, 0)) == data_size;
}
