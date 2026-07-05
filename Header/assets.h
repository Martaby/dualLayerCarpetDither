#pragma once

#include <vector>
#include <string>

class Digits {
public:
	Digits(std::string digit) {
		representation.push_back(digit);
	}

	static std::vector<std::string> representation;
	static int width;
	static int height;
};