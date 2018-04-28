/*
*********************************************************
        This code is a proprietary of iN Technologies

Following may not be used in part on in whole without the
prior permission of iN Technologies

Author: Kapil Rana
Date: 18/05/2015
Purpose: EAZE PRO Log Ids for multiple languages

*********************************************************
*/
#include <iostream>
#include <map>
#include "LogId.h"

using namespace std;

struct KF {
    const char* str; 
    const char* fmt;
    
    KF(const char* s, const char* f)
    :str(s), fmt(f){}
    
    KF(const KF & kf){
        str = kf.str;
        fmt = kf.fmt;
    }
}; 

struct FmtEntry {
    LogId::eLogId id;
    const char* str;
    const char* fmt;
};

typedef std::map<rxUInt, KF> FormatMap;
typedef FormatMap::iterator FormatMapIter;

FormatMap mFormatMap;

// STEP_2: Add your string description and format string for the English default here
FmtEntry fmtEN[] = {
    { LogId::eDebug,                "eDebug",                "Debug" },
    { LogId::eFileOpenErr,          "eFileOpenErr",          "Unable to open file: %s" },
    { LogId::eFileOpenOK,           "eFileOpenOK",           "OK: Opened file: %s" },
    { LogId::eFileRenameErr,        "eFileRenameErr",        "Unable to rename file: %s" },
    { LogId::eNoMemoryErr,          "eNoMemoryErr",          "Memory allocation failed" },
    { LogId::eRegExErr,             "eRegExErr",             "Regular expression error: %s" }
};

const char* LogId::toString(eLogId id)
{
    FormatMapIter it;

    it = mFormatMap.find(id);

    if(it != mFormatMap.end()){
        return (it->second).str;
    }else{
        return "";
    }
}

const char* LogId::getFormat(const eLogId id)
{
    FormatMapIter it;

    it = mFormatMap.find(id);

    if(it != mFormatMap.end()){
        return (it->second).fmt;
    }else{
        return "";
    }
}

rxInt LogId::init(const char* locale)
{
    if(locale != NULL){
        // open the file for the local and populate the map
    }

    int n = (sizeof(fmtEN)/sizeof(FmtEntry));

    // cout << "Number of map entries:" << n << endl;

    // if no local set or no error - load the hardcoded map
    for(int i = 0; i < n; i++){
        KF kf(fmtEN[i].str, fmtEN[i].fmt);
        mFormatMap.insert(pair<rxUInt, KF>(fmtEN[i].id, kf));
    };

    return 0;
}


void LogId::print(ostream& ostr)
{
    FormatMapIter it;

    for(it = mFormatMap.begin(); it != mFormatMap.end(); it++){
        ostr << it->first << ", " << toString((eLogId)it->first) << ", " << getFormat((eLogId)it->first) << endl;
    }
}
    


#ifdef LOGID_STANDALONE_TEST
// g++ -g -DLOGID_STANDALONE_TEST LogId.cc -o LogId 

main()
{
    LogId::init();
    
    cout << "Available Map entries: " << endl;

    LogId::print(cout);

    cout << "\nGetting format for id\n" << endl;

    cout << "LogId::eDebug = " << LogId::getFormat(LogId::eDebug) << endl;
    cout << "LogId::eSleeBase = " << LogId::getFormat(LogId::eSleeBase) << endl;
    cout << "LogId::eUnknown  = " << LogId::getFormat(LogId::eUnknown) << endl;

    cout << "\nGetting string for id\n" << endl;

    cout << "LogId::eDebug = " << LogId::toString(LogId::eDebug) << endl;
    cout << "LogId::eSleeBase = " << LogId::toString(LogId::eSleeBase) << endl;
    cout << "LogId::eUnknown  = " << LogId::toString(LogId::eUnknown) << endl;
}

#endif
    

    
    
    
  
        
