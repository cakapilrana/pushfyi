/*
*********************************************************
        This code is a proprietary of iN Technologies

Following may not be used in part on in whole without the
prior permission of iN Technologies

Author: Kapil Rana
Date: 20/06/2015
Purpose: ProcesInit class used for command line arguments

*********************************************************
*/
#ifndef __PROCESS_INIT_H__
#define __PROCESS_INIT_H__

#include <string>
#include "Log.h"
#include "STLHelpers.h"
#include "Properties.h"

class ProcessInit
{
public:
    virtual ~ProcessInit();

    static ProcessInit* instance();

    virtual bool startup(int argc, char* argv[]);
    virtual void shutdown();

    std::string getConfigDir()
    	{ return mSysProps?mSysProps->get("config.dir", ".") : "."; };

    std::string getDataDir()
    	{ return mSysProps?mSysProps->get("data.dir", ".") : "."; };

    std::string getLogDir()
    	{ return mSysProps?mSysProps->get("log.dir", ".") : "."; };

    time_t getUpTime()
    	{ return mUpTime; };

    Properties* getSysProps()
    	{ return mSysProps; };

protected:
    ProcessInit();

    virtual void parseCmdLineParams(int argc, char* argv[]);

private:
    static ProcessInit* sInstance;

    Properties* mSysProps;
    time_t		mUpTime;
};

#endif //__PROCESS_INIT_H__
