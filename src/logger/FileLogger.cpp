/*
*********************************************************
        This code is a proprietary of iN Technologies

Following may not be used in part on in whole without the
prior permission of iN Technologies

Author: Kapil Rana
Date: 18/05/2015
Purpose: EAZE PRO FILE Logging Utility

*********************************************************
*/
#include <iostream>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include "LogEntry.h"
#include "FileLogger.h"


using namespace std;

const rxUInt FileLogger::sMaxFileLogSize = 256*1048576;
const rxUInt FileLogger::sMinFileLogSize = 4096;

FileLogger::FileLogger(const string& params)
:StreamLogger(), mMaxSize(64*1024), mBackUpCounter(0)
{
    rxUInt size 	= 0;
    rxUInt offset 	= params.find_first_of(':');

    if(offset != string::npos){
        mPath = params.substr(0, offset);
        const string& s = params.substr(offset + 1);

        offset = s.find_first_of(':');
	    if(offset != string::npos){
	    	if(::isdigit(s[0])){
	        	size = strtol(s.substr(0, offset).c_str(), (char**)NULL, 10);
		    	mLevel = Log::getLogLevel(s.substr(offset+1).c_str());
	    	}
	    	else {
		    	mLevel = Log::getLogLevel(s.substr(0, offset).c_str());

	        	size = strtol(s.substr(offset + 1).c_str(), (char**)NULL, 10);
        	}
	    }
	    else {
	    	if(::isdigit(s[0])){
	        	size = strtol(s.c_str(), (char**)NULL, 10);
	    	}
	    	else {
		    	mLevel = Log::getLogLevel(s.c_str());
        	}
        }

        if(size>0){
        	setSizeLimit(size);
        }
    }
    else {
    	mPath = params;
    }

    mBackupPath = mPath + ".bak";
    mModuleMask = 0xFFFFFFFE; //  all module except Log::eTrace;

}

FileLogger::~FileLogger()
{
  if(!mFStream.is_open())
  {
      mFStream.close();
  }
}

void FileLogger::setSizeLimit(rxUInt size)
{
	if(size>sMaxFileLogSize){
		mMaxSize = sMaxFileLogSize;
	}
	else if(size<sMinFileLogSize){
		mMaxSize = sMinFileLogSize;
	}
	else {
		mMaxSize = size;
	}
}

bool FileLogger::open()
{

    mFStream.open(mPath.c_str(), ios::out|ios::app);

    if(!mFStream.is_open()){
        LOG(LogId::eFileOpenErr, Log::eProcess, Log::eAlert, mPath.c_str());
        return false;
    }
    return true;
}


void FileLogger::log(const LogEntry& entry)
{
    if(mFStream.is_open()){
        // check if we will be over the file size limit
    	if(mMaxSize>0){
    		long pos = mFStream.tellp();
    		int entry_len = 256 + entry->getDescription().size();
    		pos += entry_len;
    		if(pos > mMaxSize){
    			trimmFile();
    		}
    	}
        print(entry);
    }
}

// close and unlink primary file
// rename temp file to primary file
// reopen primary file

void FileLogger::trimmFile()
{
    if(mFStream.is_open()){
        mFStream.close();
        //::unlink(mBackupPath.c_str());

        ++mBackUpCounter;

        char backupPath[2048] = {0};
        sprintf(backupPath, "%s.%d", mBackupPath.c_str(), mBackUpCounter);

        //if(::rename(mPath.c_str(), mBackupPath.c_str()) == -1){
        if(::rename(mPath.c_str(), backupPath) == -1){
            LOG(LogId::eFileRenameErr, Log::eProcess, Log::eAlert, mPath.c_str());
        }else{
            mFStream.open(mPath.c_str(), ios::out|ios::app);

            if(!mFStream.is_open()){
                LOG(LogId::eFileOpenErr, Log::eProcess, Log::eAlert, mPath.c_str());
            }
        }
    }
}

void FileLogger::flush()
{
    //::unlink(mBackupPath.c_str());
    std::ofstream bfile;
    bfile.open(mBackupPath.c_str(), ios::out|ios::trunc);
    if(!bfile.is_open()){
        LOG(LogId::eFileOpenErr, Log::eProcess, Log::eAlert, mPath.c_str());
    }
    else{
        bfile.close();
    }

    if(mFStream.is_open()){
        mFStream.close();
        //::unlink(mPath.c_str());

        mFStream.open(mPath.c_str(), ios::out|ios::trunc);

        if(!mFStream.is_open()){
            LOG(LogId::eFileOpenErr, Log::eProcess, Log::eAlert, mPath.c_str());
        }
        else{
            //LogEntry entry = new LogEntryRep(0, 0, 0, 0, 0, 0, "flushed");
            //log(entry);
        }
    }
}

void FileLogger::print(const LogEntry& entry)
{
    struct tm bTime;
    const time_t time = entry->getTime();

    localtime_r(&time, &bTime);

    char timeStr[32] = {0};;
   	strftime(timeStr, sizeof(timeStr), "%F %T", &bTime);

    mFStream << timeStr << mDelimiter << Log::getLogLevelString(entry->getLevel())
    		<< mDelimiter << Log::getModuleString(entry->getModule());

    if(mVerbose){
         mFStream << mDelimiter << entry->getFile() << mDelimiter <<  entry->getLine();
    }

    mFStream << mDelimiter << entry->getFunc()
         << mDelimiter << entry->getDescription();

    if(mEndEntryFlag){
    	mFStream << mEndEntryFlag;
    }
    mFStream << endl;

}


