/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part or in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 18/05/2015
Purpose: Regular Expression Parser Wrapper.

*********************************************************
*/
#ifndef __RE_H__
#define __RE_H__

#ifdef PPC
#include <stddef.h>
#endif

#include <regex.h>
#include <string>
#include "pushfyi-types.h"

class RE
{
public:
    RE(const std::string& pattern, bool isMultiline = false);
    RE(const RE& toCopy);
    virtual ~RE();

    void setPattern(const std::string& pattern, bool isMultiline = false);
    std::string getPattern() const;

    rxInt match(const std::string& toMatch) const;
    rxInt match(const char* toMatch) const;
    rxInt match(const std::string& toMatch, size_t numToMatch, std::string matched[]) const;
    rxInt match(const char* toMatch, size_t numToMatch, std::string matched[]) const;

private:
    void initialize(const char* pattern, bool isMultiline = false);
    rxInt matchIt(const char* toMatch, size_t numToMatch, std::string*& matched) const;

    static rxUInt MAX_MSGBUFFER;

    regex_t     mRegEx;
    std::string mPattern;
};

inline std::string RE::getPattern() const
{
    return mPattern;
}

inline rxInt RE::match(const std::string& toMatch) const
{
    return match(toMatch, 0, 0);
}

inline rxInt RE::match(const char* toMatch) const
{
    return match(toMatch, 0, 0);
}

inline rxInt RE::match(const std::string& toMatch, size_t numToMatch, std::string matched[]) const
{
    return matchIt(toMatch.c_str(), numToMatch, matched);
}

inline rxInt RE::match(const char* toMatch, size_t numToMatch, std::string matched[]) const
{
    return matchIt(toMatch, numToMatch, matched);
}


#endif //__RE_H__
