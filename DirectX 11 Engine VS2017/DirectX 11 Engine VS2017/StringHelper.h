#pragma once
#include <string>

class StringHelper
{
public:
	static std::wstring StringToWide(std::string str);
	static std::wstring GetDirectoryFromPath(const std::wstring & filepath);
	static std::string GetDirectoryFromPath(const std::string & filepath);
	static std::wstring GetFileNameOnlyFromPath(const std::wstring & filepath);
	static std::wstring GetFileExtension(const std::wstring & filename);
};