#include "stringUtility.h"

std::string ExePath() {
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");
	return std::string(buffer).substr(0, pos);
}

// The following function was found on http ://xoomer.virgilio.it/acantato/dev/wildcard/wildmatch.html, where it was attributed to 
// the C/C++ Users Journal, written by Mike Cornelison. It is a little ugly, but it is FAST. Use this as an excercise in not reinventing the
// wheel, even if you see gotos. 
BOOL WildcardMatch(const char* pat, const char* str) {
	int i;
	int star;

new_segment:

	star = 0;
	if (*pat == '*') {
		star = 1;
		do { pat++; } while (*pat == '*'); /* enddo */
	} /* endif */

test_match:

	for (i = 0; pat[i] && (pat[i] != '*'); i++) {
		//if (mapCaseTable[str[i]] != mapCaseTable[pat[i]]) {
		if (str[i] != pat[i]) {
			if (!str[i]) return 0;
			if ((pat[i] == '?') && (str[i] != '.')) continue;
			if (!star) return 0;
			str++;
			goto test_match;
		}
	}
	if (pat[i] == '*') {
		str += i;
		pat += i;
		goto new_segment;
	}
	if (!str[i]) return 1;
	if (i && pat[i - 1] == '*') return 1;
	if (!star) return 0;
	str++;
	goto test_match;
}

std::string ws2s(const std::wstring& s) {
	int slength = (int)s.length() + 1;
	int len = WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, 0, 0, 0, 0) - 1;
	std::string r(len, '\0');
	WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, &r[0], len, 0, 0);
	return r;
}

std::wstring s2ws(const std::string& s) {
	int slength = (int)s.length() + 1;
	int len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0) - 1;
	std::wstring r(len, '\0');
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, &r[0], len);
	return r;
}

