/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part or in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 20/06/2015
Purpose: STL Utilities.

*********************************************************
*/
#ifndef __STLHELPERS_H__
#define __STLHELPERS_H__

#include <stdio.h>
#include <string>
#include <vector>
#include <ext/hash_map>
#include <stdarg.h>
#include "pushfyi-types.h"

class StringHash
{
public:
    size_t operator()(const std::string s) const;

private:
};

inline size_t StringHash::operator()(const std::string s) const
{
    return __gnu_cxx::__stl_hash_string(s.c_str());
}

class StringEqual
{
public:
    bool operator()(const std::string& arg1, const std::string& arg2) const;

private:
};

inline bool StringEqual::operator()(const std::string& arg1, const std::string& arg2) const
{
    return arg1 == arg2;
}

typedef __gnu_cxx::hash_map<std::string, bool, StringHash, StringEqual> StringMap;

class StringUtil
{
public:
    static std::string toString(rxInt value);
    static std::string toString(rxUInt value);
    static std::string toString(rxLong value);
    static std::string toString(rxULong value);
    static std::string toString(rxFloat value);
    static std::string toString(rxDouble value);

    static std::string pad(rxUInt length, char padChar, std::string str);
    static std::string rpad(rxUInt length, char padChar, std::string str);

    static std::string decodeXmlChars(const char* str);
    static std::string decodeXmlChars(std::string& str);

    static std::string encodeXmlChars(const char* str);
    static std::string encodeXmlChars(std::string& str);

    /*
     * example of split
     * 	split("a:b",	':', ret); 	ret={"a", "b"}
     * 	split("",	':', ret); 		ret={}
     * 	split("a",	':', ret); 		ret={"a"}
     * 	split("a:",	':', ret); 		ret={"a"}
     * 	split(":a",	':', ret); 		ret={"", "a"}
     * 	split(":",	':', ret); 		ret={""}
     */
    static void split(const std::string&, char, std::vector<std::string>&);

    static void replace(std::string& str, std::string& find, std::string& replace);
    static std::string format(const char* fmt, ...);

    static bool stringLT(const std::string& arg1, const std::string& arg2);
    static bool stringGT(const std::string& arg1, const std::string& arg2);

    static bool contains(std::vector<std::string>& myvector, std::string value);

    static std::string byteToHex(const rxByte* ba, rxInt len, bool upperCase=false, bool lsbFirst=false);
private:

};

inline std::string StringUtil::toString(rxInt value)
{
    char buffer[32] = {0};
    ::sprintf(buffer, "%i", value);

    return buffer;
}

inline std::string StringUtil::toString(rxUInt value)
{
    char buffer[32];
    ::sprintf(buffer, "%u", value);

    return buffer;
}

inline std::string StringUtil::toString(rxLong value)
{
    char buffer[32] = {0};
#if defined(__x86_64__)
    ::sprintf(buffer, "%ld", value);
#elif defined (__i386__)
    ::sprintf(buffer, "%lld", value);
#else
#error "could not determine os architecture"
#endif
    return buffer;
}

inline std::string StringUtil::toString(rxULong value)
{
    char buffer[32] = {0};
#if defined(__x86_64__)
    ::sprintf(buffer, "%lu", value);
#elif defined (__i386__)
    ::sprintf(buffer, "%llu", value);
#else
#error "could not determine os architecture"
#endif

    return buffer;
}

inline std::string StringUtil::toString(rxFloat value)
{
    char buffer[64];
    ::sprintf(buffer, "%f", value);

    return buffer;
}

inline std::string StringUtil::toString(rxDouble value)
{
    char buffer[96];
    ::sprintf(buffer, "%lg", value);

    return buffer;
}

inline std::string StringUtil::pad(rxUInt length, char padChar, std::string str)
{
    rxInt toAdd = length - str.size();

    if (toAdd <= 0) {
        return str;
    }

    return std::string(toAdd, padChar) + str;
}

inline std::string StringUtil::rpad(rxUInt length, char padChar, std::string str)
{
    rxInt toAdd = length - str.size();

    if (toAdd <= 0) {
        return str;
    }

    return str + std::string(toAdd, padChar);
}

