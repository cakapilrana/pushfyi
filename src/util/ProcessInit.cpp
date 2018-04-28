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
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <getopt.h>
#include <signal.h>
#include <string>
#include <stdlib.h>

#include "ProcessInit.h"
#include "SystemProperties.h"
#include "Time.h"
#include "REException.h"

using namespace std;

ProcessInit* ProcessInit::sInstance = 0;
//Factory ProcessInit::sFactory;

static void out_of_memory()
{
    ::printf("CRITICAL ERROR: Out of memory\n");
    LOG(LogId::eNoMemoryErr, Log::eProcess, Log::eInfo);
    exit(1);
}

ProcessInit::ProcessInit()
{
    mSysProps = 0;

    FILE* fp;
    double uptime, idle_time;

    fp = fopen ("/proc/uptime", "r");
    fscanf (fp, "%lf %lf\n", &uptime, &idle_time);
    fclose (fp);
    time_t current;
    time(&current);
    mUpTime = current - (time_t)uptime;
}

ProcessInit::~ProcessInit()
{
    sInstance = 0;
}

ProcessInit* ProcessInit::instance()
{
    if (sInstance == 0) {
        sInstance = new ProcessInit();
    }

    return sInstance;
}

bool ProcessInit::startup(int argc, char* argv[])
{
    // initialize trace manager and Log
    Time* systemTime = Time::instance();
    systemTime->online();
    ::set_new_handler(out_of_memory);

    try {
    	if ((argc > 1) && (*(argv[argc-1])!='-')) {
    		mSysProps =  SystemProperties::instance(argv[argc-1]);
    	} else{
    		mSysProps = SystemProperties::instance("pubnet.conf");
    	}
    }
    catch (REException e){
        cout << e.getMessage();
        exit(1);
    }
    parseCmdLineParams(argc, argv);

    //bool success = mConfigMgr->online();
    // ignore SIGPIPE which happens when you kill one end of a socket
    // and the other end tries to write to it

    signal(SIGPIPE, SIG_IGN);

    return true;
}

void ProcessInit::shutdown()
{
     // sleep so the log can drain
    sleep(1);

    //Log::deleteAllLogStrategies();
    Time* systemTime = Time::instance();

    systemTime->offline();
    delete systemTime;

    delete mSysProps;
}


// By default, the ouput shows all log messages with the exception of debug from all modules.
// The following command line options are available:
//
//  -c              - enable logging to console - default off
//  -l[level]       - log/debug messages up to and including the name level - default all
//                    Available levels are: all, alert, error, warning, info, debug, debugfiner,debugfinest,trace
//  -f[file][:size] - Log to file - default off. Default file name "file.log".
//                    Optional file argument specifies  log file path to be used for file logging.
//                    Optional file size argument specifies the max. file size. Default 100Kb.

void ProcessInit::parseCmdLineParams(int argc, char* argv[])
{
    string cmd = argv[0];
    rxUInt slash_pos = cmd.find_last_of('/');
    if (slash_pos == string::npos) {
        mSysProps->set("cmd.name", cmd);
    }
    else {
        mSysProps->set("cmd.path", cmd.substr(0, slash_pos));
        mSysProps->set("cmd.name", cmd.substr(slash_pos+1));
    }
    while (1) {
        int c = getopt(argc, argv, "dcf:l:");
        if (c == -1)
            break;

        switch (c) {
        case 'd':
            mSysProps->setBool("pubnet.daemon", true);
            break;
        case 'l':
            mSysProps->set("log.level", optarg);
            break;
        case 'f':
            mSysProps->setBool("filelog.enabled", true);
            if(optarg != NULL) {
                mSysProps->set("filelog.file", optarg);
            }
            break;
        case 'c':
            mSysProps->setBool("consolelog.enabled", true);
            break;
        }
    }
}
