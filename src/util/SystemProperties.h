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
#ifndef __SYSTEM_PROPERTIES_H__
#define __SYSTEM_PROPERTIES_H__

#include <string>
#include "Properties.h"

class SystemProperties
    : public Properties
{
public:
    static SystemProperties* instance(std::string filename = "");
    virtual ~SystemProperties();

    std::string getConfigDir();
    std::string getDataDir();
    std::string getLogDir();

    bool isSingleModule();

private:
    static SystemProperties* mInstance;

    SystemProperties(std::string filename);
};

inline std::string SystemProperties::getConfigDir()
{
    return get("config.dir", ".");
}

inline std::string SystemProperties::getDataDir()
{
    return get("data.dir", ".");
}

inline std::string SystemProperties::getLogDir()
{
    return get("log.dir", ".");
}

inline bool SystemProperties::isSingleModule()
{
    return getBool("SingleModule", false);
}

#endif
