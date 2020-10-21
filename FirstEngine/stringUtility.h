#pragma once

#include <regex>
#include <string>
#include <sstream>
#include <vector>
#include <Windows.h>
#include <DirectXMath.h>

#define MAX_DIGITS_IN_INT 12

std::string ExePath();

BOOL WildcardMatch(const char* pat, const char* str);

std::string ws2s(const std::wstring& s);
std::wstring s2ws(const std::string& s);
HRESULT AnsiToWideCch(WCHAR* wstrDestination, const CHAR* strSource, int cchDestChar);
HRESULT WideToAnsiCch(CHAR* strDestination, const WCHAR* wstrSource, int cchDestChar);
HRESULT GenericToAnsiCch(CHAR* strDestination, const TCHAR* tstrSource, int cchDestChar);
HRESULT GenericToWideCch(WCHAR* wstrDestination, const TCHAR* tstrSource, int cchDestChar);
HRESULT AnsiToGenericCch(TCHAR* tstrDestination, const CHAR* strSource, int cchDestChar);
HRESULT WideToGenericCch(TCHAR* tstrDestination, const WCHAR* wstrSource, int cchDestChar);

void TrimLeft(std::wstring& s);

std::string ToStr(int num, int base);
std::string ToStr(unsigned int num, int base);
std::string ToStr(unsigned long num, int base);
std::string ToStr(float num);
std::string ToStr(double num);
std::string ToStr(bool val);
std::string ToStr(const DirectX::XMFLOAT3& vec);

// This is basically like the Perl split() function. It splits str into substrings by cutting it at each delimiter.  
// The result is stored in vec.
void Split(const std::string& str, std::vector<std::string>& vec, char delimiter);
void split(const std::string& in, std::vector<std::string>& out, const std::string& token);
std::vector<std::string> splitR(const std::string& input, const std::string& regex);