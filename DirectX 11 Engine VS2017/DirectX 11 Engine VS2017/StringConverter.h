#pragma once
#include <string>
#include <vector>
#include <Windows.h>
#include <string>

class StringConverter
{
public:
	static std::wstring StringToWide(std::string str);
};