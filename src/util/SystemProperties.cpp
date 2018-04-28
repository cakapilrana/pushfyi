/*
*********************************************************
        This code is a proprietary of iN Technologies

Following may not be used in part on in whole without the
prior permission of iN Technologies

Author: Kapil Rana
Date: 18/05/2015
Purpose: Wrapper for System Configuration

*********************************************************
*/
#include <iostream>
#include "SystemProperties.h"
#include "Log.h"

using namespace std;

SystemProperties* SystemProperties::mInstance = 0;

SystemProperties* SystemProperties::instance(std::string filename)
{
    if (mInstance == 0) {
        if (filename.size() == 0) {
            filename = "pubnet.conf";
        }

        mInstance = new SystemProperties(filename);
    }

    return mInstance;
}

SystemProperties::SystemProperties(string filename) : Properties(filename)
{
	/*
    ifstream file(filename.c_str());
    std::cerr << "Loading file = " << filename.c_str();

    if (filename.length()) {
        load(file);
    }
    else {
        DBG(Log::eProcess, "Unable to open system properties: %s", filename.c_str());
    }
    */
}

SystemProperties::~SystemProperties()
{
    // nop
}
