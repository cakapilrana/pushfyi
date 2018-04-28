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
#include <iostream>
#include "StreamLogger.h"

StreamLogger::StreamLogger():
	LogStrategy(), mDelimiter(':'), mEndEntryFlag(0)
{
}

StreamLogger::~StreamLogger()
{
}


void StreamLogger::log(const LogEntry& entry)
{
    struct tm bTime;
    const time_t time = entry->getTime();

    localtime_r(&time, &bTime);
    
    char timeStr[32];
  	strftime(timeStr, sizeof(timeStr), "%F %T", &bTime);
    
    std::cout << timeStr 
    	<< mDelimiter << Log::getLogLevelString(entry->getLevel())
    	<< mDelimiter << Log::getModuleString(entry->getModule())
    	<< mDelimiter << entry->getFunc();
    if (entry->getLevel() > Log::eInfo){ //debug, fine, finest
		std::cout << mDelimiter << entry->getFile() << mDelimiter << entry->getLine();
    } 
    std::cout << mDelimiter << entry->getDescription() << std::endl;

}

