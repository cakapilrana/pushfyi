/*
*********************************************************
        This code is a proprietary of iN Technologies

Following may not be used in part on in whole without the
prior permission of iN Technologies

Author: Kapil Rana
Date: 18/05/2015
Purpose: System configuration wrapper class

*********************************************************
*/
#include <iostream>
#include <fstream>
#include "Log.h"
#include "LogId.h"
#include "Properties.h"
#include "Log.h"
#include "RE.h"
#include "REException.h"

using namespace std;

static char* findIndex(char* array, char ch)
{
	char* ptr = 0;
	for(ptr=&array[0]; *ptr != '\0'; ptr++) {
		if(*ptr == ch) return ptr;
	}
	return 0;
}

Properties::Properties()
{
    // nop
}

Properties::Properties(const Params& params)
    : mParams(params)
{
    // nop
}

Properties::Properties(const string& filename)
    : mFilename(filename)
{
    // need to open the file and read Java-style properties
    INFO(Log::eProcess, "Loading server config file %s", filename.c_str());

    ifstream propstream(filename.c_str());
    
    if (!propstream) {
        ERROR(Log::eProcess, "Failed to open serverproperties file %s", filename.c_str());
        string err("Unable to open properties file: ");
        err += filename;
        throw REException(err);
    }

    INFO(Log::eProcess, "Opened properties file: %s", filename.c_str());

    load(propstream);

    INFO(Log::eProcess, "Done properties file: %s", filename.c_str());
}

Properties::~Properties()
{
}

static void stripWSRight(string& str)
{
    for (int i = str.length() - 1; i > 0; i--) {
	if (str[i] == ' ') {
	    str.erase(i, 1);
	}
	else {
	    break;
	}
    }
}

rxInt Properties::load(std::ifstream& propstream)
{
    string ws       = "[ \t]*";
    string attrStr  = "([^ \t]*)";
    string valStr   = ws + "(.*)$";

    RE attrRE(attrStr);
    RE valRE(valStr);
    RE blankRE("^" + ws + "$");
    string matches[3];

    int maxLineLength = 1000;
    char line[maxLineLength];
    
    while (propstream.getline(line, maxLineLength, '\n')) {
	if (line[0] == '#' || line[0] == '\0') {
	    // treat lines starting with '#' as comments
	    // also skip empty lines

 	    DBGFINEST(Log::eProcess, "Skipping properties comment: %s", line);
	    continue;
	}

        DBGFINEST(Log::eProcess, "Parse properties non-comment line: %s", line);

	rxInt bytes = blankRE.match(line, 3, matches);

	if (bytes > 0) {
            DBGFINEST(Log::eProcess, "Skipping properties blank line");
	    continue;
	}

        //char* ptr = ::index(line, '=');
		char* ptr = findIndex(line, '=');
        if(ptr == NULL){
            ERROR(Log::eProcess, "Properties file parsing error on line = %s", line);
        } else {
            string key;
            string value;

            *ptr++ = '\0';

            bytes = attrRE.match(line, 3, matches);
            if (bytes <= 0) {
                DBGFINEST(Log::eProcess, "Attribute does not match: %s", line);
                continue;
            }else{
               key = matches[1];
            }

            bytes = valRE.match(ptr, 3, matches);
            if (bytes <= 0) {
                DBGFINEST(Log::eProcess, "Value does not match: %s", line);
                continue;
            }else{
                value = matches[1];
                stripWSRight(value);
            }

            INFO(Log::eProcess, "Properties file: key=%s, value=%s",
                key.c_str(), value.c_str());

            if (!mParams.contains(key)) {
                mOrder.push_back(key);
            }
            
            mParams.set(key, value);
	}
    }

    return 0;
}

bool Properties::save()
{
    INFO(Log::eProcess, "Saving properties file %s", mFilename.c_str());

    FILE* file = ::fopen(mFilename.c_str(), "w+");

    if (file == 0) {
        ERROR(Log::eProcess, "Unable to save properties file %s", mFilename.c_str());
        std::cerr << "failed to write system properties =" << mFilename.c_str();
        return false;
    }

    ::fprintf(file, "#-----------------------------------------------------------------------\n");
    ::fprintf(file, "# PushFYI WebSocket Gateway Generated\n");
    ::fprintf(file, "#\n");
    ::fprintf(file, "# Copyright 2014 - pushfyi.com \n");
    ::fprintf(file, "# All Rights Reserved.\n");
    ::fprintf(file, "#\n");
    ::fprintf(file, "# CONFIDENTIALITY AND LIMITED USE\n");
    ::fprintf(file, "#\n");
    ::fprintf(file, "# The following may not be used in part or in whole without previous\n");
    ::fprintf(file, "# written consent from Pushfyi Inc. unless otherwise\n");
    ::fprintf(file, "# stipulated in the agreement/contract under which it has been supplied.\n");
    ::fprintf(file, "#-----------------------------------------------------------------------\n\n");

    ListIter iter = mOrder.begin();

    for ( ; iter != mOrder.end(); iter++) {
        string key = *iter;
        string val = get(key);

        ::fprintf(file, "%s = %s\n", key.c_str(), val.c_str());
    }

    ::fclose(file);
    ERROR(Log::eProcess, "Saved properties file = %s", mFilename.c_str());
    return true;
}
