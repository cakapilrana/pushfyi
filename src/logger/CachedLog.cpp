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
#include <stdlib.h>
#include <stdarg.h>
#include <iostream>
#include <errno.h>
#include <string.h>

#include "FileLogger.h"
#include "CachedLog.h"
#include "ProcessInit.h"

using namespace std;

CachedLog::CachedLog()
{
    mFinished = false;
    mThread   = 0;
    mEnableDescription = true;
    mPrintShort = false;
    mQueue = new Queue<LogEntry>("OAM", "log", 1024);
    mLogLevel = Log::eDebug;
    mLogModules = 0xffffffff;

    updateIterators();
}

CachedLog::~CachedLog()
{
    if(mQueue != NULL){
        delete mQueue;
        mQueue = NULL;
    }
    mLogStrats.clear();
}

CachedLog* CachedLog::init()
{
    Log::sLog = new CachedLog();
    if(Log::sLog == NULL){
	cerr << "Could not initialize logger. Exiting.\n";
	exit(1);
    }
    return (CachedLog*)Log::sLog;
}

LogStrategy* CachedLog::getLogStrategy(const string& key)
{
    LogStrategyTable::const_iterator iter = mLogStrats.find(key);
    if (iter != mEnd ) {
        return (*iter).second;
    }
    return 0;
}

void CachedLog::updateIterators()
{
    const_cast<LogStrategyTable::iterator&>(mBegin) = mLogStrats.begin();
    const_cast<LogStrategyTable::iterator&>(mEnd)   = mLogStrats.end();
}

rxInt CachedLog::addLogStrategy(const string& key, LogStrategy* strat)
{
    mLogStrats[key] = strat;
    updateIterators();
    return 0;
}

rxInt CachedLog::removeLogStrategy(const string& key)
{
    mLogStrats.erase(key);
    updateIterators();
    return 0;
}

bool CachedLog::offline()
{
    mFinished = true;
    LogEntry entry(0);
    mQueue->push(entry);
    mThread->join();
    return true;
}

bool CachedLog::online()
{
	Properties* sysProps = ProcessInit::instance()->getSysProps();
    if(sysProps->contains("log.level")){
        string level = sysProps->get("log.level");
        setLogLevel(level);
    } else {
    	//default log level is finest
    	string level = "finest";
    	setLogLevel(level);
    }

    string modules = "all";
    setLogModules(modules);

    if(sysProps->getBool("filelog.enabled") == true) {
		string param;
		if(sysProps->contains("filelog.file")) {
			param = sysProps->get("filelog.file");
		} else if(sysProps->contains("defaultLogPath") && sysProps->contains("defaultLogFileName")) {
			param = sysProps->get("defaultLogPath") + "/";
			param += sysProps->get("defaultLogFileName");
		} else {
			//neither versum.conf nor command line contains log file path and name
			//use default as cmd name and path
			param = sysProps->get("cmd.name") + ".log";
		}
		LogStrategy* ls = getLogStrategy("file");
		if(!ls){
			ls = new FileLogger(param);
			if (((FileLogger*)ls)->open()){
				addLogStrategy("file", ls);
				INFO(Log::eLogger, "added filelog strategy file=%s", param.c_str());
			}
			else {
				INFO(Log::eLogger, "failed to add filelog strategy file=%s", param.c_str());
			}
		}
    }
    if(mThread == 0) {
        mThread = new Thread(this);
        mThread->start();
    }
    return true;
}

void CachedLog::enableLogStrategy(const string& key,  bool flag)
{
    LogStrategy* ls = getLogStrategy(key);
    if(ls){
    	ls->setEnable(flag);
    }
}

bool CachedLog::isLogEnabled(eLogModule module, eLogLevel level)
{
    bool bRet = (level<=mLogLevel) && (mLogModules & (0x00000001 << module));
    return bRet;
    //return true;
}

void CachedLog::log(rxUInt id, eLogModule module, eLogLevel level, const char* file, rxUInt line,
              const char* func, const char* format, ...)
{
    //due to performance reason, following check has been moved into to LOG and LOGFMT macro
    //so we don't need check it here
//    if (mLogEnabled && (module & mLogModules) && (level & mLogLevels)){
        va_list ap;
        va_start(ap, format);

        LogEntry entry;

        if(mEnableDescription){
            char str[4096] = {0};

            if(vsnprintf(str, sizeof(str), format, ap) == sizeof(str)){
                str[sizeof(str) - 1] = '\0';
            }

            entry = new LogEntryRep(id, module, level, file, line, func, str);
        } else {
            entry = new LogEntryRep(id, module, level, file, line, func, "");
        }

        mQueue->push(entry);
        //printf("%s\n", entry->toString().c_str());

        va_end(ap);
//    }
}

void CachedLog::setLogLevel(rxInt level)
{
	if (level<8) {
		setLogLevel((eLogLevel)level);
	}
	else if (level<16) {
		setLogLevel(Log::eWarning);
	}
	else if (level<32) {
		setLogLevel(Log::eInfo);
	}
	else if (level<64) {
		setLogLevel(Log::eDebug);
	}
	else if (level<128) {
		setLogLevel(Log::eFiner);
	}
	else {
		setLogLevel(Log::eFinest);
	}
}

void CachedLog::setLogLevel(eLogLevel level)
{
    mLogLevel = level;
}

void CachedLog::setLogLevel(string& s)
{
    setLogLevel(Log::getLogLevel(s.c_str()));
}


// set log levels from argVal string - used by
// command line option.
void CachedLog::setLogModules(string& argVal)
{
    char arg[256] = {0};
    rxUInt modules = 0;

    if(argVal.empty()){
        ::strcpy(arg, "all");
    }else{
        ::strncpy(arg, argVal.c_str(), sizeof(arg) - 1);
        arg[sizeof(arg) -1] = '\0';

        // make argument lower case
        char* p = arg;
        while(*p){
            *p = ::tolower(*p);
            p++;
        }
    }

    if(::strstr(arg, "all") != NULL){
        modules = -1;
    }else{
    	for (rxInt i = 0; i < Log::sModule_len; i++){
            if(::strstr(arg, Log::sModules[i]) != NULL){
                modules |= (0x0001 << i);
            }
    	}
    }

    setLogModules(modules);
}


void CachedLog::handleLog(LogEntry& entry)
{
    for(mIter = mBegin; mIter != mEnd; ++mIter) {
        LogStrategy* strategy =  (*mIter).second;
        if(strategy && strategy->isEnabled(entry)){
            strategy->log(entry);
        }
    }
}

void CachedLog::run()
{
    LogEntry entry;

    while(!mFinished){ // no way to exit the queue unless we have a custom queue object
        entry = mQueue->pop();
        try
        {
            if(entry.isNull()){
                continue;
            }
            handleLog(entry);
        }
        catch (...)
        {
            ERROR(Log::eLogger, "Unknown exception in logging");
        }
    }
}

void CachedLog::interrupt(bool immediately)
{
    LogEntry entry(0);
    if(immediately){
        mQueue->pushFront(entry);
    }
    else{
        mQueue->push(entry);
    }
}

