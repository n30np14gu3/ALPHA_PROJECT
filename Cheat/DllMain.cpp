#include <Windows.h>

#include "SDK/network/http/http_request.h"
#include "SDK/network/sokets/local_client.h"

void test_thread()
{
	//AllocConsole();
	//FILE* file = nullptr;
	//freopen_s(&file,"CONOUT$", "w", stdout);
	//freopen_s(&file, "CONOUT$", "w", stderr);

	system("pause");
	local_client client("127.0.0.1", 1337);
	client.generate_key(1024);
	client.data_exchange();
	system("pause");
}


int main()
{
	test_thread();
}