/*
*********************************************************
        This code is a proprietary of iN Technologies

Following may not be used in part on in whole without the
prior permission of iN Technologies

Author: Kapil Rana
Date: 18/05/2015
Purpose: EAZE PRO CACHED Logging Utility

*********************************************************
*/
#ifndef __CACHED_LOG_H__
#define __CACHED_LOG_H__

#include <map>
#include "Queue.h"
#include "Runnable.h"
#include "Thread.h"
#include "LogEntry.h"
//#include "util/LogId.h"
#include "LogStrategy.h"

typedef std::map<std::string, LogStrategy*> LogStrategyTable;
typedef std::map<std::string, unsigned> LogModuleTable;
typedef std::map<std::string, unsigned> LogLevelTable;

class CachedLog :
	public Runnable,
	public Log
{
public:
    static CachedLog* init();

    bool online();
    bool offline();

    LogStrategy* getLogStrategy(const std::string& key);

    rxInt addLogStrategy(const std::string& key, LogStrategy* strat);
    rxInt removeLogStrategy(const std::string& key);

    void enableLogStrategy(const std::string&, bool=true);

    virtual ~CachedLog();
    virtual void run();

    /**
     * Logs entry by creating a LogEntry object, populates it with the passed
     * parameters and puts it on the stack depending on the enabled flag.
     *
     * @param id     - log id from LogId::eLogId id or traceId of module, please use LogId for Log entries
     *                 to allow for internationalization
     * @param module - source module
     * @param level  - log level
     * @param file   - source file
     * @param line   - source line
     * @param func   - source function
     * @param format - format for output - printf style
     * @param args   - variable argument list following format
     */
    virtual void log(rxUInt traceId, eLogModule module, eLogLevel level, const char* file,
                    rxUInt line, const char* func, const char* format, ...);

    virtual bool isLogEnabled(eLogModule module, eLogLevel level);

    void setLogModules(rxUInt modules)
    {
        mLogModules = modules;
    }

    /**
     * set the log modules that are reporting. By default all modules are reporting logs.
     * Useful to restrict output for debugging. This version takes a list of modules in string format.
     * For readability, a separator (,) should be use but is not necessary.
     * Upper and lower characters are allowed.
     *
     * Example: setLogModules("slee,device,pubsub,process,sibb,imdb,epcis");
     *
     * @param module  - enable only modules in argument, possible values are: all,slee,device,pubsub,process,sibb,imdb,epcis
     *                  or any combination thereof. 'All' overrides all other options.
     */
    void setLogModules(std::string& modules); // log only messages at the passed module - except alert


    /**
     * Add the log module to the current log modules
     *
     * @param module  - add module to current set
     */
    void addLogModule(eLogModule module) // add a module to the current setting
    {
        mLogModules |= (0x0001 << module);
    }

    /**
     * Remove the log module from the current log modules
     *
     * @param module  - remove the log module
     */
    void removeLogModule(eLogModule module) // add a module to the current setting
    {
        mLogModules &= ~(0x0001 << module);
    }

    /**
     * set the log level to enable all log levels up to and including the
     * passed in log level
     *
     * @param level  - enable all levels up to and including the arg
     */
    void setLogLevel(eLogLevel level); // default - log all messages up to the level

    void setLogLevel(rxInt level);

    /**
     * set the log level to enable all log levels up to and including the
     * passed in log level
     *
     * @param level  - enable all levels up to and including the arg,
     *                 possible values for arg: alarm, critical, error, warning, info, debug, finer or finest
     */
    void setLogLevel(std::string& argVal);

    /**
     * set the log level to enable only the passed log levels plus alert.
     * Alert will always be logged. Example: setLogLevels(Log::eAlert | Log::eCritical | Log::eDebug);
     *
     * @param level  - enable only levels in argument for log reporting
     */


    /**
     * Get the current log level mask
     */
    inline rxUInt getLogLevel(){
        return mLogLevel;
    }

    /**
     * Get the current module mask
     */
    inline rxUInt getLogModules(){
        return mLogModules;
    }

    /**
     * Enable description processing. This will allow for looking up the
     * format string and substituting the arguments if necessary
     */
    void setEnableDescription(bool flag)
    {
        mEnableDescription = flag;
    }


    /**
     * Set console output print format(true=short, false=long)
     */
    void setPrintShort(bool flag)
    {
        mPrintShort = flag;
    }

    bool isPrintShort()
    {
        return mPrintShort;
    }

    /*
     * interrupt log thread, do some management task
     */
    void interrupt(bool immediately=true);

    /**
     * Convert module string to enum
     */

    static unsigned getModuleEnum (const std::string& moduleName);
    /**
     * Convert level string to enum
     */
    static unsigned getLevelEnum (const std::string& levelName);


private:
    CachedLog();
    void handleLog(LogEntry& entry);
    void updateIterators();

    /**
     * Code for thread taking entries off the queue and processing them
     */
    bool mFinished;
    Thread* mThread;

    rxUInt  mLogModules;
    rxInt   mLogLevel;

    Queue<LogEntry>* mQueue;
    bool   mEnableDescription;
    bool   mPrintShort;

    // performance optimizations
    const LogStrategyTable::iterator mBegin;
    const LogStrategyTable::iterator mEnd;
    LogStrategyTable::iterator mIter;

    LogStrategyTable mLogStrats;

    static LogModuleTable mLogModuleTable;
    static LogLevelTable mLogLevelTable;
};

#endif // __CACHED_LOG_H__
