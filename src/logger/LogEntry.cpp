/*
*********************************************************
        This code is a proprietary of iN Technologies

Following may not be used in part on in whole without the
prior permission of iN Technologies

Author: Kapil Rana
Date: 20/06/2015
Purpose: EAZE PRO Log Entry

*********************************************************
*/
#include <stdarg.h>
#include <time.h>
#include <stdio.h>
#include "LogEntry.h"
#include "Time.h"

using namespace std;

LogEntryRep::LogEntryRep(rxUInt id, Log::eLogModule module, Log::eLogLevel level, const char* file, rxUInt line,
                          const char* func, const char* description)
{
    mId = id;
    mModule = module;
    mLevel = level;
    mFile = file;
    mLine = line;
    mFunc = func;
    mDescription = description;
    time(&mTime);
}

inline LogEntryRep& LogEntryRep::operator=(const LogEntryRep & from)
{
    this->mId     = from.mId;
    this->mModule = from.mModule;
    this->mLevel  = from.mLevel;
    this->mFile   = from.mFile;
    this->mLine   = from.mLine;
    this->mFunc   = from.mFunc;
    this->mDescription = from.mDescription;

    return *this;
}

bool LogEntryRep::operator==(const LogEntryRep & other)
{
    if( (this->mId == other.mId)
        && (this->mModule == other.mModule)
        && (this->mLevel  == other.mLevel)
        && (this->mFile   == other.mFile)
        && (this->mLine   == other.mLine)
        && (this->mFunc   == other.mFunc)){

        return true;
    }else{
        return false;
    }
}

std::string LogEntryRep::toString()
{
    struct tm bTime;
    const time_t time = getTime();

    localtime_r(&time, &bTime);

    char timeStr[32] = {0};
    strftime(timeStr, sizeof(timeStr), "%F %T:", &bTime);
    const rxInt buf_len = 1024;
    char buf[buf_len];
    rxInt n = snprintf(buf, buf_len, "%s %s:%s:%s:%d:[%s] ",
    		timeStr,
    		Log::getModuleString((Log::eLogModule)getModule()),
    		Log::getLogLevelString((Log::eLogLevel)getLevel()),
    		getFile(),
    		getLine(),
    		getFunc());

    if(n == buf_len){
    	buf[buf_len-1] = '\0';
    }

    string ret = buf;

    ret += getDescription();

    return ret;
}

// helper function to print LogEntries to streams
/*
std::ostream& LogEntryRep::printLong(std::ostream& ostr)
{
    struct tm bTime;
    const time_t time = getTime();

    localtime_r(&time, &bTime);

    char timeStr[32];
    strftime(timeStr, sizeof(timeStr), "%F %T:", &bTime);

    ostr << timeStr
         << ":" << Log::getModuleString((Log::eLogModule)getModule())
         << ":" << Log::getLogLevelString((Log::eLogLevel)getLevel())
         << ":" << getFile() << ":" <<  getLine()
         << ":" << getFunc()
         << ":" << getDescription() << endl;

    return ostr;
}


std::ostream& LogEntryRep::printShort(std::ostream& ostr)
{
    struct tm bTime;
    const time_t time = getTime();

    localtime_r(&time, &bTime);

    char timeStr[32];
    strftime(timeStr, sizeof(timeStr), "%T:", &bTime);

    ostr << timeStr
         << ":" << Log::getModuleString((Log::eLogModule)getModule())
         << ":" << getFunc()
         << ":" << getDescription() << endl;

    return ostr;
}

std::ostream& LogEntryRep::printTimeAndDescription(std::ostream& ostr)
{
    struct tm bTime;
    const time_t time = getTime();

    localtime_r(&time, &bTime);

    char timeStr[32];
    strftime(timeStr, sizeof(timeStr), "%T:", &bTime);

    ostr << timeStr  << ":" << getDescription() << endl;

    return ostr;
}


std::ostream& LogEntryRep::printTrace(std::ostream& ostr, std::string& module, std::string& submodule)
{
    string time = Time::toString("%F %T", Time::timeInSeconds());

    ostr << time  << "|" << module << "|" << submodule << ":" << mId << "|" << getDescription() << "`" << endl;

    return ostr;
}
*/
