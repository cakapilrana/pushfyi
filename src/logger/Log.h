/*
*********************************************************
        This code is a proprietary of iN Technologies

Following may not be used in part on in whole without the
prior permission of iN Technologies

Author: Kapil Rana
Date: 20/06/2015
Purpose: PUBNET Logging Utility

*********************************************************
*/
#ifndef __LOG_H__
#define __LOG_H__

#include "LogId.h"

/**
 * @class Log
 * @ingroup util
 * The Log is a singleton class used to print log information to the console
 * or other destinations by using log strategies.
 *
 * The log interface is called via macros to pass extra information for FILE, LINE and
 * function.
 * The macros are:
 * LOG  - The base version using a lookup from LogId to get the format string for localization
 * LOGFMT - The api user provides his own format string.
 * DBG, DBGFINER, DBBFINEST are debug macros with predefined levels using LOGFMT.
 * Usage:
 *   Initialization: Log and LogId singleton need to be initialized first.
 *    if(Log::init() == -1){
 *      cerr << "Log Init failed" << endl;
 *    }
 *    if(LogId::init() == -1){
 *       LOGFMT(LogId::eLogIdInitErr, Log::eSlee, Log::eAlert, "Log Id init failed");
 *    }
 * The above code initializes the Log instance and enables console output by default.
 * It also initializes the LogId format data.
 * If additional log strategies are required, they are initialized and registered with
 * Log next.
 * Usage:
 *   SnmpLog* snmpLog = new SnmpLog();
 *   Log::addLogStrategy(snmpLog, Log::eSnmpLevels);
 *
 * or to add a file output with a limit of 100K bytes for each log and backup file
 *
 *   FileLog* fileLog = new FileLog;
 *
 *   if(fileLog->open("file.log", "file-bk.log", 100000) == -1){
 *       LOG(LogId::eFileOpenErr, Log::eProcess, Log::eAlert, "file.log");
 *   }
 *   LOG(LogId::eFileOpenOK, Log::eSlee, Log::eInfo, "file.log");
 *
 *   Log::addLogStrategy(fileLog, Log::eAllLevels);
 *
 * New strategies can be added as needed. For example, we might decide to log to
 * syslog in addition to snmp.
 *
 * Once initialized, the log and debug macros can be used anywhere in the program.
 * Usage:
 *   Normal macro using LogId value to look up format string
 *    LOG(LogId::eFileOpenOK, Log::eSlee, Log::eInfo, "file.log");
 *
 *   If for some reason, the format string needs to be provided (ex. SIBBS)
 *    LOGFMT(LogId::eLogIdInitErr, Log::eSlee, Log::eAlert, "Log Id init failed");
 *
 *   Debug macros:
 *    DBG(Log::eSlee, "Debug log entry");
 *    DBGFINER(Log::eSlee, "Finer Debug log entry");
 *    DBGFINEST(Log::eSlee, "Finest Debug log entry");
 *
 * By default, the Log will print to the console. This output can be changed using:
 *    LOG::enable/disableConsoleLog();
 *
 * Levels and modules for log output can be changed at runtime.
 *    LOG::set/add/removeLogModule(s)
 *    LOG::setLogLevel - enables all log levels up to and including the passed one
 *    LOG::setSpecificLogLevels - enable or'd list of levels
 *    LOG::add/removeLogLevel
 *
 * Logging can be disabled at run time. This will prevent entries from being put
 * onto the log queue.
 *    LOG::enable/disable
 *
 * In some cases, the processing of the description using the lookup of the format
 * and substituting the arguments might affect performance or the detail might not
 * be desired. The description processing can be turned off using:
 *    LOG::enable/disableDescription
 *
 * If desired, the macros can be compiled out or changed to call a different interface
 * without having to change all source code entries.
 *
 *
 */


class Log
{
public:
    static Log* sLog;
    static Log* instance();
    static const int sModule_len;
    static const int sLevel_len;

    static const char* sModules[];
    static const char* sLevels[];

    /**
     * Module enums,
     * NOTE: limit to 30 values
     */
    enum eLogModule {
        eLogger = 0,
        eProcess,
        ePubSub,
        eNetwork,
        eDB,
        eTCPServer,
        eSecureTCPServer,
        eTCPReader,
        eProtocol,
        ePushFYIManager,
        eCluster,
        eClientFactory,
        eSoap,
        eAuth,
    };

    /**
     * Return string representation of Module enum
     */

    /**
     * Log level enums
     */
    enum eLogLevel {    // enum for log levels. Mapped from syslog call
        eAlert = 0,     	// LOG_ALERT   action must be taken immediately - always logged
        eCritical,     	// LOG_CRIT    critical conditions
        eError,     	// LOG_ERR     error conditions
        eWarning,     	// LOG_WARNING warning conditions
        eInfo,     		// LOG_INFO    informational message
        eDebug,     	// LOG_DEBUG   debug-level message - level default
        eFiner,    		// LOG_DEBUG   debug-level message - level finer
        eFinest,   		// LOG_DEBUG   debug-level message - level finest

    };

    /**
     * Return string representation of Log level enum
     */
    static const char* getLogLevelString(eLogLevel level);
    static const char* getModuleString(eLogModule module);
    static eLogLevel   getLogLevel(const char* sLevel);

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
                    rxUInt line, const char* func, const char* format, ...) = 0;
    virtual bool isLogEnabled(eLogModule module, eLogLevel level) = 0;

    virtual ~Log();


protected:
    Log();

};

inline Log* Log::instance()
{
    return sLog;
}

#define LOGFMT(id, module, level, format, arg...)  \
do {    \
    if(Log::sLog && Log::sLog->isLogEnabled(module, level)) { \
        Log::sLog->log(id, module, level, __FILE__ , __LINE__,  __func__, format, ##arg); \
    }   \
} while (0)

// macro to add entries to the Log - by default logging is enabled
#ifndef RX_LOG_DISABLED

#define LOG(id, module, level, arg...)  \
do {    \
    if(Log::sLog && Log::sLog->isLogEnabled(module, level)) { \
        Log::sLog->log(id, module, level, __FILE__ , __LINE__,  __func__, LogId::getFormat(id), ##arg); \
    }   \
} while (0)

#define INFO(module, fmt, arg...)  LOGFMT(0, module, Log::eInfo,  fmt, ##arg)
#define WARN(module, fmt, arg...)  LOGFMT(0, module, Log::eWarning,  fmt, ##arg)
#define ERROR(module, fmt, arg...) LOGFMT(0, module, Log::eError,  fmt, ##arg)

#else   // ! _LOG_ENABLED
#define LOG(mod, level, arg...)
#define INFO(module, fmt, arg...)
#define WARN(module, fmt, arg...)
#define ERROR(module, fmt, arg...)
#endif

// debug macros
#ifdef DEBUG
#define DBG(module, fmt, arg...)       LOGFMT(LogId::eDebug, module, Log::eDebug,  fmt, ##arg)

#define DBGFINER(module, fmt, arg...)  LOGFMT(LogId::eDebug, module, Log::eFiner,  fmt, ##arg)

#define DBGFINEST(module, fmt, arg...) LOGFMT(LogId::eDebug, module, Log::eFinest, fmt, ##arg)

#else

#define DBG(module, fmt, arg...)

#define DBGFINER(module, fmt, arg...)

#define DBGFINEST(module, fmt, arg...)

#endif

          // debug macros
#endif // __LOG_H__
