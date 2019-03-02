#pragma once
#include <comdef.h>
#include "StringHelper.h"

#define WIDE2(x) L##x
#define DEFTOWIDE(x) WIDE2(x)
#define COM_ERROR_IF_FAILED( hr, msg ) if( FAILED( hr ) ) throw COMException( hr, msg, DEFTOWIDE(__FILE__), DEFTOWIDE(__FUNCTION__), __LINE__ )

class COMException
{
public:
	COMException(HRESULT hr, const wchar_t * msg, const std::wstring& file, const std::wstring& function, int line)
	{
		_com_error error(hr);
		whatmsg = L"Msg: ";
		whatmsg += msg;
		whatmsg += L"\n";
		whatmsg += error.ErrorMessage();
		whatmsg += L"\nFile: " + file;
		whatmsg += L"\nFunction: " + function;
		whatmsg += L"\nLine: " + std::to_wstring(line);
	}

	const wchar_t * what() const
	{
		return whatmsg.c_str();
	}
private:
	std::wstring whatmsg;
};