#include <Windows.h>
#include <WinInet.h>
#include "http_request.h"
#pragma comment (lib, "Wininet.lib")



string sendHttpRequest(
	const string& server,
	const string& route,
	const string& user_agent,
	const string& params,
	const string& method
)
{
	HINTERNET hInternet = InternetOpen(user_agent.c_str(), INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, INTERNET_FLAG_SECURE);
	if(hInternet != nullptr)
	{
		HINTERNET hConnect = InternetConnect(hInternet, server.c_str(), INTERNET_DEFAULT_HTTPS_PORT, nullptr, nullptr, INTERNET_SERVICE_HTTP, INTERNET_FLAG_SECURE, 1);
		if(hConnect != nullptr)
		{
			HINTERNET hRequest = HttpOpenRequest(hConnect, method.c_str(), route.c_str(), nullptr, nullptr, nullptr, INTERNET_FLAG_KEEP_CONNECTION, 1);
			if(hRequest != nullptr)
			{
				if (HttpSendRequest(hRequest, nullptr, 0, LPVOID(params.c_str()), params.length() + 1))
				{
					unsigned char c = 0;
					string response = "";
					for(;;)
					{
						DWORD readed = 0;
						BOOL isRead = InternetReadFile(hRequest, LPVOID(&c), 1, &readed);
						if(readed == 0 || !isRead)
							break;

						response += c;
					}
					return  response;
				}
			}
		}
	}
	return "";
}