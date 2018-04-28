/*
*********************************************************
        This code is a proprietary of iN Technologies

Following may not be used in part on in whole without the
prior permission of iN Technologies

Author: Kapil Rana
Date: 18/05/2015
Purpose: Regular Expression Exception.

*********************************************************
*/
#ifndef __RE_EXCEPTION_H__
#define __RE_EXCEPTION_H__

#include <string>

class REException
{
public:
    REException(const std::string& message);
    virtual ~REException();

    std::string getMessage();

private:
    std::string mMessage;
};

inline std::string REException::getMessage()
{
    return mMessage;
}

#endif //__RE_EXCEPTION_H__