//-----------------------------------------------------------------------------
// Name: AnsiToWideCch()
// Desc: This is a UNICODE conversion utility to convert a CHAR string into a
//       WCHAR string. 
//       cchDestChar is the size in TCHARs of wstrDestination.  Be careful not to 
//       pass in sizeof(strDest) 
//-----------------------------------------------------------------------------
HRESULT AnsiToWideCch(WCHAR* wstrDestination, const CHAR* strSource, int cchDestChar) {
	if (wstrDestination == NULL || strSource == NULL || cchDestChar < 1)
		return E_INVALIDARG;

	int nResult = MultiByteToWideChar(CP_ACP, 0, strSource, -1, wstrDestination, cchDestChar);
	wstrDestination[cchDestChar - 1] = 0;

	if (nResult == 0) {
		return E_FAIL;
	}
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: WideToAnsi()
// Desc: This is a UNICODE conversion utility to convert a WCHAR string into a
//       CHAR string. 
//       cchDestChar is the size in TCHARs of strDestination
//-----------------------------------------------------------------------------
HRESULT WideToAnsiCch(CHAR* strDestination, const WCHAR* wstrSource, int cchDestChar) {
	if (strDestination == NULL || wstrSource == NULL || cchDestChar < 1) {
		return E_INVALIDARG;
	}

	int nResult = WideCharToMultiByte(CP_ACP, 0, wstrSource, -1, strDestination, cchDestChar * sizeof(CHAR), NULL, NULL);
	strDestination[cchDestChar - 1] = 0;

	if (nResult == 0) {
		return E_FAIL;
	}
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: GenericToAnsi()
// Desc: This is a UNICODE conversion utility to convert a TCHAR string into a
//       CHAR string. 
//       cchDestChar is the size in TCHARs of strDestination
//-----------------------------------------------------------------------------
HRESULT GenericToAnsiCch(CHAR* strDestination, const TCHAR* tstrSource, int cchDestChar) {
	if (strDestination == NULL || tstrSource == NULL || cchDestChar < 1) {
		return E_INVALIDARG;
	}

#ifdef _UNICODE
	return WideToAnsiCch(strDestination, tstrSource, cchDestChar);
#else
	strncpy(strDestination, tstrSource, cchDestChar);
	strDestination[cchDestChar - 1] = '\0';
	return S_OK;
#endif   
}

//-----------------------------------------------------------------------------
// Name: GenericToWide()
// Desc: This is a UNICODE conversion utility to convert a TCHAR string into a
//       WCHAR string. 
//       cchDestChar is the size in TCHARs of wstrDestination.  Be careful not to 
//       pass in sizeof(strDest) 
//-----------------------------------------------------------------------------
HRESULT GenericToWideCch(WCHAR* wstrDestination, const TCHAR* tstrSource, int cchDestChar) {
	if (wstrDestination == NULL || tstrSource == NULL || cchDestChar < 1) {
		return E_INVALIDARG;
	}

#ifdef _UNICODE
	wcsncpy(wstrDestination, tstrSource, cchDestChar);
	wstrDestination[cchDestChar - 1] = L'\0';
	return S_OK;
#else
	return AnsiToWideCch(wstrDestination, tstrSource, cchDestChar);
#endif    
}

//-----------------------------------------------------------------------------
// Name: AnsiToGeneric()
// Desc: This is a UNICODE conversion utility to convert a CHAR string into a
//       TCHAR string. 
//       cchDestChar is the size in TCHARs of tstrDestination.  Be careful not to 
//       pass in sizeof(strDest) on UNICODE builds
//-----------------------------------------------------------------------------
HRESULT AnsiToGenericCch(TCHAR* tstrDestination, const CHAR* strSource, int cchDestChar) {
	if (tstrDestination == NULL || strSource == NULL || cchDestChar < 1) {
		return E_INVALIDARG;
	}

#ifdef _UNICODE
	return AnsiToWideCch(tstrDestination, strSource, cchDestChar);
#else
	strncpy(tstrDestination, strSource, cchDestChar);
	tstrDestination[cchDestChar - 1] = '\0';
	return S_OK;
#endif    
}

//-----------------------------------------------------------------------------
// Name: AnsiToGeneric()
// Desc: This is a UNICODE conversion utility to convert a WCHAR string into a
//       TCHAR string. 
//       cchDestChar is the size in TCHARs of tstrDestination.  Be careful not to 
//       pass in sizeof(strDest) on UNICODE builds
//-----------------------------------------------------------------------------
HRESULT WideToGenericCch(TCHAR* tstrDestination, const WCHAR* wstrSource, int cchDestChar) {
	if (tstrDestination == NULL || wstrSource == NULL || cchDestChar < 1) {
		return E_INVALIDARG;
	}

#ifdef _UNICODE
	wcsncpy(tstrDestination, wstrSource, cchDestChar);
	tstrDestination[cchDestChar - 1] = L'\0';
	return S_OK;
#else
	return WideToAnsiCch(tstrDestination, wstrSource, cchDestChar);
#endif
}

// Remove all leading whitespace
void TrimLeft(std::wstring& s) {
	// find first non-space character
	int i = 0;
	int len = (int)s.length();
	while (i < len) {
		TCHAR ch = s[i];
		int white =
#ifdef UNICODE
			iswspace(ch);
#else
			isspace(ch);
#endif
		if (!white)
			break;
		++i;
	}

	if (i < len) {
		s = s.substr(i);
	}
}

std::string ToStr(int num, int base) {
	char str[MAX_DIGITS_IN_INT];
	memset(str, 0, MAX_DIGITS_IN_INT);
	_itoa_s(num, str, MAX_DIGITS_IN_INT, base);
	return (std::string(str));
}  // end ToStr()

std::string ToStr(unsigned int num, int base) {
	char str[MAX_DIGITS_IN_INT];
	memset(str, 0, MAX_DIGITS_IN_INT);
	_ultoa_s((unsigned long)num, str, MAX_DIGITS_IN_INT, base);
	return (std::string(str));
}

std::string ToStr(unsigned long num, int base) {
	char str[MAX_DIGITS_IN_INT];
	memset(str, 0, MAX_DIGITS_IN_INT);
	_ultoa_s(num, str, MAX_DIGITS_IN_INT, base);
	return (std::string(str));
}

std::string ToStr(float num) {
	char str[64];  // I'm sure this is overkill
	memset(str, 0, 64);
	_sprintf_p(str, 64, "%f", num);
	return (std::string(str));
}

std::string ToStr(double num) {
	char str[64];  // I'm sure this is overkill
	memset(str, 0, 64);
	_sprintf_p(str, 64, "%fL", num);
	return (std::string(str));
}

std::string ToStr(bool val) {
	return (std::string((val == true ? "true" : "false")));
}

std::string ToStr(const DirectX::XMFLOAT3& vec) {
	return std::string("(" + ToStr(vec.x) + "," + ToStr(vec.y) + "," + ToStr(vec.z) + ")");
}

//---------------------------------------------------------------------------------------------------------------------
// This is basically like the Perl split() function.  It splits str into substrings by cutting it at each delimiter.  
// The result is stored in vec.
//---------------------------------------------------------------------------------------------------------------------
void Split(const std::string& str, std::vector<std::string>& vec, char delimiter) {
	vec.clear();
	size_t strLen = str.size();
	if (strLen == 0) {
		return;
	}

	size_t startIndex = 0;
	size_t indexOfDel = str.find_first_of(delimiter, startIndex);
	while (indexOfDel != std::string::npos) {
		vec.push_back(str.substr(startIndex, indexOfDel - startIndex));
		startIndex = indexOfDel + 1;
		if (startIndex >= strLen) {
			break;
		}
		indexOfDel = str.find_first_of(delimiter, startIndex);
	}
	if (startIndex < strLen) {
		vec.push_back(str.substr(startIndex));
	}
}

std::vector<std::string> splitR(const std::string& input, const std::string& regex) {
	std::regex re(regex);
	std::sregex_token_iterator first(input.begin(), input.end(), re, -1);
	std::sregex_token_iterator last;
	return std::vector<std::string>(first, last);
}

void split(const std::string& in, std::vector<std::string>& out, const std::string& token) {
	out.clear();
	std::string temp;
	int sz = int(in.size());
	for (int i = 0; i < sz; ++i) {
		std::string test = in.substr(i, token.size());
		if (test == token) {
			if (!temp.empty()) {
				out.push_back(temp);
				temp.clear();
				i += (int)token.size() - 1;
			}
			else {
				out.push_back("");
			}
		}
		else if (i + token.size() >= in.size()) {
			temp += in.substr(i, token.size());
			out.push_back(temp);
			break;
		}
		else {
			temp += in[i];
		}
	}
}

void split2(const std::string& in, std::vector<std::string>& out, char delim) {
	std::istringstream iss(in);
	std::vector<std::string> vs;
	std::string s;
	while (std::getline(iss, s, delim)) {
		out.push_back(s);
	}
}