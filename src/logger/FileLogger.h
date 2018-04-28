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
#ifndef __LOG_TO_FILE_H__
#define __LOG_TO_FILE_H__

#include <fstream>
#include <time.h>

#include "StreamLogger.h"

class FileLogger : 
	public StreamLogger
{
public:
    static const rxUInt sMaxFileLogSize;
    static const rxUInt sMinFileLogSize;
    
    //params has such a format file_path:size_limit
    FileLogger(const std::string& params);

    virtual ~FileLogger();

    bool open();

    rxUInt getSizeLimit(){ return mMaxSize; }
    void setSizeLimit(rxUInt maxSize);

    const std::string& getPath() { return mPath; }
    void setPath(const char* path) { mPath = path; }
    void setPath(const std::string& path) { mPath = path; }

    const std::string& getBackupPath() { return mBackupPath; }
    void setBackupPath(const char* path) { mBackupPath = path; }
    void setBackupPath(const std::string& path) { mBackupPath = path; }

    void log(const LogEntry& entry);
    void flush();

    const char* type() { return "FileLog"; }

protected:
    std::ofstream mFStream;
    virtual void print(const LogEntry& entry);

private:
    void trimmFile();

    std::string  mPath;
    std::string  mBackupPath;
    long mMaxSize;    // using long since our rxLong is 64 bit

    rxUInt mBackUpCounter;
};

#endif //__LOG_TO_FILE_H__

