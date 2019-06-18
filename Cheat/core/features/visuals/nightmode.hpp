#pragma once
#include "../../../dependencies/common_includes.hpp"

class c_nightmode {
public:
	void run() noexcept;
	void apply() noexcept;
	void remove() noexcept;

	bool isActive = true;
	const int moduleId = 0;
};

extern c_nightmode night_mode;