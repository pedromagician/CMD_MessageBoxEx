#pragma once

#include "stdafx.h"

class Conversion {
public:
	static int ToInt(const wstring& _buff, int _base = 10);

	static wstring LeftTrimString(const wstring& _str, const wstring& _val);
	static wstring RightTrimString(const wstring& _str, const wstring& _val);
	static wstring TrimString(const wstring& _str, const wstring& _val);
	static wstring TrimWhiteChar(const wstring& _val);

	static wstring ToLower(const wstring& _val);

	static void StringReplaceAll(wstring& _mess, const wstring& _oldStr, const wstring& _newStr);

	static wstring Utf32ToUtf16(char32_t ch);
	static void UnicodeCodeConverter(wstring& _mess);

	static wstring ParseEscapeString(const wstring& _str);
};
