/*
*********************************************************
        This code is a proprietary of iN Technologies

Following may not be used in part on in whole without the
prior permission of iN Technologies

Author: Kapil Rana
Date: 18/05/2015
Purpose: EAZE PRO Log Stream for writting to pipes

*********************************************************
*/
#ifndef __STREAMLOGGER_H__
#define __STREAMLOGGER_H__

#include "LogStrategy.h"

class StreamLogger : public LogStrategy
{
public:
    StreamLogger();
    virtual ~StreamLogger();

    void log(const LogEntry& entry);

    const char* type() { return "StreamLog"; }

protected:
    char mDelimiter;
    char mEndEntryFlag;
};

#endif //__STREAMLOGGER_H__
