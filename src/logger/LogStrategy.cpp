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
#include "LogStrategy.h"

LogStrategy::LogStrategy():
	mEnable(true),
	mVerbose(false),
	mModuleMask(-1),
	mLevel(Log::eInfo)
{
}

LogStrategy::LogStrategy(rxUInt moduleMask, Log::eLogLevel level):
	mEnable(true),
	mVerbose(false),
	mModuleMask(moduleMask),
	mLevel(level)
{
}

LogStrategy::~LogStrategy()
{
}

bool LogStrategy::isEnabled(const LogEntry& entry)
{
    //return mEnable && (entry->getLevel()<=mLevel) && (mModuleMask & (0x00000001 << entry->getModule()));
    return mEnable;
}