inline std::string StringUtil::decodeXmlChars(const char* str)
{
    std::string result(str);

    return decodeXmlChars(result);
}

inline std::string StringUtil::decodeXmlChars(std::string& str)
{
    for (rxInt i = str.size() - 1; i >= 0; i--) {
        if (str[i] == '&') {
            if (str.compare(i, 4, "&lt;") == 0) {
                str[i] = '<';
                str.erase(i + 1, 3);
            }
            else if (str.compare(i, 4, "&gt;") == 0) {
                str[i] = '>';
                str.erase(i + 1, 3);
            }
            else if (str.compare(i, 5, "&amp;") == 0) {
                str.erase(i + 1, 4);
            }
            else if (str.compare(i, 6, "&quot;") == 0) {
                str[i] = '"';
                str.erase(i + 1, 5);
            }
            else if (str.compare(i, 6, "&apos;") == 0) {
                str[i] = '\'';
                str.erase(i + 1, 5);
            }
        }
    }

    return str;
}

inline std::string StringUtil::encodeXmlChars(const char* str)
{
    std::string result(str);

    return encodeXmlChars(result);
}

inline std::string StringUtil::encodeXmlChars(std::string& str)
{
    for (rxInt i = str.size() - 1; i >= 0; i--) {
        switch (str[i]) {
        case  '<': str[i] = '&'; str.insert(i + 1, "lt;");   break;
        case  '>': str[i] = '&'; str.insert(i + 1, "gt;");   break;
        case '\'': str[i] = '&'; str.insert(i + 1, "apos;"); break;
        case  '"': str[i] = '&'; str.insert(i + 1, "quot;"); break;
        case  '&':               str.insert(i + 1, "amp;");  break;
        }
    }

    return str;
}

inline void StringUtil::split(const std::string& s, const char c, std::vector<std::string>& ret)
{
	ret.clear();
    rxUInt found, index = 0;
    while((found=s.find(c, index)) != std::string::npos){
        if(found>index){
            ret.push_back(s.substr(index, found-index));
        }
        else {
            ret.push_back("");
        }
        index = found + 1;
    }
    if(index<s.size()){
        ret.push_back(s.substr(index));
    }
}

inline void StringUtil::replace(std::string& str, std::string& find, std::string& replace)
{
    rxSize index = str.find(find);

    while (index != std::string::npos) {
        str.replace(index, find.length(), replace);
        index += replace.length();
        index = str.find(find, index);
    }
}

inline std::string StringUtil::format(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    char str[2000];
    vsnprintf(str, sizeof(str), fmt, ap);
    str[sizeof(str) - 1] = '\0'; // in case of truncation

    return str;
}

inline bool StringUtil::stringLT(const std::string& arg1, const std::string& arg2)
{
    return arg1 < arg2;
}

inline bool StringUtil::stringGT(const std::string& arg1, const std::string& arg2)
{
    return arg1 > arg2;
}

inline bool contains(std::vector<std::string>& myvector, std::string value)
{
    for (rxUInt i = 0; i < myvector.size(); i++) {
        if (myvector[i] == value) {
            return true; // FOUND
        }
    }

    return false; // NOT FOUND
}

inline std::string StringUtil::byteToHex(const rxByte* ba, rxInt len, bool upperCase, bool lsbFirst)
{
	std::string str(2*len, '\0');

	for(rxInt i = 0, j = 0; i < len; i++, j += 2){
		rxByte b;
		if(lsbFirst){
			b = ba[len-i-1];
		}
		else{
			b = ba[i];
		}

		rxByte low  = b & 0x0F;
		rxByte high = b >> 4;

		if(high<10){
			str[j] = high | 0x30;
		}
		else {
			str[j] = (high - 9) | (upperCase ? 0x40 : 0x60);
		}

		if(low<10){
			str[j+1] = low | 0x30;
		}
		else {
			str[j+1] = (low - 9) | (upperCase ? 0x40 : 0x60);
		}
	}
	return str;
}


#endif //__STLHELPERS_H__
