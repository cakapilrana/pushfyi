/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part or in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 20/06/2015
Purpose: PUBNET Log Entry

*********************************************************
*/
#ifndef __LOG_ENTRY_H__
#define __LOG_ENTRY_H__

#include <time.h>

#include "pushfyi-types.h"
#include "TRef.h"
#include "Log.h"

class LogEntryRep
    : public Ref::Rep
{
public:
    LogEntryRep(){}

    LogEntryRep(rxUInt id, Log::eLogModule module, Log::eLogLevel level, const char* file,
             rxUInt line, const char* func, const char* description);

    ~LogEntryRep(){}

    LogEntryRep(const LogEntryRep & toCopy);

    LogEntryRep& operator=(const LogEntryRep & toCopy);

    bool operator==(const LogEntryRep & other);

    rxUInt getId() { return mId; }
    void setId(rxUInt id) { mId = id; }

    Log::eLogModule getModule() { return mModule; }
    void setModule(Log::eLogModule module) { mModule = module; }

    Log::eLogLevel getLevel() { return mLevel; }
    void setLevel(Log::eLogLevel level) { mLevel = level; }

    const char* getFile() { return mFile; }
    /**
     * @parm - file name - usually __FILE__ macro which stays constant
     */
    void setFile(const char* file) { mFile = file; }

    rxUInt getLine() { return mLine; }
    void setLine(rxUInt line) { mLine = line; }

    const char* getFunc() { return mFunc; }
    void setFunc(const char* func) { mFunc = func; }

    std::string getDescription() { return mDescription; }
    void setDescription(const std::string& str) { mDescription = str; }

    std::string getSourceName() { return mSourceName; }
    void setSourceName(const std::string& str) { mSourceName = str; }

    const time_t& getTime() { return mTime; }
    void setTime(const time_t& time) { mTime = time; }

    std::string toString();

    /*
    std::ostream& printTimeAndDescription(std::ostream& ostr); // used by Trace

    std::ostream& printTrace(std::ostream& ostr, std::string& module, std::string& submodule);
*/
private:
    //std::ostream& printShort(std::ostream& ostr);
    //std::ostream& printLong(std::ostream& ostr);

    Log::eLogModule mModule;
    Log::eLogLevel mLevel;
    rxUInt mId;         //descriptor ID, cf. LogId:: eLogId in LogId.h
    const char* mFile;
    rxUInt mLine;
    const char* mFunc;
    time_t mTime;
    std::string mDescription;

    //added for alarm handling
    std:: string mSourceName;   //source name, furnished by the caller
};

typedef TRef<LogEntryRep> LogEntry;

inline LogEntryRep::LogEntryRep(const LogEntryRep & from)
  : Ref::Rep()
{
    *this = from;
}


#endif // __LOG_ENTRY_H__
