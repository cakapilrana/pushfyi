/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part or in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 20/06/2015
Purpose: PUBNET Log Ids for multiple languages

*********************************************************
*/
#ifndef __LOG_ID_H__
#define __LOG_ID_H__

#include <ostream>
#include <string>
#include "pushfyi-types.h"

    /**
     * @class LogId
     * Used to define the enums for the log ids. It is also used to load up
     * the id - format map according to locale. By default, it will load the
     * English mapping declared in memory. If a locale is given, it will open
     * the file and parse the entries into its internal representation.
     *
     * Steps to adding a new LogId. The steps are marked in the source
     * files LogId.h and LogId.cc with STEP_1 to STEP_3.
     *
     * STEP_1: Add new log id enum
     * STEP_2: Add string description and format string
     * STEP_3: Not implemented yet: Add your format string to the locale files
     *         in the required language.
     */

class LogId
{
public:
    ~LogId(){};

    static rxInt init(const char* locale = NULL);

    // STEP_1 add your log id enum below. Please note your module base.
    enum eLogId {
        eDebug                     =   0,
        eFileOpenErr               =   1,
        eFileOpenOK                =   2,
        eFileRenameErr             =   3,
        eNoMemoryErr               =   4,
        eRegExErr                  =   5,
        eUnknown       		= 2999
    };

    static const char* toString(const eLogId id);

    static const char* getFormat(const eLogId id);

    static void print(std::ostream& ostr);

private:
    LogId(){}

};

#endif //__LOG_ID_H__
