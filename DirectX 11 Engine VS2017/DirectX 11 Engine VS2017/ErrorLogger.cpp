#include "ErrorLogger.h"
#include <comdef.h>

void ErrorLogger::Log(std::wstring message)
{
	std::wstring error_message = L"Error: " + message + L"\n";
	MessageBoxW(NULL, error_message.c_str(), L"Error", MB_ICONERROR);
}

void ErrorLogger::Log(HRESULT hr, std::wstring message)
{
	_com_error error(hr);
	std::wstring error_message = L"Error: " + message + L"\n" + error.ErrorMessage();
	MessageBoxW(NULL, error_message.c_str(), L"Error", MB_ICONERROR);
}

void ErrorLogger::Log(COMException & exception)
{
	std::wstring error_message = exception.what();
	MessageBoxW(NULL, error_message.c_str(), L"Error", MB_ICONERROR);
}
