/*
*********************************************************
        This code is a proprietary of iN Technologies

Following may not be used in part on in whole without the
prior permission of iN Technologies

Author: Kapil Rana
Date: 20/06/2015
Purpose: PUBNET Log Strategy

*********************************************************
*/
#ifndef __LOG_STRATEGY_H__
#define __LOG_STRATEGY_H__


#include "LogEntry.h"

class LogStrategy
{
public:

    LogStrategy();
    LogStrategy(rxUInt moduleMask, Log::eLogLevel level);

    virtual ~LogStrategy();

    virtual void log(const LogEntry& entry) = 0;
    virtual const char* type() = 0;
    virtual bool isEnabled(const LogEntry& entry);

    void setEnable(bool b) { mEnable = b; }
    bool isEnable() { return mEnable; }

    void   setModuleMask(rxUInt moduleMask) { mModuleMask = moduleMask; }
    rxUInt getModuleMask() { return mModuleMask; }

    void setLevel(Log::eLogLevel level) { mLevel = level; }
    Log::eLogLevel getLevel() { return mLevel; }


protected:
    bool 	mEnable;
    bool 	mVerbose;
    rxUInt  mModuleMask;
    Log::eLogLevel  mLevel;

};

#endif // __LOG_STRATEGY_H__

