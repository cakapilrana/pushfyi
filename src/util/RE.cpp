/*
*********************************************************
        This code is a proprietary of iN Technologies

Following may not be used in part on in whole without the
prior permission of iN Technologies

Author: Kapil Rana
Date: 18/05/2015
Purpose: Regular Expression Parser Wrapper.

*********************************************************
*/
#include "RE.h"
#include "REException.h"

using namespace std;

rxUInt RE::MAX_MSGBUFFER = 8192;

RE::RE(const string& pattern, bool isMultiline)
    : mPattern(pattern)
{
    initialize(mPattern.c_str(), isMultiline);
}

RE::RE(const RE& toCopy)
    : mPattern(toCopy.mPattern)
{
    initialize(mPattern.c_str(), false);
}

RE::~RE()
{
    ::regfree(&mRegEx);
}

void RE::setPattern(const string& pattern, bool isMultiline)
{
    regex_t regEx;
    rxInt   flags  = isMultiline ? (REG_EXTENDED | REG_NEWLINE) : REG_EXTENDED;
    rxInt   result = ::regcomp(&regEx, pattern.c_str(), flags);

    if (result != 0) {
        char message[MAX_MSGBUFFER];
        ::regerror(result, &mRegEx, message, MAX_MSGBUFFER);
        throw REException(message);
    }

    ::regfree(&mRegEx);

    mPattern = pattern;
    mRegEx   = regEx;
}

void RE::initialize(const char* pattern, bool isMultiline)
{
    rxInt flags  = isMultiline ? (REG_EXTENDED | REG_NEWLINE) : REG_EXTENDED;
    rxInt result = ::regcomp(&mRegEx, pattern, flags);

    if (result != 0) {
        char message[MAX_MSGBUFFER];
        ::regerror(result, &mRegEx, message, MAX_MSGBUFFER);
        throw REException(message);
    }
}

rxInt RE::matchIt(const char* toMatch, size_t numToMatch, string*& matched) const
{
    regmatch_t matchPtr[numToMatch + 1];
    rxInt      result = ::regexec(&mRegEx, toMatch, numToMatch + 1, matchPtr, 0);

    if (result == REG_NOMATCH) {
        return -1;
    }
    else if (result != 0) {
        char message[2048];
        ::regerror(result, &mRegEx, message, 2048);
        throw REException(message);
    }

    // Only return the resulting matches if 'matched' is non-NULL.
    if (matched != NULL) {
        for (int i = 0; i < (int)numToMatch; i++) {
            matched[i] = string(toMatch + matchPtr[i].rm_so, matchPtr[i].rm_eo - matchPtr[i].rm_so);
        }
    }

    return matchPtr[0].rm_eo;
}

