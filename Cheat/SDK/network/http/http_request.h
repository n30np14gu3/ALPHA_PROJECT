#pragma once
#include <string>
#include <iostream>

using namespace std;

string sendHttpRequest(
	const string& server,
	const string& route,
	const string& user_agent, 
	const string& params, 
	const string& method
);